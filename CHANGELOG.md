# Changelog

**English** | [Português](#português)

All notable changes to AstroFind are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## English

### [0.5.0] — 2026-03-21

**Phase 4 — Advanced rendering, formats, WCS pipeline, detection algorithms, import/export, quality audit.**

#### Added — Display & Rendering
- Transfer functions: Linear, Square Root, Asinh, Logarithmic, Histogram Equalization
- Transfer function selector integrated in BackgroundRangeDialog
- False color / LUT presets: Hot, Cool, Viridis, Grayscale
- NaN pixel highlight in a distinct color overlay
- Laplacian / Unsharp-mask sharpening toggle in BlinkWidget (Off / USM / LoG)
- 2D power spectrum dialog (FFT with Hann window, log₁₊ scale, frequency tooltip)
- Pixel histogram dialog with Gaussian fit overlay

#### Added — Image Formats
- Color FITS support via NAXIS3 = 3 (RGB planes)
- Color FITS via multi-extension HDUs (3 separate image extensions)
- HDU navigator for multi-extension FITS files
- SER, XISF, TIFF 16-bit, PNG 16-bit, RAW DSLR (libraw), PDS format support
- FITS data cube animation — NAXIS3 > 3 planes played as blink animation (MDI sub-window)
- 1D spectrum plot dialog for FITS NAXIS=1 spectra (Qt6::Charts, dark theme, WCS axis)
- libarchive support: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR (optional dependency)

#### Added — Astrometry & WCS
- Full WCS projection pipeline beyond TAN: SIN, ARC, GLS/SFL, MER, AIT, STG (Calabretta & Greisen 2002)
- Atmospheric refraction correction applied before plate-solve and ADES export
- ICRS → CIRS → topocentric coordinate chain; `<sys>ICRF</sys>` in XML+PSV output
- Annual aberration, precession J2000→date and nutation applied
- Ecliptic plane overlay + Galactic plane proximity warning

#### Added — Detection & Measurement
- Elliptical PSF fitting with Levenberg-Marquardt solver (FWHM, coma, tracking quality)
- Bad pixel mask with nearest-neighbor interpolation
- Streak/trail detection by blob elongation analysis
- ClumpFind algorithm for blended sources
- Background median sliding filter (tile grid, sigma-clipping)

#### Added — Import / Export
- Import DAOPHOT/SExtractor FITS BINTABLE detected-star catalogs
- Import local offline catalogs from FITS BINTABLE (USNO-B/UCAC/Gaia-style; cone filter)
- Import reduction tables from IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/ycentroid)
- Settings → Connections: catalog source selector (VizieR / Local FITS)

#### Added — Infrastructure & Quality
- CCfits 2.7 bundled source; `FitsTableReader` built on CCfits BINTABLE API
- Numerical audit targets: `cmake --build build --target audit` (cppcheck, clang-tidy, Valgrind, ASan+UBSan)
- 4 bugs fixed by audit: FFTW r2c buffer size for non-square images, strict aliasing in ImageLoader, `Qt::UTC` deprecation, missing automatic-move optimization
- `INSTALL.md` with per-distro dependency installation instructions (10 distros)

#### Fixed
- B&W vs Color auto-detection warning for unexpected image type
- `QImage::flipped()` replaced with `mirrored()` for Qt 6.4 compatibility

---

### [0.4.0] — 2026-03-01

**Phase 3 — Complete science workflow: school UX, i18n, photometry, calibration, MOD, ADES reporting.**

#### Added — Interface & UX
- Full bilingual interface: English and Brazilian Portuguese (1007 strings)
- School workflow: step-by-step guided mode for students
- `Toast` notification widget (non-blocking: success/info/warning/error)
- Day/Night/Auto UI theme toggle (Ctrl+Shift+T)
- Export/Import School Configuration (File menu)
- "Send to Teacher" button in Report Preview (mailto:)
- School email field in Settings → Observer tab
- Dynamic WCS status in workflow panel

#### Added — Help & Documentation
- Built-in bilingual help (HTML) with FAQ and "For Students" sections
- AboutDialog: PolyForm license link; reference implementations with GitHub links

#### Added — Reporting & Submission
- ADES 2022 XML and PSV report generation with `<sys>ICRF</sys>` annotation
- MPC report preview dialog with HTML rendering
- MPC direct submission endpoint
- Solar System body identification via IMCCE SkyBoT
- JPL Horizons ephemeris query dialog

#### Added — Measurement
- Centroid engine: intensity-weighted moments + Gaussian PSF fitting
- Aperture photometry: differential, zero-point calibration, airmass correction
- Light curve dialog and growth curve dialog
- Region statistics panel (mean, median, σ, min/max)
- VerificationDialog: star match residuals and known-object check

#### Added — Calibration & Stacking
- Bias/dark/flat calibration pipeline
- Calibration wizard (step-by-step)
- Master dark / master flat builder dialog
- Image stacker: Add, Average, Median, Sigma-clipped; sub-pixel FFT registration
- Track & Stack with asteroid-tracking alignment

