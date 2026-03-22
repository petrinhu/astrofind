# Changelog

**English** | [Português](#português)

All notable changes to AstroFind are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## English

### [0.4.0] — 2026-03-21

#### Added — Display & Rendering
- B&W vs Color auto-detection with warning for unexpected image type
- Transfer functions: Linear, Square Root, Asinh, Logarithmic, Histogram Equalization
- Transfer function selector integrated in BackgroundRangeDialog
- False color / LUT presets: Hot, Viridis, Plasma, Inferno, Cividis, Turbo, Gray
- NaN pixel highlight in a distinct color overlay
- Laplacian / Unsharp-mask sharpening toggle in BlinkWidget (Off / USM / LoG)
- 2D power spectrum dialog (FFT with Hann window, log₁₊ scale, frequency tooltip)
- Pixel histogram dialog with Gaussian fit overlay

#### Added — Image Formats & Data Cubes
- Color FITS support via NAXIS3 = 3 (RGB planes)
- Color FITS via multi-extension HDUs (3 separate image extensions)
- HDU navigator for multi-extension FITS files
- Other format support: SER, XISF, TIFF, PNG (via Qt/CFITSIO)
- FITS data cube animation — NAXIS3 > 3 planes played in BlinkWidget MDI subwindow
- 1D spectrum plot dialog for FITS NAXIS=1 spectra (Qt6::Charts, dark theme, WCS axis)
- FITS NAXIS=3 cube animation accessible via context menu → "Animate Cube (N frames)…"
- libarchive support for archive formats: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR (optional dependency)

#### Added — Astrometry & WCS
- Atmospheric refraction correction applied before plate-solve and ADES export
- Full WCS projection pipeline beyond TAN: SIN, ARC, GLS, MER, AIT, STG (Calabretta & Greisen 2002)
- ICRS → CIRS → topocentric coordinate chain for ADES compliance; `<sys>ICRF</sys>` in XML+PSV output
- Annual aberration components and precession/nutation applied (sunEclipticPosition, applyPrecessionJ2000ToDate)
- Ecliptic plane overlay + Galactic plane proximity warning (source: Boost.Astronomy)

#### Added — Detection & Measurement
- Elliptical PSF fitting with Levenberg-Marquardt solver
- Bad pixel mask + bilinear interpolation for masked pixels
- Streak/trail detection by blob elongation analysis
- ClumpFind algorithm for blended sources — double-circle magenta overlay for blended detections
- Background median sliding filter (source: GDL medianfilter)

#### Added — Import / Export
- Import DAOPHOT/SExtractor FITS BINTABLE detected-star catalogs (X_IMAGE/XPOS/X_CENTER aliases)
- Import local offline catalogs from FITS BINTABLE (USNO-B/UCAC/Gaia-style; cone filter)
- Import reduction tables from IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/ycentroid; fills RA/Dec when sky coords present)
- Settings → Connections: catalog source selector (VizieR / Local FITS) with file browser

#### Added — Infrastructure
- CCfits library integrated from bundled source (CCfits.tar.gz); `FitsTableReader` built on CCfits BINTABLE API
- libarchive optional dependency: auto-detected via pkg-config, falls back to find_library; `ASTROFIND_HAS_LIBARCHIVE` compile flag

#### Changed
- All `tr()` string counts updated; run `lupdate-qt6` + `lrelease-qt6` after update

---

### [0.3.0] — 2026-03-21

#### Added
- Phase 3: school UX, toast notifications, complete i18n (1007 strings), help content
- PolyForm Noncommercial 1.0.0 license
- Full bilingual interface: English and Brazilian Portuguese
- School workflow: step-by-step guided mode for students
- `Toast` notification widget (non-blocking success/info/warning/error)
- FAQ and "For Students" sections in built-in help
- Export/Import School Configuration (File menu)
- "Send to Teacher" button in Report Preview (mailto:)
- School email field in Settings → Observer tab
- Dynamic WCS status in workflow panel
- ASTAP offline plate solver support
- MPC/ADES report preview dialog with HTML rendering
- Centroid and aperture photometry engine
- Moving object detector with blink-based tracklet linking
- Image stacker (mean/median/sigma-clipped)
- Calibration wizard (bias/dark/flat)
- MPCORB offline catalog (KOO engine)
- Observatory preset database (MPC codes)
- Day/Night/Auto UI theme toggle (Ctrl+Shift+T)
- Thumbnail bar for multi-image navigation
- Background/range dialog for display adjustment

#### Changed
- AboutDialog: copyright updated; license link opens PolyForm dialog; reference implementations have GitHub links

---

### [0.2.0] — 2026-02

#### Added
- Phase 2: star detection, plate solving (astrometry.net), VizieR catalog overlay, KOO engine

---

### [0.1.0] — 2025-12

#### Added
- Phase 1: blink engine, thumbnail bar, background/range dialog

---

### [0.0.1] — 2025-11

#### Added
- Phase 0: working Qt6/C++ skeleton with FITS image viewer

---

## Português

### [0.4.0] — 2026-03-21

