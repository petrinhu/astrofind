# AstroFind — Claude Code Instructions

## MCP Tools (always use)

### context7
Always use context7 when referencing Qt6, CMake, or any external library API.
Before answering questions about Qt classes, methods, or signals/slots, fetch the current docs via context7.

### superpowers-mcp
For any non-trivial implementation task, use `recommend_skills` to check if a relevant skill exists.
For debugging, use the `debugging` skill. For new features, use the `tdd` or `brainstorm` skill as appropriate.

## Project

AstroFind is a Qt6/C++ astrometry application for asteroid detection and measurement.
Stack: Qt6, CMake, cfitsio, Eigen3, fftw3.
Language: C++ (src/) with Qt translations (i18n/astrofind_pt_BR.ts → .qm).

## Conventions

- UI files: `src/ui/` — Qt widgets, no .ui files (all code-built)
- Core logic: `src/core/` — no Qt UI dependencies
- Icons: drawn programmatically via QPainter in `AppIcons.cpp`
- Translations: run `lupdate-qt6` then `lrelease-qt6` after adding `tr()` strings
- Settings keys: `observer/*`, `camera/*`, `astrometry/*`, `catalog/*`, `photometry/*`, `report/*`, `ui/*`, `display/*`
- No toolbar modifications — features added via menus, context menus, docks, keyboard shortcuts

## Pending Tasks Table

Whenever tasks from the table below are pending, **show the full updated table at the end of every completed task response**, with:
- ✅ for completed items
- ❌ for pending items

The table must be shown automatically — do not wait for the user to ask.

| # | Feature | Categoria | Prioridade | Dificuldade | Status |
|---|---------|-----------|:----------:|:-----------:|:------:|
| 1 | Aviso B&W vs Color ao carregar (checkbox "Mostrar sempre" + Settings) | UX | Alta | D1 | ❌ |
| 2 | Transfer function: Linear (seletor de modo) | Renderização | Alta | D1 | ❌ |
| 3 | Transfer function: Logarítmica | Renderização | Alta | D2 | ❌ |
| 4 | Transfer function: Raiz Quadrada | Renderização | Alta | D1 | ❌ |
| 5 | Transfer function: Asinh | Renderização | Alta | D1 | ❌ |
| 6 | Seletor de transfer function no BackgroundRangeDialog | UX | Alta | D2 | ❌ |
| 7 | Suporte a imagens coloridas NAXIS3=3 | Core | Alta | D3 | ❌ |
| 8 | Suporte a cor via multi-extensão (3 HDUs) | Core | Média | D3 | ❌ |
| 9 | Renderização de pixels NaN/indefinidos em cor distinta | Renderização | Média | D2 | ❌ |
| 10 | False color / LUT (Hot, Cool, Viridis, Grayscale) | Renderização | Média | D2 | ❌ |
| 11 | Transfer function: Equalização de histograma | Renderização | Média | D2 | ❌ |
| 12 | Navegação de extensões HDU (painel lateral) | Core/UI | Média | D3 | ❌ |
| 13 | Cubo FITS NAXIS=3 — animação temporal | Avançado | Baixa | D4 | ❌ |
| 14 | Plot de espectro NAXIS=1 (Qt6::Charts) | Avançado | Baixa | D4 | ❌ |
| 15 | Integração CCfits (bundled em ./originals) | Infraestrutura | Baixa | D3 | ❌ |
| 16 | Atualizar Help (PT-BR e EN) | Documentação | Alta | D2 | ❌ |
| 17 | Atualizar traduções (.ts → .qm, PT-BR e EN) | Documentação | Alta | D1 | ❌ |
| 18 | Manual resumido (com links navegáveis) | Documentação | Média | D2 | ❌ |
| 19 | Manual extenso (com links navegáveis) | Documentação | Média | D3 | ❌ |
| 20 | HowTo passo a passo de todas as funções (com links navegáveis) | Documentação | Média | D3 | ❌ |
| 21 | Suporte a outros formatos astronômicos: SER (.ser), XISF (.xisf), TIFF 16-bit (.tif), PNG 16-bit (.png), RAW DSLR (.cr2/.nef/.arw via libraw), PDS/PDS4 (.img+.lbl) | Core/IO | Média | D3 | ❌ |
| 22 | Auditoria numérica: verificar todos os tipos numéricos (float/int overflow, div/0, UB, cast inseguro, stack overflow em recursão) | Qualidade | P0 | D3 | ❌ |
