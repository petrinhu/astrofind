#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
#
# Prints the project version from the top-level CMakeLists.txt (the single
# source of truth; the packaging files are checked against it in release.yml).
set -euo pipefail
root=${1:-$(cd "$(dirname "$0")/../.." && pwd)}
sed -nE 's/^[[:space:]]*VERSION[[:space:]]+([0-9]+\.[0-9]+\.[0-9]+).*/\1/p' "$root/CMakeLists.txt" | head -1
