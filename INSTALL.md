# AstroFind: Installation Instructions / Instruções de Instalação

> **Last reviewed / Última revisão:** 2026-07-10
> **Owner:** Petrus Silva Costa
> **Applies to / Aplica-se a:** AstroFind v0.9.0+

---

## Table of Contents / Índice

- [Quick install / Instalação rápida](#quick-install--instalação-rápida)
- [Build from source / Compilação do código-fonte](#build-from-source--compilação-do-código-fonte)
- [Fedora / RHEL / Rocky Linux](#fedora--rhel--rocky-linux)
- [Ubuntu 24.04 LTS](#ubuntu-2404-lts)
- [Debian 12 Bookworm](#debian-12-bookworm)
- [Linux Mint 22](#linux-mint-22)
- [Pop!_OS 22.04](#popos-2204)
- [Zorin OS 17](#zorin-os-17)
- [Arch Linux](#arch-linux)
- [Manjaro](#manjaro)
- [openSUSE Tumbleweed](#opensuse-tumbleweed)
- [Rocky Linux 9](#rocky-linux-9)
- [Optional dependencies / Dependências opcionais](#optional-dependencies--dependências-opcionais)

---

## Quick install / Instalação rápida

### 🇬🇧 English (binary packages, recommended)

Download the universal installer from the
[latest release](https://codeberg.org/petrinhu/astrofind/releases/latest):

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/install.sh
chmod +x install.sh
./install.sh
```

The installer auto-detects your Linux distribution ("distro" for short: the specific
operating system flavor, e.g. Fedora, Ubuntu, Arch), downloads the correct binary
package (RPM/DEB/PKGBUILD, the three main Linux package formats), installs
dependencies, and sets up desktop integration (menu entry, icon). Bilingual (EN/PT-BR)
interactive interface.

#### Direct package download

Replace `v0.9.0-beta` with the tag of the release you want.

**Fedora / RHEL / Rocky Linux (RPM package):**

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/astrofind-0.9.0-0.beta1.x86_64.rpm
sudo rpm -i astrofind-0.9.0-0.beta1.x86_64.rpm
```

Dependencies not bundled in the RPM (install separately):

```bash
sudo dnf install qt6-qtbase qt6-qtcharts qt6-qtopengl cfitsio fftw libarchive
```

**Ubuntu / Debian / Linux Mint / Pop!_OS / Zorin OS (DEB package):**

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/astrofind_0.9.0~beta-1_amd64.deb
sudo apt-get install ./astrofind_0.9.0~beta-1_amd64.deb
```

`apt-get install ./file.deb` resolves dependencies automatically.

**Arch Linux / Manjaro (PKGBUILD, a build recipe script used by Arch's package
manager):**

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/astrofind-0.9.0-beta-arch-PKGBUILD.tar.gz
tar xzf astrofind-0.9.0-beta-arch-PKGBUILD.tar.gz
cd astrofind-arch
makepkg -si
```

`makepkg -si` installs declared dependencies via `pacman` and builds + installs the
package.

### 🇧🇷 Português (pacotes binários, recomendado)

Baixe o instalador universal da
[última versão](https://codeberg.org/petrinhu/astrofind/releases/latest):

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/install.sh
chmod +x install.sh
./install.sh
```

O instalador detecta automaticamente sua distribuição Linux ("distro", a variante
específica do sistema operacional, ex.: Fedora, Ubuntu, Arch), baixa o pacote binário
correto (RPM/DEB/PKGBUILD, os três principais formatos de pacote do Linux), instala as
dependências e configura a integração com o desktop (entrada de menu, ícone). Interface
interativa bilíngue (EN/PT-BR).

#### Download direto do pacote

Substitua `v0.9.0-beta` pela tag da versão desejada.

**Fedora / RHEL / Rocky Linux (pacote RPM):**

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/astrofind-0.9.0-0.beta1.x86_64.rpm
sudo rpm -i astrofind-0.9.0-0.beta1.x86_64.rpm
```

Dependências não incluídas no RPM (instalar separadamente):

```bash
sudo dnf install qt6-qtbase qt6-qtcharts qt6-qtopengl cfitsio fftw libarchive
```

**Ubuntu / Debian / Linux Mint / Pop!_OS / Zorin OS (pacote DEB):**

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/astrofind_0.9.0~beta-1_amd64.deb
sudo apt-get install ./astrofind_0.9.0~beta-1_amd64.deb
```

`apt-get install ./arquivo.deb` resolve dependências automaticamente.

**Arch Linux / Manjaro (PKGBUILD, um script-receita de build usado pelo gerenciador de
pacotes do Arch):**

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/astrofind-0.9.0-beta-arch-PKGBUILD.tar.gz
tar xzf astrofind-0.9.0-beta-arch-PKGBUILD.tar.gz
cd astrofind-arch
makepkg -si
```

`makepkg -si` instala as dependências declaradas via `pacman` e compila + instala o
pacote.

---

## Build from source / Compilação do código-fonte

### 🇬🇧 English

This section lists the commands needed to install build dependencies on each
supported Linux distribution. After installing dependencies, build with:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

`cmake -B build` configures the project into a `build/` directory ("out-of-source
build": the generated files stay separate from your source code). `-j$(nproc)` uses
all available CPU cores to compile in parallel.

To also build the test suite, omit `--target AstroFind`:

```bash
cmake --build build -j$(nproc)
```

### 🇧🇷 Português

Esta seção lista os comandos necessários para instalar as dependências de build em
cada distribuição Linux suportada. Após instalar as dependências, compile com:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

`cmake -B build` configura o projeto num diretório `build/` ("build fora da árvore de
fontes": os arquivos gerados ficam separados do seu código-fonte). `-j$(nproc)` usa
todos os núcleos de CPU disponíveis para compilar em paralelo.

Para também compilar a suíte de testes, omita `--target AstroFind`:

```bash
cmake --build build -j$(nproc)
```

---

## Fedora / RHEL / Rocky Linux

> Tested distribution / Distribuição testada: Fedora 39+ (primary development platform
> / plataforma de desenvolvimento primária, item 55.10 ✅)

### 🇬🇧 English

**Required:**

```bash
sudo dnf install \
    cmake gcc-c++ \
    qt6-qtbase-devel qt6-qtcharts-devel \
    qt6-qtopengl-devel qt6-qtnetwork-devel \
    qt6-qtsql-devel qt6-qtxml-devel \
    qt6-qtconcurrent-devel \
    cfitsio-devel fftw-devel
```

**Optional:**

```bash
sudo dnf install \
    qt6-qt5compat-devel \
    libarchive-devel \
    qtkeychain-qt6-devel libsecret-devel \
    qt6-linguist \
    valgrind cppcheck clang-tools-extra
```

### 🇧🇷 Português

**Obrigatórias:**

```bash
sudo dnf install \
    cmake gcc-c++ \
    qt6-qtbase-devel qt6-qtcharts-devel \
    qt6-qtopengl-devel qt6-qtnetwork-devel \
    qt6-qtsql-devel qt6-qtxml-devel \
    qt6-qtconcurrent-devel \
    cfitsio-devel fftw-devel
```

**Opcionais:**

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

> Item 55.1, Docker QA target (a Docker container is used in CI to verify the build
> on this distro without needing a real machine running it / um container Docker é
> usado em CI para verificar o build nesta distro sem precisar de uma máquina real
> rodando-a)

### 🇬🇧 English

**Required:**

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

> **Note:** Qt6 Network and Concurrent are typically included with `qt6-base-dev` on
> Ubuntu 24.04. If CMake cannot find them, also install `qt6-base-dev-tools`.

**Optional:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libqt6keychain1 qt6-qtkeychain-dev libsecret-1-dev \
    qt6-l10n-tools \
    valgrind cppcheck clang-tidy
```

### 🇧🇷 Português

**Obrigatórias:**

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

> **Nota:** Qt6 Network e Concurrent normalmente já vêm com `qt6-base-dev` no
> Ubuntu 24.04. Se o CMake não conseguir encontrá-los, instale também
> `qt6-base-dev-tools`.

**Opcionais:**

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

> Item 55.2, Docker QA target

### 🇬🇧 English

**Required:**

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Optional:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools \
    valgrind cppcheck clang-tidy
```

> **Note:** `qt6-qtkeychain-dev` may not be available in Debian 12 repos. Secure API
> key storage will fall back to `QSettings`; the application builds and runs normally
> without it.

### 🇧🇷 Português

**Obrigatórias:**

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Opcionais:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools \
    valgrind cppcheck clang-tidy
```

> **Nota:** `qt6-qtkeychain-dev` pode não estar disponível nos repositórios do
> Debian 12. O armazenamento seguro de chave de API cai para `QSettings`
> (fallback); a aplicação compila e roda normalmente sem ele.

---

## Linux Mint 22

> Item 55.5, Docker QA target. Linux Mint 22 is based on Ubuntu 24.04, use the same
> commands / Linux Mint 22 é baseado no Ubuntu 24.04, use os mesmos comandos.

### 🇬🇧 English

**Required:**

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Optional:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

### 🇧🇷 Português

**Obrigatórias:**

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Opcionais:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Pop!_OS 22.04

> Item 55.7, Docker QA target. Pop!_OS 22.04 is based on Ubuntu 22.04 (Jammy). Qt 6.4
> is available from the Ubuntu repos / Pop!_OS 22.04 é baseado no Ubuntu 22.04
> (Jammy). Qt 6.4 está disponível nos repositórios do Ubuntu.

### 🇬🇧 English

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Optional:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

### 🇧🇷 Português

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Opcionais:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Zorin OS 17

> Item 55.9, Docker QA target. Zorin OS 17 is based on Ubuntu 22.04. Use the same
> commands as Pop!_OS 22.04 / Zorin OS 17 é baseado no Ubuntu 22.04. Use os mesmos
> comandos do Pop!_OS 22.04.

### 🇬🇧 English

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Optional:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

### 🇧🇷 Português

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake g++ \
    qt6-base-dev libqt6charts6-dev \
    libqt6opengl6-dev \
    libqt6sql6-dev libqt6xml6-dev \
    libcfitsio-dev libfftw3-dev
```

**Opcionais:**

```bash
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libarchive-dev \
    libsecret-1-dev \
    qt6-l10n-tools
```

---

## Arch Linux

> Item 55.3, Docker QA target

### 🇬🇧 English

**Required:**

```bash
sudo pacman -S --needed \
    cmake gcc \
    qt6-base qt6-charts \
    cfitsio fftw
```

**Optional:**

```bash
sudo pacman -S --needed \
    qt6-5compat \
    libarchive \
    qtkeychain \
    qt6-tools \
    valgrind cppcheck clang
```

### 🇧🇷 Português

**Obrigatórias:**

```bash
sudo pacman -S --needed \
    cmake gcc \
    qt6-base qt6-charts \
    cfitsio fftw
```

**Opcionais:**

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

> Item 55.4, Docker QA target. Manjaro uses the Arch package repositories, use the
> same `pacman` commands as Arch Linux / Manjaro usa os repositórios de pacotes do
> Arch, use os mesmos comandos `pacman` do Arch Linux.

### 🇬🇧 English

```bash
sudo pacman -S --needed \
    cmake gcc \
    qt6-base qt6-charts \
    cfitsio fftw
```

**Optional:**

```bash
sudo pacman -S --needed \
    qt6-5compat \
    libarchive \
    qtkeychain \
    qt6-tools \
    valgrind cppcheck clang
```

### 🇧🇷 Português

```bash
sudo pacman -S --needed \
    cmake gcc \
    qt6-base qt6-charts \
    cfitsio fftw
```

**Opcionais:**

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

> Item 55.6, Docker QA target

### 🇬🇧 English

**Required:**

```bash
sudo zypper install -y \
    cmake gcc-c++ \
    qt6-base-devel qt6-charts-devel \
    qt6-opengl-devel \
    cfitsio-devel fftw3-devel
```

**Optional:**

```bash
sudo zypper install -y \
    qt6-core5compat-devel \
    libarchive-devel \
    qtkeychain-qt6-devel libsecret-devel \
    qt6-linguist-devel \
    valgrind cppcheck clang-tools
```

### 🇧🇷 Português

**Obrigatórias:**

```bash
sudo zypper install -y \
    cmake gcc-c++ \
    qt6-base-devel qt6-charts-devel \
    qt6-opengl-devel \
    cfitsio-devel fftw3-devel
```

**Opcionais:**

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

> Item 55.8, Docker QA target. Rocky Linux 9 ships Qt 5 in the default repos. Qt 6.4
> must be installed from EPEL ("Extra Packages for Enterprise Linux", a community
> repository with extra packages for RHEL-family distros) or built from source. The
> simplest path is to enable EPEL and use the Qt6 packages available there. / Rocky
> Linux 9 traz Qt 5 nos repositórios padrão. Qt 6.4 precisa ser instalado a partir do
> EPEL ("Extra Packages for Enterprise Linux", um repositório comunitário com pacotes
> extras para distros da família RHEL) ou compilado do fonte. O caminho mais simples
> é habilitar o EPEL e usar os pacotes Qt6 disponíveis lá.

### 🇬🇧 English

**Enable EPEL and CRB** ("CodeReady Builder", a repo that unlocks `-devel` packages
needed to compile software on RHEL/Rocky):

```bash
sudo dnf install -y epel-release
sudo dnf config-manager --set-enabled crb
```

**Required:**

```bash
sudo dnf install -y \
    cmake gcc-c++ \
    qt6-qtbase-devel qt6-qtcharts-devel \
    qt6-qtopengl-devel qt6-qtnetwork-devel \
    qt6-qtsql-devel qt6-qtxml-devel \
    qt6-qtconcurrent-devel \
    cfitsio-devel fftw-devel
```

> **Note:** If Qt 6.4+ is not available in EPEL for Rocky 9, build Qt 6 from source
> or use a Fedora 39+ container as the build environment.

**Optional:**

```bash
sudo dnf install -y \
    qt6-qt5compat-devel \
    libarchive-devel \
    libsecret-devel \
    qt6-linguist
```

### 🇧🇷 Português

**Habilitar EPEL e CRB** ("CodeReady Builder", um repositório que libera os pacotes
`-devel` necessários para compilar software no RHEL/Rocky):

```bash
sudo dnf install -y epel-release
sudo dnf config-manager --set-enabled crb
```

**Obrigatórias:**

```bash
sudo dnf install -y \
    cmake gcc-c++ \
    qt6-qtbase-devel qt6-qtcharts-devel \
    qt6-qtopengl-devel qt6-qtnetwork-devel \
    qt6-qtsql-devel qt6-qtxml-devel \
    qt6-qtconcurrent-devel \
    cfitsio-devel fftw-devel
```

> **Nota:** Se Qt 6.4+ não estiver disponível no EPEL para Rocky 9, compile o Qt 6 do
> fonte ou use um container Fedora 39+ como ambiente de build.

**Opcionais:**

```bash
sudo dnf install -y \
    qt6-qt5compat-devel \
    libarchive-devel \
    libsecret-devel \
    qt6-linguist
```

---

## Optional dependencies / Dependências opcionais

### 🇬🇧 English

| Dependency | Purpose | Install |
|------------|---------|---------|
| QuaZip / Qt6Core5Compat | ZIP archive extraction | see distro section above |
| libarchive | TAR.GZ/BZ2/XZ, 7Z, RAR extraction | see distro section above |
| Qt6Keychain | Secure API key storage (falls back to QSettings if absent) | see distro section above |
| Qt6LinguistTools | Recompile `.ts` translation files after editing | see distro section above |
| ASTAP | Offline plate solver ("plate solving": figuring out which patch of sky an image shows, by matching star patterns to a catalog) | download from [hnsky.org](https://www.hnsky.org/astap.htm) |
| valgrind | Memory-check audit (`cmake --build build --target audit-valgrind`) | see distro section above |
| cppcheck | Static analysis, checks the code without running it (`cmake --build build --target audit-cppcheck`) | see distro section above |
| clang-tidy | Static analysis (`cmake --build build --target audit-clang-tidy`) | see distro section above |

### 🇧🇷 Português

| Dependência | Propósito | Instalação |
|------------|---------|---------|
| QuaZip / Qt6Core5Compat | Extração de arquivos ZIP | ver seção da distro acima |
| libarchive | Extração de TAR.GZ/BZ2/XZ, 7Z, RAR | ver seção da distro acima |
| Qt6Keychain | Armazenamento seguro de chave de API (cai para QSettings se ausente) | ver seção da distro acima |
| Qt6LinguistTools | Recompilar arquivos de tradução `.ts` após edição | ver seção da distro acima |
| ASTAP | Plate solver offline ("plate solving": descobrir qual pedaço do céu uma imagem mostra, casando o padrão de estrelas com um catálogo) | baixar em [hnsky.org](https://www.hnsky.org/astap.htm) |
| valgrind | Auditoria de checagem de memória (`cmake --build build --target audit-valgrind`) | ver seção da distro acima |
| cppcheck | Análise estática, checa o código sem executá-lo (`cmake --build build --target audit-cppcheck`) | ver seção da distro acima |
| clang-tidy | Análise estática (`cmake --build build --target audit-clang-tidy`) | ver seção da distro acima |

---

*AstroFind. Copyright (c) 2024-2026 Petrus Silva Costa. GNU Affero General Public
License v3.0 (AGPL-3.0).*
