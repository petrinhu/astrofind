# AstroFind

> 🇬🇧 Modern astrometry software for asteroid detection and measurement, built for citizen
> science, education, and the IASC / Minor Planet Center (MPC) workflow.
> 🇧🇷 Software moderno de astrometria para detecção e medição de asteroides, feito para
> ciência cidadã, educação e o fluxo de trabalho do IASC / Minor Planet Center (MPC).

<!-- Release / license / CI -->
[![License: AGPL v3](https://img.shields.io/badge/license-AGPL--3.0-blue)](LICENSE)
[![Release](https://img.shields.io/badge/release-v0.9.0-orange)](https://codeberg.org/petrinhu/astrofind/releases/latest)
[![Build](https://github.com/petrinhu/astrofind/actions/workflows/build.yml/badge.svg)](https://github.com/petrinhu/astrofind/actions/workflows/build.yml)
[![Audit](https://github.com/petrinhu/astrofind/actions/workflows/audit.yml/badge.svg)](https://github.com/petrinhu/astrofind/actions/workflows/audit.yml)

<!-- Stack -->
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white)](CMakeLists.txt)
[![Qt](https://img.shields.io/badge/Qt-6.4%2B-41cd52?logo=qt)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-%E2%89%A53.20-064F8C?logo=cmake)](CMakeLists.txt)
[![Platform](https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black)](INSTALL.md)

<!-- Quality -->
[![Tests](https://img.shields.io/badge/tests-5480%20assertions%20passing-brightgreen)](#testing--testes)
[![ASan/UBSan](https://img.shields.io/badge/ASan%2FUBSan-clean-success)](.github/workflows/audit.yml)
[![cppcheck](https://img.shields.io/badge/cppcheck-clean-success)](.github/workflows/audit.yml)
[![clang-tidy](https://img.shields.io/badge/clang--tidy-checked-success)](.github/workflows/audit.yml)

<!-- QA matrix, validated distros -->
[![Ubuntu](https://img.shields.io/badge/Ubuntu-24.04-E95420?logo=ubuntu&logoColor=white)](docs/qa-distros.md)
[![Debian](https://img.shields.io/badge/Debian-12-A81D33?logo=debian&logoColor=white)](docs/qa-distros.md)
[![Fedora](https://img.shields.io/badge/Fedora-latest-51A2DA?logo=fedora&logoColor=white)](docs/qa-distros.md)
[![Arch Linux](https://img.shields.io/badge/Arch%20Linux-rolling-1793D1?logo=archlinux&logoColor=white)](docs/qa-distros.md)
[![Manjaro](https://img.shields.io/badge/Manjaro-rolling-35BF5C?logo=manjaro&logoColor=white)](docs/qa-distros.md)
[![openSUSE](https://img.shields.io/badge/openSUSE-Tumbleweed-73BA25?logo=opensuse&logoColor=white)](docs/qa-distros.md)
[![Rocky Linux](https://img.shields.io/badge/Rocky%20Linux-9-10B981?logo=rockylinux&logoColor=white)](docs/qa-distros.md)
[![Linux Mint](https://img.shields.io/badge/Linux%20Mint-22-87CF3E?logo=linuxmint&logoColor=white)](docs/qa-distros.md)
[![Pop!_OS](https://img.shields.io/badge/Pop!__OS-22.04-48B9C7?logo=popos&logoColor=white)](docs/qa-distros.md)
[![Zorin OS](https://img.shields.io/badge/Zorin%20OS-17-0CC1F3)](docs/qa-distros.md)

<!-- Community -->
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen)](CONTRIBUTING.md)
[![Made for astronomers](https://img.shields.io/badge/made%20for-astronomers-blueviolet)](#what-it-is--o-que-é)
[![i18n](https://img.shields.io/badge/i18n-EN%20%7C%20PT--BR-informational)](i18n/)

<!-- Topics -->
[![astrometry](https://img.shields.io/badge/topic-astrometry-lightgrey)](docs/technical-reference.md)
[![asteroid detection](https://img.shields.io/badge/topic-asteroid--detection-lightgrey)](docs/technical-reference.md)
[![FITS](https://img.shields.io/badge/topic-FITS-lightgrey)](docs/technical-reference.md#1-fits-file-structure)
[![WCS](https://img.shields.io/badge/topic-WCS-lightgrey)](docs/technical-reference.md#3-wcs-pipeline--pixel-to-sky)
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

> **⚠️ Beta software, not yet validated with real observatory FITS files.**
> All features are implemented, the full numerical audit (ASan/UBSan, Valgrind, cppcheck,
> clang-tidy) is clean, and the test suite passes on 11 Linux distributions, but
> end-to-end validation with real telescope imagery has not been performed yet. Verify
> astrometry, photometry, and ADES report results independently before submitting to the
> MPC. Real-world test reports and feedback are very welcome; see
> [Contributing](#contributing--contribuindo).

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

> **⚠️ Software em fase beta, ainda não validado com arquivos FITS reais de observatório.**
> Todas as funcionalidades estão implementadas, a auditoria numérica completa (ASan/UBSan,
> Valgrind, cppcheck, clang-tidy) está limpa, e a suíte de testes passa em 11 distribuições
> Linux, mas a validação ponta a ponta com imagens reais de telescópio ainda não foi
> realizada. Verifique de forma independente os resultados de astrometria, fotometria e
> relatórios ADES antes de qualquer submissão ao MPC. Relatos de testes com dados reais e
> feedback são muito bem-vindos; veja [Contribuindo](#contributing--contribuindo).

> *"Para meu filho Guga, usuário de Linux como eu,*
> *que estava sem uma boa opção para buscar asteroides."*

---

## Features / Recursos

### 🇬🇧 English

- **Star detection and measurement**: automated star detection (SEP / Source Extractor),
  moment centroids, and elliptical **PSF** (Point Spread Function, the blur profile a
  point of light forms on the sensor) fitting via Levenberg-Marquardt, which reveals coma
  and tracking errors from the star's shape.
- **Astrometry / WCS**: full **WCS** (World Coordinate System, the mapping between pixel
  and sky coordinates) pipeline across 8 projections (TAN, SIN, ARC, STG, CAR, MER, GLS,
  AIT), online plate solving via astrometry.net, offline via ASTAP, atmospheric refraction
  correction, and ICRS to CIRS to topocentric conversion for sub-arcsecond report
  residuals.
- **Photometry**: aperture and differential **photometry** (brightness measurement) with
  zero-point calibration, airmass/extinction correction, light-curve and growth-curve
  dialogs.
- **Moving-object detection**: tracklet linking across 3 or more images, streak/trail
  detection for fast movers and satellites, Track and Stack alignment, and image stacking
  (Add / Average / Median / Sigma-clipped with FFT sub-pixel registration).
- **Reporting**: ADES 2022 XML/PSV generation, MPC report preview and direct submission,
  Solar System body identification (IMCCE SkyBoT, JPL Horizons).
- **File formats**: FITS (single/multi-HDU, RGB, temporal cubes), SER video, XISF
  (PixInsight), TIFF/PNG/BMP/JPEG, and compressed archives (ZIP, TAR.*, 7Z, RAR).
- **Image display**: zoom/pan/blink comparison across frames, multiple stretch/LUT modes,
  histogram and 2-D power-spectrum diagnostics.
- **Calibration**: bias/dark/flat pipeline, calibration wizard, bad-pixel masking.
- **Interface**: full bilingual UI (English / Brazilian Portuguese), Day/Night/Auto theme,
  guided school workflow for students.

See [`docs/technical-reference.md`](docs/technical-reference.md) for the full algorithm
and coordinate-system reference, and the [Wiki](https://github.com/petrinhu/astrofind/wiki)
for the complete feature list and how-to guides.

### 🇧🇷 Português

- **Detecção e medição de estrelas**: detecção automática de estrelas (SEP / Source
  Extractor), centroides por momentos, e ajuste de **PSF** elíptica (Point Spread
  Function, o perfil de borrão que um ponto de luz forma no sensor) via
  Levenberg-Marquardt, que revela coma e erros de rastreamento pela forma da estrela.
- **Astrometria / WCS**: pipeline **WCS** (World Coordinate System, o mapeamento entre
  coordenadas de pixel e coordenadas celestes) completo em 8 projeções (TAN, SIN, ARC, STG,
  CAR, MER, GLS, AIT), plate solving online via astrometry.net, offline via ASTAP, correção
  de refração atmosférica, e conversão ICRS para CIRS para topocêntrico para resíduos de
  relatório sub-arcseg.
- **Fotometria**: **fotometria** de abertura e diferencial (medição de brilho) com
  calibração de zero-point, correção de massa de ar/extinção, diálogos de curva de luz e
  curva de crescimento.
- **Detecção de objetos em movimento**: ligação de tracklets em 3 ou mais imagens, detecção
  de traço/streak para objetos rápidos e satélites, alinhamento Track and Stack, e
  empilhamento de imagens (Adição / Média / Mediana / Sigma-clipped com registro sub-pixel
  por FFT).
- **Relatórios**: geração ADES 2022 XML/PSV, pré-visualização e submissão direta ao MPC,
  identificação de corpos do Sistema Solar (IMCCE SkyBoT, JPL Horizons).
- **Formatos de arquivo**: FITS (HDU único/multi-HDU, RGB, cubos temporais), vídeo SER,
  XISF (PixInsight), TIFF/PNG/BMP/JPEG, e arquivos compactados (ZIP, TAR.*, 7Z, RAR).
- **Visualização de imagem**: zoom/pan/comparação em blink entre quadros, múltiplos modos
  de esticamento/LUT, diagnósticos de histograma e espectro de potência 2-D.
- **Calibração**: pipeline bias/dark/flat, assistente de calibração, máscara de pixels
  ruins.
- **Interface**: UI bilíngue completa (inglês / português brasileiro), tema Dia/Noite/
  Automático, fluxo guiado para escolas.

Veja [`docs/technical-reference.md`](docs/technical-reference.md) para a referência
completa de algoritmos e sistemas de coordenadas, e a
[Wiki](https://github.com/petrinhu/astrofind/wiki) para a lista completa de
funcionalidades e guias passo a passo.

---

## Screenshots / Capturas de tela

### 🇬🇧 English

Screenshots are being prepared for the [Wiki](https://github.com/petrinhu/astrofind/wiki).
Until then, the fastest way to see the interface is to build it yourself (5 minutes, see
[Installation](#installation--instalação)) or check the guided walkthrough once the Wiki is
published.

### 🇧🇷 Português

As capturas de tela estão sendo preparadas para a
[Wiki](https://github.com/petrinhu/astrofind/wiki). Até lá, a forma mais rápida de ver a
interface é compilando você mesmo (5 minutos, veja [Instalação](#installation--instalação))
ou conferindo o passo a passo guiado assim que a Wiki for publicada.

---

## Installation / Instalação

### 🇬🇧 English

**Universal installer (recommended)**: auto-detects your distribution, downloads the
correct package, resolves dependencies, and sets up desktop integration. Bilingual
(EN/PT-BR).

```bash
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/install.sh
chmod +x install.sh
./install.sh
```

**Direct package install:**

| Distro family | Package |
|---|---|
| Fedora / RHEL / Rocky Linux | `.rpm` |
| Ubuntu / Debian / Mint / Pop!\_OS / Zorin | `.deb` |
| Arch Linux / Manjaro | PKGBUILD tarball |

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
curl -LO https://codeberg.org/petrinhu/astrofind/releases/download/v0.9.0-beta/install.sh
chmod +x install.sh
./install.sh
```

**Instalação direta do pacote:**

| Família de distro | Pacote |
|---|---|
| Fedora / RHEL / Rocky Linux | `.rpm` |
| Ubuntu / Debian / Mint / Pop!\_OS / Zorin | `.deb` |
| Arch Linux / Manjaro | tarball PKGBUILD |

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

1. Open a FITS file (**FITS** means Flexible Image Transport System, the standard
   astronomical image format) or a folder of images.
2. Run star detection and plate solving (online via astrometry.net or offline via ASTAP).
3. Let AstroFind flag moving-object candidates across 3 or more frames.
4. Click a candidate to measure its centroid and photometry.
5. Export the ADES report and, once verified, submit it to the MPC.

A full guided walkthrough with screenshots lives in the
**[Wiki](https://github.com/petrinhu/astrofind/wiki)**.

### 🇧🇷 Português

1. Abra um arquivo FITS (**FITS** significa Flexible Image Transport System, o formato
   padrão de imagem astronômica) ou uma pasta de imagens.
2. Rode a detecção de estrelas e o plate solving (online via astrometry.net ou offline via
   ASTAP).
3. Deixe o AstroFind sinalizar candidatos a objeto em movimento em 3 ou mais quadros.
4. Clique num candidato para medir seu centroide e fotometria.
5. Exporte o relatório ADES e, após verificação, envie ao MPC.

Um passo a passo guiado com capturas de tela vive na
**[Wiki](https://github.com/petrinhu/astrofind/wiki)**.

---

## Technical stack / Stack técnica

### 🇬🇧 English

C++23, Qt 6.4+ (Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml), CMake 3.20+,
cfitsio, FFTW3, SEP (Source Extractor), spdlog, nlohmann/json.

### 🇧🇷 Português

C++23, Qt 6.4+ (Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml), CMake 3.20+,
cfitsio, FFTW3, SEP (Source Extractor), spdlog, nlohmann/json.

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
| CMake | >= 3.20 | n/a | Build system, not distributed |
| C++ | 23 | n/a | Compiler: GCC 13+ or Clang 16+, not distributed |

**Optional**

| Dependency | License | Notes |
|------------|---------|-------|
| QuaZip + Qt6Core5Compat | LGPL-2.1 + static-linking exception | ZIP archive extraction |
| libarchive | BSD-2/3-Clause | TAR.GZ/BZ2/XZ, 7Z, RAR extraction |
| Qt6Keychain | LGPL-2.1 | Secure API key storage |
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
| CMake | >= 3.20 | n/a | Sistema de build, não distribuído |
| C++ | 23 | n/a | Compilador: GCC 13+ ou Clang 16+, não distribuído |

**Opcionais**

| Dependência | Licença | Notas |
|-------------|---------|-------|
| QuaZip + Qt6Core5Compat | LGPL-2.1 + exceção de linkagem estática | Extração de arquivos ZIP |
| libarchive | BSD-2/3-Clause | Extração TAR.GZ/BZ2/XZ, 7Z, RAR |
| Qt6Keychain | LGPL-2.1 | Armazenamento seguro de chave API |
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

# UI tests (requires a display)
./build/bin/astrofind_ui_tests
```

The numerical audit build (ASan + UBSan) passes **5480 assertions across 159 test cases**,
`detect_leaks=1` included. `audit.yml` runs ASan/UBSan, cppcheck, clang-tidy, and Valgrind
on every pull request into `main` and on every release tag. See
[`AUDIT_FIND.md`](AUDIT_FIND.md) for the full audit trail.

### 🇧🇷 Português

```bash
# Testes do núcleo (sem display)
./build/bin/astrofind_tests

# Testes de UI (requer display)
./build/bin/astrofind_ui_tests
```

A build de auditoria numérica (ASan + UBSan) passa em **5480 assertions em 159 casos de
teste**, incluindo `detect_leaks=1`. O `audit.yml` roda ASan/UBSan, cppcheck, clang-tidy e
Valgrind em todo pull request para a `main` e em toda tag de release. Veja
[`AUDIT_FIND.md`](AUDIT_FIND.md) para o histórico completo de auditoria.

---

## Documentation / Documentação

### 🇬🇧 English

| Document | Type | What it's for |
|---|---|---|
| [`INSTALL.md`](INSTALL.md) | How-to | Exact install commands per distribution |
| [`docs/technical-reference.md`](docs/technical-reference.md) | Reference | Algorithms and coordinate-system math (WCS, refraction, PSF, ADES) |
| [`docs/qa-distros.md`](docs/qa-distros.md) | Reference | Multi-distro QA strategy and per-distro results |
| [Wiki](https://github.com/petrinhu/astrofind/wiki) | Tutorial / How-to | Guided walkthroughs, screenshots, day-to-day usage |
| [`CONTRIBUTING.md`](CONTRIBUTING.md) | How-to | Project layout, code style, how to submit a change |
| [`CHANGELOG.md`](CHANGELOG.md) | Reference | Release notes per version |

### 🇧🇷 Português

| Documento | Tipo | Para que serve |
|---|---|---|
| [`INSTALL.md`](INSTALL.md) | How-to | Comandos exatos de instalação por distribuição |
| [`docs/technical-reference.md`](docs/technical-reference.md) | Reference | Matemática de algoritmos e sistemas de coordenadas (WCS, refração, PSF, ADES) |
| [`docs/qa-distros.md`](docs/qa-distros.md) | Reference | Estratégia de QA multi-distro e resultados por distribuição |
| [Wiki](https://github.com/petrinhu/astrofind/wiki) | Tutorial / How-to | Passo a passo guiado, capturas de tela, uso do dia a dia |
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
[libarchive](https://www.libarchive.org/).

See [`NOTICE`](NOTICE) for the license and copyright of every item above.

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
[libarchive](https://www.libarchive.org/).

Ver [`NOTICE`](NOTICE) para a licença e o copyright de cada item acima.

---

Repository: [github.com/petrinhu/astrofind](https://github.com/petrinhu/astrofind);
mirror: [codeberg.org/petrinhu/astrofind](https://codeberg.org/petrinhu/astrofind)

*Last reviewed / última revisão: 2026-07-10. Owner: Petrus Silva Costa
([@petrinhu](https://github.com/petrinhu))*
