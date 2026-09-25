#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Builds the RPM inside a Fedora, RHEL-9-family or openSUSE container (run as
# root): installs the spec's BuildRequires, runs rpmbuild -ba (which also runs
# the core unit tests in %check) and copies the binary RPM to <outdir> under
# a name that says which distribution it is for.
#
#   packaging/ci/build-rpm.sh <astrofind-X.Y.Z-source.tar.gz> <outdir>
set -euo pipefail

tarball=$(realpath "${1:?usage: build-rpm.sh <source tarball> <outdir>}")
outdir=${2:?usage: build-rpm.sh <source tarball> <outdir>}
mkdir -p "$outdir"; outdir=$(realpath "$outdir")
here=$(cd "$(dirname "$0")" && pwd)
spec="$here/../rpm/astrofind.spec"

. /etc/os-release
case " ${ID} ${ID_LIKE:-} " in
  *" opensuse"*|*" suse "*)
    zypper --non-interactive refresh
    zypper --non-interactive install --no-recommends rpm-build rpmdevtools
    mapfile -t deps < <(rpmspec -q --buildrequires "$spec" | sed 's/ .*//')
    zypper --non-interactive install --no-recommends "${deps[@]}"
    label="opensuse-tumbleweed"
    ;;
  *" rhel "*|*" centos "*|*" rocky "*|*" almalinux "*)
    dnf -y install epel-release dnf-plugins-core
    dnf config-manager --set-enabled crb
    dnf -y install rpm-build rpmdevtools
    dnf -y builddep "$spec"
    label=""
    ;;
  *" fedora "*)
    dnf -y install rpm-build rpmdevtools dnf-plugins-core
    dnf -y builddep "$spec"
    label=""
    ;;
  *) echo "build-rpm.sh: unsupported distribution '${ID}'" >&2; exit 1 ;;
esac

top=$(mktemp -d)
mkdir -p "$top"/{SOURCES,SPECS,BUILD,RPMS,SRPMS}
cp "$tarball" "$top/SOURCES/"
cp "$spec" "$top/SPECS/"
rpmbuild -ba --define "_topdir $top" "$top/SPECS/astrofind.spec"

for rpm in "$top"/RPMS/x86_64/astrofind-[0-9]*.x86_64.rpm; do
  name=$(basename "$rpm")
  # Fedora/EL already carry .fcNN/.elN; openSUSE has no dist tag, so add one.
  [ -n "$label" ] && name="${name%.x86_64.rpm}.${label}.x86_64.rpm"
  cp "$rpm" "$outdir/$name"
  echo "$outdir/$name"
done
