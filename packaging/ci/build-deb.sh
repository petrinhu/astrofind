#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Builds the .deb inside a Debian or Ubuntu container (run as root) from the
# release source tarball, with packaging/debian as debian/. The Debian version
# gets a "~<distro><release>" suffix (e.g. 1.2.0-1~ubuntu24.04, 1.2.0-1~debian13)
# so each file says which distribution it was built for; dh_auto_test runs the
# core unit tests.
#
#   packaging/ci/build-deb.sh <astrofind-X.Y.Z-source.tar.gz> <outdir>
set -euo pipefail

tarball=$(realpath "${1:?usage: build-deb.sh <source tarball> <outdir>}")
outdir=${2:?usage: build-deb.sh <source tarball> <outdir>}
mkdir -p "$outdir"; outdir=$(realpath "$outdir")
here=$(cd "$(dirname "$0")" && pwd)

. /etc/os-release
case "$ID" in
  ubuntu) suffix="ubuntu${VERSION_ID}" ;;
  debian) suffix="debian${VERSION_ID}" ;;
  *) echo "build-deb.sh: unsupported distribution '${ID}'" >&2; exit 1 ;;
esac

export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq --no-install-recommends build-essential devscripts ca-certificates >/dev/null

work=$(mktemp -d)
tar -xzf "$tarball" -C "$work"
src=$(find "$work" -mindepth 1 -maxdepth 1 -type d -name 'astrofind-*' | head -1)
cp -r "$here/../debian" "$src/debian"
chmod +x "$src/debian/rules"
# First changelog line: "astrofind (1.2.0-1) unstable; ..." -> "(1.2.0-1~debian13)".
sed -i "1s/^astrofind (\([^)]*\))/astrofind (\1~${suffix})/" "$src/debian/changelog"
head -1 "$src/debian/changelog"

apt-get build-dep -y -qq "$src" >/dev/null
(cd "$src" && dpkg-buildpackage -b -us -uc)

for deb in "$work"/astrofind_*_amd64.deb; do
  cp "$deb" "$outdir/"
  echo "$outdir/$(basename "$deb")"
done
