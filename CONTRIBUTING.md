# Contributing to AstroFind

**English** | [Português](#português)

---

## English

Thank you for your interest in contributing to AstroFind!

### Getting Started

1. Fork the repository and clone your fork.
2. Build the project following the instructions in [README.md](README.md).
3. Create a new branch for your change: `git checkout -b feature/my-change`

### Code Style

- C++17, Qt6 idioms. No `.ui` files — all widgets built in code.
- Core logic in `src/core/` must have no Qt UI dependencies.
- Icons drawn with `QPainter` in `src/ui/AppIcons.cpp`.
- Settings keys follow the namespace pattern: `observer/*`, `camera/*`, `astrometry/*`, etc.
- No toolbar modifications — new features go in menus, docks, or keyboard shortcuts.

### Adding UI Strings

Always wrap user-visible text with `tr()`. After adding new strings:

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```

Fill in `<translation>` tags in `astrofind_pt_BR.ts` before submitting.

### Submitting Changes

- Open a pull request with a clear description of the change and why it's needed.
- Include screenshots for UI changes.
- All new code should have corresponding tests in `tests/`.

### License

By contributing, you agree that your contributions will be licensed under the same [PolyForm Noncommercial License 1.0.0](LICENSE) that covers the project.

---

## Português

Obrigado pelo seu interesse em contribuir com o AstroFind!

### Começando

1. Faça um fork do repositório e clone o seu fork.
2. Compile o projeto seguindo as instruções em [README.md](README.md).
3. Crie um novo branch para sua alteração: `git checkout -b feature/minha-mudanca`

### Estilo de Código

- C++17, idiomas Qt6. Sem arquivos `.ui` — todos os widgets construídos em código.
- Lógica central em `src/core/` não deve ter dependências de UI Qt.
- Ícones desenhados com `QPainter` em `src/ui/AppIcons.cpp`.
- Chaves de configurações seguem o padrão de namespace: `observer/*`, `camera/*`, `astrometry/*`, etc.
- Sem modificações na barra de ferramentas — novos recursos vão em menus, docks ou atalhos de teclado.

### Adicionando Strings de UI

Sempre envolva texto visível ao usuário com `tr()`. Após adicionar novas strings:

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```

Preencha as tags `<translation>` em `astrofind_pt_BR.ts` antes de enviar.

### Enviando Alterações

- Abra um pull request com uma descrição clara da alteração e por que ela é necessária.
- Inclua capturas de tela para alterações de UI.
- Todo código novo deve ter testes correspondentes em `tests/`.

### Licença

Ao contribuir, você concorda que suas contribuições serão licenciadas sob a mesma [PolyForm Noncommercial License 1.0.0](LICENSE) que cobre o projeto.
