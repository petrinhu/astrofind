#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Installs one release file into a FRESH container of its target distribution
# (run as root) with the native package manager, which must resolve every
# declared dependency from the distribution's own repositories, then runs the
# installed program headless. A missing library, Qt plugin or wrong package
# name fails here, not on a user's machine.
#
#   packaging/ci/smoke-test.sh <package file>
#   (.rpm / .deb / .pkg.tar.zst / .AppImage)
set -euo pipefail

pkg=$(realpath "${1:?usage: smoke-test.sh <package file>}")
expected=$(basename "$pkg" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
. /etc/os-release

case "$pkg" in
  *.rpm)
    case " ${ID} ${ID_LIKE:-} " in
      *" opensuse"*|*" suse "*)
        zypper --non-interactive --no-gpg-checks install "$pkg" ;;
      *" rhel "*|*" rocky "*|*" almalinux "*|*" centos "*)
        dnf -y install epel-release dnf-plugins-core
        dnf config-manager --set-enabled crb
        dnf -y install "$pkg" ;;
      *) dnf -y install "$pkg" ;;
    esac
    bin=/usr/bin/AstroFind ;;
  *.deb)
    export DEBIAN_FRONTEND=noninteractive
    apt-get update -qq
    apt-get install -y "$pkg"
    bin=/usr/bin/AstroFind ;;
  *.pkg.tar.zst)
    pacman -Syu --noconfirm
    pacman -U --noconfirm "$pkg"
    bin=/usr/bin/AstroFind ;;
  *.AppImage)
    # A desktop system has these; minimal container images do not. They are
    # the libraries the AppImage excludelist deliberately leaves to the host
    # (OpenGL/EGL via glvnd, fontconfig, freetype, harfbuzz, xkbcommon, D-Bus,
    # GLib), because they must match the host's drivers and configuration.
    case " ${ID} ${ID_LIKE:-} " in
      *" debian "*|*" ubuntu "*)
        export DEBIAN_FRONTEND=noninteractive; apt-get update -qq
        apt-get install -y -qq libgl1 libegl1 libopengl0 libfontconfig1 libfreetype6 \
            libharfbuzz0b libxkbcommon0 libdbus-1-3 libglib2.0-0 >/dev/null ;;
      *" fedora "*) dnf -y -q install mesa-libGL mesa-libEGL libglvnd-opengl fontconfig \
            freetype harfbuzz libxkbcommon dbus-libs glib2 ;;
      *" arch "*) pacman -Syu --noconfirm --needed mesa libglvnd fontconfig freetype2 \
            harfbuzz libxkbcommon dbus glib2 ;;
      *" opensuse"*|*" suse "*) zypper --non-interactive install Mesa-libGL1 Mesa-libEGL1 \
            libglvnd fontconfig libfreetype6 libharfbuzz0 libxkbcommon0 libdbus-1-3 \
            libglib-2_0-0 ;;
    esac
    chmod +x "$pkg"
    export APPIMAGE_EXTRACT_AND_RUN=1
    bin=$pkg ;;
  *) echo "smoke-test.sh: unknown package type: $pkg" >&2; exit 1 ;;
esac

export QT_QPA_PLATFORM=offscreen
out=$("$bin" --version 2>&1) || { echo "$out"; echo "FAIL: AstroFind --version exited non-zero" >&2; exit 1; }
echo "$out"
echo "$out" | grep -q "AstroFind ${expected}" \
  || { echo "FAIL: expected 'AstroFind ${expected}'" >&2; exit 1; }

# Start the real main window for a few seconds: it must still be running when
# timeout stops it (exit 124), i.e. it did not crash on start-up.
set +e
timeout 8 "$bin" > /tmp/astrofind-smoke.log 2>&1
rc=$?
set -e
cat /tmp/astrofind-smoke.log
[ "$rc" -eq 124 ] || { echo "FAIL: AstroFind exited with $rc during start-up" >&2; exit 1; }
grep -q "MainWindow created" /tmp/astrofind-smoke.log \
  || { echo "FAIL: main window was not created" >&2; exit 1; }
echo "PASS: $(basename "$pkg") on ${PRETTY_NAME}"