#### Added — Moving Object Detection
- Moving Object Detector across ≥3 images with tracklet linking

#### Added — Plate Solving
- ASTAP offline plate solver support

#### Added — Catalogs
- MPCORB offline catalog (KOO engine)
- Observatory preset database (MPC codes)

---

### [0.3.0] — 2026-02-01

**Phase 2 — Online astrometry: star detection, plate solving, catalog overlay, known-object engine.**

#### Added
- Automated star detection via SEP (Source Extractor C library)
- Online plate solving via astrometry.net REST API
- WCS TAN projection pipeline; WCS solution written back to FITS header
- VizieR star catalog overlay (UCAC4, Gaia DR3) with SQLite local cache
- Known Object Overlay (KOO) engine via IMCCE SkyBoT
- Session comparison dialog

---

### [0.2.0] — 2025-12-01

**Phase 1 — Blink comparison workflow and image navigation.**

#### Added
- Multi-image blink engine (BlinkWidget) with configurable interval
- Thumbnail navigator bar for rapid frame selection
- Background/range dialog for interactive display adjustment
- Log panel (scrollable info/warning/error messages)
- B&W vs Color image type warning with "Do not show again" checkbox

---

### [0.1.0] — 2025-11-01

**Phase 0 — Qt6/C++ skeleton with FITS image viewer.**

#### Added
- Qt6 application skeleton with MDI sub-window layout
- FITS file loading (cfitsio): single HDU, primary image
- FITS image viewer: zoom, pan, invert, flip horizontal/vertical
- Bilingual menu structure (English / Brazilian Portuguese stubs)
- CMake build system (Qt6, cfitsio, spdlog, nlohmann/json via FetchContent)
- PolyForm Noncommercial 1.0.0 license

---

## Português

### [0.5.0] — 2026-03-21

**Fase 4 — Renderização avançada, formatos, pipeline WCS, algoritmos de detecção, importação/exportação, auditoria de qualidade.**

#### Adicionado — Exibição e Renderização
- Funções de transferência: Linear, Raiz Quadrada, Asinh, Logarítmica, Equalização de Histograma
- Seletor de função de transferência integrado ao BackgroundRangeDialog
- Presets de cor falsa / LUT: Hot, Cool, Viridis, Escala de Cinza
- Destaque de pixels NaN em cor distinta
- Alternância de nitidez Laplaciano / Unsharp-mask no BlinkWidget (Off / USM / LoG)
- Diálogo de espectro de potência 2D (FFT com janela Hann, escala log₁₊, tooltip de frequência)
- Diálogo de histograma de pixels com ajuste gaussiano

#### Adicionado — Formatos de Imagem
- Suporte a FITS colorido via NAXIS3 = 3 (planos RGB)
- FITS colorido via multi-extensão HDU (3 extensões de imagem separadas)
- Navegador de HDUs para arquivos FITS multi-extensão
- Suporte a SER, XISF, TIFF 16-bit, PNG 16-bit, RAW DSLR (libraw), PDS
- Animação de cubo de dados FITS — NAXIS3 > 3 planos reproduzidos como animação blink
- Diálogo de espectro 1D para FITS NAXIS=1 (Qt6::Charts, tema escuro, eixo WCS)
- Suporte a arquivos compactados via libarchive: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR (opcional)

#### Adicionado — Astrometria e WCS
- Pipeline completo de projeção WCS além de TAN: SIN, ARC, GLS/SFL, MER, AIT, STG (Calabretta & Greisen 2002)
- Correção de refração atmosférica aplicada antes do plate-solve e exportação ADES
- Cadeia ICRS → CIRS → topocêntrico; `<sys>ICRF</sys>` em XML+PSV
- Aberração anual, precessão J2000→data e nutação aplicadas
- Overlay do plano eclíptico + aviso de proximidade do plano galáctico

#### Adicionado — Detecção e Medição
- Ajuste de PSF elíptica com solver Levenberg-Marquardt (FWHM, coma, qualidade de tracking)
- Máscara de pixels ruins com interpolação por vizinhos mais próximos
- Detecção de traços/rastros por análise de elongação de blob
- Algoritmo ClumpFind para fontes blendadas
- Filtro mediano deslizante de fundo (grade de tiles, sigma-clipping)

#### Adicionado — Importação / Exportação
- Importar catálogos FITS BINTABLE de estrelas detectadas DAOPHOT/SExtractor
- Importar catálogos offline locais de FITS BINTABLE (estilo USNO-B/UCAC/Gaia; filtro por cone)
- Importar tabelas de redução IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/ycentroid)
- Configurações → Conexões: seletor de fonte de catálogo (VizieR / FITS Local)

