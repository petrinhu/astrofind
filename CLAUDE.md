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
