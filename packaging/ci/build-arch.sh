#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Builds the Arch package inside an archlinux or cachyos container (run as root) with
# packaging/arch/PKGBUILD. The release source tarball is placed next to the
# PKGBUILD under the name its source=() entry expects, so makepkg uses it
# instead of downloading. makepkg refuses to run as root: it runs as an
# unprivileged "builder" user after root installs the dependencies.
#
#   packaging/ci/build-arch.sh <astrofind-X.Y.Z-source.tar.gz> <outdir>
set -euo pipefail

tarball=$(realpath "${1:?usage: build-arch.sh <source tarball> <outdir>}")
outdir=${2:?usage: build-arch.sh <source tarball> <outdir>}
mkdir -p "$outdir"; outdir=$(realpath "$outdir")
here=$(cd "$(dirname "$0")" && pwd)

pacman -Syu --noconfirm --needed base-devel
# shellcheck disable=SC1091
source "$here/../arch/PKGBUILD"
pacman -S --noconfirm --needed "${depends[@]}" "${makedepends[@]}"

id builder >/dev/null 2>&1 || useradd -m builder
# CachyOS's makepkg.conf compiles with -march=native, i.e. for the CPU of the
# CI machine that happens to build the package: on a CPU without the same
# instruction sets (AVX-512, for example) AstroFind dies with "Illegal
# instruction". Build for the x86-64 baseline, like Arch, so the package runs
# on every CPU CachyOS supports. makepkg reads ~/.makepkg.conf after
# /etc/makepkg.conf.
cat > ~builder/.makepkg.conf <<'CONF'
CFLAGS="${CFLAGS//-march=native/-march=x86-64}"; CFLAGS="${CFLAGS//-mtune=native/-mtune=generic}"
CXXFLAGS="${CXXFLAGS//-march=native/-march=x86-64}"; CXXFLAGS="${CXXFLAGS//-mtune=native/-mtune=generic}"
CONF
chown builder: ~builder/.makepkg.conf
# shellcheck disable=SC2016
flags=$(su builder -s /bin/bash -c 'source /etc/makepkg.conf; for f in /etc/makepkg.conf.d/*.conf; do [ -f "$f" ] && source "$f"; done; source ~/.makepkg.conf; echo "$CFLAGS $CXXFLAGS"')
echo "makepkg flags: $flags"
case "$flags" in *native*) echo "build-arch.sh: -march/-mtune=native still set" >&2; exit 1 ;; esac
work=$(mktemp -d)
cp "$here/../arch/PKGBUILD" "$work/"
cp "$tarball" "$work/${pkgname}-${pkgver}.tar.gz"
chown -R builder: "$work"
su builder -c "cd '$work' && PKGDEST='$work' makepkg --noconfirm"

# CachyOS is its own distribution (own repositories and rebuilds): its package
# is built there and named apart from the Arch one. pacman reads the package
# metadata from inside the file, so the name does not affect installation.
. /etc/os-release
for pkg in "$work"/astrofind-[0-9]*-x86_64.pkg.tar.zst; do
  name=$(basename "$pkg")
  [ "${ID}" = "cachyos" ] && name="${name%-x86_64.pkg.tar.zst}-cachyos-x86_64.pkg.tar.zst"
  cp "$pkg" "$outdir/$name"
  echo "$outdir/$name"
done
