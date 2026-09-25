# Contributing to AstroFind / Contribuindo com o AstroFind

> **Last reviewed / Última revisão:** 2026-09-24
> **Owner:** Petrus Silva Costa

Thank you for your interest in contributing to AstroFind! / Obrigado pelo seu interesse em contribuir com o AstroFind!

---

## Getting Started / Começando

### 🇬🇧 English

1. Fork the repository and clone your fork.
2. Build the project following the instructions in [README.md](README.md).
3. Create a new branch for your change: `git checkout -b feature/my-change`

### 🇧🇷 Português

1. Faça um fork do repositório e clone o seu fork.
2. Compile o projeto seguindo as instruções em [README.md](README.md).
3. Crie um novo branch para sua alteração: `git checkout -b feature/minha-mudanca`

---

## Prerequisites / Pré-requisitos

### 🇬🇧 English

```
C++ compiler supporting C++23 (GCC 12+ — Debian 12 is the oldest tested — or Clang 16+)
Qt 6.4+ (Core, Gui, Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml)
CMake 3.22+
cfitsio, fftw3
spdlog, nlohmann_json, SEP, Catch2 (fetched automatically by CMake FetchContent)
CCfits (bundled, extracted automatically from originals/CCfits.tar.gz)
Optional: Qt6Keychain (secure API-key storage), libarchive (TAR.GZ/BZ2/XZ/7Z/RAR),
          LibRaw (DSLR RAW files); unzip(1) at run time for ZIP image sets
```

Note for beginners: "bundled" means the library's source code ships inside this
repository (`originals/`), so you do not need to download it separately; CMake
extracts and builds it automatically as part of the project build.

### 🇧🇷 Português

```
Compilador C++ com suporte a C++23 (GCC 12+ — o Debian 12 é o mais antigo testado — ou Clang 16+)
Qt 6.4+ (Core, Gui, Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml)
CMake 3.22+
cfitsio, fftw3
spdlog, nlohmann_json, SEP, Catch2 (baixados automaticamente pelo FetchContent do CMake)
CCfits (empacotado, extraído automaticamente de originals/CCfits.tar.gz)
Opcionais: Qt6Keychain (armazenamento seguro da chave de API), libarchive (TAR.GZ/BZ2/XZ/
           7Z/RAR), LibRaw (arquivos RAW de DSLR); unzip(1) em tempo de execução para ZIP
```

Nota para iniciantes: "empacotado" (bundled) significa que o código-fonte da
biblioteca já vem dentro deste repositório (`originals/`), então você não precisa
baixá-la separadamente; o CMake extrai e compila automaticamente como parte do
build do projeto.

---

## Project Layout / Estrutura do Projeto

### 🇬🇧 English

| Directory | Contents |
|-----------|----------|
| `src/core/` | Business logic, no Qt UI dependencies |
| `src/ui/` | Qt widgets, dialogs, panels, built entirely in code (no `.ui` files) |
| `i18n/` | Qt Linguist files (`.ts` source, `.qm` compiled, "i18n" = internationalization) |
| `resources/` | Icons (programmatic), help HTML, QRC manifest |
| `tests/` | 116 core unit tests + 23 UI integration tests |
| `cmake/` | Find modules and optional dependency detection |
| `originals/` | Bundled library archives (CCfits) |

### 🇧🇷 Português

| Diretório | Conteúdo |
|-----------|----------|
| `src/core/` | Lógica de negócio, sem dependências de UI Qt |
| `src/ui/` | Widgets Qt, diálogos, painéis, construídos totalmente em código (sem arquivos `.ui`) |
| `i18n/` | Arquivos Qt Linguist (`.ts` fonte, `.qm` compilado; "i18n" = internacionalização) |
| `resources/` | Ícones (programáticos), HTML de ajuda, manifesto QRC |
| `tests/` | 116 testes unitários de core + 23 testes de integração de UI |
| `cmake/` | Módulos Find e detecção de dependências opcionais |
| `originals/` | Arquivos de bibliotecas empacotadas (CCfits) |

---

## Code Style / Estilo de Código

### 🇬🇧 English

- **C++23**, Qt6 idioms. No `.ui` files, all widgets built in code.
- Core logic in `src/core/` must have **no Qt UI dependencies**.
- Icons drawn with `QPainter` in `src/ui/AppIcons.cpp`.
- Settings keys follow the namespace pattern: `observer/*`, `camera/*`, `astrometry/*`,
  `catalog/*`, `photometry/*`, `report/*`, `ui/*`, `display/*`.
