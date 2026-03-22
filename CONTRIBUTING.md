# Contributing to AstroFind

**English** | [Português](#português)

---

## English

Thank you for your interest in contributing to AstroFind!

### Getting Started

1. Fork the repository and clone your fork.
2. Build the project following the instructions in [README.md](README.md).
3. Create a new branch for your change: `git checkout -b feature/my-change`

### Prerequisites

```
C++ compiler supporting C++23 (GCC 13+ or Clang 17+)
Qt 6.4+ (Core, Widgets, Concurrent, Charts, Network, Keychain)
CMake 3.20+
cfitsio, Eigen3, fftw3, spdlog, nlohmann_json
CCfits (bundled — extracted automatically from originals/CCfits.tar.gz)
libarchive (optional — for TAR.GZ/BZ2/XZ/7Z/RAR support)
```

### Project Layout

| Directory | Contents |
|-----------|----------|
| `src/core/` | Business logic — no Qt UI dependencies |
| `src/ui/` | Qt widgets, dialogs, panels — built entirely in code (no `.ui` files) |
| `i18n/` | Qt Linguist files (`.ts` source, `.qm` compiled) |
| `resources/` | Icons (programmatic), help HTML, QRC manifest |
| `tests/` | 116 core unit tests + 23 UI integration tests |
| `cmake/` | Find modules and optional dependency detection |
| `originals/` | Bundled library archives (CCfits) |

### Code Style

- **C++23**, Qt6 idioms. No `.ui` files — all widgets built in code.
- Core logic in `src/core/` must have **no Qt UI dependencies**.
- Icons drawn with `QPainter` in `src/ui/AppIcons.cpp`.
- Settings keys follow the namespace pattern: `observer/*`, `camera/*`, `astrometry/*`, `catalog/*`, `photometry/*`, `report/*`, `ui/*`, `display/*`.
- No toolbar modifications — new features go in **menus, context menus, docks, or keyboard shortcuts**.
- Use `std::expected<T, QString>` for fallible operations returning errors to the caller.
- Optional dependencies guarded by `#ifdef ASTROFIND_HAS_<FEATURE>` compile-time flags.

### Adding New Source Files

- Core files: add to `src/core/CMakeLists.txt` under `CORE_SOURCES` / `CORE_HEADERS`.
- UI files: add to `src/ui/CMakeLists.txt` under `UI_SOURCES` / `UI_HEADERS`.
- New Qt modules: add to `target_link_libraries` in the relevant `CMakeLists.txt`.

### Adding UI Strings

Always wrap user-visible text with `tr()`. After adding new strings:

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```

Fill in `<translation>` tags in `astrofind_pt_BR.ts` before submitting.

### Running Tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
# or directly:
build/bin/astrofind_tests      # 116 core tests
build/bin/astrofind_ui_tests   # 23 UI integration tests
```

Three pre-existing failures in `astrofind_tests` are known and tracked (network-dependent tests skipped in CI).

### Submitting Changes

- Open a pull request with a clear description of the change and why it's needed.
- Include screenshots for UI changes.
- All new code should have corresponding tests in `tests/`.
- Update `CHANGELOG.md` with a summary entry under `[Unreleased]`.

### License

By contributing, you agree that your contributions will be licensed under the same [PolyForm Noncommercial License 1.0.0](LICENSE) that covers the project.

---

## Português

Obrigado pelo seu interesse em contribuir com o AstroFind!

### Começando

1. Faça um fork do repositório e clone o seu fork.
2. Compile o projeto seguindo as instruções em [README.md](README.md).
3. Crie um novo branch para sua alteração: `git checkout -b feature/minha-mudanca`

### Pré-requisitos

```
Compilador C++ com suporte a C++23 (GCC 13+ ou Clang 17+)
Qt 6.4+ (Core, Widgets, Concurrent, Charts, Network, Keychain)
CMake 3.20+
cfitsio, Eigen3, fftw3, spdlog, nlohmann_json
CCfits (empacotado — extraído automaticamente de originals/CCfits.tar.gz)
libarchive (opcional — para suporte a TAR.GZ/BZ2/XZ/7Z/RAR)
```

### Estrutura do Projeto

| Diretório | Conteúdo |
|-----------|----------|
| `src/core/` | Lógica de negócio — sem dependências de UI Qt |
| `src/ui/` | Widgets Qt, diálogos, painéis — construídos totalmente em código (sem arquivos `.ui`) |
| `i18n/` | Arquivos Qt Linguist (`.ts` fonte, `.qm` compilado) |
| `resources/` | Ícones (programáticos), HTML de ajuda, manifesto QRC |
| `tests/` | 116 testes unitários de core + 23 testes de integração de UI |
| `cmake/` | Módulos Find e detecção de dependências opcionais |
| `originals/` | Arquivos de bibliotecas empacotadas (CCfits) |

### Estilo de Código

- **C++23**, idiomas Qt6. Sem arquivos `.ui` — todos os widgets construídos em código.
- Lógica central em `src/core/` não deve ter **dependências de UI Qt**.
- Ícones desenhados com `QPainter` em `src/ui/AppIcons.cpp`.
- Chaves de configurações seguem o padrão de namespace: `observer/*`, `camera/*`, `astrometry/*`, `catalog/*`, `photometry/*`, `report/*`, `ui/*`, `display/*`.
- Sem modificações na barra de ferramentas — novos recursos vão em **menus, menus de contexto, docks ou atalhos de teclado**.
- Use `std::expected<T, QString>` para operações falíveis que retornam erros ao chamador.
- Dependências opcionais protegidas por flags de compilação `#ifdef ASTROFIND_HAS_<FEATURE>`.

### Adicionando Novos Arquivos Fonte

- Arquivos core: adicionar em `src/core/CMakeLists.txt` em `CORE_SOURCES` / `CORE_HEADERS`.
- Arquivos UI: adicionar em `src/ui/CMakeLists.txt` em `UI_SOURCES` / `UI_HEADERS`.
- Novos módulos Qt: adicionar em `target_link_libraries` no `CMakeLists.txt` relevante.

### Adicionando Strings de UI

Sempre envolva texto visível ao usuário com `tr()`. Após adicionar novas strings:

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```

Preencha as tags `<translation>` em `astrofind_pt_BR.ts` antes de enviar.

### Executando Testes

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
# ou diretamente:
build/bin/astrofind_tests      # 116 testes core
build/bin/astrofind_ui_tests   # 23 testes de integração UI
```

Três falhas pré-existentes em `astrofind_tests` são conhecidas e rastreadas (testes dependentes de rede ignorados em CI).

### Enviando Alterações

- Abra um pull request com uma descrição clara da alteração e por que ela é necessária.
- Inclua capturas de tela para alterações de UI.
- Todo código novo deve ter testes correspondentes em `tests/`.
- Atualize `CHANGELOG.md` com uma entrada de resumo em `[Não lançado]`.

### Licença

Ao contribuir, você concorda que suas contribuições serão licenciadas sob a mesma [PolyForm Noncommercial License 1.0.0](LICENSE) que cobre o projeto.
