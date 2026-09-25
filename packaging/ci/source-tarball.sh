#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Writes <outdir>/astrofind-<version>-source.tar.gz from HEAD (top directory
# astrofind-<version>/). Every package in release.yml is built from this file,
# so a missing file in the tarball fails the build instead of shipping.
#
#   packaging/ci/source-tarball.sh <outdir>
set -euo pipefail

outdir=${1:?usage: source-tarball.sh <outdir>}
root=$(git rev-parse --show-toplevel)
version=$("$root/packaging/ci/version.sh")
mkdir -p "$outdir"
out="$(cd "$outdir" && pwd)/astrofind-${version}-source.tar.gz"
git -C "$root" archive --format=tar.gz --prefix="astrofind-${version}/" -o "$out" HEAD
echo "$out"