#### Adicionado — Exibição e Renderização
- Detecção automática de B&W vs Color com aviso para tipo de imagem inesperado
- Funções de transferência: Linear, Raiz Quadrada, Asinh, Logarítmica, Equalização de Histograma
- Seletor de função de transferência integrado ao BackgroundRangeDialog
- Presets de cor falsa / LUT: Hot, Viridis, Plasma, Inferno, Cividis, Turbo, Gray
- Destaque de pixels NaN em cor distinta no overlay
- Nitidez via Laplaciano / Unsharp-mask no BlinkWidget (Off / USM / LoG)
- Diálogo de espectro de potência 2D (FFT com janela Hann, escala log₁₊, tooltip de frequência)
- Diálogo de histograma de pixels com ajuste gaussiano

#### Adicionado — Formatos de Imagem e Cubos de Dados
- Suporte a FITS colorido via NAXIS3 = 3 (planos RGB)
- FITS colorido via multi-extensão HDU (3 extensões de imagem separadas)
- Navegador de HDUs para arquivos FITS multi-extensão
- Suporte a outros formatos: SER, XISF, TIFF, PNG
- Animação de cubo de dados FITS — NAXIS3 > 3 planos reproduzidos em BlinkWidget MDI
- Diálogo de espectro 1D para FITS NAXIS=1 (Qt6::Charts, tema escuro, eixo WCS)
- Animação de cubo FITS acessível via menu de contexto → "Animate Cube (N frames)…"
- Suporte a arquivos compactados via libarchive: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR (dependência opcional)

#### Adicionado — Astrometria e WCS
- Correção de refração atmosférica aplicada antes do plate-solve e exportação ADES
- Pipeline completo de projeção WCS além de TAN: SIN, ARC, GLS, MER, AIT, STG (Calabretta & Greisen 2002)
- Cadeia de coordenadas ICRS → CIRS → topocêntrico para conformidade ADES; `<sys>ICRF</sys>` em XML+PSV
- Aberração anual e precessão/nutação aplicadas (sunEclipticPosition, applyPrecessionJ2000ToDate)
- Overlay do plano eclíptico + aviso de proximidade do plano galáctico (fonte: Boost.Astronomy)

#### Adicionado — Detecção e Medição
- Ajuste de PSF elíptica com solver Levenberg-Marquardt
- Máscara de pixels ruins + interpolação bilinear para pixels mascarados
- Detecção de traços/rastros por análise de elongação de blob
- Algoritmo ClumpFind para fontes blendadas — overlay de círculo duplo magenta para detecções blendadas
- Filtro mediano deslizante de fundo (fonte: GDL medianfilter)

#### Adicionado — Importação / Exportação
- Importar catálogos FITS BINTABLE de estrelas detectadas DAOPHOT/SExtractor (aliases X_IMAGE/XPOS/X_CENTER)
- Importar catálogos offline locais de FITS BINTABLE (estilo USNO-B/UCAC/Gaia; filtro por cone)
- Importar tabelas de redução IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/ycentroid; preenche AR/Dec quando coordenadas de céu presentes)
- Configurações → Conexões: seletor de fonte de catálogo (VizieR / FITS Local) com navegador de arquivos

#### Adicionado — Infraestrutura
- Biblioteca CCfits integrada a partir do código-fonte empacotado (CCfits.tar.gz); `FitsTableReader` construído sobre a API BINTABLE do CCfits
- Dependência opcional libarchive: detectada automaticamente via pkg-config, com fallback para find_library; flag de compilação `ASTROFIND_HAS_LIBARCHIVE`

#### Alterado
- Contagens de strings `tr()` atualizadas; execute `lupdate-qt6` + `lrelease-qt6` após atualização

---

### [0.3.0] — 2026-03-21

#### Adicionado
- Fase 3: UX escolar, notificações toast, i18n completo (1007 strings), conteúdo de ajuda
- Licença PolyForm Noncommercial 1.0.0
- Interface bilíngue completa: inglês e português brasileiro
- Fluxo escolar: modo guiado passo a passo para alunos
- Widget de notificação `Toast` (não bloqueante: sucesso/info/aviso/erro)
- Seções FAQ e "Para Alunos" na ajuda integrada
- Exportar/Importar Configuração da Escola (menu Arquivo)
- Botão "Enviar para Professor" na Pré-visualização de Relatório (mailto:)
- Campo de e-mail da escola em Configurações → aba Observador
- Status WCS dinâmico no painel de workflow
- Suporte ao plate solver offline ASTAP
- Diálogo de pré-visualização de relatório MPC/ADES com renderização HTML
- Motor de centroide e fotometria de abertura
- Detector de objetos em movimento com ligação de tracklets baseada em blink
- Empilhador de imagens (média/mediana/sigma-clipped)
- Assistente de calibração (bias/dark/flat)
- Catálogo offline MPCORB (motor KOO)
- Banco de dados de observatórios predefinidos (códigos MPC)
- Alternância de tema de UI Dia/Noite/Automático (Ctrl+Shift+T)
- Barra de miniaturas para navegação entre múltiplas imagens
- Diálogo de fundo/alcance para ajuste de exibição

#### Alterado
- AboutDialog: copyright atualizado; link de licença abre diálogo PolyForm; implementações de referência têm links do GitHub

---

### [0.2.0] — 2026-02

#### Adicionado
- Fase 2: detecção de estrelas, plate solving (astrometry.net), sobreposição do catálogo VizieR, motor KOO

---

### [0.1.0] — 2025-12

#### Adicionado
- Fase 1: motor de blink, barra de miniaturas, diálogo de fundo/alcance

---

### [0.0.1] — 2025-11

#### Adicionado
- Fase 0: esqueleto Qt6/C++ funcional com visualizador de imagens FITS
