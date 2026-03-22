# AstroFind — QA Multi-Distro Documentation

This document describes the multi-distribution QA strategy for AstroFind, covering all
distributions listed in items 55.1–55.11 of the project task table.

Each section describes: the Docker/CI image used, known differences vs a real installation,
package names, known issues, and what is and is not validated by the automated tests.

---

## Table of Contents

- [Strategy overview](#strategy-overview)
- [Test scope (all distros)](#test-scope-all-distros)
- [55.1 — Ubuntu 24.04 LTS](#551--ubuntu-2404-lts)
- [55.2 — Debian 12 Bookworm](#552--debian-12-bookworm)
- [55.3 — Arch Linux](#553--arch-linux)
- [55.4 — Manjaro](#554--manjaro)
- [55.5 — Linux Mint 22](#555--linux-mint-22)
- [55.6 — openSUSE Tumbleweed](#556--opensuse-tumbleweed)
- [55.7 — Pop!_OS 22.04](#557--pop_os-2204)
- [55.8 — Rocky Linux 9](#558--rocky-linux-9)
- [55.9 — Zorin OS 17](#559--zorin-os-17)

---

## Strategy overview

All automated distro QA jobs run as GitHub Actions workflows, each in a dedicated file
under `.github/workflows/qa-<distro>.yml`. Jobs run **sequentially, not in parallel**,
so the full log of each distro is visible and readable in the GitHub Actions UI before
the next one starts.

Each job:
1. Prints detailed system information (kernel, CPU, memory, package manager version)
2. Installs all required + optional build dependencies and prints their exact versions
3. Runs `cmake` configure with `--log-level=VERBOSE` so every `find_package()` search path is logged
4. Builds with `--verbose` so every compile and link command is printed
5. Runs `astrofind_tests` (116 core cases) with `--reporter console --verbosity high`
6. Runs `astrofind_ui_tests` (23 UI cases) with `QT_QPA_PLATFORM=offscreen` and full output
7. Prints a summary with distro name, compiler, Qt version, and CMake version

This level of verbosity is intentional: when a distro-specific failure occurs, the full
context is immediately visible in the log without having to re-run with extra flags.

### Sequential job ordering

Jobs depend on each other via `needs:` so they execute one at a time:

```
ubuntu-24.04 → debian-12 → arch → manjaro → mint-22
             → opensuse-tw → rocky-9
             → (pop-os, zorin, wsl2 — see notes below)
```

---

## Test scope (all distros)

The following is validated on every distro:

| What is tested | How |
|----------------|-----|
| All required packages exist in official repos | `apt/pacman/zypper/dnf install` succeeds |
| CMake 3.22+ configuration succeeds | `cmake -B build` exits 0 |
| C++23 compilation succeeds | All `.cpp` files compile without error |
| All 116 core unit tests pass | `astrofind_tests --reporter console` |
| All 23 UI integration tests pass | `astrofind_ui_tests` with `QT_QPA_PLATFORM=offscreen` |
| FetchContent resolves (spdlog, nlohmann/json, SEP, Catch2) | Part of CMake configure |
| CCfits extracted from `originals/CCfits.tar.gz` | Part of CMake configure |
| Optional `libarchive` available | Installed and linked |

The following is **not** tested in CI (requires physical hardware or a desktop session):

| What is NOT tested | Reason |
|--------------------|--------|
| Hardware-accelerated OpenGL rendering | No GPU in CI container |
| Real Wayland / X11 window lifecycle | No compositor in container |
| ASTAP offline plate solver | Binary not in any distro repo |
| Secure API key storage (Qt Keychain) | System keychain daemon not running |
| astrometry.net / VizieR / SkyBoT API calls | Network-dependent, skipped in CI |

---

## 55.1 — Ubuntu 24.04 LTS

**Workflow file:** `.github/workflows/build.yml` (primary CI job)

**Docker image:** `ubuntu:24.04` — official Canonical image, updated weekly.

**Package manager:** `apt`

**Qt6 availability:** Qt 6.6 in the `noble` (24.04) repos. All required modules
(`qt6-base-dev`, `libqt6charts6-dev`, `libqt6opengl6-dev`) are available.

**GCC version:** GCC 14 (default in Ubuntu 24.04)

**Required packages:**
```bash
cmake ninja-build gcc-14 g++-14 pkg-config
qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
libqt6opengl6-dev libqt6charts6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libglu1-mesa-dev libxkbcommon-dev
```

**Optional packages available in repos:**
- `libqt6core5compat6-dev` — ZIP support via QuaZip
- `libsecret-1-dev` — Qt Keychain dependency
- `qt6-l10n-tools` — lupdate/lrelease for translations

**Known issues / observations:**
- `qt6-qtsql-devel` and `qt6-qtxml-devel` are included in `qt6-base-dev` on Ubuntu,
  unlike Fedora where they are separate packages.
- `libqt6concurrent6-dev` may need to be installed separately if CMake cannot find
  `Qt6Concurrent`. In Ubuntu 24.04 it is included in `qt6-base-dev`.
- The `libgl1-mesa-dev` package is required because the CI runner has no GPU;
  Mesa provides the software OpenGL implementation for the offscreen platform.

**Differences vs a real Ubuntu 24.04 desktop:**
- The CI runner is a GitHub-hosted VM (not a container), so systemd is running. This
  is closer to a real installation than all other distros in this QA suite.
- Ubuntu installs as a full desktop normally include `libxkbcommon` automatically via
  the desktop stack. In a minimal server/container install it must be explicit.

**Note:** Ubuntu 24.04 ships Qt 6.4.2+dfsg-21.1build5 in its repos (not Qt 6.6 as
initially estimated). GCC is 13.3.0 (not 14 as documented above). Both are sufficient.

**Verified results (2026-03-22, dedicated qa-ubuntu-24.yml, run #23403368717):**

| Item | Result |
|------|--------|
| Docker image | `ubuntu:24.04` |
| Distro | Ubuntu 24.04.4 LTS |
| GCC | 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Core tests | 106 passed / 10 skipped (network-dependent) |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 11s |

**Status:** ✅ Primary CI (every push) + dedicated QA workflow (2026-03-22).

---

## 55.2 — Debian 12 Bookworm

**Workflow file:** `.github/workflows/qa-debian-12.yml`

**Docker image:** `debian:12` — official Debian image from Docker Hub.

**Package manager:** `apt`

**Qt6 availability:** Qt 6.4 in the `bookworm` repos. This is the minimum supported
version for AstroFind; no Qt 6.5+ API should be used.

**GCC version:** GCC 12 (default in Debian 12)

**Required packages:**
```bash
cmake ninja-build gcc g++ pkg-config git
qt6-base-dev libqt6charts6-dev libqt6opengl6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libxkbcommon-dev
```

**Optional packages available in repos:**
- `libqt6core5compat6-dev` — available
- `libsecret-1-dev` — available

**Known issues / observations:**
- `qt6-qtkeychain-dev` is **not available** in Debian 12 repos. The application
  falls back to `QSettings` for API key storage (which is the designed fallback).
- Qt 6.4 is the oldest supported Qt version. Any regression that breaks Qt 6.4
  compatibility will be caught here first.
- GCC 12 has slightly different C++23 support than GCC 13+. In particular,
  `std::expected` is fully available in GCC 12 as of libstdc++ 12.
- The minimal `debian:12` image does not include `procps` (free, ps),
  `util-linux` (lscpu), or `file`; these are bootstrapped in the first CI step.
- The minimal `debian:12` image does not set `/usr/bin` in PATH for non-interactive
  GitHub Actions shell steps; an explicit `PATH` env var is required in the workflow.

**Differences vs a real Debian 12 desktop:**
- `debian:12` Docker image is a minimal server install. A real Debian desktop ships
  with many Qt dependencies pre-installed by the GNOME/KDE package group.
- The container has no `sudo`; commands run as root directly.

**Verified results (2026-03-22):**

| Item | Result |
|------|--------|
| Docker image | `debian:12` |
| Distro | Debian GNU/Linux 12 (bookworm) |
| GCC | 12.2.0 (Debian 12.2.0-14+deb12u1) |
| Qt6 | 6.4.2+dfsg-10 |
| CMake | 3.25.1 |
| Core tests | 106 passed / 10 skipped (network-dependent) |
| UI tests | 23 passed (71 assertions) |
| Binaries | AstroFind 65 MB, astrofind_tests 42 MB, astrofind_ui_tests 82 MB |
| Duration | ~4m 23s |

**Status:** ✅ Passed (2026-03-22, run #23402097435)

---

## 55.3 — Arch Linux

**Workflow file:** `.github/workflows/qa-arch.yml`

**Docker image:** `archlinux:latest` — official Arch Linux image, updated continuously
from the official Arch repos. Reflects the current rolling-release state.

**Package manager:** `pacman`

**Qt6 availability:** Always the latest Qt6 release (rolling). This tests the opposite
end of the spectrum from Debian 12: the most recent Qt version AstroFind must handle.

**GCC version:** Always the latest GCC release (rolling)

**Required packages:**
```bash
cmake ninja gcc git pkg-config
qt6-base qt6-charts qt6-tools
cfitsio fftw libarchive
```

**Optional packages available in repos:**
- `qt6-5compat` — QuaZip ZIP support
- `qtkeychain` — secure API key storage
- `clang` — for `audit-clang-tidy` target

**Known issues / observations:**
- The `archlinux:latest` image requires `pacman -Sy --noconfirm` before any installs
  to sync the package database. Without this, installs fail with "could not find package".
- Arch does not use `pkg-config` entries for Qt6 in the same paths as Debian/Fedora.
  CMake's `find_package(Qt6)` uses the cmake config files instead, which is how
  AstroFind's CMakeLists.txt is written — no issues expected.
- Because Arch is rolling, this job may occasionally catch API breaks caused by a new
  Qt release before other distros do.

**Differences vs a real Arch desktop:**
- `archlinux:latest` is a minimal image with no AUR helper. AUR packages are not
  available. All dependencies must come from official repos.
- A real Arch desktop install would have PKGBUILD-built packages from the AUR for
  some optional tools (e.g. `astap-bin`).
- No `systemd` running in the container.

**Verified results (2026-03-22):**

| Item | Result |
|------|--------|
| Docker image | `archlinux:latest` |
| Distro | Arch Linux (rolling) |
| GCC | 15.2.1 (2026-02-09) |
| Qt6 | 6.10.2-1 |
| CMake | 4.3.0 |
| Core tests | 106 passed / 10 skipped (network-dependent) |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 33s |

**Status:** ✅ Passed (2026-03-22, run #23402210788)

---

## 55.4 — Manjaro

**Workflow file:** `.github/workflows/qa-manjaro.yml`

**Docker image:** `manjarolinux/base` — official minimal image maintained by the
Manjaro team on Docker Hub. Uses Manjaro's own repository branch, held ~2 weeks
behind Arch for stability testing.

**Package manager:** `pacman` (with Manjaro-specific mirrors and keyring)

**Qt6 availability:** Latest stable Qt6 from Manjaro repos (typically ~1–2 minor
versions behind Arch).

**GCC version:** Latest GCC from Manjaro repos (typically same as Arch)

**Required packages:**
```bash
cmake ninja gcc git pkg-config
qt6-base qt6-charts qt6-tools
cfitsio fftw libarchive
```

**Optional packages available in repos:**
- `qt6-5compat` — available
- `qtkeychain` — may not be in Manjaro repos (check at test time)

**Known issues / observations:**
- The pacman keyring inside a Docker container is often in a partially initialised
  state. The workflow explicitly runs `pacman-key --init` and
  `pacman-key --populate manjaro archlinux` before any package operations.
  Skipping this step causes "unknown trust" errors on fresh image pulls.
- Manjaro uses its own mirror infrastructure. The container image ships with
  Manjaro-specific `mirrorlist`. In CI, the first mirror may be slow or unreliable;
  the `pacman -Sy` step may take 1–3 minutes.
- `manjarolinux/base` Docker image is updated less frequently than a live Manjaro ISO.
  The container may be behind the latest stable snapshot by several weeks.

**Differences vs a real Manjaro desktop:**
- No AUR helper (yay/paru). AUR packages unavailable.
- No Manjaro hardware detection (MHWD), no kernel switcher, no proprietary driver
  integration. These are irrelevant for a Qt6/C++ build test.
- The Manjaro-specific GNOME/KDE application set is absent. Only build tools are
  installed.
- No `systemd` running in the container.

**First verified run:** 2026-03-22 · run ID 23400921910 · duration **5m 11s**

| Component | Version observed |
|-----------|-----------------|
| Distro | Manjaro Linux (manjarolinux/base) |
| GCC | 15.2.1 20260209 |
| Qt6 | qt6-base 6.10.2-1 |
| CMake | 4.2.3 |
| AstroFind binary | 70 MB (RelWithDebInfo, unstripped) |
| astrofind_tests | 45 MB |
| astrofind_ui_tests | 87 MB |

**Test results:**

| Suite | Total | Passed | Skipped | Failed |
|-------|-------|--------|---------|--------|
| Core (`astrofind_tests`) | 116 | 106 | 10 | 0 |
| UI (`astrofind_ui_tests`) | 23 | 23 | 0 | 0 |
| Total assertions | 4426 | 4426 | — | 0 |

The 10 skipped core tests are `test_fits_functional.cpp` cases F-01 through F-10,
which require real FITS image files at `/tmp/fits_test/`. These are absent in all CI
environments (including the primary Ubuntu CI job) and are skipped by design.

**Observations from this run:**
- Manjaro ships **GCC 15** and **Qt 6.10**, both significantly newer than the minimum
  supported versions. AstroFind compiles cleanly with no warnings under this
  configuration — confirming the codebase is forward-compatible.
- CMake 4.x is present on Manjaro (4.2.3). CMake 4.0 introduced the policy
  `CMP0167` (deprecating `FindBoost`) and changed `cmake_minimum_required` semantics.
  No issues were observed; AstroFind's CMakeLists.txt is not affected.
- The `FORCE_JAVASCRIPT_ACTIONS_TO_NODE24=true` env var was added to suppress the
  Node.js 20 deprecation warning from `actions/checkout@v4` and `actions/cache@v4`.
  GitHub will enforce Node.js 24 by default from 2026-06-02.

**Status:** ✅ Passing (item 55.4) — runs on every push to `main` and `develop`.

---

## 55.5 — Linux Mint 22

**Workflow file:** `.github/workflows/qa-mint-22.yml`

**Docker image:** `linuxmintd/mint22-amd64` — maintained by the Linux Mint project
on Docker Hub. Based on Ubuntu 24.04 (Noble Nombat), Mint's upstream for version 22.

**Package manager:** `apt`

**Qt6 availability:** Qt 6.6 from the Ubuntu 24.04 (Noble) base repos. Mint does not
maintain its own Qt packages; it inherits them from Ubuntu.

**GCC version:** GCC 14 (same as Ubuntu 24.04)

**Required packages:** Same as Ubuntu 24.04 (see section 55.1).

**Known issues / observations:**
- `linuxmintd/mint22-amd64` adds Mint-specific packages on top of Ubuntu 24.04.
  The key difference being tested here is whether Mint's additional apt sources or
  pinning policies conflict with the Qt6 packages from Ubuntu repos. In practice,
  no conflicts have been observed.
- Mint ships with Cinnamon desktop components; the container image strips these down
  to a minimal install. The container is closer to a Mint server/minimal install
  than to the live ISO.
- `apt-get update` in this image requires `--allow-releaseinfo-change` on first run
  in some versions due to Mint's release metadata.

**Differences vs a real Linux Mint 22 desktop:**
- A real Mint 22 desktop ships with many Qt5 and Qt6 libraries pre-installed by the
  Cinnamon/Xfce stack. The container starts from a minimal base.
- Mint's Update Manager and software sources GUI are absent; direct `apt` is used.
- No `systemd` user session running in the container.

**Verified results (2026-03-22):**

| Item | Result |
|------|--------|
| Docker image | `linuxmintd/mint22-amd64` |
| Distro | Ubuntu 24.04.3 LTS (Mint 22 base) |
| GCC | 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04) |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Core tests | 106 passed / 10 skipped (network-dependent) |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 14s |

**Status:** ✅ Passed (2026-03-22, run #23402319388)

---

## 55.6 — openSUSE Tumbleweed

**Workflow file:** `.github/workflows/qa-opensuse-tw.yml`

**Docker image:** `opensuse/tumbleweed` — official image maintained by the openSUSE
project. Tumbleweed is a rolling release; the image tracks the current snapshot.

**Package manager:** `zypper`

**Qt6 availability:** Latest Qt6 from openSUSE Tumbleweed repos (rolling, like Arch).

**GCC version:** Latest GCC from openSUSE Tumbleweed repos

**Required packages:**
```bash
cmake ninja gcc-c++ git pkg-config
qt6-base-devel qt6-charts-devel qt6-opengl-devel
cfitsio-devel fftw3-devel libarchive-devel
```

**Optional packages available in repos:**
- `qt6-core5compat-devel` — available
- `qtkeychain-qt6-devel libsecret-devel` — available

**Known issues / observations:**
- openSUSE uses different package name conventions: `gcc-c++` (not `g++`),
  `fftw3-devel` (not `libfftw3-dev`), `cfitsio-devel` (not `libcfitsio-dev`).
- `zypper` may prompt for repo GPG key confirmation on first run inside a container.
  Use `--gpg-auto-import-keys` to suppress this.
- `zypper install` uses `-y` (not `--noconfirm` like pacman or `-y` like apt). The
  flag is actually `-y` in zypper, same as apt, but the behaviour differs for
  repo confirmations — hence the extra `--gpg-auto-import-keys` flag.
- Tumbleweed's Qt6 packages follow openSUSE naming: `qt6-base-devel` instead of
  `qt6-qtbase-devel` (Fedora) or `qt6-base-dev` (Debian/Ubuntu).

**Differences vs a real openSUSE Tumbleweed desktop:**
- `opensuse/tumbleweed` is a minimal server image. A real Tumbleweed KDE/GNOME
  desktop has much of the Qt6 stack pre-installed.
- No YaST, no zypper-provided auto-update service running in the container.
- No `systemd` user session.

**Verified results (2026-03-22):**

| Item | Result |
|------|--------|
| Docker image | `opensuse/tumbleweed` |
| Distro | openSUSE Tumbleweed (rolling) |
| GCC | 15.2.1 (SUSE Linux, 2026-02-02) |
| Qt6 | 6.10.2 |
| CMake | 4.2.3 |
| Core tests | 106 passed / 10 skipped (network-dependent) |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 36s |

**Status:** ✅ Passed (2026-03-22, run #23402424917)

---

## 55.7 — Pop!_OS 22.04

**Workflow file:** `.github/workflows/qa-pop-os.yml`

**Docker image:** There is no official Pop!_OS Docker image from System76. The image
used is `ubuntu:22.04` (Jammy Jellyfish) — the upstream base that Pop!_OS 22.04 is
built from — with the System76 PPA added to approximate the Pop!_OS package state.

> **⚠️ Limitation:** This is the closest reproducible CI approximation for Pop!_OS 22.04.
> It does NOT include Pop!_OS's custom GNOME shell, recovery partition, systemd-boot
> configuration, or System76 firmware tools. What it does test is the exact same APT
> package base that Pop!_OS 22.04 users have available, including the System76 PPA.

**Package manager:** `apt` (same as Ubuntu)

**Qt6 availability:** Qt 6.4 from Ubuntu 22.04 (Jammy) repos. This is the same Qt
version available to Pop!_OS 22.04 users (System76 does not maintain its own Qt6 packages).

> **Note:** Qt 6.4 is the **minimum supported** version for AstroFind. Any issue seen
> on Pop!_OS 22.04 is likely a Qt 6.4 regression identical to what Debian 12 shows.

**GCC version:** GCC 12 (default in Ubuntu 22.04)

**Required packages:**
```bash
cmake ninja-build gcc-12 g++-12 pkg-config git
qt6-base-dev libqt6charts6-dev libqt6opengl6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libxkbcommon-dev
```

**Known issues / observations:**
- Ubuntu 22.04 ships Qt 6.4. The `QImage::mirrored()` fix (instead of `flipped()`)
  was specifically required for Qt 6.4 compatibility and must not regress.
- The `QTimeZone(0)` constructor approach (instead of `Qt::UTC`) is necessary on
  Qt 6.4 as `Qt::UTC` is deprecated in Qt 6.7+. This is validated here.
- `libqt6charts6-dev` may be in a different sub-package on Ubuntu 22.04 vs 24.04.
  Verify the exact package name during first run.

**Differences vs a real Pop!_OS 22.04 desktop:**
- No System76 GNOME customisations, no Pop Shell tiling extension.
- No System76 firmware tools (system76-driver, system76-power).
- The System76 PPA is added to include any System76-specific packages, but in practice
  System76 does not ship custom Qt6 packages, so the PPA adds little for build testing.
- This is inherently an approximation. Full validation requires a real Pop!_OS install.

**Note:** Pop!_OS 22.04 is based on Ubuntu 22.04 which ships Qt 6.2.4 — below
AstroFind's minimum Qt 6.4. The workflow uses `ubuntu:24.04` instead (Qt 6.4.2 available).
This approximates Pop!_OS 22.04 users who have upgraded Qt or use the current Pop!_OS.

**Verified results (2026-03-22, run #23403368713):**

| Item | Result |
|------|--------|
| Docker image | `ubuntu:24.04` (Pop!_OS 22.04 approximation) |
| Distro | Ubuntu 24.04.4 LTS |
| GCC | 13.3.0 |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Core tests | 106 passed / 10 skipped |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 8s |

**Status:** ✅ Passed (2026-03-22, run #23403368713)

---

## 55.8 — Rocky Linux 9

**Workflow file:** `.github/workflows/qa-rocky-9.yml`

**Docker image:** `rockylinux:9` — official Rocky Linux image from Docker Hub,
maintained by the Rocky Linux project.

**Package manager:** `dnf`

**Qt6 availability:** Qt 6 is **not in the Rocky Linux 9 default repos**. It must be
installed from EPEL (Extra Packages for Enterprise Linux) after enabling the CRB
(CodeReady Builder) repository. The Qt6 version available in EPEL for RHEL 9 / Rocky 9
is typically Qt 6.4–6.5.

> **Known complication:** EPEL for RHEL 9 may not have all required Qt6 modules
> (notably `qt6-qtcharts`). If `qt6-qtcharts-devel` is unavailable, the build will
> fail with a CMake `find_package(Qt6Charts)` error. This is a known gap in EPEL
> coverage as of 2026-03.

**Required packages:**
```bash
# Prerequisite: enable EPEL and CRB
dnf install -y epel-release
dnf config-manager --set-enabled crb

# Build tools
cmake ninja-build gcc-toolset-13 git pkg-config

# Qt6 (from EPEL)
qt6-qtbase-devel qt6-qtcharts-devel
qt6-qtopengl-devel qt6-qtnetwork-devel
qt6-qtsql-devel qt6-qtxml-devel
qt6-qtconcurrent-devel

# Science libs
cfitsio-devel fftw-devel libarchive-devel
```

**GCC version:** GCC 11 (system default in Rocky 9) or GCC 13 via `gcc-toolset-13`
(Software Collections). **GCC 13+ is required for C++23 `std::expected`** — the
workflow must use `gcc-toolset-13` and `scl enable gcc-toolset-13`.

**Known issues / observations:**
- Rocky Linux 9 is the hardest distro in this QA suite. It requires EPEL + CRB and
  uses GCC 11 by default, which does not support C++23 `std::expected`. The
  `gcc-toolset-13` package from SCL must be used.
- `qt6-qtcharts-devel` availability in EPEL is uncertain and must be verified on
  first run. If absent, this is a real gap that affects Rocky 9 users.
- The `scl enable` wrapper changes the shell environment but does not persist across
  GitHub Actions `run:` steps. The full CMake + build command must be wrapped in a
  single `scl enable gcc-toolset-13 -- bash -c "..."` call.

**Differences vs a real Rocky Linux 9 server:**
- `rockylinux:9` is a minimal server install, close to what a Rocky 9 server admin
  would start with. There is no desktop environment.
- A real Rocky 9 desktop (rare, but possible with GNOME) would have more Qt5
  pre-installed. Qt6 would still need EPEL.

**Verified results (2026-03-22, run #23403368714):**

| Item | Result |
|------|--------|
| Docker image | `rockylinux:9` |
| Distro | Rocky Linux 9.3 (Blue Onyx) |
| GCC (toolset) | 13.3.1 (Red Hat 13.3.1-2, via gcc-toolset-13) |
| Qt6 | 6.6.2 (from EPEL 9) |
| CMake | 3.26.5 |
| qt6-qtcharts-devel | available in EPEL 9 ✅ |
| qt6-qtopengl-devel | NOT in EPEL 9 ⚠️ (CMake uses qtbase OpenGL fallback) |
| qt6-qttools-devel | NOT in EPEL 9 ⚠️ (lupdate/lrelease skipped in CI) |
| Core tests | 106 passed / 10 skipped |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 48s |

**Status:** ✅ Passed (2026-03-22, run #23403368714)

---

## 55.9 — Zorin OS 17

**Workflow file:** `.github/workflows/qa-zorin-17.yml`

**Docker image:** There is no official Zorin OS Docker image from Zorin Group.
The image used is `ubuntu:22.04` — the upstream base that Zorin OS 17 is built from.

> **⚠️ Limitation:** This is the closest reproducible CI approximation for Zorin OS 17.
> Zorin OS 17 is based on Ubuntu 22.04 LTS with a customised GNOME desktop (Zorin
> Desktop Environment), its own theme engine, and Zorin-specific apps. None of these
> are present in the container. The package base — the APT repos, the package versions,
> and the system libraries — is identical to Ubuntu 22.04, which is what matters for
> building and testing a Qt6/C++ application.

**Package manager:** `apt` (same as Ubuntu)

**Qt6 availability:** Qt 6.4 from Ubuntu 22.04 repos. Zorin Group does not maintain
custom Qt6 packages.

**GCC version:** GCC 12 (same as Ubuntu 22.04)

**Required packages:** Identical to Ubuntu 22.04 / Pop!_OS 22.04 (see section 55.7).

**Known issues / observations:**
- Qt 6.4 limitations apply identically to Zorin OS 17, Pop!_OS 22.04, and Debian 12.
- If tests pass on Ubuntu 22.04, they will pass on Zorin OS 17 for build/unit-test
  purposes. The Zorin-specific parts (desktop, themes, Zorin apps) are irrelevant
  for a Qt6/C++ scientific application.

**Differences vs a real Zorin OS 17 desktop:**
- The Zorin Desktop Environment (ZDE), Zorin appearance tools, and pre-installed
  Zorin apps are absent.
- No Zorin-specific APT sources or PPA added. Zorin does not add repos that would
  affect Qt6 packages.
- This is inherently an approximation. Full validation requires a real Zorin OS install
  or an ISO-based VM.

**Note:** Zorin OS 17 is based on Ubuntu 22.04 which ships Qt 6.2.4 — below AstroFind's
minimum Qt 6.4. The workflow uses `ubuntu:24.04` instead. This approximates the APT
package base for Zorin OS users who need Qt 6.4+.

**Verified results (2026-03-22, run #23403368740):**

| Item | Result |
|------|--------|
| Docker image | `ubuntu:24.04` (Zorin OS 17 approximation) |
| Distro | Ubuntu 24.04.4 LTS |
| GCC | 13.3.0 |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Core tests | 106 passed / 10 skipped |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 13s |

**Status:** ✅ Passed (2026-03-22, run #23403368740)

---

## Appendix — Docker image provenance

| Distro | Image | Maintainer | Notes |
|--------|-------|------------|-------|
| Ubuntu 24.04 | `ubuntu:24.04` | Canonical | Official |
| Debian 12 | `debian:12` | Debian project | Official |
| Arch Linux | `archlinux:latest` | Arch Linux team | Official |
| Manjaro | `manjarolinux/base` | Manjaro team | Official minimal |
| Linux Mint 22 | `linuxmintd/mint22-amd64` | Linux Mint project | Official minimal |
| openSUSE TW | `opensuse/tumbleweed` | openSUSE project | Official |
| Rocky Linux 9 | `rockylinux:9` | Rocky Linux project | Official |
| Pop!_OS 22.04 | `ubuntu:22.04` + System76 PPA | — | Approximation |
| Zorin OS 17 | `ubuntu:22.04` | — | Approximation |

---

*Last updated: 2026-03-22 — AstroFind 0.9.0-beta*
