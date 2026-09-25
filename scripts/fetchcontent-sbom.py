#!/usr/bin/env python3
# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 Petrus Silva Costa
"""AUD-SEC-9: CycloneDX SBOM for the C/C++ sources AstroFind compiles in.

syft (see .github/workflows/sbom.yml) catalogs package-manager manifests; it
cannot read cmake/dependencies.cmake, so the libraries fetched by CMake
FetchContent and the CCfits tarball bundled in originals/ would be missing
from its SBOM. This script lists exactly those, straight from the pins:

    python3 scripts/fetchcontent-sbom.py > fetchcontent.cdx.json

System libraries (Qt6, cfitsio, FFTW3, libarchive, LibRaw) come from the
distro package manager and are covered by the distro's own SBOM/metadata.
Standard library only; no network access.
"""

import hashlib
import json
import pathlib
import re
import sys
import uuid

ROOT = pathlib.Path(__file__).resolve().parent.parent
DEPS = ROOT / "cmake" / "dependencies.cmake"
CCFITS_TARBALL = ROOT / "originals" / "CCfits.tar.gz"

# SPDX ids, same attribution as NOTICE.
LICENSES = {
    "spdlog": "MIT",
    "nlohmann_json": "MIT",
    "cfitsio": "CFITSIO",
    "sep": "LGPL-3.0-or-later AND BSD-3-Clause AND MIT",
    "catch2": "BSL-1.0",
    "ccfits": "CFITSIO",
}
# Test-only dependencies never reach the shipped binary.
SCOPE = {"catch2": "excluded"}
# Declared, but only fetched in some configurations.
CONDITION = {
    "cfitsio": "fallback: fetched only when no system cfitsio is found",
    "ccfits": "fallback: fetched only when originals/CCfits.tar.gz is absent",
}

DECLARE = re.compile(r"FetchContent_Declare\(\s*(?P<body>.*?)\)", re.S)


def parse_declares(text):
    for m in DECLARE.finditer(text):
        body = m.group("body")
        name = body.split()[0]
        repo = re.search(r"GIT_REPOSITORY\s+(\S+)", body)
        tag = re.search(r"GIT_TAG\s+(\S+)(?:\s*#\s*(\S+))?", body)
        if not repo or not tag:
            continue
        yield name, repo.group(1), tag.group(1), tag.group(2)


def component(name, repo, ref, version):
    key = re.sub(r"_(dl|src)$", "", name.lower())
    owner_repo = re.sub(r"^https://github\.com/|\.git$", "", repo)
    pinned = re.fullmatch(r"[0-9a-f]{40}", ref) is not None
    comp = {
        "type": "library",
        "bom-ref": f"fetchcontent:{name}",
        "name": key,
        "version": (version or ref).removeprefix(f"{key}-").lstrip("v"),
        "purl": f"pkg:github/{owner_repo}@{ref}",
        "externalReferences": [{"type": "vcs", "url": repo}],
        "properties": [
            {"name": "astrofind:source", "value": "cmake/dependencies.cmake FetchContent"},
            {"name": "astrofind:git-ref", "value": ref},
            {"name": "astrofind:pinned-to-commit", "value": str(pinned).lower()},
        ],
    }
    if key in LICENSES:
        comp["licenses"] = [{"expression": LICENSES[key]}]
    if key in SCOPE:
        comp["scope"] = SCOPE[key]
    if key in CONDITION:
        comp["properties"].append({"name": "astrofind:condition", "value": CONDITION[key]})
    return comp


def main():
    components = [component(*d) for d in parse_declares(DEPS.read_text(encoding="utf-8"))]
    if CCFITS_TARBALL.exists():
        digest = hashlib.sha256(CCFITS_TARBALL.read_bytes()).hexdigest()
        components.append({
            "type": "library",
            "bom-ref": "bundled:ccfits",
            "name": "ccfits",
            "version": "2.7",
            "hashes": [{"alg": "SHA-256", "content": digest}],
            "licenses": [{"expression": LICENSES["ccfits"]}],
            "properties": [{"name": "astrofind:source",
                            "value": "originals/CCfits.tar.gz (bundled)"}],
        })
    if not components:
        sys.exit("fetchcontent-sbom: no FetchContent_Declare found in " + str(DEPS))

    bom = {
        "bomFormat": "CycloneDX",
        "specVersion": "1.5",
        "serialNumber": f"urn:uuid:{uuid.uuid4()}",
        "version": 1,
        "metadata": {"component": {"type": "application", "name": "AstroFind",
                                   "bom-ref": "astrofind"}},
        "components": components,
    }
    json.dump(bom, sys.stdout, indent=2)
    sys.stdout.write("\n")


if __name__ == "__main__":
    main()