- No toolbar modifications, new features go in **menus, context menus, docks, or
  keyboard shortcuts**.
- Use `std::expected<T, QString>` for fallible operations returning errors to the
  caller ("fallible" = an operation that can fail, e.g. loading a file; `std::expected`
  is a C++23 type that carries either a success value or an error, instead of throwing).
- Optional dependencies guarded by `#ifdef ASTROFIND_HAS_<FEATURE>` compile-time flags.

### 🇧🇷 Português

- **C++23**, idiomas Qt6. Sem arquivos `.ui`, todos os widgets construídos em código.
- Lógica central em `src/core/` não deve ter **dependências de UI Qt**.
- Ícones desenhados com `QPainter` em `src/ui/AppIcons.cpp`.
- Chaves de configurações seguem o padrão de namespace: `observer/*`, `camera/*`,
  `astrometry/*`, `catalog/*`, `photometry/*`, `report/*`, `ui/*`, `display/*`.
- Sem modificações na barra de ferramentas, novos recursos vão em **menus, menus de
  contexto, docks ou atalhos de teclado**.
- Use `std::expected<T, QString>` para operações falíveis que retornam erros ao
  chamador ("falível" = uma operação que pode falhar, ex.: carregar um arquivo;
  `std::expected` é um tipo do C++23 que carrega um valor de sucesso ou um erro, em
  vez de lançar uma exceção).
- Dependências opcionais protegidas por flags de compilação `#ifdef ASTROFIND_HAS_<FEATURE>`.

---

## Adding New Source Files / Adicionando Novos Arquivos Fonte

### 🇬🇧 English

- Core files: add the `.cpp` and `.h` to the `add_library(astrofind_core STATIC …)` list in
  `src/core/CMakeLists.txt`.
- UI files: add them to the `add_library(astrofind_ui STATIC …)` list in `src/ui/CMakeLists.txt`.
- New tests: add the `test_*.cpp` to `tests/CMakeLists.txt` (`astrofind_tests` for core,
  `astrofind_ui_tests` for UI). Catch2 test names must not contain commas (a comma breaks
  name filtering: "No tests ran", exit 0 — AUD-TEST-5).
- New Qt modules: add to `target_link_libraries` in the relevant `CMakeLists.txt`.

### 🇧🇷 Português

- Arquivos core: adicionar o `.cpp` e o `.h` na lista do `add_library(astrofind_core STATIC …)`
  em `src/core/CMakeLists.txt`.
- Arquivos UI: adicionar na lista do `add_library(astrofind_ui STATIC …)` em
  `src/ui/CMakeLists.txt`.
- Testes novos: adicionar o `test_*.cpp` em `tests/CMakeLists.txt` (`astrofind_tests` para o
  core, `astrofind_ui_tests` para a UI). Nomes de teste Catch2 não podem ter vírgula (a vírgula
  quebra o filtro por nome: "No tests ran", saída 0 — AUD-TEST-5).
- Novos módulos Qt: adicionar em `target_link_libraries` no `CMakeLists.txt` relevante.

---

## Adding UI Strings / Adicionando Strings de UI

### 🇬🇧 English

