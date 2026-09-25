#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Builds AstroFind-<version>-x86_64.AppImage inside a debian:12 container (run
# as root). Debian 12 is the oldest base the code builds on (Qt 6.4, GCC 12),
# so the AppImage needs glibc >= 2.36 and runs on that and anything newer.
# linuxdeploy and its Qt plugin are pinned to a release and verified by
# SHA-256 before they are executed.
#
#   packaging/ci/build-appimage.sh <astrofind-X.Y.Z-source.tar.gz> <outdir>
set -euo pipefail

tarball=$(realpath "${1:?usage: build-appimage.sh <source tarball> <outdir>}")
outdir=${2:?usage: build-appimage.sh <source tarball> <outdir>}
mkdir -p "$outdir"; outdir=$(realpath "$outdir")

LINUXDEPLOY_TAG="1-alpha-20251107-1"
LINUXDEPLOY_SHA256="c20cd71e3a4e3b80c3483cef793cda3f4e990aca14014d23c544ca3ce1270b4d"
PLUGIN_QT_TAG="1-alpha-20250213-1"
PLUGIN_QT_SHA256="15106be885c1c48a021198e7e1e9a48ce9d02a86dd0a1848f00bdbf3c1c92724"

export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq --no-install-recommends \
    ca-certificates curl file git build-essential cmake ninja-build pkg-config \
    desktop-file-utils \
    qt6-base-dev qt6-base-dev-tools qt6-charts-dev libqt6opengl6-dev \
    qt6-5compat-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools \
    qtkeychain-qt6-dev libcfitsio-dev libfftw3-dev libarchive-dev libraw-dev \
    libgl-dev libxkbcommon-dev \
    libqt6sql6-sqlite qt6-qpa-plugins qt6-image-formats-plugins qt6-wayland \
    >/dev/null

tools=$(mktemp -d)
fetch() {  # fetch <url> <sha256> <dest>
  curl -fsSL -o "$3" "$1"
  echo "$2  $3" | sha256sum -c -
  chmod +x "$3"
}
fetch "https://github.com/linuxdeploy/linuxdeploy/releases/download/${LINUXDEPLOY_TAG}/linuxdeploy-x86_64.AppImage" \
      "$LINUXDEPLOY_SHA256" "$tools/linuxdeploy-x86_64.AppImage"
fetch "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/${PLUGIN_QT_TAG}/linuxdeploy-plugin-qt-x86_64.AppImage" \
      "$PLUGIN_QT_SHA256" "$tools/linuxdeploy-plugin-qt-x86_64.AppImage"

work=$(mktemp -d)
tar -xzf "$tarball" -C "$work"
src=$(find "$work" -mindepth 1 -maxdepth 1 -type d -name 'astrofind-*' | head -1)
version=${src##*/astrofind-}

cmake -S "$src" -B "$work/build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$work/build" --parallel
"$work/build/bin/astrofind_tests" --reporter compact
desktop-file-validate "$src/astrofind.desktop"

appdir="$work/AppDir"
DESTDIR="$appdir" cmake --install "$work/build"
install -Dm644 -t "$appdir/usr/share/doc/astrofind" "$src/LICENSE" "$src/NOTICE"
install -Dm644 -t "$appdir/usr/share/doc/astrofind/LICENSES" "$src"/LICENSES/*.txt

# No FUSE inside containers: let the linuxdeploy AppImages extract themselves.
export APPIMAGE_EXTRACT_AND_RUN=1
export QMAKE=/usr/lib/qt6/bin/qmake
export LINUXDEPLOY_OUTPUT_VERSION="$version"
# Besides X11 (xcb, always deployed): native Wayland, and "offscreen" so
# `AstroFind --version` also works without a display.
export EXTRA_PLATFORM_PLUGINS="libqwayland-generic.so;libqoffscreen.so"
cd "$work"
"$tools/linuxdeploy-x86_64.AppImage" --appdir "$appdir" \
    --executable "$appdir/usr/bin/AstroFind" \
    --desktop-file "$appdir/usr/share/applications/astrofind.desktop" \
    --icon-file "$appdir/usr/share/icons/hicolor/256x256/apps/astrofind.png" \
    --plugin qt --output appimage

out="$work/AstroFind-${version}-x86_64.AppImage"
[ -f "$out" ] || { ls -la "$work"; echo "AppImage not produced" >&2; exit 1; }
cp "$out" "$outdir/"
echo "$outdir/$(basename "$out")"
