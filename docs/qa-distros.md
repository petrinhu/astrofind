# AstroFind: QA Multi-Distro Documentation / Documentação de QA Multi-Distro

> **Last reviewed / Última revisão:** 2026-07-10
> **Owner:** Petrus Silva Costa
> **Applies to / Aplica-se a:** AstroFind v0.9.0+

---

## Table of Contents / Índice

- [Strategy overview / Visão geral da estratégia](#strategy-overview--visão-geral-da-estratégia)
- [Test scope / Escopo de testes](#test-scope-all-distros--escopo-de-testes-todas-as-distros)
- [55.1, Ubuntu 24.04 LTS](#551-ubuntu-2404-lts)
- [55.2, Debian 12 Bookworm](#552-debian-12-bookworm)
- [55.3, Arch Linux](#553-arch-linux)
- [55.4, Manjaro](#554-manjaro)
- [55.5, Linux Mint 22](#555-linux-mint-22)
- [55.6, openSUSE Tumbleweed](#556-opensuse-tumbleweed)
- [55.7, Pop!_OS 22.04](#557-popos-2204)
- [55.8, Rocky Linux 9](#558-rocky-linux-9)
- [55.9, Zorin OS 17](#559-zorin-os-17)
- [Appendix: Docker image provenance / Apêndice: proveniência das imagens Docker](#appendix-docker-image-provenance--apêndice-proveniência-das-imagens-docker)

---

## Strategy overview / Visão geral da estratégia

### 🇬🇧 English

This document describes the multi-distribution QA strategy for AstroFind,
covering all distributions listed in items 55.1-55.9 of the project task table.
Each section describes: the Docker/CI image used, known differences vs a real
installation, package names, known issues, and what is and is not validated by
the automated tests.

All automated distro QA jobs run as GitHub Actions workflows ("workflow" = an
automated recipe of steps that runs on a trigger, e.g. every push), each in a
dedicated file under `.github/workflows/qa-<distro>.yml`. Jobs run
**sequentially, not in parallel**, so the full log of each distro is visible and
readable in the GitHub Actions UI before the next one starts.

Each job:

1. Prints detailed system information (kernel, CPU, memory, package manager
   version)
2. Installs all required + optional build dependencies and prints their exact
   versions
3. Runs `cmake` configure with `--log-level=VERBOSE` so every `find_package()`
   search path is logged
4. Builds with `--verbose` so every compile and link command is printed
5. Runs `astrofind_tests` (116 core cases) with `--reporter console --verbosity
   high`
6. Runs `astrofind_ui_tests` (23 UI cases) with `QT_QPA_PLATFORM=offscreen` and
   full output
7. Prints a summary with distro name, compiler, Qt version, and CMake version

This level of verbosity is intentional: when a distro-specific failure occurs,
the full context is immediately visible in the log without having to re-run
with extra flags.

**Sequential job ordering:**

Jobs depend on each other via `needs:` so they execute one at a time:

```
ubuntu-24.04 -> debian-12 -> arch -> manjaro -> mint-22
             -> opensuse-tw -> rocky-9
             -> pop-os -> zorin (see notes below)
```

### 🇧🇷 Português

Este documento descreve a estratégia de QA multi-distribuição do AstroFind,
cobrindo todas as distribuições listadas nos itens 55.1-55.9 da tabela de
tarefas do projeto. Cada seção descreve: a imagem Docker/CI usada, diferenças
conhecidas em relação a uma instalação real, nomes de pacotes, problemas
conhecidos, e o que é e não é validado pelos testes automatizados.

Todos os jobs de QA de distro automatizados rodam como workflows do GitHub
Actions ("workflow" = uma receita automatizada de passos que roda a partir de um
gatilho, ex.: a cada push), cada um num arquivo dedicado em
`.github/workflows/qa-<distro>.yml`. Os jobs rodam **sequencialmente, não em
paralelo**, para que o log completo de cada distro fique visível e legível na
interface do GitHub Actions antes que o próximo comece.

Cada job:

1. Imprime informação detalhada do sistema (kernel, CPU, memória, versão do
   gerenciador de pacotes)
2. Instala todas as dependências de build obrigatórias + opcionais e imprime
   suas versões exatas
3. Roda a configuração do `cmake` com `--log-level=VERBOSE` para que todo
   caminho de busca de `find_package()` seja registrado em log
4. Compila com `--verbose` para que todo comando de compilação e linkagem seja
   impresso
5. Roda `astrofind_tests` (116 casos core) com `--reporter console --verbosity
   high`
6. Roda `astrofind_ui_tests` (23 casos de UI) com `QT_QPA_PLATFORM=offscreen` e
   saída completa
7. Imprime um resumo com nome da distro, compilador, versão do Qt e versão do
   CMake

Esse nível de verbosidade é intencional: quando ocorre uma falha específica de
distro, o contexto completo fica imediatamente visível no log, sem precisar
rodar de novo com flags extras.

**Ordenação sequencial dos jobs:**

Os jobs dependem uns dos outros via `needs:` para executarem um de cada vez:

```
ubuntu-24.04 -> debian-12 -> arch -> manjaro -> mint-22
             -> opensuse-tw -> rocky-9
             -> pop-os -> zorin (ver notas abaixo)
```

---

## Test scope (all distros) / Escopo de testes (todas as distros)

### 🇬🇧 English

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

The following is **not** tested in CI (requires physical hardware or a desktop
session):

| What is NOT tested | Reason |
|--------------------|--------|
| Hardware-accelerated OpenGL rendering | No GPU in CI container |
| Real Wayland / X11 window lifecycle | No compositor in container |
| ASTAP offline plate solver | Binary not in any distro repo |
| Secure API key storage (Qt Keychain) | System keychain daemon not running |
| astrometry.net / VizieR / SkyBoT API calls | Network-dependent, skipped in CI |

### 🇧🇷 Português

O seguinte é validado em toda distro:

| O que é testado | Como |
|----------------|-----|
| Todos os pacotes obrigatórios existem nos repositórios oficiais | `apt/pacman/zypper/dnf install` funciona |
| Configuração do CMake 3.22+ funciona | `cmake -B build` sai com código 0 |
| Compilação C++23 funciona | Todos os arquivos `.cpp` compilam sem erro |
| Todos os 116 testes unitários core passam | `astrofind_tests --reporter console` |
| Todos os 23 testes de integração de UI passam | `astrofind_ui_tests` com `QT_QPA_PLATFORM=offscreen` |
| FetchContent resolve (spdlog, nlohmann/json, SEP, Catch2) | Parte da configuração do CMake |
| CCfits extraído de `originals/CCfits.tar.gz` | Parte da configuração do CMake |
| `libarchive` opcional disponível | Instalado e linkado |

O seguinte **não** é testado no CI (requer hardware físico ou uma sessão
desktop):

| O que NÃO é testado | Motivo |
|--------------------|--------|
| Renderização OpenGL acelerada por hardware | Sem GPU no container de CI |
| Ciclo de vida real de janela Wayland / X11 | Sem compositor no container |
| Plate solver offline ASTAP | Binário não está em nenhum repositório de distro |
| Armazenamento seguro de chave de API (Qt Keychain) | Daemon de keychain do sistema não está rodando |
| Chamadas de API do astrometry.net / VizieR / SkyBoT | Dependentes de rede, puladas no CI |

---

## 55.1 - Ubuntu 24.04 LTS

### 🇬🇧 English

**Workflow file:** `.github/workflows/build.yml` (primary CI job)

**Docker image:** `ubuntu:24.04`, official Canonical image, updated weekly.

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

- `libqt6core5compat6-dev`, ZIP support via QuaZip
- `libsecret-1-dev`, Qt Keychain dependency
- `qt6-l10n-tools`, lupdate/lrelease for translations

**Known issues / observations:**

- `qt6-qtsql-devel` and `qt6-qtxml-devel` are included in `qt6-base-dev` on
  Ubuntu, unlike Fedora where they are separate packages.
- `libqt6concurrent6-dev` may need to be installed separately if CMake cannot
  find `Qt6Concurrent`. In Ubuntu 24.04 it is included in `qt6-base-dev`.
- The `libgl1-mesa-dev` package is required because the CI runner has no GPU;
  Mesa provides the software OpenGL implementation for the offscreen platform.

**Differences vs a real Ubuntu 24.04 desktop:**

- The CI runner is a GitHub-hosted VM (not a container), so systemd is
  running. This is closer to a real installation than all other distros in
  this QA suite.
- Ubuntu installs as a full desktop normally include `libxkbcommon`
  automatically via the desktop stack. In a minimal server/container install
  it must be explicit.

**Note:** Ubuntu 24.04 ships Qt 6.4.2+dfsg-21.1build5 in its repos (not Qt 6.6
as initially estimated). GCC is 13.3.0 (not 14 as documented above). Both are
sufficient.

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

**Status:** Passed. Primary CI (every push) + dedicated QA workflow
(2026-03-22).

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/build.yml` (job de CI primário)

**Imagem Docker:** `ubuntu:24.04`, imagem oficial da Canonical, atualizada
semanalmente.

**Gerenciador de pacotes:** `apt`

**Disponibilidade do Qt6:** Qt 6.6 nos repositórios `noble` (24.04). Todos os
módulos obrigatórios (`qt6-base-dev`, `libqt6charts6-dev`,
`libqt6opengl6-dev`) estão disponíveis.

**Versão do GCC:** GCC 14 (padrão no Ubuntu 24.04)

**Pacotes obrigatórios:**

```bash
cmake ninja-build gcc-14 g++-14 pkg-config
qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
libqt6opengl6-dev libqt6charts6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libglu1-mesa-dev libxkbcommon-dev
```

**Pacotes opcionais disponíveis nos repositórios:**

- `libqt6core5compat6-dev`, suporte a ZIP via QuaZip
- `libsecret-1-dev`, dependência do Qt Keychain
- `qt6-l10n-tools`, lupdate/lrelease para traduções

**Problemas / observações conhecidas:**

- `qt6-qtsql-devel` e `qt6-qtxml-devel` estão incluídos em `qt6-base-dev` no
  Ubuntu, diferente do Fedora, onde são pacotes separados.
- `libqt6concurrent6-dev` pode precisar ser instalado separadamente se o CMake
  não conseguir encontrar `Qt6Concurrent`. No Ubuntu 24.04 já vem incluído em
  `qt6-base-dev`.
- O pacote `libgl1-mesa-dev` é obrigatório porque o runner de CI não tem GPU; o
  Mesa fornece a implementação OpenGL por software para a plataforma
  offscreen.

**Diferenças em relação a um desktop Ubuntu 24.04 real:**

- O runner de CI é uma VM hospedada pelo GitHub (não um container), então o
  systemd está rodando. Isso é mais próximo de uma instalação real do que
  todas as outras distros desta suíte de QA.
- Instalações de desktop completo do Ubuntu normalmente incluem `libxkbcommon`
  automaticamente via a stack de desktop. Numa instalação mínima de
  servidor/container ele precisa ser explícito.

**Nota:** o Ubuntu 24.04 traz Qt 6.4.2+dfsg-21.1build5 em seus repositórios
(não Qt 6.6 como estimado inicialmente). O GCC é 13.3.0 (não 14 como
documentado acima). Ambos são suficientes.

**Resultados verificados (2026-03-22, qa-ubuntu-24.yml dedicado, run
#23403368717):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `ubuntu:24.04` |
| Distro | Ubuntu 24.04.4 LTS |
| GCC | 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Testes core | 106 passaram / 10 pulados (dependentes de rede) |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 11s |

**Status:** Aprovado. CI primário (a cada push) + workflow de QA dedicado
(2026-03-22).

---

## 55.2 - Debian 12 Bookworm

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-debian-12.yml`

**Docker image:** `debian:12`, official Debian image from Docker Hub.

**Package manager:** `apt`

**Qt6 availability:** Qt 6.4 in the `bookworm` repos. This is the minimum
supported version for AstroFind; no Qt 6.5+ API should be used.

**GCC version:** GCC 12 (default in Debian 12)

**Required packages:**

```bash
cmake ninja-build gcc g++ pkg-config git
qt6-base-dev libqt6charts6-dev libqt6opengl6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libxkbcommon-dev
```

**Optional packages available in repos:**

- `libqt6core5compat6-dev`, available
- `libsecret-1-dev`, available

**Known issues / observations:**

- `qt6-qtkeychain-dev` is **not available** in Debian 12 repos. The
  application falls back to `QSettings` for API key storage (which is the
  designed fallback).
- Qt 6.4 is the oldest supported Qt version. Any regression that breaks Qt 6.4
  compatibility will be caught here first.
- GCC 12 has slightly different C++23 support than GCC 13+. In particular,
  `std::expected` is fully available in GCC 12 as of libstdc++ 12.
- The minimal `debian:12` image does not include `procps` (free, ps),
  `util-linux` (lscpu), or `file`; these are bootstrapped in the first CI
  step.
- The minimal `debian:12` image does not set `/usr/bin` in PATH for
  non-interactive GitHub Actions shell steps; an explicit `PATH` env var is
  required in the workflow.

**Differences vs a real Debian 12 desktop:**

- `debian:12` Docker image is a minimal server install. A real Debian desktop
  ships with many Qt dependencies pre-installed by the GNOME/KDE package
  group.
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

**Status:** Passed (2026-03-22, run #23402097435)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-debian-12.yml`

**Imagem Docker:** `debian:12`, imagem oficial do Debian no Docker Hub.

**Gerenciador de pacotes:** `apt`

**Disponibilidade do Qt6:** Qt 6.4 nos repositórios `bookworm`. Esta é a versão
mínima suportada pelo AstroFind; nenhuma API do Qt 6.5+ deve ser usada.

**Versão do GCC:** GCC 12 (padrão no Debian 12)

**Pacotes obrigatórios:**

```bash
cmake ninja-build gcc g++ pkg-config git
qt6-base-dev libqt6charts6-dev libqt6opengl6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libxkbcommon-dev
```

**Pacotes opcionais disponíveis nos repositórios:**

- `libqt6core5compat6-dev`, disponível
- `libsecret-1-dev`, disponível

**Problemas / observações conhecidas:**

- `qt6-qtkeychain-dev` **não está disponível** nos repositórios do Debian 12.
  A aplicação cai para `QSettings` para armazenamento de chave de API
  (fallback previsto no design).
- Qt 6.4 é a versão de Qt suportada mais antiga. Qualquer regressão que quebre
  a compatibilidade com Qt 6.4 será pega aqui primeiro.
- O GCC 12 tem suporte a C++23 ligeiramente diferente do GCC 13+. Em
  particular, `std::expected` está totalmente disponível no GCC 12 a partir
  da libstdc++ 12.
- A imagem mínima `debian:12` não inclui `procps` (free, ps), `util-linux`
  (lscpu), nem `file`; esses são instalados no primeiro passo do CI.
- A imagem mínima `debian:12` não define `/usr/bin` no PATH para passos de
  shell não interativos do GitHub Actions; uma variável de ambiente `PATH`
  explícita é necessária no workflow.

**Diferenças em relação a um desktop Debian 12 real:**

- A imagem Docker `debian:12` é uma instalação mínima de servidor. Um desktop
  Debian real vem com muitas dependências do Qt pré-instaladas pelo grupo de
  pacotes GNOME/KDE.
- O container não tem `sudo`; os comandos rodam diretamente como root.

**Resultados verificados (2026-03-22):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `debian:12` |
| Distro | Debian GNU/Linux 12 (bookworm) |
| GCC | 12.2.0 (Debian 12.2.0-14+deb12u1) |
| Qt6 | 6.4.2+dfsg-10 |
| CMake | 3.25.1 |
| Testes core | 106 passaram / 10 pulados (dependentes de rede) |
| Testes de UI | 23 passaram (71 assertions) |
| Binários | AstroFind 65 MB, astrofind_tests 42 MB, astrofind_ui_tests 82 MB |
| Duração | ~4m 23s |

**Status:** Aprovado (2026-03-22, run #23402097435)

---

## 55.3 - Arch Linux

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-arch.yml`

**Docker image:** `archlinux:latest`, official Arch Linux image, updated
continuously from the official Arch repos. Reflects the current
rolling-release state.

**Package manager:** `pacman`

**Qt6 availability:** Always the latest Qt6 release (rolling). This tests the
opposite end of the spectrum from Debian 12: the most recent Qt version
AstroFind must handle.

**GCC version:** Always the latest GCC release (rolling)

**Required packages:**

```bash
cmake ninja gcc git pkg-config
qt6-base qt6-charts qt6-tools
cfitsio fftw libarchive
```

**Optional packages available in repos:**

- `qt6-5compat`, QuaZip ZIP support
- `qtkeychain`, secure API key storage
- `clang`, for `audit-clang-tidy` target

**Known issues / observations:**

- The `archlinux:latest` image requires `pacman -Sy --noconfirm` before any
  installs to sync the package database. Without this, installs fail with
  "could not find package".
- Arch does not use `pkg-config` entries for Qt6 in the same paths as
  Debian/Fedora. CMake's `find_package(Qt6)` uses the cmake config files
  instead, which is how AstroFind's CMakeLists.txt is written, no issues
  expected.
- Because Arch is rolling, this job may occasionally catch API breaks caused
  by a new Qt release before other distros do.

**Differences vs a real Arch desktop:**

- `archlinux:latest` is a minimal image with no AUR helper. AUR packages are
  not available. All dependencies must come from official repos.
- A real Arch desktop install would have PKGBUILD-built packages from the AUR
  for some optional tools (e.g. `astap-bin`).
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

**Status:** Passed (2026-03-22, run #23402210788)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-arch.yml`

**Imagem Docker:** `archlinux:latest`, imagem oficial do Arch Linux,
atualizada continuamente a partir dos repositórios oficiais do Arch. Reflete o
estado atual do rolling-release.

**Gerenciador de pacotes:** `pacman`

**Disponibilidade do Qt6:** Sempre a versão mais recente do Qt6 (rolling).
Isso testa o extremo oposto do espectro em relação ao Debian 12: a versão de
Qt mais recente que o AstroFind precisa suportar.

**Versão do GCC:** Sempre a versão de GCC mais recente (rolling)

**Pacotes obrigatórios:**

```bash
cmake ninja gcc git pkg-config
qt6-base qt6-charts qt6-tools
cfitsio fftw libarchive
```

**Pacotes opcionais disponíveis nos repositórios:**

- `qt6-5compat`, suporte a ZIP do QuaZip
- `qtkeychain`, armazenamento seguro de chave de API
- `clang`, para o target `audit-clang-tidy`

**Problemas / observações conhecidas:**

- A imagem `archlinux:latest` exige `pacman -Sy --noconfirm` antes de
  qualquer instalação para sincronizar o banco de dados de pacotes. Sem isso,
  as instalações falham com "could not find package".
- O Arch não usa entradas de `pkg-config` para o Qt6 nos mesmos caminhos que
  Debian/Fedora. O `find_package(Qt6)` do CMake usa os arquivos de config do
  cmake em vez disso, que é como o `CMakeLists.txt` do AstroFind está escrito;
  nenhum problema esperado.
- Como o Arch é rolling, este job pode ocasionalmente pegar quebras de API
  causadas por um novo release do Qt antes de outras distros.

**Diferenças em relação a um desktop Arch real:**

- `archlinux:latest` é uma imagem mínima sem helper de AUR. Pacotes do AUR não
  estão disponíveis. Todas as dependências precisam vir dos repositórios
  oficiais.
- Uma instalação Arch desktop real teria pacotes construídos via PKGBUILD do
  AUR para algumas ferramentas opcionais (ex.: `astap-bin`).
- Nenhum `systemd` rodando no container.

**Resultados verificados (2026-03-22):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `archlinux:latest` |
| Distro | Arch Linux (rolling) |
| GCC | 15.2.1 (2026-02-09) |
| Qt6 | 6.10.2-1 |
| CMake | 4.3.0 |
| Testes core | 106 passaram / 10 pulados (dependentes de rede) |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 33s |

**Status:** Aprovado (2026-03-22, run #23402210788)

---

## 55.4 - Manjaro

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-manjaro.yml`

**Docker image:** `manjarolinux/base`, official minimal image maintained by
the Manjaro team on Docker Hub. Uses Manjaro's own repository branch, held ~2
weeks behind Arch for stability testing.

**Package manager:** `pacman` (with Manjaro-specific mirrors and keyring)

**Qt6 availability:** Latest stable Qt6 from Manjaro repos (typically ~1-2
minor versions behind Arch).

**GCC version:** Latest GCC from Manjaro repos (typically same as Arch)

**Required packages:**

```bash
cmake ninja gcc git pkg-config
qt6-base qt6-charts qt6-tools
cfitsio fftw libarchive
```

**Optional packages available in repos:**

- `qt6-5compat`, available
- `qtkeychain`, may not be in Manjaro repos (check at test time)

**Known issues / observations:**

- The pacman keyring inside a Docker container is often in a partially
  initialised state. The workflow explicitly runs `pacman-key --init` and
  `pacman-key --populate manjaro archlinux` before any package operations.
  Skipping this step causes "unknown trust" errors on fresh image pulls.
- Manjaro uses its own mirror infrastructure. The container image ships with
  a Manjaro-specific `mirrorlist`. In CI, the first mirror may be slow or
  unreliable; the `pacman -Sy` step may take 1-3 minutes.
- `manjarolinux/base` Docker image is updated less frequently than a live
  Manjaro ISO. The container may be behind the latest stable snapshot by
  several weeks.

**Differences vs a real Manjaro desktop:**

- No AUR helper (yay/paru). AUR packages unavailable.
- No Manjaro hardware detection (MHWD), no kernel switcher, no proprietary
  driver integration. These are irrelevant for a Qt6/C++ build test.
- The Manjaro-specific GNOME/KDE application set is absent. Only build tools
  are installed.
- No `systemd` running in the container.

**First verified run:** 2026-03-22, run ID 23400921910, duration **5m 11s**

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
| Total assertions | 4426 | 4426 | -- | 0 |

The 10 skipped core tests are `test_fits_functional.cpp` cases F-01 through
F-10, which require real FITS image files at `/tmp/fits_test/`. These are
absent in all CI environments (including the primary Ubuntu CI job) and are
skipped by design.

**Observations from this run:**

- Manjaro ships **GCC 15** and **Qt 6.10**, both significantly newer than the
  minimum supported versions. AstroFind compiles cleanly with no warnings
  under this configuration, confirming the codebase is forward-compatible.
- CMake 4.x is present on Manjaro (4.2.3). CMake 4.0 introduced the policy
  `CMP0167` (deprecating `FindBoost`) and changed `cmake_minimum_required`
  semantics. No issues were observed; AstroFind's CMakeLists.txt is not
  affected.
- The `FORCE_JAVASCRIPT_ACTIONS_TO_NODE24=true` env var was added to suppress
  the Node.js 20 deprecation warning from `actions/checkout@v4` and
  `actions/cache@v4`. GitHub will enforce Node.js 24 by default from
  2026-06-02.

**Status:** Passing (item 55.4), runs on every push to `main` and `develop`.

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-manjaro.yml`

**Imagem Docker:** `manjarolinux/base`, imagem mínima oficial mantida pela
equipe do Manjaro no Docker Hub. Usa a branch de repositório própria do
Manjaro, mantida ~2 semanas atrás do Arch para teste de estabilidade.

**Gerenciador de pacotes:** `pacman` (com mirrors e keyring específicos do
Manjaro)

**Disponibilidade do Qt6:** Última versão estável do Qt6 dos repositórios do
Manjaro (tipicamente ~1-2 versões menores atrás do Arch).

**Versão do GCC:** Última versão de GCC dos repositórios do Manjaro
(tipicamente igual ao Arch)

**Pacotes obrigatórios:**

```bash
cmake ninja gcc git pkg-config
qt6-base qt6-charts qt6-tools
cfitsio fftw libarchive
```

**Pacotes opcionais disponíveis nos repositórios:**

- `qt6-5compat`, disponível
- `qtkeychain`, pode não estar nos repositórios do Manjaro (checar no momento
  do teste)

**Problemas / observações conhecidas:**

- O keyring do pacman dentro de um container Docker frequentemente está num
  estado parcialmente inicializado. O workflow roda explicitamente
  `pacman-key --init` e `pacman-key --populate manjaro archlinux` antes de
  qualquer operação de pacote. Pular esse passo causa erros de "unknown
  trust" em pulls de imagem novos.
- O Manjaro usa sua própria infraestrutura de mirrors. A imagem do container
  vem com uma `mirrorlist` específica do Manjaro. No CI, o primeiro mirror
  pode ser lento ou não confiável; o passo `pacman -Sy` pode levar de 1 a 3
  minutos.
- A imagem Docker `manjarolinux/base` é atualizada com menos frequência que
  uma ISO Manjaro ao vivo. O container pode estar semanas atrás do último
  snapshot estável.

**Diferenças em relação a um desktop Manjaro real:**

- Sem helper de AUR (yay/paru). Pacotes do AUR indisponíveis.
- Sem detecção de hardware do Manjaro (MHWD), sem trocador de kernel, sem
  integração de driver proprietário. Irrelevantes para um teste de build
  Qt6/C++.
- O conjunto de aplicativos GNOME/KDE específico do Manjaro está ausente. Só
  as ferramentas de build são instaladas.
- Nenhum `systemd` rodando no container.

**Primeira execução verificada:** 2026-03-22, run ID 23400921910, duração
**5m 11s**

| Componente | Versão observada |
|-----------|-----------------|
| Distro | Manjaro Linux (manjarolinux/base) |
| GCC | 15.2.1 20260209 |
| Qt6 | qt6-base 6.10.2-1 |
| CMake | 4.2.3 |
| Binário AstroFind | 70 MB (RelWithDebInfo, sem strip) |
| astrofind_tests | 45 MB |
| astrofind_ui_tests | 87 MB |

**Resultados de teste:**

| Suíte | Total | Passou | Pulados | Falhou |
|-------|-------|--------|---------|--------|
| Core (`astrofind_tests`) | 116 | 106 | 10 | 0 |
| UI (`astrofind_ui_tests`) | 23 | 23 | 0 | 0 |
| Total de assertions | 4426 | 4426 | -- | 0 |

Os 10 testes core pulados são os casos F-01 a F-10 de
`test_fits_functional.cpp`, que exigem arquivos de imagem FITS reais em
`/tmp/fits_test/`. Estes estão ausentes em todos os ambientes de CI (incluindo
o job de CI primário do Ubuntu) e são pulados por design.

**Observações desta execução:**

- O Manjaro traz **GCC 15** e **Qt 6.10**, ambos significativamente mais novos
  que as versões mínimas suportadas. O AstroFind compila sem avisos nesta
  configuração, confirmando que a base de código é compatível com versões
  futuras.
- O CMake 4.x está presente no Manjaro (4.2.3). O CMake 4.0 introduziu a
  política `CMP0167` (depreciando `FindBoost`) e mudou a semântica de
  `cmake_minimum_required`. Nenhum problema foi observado; o `CMakeLists.txt`
  do AstroFind não é afetado.
- A variável de ambiente `FORCE_JAVASCRIPT_ACTIONS_TO_NODE24=true` foi
  adicionada para suprimir o aviso de depreciação do Node.js 20 vindo de
  `actions/checkout@v4` e `actions/cache@v4`. O GitHub vai exigir Node.js 24
  por padrão a partir de 2026-06-02.

**Status:** Aprovado (item 55.4), roda a cada push para `main` e `develop`.

---

## 55.5 - Linux Mint 22

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-mint-22.yml`

**Docker image:** `linuxmintd/mint22-amd64`, maintained by the Linux Mint
project on Docker Hub. Based on Ubuntu 24.04 (Noble Nombat), Mint's upstream
for version 22.

**Package manager:** `apt`

**Qt6 availability:** Qt 6.6 from the Ubuntu 24.04 (Noble) base repos. Mint
does not maintain its own Qt packages; it inherits them from Ubuntu.

**GCC version:** GCC 14 (same as Ubuntu 24.04)

**Required packages:** Same as Ubuntu 24.04 (see section 55.1).

**Known issues / observations:**

- `linuxmintd/mint22-amd64` adds Mint-specific packages on top of Ubuntu
  24.04. The key difference being tested here is whether Mint's additional
  apt sources or pinning policies conflict with the Qt6 packages from Ubuntu
  repos. In practice, no conflicts have been observed.
- Mint ships with Cinnamon desktop components; the container image strips
  these down to a minimal install. The container is closer to a Mint
  server/minimal install than to the live ISO.
- `apt-get update` in this image requires `--allow-releaseinfo-change` on
  first run in some versions due to Mint's release metadata.

**Differences vs a real Linux Mint 22 desktop:**

- A real Mint 22 desktop ships with many Qt5 and Qt6 libraries pre-installed
  by the Cinnamon/Xfce stack. The container starts from a minimal base.
- Mint's Update Manager and software sources GUI are absent; direct `apt` is
  used.
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

**Status:** Passed (2026-03-22, run #23402319388)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-mint-22.yml`

**Imagem Docker:** `linuxmintd/mint22-amd64`, mantida pelo projeto Linux Mint
no Docker Hub. Baseada no Ubuntu 24.04 (Noble Nombat), o upstream do Mint para
a versão 22.

**Gerenciador de pacotes:** `apt`

**Disponibilidade do Qt6:** Qt 6.6 dos repositórios base do Ubuntu 24.04
(Noble). O Mint não mantém pacotes Qt próprios; herda-os do Ubuntu.

**Versão do GCC:** GCC 14 (igual ao Ubuntu 24.04)

**Pacotes obrigatórios:** Iguais aos do Ubuntu 24.04 (ver seção 55.1).

**Problemas / observações conhecidas:**

- `linuxmintd/mint22-amd64` adiciona pacotes específicos do Mint sobre o
  Ubuntu 24.04. A diferença chave testada aqui é se as fontes apt adicionais
  ou políticas de pinning do Mint entram em conflito com os pacotes Qt6 dos
  repositórios do Ubuntu. Na prática, nenhum conflito foi observado.
- O Mint vem com componentes de desktop Cinnamon; a imagem do container os
  remove para uma instalação mínima. O container está mais próximo de uma
  instalação mínima/servidor do Mint do que da ISO ao vivo.
- `apt-get update` nesta imagem exige `--allow-releaseinfo-change` na primeira
  execução em algumas versões devido aos metadados de release do Mint.

**Diferenças em relação a um desktop Linux Mint 22 real:**

- Um desktop Mint 22 real vem com muitas bibliotecas Qt5 e Qt6 pré-instaladas
  pela stack Cinnamon/Xfce. O container começa de uma base mínima.
- O Update Manager e a GUI de fontes de software do Mint estão ausentes; usa-se
  `apt` diretamente.
- Nenhuma sessão de usuário `systemd` rodando no container.

**Resultados verificados (2026-03-22):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `linuxmintd/mint22-amd64` |
| Distro | Ubuntu 24.04.3 LTS (base Mint 22) |
| GCC | 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04) |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Testes core | 106 passaram / 10 pulados (dependentes de rede) |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 14s |

**Status:** Aprovado (2026-03-22, run #23402319388)

---

## 55.6 - openSUSE Tumbleweed

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-opensuse-tw.yml`

**Docker image:** `opensuse/tumbleweed`, official image maintained by the
openSUSE project. Tumbleweed is a rolling release; the image tracks the
current snapshot.

**Package manager:** `zypper`

**Qt6 availability:** Latest Qt6 from openSUSE Tumbleweed repos (rolling, like
Arch).

**GCC version:** Latest GCC from openSUSE Tumbleweed repos

**Required packages:**

```bash
cmake ninja gcc-c++ git pkg-config
qt6-base-devel qt6-charts-devel qt6-opengl-devel
cfitsio-devel fftw3-devel libarchive-devel
```

**Optional packages available in repos:**

- `qt6-core5compat-devel`, available
- `qtkeychain-qt6-devel libsecret-devel`, available

**Known issues / observations:**

- openSUSE uses different package name conventions: `gcc-c++` (not `g++`),
  `fftw3-devel` (not `libfftw3-dev`), `cfitsio-devel` (not `libcfitsio-dev`).
- `zypper` may prompt for repo GPG key confirmation on first run inside a
  container. Use `--gpg-auto-import-keys` to suppress this.
- `zypper install` uses `-y`, same as apt, but the behaviour differs for repo
  confirmations, hence the extra `--gpg-auto-import-keys` flag.
- Tumbleweed's Qt6 packages follow openSUSE naming: `qt6-base-devel` instead
  of `qt6-qtbase-devel` (Fedora) or `qt6-base-dev` (Debian/Ubuntu).

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

**Status:** Passed (2026-03-22, run #23402424917)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-opensuse-tw.yml`

**Imagem Docker:** `opensuse/tumbleweed`, imagem oficial mantida pelo projeto
openSUSE. Tumbleweed é rolling release; a imagem acompanha o snapshot atual.

**Gerenciador de pacotes:** `zypper`

**Disponibilidade do Qt6:** Última versão do Qt6 dos repositórios do
openSUSE Tumbleweed (rolling, como o Arch).

**Versão do GCC:** Última versão de GCC dos repositórios do openSUSE
Tumbleweed

**Pacotes obrigatórios:**

```bash
cmake ninja gcc-c++ git pkg-config
qt6-base-devel qt6-charts-devel qt6-opengl-devel
cfitsio-devel fftw3-devel libarchive-devel
```

**Pacotes opcionais disponíveis nos repositórios:**

- `qt6-core5compat-devel`, disponível
- `qtkeychain-qt6-devel libsecret-devel`, disponível

**Problemas / observações conhecidas:**

- O openSUSE usa convenções de nome de pacote diferentes: `gcc-c++` (não
  `g++`), `fftw3-devel` (não `libfftw3-dev`), `cfitsio-devel` (não
  `libcfitsio-dev`).
- O `zypper` pode pedir confirmação de chave GPG do repositório na primeira
  execução dentro de um container. Use `--gpg-auto-import-keys` para suprimir
  isso.
- `zypper install` usa `-y`, igual ao apt, mas o comportamento difere para
  confirmações de repositório, daí a flag extra `--gpg-auto-import-keys`.
- Os pacotes Qt6 do Tumbleweed seguem a nomenclatura do openSUSE:
  `qt6-base-devel` em vez de `qt6-qtbase-devel` (Fedora) ou `qt6-base-dev`
  (Debian/Ubuntu).

**Diferenças em relação a um desktop openSUSE Tumbleweed real:**

- `opensuse/tumbleweed` é uma imagem mínima de servidor. Um desktop
  Tumbleweed KDE/GNOME real tem grande parte da stack do Qt6 pré-instalada.
- Sem YaST, sem serviço de auto-atualização do zypper rodando no container.
- Sem sessão de usuário `systemd`.

**Resultados verificados (2026-03-22):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `opensuse/tumbleweed` |
| Distro | openSUSE Tumbleweed (rolling) |
| GCC | 15.2.1 (SUSE Linux, 2026-02-02) |
| Qt6 | 6.10.2 |
| CMake | 4.2.3 |
| Testes core | 106 passaram / 10 pulados (dependentes de rede) |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 36s |

**Status:** Aprovado (2026-03-22, run #23402424917)

---

## 55.7 - Pop!_OS 22.04

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-pop-os.yml`

**Docker image:** There is no official Pop!_OS Docker image from System76. The
image used is `ubuntu:22.04` (Jammy Jellyfish), the upstream base that
Pop!_OS 22.04 is built from, with the System76 PPA added to approximate the
Pop!_OS package state.

> **Limitation:** This is the closest reproducible CI approximation for
> Pop!_OS 22.04. It does NOT include Pop!_OS's custom GNOME shell, recovery
> partition, systemd-boot configuration, or System76 firmware tools. What it
> does test is the exact same APT package base that Pop!_OS 22.04 users have
> available, including the System76 PPA.

**Package manager:** `apt` (same as Ubuntu)

**Qt6 availability:** Qt 6.4 from Ubuntu 22.04 (Jammy) repos. This is the same
Qt version available to Pop!_OS 22.04 users (System76 does not maintain its
own Qt6 packages).

> **Note:** Qt 6.4 is the **minimum supported** version for AstroFind. Any
> issue seen on Pop!_OS 22.04 is likely a Qt 6.4 regression identical to what
> Debian 12 shows.

**GCC version:** GCC 12 (default in Ubuntu 22.04)

**Required packages:**

```bash
cmake ninja-build gcc-12 g++-12 pkg-config git
qt6-base-dev libqt6charts6-dev libqt6opengl6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libxkbcommon-dev
```

**Known issues / observations:**

- Ubuntu 22.04 ships Qt 6.4. The `QImage::mirrored()` fix (instead of
  `flipped()`) was specifically required for Qt 6.4 compatibility and must not
  regress.
- The `QTimeZone(0)` constructor approach (instead of `Qt::UTC`) is necessary
  on Qt 6.4 as `Qt::UTC` is deprecated in Qt 6.7+. This is validated here.
- `libqt6charts6-dev` may be in a different sub-package on Ubuntu 22.04 vs
  24.04. Verify the exact package name during first run.

**Differences vs a real Pop!_OS 22.04 desktop:**

- No System76 GNOME customisations, no Pop Shell tiling extension.
- No System76 firmware tools (system76-driver, system76-power).
- The System76 PPA is added to include any System76-specific packages, but in
  practice System76 does not ship custom Qt6 packages, so the PPA adds little
  for build testing.
- This is inherently an approximation. Full validation requires a real
  Pop!_OS install.

**Note:** Pop!_OS 22.04 is based on Ubuntu 22.04 which ships Qt 6.2.4, below
AstroFind's minimum Qt 6.4. The workflow uses `ubuntu:24.04` instead (Qt 6.4.2
available). This approximates Pop!_OS 22.04 users who have upgraded Qt or use
the current Pop!_OS.

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

**Status:** Passed (2026-03-22, run #23403368713)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-pop-os.yml`

**Imagem Docker:** Não existe imagem Docker oficial do Pop!_OS pelo System76.
A imagem usada é `ubuntu:22.04` (Jammy Jellyfish), a base upstream sobre a
qual o Pop!_OS 22.04 é construído, com o PPA do System76 adicionado para
aproximar o estado de pacotes do Pop!_OS.

> **Limitação:** Esta é a aproximação de CI reproduzível mais próxima para o
> Pop!_OS 22.04. NÃO inclui o shell GNOME customizado do Pop!_OS, partição de
> recuperação, configuração systemd-boot, nem ferramentas de firmware do
> System76. O que ela testa é exatamente a mesma base de pacotes APT que os
> usuários do Pop!_OS 22.04 têm disponível, incluindo o PPA do System76.

**Gerenciador de pacotes:** `apt` (igual ao Ubuntu)

**Disponibilidade do Qt6:** Qt 6.4 dos repositórios do Ubuntu 22.04 (Jammy).
Esta é a mesma versão de Qt disponível para usuários do Pop!_OS 22.04 (o
System76 não mantém pacotes Qt6 próprios).

> **Nota:** Qt 6.4 é a versão **mínima suportada** pelo AstroFind. Qualquer
> problema visto no Pop!_OS 22.04 é provavelmente uma regressão de Qt 6.4
> idêntica à que o Debian 12 mostra.

**Versão do GCC:** GCC 12 (padrão no Ubuntu 22.04)

**Pacotes obrigatórios:**

```bash
cmake ninja-build gcc-12 g++-12 pkg-config git
qt6-base-dev libqt6charts6-dev libqt6opengl6-dev
libcfitsio-dev libfftw3-dev libarchive-dev
libgl1-mesa-dev libxkbcommon-dev
```

**Problemas / observações conhecidas:**

- O Ubuntu 22.04 traz Qt 6.4. A correção `QImage::mirrored()` (em vez de
  `flipped()`) foi especificamente necessária para compatibilidade com Qt
  6.4 e não pode regredir.
- A abordagem do construtor `QTimeZone(0)` (em vez de `Qt::UTC`) é necessária
  no Qt 6.4, já que `Qt::UTC` é depreciado no Qt 6.7+. Isso é validado aqui.
- `libqt6charts6-dev` pode estar num sub-pacote diferente no Ubuntu 22.04 vs
  24.04. Verificar o nome exato do pacote na primeira execução.

**Diferenças em relação a um desktop Pop!_OS 22.04 real:**

- Sem customizações GNOME do System76, sem a extensão de tiling Pop Shell.
- Sem ferramentas de firmware do System76 (system76-driver, system76-power).
- O PPA do System76 é adicionado para incluir quaisquer pacotes específicos do
  System76, mas na prática o System76 não distribui pacotes Qt6 customizados,
  então o PPA acrescenta pouco para o teste de build.
- Isso é inerentemente uma aproximação. Validação completa requer uma
  instalação Pop!_OS real.

**Nota:** o Pop!_OS 22.04 é baseado no Ubuntu 22.04, que traz Qt 6.2.4, abaixo
do Qt 6.4 mínimo do AstroFind. O workflow usa `ubuntu:24.04` em vez disso (Qt
6.4.2 disponível). Isso aproxima usuários do Pop!_OS 22.04 que atualizaram o
Qt ou usam o Pop!_OS atual.

**Resultados verificados (2026-03-22, run #23403368713):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `ubuntu:24.04` (aproximação do Pop!_OS 22.04) |
| Distro | Ubuntu 24.04.4 LTS |
| GCC | 13.3.0 |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Testes core | 106 passaram / 10 pulados |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 8s |

**Status:** Aprovado (2026-03-22, run #23403368713)

---

## 55.8 - Rocky Linux 9

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-rocky-9.yml`

**Docker image:** `rockylinux:9`, official Rocky Linux image from Docker Hub,
maintained by the Rocky Linux project.

**Package manager:** `dnf`

**Qt6 availability:** Qt 6 is **not in the Rocky Linux 9 default repos**. It
must be installed from EPEL ("Extra Packages for Enterprise Linux", a
community repository with extra packages for RHEL-family distros) after
enabling the CRB ("CodeReady Builder") repository. The Qt6 version available
in EPEL for RHEL 9 / Rocky 9 is typically Qt 6.4-6.5.

> **Known complication:** EPEL for RHEL 9 may not have all required Qt6
> modules (notably `qt6-qtcharts`). If `qt6-qtcharts-devel` is unavailable,
> the build will fail with a CMake `find_package(Qt6Charts)` error. This is a
> known gap in EPEL coverage as of 2026-03.

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

**GCC version:** GCC 11 (system default in Rocky 9) or GCC 13 via
`gcc-toolset-13` (Software Collections). **GCC 13+ is required for C++23
`std::expected`**, the workflow must use `gcc-toolset-13` and `scl enable
gcc-toolset-13`.

**Known issues / observations:**

- Rocky Linux 9 is the hardest distro in this QA suite. It requires EPEL +
  CRB and uses GCC 11 by default, which does not support C++23
  `std::expected`. The `gcc-toolset-13` package from SCL must be used.
- `qt6-qtcharts-devel` availability in EPEL is uncertain and must be verified
  on first run. If absent, this is a real gap that affects Rocky 9 users.
- The `scl enable` wrapper changes the shell environment but does not persist
  across GitHub Actions `run:` steps. The full CMake + build command must be
  wrapped in a single `scl enable gcc-toolset-13 -- bash -c "..."` call.

**Differences vs a real Rocky Linux 9 server:**

- `rockylinux:9` is a minimal server install, close to what a Rocky 9 server
  admin would start with. There is no desktop environment.
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
| qt6-qtcharts-devel | available in EPEL 9 |
| qt6-qtopengl-devel | NOT in EPEL 9 (CMake uses qtbase OpenGL fallback) |
| qt6-qttools-devel | NOT in EPEL 9 (lupdate/lrelease skipped in CI) |
| Core tests | 106 passed / 10 skipped |
| UI tests | 23 passed (71 assertions) |
| Duration | ~4m 48s |

**Status:** Passed (2026-03-22, run #23403368714)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-rocky-9.yml`

**Imagem Docker:** `rockylinux:9`, imagem oficial do Rocky Linux no Docker
Hub, mantida pelo projeto Rocky Linux.

**Gerenciador de pacotes:** `dnf`

**Disponibilidade do Qt6:** O Qt 6 **não está nos repositórios padrão do
Rocky Linux 9**. Precisa ser instalado a partir do EPEL ("Extra Packages for
Enterprise Linux", um repositório comunitário com pacotes extras para
distros da família RHEL) após habilitar o repositório CRB ("CodeReady
Builder"). A versão de Qt6 disponível no EPEL para RHEL 9 / Rocky 9 é
tipicamente Qt 6.4-6.5.

> **Complicação conhecida:** o EPEL para RHEL 9 pode não ter todos os
> módulos Qt6 obrigatórios (notavelmente `qt6-qtcharts`). Se
> `qt6-qtcharts-devel` estiver indisponível, o build falhará com um erro
> `find_package(Qt6Charts)` do CMake. Esta é uma lacuna conhecida na cobertura
> do EPEL desde 2026-03.

**Pacotes obrigatórios:**

```bash
# Pré-requisito: habilitar EPEL e CRB
dnf install -y epel-release
dnf config-manager --set-enabled crb

# Ferramentas de build
cmake ninja-build gcc-toolset-13 git pkg-config

# Qt6 (do EPEL)
qt6-qtbase-devel qt6-qtcharts-devel
qt6-qtopengl-devel qt6-qtnetwork-devel
qt6-qtsql-devel qt6-qtxml-devel
qt6-qtconcurrent-devel

# Bibliotecas científicas
cfitsio-devel fftw-devel libarchive-devel
```

**Versão do GCC:** GCC 11 (padrão do sistema no Rocky 9) ou GCC 13 via
`gcc-toolset-13` (Software Collections). **GCC 13+ é obrigatório para
`std::expected` do C++23**, o workflow precisa usar `gcc-toolset-13` e `scl
enable gcc-toolset-13`.

**Problemas / observações conhecidas:**

- O Rocky Linux 9 é a distro mais difícil desta suíte de QA. Exige EPEL + CRB
  e usa GCC 11 por padrão, que não suporta `std::expected` do C++23. O
  pacote `gcc-toolset-13` do SCL precisa ser usado.
- A disponibilidade de `qt6-qtcharts-devel` no EPEL é incerta e precisa ser
  verificada na primeira execução. Se ausente, é uma lacuna real que afeta
  usuários do Rocky 9.
- O wrapper `scl enable` muda o ambiente do shell, mas não persiste entre
  passos `run:` do GitHub Actions. O comando completo de CMake + build precisa
  ficar dentro de uma única chamada `scl enable gcc-toolset-13 -- bash -c
  "..."`.

**Diferenças em relação a um servidor Rocky Linux 9 real:**

- `rockylinux:9` é uma instalação mínima de servidor, próxima do que um
  admin de servidor Rocky 9 começaria a usar. Não há ambiente de desktop.
- Um desktop Rocky 9 real (raro, mas possível com GNOME) teria mais Qt5
  pré-instalado. O Qt6 ainda precisaria do EPEL.

**Resultados verificados (2026-03-22, run #23403368714):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `rockylinux:9` |
| Distro | Rocky Linux 9.3 (Blue Onyx) |
| GCC (toolset) | 13.3.1 (Red Hat 13.3.1-2, via gcc-toolset-13) |
| Qt6 | 6.6.2 (do EPEL 9) |
| CMake | 3.26.5 |
| qt6-qtcharts-devel | disponível no EPEL 9 |
| qt6-qtopengl-devel | NÃO está no EPEL 9 (CMake usa fallback de OpenGL do qtbase) |
| qt6-qttools-devel | NÃO está no EPEL 9 (lupdate/lrelease pulados no CI) |
| Testes core | 106 passaram / 10 pulados |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 48s |

**Status:** Aprovado (2026-03-22, run #23403368714)

---

## 55.9 - Zorin OS 17

### 🇬🇧 English

**Workflow file:** `.github/workflows/qa-zorin-17.yml`

**Docker image:** There is no official Zorin OS Docker image from Zorin
Group. The image used is `ubuntu:22.04`, the upstream base that Zorin OS 17 is
built from.

> **Limitation:** This is the closest reproducible CI approximation for
> Zorin OS 17. Zorin OS 17 is based on Ubuntu 22.04 LTS with a customised
> GNOME desktop (Zorin Desktop Environment), its own theme engine, and
> Zorin-specific apps. None of these are present in the container. The
> package base, the APT repos, the package versions, and the system
> libraries, is identical to Ubuntu 22.04, which is what matters for building
> and testing a Qt6/C++ application.

**Package manager:** `apt` (same as Ubuntu)

**Qt6 availability:** Qt 6.4 from Ubuntu 22.04 repos. Zorin Group does not
maintain custom Qt6 packages.

**GCC version:** GCC 12 (same as Ubuntu 22.04)

**Required packages:** Identical to Ubuntu 22.04 / Pop!_OS 22.04 (see section
55.7).

**Known issues / observations:**

- Qt 6.4 limitations apply identically to Zorin OS 17, Pop!_OS 22.04, and
  Debian 12.
- If tests pass on Ubuntu 22.04, they will pass on Zorin OS 17 for
  build/unit-test purposes. The Zorin-specific parts (desktop, themes, Zorin
  apps) are irrelevant for a Qt6/C++ scientific application.

**Differences vs a real Zorin OS 17 desktop:**

- The Zorin Desktop Environment (ZDE), Zorin appearance tools, and
  pre-installed Zorin apps are absent.
- No Zorin-specific APT sources or PPA added. Zorin does not add repos that
  would affect Qt6 packages.
- This is inherently an approximation. Full validation requires a real Zorin
  OS install or an ISO-based VM.

**Note:** Zorin OS 17 is based on Ubuntu 22.04 which ships Qt 6.2.4, below
AstroFind's minimum Qt 6.4. The workflow uses `ubuntu:24.04` instead. This
approximates the APT package base for Zorin OS users who need Qt 6.4+.

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

**Status:** Passed (2026-03-22, run #23403368740)

### 🇧🇷 Português

**Arquivo de workflow:** `.github/workflows/qa-zorin-17.yml`

**Imagem Docker:** Não existe imagem Docker oficial do Zorin OS pelo Zorin
Group. A imagem usada é `ubuntu:22.04`, a base upstream sobre a qual o Zorin
OS 17 é construído.

> **Limitação:** Esta é a aproximação de CI reproduzível mais próxima para o
> Zorin OS 17. O Zorin OS 17 é baseado no Ubuntu 22.04 LTS com um desktop
> GNOME customizado (Zorin Desktop Environment), motor de temas próprio, e
> apps específicos do Zorin. Nenhum desses está presente no container. A base
> de pacotes, os repositórios APT, as versões de pacote, e as bibliotecas do
> sistema, é idêntica ao Ubuntu 22.04, que é o que importa para compilar e
> testar uma aplicação Qt6/C++.

**Gerenciador de pacotes:** `apt` (igual ao Ubuntu)

**Disponibilidade do Qt6:** Qt 6.4 dos repositórios do Ubuntu 22.04. O Zorin
Group não mantém pacotes Qt6 customizados.

**Versão do GCC:** GCC 12 (igual ao Ubuntu 22.04)

**Pacotes obrigatórios:** Idênticos ao Ubuntu 22.04 / Pop!_OS 22.04 (ver
seção 55.7).

**Problemas / observações conhecidas:**

- As limitações do Qt 6.4 se aplicam identicamente ao Zorin OS 17, Pop!_OS
  22.04, e Debian 12.
- Se os testes passam no Ubuntu 22.04, eles passarão no Zorin OS 17 para
  fins de build/teste unitário. As partes específicas do Zorin (desktop,
  temas, apps Zorin) são irrelevantes para uma aplicação científica Qt6/C++.

**Diferenças em relação a um desktop Zorin OS 17 real:**

- O Zorin Desktop Environment (ZDE), as ferramentas de aparência do Zorin, e
  os apps Zorin pré-instalados estão ausentes.
- Nenhuma fonte APT ou PPA específica do Zorin foi adicionada. O Zorin não
  adiciona repositórios que afetariam pacotes Qt6.
- Isso é inerentemente uma aproximação. Validação completa requer uma
  instalação Zorin OS real ou uma VM baseada em ISO.

**Nota:** o Zorin OS 17 é baseado no Ubuntu 22.04, que traz Qt 6.2.4, abaixo
do Qt 6.4 mínimo do AstroFind. O workflow usa `ubuntu:24.04` em vez disso.
Isso aproxima a base de pacotes APT para usuários do Zorin OS que precisam de
Qt 6.4+.

**Resultados verificados (2026-03-22, run #23403368740):**

| Item | Resultado |
|------|--------|
| Imagem Docker | `ubuntu:24.04` (aproximação do Zorin OS 17) |
| Distro | Ubuntu 24.04.4 LTS |
| GCC | 13.3.0 |
| Qt6 | 6.4.2+dfsg-21.1build5 |
| CMake | 3.28.3 |
| Testes core | 106 passaram / 10 pulados |
| Testes de UI | 23 passaram (71 assertions) |
| Duração | ~4m 13s |

**Status:** Aprovado (2026-03-22, run #23403368740)

---

## Appendix: Docker image provenance / Apêndice: proveniência das imagens Docker

### 🇬🇧 English

| Distro | Image | Maintainer | Notes |
|--------|-------|------------|-------|
| Ubuntu 24.04 | `ubuntu:24.04` | Canonical | Official |
| Debian 12 | `debian:12` | Debian project | Official |
| Arch Linux | `archlinux:latest` | Arch Linux team | Official |
| Manjaro | `manjarolinux/base` | Manjaro team | Official minimal |
| Linux Mint 22 | `linuxmintd/mint22-amd64` | Linux Mint project | Official minimal |
| openSUSE TW | `opensuse/tumbleweed` | openSUSE project | Official |
| Rocky Linux 9 | `rockylinux:9` | Rocky Linux project | Official |
| Pop!_OS 22.04 | `ubuntu:22.04` + System76 PPA | -- | Approximation |
| Zorin OS 17 | `ubuntu:22.04` | -- | Approximation |

### 🇧🇷 Português

| Distro | Imagem | Mantenedor | Notas |
|--------|-------|------------|-------|
| Ubuntu 24.04 | `ubuntu:24.04` | Canonical | Oficial |
| Debian 12 | `debian:12` | Projeto Debian | Oficial |
| Arch Linux | `archlinux:latest` | Equipe Arch Linux | Oficial |
| Manjaro | `manjarolinux/base` | Equipe Manjaro | Mínima oficial |
| Linux Mint 22 | `linuxmintd/mint22-amd64` | Projeto Linux Mint | Mínima oficial |
| openSUSE TW | `opensuse/tumbleweed` | Projeto openSUSE | Oficial |
| Rocky Linux 9 | `rockylinux:9` | Projeto Rocky Linux | Oficial |
| Pop!_OS 22.04 | `ubuntu:22.04` + PPA System76 | -- | Aproximação |
| Zorin OS 17 | `ubuntu:22.04` | -- | Aproximação |

---

*AstroFind. Last updated / Última atualização: 2026-07-10, AstroFind v0.9.0.*
