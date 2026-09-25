# AstroFind

> 🇬🇧 Modern astrometry software for asteroid detection and measurement, built for citizen
> science, education, and the IASC / Minor Planet Center (MPC) workflow.
> 🇧🇷 Software moderno de astrometria para detecção e medição de asteroides, feito para
> ciência cidadã, educação e o fluxo de trabalho do IASC / Minor Planet Center (MPC).

<!-- Release / license / CI -->
[![License: AGPL v3](https://img.shields.io/badge/license-AGPL--3.0-blue)](LICENSE)
[![Release](https://img.shields.io/badge/release-v1.2.0-orange)](https://github.com/petrinhu/astrofind/releases/latest)
[![CI](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/build.yml?branch=main&label=CI&logo=githubactions&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/build.yml)
[![Audit](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/audit.yml?branch=main&label=audit&logo=githubactions&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/audit.yml)

<!-- Stack -->
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![Qt6](https://img.shields.io/badge/Qt6-6.4%2B-41CD52?logo=qt&logoColor=white)](https://doc.qt.io/qt-6/)
[![CMake](https://img.shields.io/badge/CMake-%E2%89%A53.22-064F8C?logo=cmake)](CMakeLists.txt)
[![Platform](https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black)](INSTALL.md)

<!-- Quality -->
[![Tests](https://img.shields.io/badge/tests-206%20cases%20passing-brightgreen)](#testing--testes)
[![ASan/UBSan](https://img.shields.io/badge/ASan%2FUBSan-clean-success)](.github/workflows/audit.yml)
[![cppcheck](https://img.shields.io/badge/cppcheck-clean-success)](.github/workflows/audit.yml)
[![clang-tidy](https://img.shields.io/badge/clang--tidy-checked-success)](.github/workflows/audit.yml)

<!-- QA matrix, validated distros: live CI status on main (green "passing" = build + tests OK); Fedora and CachyOS run in the audit matrix -->
[![Ubuntu](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-ubuntu-24.yml?branch=main&label=Ubuntu%2024.04&logo=ubuntu&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-ubuntu-24.yml)
[![Debian](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-debian-12.yml?branch=main&label=Debian%2012&logo=debian&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-debian-12.yml)
[![Fedora](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/audit.yml?branch=main&label=Fedora%2044&logo=fedora&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/audit.yml)
[![CachyOS](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/audit.yml?branch=main&label=CachyOS)](https://github.com/petrinhu/astrofind/actions/workflows/audit.yml)
[![Arch Linux](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-arch.yml?branch=main&label=Arch%20Linux&logo=archlinux&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-arch.yml)
[![Manjaro](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-manjaro.yml?branch=main&label=Manjaro&logo=manjaro&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-manjaro.yml)
[![openSUSE](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-opensuse-tw.yml?branch=main&label=openSUSE%20Tumbleweed&logo=opensuse&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-opensuse-tw.yml)
[![Rocky Linux](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-rocky-9.yml?branch=main&label=Rocky%20Linux%209&logo=rockylinux&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-rocky-9.yml)
[![Linux Mint](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-mint-22.yml?branch=main&label=Linux%20Mint%2022&logo=linuxmint&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-mint-22.yml)
[![Pop!_OS](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-pop-os-22.yml?branch=main&label=Pop!_OS%20%2824.04%20base%29&logo=popos&logoColor=white)](https://github.com/petrinhu/astrofind/actions/workflows/qa-pop-os-22.yml)
[![Zorin OS](https://img.shields.io/github/actions/workflow/status/petrinhu/astrofind/qa-zorin-17.yml?branch=main&label=Zorin%20OS%20%2824.04%20base%29)](https://github.com/petrinhu/astrofind/actions/workflows/qa-zorin-17.yml)

<!-- Community -->
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen)](CONTRIBUTING.md)
[![Made for astronomers](https://img.shields.io/badge/made%20for-astronomers-blueviolet)](#what-it-is--o-que-é)
[![i18n](https://img.shields.io/badge/i18n-EN%20%7C%20PT--BR-informational)](i18n/)

<!-- Topics -->
[![astrometry](https://img.shields.io/badge/topic-astrometry-lightgrey)](docs/technical-reference.md)
[![asteroid detection](https://img.shields.io/badge/topic-asteroid--detection-lightgrey)](docs/technical-reference.md)
[![FITS](https://img.shields.io/badge/topic-FITS-lightgrey)](docs/technical-reference.md#1-fits-file-structure--estrutura-de-arquivo-fits)
[![WCS](https://img.shields.io/badge/topic-WCS-lightgrey)](docs/technical-reference.md#3-wcs-pipeline-pixel-to-sky--pipeline-wcs-pixel-para-céu)
[![MPC/ADES](https://img.shields.io/badge/topic-MPC%2FADES-lightgrey)](https://minorplanetcenter.net/ades)
[![photometry](https://img.shields.io/badge/topic-photometry-lightgrey)](docs/technical-reference.md)

---

## Table of contents / Índice

- [What it is / O que é](#what-it-is--o-que-é)
- [Features / Recursos](#features--recursos)
- [Screenshots / Capturas de tela](#screenshots--capturas-de-tela)
- [Installation / Instalação](#installation--instalação)
- [Quick start / Uso rápido](#quick-start--uso-rápido)
- [Technical stack / Stack técnica](#technical-stack--stack-técnica)
- [Dependencies / Dependências](#dependencies--dependências)
- [Testing / Testes](#testing--testes)
- [Documentation / Documentação](#documentation--documentação)
- [Contributing / Contribuindo](#contributing--contribuindo)
- [License / Licença](#license--licença)
- [Acknowledgments / Agradecimentos](#acknowledgments--agradecimentos)

---

## What it is / O que é

### 🇬🇧 English

AstroFind is a desktop application for **astrometry** (the measurement of the precise
position of an object in the sky), applied to asteroid detection and measurement. It
follows the workflow used by the International Asteroid Search Campaign (IASC) and the
**Minor Planet Center (MPC)**: the organization that catalogs every confirmed asteroid and
comet observation worldwide. AstroFind loads astronomical images, detects moving objects
across a sequence of frames, measures their position and brightness, and exports the
result as an **ADES** report (Astrometry Data Exchange Standard, the XML/PSV format the
MPC requires for submissions).

> **⚠️ Validate before you submit.** The test suite passes on 11 Linux distributions and
> the numerical audit (ASan/UBSan, Valgrind, cppcheck, clang-tidy) runs on every pull
> request, but an end-to-end validation against real observatory images is still to be
> published, and a few audit items are open (see [`TODO.md`](TODO.md)). Check astrometry,
> photometry and the ADES report independently before submitting to the MPC. Real-world
> test reports are very welcome; see [Contributing](#contributing--contribuindo).

> *"To my son Guga, a Linux user like me,*
> *who had no good option for finding asteroids."*

### 🇧🇷 Português

O AstroFind é uma aplicação de desktop para **astrometria** (a medição da posição precisa
de um objeto no céu), aplicada à detecção e medição de asteroides. Ele segue o fluxo de
trabalho usado pela International Asteroid Search Campaign (IASC) e pelo **Minor Planet
Center (MPC)**: a organização que cataloga toda observação confirmada de asteroide e
cometa no mundo. O AstroFind carrega imagens astronômicas, detecta objetos em movimento ao
longo de uma sequência de quadros, mede sua posição e brilho, e exporta o resultado como um
relatório **ADES** (Astrometry Data Exchange Standard, o formato XML/PSV exigido pelo MPC
para submissões).

> **⚠️ Valide antes de submeter.** A suíte de testes passa em 11 distribuições Linux e a
> auditoria numérica (ASan/UBSan, Valgrind, cppcheck, clang-tidy) roda em todo pull
> request, mas uma validação ponta a ponta com imagens reais de observatório ainda não foi
> publicada, e alguns itens da auditoria seguem abertos (veja [`TODO.md`](TODO.md)).
> Confira astrometria, fotometria e o relatório ADES de forma independente antes de
> submeter ao MPC. Relatos de testes com dados reais são muito bem-vindos; veja
> [Contribuindo](#contributing--contribuindo).

> *"Para meu filho Guga, usuário de Linux como eu,*
> *que estava sem uma boa opção para buscar asteroides."*

---

## Features / Recursos

### 🇬🇧 English

- **Star detection and measurement**: automated star detection (SEP / Source Extractor),
  streak and blended-source detection, and elliptical **PSF** (Point Spread Function, the
  blur profile a point of light forms on the sensor) fitting via Levenberg-Marquardt for
  the measured object, which also reveals coma and tracking errors from the star's shape.
- **Astrometry / WCS**: **WCS** (World Coordinate System, the mapping between pixel and sky
  coordinates) in 8 projections (TAN, SIN, ARC, STG, CAR, MER, GLS, AIT); plate solving
  online via astrometry.net or offline via ASTAP (images that already carry a WCS are kept);
  reference stars from UCAC4 or Gaia DR3 (VizieR, with proper motion to the image epoch) or a
  local FITS catalog; atmospheric refraction correction (Bennett, skipped for space
  telescopes). Reported positions are ICRF.
- **Photometry**: aperture and differential **photometry** (brightness measurement) with
  zero-point from catalog stars, airmass/extinction correction, light-curve and
  growth-curve dialogs.
- **Moving-object detection**: automatic candidate search across 3 or more images, blink
  comparison, streak/trail detection for fast movers and satellites, Track and Stack along
  a motion vector, and image stacking (Add / Average / Median, aligned by stars or FFT
  phase correlation).
- **Known objects and reporting**: known asteroids/comets in the field from IMCCE SkyBoT
  (or offline from MPCORB.DAT), JPL Horizons ephemerides, ADES 2022 XML/PSV report with
  preview, and submission to the MPC by HTTP or e-mail.
- **File formats**: FITS (single/multi-HDU, RGB, temporal cubes), SER video, XISF
  (PixInsight), DSLR RAW (CR2/CR3/NEF/ARW/DNG/… via LibRaw, optional), NASA PDS3/PDS4,
  TIFF/PNG/BMP/JPEG, and compressed archives (ZIP, TAR.*, 7Z, RAR).
- **Image display**: zoom/pan/blink comparison across frames, multiple stretch/LUT modes,
  histogram and 2-D power-spectrum diagnostics.
- **Calibration**: dark/flat pipeline with master-frame builder, calibration wizard,
  bad-pixel correction, sliding-median background removal.
- **Interface**: bilingual UI (English / Brazilian Portuguese), Night/Day/Auto theme, a
  six-step guided workflow panel (with an optional automatic mode), and school features
  (teacher e-mail, shareable school configuration).

See [`docs/technical-reference.md`](docs/technical-reference.md) for the full algorithm
and coordinate-system reference, and the [Wiki](https://github.com/petrinhu/astrofind/wiki)
for the complete feature list and how-to guides.

### 🇧🇷 Português

- **Detecção e medição de estrelas**: detecção automática de estrelas (SEP / Source
  Extractor), detecção de traços e de fontes sobrepostas, e ajuste de **PSF** elíptica
  (Point Spread Function, o perfil de borrão que um ponto de luz forma no sensor) via
  Levenberg-Marquardt no objeto medido, que também revela coma e erros de rastreamento
  pela forma da estrela.
- **Astrometria / WCS**: **WCS** (World Coordinate System, o mapeamento entre coordenadas
  de pixel e coordenadas celestes) em 8 projeções (TAN, SIN, ARC, STG, CAR, MER, GLS, AIT);
  plate solving online via astrometry.net ou offline via ASTAP (imagens que já têm WCS são
  mantidas); estrelas de referência do UCAC4 ou Gaia DR3 (VizieR, com movimento próprio
  levado à época da imagem) ou de um catálogo FITS local; correção de refração atmosférica
  (Bennett, ignorada para telescópios espaciais). As posições reportadas são ICRF.
- **Fotometria**: **fotometria** de abertura e diferencial (medição de brilho) com
  zero-point a partir das estrelas do catálogo, correção de massa de ar/extinção, diálogos
  de curva de luz e curva de crescimento.
- **Detecção de objetos em movimento**: busca automática de candidatos em 3 ou mais
  imagens, comparação em blink, detecção de traço/streak para objetos rápidos e satélites,
  Track and Stack ao longo de um vetor de movimento, e empilhamento de imagens (Adição /
  Média / Mediana, alinhado por estrelas ou por correlação de fase FFT).
- **Objetos conhecidos e relatórios**: asteroides/cometas conhecidos no campo pelo IMCCE
  SkyBoT (ou offline pelo MPCORB.DAT), efemérides do JPL Horizons, relatório ADES 2022
  XML/PSV com pré-visualização, e envio ao MPC por HTTP ou e-mail.
- **Formatos de arquivo**: FITS (HDU único/multi-HDU, RGB, cubos temporais), vídeo SER,
  XISF (PixInsight), RAW de DSLR (CR2/CR3/NEF/ARW/DNG/… via LibRaw, opcional), NASA
  PDS3/PDS4, TIFF/PNG/BMP/JPEG, e arquivos compactados (ZIP, TAR.*, 7Z, RAR).
- **Visualização de imagem**: zoom/pan/comparação em blink entre quadros, múltiplos modos
  de esticamento/LUT, diagnósticos de histograma e espectro de potência 2-D.
- **Calibração**: pipeline dark/flat com construtor de master frames, assistente de
  calibração, correção de pixels ruins, remoção de fundo por mediana deslizante.
- **Interface**: UI bilíngue (inglês / português brasileiro), tema Noite/Dia/Automático,
  painel de fluxo guiado em seis passos (com modo automático opcional) e recursos para
  escolas (e-mail do professor, configuração da escola compartilhável).

Veja [`docs/technical-reference.md`](docs/technical-reference.md) para a referência
completa de algoritmos e sistemas de coordenadas, e a
[Wiki](https://github.com/petrinhu/astrofind/wiki) para a lista completa de
funcionalidades e guias passo a passo.

---

## Screenshots / Capturas de tela

### 🇬🇧 English

Screenshots are being prepared. Until then, the fastest way to see the interface is to
install it (see [Installation](#installation--instalação)) and follow the
[Quick Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) in the Wiki, which
describes every screen step by step.

### 🇧🇷 Português

As capturas de tela estão sendo preparadas. Até lá, a forma mais rápida de ver a interface
é instalar o programa (veja [Instalação](#installation--instalação)) e seguir o
[Início Rápido](https://github.com/petrinhu/astrofind/wiki/Quick-Start) da Wiki, que
descreve cada tela passo a passo.

---

## Installation / Instalação

### 🇬🇧 English

**Universal installer (recommended)**: auto-detects your distribution, downloads the
correct package, resolves dependencies, and sets up desktop integration. Bilingual
(EN/PT-BR).

```bash
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.2.0/packaging/install.sh
chmod +x install.sh
./install.sh
```

**Direct package install:**

| Distro family | Package |
|---|---|
| Fedora 44 | `.fc44.x86_64.rpm` |
| Rocky Linux / AlmaLinux / RHEL 9 | `.el9.x86_64.rpm` (enable EPEL and CRB first) |
| openSUSE Tumbleweed | `.opensuse-tumbleweed.x86_64.rpm` |
| Ubuntu 24.04 / Mint 22 / Pop!\_OS 24.04 / Zorin OS 18 | `~ubuntu24.04_amd64.deb` |
| Debian 12 / Debian 13 | `~debian12_amd64.deb` / `~debian13_amd64.deb` |
| Arch Linux / Manjaro / CachyOS / EndeavourOS | `.pkg.tar.zst` (or PKGBUILD, built locally with `makepkg`) |
| Any other x86-64 distro (glibc ≥ 2.36) | `.AppImage` (no install needed) |

Distros on an Ubuntu 22.04 base (Pop!\_OS 22.04, Zorin OS 17, Mint 21) are not supported:
their Qt 6.2 is older than the Qt 6.4 AstroFind needs, and their glibc 2.35 is too old for the
AppImage.

Every exact command, per-distro dependency list, and build-from-source instructions are in
**[INSTALL.md](INSTALL.md)**: keep that file as the single source of truth for install
commands (this README does not duplicate it).

To build from source:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

### 🇧🇷 Português

**Instalador universal (recomendado)**: detecta automaticamente sua distribuição, baixa o
pacote correto, resolve dependências e configura a integração com o desktop. Bilíngue
(EN/PT-BR).

```bash
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.2.0/packaging/install.sh
chmod +x install.sh
./install.sh
```

**Instalação direta do pacote:**

| Família de distro | Pacote |
|---|---|
| Fedora 44 | `.fc44.x86_64.rpm` |
| Rocky Linux / AlmaLinux / RHEL 9 | `.el9.x86_64.rpm` (ative EPEL e CRB antes) |
| openSUSE Tumbleweed | `.opensuse-tumbleweed.x86_64.rpm` |
| Ubuntu 24.04 / Mint 22 / Pop!\_OS 24.04 / Zorin OS 18 | `~ubuntu24.04_amd64.deb` |
| Debian 12 / Debian 13 | `~debian12_amd64.deb` / `~debian13_amd64.deb` |
| Arch Linux / Manjaro / CachyOS / EndeavourOS | `.pkg.tar.zst` (ou PKGBUILD, compilado localmente com `makepkg`) |
| Qualquer outra distro x86-64 (glibc ≥ 2.36) | `.AppImage` (não precisa instalar) |

Distros com base Ubuntu 22.04 (Pop!\_OS 22.04, Zorin OS 17, Mint 21) não são suportadas: o
Qt 6.2 delas é mais antigo que o Qt 6.4 exigido pelo AstroFind, e a glibc 2.35 é antiga demais
para o AppImage.

Todos os comandos exatos, dependências por distribuição e instruções de compilação a
partir do código-fonte estão em **[INSTALL.md](INSTALL.md)**: mantenha esse arquivo como
fonte única da verdade para comandos de instalação (este README não os duplica).

Para compilar a partir do código-fonte:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

---

## Quick start / Uso rápido

### 🇬🇧 English

The **Workflow** panel on the left walks you through the same six steps:

1. **File → Import Images…**: open 3 or more frames of the same field (FITS, or any format in
   [Features](#features--recursos); **FITS** = Flexible Image Transport System, the
   standard astronomical image format).
2. **Astrometry Tools → Run Data Reduction…** (`Ctrl+A`): calibration, star detection and plate
   solving (online via astrometry.net, free API key, or offline via ASTAP).
3. **Utilities → Show Known Objects** (`Ctrl+K`): reference stars and the known asteroids in
   the field.
4. **Utilities → Begin Blink Mode** (`Ctrl+B`) or **Astrometry Tools → Detect Moving Objects…**
   (`Ctrl+M`): find what moved.
5. **Edit → Aperture Tool** (`A`) or the **Measure Object** button (`M`): click the object; check
   the Verification dialog and accept.
6. **File → View ADES Report File**: review the ADES report and, once verified, submit it
   to the MPC.

The [Wiki](https://github.com/petrinhu/astrofind/wiki) explains each step for absolute
beginners, with a menu and settings reference.

### 🇧🇷 Português

O painel **Fluxo** à esquerda conduz pelos mesmos seis passos:

1. **Arquivo → Importar Imagens…**: abra 3 ou mais quadros do mesmo campo (FITS, ou qualquer
   formato em [Recursos](#features--recursos); **FITS** = Flexible Image Transport System, o
   formato padrão de imagem astronômica).
2. **Ferramentas de Astrometria → Executar Redução de Dados…** (`Ctrl+A`): calibração, detecção de estrelas e plate
   solving (online via astrometry.net, com chave de API gratuita, ou offline via ASTAP).
3. **Utilitários → Mostrar Objetos Conhecidos** (`Ctrl+K`): estrelas de referência
   e os asteroides conhecidos no campo.
4. **Utilitários → Ativar Modo de Piscagem** (`Ctrl+B`) ou **Ferramentas de Astrometria → Detectar
   Objetos em Movimento…** (`Ctrl+M`): encontre o que se moveu.
5. **Editar → Ferramenta Abertura** (`A`) ou o botão **Medir Objeto** (`M`): clique no objeto; confira
   o diálogo de Verificação e aceite.
6. **Arquivo → Ver Arquivo de Relatório ADES**: revise o relatório ADES e, após verificação,
   envie ao MPC.

A [Wiki](https://github.com/petrinhu/astrofind/wiki) explica cada passo para iniciantes
absolutos, com referência de menus e de configurações.

---

## Technical stack / Stack técnica

### 🇬🇧 English

C++23, Qt 6.4+ (Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml), CMake 3.22+,
cfitsio + CCfits, FFTW3, SEP (Source Extractor), spdlog, nlohmann/json; optional: LibRaw,
libarchive, QuaZip, Qt6Keychain.

### 🇧🇷 Português

C++23, Qt 6.4+ (Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml), CMake 3.22+,
cfitsio + CCfits, FFTW3, SEP (Source Extractor), spdlog, nlohmann/json; opcionais: LibRaw,
libarchive, QuaZip, Qt6Keychain.

---

## Dependencies / Dependências

### 🇬🇧 English

**Required**

| Dependency | Version | License | Notes |
|------------|---------|---------|-------|
| Qt | >= 6.4 | LGPL-3.0 | Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml |
| cfitsio | any | NASA permissive | FITS I/O (system package or FetchContent) |
| FFTW3 | any | GPL-2.0-or-later | FFT for image stacking and power spectrum |
| SEP | >= 1.2 | LGPL-3.0 + BSD-3-Clause + MIT | Source Extractor C library (FetchContent) |
| spdlog | >= 1.11 | MIT | Logging (FetchContent) |
| nlohmann/json | >= 3.11 | MIT | Project file serialisation (bundled) |
| Qt6Keychain | any | LGPL-2.1 | Secure API key storage (dynamically linked, required at runtime) |
| CMake | >= 3.22 | n/a | Build system, not distributed |
| C++ | 23 | n/a | Compiler: GCC 12+ (Debian 12 is the oldest tested) or Clang 16+, not distributed |

**Optional**

| Dependency | License | Notes |
|------------|---------|-------|
| QuaZip + Qt6Core5Compat | LGPL-2.1 + static-linking exception | ZIP archive extraction |
| libarchive | BSD-2/3-Clause | TAR.GZ/BZ2/XZ, 7Z, RAR extraction |
| LibRaw | LGPL-2.1 / CDDL-1.0 | DSLR RAW loading (optional) |
| Qt6LinguistTools | LGPL-3.0 | Compile `.ts` translations |
| ASTAP | proprietary freeware | Offline plate solver ([hnsky.org](https://www.hnsky.org/astap.htm)) |
| CCfits | NASA permissive | Bundled C++ wrapper around cfitsio |
| Catch2 | BSL-1.0 | Test framework only, not linked into the production binary |

Full third-party attribution, copyright holders, and required license notices:
see [`NOTICE`](NOTICE). Exact per-distro package names: see [INSTALL.md](INSTALL.md).

### 🇧🇷 Português

**Obrigatórios**

| Dependência | Versão | Licença | Notas |
|-------------|--------|---------|-------|
| Qt | >= 6.4 | LGPL-3.0 | Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml |
| cfitsio | qualquer | Permissiva NASA | I/O FITS (pacote do sistema ou FetchContent) |
| FFTW3 | qualquer | GPL-2.0-or-later | FFT para empilhamento de imagens e espectro de potência |
| SEP | >= 1.2 | LGPL-3.0 + BSD-3-Clause + MIT | Biblioteca C do Source Extractor (FetchContent) |
| spdlog | >= 1.11 | MIT | Log (FetchContent) |
| nlohmann/json | >= 3.11 | MIT | Serialização de arquivo de projeto (bundled) |
| Qt6Keychain | qualquer | LGPL-2.1 | Armazenamento seguro de chave API (linkado dinamicamente, obrigatório em runtime) |
| CMake | >= 3.22 | n/a | Sistema de build, não distribuído |
| C++ | 23 | n/a | Compilador: GCC 12+ (o Debian 12 é o mais antigo testado) ou Clang 16+, não distribuído |

**Opcionais**

| Dependência | Licença | Notas |
|-------------|---------|-------|
| QuaZip + Qt6Core5Compat | LGPL-2.1 + exceção de linkagem estática | Extração de arquivos ZIP |
| libarchive | BSD-2/3-Clause | Extração TAR.GZ/BZ2/XZ, 7Z, RAR |
| LibRaw | LGPL-2.1 / CDDL-1.0 | Leitura de RAW de DSLR (opcional) |
| Qt6LinguistTools | LGPL-3.0 | Compilar traduções `.ts` |
| ASTAP | freeware proprietário | Plate solver offline ([hnsky.org](https://www.hnsky.org/astap.htm)) |
| CCfits | Permissiva NASA | Wrapper C++ do cfitsio, bundled |
| Catch2 | BSL-1.0 | Apenas framework de testes, não linkado no binário de produção |

Atribuição completa de terceiros, detentores de copyright e avisos de licença exigidos:
ver [`NOTICE`](NOTICE). Nomes exatos de pacotes por distribuição: ver
[INSTALL.md](INSTALL.md).

---

## Testing / Testes

### 🇬🇧 English

```bash
# Core tests (no display needed)
./build/bin/astrofind_tests

# UI tests (need a display, or run headless with QT_QPA_PLATFORM=offscreen)
./build/bin/astrofind_ui_tests
```

The suites have **179 core test cases (38 515 assertions) and 27 UI test cases**, all
passing on the 10 QA distributions. `audit.yml` rebuilds everything with ASan + UBSan
(LeakSanitizer on) and also runs cppcheck, clang-tidy and Valgrind, on Fedora 44,
CachyOS, Arch, Ubuntu 24.04 and Debian 12, for every pull request into `main` and every
release tag. Locally: `cmake --build build --target audit` (reports in `build/audit/`).
See [`AUDIT_FIND.md`](AUDIT_FIND.md) for the audit trail and [`TODO.md`](TODO.md) for the
open items.

### 🇧🇷 Português

```bash
# Testes do núcleo (sem display)
./build/bin/astrofind_tests

# Testes de UI (precisam de display, ou rode sem tela com QT_QPA_PLATFORM=offscreen)
./build/bin/astrofind_ui_tests
```

As suítes têm **179 casos de teste do núcleo (38 515 asserções) e 27 casos de UI**, todos
passando nas 10 distribuições de QA. O `audit.yml` recompila tudo com ASan + UBSan
(LeakSanitizer ligado) e roda também cppcheck, clang-tidy e Valgrind, em Fedora 44,
CachyOS, Arch, Ubuntu 24.04 e Debian 12, em todo pull request para a `main` e em toda tag
de release. Localmente: `cmake --build build --target audit` (relatórios em
`build/audit/`). Veja [`AUDIT_FIND.md`](AUDIT_FIND.md) para o histórico da auditoria e o
[`TODO.md`](TODO.md) para os itens abertos.

---

## Documentation / Documentação

### 🇬🇧 English

| Document | Type | What it's for |
|---|---|---|
| [`INSTALL.md`](INSTALL.md) | How-to | Exact install commands per distribution |
| [`docs/technical-reference.md`](docs/technical-reference.md) | Reference | Algorithms and coordinate-system math (WCS, refraction, PSF, ADES) |
| [`docs/qa-distros.md`](docs/qa-distros.md) | Reference | Multi-distro QA strategy and per-distro results |
| [Wiki](https://github.com/petrinhu/astrofind/wiki) | Tutorial / How-to | Beginner-to-advanced guides: installation, first measurement, every menu and setting, troubleshooting (source in [`docs/wiki/`](docs/wiki/), published with `scripts/sync-wiki.sh`) |
| [`CONTRIBUTING.md`](CONTRIBUTING.md) | How-to | Project layout, code style, how to submit a change |
| [`CHANGELOG.md`](CHANGELOG.md) | Reference | Release notes per version |

### 🇧🇷 Português

| Documento | Tipo | Para que serve |
|---|---|---|
| [`INSTALL.md`](INSTALL.md) | How-to | Comandos exatos de instalação por distribuição |
| [`docs/technical-reference.md`](docs/technical-reference.md) | Reference | Matemática de algoritmos e sistemas de coordenadas (WCS, refração, PSF, ADES) |
| [`docs/qa-distros.md`](docs/qa-distros.md) | Reference | Estratégia de QA multi-distro e resultados por distribuição |
| [Wiki](https://github.com/petrinhu/astrofind/wiki) | Tutorial / How-to | Guias do iniciante ao avançado: instalação, primeira medição, cada menu e configuração, solução de problemas (fonte em [`docs/wiki/`](docs/wiki/), publicada com `scripts/sync-wiki.sh`) |
| [`CONTRIBUTING.md`](CONTRIBUTING.md) | How-to | Estrutura do projeto, estilo de código, como enviar uma mudança |
| [`CHANGELOG.md`](CHANGELOG.md) | Reference | Notas de lançamento por versão |

---

## Contributing / Contribuindo

### 🇬🇧 English

Contributions are welcome. Start with **[CONTRIBUTING.md](CONTRIBUTING.md)** for project
layout, code style (C++23, no `.ui` files, `src/core/` has no Qt UI dependency), and how to
submit a pull request.

### 🇧🇷 Português

Contribuições são bem-vindas. Comece por **[CONTRIBUTING.md](CONTRIBUTING.md)** para a
estrutura do projeto, estilo de código (C++23, sem arquivos `.ui`, `src/core/` sem
dependência de UI do Qt) e como enviar um pull request.

---

## License / Licença

### 🇬🇧 English

[GNU Affero General Public License v3.0](LICENSE): strong copyleft; commercial use is
allowed, but distributing (or running as a network service) requires making the source
code available. Copyright (c) 2024-2026 Petrus Silva Costa.
Third-party attribution: [`NOTICE`](NOTICE).

### 🇧🇷 Português

[GNU Affero General Public License v3.0](LICENSE): copyleft forte; uso comercial é
permitido, mas distribuir (ou rodar como serviço de rede) obriga a disponibilizar o
código-fonte. Copyright (c) 2024-2026 Petrus Silva Costa.
Atribuição de terceiros: [`NOTICE`](NOTICE).

---

## Acknowledgments / Agradecimentos

### 🇬🇧 English

AstroFind was inspired by and learned from these excellent projects:

| Project | Author(s) | What we learned |
|---------|-----------|-----------------|
| [Astrometrica](http://www.astrometrica.at/) | Herbert Raab | Primary UX inspiration: the standard Windows tool for IASC/MPC work |
| [find_orb](https://github.com/Bill-Gray/find_orb) | Bill Gray (Project Pluto) | ADES output format, MPC submission, orbit determination |
| [umbrella2](https://github.com/mostanes/umbrella2) | CSCF | Moving object detection algorithms, tracklet linking |
| [Siril](https://gitlab.com/free-astro/siril) | Free-Astro team | PSF fitting, background extraction, photometry pipeline |
| [astrometry.net](https://github.com/dstndstn/astrometry.net) | Dustin Lang et al. | Plate-solving REST API used directly |
| [IRAF](https://github.com/iraf-community/iraf) | NOAO / community | Aperture photometry concepts and table column conventions |
| [GDL](https://github.com/gnudatalanguage/gdl) | GDL team | PSF elliptical / MPFIT, sliding median, FFT algorithms |
| [NEMO](https://github.com/teuben/nemo) | P. Teuben | Bad pixel masks, blob/streak, ClumpFind, histogram, Laplacian |
| [WCSLIB](https://www.atnf.csiro.au/people/mcalabre/WCS/) | Mark Calabretta, CSIRO/ATNF | WCS projection formulas adapted from `prj.c`; see [`NOTICE`](NOTICE) |
| [Boost.Astronomy](https://github.com/BoostGSoC19/astronomy) | GSoC 2019 | Atmospheric refraction, CIRS/ADES, ecliptic/galactic overlays |

Libraries used: [Qt6](https://github.com/qt/qtbase),
[cfitsio](https://github.com/HEASARC/cfitsio),
[CCfits 2.7](https://heasarc.gsfc.nasa.gov/fitsio/CCfits/),
[FFTW3](https://github.com/FFTW/fftw3),
[SEP](https://github.com/kbarbary/sep),
[spdlog](https://github.com/gabime/spdlog),
[nlohmann/json](https://github.com/nlohmann/json),
[Catch2](https://github.com/catchorg/Catch2),
[QuaZip](https://github.com/stachenov/quazip),
[qt-keychain](https://github.com/frankosterfeld/qtkeychain),
[libarchive](https://www.libarchive.org/),
[LibRaw](https://www.libraw.org/).

See [`NOTICE`](NOTICE) for the license and copyright of every library above and of
WCSLIB, whose projection code was adapted. The other rows of the table are credited for
inspiration only: no code from them is included, and `NOTICE` §9-§10 says so for each one.

### 🇧🇷 Português

O AstroFind foi inspirado e aprendeu com estes excelentes projetos:

| Projeto | Autor(es) | O que aprendemos |
|---------|-----------|------------------|
| [Astrometrica](http://www.astrometrica.at/) | Herbert Raab | Inspiração principal de UX: ferramenta padrão Windows para IASC/MPC |
| [find_orb](https://github.com/Bill-Gray/find_orb) | Bill Gray (Project Pluto) | Formato de saída ADES, submissão MPC, determinação de órbita |
| [umbrella2](https://github.com/mostanes/umbrella2) | CSCF | Algoritmos de detecção de objetos em movimento, ligação de tracklets |
| [Siril](https://gitlab.com/free-astro/siril) | Free-Astro team | Ajuste de PSF, extração de fundo, pipeline de fotometria |
| [astrometry.net](https://github.com/dstndstn/astrometry.net) | Dustin Lang et al. | API REST de plate-solving usada diretamente |
| [IRAF](https://github.com/iraf-community/iraf) | NOAO / comunidade | Conceitos de fotometria de abertura e convenções de colunas de tabela |
| [GDL](https://github.com/gnudatalanguage/gdl) | equipe GDL | PSF elíptica / MPFIT, mediana deslizante, algoritmos FFT |
| [NEMO](https://github.com/teuben/nemo) | P. Teuben | Máscaras de pixels ruins, blob/streak, ClumpFind, histograma, Laplaciano |
| [WCSLIB](https://www.atnf.csiro.au/people/mcalabre/WCS/) | Mark Calabretta, CSIRO/ATNF | Fórmulas de projeção WCS adaptadas de `prj.c`; ver [`NOTICE`](NOTICE) |
| [Boost.Astronomy](https://github.com/BoostGSoC19/astronomy) | GSoC 2019 | Refração atmosférica, CIRS/ADES, sobreposições eclíptica/galáctica |

Bibliotecas utilizadas: [Qt6](https://github.com/qt/qtbase),
[cfitsio](https://github.com/HEASARC/cfitsio),
[CCfits 2.7](https://heasarc.gsfc.nasa.gov/fitsio/CCfits/),
[FFTW3](https://github.com/FFTW/fftw3),
[SEP](https://github.com/kbarbary/sep),
[spdlog](https://github.com/gabime/spdlog),
[nlohmann/json](https://github.com/nlohmann/json),
[Catch2](https://github.com/catchorg/Catch2),
[QuaZip](https://github.com/stachenov/quazip),
[qt-keychain](https://github.com/frankosterfeld/qtkeychain),
[libarchive](https://www.libarchive.org/),
[LibRaw](https://www.libraw.org/).

Ver [`NOTICE`](NOTICE) para a licença e o copyright de cada biblioteca acima e da
WCSLIB, cujo código de projeção foi adaptado. As demais linhas da tabela são crédito de
inspiração: nenhum código delas está incluído, e o `NOTICE` §9-§10 registra isso para cada uma.

---

Repository: [github.com/petrinhu/astrofind](https://github.com/petrinhu/astrofind)

*Last reviewed / última revisão: 2026-09-24. Owner: Petrus Silva Costa
([@petrinhu](https://github.com/petrinhu))*