Always wrap user-visible text with `tr()` (Qt's translation marker function). After
adding new strings:

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```

`lupdate` scans the source for `tr()` calls and updates the `.ts` (translation
source, XML) files; `lrelease` compiles them into the binary `.qm` files the app
loads at runtime. Fill in `<translation>` tags in `astrofind_pt_BR.ts` before
submitting.

### 🇧🇷 Português

Sempre envolva texto visível ao usuário com `tr()` (função marcadora de tradução do
Qt). Após adicionar novas strings:

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```

`lupdate` varre o código-fonte em busca de chamadas `tr()` e atualiza os arquivos
`.ts` (fonte de tradução, XML); `lrelease` compila-os nos arquivos binários `.qm`
que o app carrega em tempo de execução. Preencha as tags `<translation>` em
`astrofind_pt_BR.ts` antes de enviar.

---

## Running Tests / Executando Testes

### 🇬🇧 English

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
# or directly:
build/bin/astrofind_tests      # 179 core test cases
build/bin/astrofind_ui_tests   # 27 UI integration test cases (needs a display, or QT_QPA_PLATFORM=offscreen)
```

Every test passes in CI ("CI" = Continuous Integration, the automated build/test pipeline
that runs on every push) on 10 distributions, and the numerical audit (ASan/UBSan,
cppcheck, clang-tidy, Valgrind) runs on Fedora, CachyOS, Arch, Ubuntu and Debian for every
pull request into `main`. Run `cmake --build build --target audit` to get the same reports
locally in `build/audit/`.

### 🇧🇷 Português

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
# ou diretamente:
build/bin/astrofind_tests      # 179 casos de teste do core
build/bin/astrofind_ui_tests   # 27 casos de integração de UI (precisa de display, ou QT_QPA_PLATFORM=offscreen)
```

Todos os testes passam no CI ("CI" = Integração Contínua, o pipeline automatizado de
build/teste que roda a cada push) em 10 distribuições, e a auditoria numérica (ASan/UBSan,
cppcheck, clang-tidy, Valgrind) roda em Fedora, CachyOS, Arch, Ubuntu e Debian em todo pull
request para a `main`. Rode `cmake --build build --target audit` para gerar os mesmos
relatórios localmente em `build/audit/`.

---

## Git Hooks (fast local gate) / Git Hooks (gate rápido local)

### 🇬🇧 English

AUD-CI-3: install the local `pre-commit` hook once per clone to build incrementally
and run `ctest` before every commit ("hook" = a script Git runs automatically at a
specific point, here before a commit is created); it catches a broken build/test on
your machine instead of on the PR:

```bash
ln -sf ../../scripts/pre-commit .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

Works on Linux/macOS (native bash) and Windows (Git Bash, which is what runs git
hooks there too). Emergency bypass: `SKIP_TESTS=1 git commit ...` (use sparingly:
it is not a substitute for `--no-verify`, it just skips the local double-check).

The heavier numerical audit (ASan/UBSan, cppcheck, clang-tidy, valgrind; items
38.1-38.5, tools that catch memory bugs and static-analysis issues) does not run
in this hook. It runs in CI on `pull_request -> main` and on release tags only
(`.github/workflows/audit.yml`, self-hosted runner), because it takes minutes,
not seconds.

### 🇧🇷 Português

AUD-CI-3: instale o hook local `pre-commit` uma vez por clone para compilar
incrementalmente e rodar `ctest` antes de cada commit ("hook" = um script que o Git
executa automaticamente num ponto específico, aqui antes de um commit ser criado);
ele pega build/teste quebrado na sua máquina, antes do PR:

```bash
ln -sf ../../scripts/pre-commit .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

Funciona em Linux/macOS (bash nativo) e Windows (Git Bash, que é o que roda git
hooks lá também). Bypass de emergência: `SKIP_TESTS=1 git commit ...` (usar com
parcimônia: não substitui `--no-verify`, só pula a checagem local).

A auditoria numérica mais pesada (ASan/UBSan, cppcheck, clang-tidy, valgrind; itens
38.1-38.5, ferramentas que pegam bugs de memória e problemas de análise estática)
não roda neste hook. Ela roda em CI só em `pull_request -> main` e em tags de
release (`.github/workflows/audit.yml`, runner self-hosted), porque leva minutos,
não segundos.

---

## Submitting Changes / Enviando Alterações

### 🇬🇧 English

- Open a pull request with a clear description of the change and why it's needed.
- Include screenshots for UI changes.
- All new code should have corresponding tests in `tests/`.
- Update `CHANGELOG.md` with a summary entry under `[Unreleased]`.

### 🇧🇷 Português

- Abra um pull request com uma descrição clara da alteração e por que ela é necessária.
- Inclua capturas de tela para alterações de UI.
- Todo código novo deve ter testes correspondentes em `tests/`.
- Atualize `CHANGELOG.md` com uma entrada de resumo em `[Não lançado]`.

---

## License / Licença

### 🇬🇧 English

By contributing, you agree that your contributions will be licensed under the same
[GNU Affero General Public License v3.0 (AGPL-3.0)](LICENSE) that covers the project.

### 🇧🇷 Português

Ao contribuir, você concorda que suas contribuições serão licenciadas sob a mesma
[GNU Affero General Public License v3.0 (AGPL-3.0)](LICENSE) que cobre o projeto.
