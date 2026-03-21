# Changelog

**English** | [Português](#português)

All notable changes to AstroFind are documented here.  
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## English

### [Unreleased]

#### Added
- PolyForm Noncommercial 1.0.0 license (replaces MIT)
- Full bilingual interface: English and Brazilian Portuguese (1007 strings)
- School workflow: step-by-step guided mode for students
- `Toast` notification widget — non-blocking success/info/warning/error toasts
- FAQ and "For Students" sections in built-in help (both languages)
- Export/Import School Configuration (File menu)
- "Send to Teacher" button in Report Preview (uses mailto:)
- School email field in Settings → Observer tab
- Dynamic WCS status in workflow panel (shows plate-solve progress per image)
- ASTAP offline plate solver support
- MPC/ADES report preview dialog with HTML rendering
- Centroid and aperture photometry engine
- Moving object detector with blink-based tracklet linking
- Image stacker (mean/median/sigma-clipped)
- Calibration wizard (bias/dark/flat)
- MPCORB offline catalog support (KOO engine)
- Observatory preset database (MPC codes)
- Day/Night/Auto UI theme toggle (Ctrl+Shift+T)
- Thumbnail bar for multi-image navigation
- Background/range dialog for display adjustment

#### Changed
- AboutDialog: updated copyright to Petrus Silva Costa; license link opens PolyForm dialog; reference implementations now have GitHub links

### [0.3.0] — 2026-03-21

#### Added
- Phase 3: school UX, toast notifications, complete i18n, help content

### [0.2.0] — 2026-02

#### Added
- Phase 2: star detection, plate solving (astrometry.net), VizieR catalog overlay, KOO engine

### [0.1.0] — 2025-12

#### Added
- Phase 1: blink engine, thumbnail bar, background/range dialog

### [0.0.1] — 2025-11

#### Added
- Phase 0: working Qt6/C++ skeleton with FITS image viewer

---

## Português

### [Não lançado]

#### Adicionado
- Licença PolyForm Noncommercial 1.0.0 (substitui MIT)
- Interface bilíngue completa: inglês e português brasileiro (1007 strings)
- Fluxo escolar: modo guiado passo a passo para alunos
- Widget de notificação `Toast` — toasts não bloqueantes de sucesso/info/aviso/erro
- Seções FAQ e "Para Alunos" na ajuda integrada (ambos os idiomas)
- Exportar/Importar Configuração da Escola (menu Arquivo)
- Botão "Enviar para Professor" na Pré-visualização de Relatório (usa mailto:)
- Campo de e-mail da escola em Configurações → aba Observador
- Status WCS dinâmico no painel de workflow (mostra progresso do plate-solve por imagem)
- Suporte ao plate solver offline ASTAP
- Diálogo de pré-visualização de relatório MPC/ADES com renderização HTML
- Motor de centroide e fotometria de abertura
- Detector de objetos em movimento com ligação de tracklets baseada em blink
- Empilhador de imagens (média/mediana/sigma-clipped)
- Assistente de calibração (bias/dark/flat)
- Suporte ao catálogo offline MPCORB (motor KOO)
- Banco de dados de observatórios predefinidos (códigos MPC)
- Alternância de tema de UI Dia/Noite/Automático (Ctrl+Shift+T)
- Barra de miniaturas para navegação entre múltiplas imagens
- Diálogo de fundo/alcance para ajuste de exibição

#### Alterado
- AboutDialog: copyright atualizado para Petrus Silva Costa; link de licença abre diálogo PolyForm; implementações de referência agora têm links do GitHub

### [0.3.0] — 2026-03-21

#### Adicionado
- Fase 3: UX escolar, notificações toast, i18n completo, conteúdo de ajuda

### [0.2.0] — 2026-02

#### Adicionado
- Fase 2: detecção de estrelas, plate solving (astrometry.net), sobreposição do catálogo VizieR, motor KOO

### [0.1.0] — 2025-12

#### Adicionado
- Fase 1: motor de blink, barra de miniaturas, diálogo de fundo/alcance

### [0.0.1] — 2025-11

#### Adicionado
- Fase 0: esqueleto Qt6/C++ funcional com visualizador de imagens FITS