#### Adicionado — Infraestrutura e Qualidade
- CCfits 2.7 bundled; `FitsTableReader` construído sobre a API BINTABLE do CCfits
- Targets de auditoria numérica: `cmake --build build --target audit` (cppcheck, clang-tidy, Valgrind, ASan+UBSan)
- 4 bugs corrigidos pela auditoria: buffer FFTW r2c para imagens não quadradas, aliasing estrito no ImageLoader, deprecação de `Qt::UTC`, otimização de move automático ausente
- `INSTALL.md` com instruções de instalação de dependências por distribuição (10 distros)

#### Corrigido
- Aviso de detecção automática B&W vs Cor para tipo de imagem inesperado
- `QImage::flipped()` substituído por `mirrored()` para compatibilidade com Qt 6.4

---

### [0.4.0] — 2026-03-01

**Fase 3 — Fluxo científico completo: UX escolar, i18n, fotometria, calibração, MOD, relatórios ADES.**

#### Adicionado — Interface e UX
- Interface bilíngue completa: inglês e português brasileiro (1007 strings)
- Fluxo escolar: modo guiado passo a passo para alunos
- Widget de notificação `Toast` (não bloqueante: sucesso/info/aviso/erro)
- Alternância de tema Dia/Noite/Automático (Ctrl+Shift+T)
- Exportar/Importar Configuração da Escola (menu Arquivo)
- Botão "Enviar para Professor" na pré-visualização de relatório (mailto:)
- Campo de e-mail da escola em Configurações → aba Observador
- Status WCS dinâmico no painel de workflow

#### Adicionado — Ajuda e Documentação
- Ajuda integrada bilíngue (HTML) com seções FAQ e "Para Alunos"
- AboutDialog: link de licença PolyForm; implementações de referência com links do GitHub

#### Adicionado — Relatórios e Submissão
- Geração de relatório ADES 2022 XML e PSV com anotação `<sys>ICRF</sys>`
- Diálogo de pré-visualização de relatório MPC com renderização HTML
- Endpoint de submissão direta ao MPC
- Identificação de corpos do Sistema Solar via IMCCE SkyBoT
- Diálogo de consulta de efemérides JPL Horizons

#### Adicionado — Medição
- Motor de centroide: momentos ponderados por intensidade + ajuste de PSF gaussiana
- Fotometria de abertura: diferencial, calibração de zero-point, correção de massa de ar
- Diálogo de curva de luz e diálogo de curva de crescimento
- Painel de estatísticas de região (média, mediana, σ, mín/máx)
- VerificationDialog: resíduos de correspondência de estrelas e verificação de objetos conhecidos

#### Adicionado — Calibração e Empilhamento
- Pipeline de calibração bias/dark/flat
- Assistente de calibração (passo a passo)
- Diálogo construtor de master dark / master flat
- Empilhador de imagens: Adição, Média, Mediana, Sigma-clipped; registro sub-pixel por FFT
- Track & Stack com alinhamento por rastreamento de asteroide

#### Adicionado — Detecção de Objetos em Movimento
- Detector de Objetos em Movimento em ≥3 imagens com ligação de tracklets

#### Adicionado — Plate Solving
- Suporte ao plate solver offline ASTAP

#### Adicionado — Catálogos
- Catálogo offline MPCORB (motor KOO)
- Banco de dados de observatórios predefinidos (códigos MPC)

---

### [0.3.0] — 2026-02-01

**Fase 2 — Astrometria online: detecção de estrelas, plate solving, sobreposição de catálogo, motor de objetos conhecidos.**

#### Adicionado
- Detecção automática de estrelas via SEP (biblioteca C do Source Extractor)
- Plate solving online via API REST do astrometry.net
- Pipeline de projeção WCS TAN; solução WCS gravada de volta no cabeçalho FITS
- Sobreposição do catálogo de estrelas VizieR (UCAC4, Gaia DR3) com cache local SQLite
- Motor de Objetos Conhecidos (KOO) via IMCCE SkyBoT
- Diálogo de comparação de sessão

---

### [0.2.0] — 2025-12-01

**Fase 1 — Fluxo de comparação blink e navegação entre imagens.**

#### Adicionado
- Motor de blink multi-imagem (BlinkWidget) com intervalo configurável
- Barra de miniaturas para seleção rápida de frames
- Diálogo de fundo/alcance para ajuste interativo de exibição
- Painel de log (mensagens info/aviso/erro com rolagem)
- Aviso de tipo de imagem B&W vs Cor com checkbox "Não mostrar novamente"

---

### [0.1.0] — 2025-11-01

**Fase 0 — Esqueleto Qt6/C++ com visualizador de imagens FITS.**

#### Adicionado
- Esqueleto de aplicativo Qt6 com layout MDI de sub-janelas
- Carregamento de arquivos FITS (cfitsio): HDU único, imagem primária
- Visualizador de imagens FITS: zoom, pan, inverter, espelhar horizontal/vertical
- Estrutura de menus bilíngue (inglês / português brasileiro — stubs)
- Sistema de build CMake (Qt6, cfitsio, spdlog, nlohmann/json via FetchContent)
- Licença PolyForm Noncommercial 1.0.0
