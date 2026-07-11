# Contributing to AstroFind / Contribuindo com o AstroFind

> **Last reviewed / Última revisão:** 2026-07-10
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
C++ compiler supporting C++23 (GCC 13+ or Clang 17+)
Qt 6.4+ (Core, Widgets, Concurrent, Charts, Network, Keychain)
CMake 3.20+
cfitsio, Eigen3, fftw3, spdlog, nlohmann_json
CCfits (bundled, extracted automatically from originals/CCfits.tar.gz)
libarchive (optional, for TAR.GZ/BZ2/XZ/7Z/RAR support)
```

Note for beginners: "bundled" means the library's source code ships inside this
repository (`originals/`), so you do not need to download it separately; CMake
extracts and builds it automatically as part of the project build.

### 🇧🇷 Português

```
Compilador C++ com suporte a C++23 (GCC 13+ ou Clang 17+)
Qt 6.4+ (Core, Widgets, Concurrent, Charts, Network, Keychain)
CMake 3.20+
cfitsio, Eigen3, fftw3, spdlog, nlohmann_json
CCfits (empacotado, extraído automaticamente de originals/CCfits.tar.gz)
libarchive (opcional, para suporte a TAR.GZ/BZ2/XZ/7Z/RAR)
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

- Core files: add to `src/core/CMakeLists.txt` under `CORE_SOURCES` / `CORE_HEADERS`.
- UI files: add to `src/ui/CMakeLists.txt` under `UI_SOURCES` / `UI_HEADERS`.
- New Qt modules: add to `target_link_libraries` in the relevant `CMakeLists.txt`.

### 🇧🇷 Português

- Arquivos core: adicionar em `src/core/CMakeLists.txt` em `CORE_SOURCES` / `CORE_HEADERS`.
- Arquivos UI: adicionar em `src/ui/CMakeLists.txt` em `UI_SOURCES` / `UI_HEADERS`.
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
build/bin/astrofind_tests      # 116 core tests
build/bin/astrofind_ui_tests   # 23 UI integration tests
```

Three pre-existing failures in `astrofind_tests` are known and tracked
(network-dependent tests skipped in CI, "CI" = Continuous Integration, the automated
build/test pipeline that runs on every push).

### 🇧🇷 Português

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
# ou diretamente:
build/bin/astrofind_tests      # 116 testes core
build/bin/astrofind_ui_tests   # 23 testes de integração UI
```

Três falhas pré-existentes em `astrofind_tests` são conhecidas e rastreadas (testes
dependentes de rede ignorados no CI; "CI" = Integração Contínua, o pipeline
automatizado de build/teste que roda a cada push).

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
