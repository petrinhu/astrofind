#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Builds the Arch package inside an archlinux container (run as root) with
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
work=$(mktemp -d)
cp "$here/../arch/PKGBUILD" "$work/"
cp "$tarball" "$work/${pkgname}-${pkgver}.tar.gz"
chown -R builder: "$work"
su builder -c "cd '$work' && PKGDEST='$work' makepkg --noconfirm"

for pkg in "$work"/astrofind-[0-9]*-x86_64.pkg.tar.zst; do
  cp "$pkg" "$outdir/"
  echo "$outdir/$(basename "$pkg")"
done
