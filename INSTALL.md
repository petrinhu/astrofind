# AstroFind — Installation Instructions

This file lists the commands needed to install AstroFind's build dependencies on each
supported Linux distribution (and WSL2).
After installing dependencies, build with:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

To also build the test suite, omit `--target AstroFind`:

```bash
cmake --build build -j$(nproc)
```

---

## Table of Contents

- [Fedora / RHEL / Rocky Linux](#fedora--rhel--rocky-linux)
- [Ubuntu 24.04 LTS](#ubuntu-2404-lts)
- [Debian 12 Bookworm](#debian-12-bookworm)
- [Linux Mint 22](#linux-mint-22)
- [Pop!_OS 22.04](#pop_os-2204)
- [Zorin OS 17](#zorin-os-17)
- [Arch Linux](#arch-linux)
- [Manjaro](#manjaro)
- [openSUSE Tumbleweed](#opensuse-tumbleweed)
- [Rocky Linux 9](#rocky-linux-9)
- [WSL2 / Ubuntu on Windows 11](#wsl2--ubuntu-on-windows-11)
- [Optional dependencies](#optional-dependencies)

---

## Fedora / RHEL / Rocky Linux

> Tested distribution: Fedora 39+ (primary development platform — item 55.10 ✅)

### Required

```bash
sudo dnf install \
    cmake gcc-c++ \
    qt6-qtbase-devel qt6-qtcharts-devel \
    qt6-qtopengl-devel qt6-qtnetwork-devel \
    qt6-qtsql-devel qt6-qtxml-devel \
    qt6-qtconcurrent-devel \
    cfitsio-devel fftw-devel
```

### Optional

```bash
sudo dnf install \
    qt6-qt5compat-devel \
    libarchive-devel \
    qtkeychain-qt6-devel libsecret-devel \
    qt6-linguist \
    valgrind cppcheck clang-tools-extra
```

---

## Ubuntu 24.04 LTS

> Item 55.1 — Docker QA target

### Required

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

> **Note:** Qt6 Network and Concurrent are typically included with `qt6-base-dev` on Ubuntu 24.04.
> If CMake cannot find them, also install `qt6-base-dev-tools`.

### Optional

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libqt6keychain1 qt6-qtkeychain-dev libsecret-1-dev \
    qt6-l10n-tools \
    valgrind cppcheck clang-tidy
```

---

## Debian 12 Bookworm

> Item 55.2 — Docker QA target

### Required

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

### Optional

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools \
    valgrind cppcheck clang-tidy
```

> **Note:** `qt6-qtkeychain-dev` may not be available in Debian 12 repos. Secure API key
> storage will fall back to `QSettings`; the application builds and runs normally without it.

---

## Linux Mint 22

> Item 55.5 — Docker QA target
> Linux Mint 22 is based on Ubuntu 24.04 — use the same commands.

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

### Optional

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Pop!_OS 22.04

> Item 55.7 — Docker QA target
> Pop!_OS 22.04 is based on Ubuntu 22.04 (Jammy). Qt 6.4 is available from the Ubuntu repos.

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

### Optional

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Zorin OS 17

> Item 55.9 — Docker QA target
> Zorin OS 17 is based on Ubuntu 22.04. Use the same commands as Pop!_OS 22.04.

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

### Optional

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Arch Linux

> Item 55.3 — Docker QA target

### Required

```bash
sudo pacman -S --needed \
    cmake gcc \
    qt6-base qt6-charts \
    cfitsio fftw
```

### Optional

```bash
sudo pacman -S --needed \
    qt6-5compat \
    libarchive \
    qtkeychain \
    qt6-tools \
    valgrind cppcheck clang
```

---

## Manjaro

> Item 55.4 — Docker QA target
> Manjaro uses the Arch package repositories — use the same `pacman` commands as Arch Linux.

```bash
sudo pacman -S --needed \
    cmake gcc \
    qt6-base qt6-charts \
    cfitsio fftw
```

### Optional

```bash
sudo pacman -S --needed \
    qt6-5compat \
    libarchive \
    qtkeychain \
    qt6-tools \
    valgrind cppcheck clang
```

---

## openSUSE Tumbleweed

> Item 55.6 — Docker QA target

### Required

```bash
sudo zypper install -y \
    cmake gcc-c++ \
    qt6-base-devel qt6-charts-devel \
    qt6-opengl-devel \
    cfitsio-devel fftw3-devel
```

### Optional

```bash
sudo zypper install -y \
    qt6-core5compat-devel \
    libarchive-devel \
    qtkeychain-qt6-devel libsecret-devel \
    qt6-linguist-devel \
    valgrind cppcheck clang-tools
```

---

## Rocky Linux 9

> Item 55.8 — Docker QA target
> Rocky Linux 9 ships Qt 5 in the default repos. Qt 6.4 must be installed from EPEL or built
> from source. The simplest path is to enable EPEL and use the Qt6 packages available there.

### Enable EPEL and CRB

```bash
sudo dnf install -y epel-release
sudo dnf config-manager --set-enabled crb
```

### Required

```bash
sudo dnf install -y \
    cmake gcc-c++ \
    qt6-qtbase-devel qt6-qtcharts-devel \
    qt6-qtopengl-devel qt6-qtnetwork-devel \
    qt6-qtsql-devel qt6-qtxml-devel \
    qt6-qtconcurrent-devel \
    cfitsio-devel fftw-devel
```

> **Note:** If Qt 6.4+ is not available in EPEL for Rocky 9, build Qt 6 from source or use
> a Fedora 39+ container as the build environment.

### Optional

```bash
sudo dnf install -y \
    qt6-qt5compat-devel \
    libarchive-devel \
    libsecret-devel \
    qt6-linguist
```

---

## WSL2 / Ubuntu on Windows 11

> Item 55.11 — WSL2 with WSLg (Windows Subsystem for Linux GUI)

Unit tests (`astrofind_tests`, `astrofind_ui_tests`) run without a display under WSL2.
The full GUI requires **WSLg** (included in Windows 11 builds ≥ 22000) and an OpenGL
software renderer if the GPU driver does not expose an OpenGL ICD in WSL.

### Required

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

### OpenGL software fallback (GUI only)

If the application fails to start with an OpenGL error, install Mesa's software renderer:

```bash
sudo apt-get install -y libgl1-mesa-dri mesa-utils
export LIBGL_ALWAYS_SOFTWARE=1   # add to ~/.bashrc for persistence
```

### Optional

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Optional dependencies

| Dependency | Purpose | Install |
|------------|---------|---------|
| QuaZip / Qt6Core5Compat | ZIP archive extraction | see distro section above |
| libarchive | TAR.GZ/BZ2/XZ, 7Z, RAR extraction | see distro section above |
| Qt6Keychain | Secure API key storage (falls back to QSettings if absent) | see distro section above |
| Qt6LinguistTools | Recompile `.ts` translation files after editing | see distro section above |
| ASTAP | Offline plate solver | download from [hnsky.org](https://www.hnsky.org/astap.htm) |
| valgrind | Memory-check audit (`cmake --build build --target audit-valgrind`) | see distro section above |
| cppcheck | Static analysis (`cmake --build build --target audit-cppcheck`) | see distro section above |
| clang-tidy | Static analysis (`cmake --build build --target audit-clang-tidy`) | see distro section above |

---

*AstroFind — Copyright (c) 2024–2026 Petrus Silva Costa — PolyForm Noncommercial License 1.0.0*
