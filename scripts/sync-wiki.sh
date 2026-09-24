#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Publish docs/wiki/*.md to the GitHub wiki (https://github.com/petrinhu/astrofind/wiki).
#
# The wiki is a separate git repository (astrofind.wiki.git). Its pages are kept
# versioned here, next to the code they describe, and this script copies them over.
# Usage (from the repository root, with push access to the wiki):
#   scripts/sync-wiki.sh            # clone, copy, commit, push
#   scripts/sync-wiki.sh --dry-run  # show what would change, push nothing
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="${REPO_ROOT}/docs/wiki"
WIKI_URL="${WIKI_URL:-https://github.com/petrinhu/astrofind.wiki.git}"
DRY_RUN=false
[[ "${1:-}" == "--dry-run" ]] && DRY_RUN=true

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

git clone --quiet "$WIKI_URL" "$TMP/wiki"
# Mirror: pages removed from docs/wiki/ are removed from the wiki too.
find "$TMP/wiki" -maxdepth 1 -name '*.md' -delete
cp "$SRC"/*.md "$TMP/wiki/"

cd "$TMP/wiki"
git add -A
if git diff --cached --quiet; then
    echo "Wiki already up to date."
    exit 0
fi
git --no-pager diff --cached --stat
if $DRY_RUN; then
    echo "(dry run: nothing pushed)"
    exit 0
fi
REV="$(git -C "$REPO_ROOT" rev-parse --short HEAD)"
git commit --quiet -m "docs(wiki): sync from astrofind@${REV} (docs/wiki)"
git push --quiet origin HEAD
echo "Wiki updated from astrofind@${REV}."
