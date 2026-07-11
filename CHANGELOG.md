# Changelog

> **Last reviewed / Última revisão:** 2026-07-10
> **Owner:** Petrus Silva Costa
> Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

All notable changes to AstroFind are documented here, most recent version first. /
Todas as mudanças notáveis do AstroFind estão documentadas aqui, versão mais recente
primeiro.

---

## [0.9.0] - 2026-07-10

### 🇬🇧 English

**Stable release, first release after a full internal security/robustness/correctness
audit. Recommended over 0.9.0-beta for all users.**

A dedicated audit pass ("audit": a systematic review looking for bugs, security
issues and mismatches between docs and code) found and fixed **29 issues** across
input-handling robustness, memory safety, astrometric correctness, and dependency
security. The full write-up, with reproduction steps and remediation commits for
every item, is in [`AUDIT_FIND.md`](AUDIT_FIND.md).

#### Security

- `.runner` (a live Forgejo CI runner token accidentally left on disk) removed from
  tracking and the token rotated.
- All `FetchContent` dependencies (6 libraries fetched at build time) pinned to an
  exact commit SHA instead of a floating branch/tag, closing a supply-chain
  ("supply-chain attack": tampering with a dependency you pull in, instead of
  attacking your own code directly) exposure window.
- HTTP clients (astrometry.net, VizieR, JPL Horizons, SkyBoT) now enforce
  `https://` (or localhost-only `http://`) and have a request timeout + cancel path,
  closing a hang/DoS risk on a slow or malicious server.
- The plain-text API key fallback file (used only when Qt Keychain is unavailable)
  is now created with `chmod 600` permissions (owner read/write only).

#### Fixed - Crash and robustness (attacker-controlled files)

- FITS files with `NAXIS > 3` are now rejected before any cfitsio pixel read
  (previously: stack-buffer-overflow inside cfitsio).
- FITS image dimensions are validated against a ceiling and the real file size
  before any pixel-count multiplication (previously: a 2.9 KB file with a lying
  header could trigger a ~40 GB allocation attempt and an uncaught exception).
  Also fixed: a `NAXIS3` overflow bypass discovered in review of the first fix.
- SER video files reject width/height values with the sign bit set, before the
  narrowing cast to `int`.
- Archive extraction (ZIP/TAR/7Z/RAR) now rejects symlink, FIFO, and device
  entries, preventing a maliciously crafted archive from writing outside the
  extraction directory.
- BINTABLE (FITS binary table) column reads now propagate real errors instead of
  silently fabricating `NaN` rows, and out-of-bounds reads on optional columns are
  guarded.
- Centroid measurement (the pixel-position refinement step) now guards against
  non-finite (`NaN`/`Inf`) input before any `static_cast<int>` (previously: a bad
  pixel could reach `INT_MIN` and abort via signed-overflow), and photometry
  rejects a `NaN` net flux instead of reporting it as a successful measurement.
- Fixed the upstream SEP (Source Extractor library) double-allocation leak via a
  local patch applied at `FetchContent` time.

#### Fixed - Astrometric correctness

- **Non-zenithal WCS projections (CAR/MER/GLS/AIT) reported a celestial coordinate
  off by roughly 90 degrees with RA/Dec swapped.** This was the most severe finding:
  the wrong native-pole derivation fed straight into the ADES/MPC report for any
  field using one of those four projections. Fixed and validated against `astropy`
  (168/168 test cases) as an external, independent reference.
- Added a value-absolute (not just round-trip) test oracle for all 8 supported WCS
  projections, so a future regression like the one above is caught immediately.
- Added a refraction-correction test suite with known geometry, and confirmed the
  `isSpaceTelescope` gate correctly skips the correction for space telescopes.
- Removed `applyPrecessionJ2000ToDate()` and `applyNutation()`: dead code with zero
  callers anywhere in the pipeline (never actually wired in, see
  [`docs/technical-reference.md`](docs/technical-reference.md) §6/§8 for why the
  WCS plate-solve approach makes them unnecessary). Added oracle tests against
  aberration/precession/nutation/ecliptic reference values.
- Replaced circular photometry test assertions (which compared the code's output
  to itself) with an independent oracle using a known flux and zero-point.
- Centroid FWHM conversion now uses the precise constant
  `2.354820045` (`2√(2 ln 2)`) instead of a truncated literal.

#### Changed

- **License migrated from PolyForm Noncommercial 1.0.0 to the GNU Affero General
  Public License v3.0 (AGPL-3.0):** strong copyleft; commercial use is allowed but
  distributing a modified version (including running it as a network service)
  requires releasing the source code.
- Added SPDX license headers to all 127 source files under `src/` (REUSE
  compliance, a standard way to machine-check every file's license).
- Added a `NOTICE` file consolidating third-party attribution and copyright for
  every bundled/fetched dependency (CCfits, SEP, QuaZip, spdlog, nlohmann/json,
  FFTW3, cfitsio); README now shows a license column per dependency.
- Corrected the `docs/technical-reference.md` atmospheric-refraction section,
  which previously described a two-branch model that did not match the actual
  implementation (a single, unconditional Bennett-1982 formula).
- `README.md`'s GDL/NEMO/Siril reference-implementation credits reworded to make
  clear they are learning references, not code taken from those projects.

#### Infrastructure

- Added `SECURITY.md` with a responsible-disclosure contact.
- Added a `pull_request -> main` / release-tag-only CI workflow
  (`.github/workflows/audit.yml`) running the full numerical audit (ASan/UBSan,
  cppcheck, clang-tidy, Valgrind) plus a fast local `pre-commit` git hook that
  builds incrementally and runs `ctest` before every commit.
- Removed the orphaned `qa-wsl2.yml` CI workflow (WSL2 is no longer a supported
  target; all remaining distro QA items are complete, see
  [`docs/qa-distros.md`](docs/qa-distros.md)).
- Renormalized CRLF line endings to LF across all CI workflow files.
- Replaced 10 phantom-`SKIP` core tests with real, versioned synthetic FITS
  fixtures on disk.

#### Test suite

- ASan/UBSan sanitizer test run grew from **4426 to 5480 assertions** across
  **159 cases**, all green, including the new `[bintable]` and `[wcs]` oracle
  suites, with `detect_leaks=1` enabled.

### 🇧🇷 Português

**Versão estável, primeira versão após uma auditoria interna completa de
segurança/robustez/corretude. Recomendada sobre a 0.9.0-beta para todos os usuários.**

Uma passada de auditoria dedicada ("audit": uma revisão sistemática buscando bugs,
problemas de segurança e divergências entre a documentação e o código) encontrou e
corrigiu **29 problemas** em robustez de tratamento de entrada, segurança de memória,
corretude astrométrica e segurança de dependências. O relatório completo, com passos
de reprodução e commits de correção de cada item, está em
[`AUDIT_FIND.md`](AUDIT_FIND.md).

#### Segurança

- `.runner` (um token vivo do runner de CI do Forgejo deixado por acidente no disco)
  removido do rastreamento e o token foi rotacionado.
- Todas as dependências via `FetchContent` (6 bibliotecas baixadas em tempo de
  build) foram fixadas num commit SHA exato em vez de uma branch/tag flutuante,
  fechando uma janela de exposição de supply-chain ("ataque de supply-chain":
  adulterar uma dependência que você importa, em vez de atacar seu próprio código
  diretamente).
- Os clientes HTTP (astrometry.net, VizieR, JPL Horizons, SkyBoT) agora exigem
  `https://` (ou `http://` restrito a localhost) e têm timeout de requisição +
  cancelamento, fechando um risco de travamento/DoS num servidor lento ou malicioso.
- O arquivo de fallback de chave de API em texto plano (usado só quando o Qt
  Keychain não está disponível) agora é criado com permissões `chmod 600`
  (leitura/escrita só para o dono).

#### Corrigido - Crash e robustez (arquivos controlados por atacante)

- Arquivos FITS com `NAXIS > 3` agora são rejeitados antes de qualquer leitura de
  pixel via cfitsio (antes: stack-buffer-overflow dentro do cfitsio).
- As dimensões de imagem FITS são validadas contra um teto e o tamanho real do
  arquivo antes de qualquer multiplicação de contagem de pixels (antes: um arquivo
  de 2,9 KB com um cabeçalho mentiroso podia disparar uma tentativa de alocação de
  ~40 GB e uma exceção não capturada). Também corrigido: um bypass de overflow em
  `NAXIS3` descoberto durante a revisão da primeira correção.
- Arquivos de vídeo SER rejeitam valores de largura/altura com o bit de sinal
  ligado, antes do cast de estreitamento para `int`.
- A extração de arquivos compactados (ZIP/TAR/7Z/RAR) agora rejeita entradas
  symlink, FIFO e de dispositivo, prevenindo que um arquivo compactado malicioso
  escreva fora do diretório de extração.
- Leituras de coluna de BINTABLE (tabela binária FITS) agora propagam erros reais
  em vez de fabricar silenciosamente linhas `NaN`, e leituras fora dos limites em
  colunas opcionais estão protegidas.
- A medição de centroide (etapa de refinamento da posição do pixel) agora protege
  contra entrada não finita (`NaN`/`Inf`) antes de qualquer `static_cast<int>`
  (antes: um pixel ruim podia chegar a `INT_MIN` e abortar por overflow com sinal),
  e a fotometria rejeita um fluxo líquido `NaN` em vez de reportá-lo como medição
  bem-sucedida.
- Corrigido o vazamento de dupla alocação do SEP (biblioteca Source Extractor) via
  um patch local aplicado no momento do `FetchContent`.

#### Corrigido - Corretude astrométrica

- **Projeções WCS não zenitais (CAR/MER/GLS/AIT) reportavam uma coordenada
  celeste com erro de aproximadamente 90 graus, com RA/Dec trocados.** Este foi o
  achado mais severo: a derivação errada do polo nativo ia direto para o relatório
  ADES/MPC em qualquer campo usando uma dessas quatro projeções. Corrigido e
  validado contra o `astropy` (168/168 casos de teste) como referência externa e
  independente.
- Adicionado um oráculo de teste por valor absoluto (não só ida-e-volta) para as 8
  projeções WCS suportadas, para que uma regressão futura como a acima seja pega
  imediatamente.
- Adicionada uma suíte de testes de correção de refração com geometria conhecida,
  e confirmado que o gate `isSpaceTelescope` pula corretamente a correção para
  telescópios espaciais.
- Removidas `applyPrecessionJ2000ToDate()` e `applyNutation()`: código morto sem
  nenhum chamador em todo o pipeline (nunca foi de fato conectado, ver
  [`docs/technical-reference.md`](docs/technical-reference.md) §6/§8 para o motivo
  pelo qual a abordagem de plate-solve WCS as torna desnecessárias). Adicionados
  testes de oráculo contra valores de referência de aberração/precessão/
  nutação/eclíptica.
- Substituídas asserções de teste de fotometria circulares (que comparavam a saída
  do código com ela mesma) por um oráculo independente usando fluxo e zero-point
  conhecidos.
- A conversão de FWHM do centroide agora usa a constante precisa `2.354820045`
  (`2√(2 ln 2)`) em vez de um literal truncado.

#### Alterado

- **Licença migrada de PolyForm Noncommercial 1.0.0 para a GNU Affero General
  Public License v3.0 (AGPL-3.0):** copyleft forte; uso comercial é permitido, mas
  distribuir uma versão modificada (inclusive rodando-a como serviço de rede) exige
  disponibilizar o código-fonte.
- Adicionados cabeçalhos de licença SPDX em todos os 127 arquivos-fonte de `src/`
  (conformidade REUSE, uma forma padronizada de checar a licença de cada arquivo
  por máquina).
- Adicionado um arquivo `NOTICE` consolidando atribuição de terceiros e copyright
  para cada dependência empacotada/baixada (CCfits, SEP, QuaZip, spdlog,
  nlohmann/json, FFTW3, cfitsio); o README agora mostra uma coluna de licença por
  dependência.
- Corrigida a seção de refração atmosférica de `docs/technical-reference.md`, que
  antes descrevia um modelo de dois ramos que não correspondia à implementação
  real (uma única fórmula Bennett-1982 incondicional).
- Os créditos de implementação de referência GDL/NEMO/Siril do `README.md` foram
  reformulados para deixar claro que são referências de aprendizado, não código
  retirado desses projetos.

#### Infraestrutura

- Adicionado `SECURITY.md` com contato para divulgação responsável.
- Adicionado um workflow de CI só em `pull_request -> main` / tag de release
  (`.github/workflows/audit.yml`) rodando a auditoria numérica completa (ASan/UBSan,
  cppcheck, clang-tidy, Valgrind), além de um git hook local `pre-commit` rápido que
  compila incrementalmente e roda `ctest` antes de cada commit.
- Removido o workflow de CI órfão `qa-wsl2.yml` (WSL2 não é mais um alvo suportado;
  todos os itens restantes de QA de distro estão completos, ver
  [`docs/qa-distros.md`](docs/qa-distros.md)).
- Renormalizados os fins de linha CRLF para LF em todos os arquivos de workflow de
  CI.
- Substituídos 10 testes core com `SKIP` fantasma por fixtures FITS sintéticas
  reais e versionadas em disco.

#### Suíte de testes

- A execução dos sanitizers ASan/UBSan cresceu de **4426 para 5480 assertions** em
  **159 casos**, todos verdes, incluindo as novas suítes de oráculo `[bintable]` e
  `[wcs]`, com `detect_leaks=1` habilitado.

---

## [0.9.0-beta] - 2026-03-22

### 🇬🇧 English

**Beta release, all features complete, pending real-world FITS validation.**

#### Added

- Codeberg Actions release pipeline (`.forgejo/workflows/release.yml`): automated
  RPM, DEB, Arch PKGBUILD tarball, and `install.sh` artifacts on every version tag
  push
- `packaging/debian/control-binary`: binary-only DEB control template used by CI
- `packaging/install.sh`: universal bilingual (EN/PT-BR) installer; auto-detects
  distro, downloads correct package, resolves dependencies, sets up desktop
  integration
- `INSTALL.md`: "Quick install, binary packages" section with per-distro `curl` +
  install commands
- `README.md`: "Direct package install" section with one-liner commands for all
  supported distros

#### Changed

- Version bumped from 0.5.0 to 0.9.0-beta to reflect feature-complete beta status
- README: added beta warning (not yet validated with real observatory FITS files)

#### Fixed

- Invert / flip broken in blink mode (precomputed image path now uses
  `invertPixels()` / `mirrored()` directly)
- `QSpinBox` / `QDoubleSpinBox` / `QComboBox` button widths inconsistent between
  dark and light themes
- Overlay marking colors did not match catalog table colors (unified via
  `Theme::mark*()` constants)
- 10 truncated PT-BR translations caused by AI bulk-generation hitting output
  token limit

#### Infrastructure

- `Theme::dp(int)`: DPI-proportional sizing; all hard-coded pixel values scaled to
  logical DPI (no visual change at 96 DPI; proportional on HiDPI screens without
  OS-level scaling)
- `Theme::mark*()` inline color constants as single source of truth for overlay +
  table colors

### 🇧🇷 Português

**Versão beta, todas as funcionalidades completas, aguardando validação com FITS
reais.**

#### Adicionado

- Pipeline de release no Codeberg Actions (`.forgejo/workflows/release.yml`):
  geração automática de RPM, DEB, tarball PKGBUILD para Arch e `install.sh` a cada
  push de tag de versão
- `packaging/debian/control-binary`: template de controle DEB binário usado pelo CI
- `packaging/install.sh`: instalador universal bilíngue (EN/PT-BR); detecta distro
  automaticamente, baixa o pacote correto, resolve dependências, configura
  integração com desktop
- `INSTALL.md`: seção "Instalação rápida, pacotes binários" com comandos `curl` +
  instalação por distro
- `README.md`: seção "Instalação direta do pacote" com comandos de uma linha para
  todas as distros suportadas

#### Alterado

- Versão incrementada de 0.5.0 para 0.9.0-beta para refletir o estado beta com
  funcionalidades completas
- README: adicionado aviso de beta (ainda não validado com FITS reais de
  observatório)

#### Corrigido

- Inversor / espelhamento quebrado em modo blink (caminho de imagem
  pré-computada agora usa `invertPixels()` / `mirrored()` diretamente)
- Largura dos botões `QSpinBox` / `QDoubleSpinBox` / `QComboBox` inconsistente
  entre temas escuro e claro
- Cores das marcações no overlay não correspondiam às cores na tabela do
  catálogo (unificadas via constantes `Theme::mark*()`)
- 10 traduções PT-BR truncadas causadas por geração em lote de IA que atingiu o
  limite de tokens de saída

#### Infraestrutura

- `Theme::dp(int)`: dimensionamento proporcional ao DPI; todos os valores de
  pixels em código escalados ao DPI lógico (sem mudança visual a 96 DPI;
  proporcional em telas HiDPI sem escala configurada pelo SO)
- Constantes de cor `Theme::mark*()` como fonte única de verdade para cores do
  overlay e da tabela

---

## [0.5.0] - 2026-03-21

### 🇬🇧 English

**Phase 4: Advanced rendering, formats, WCS pipeline, detection algorithms,
import/export, quality audit.**

#### Added - Display & Rendering

- Transfer functions: Linear, Square Root, Asinh, Logarithmic, Histogram
  Equalization
- Transfer function selector integrated in BackgroundRangeDialog
- False color / LUT presets: Hot, Cool, Viridis, Grayscale
- NaN pixel highlight in a distinct color overlay
- Laplacian / Unsharp-mask sharpening toggle in BlinkWidget (Off / USM / LoG)
- 2D power spectrum dialog (FFT with Hann window, log₁₊ scale, frequency tooltip)
- Pixel histogram dialog with Gaussian fit overlay

#### Added - Image Formats

- Color FITS support via NAXIS3 = 3 (RGB planes)
- Color FITS via multi-extension HDUs (3 separate image extensions)
- HDU navigator for multi-extension FITS files
- SER, XISF, TIFF 16-bit, PNG 16-bit, RAW DSLR (libraw), PDS format support
- FITS data cube animation: NAXIS3 > 3 planes played as blink animation (MDI
  sub-window)
- 1D spectrum plot dialog for FITS NAXIS=1 spectra (Qt6::Charts, dark theme, WCS
  axis)
- libarchive support: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR (optional dependency)

#### Added - Astrometry & WCS

- Full WCS projection pipeline beyond TAN: SIN, ARC, GLS/SFL, MER, AIT, STG
  (Calabretta & Greisen 2002)
- Atmospheric refraction correction applied before plate-solve and ADES export
- ICRS → CIRS → topocentric coordinate chain; `<sys>ICRF</sys>` in XML+PSV output
- Annual aberration, precession J2000→date and nutation applied
- Ecliptic plane overlay + Galactic plane proximity warning

#### Added - Detection & Measurement

- Elliptical PSF fitting with Levenberg-Marquardt solver (FWHM, coma, tracking
  quality)
- Bad pixel mask with nearest-neighbor interpolation
- Streak/trail detection by blob elongation analysis
- ClumpFind algorithm for blended sources
- Background median sliding filter (tile grid, sigma-clipping)

#### Added - Import / Export

- Import DAOPHOT/SExtractor FITS BINTABLE detected-star catalogs
- Import local offline catalogs from FITS BINTABLE (USNO-B/UCAC/Gaia-style; cone
  filter)
- Import reduction tables from IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/
  ycentroid)
- Settings → Connections: catalog source selector (VizieR / Local FITS)

#### Added - Infrastructure & Quality

- CCfits 2.7 bundled source; `FitsTableReader` built on CCfits BINTABLE API
- Numerical audit targets: `cmake --build build --target audit` (cppcheck,
  clang-tidy, Valgrind, ASan+UBSan)
- 4 bugs fixed by audit: FFTW r2c buffer size for non-square images, strict
  aliasing in ImageLoader, `Qt::UTC` deprecation, missing automatic-move
  optimization
- `INSTALL.md` with per-distro dependency installation instructions (10 distros)

#### Fixed

- B&W vs Color auto-detection warning for unexpected image type
- `QImage::flipped()` replaced with `mirrored()` for Qt 6.4 compatibility

### 🇧🇷 Português

**Fase 4: Renderização avançada, formatos, pipeline WCS, algoritmos de detecção,
importação/exportação, auditoria de qualidade.**

#### Adicionado - Exibição e Renderização

- Funções de transferência: Linear, Raiz Quadrada, Asinh, Logarítmica,
  Equalização de Histograma
- Seletor de função de transferência integrado ao BackgroundRangeDialog
- Presets de cor falsa / LUT: Hot, Cool, Viridis, Escala de Cinza
- Destaque de pixels NaN em cor distinta
- Alternância de nitidez Laplaciano / Unsharp-mask no BlinkWidget (Off / USM /
  LoG)
- Diálogo de espectro de potência 2D (FFT com janela Hann, escala log₁₊, tooltip
  de frequência)
- Diálogo de histograma de pixels com ajuste gaussiano

#### Adicionado - Formatos de Imagem

- Suporte a FITS colorido via NAXIS3 = 3 (planos RGB)
- FITS colorido via multi-extensão HDU (3 extensões de imagem separadas)
- Navegador de HDUs para arquivos FITS multi-extensão
- Suporte a SER, XISF, TIFF 16-bit, PNG 16-bit, RAW DSLR (libraw), PDS
- Animação de cubo de dados FITS: NAXIS3 > 3 planos reproduzidos como animação
  blink
- Diálogo de espectro 1D para FITS NAXIS=1 (Qt6::Charts, tema escuro, eixo WCS)
- Suporte a arquivos compactados via libarchive: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR
  (opcional)

#### Adicionado - Astrometria e WCS

- Pipeline completo de projeção WCS além de TAN: SIN, ARC, GLS/SFL, MER, AIT, STG
  (Calabretta & Greisen 2002)
- Correção de refração atmosférica aplicada antes do plate-solve e exportação
  ADES
- Cadeia ICRS → CIRS → topocêntrico; `<sys>ICRF</sys>` em XML+PSV
- Aberração anual, precessão J2000→data e nutação aplicadas
- Overlay do plano eclíptico + aviso de proximidade do plano galáctico

#### Adicionado - Detecção e Medição

- Ajuste de PSF elíptica com solver Levenberg-Marquardt (FWHM, coma, qualidade de
  tracking)
- Máscara de pixels ruins com interpolação por vizinhos mais próximos
- Detecção de traços/rastros por análise de elongação de blob
- Algoritmo ClumpFind para fontes blendadas
- Filtro mediano deslizante de fundo (grade de tiles, sigma-clipping)

#### Adicionado - Importação / Exportação

- Importar catálogos FITS BINTABLE de estrelas detectadas DAOPHOT/SExtractor
- Importar catálogos offline locais de FITS BINTABLE (estilo USNO-B/UCAC/Gaia;
  filtro por cone)
- Importar tabelas de redução IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/
  ycentroid)
- Configurações → Conexões: seletor de fonte de catálogo (VizieR / FITS Local)

#### Adicionado - Infraestrutura e Qualidade

- CCfits 2.7 empacotado; `FitsTableReader` construído sobre a API BINTABLE do
  CCfits
- Targets de auditoria numérica: `cmake --build build --target audit` (cppcheck,
  clang-tidy, Valgrind, ASan+UBSan)
- 4 bugs corrigidos pela auditoria: buffer FFTW r2c para imagens não quadradas,
  aliasing estrito no ImageLoader, deprecação de `Qt::UTC`, otimização de move
  automático ausente
- `INSTALL.md` com instruções de instalação de dependências por distribuição (10
  distros)

#### Corrigido

- Aviso de detecção automática B&W vs Cor para tipo de imagem inesperado
- `QImage::flipped()` substituído por `mirrored()` para compatibilidade com Qt
  6.4

---

## [0.4.0] - 2026-03-01

### 🇬🇧 English

**Phase 3: Complete science workflow, school UX, i18n, photometry, calibration,
MOD, ADES reporting.**

#### Added - Interface & UX

- Full bilingual interface: English and Brazilian Portuguese (1007 strings)
- School workflow: step-by-step guided mode for students
- `Toast` notification widget (non-blocking: success/info/warning/error)
- Day/Night/Auto UI theme toggle (Ctrl+Shift+T)
- Export/Import School Configuration (File menu)
- "Send to Teacher" button in Report Preview (mailto:)
- School email field in Settings → Observer tab
- Dynamic WCS status in workflow panel

#### Added - Help & Documentation

- Built-in bilingual help (HTML) with FAQ and "For Students" sections
- AboutDialog: license link; reference implementations with GitHub links

#### Added - Reporting & Submission

- ADES 2022 XML and PSV report generation with `<sys>ICRF</sys>` annotation
- MPC report preview dialog with HTML rendering
- MPC direct submission endpoint
- Solar System body identification via IMCCE SkyBoT
- JPL Horizons ephemeris query dialog

#### Added - Measurement

- Centroid engine: intensity-weighted moments + Gaussian PSF fitting
- Aperture photometry: differential, zero-point calibration, airmass correction
- Light curve dialog and growth curve dialog
- Region statistics panel (mean, median, σ, min/max)
- VerificationDialog: star match residuals and known-object check

#### Added - Calibration & Stacking

- Bias/dark/flat calibration pipeline
- Calibration wizard (step-by-step)
- Master dark / master flat builder dialog
- Image stacker: Add, Average, Median, Sigma-clipped; sub-pixel FFT registration
- Track & Stack with asteroid-tracking alignment

#### Added - Moving Object Detection

- Moving Object Detector across ≥3 images with tracklet linking

#### Added - Plate Solving

- ASTAP offline plate solver support

#### Added - Catalogs

- MPCORB offline catalog (KOO engine)
- Observatory preset database (MPC codes)

### 🇧🇷 Português

**Fase 3: Fluxo científico completo, UX escolar, i18n, fotometria, calibração,
MOD, relatórios ADES.**

#### Adicionado - Interface e UX

- Interface bilíngue completa: inglês e português brasileiro (1007 strings)
- Fluxo escolar: modo guiado passo a passo para alunos
- Widget de notificação `Toast` (não bloqueante: sucesso/info/aviso/erro)
- Alternância de tema Dia/Noite/Automático (Ctrl+Shift+T)
- Exportar/Importar Configuração da Escola (menu Arquivo)
- Botão "Enviar para Professor" na pré-visualização de relatório (mailto:)
- Campo de e-mail da escola em Configurações → aba Observador
- Status WCS dinâmico no painel de workflow

#### Adicionado - Ajuda e Documentação

- Ajuda integrada bilíngue (HTML) com seções FAQ e "Para Alunos"
- AboutDialog: link de licença; implementações de referência com links do GitHub

#### Adicionado - Relatórios e Submissão

- Geração de relatório ADES 2022 XML e PSV com anotação `<sys>ICRF</sys>`
- Diálogo de pré-visualização de relatório MPC com renderização HTML
- Endpoint de submissão direta ao MPC
- Identificação de corpos do Sistema Solar via IMCCE SkyBoT
- Diálogo de consulta de efemérides JPL Horizons

#### Adicionado - Medição

- Motor de centroide: momentos ponderados por intensidade + ajuste de PSF
  gaussiana
- Fotometria de abertura: diferencial, calibração de zero-point, correção de
  massa de ar
- Diálogo de curva de luz e diálogo de curva de crescimento
- Painel de estatísticas de região (média, mediana, σ, mín/máx)
- VerificationDialog: resíduos de correspondência de estrelas e verificação de
  objetos conhecidos

#### Adicionado - Calibração e Empilhamento

- Pipeline de calibração bias/dark/flat
- Assistente de calibração (passo a passo)
- Diálogo construtor de master dark / master flat
- Empilhador de imagens: Adição, Média, Mediana, Sigma-clipped; registro
  sub-pixel por FFT
- Track & Stack com alinhamento por rastreamento de asteroide

#### Adicionado - Detecção de Objetos em Movimento

- Detector de Objetos em Movimento em ≥3 imagens com ligação de tracklets

#### Adicionado - Plate Solving

- Suporte ao plate solver offline ASTAP

#### Adicionado - Catálogos

- Catálogo offline MPCORB (motor KOO)
- Banco de dados de observatórios predefinidos (códigos MPC)

---

## [0.3.0] - 2026-02-01

### 🇬🇧 English

**Phase 2: Online astrometry, star detection, plate solving, catalog overlay,
known-object engine.**

#### Added

- Automated star detection via SEP (Source Extractor C library)
- Online plate solving via astrometry.net REST API
- WCS TAN projection pipeline; WCS solution written back to FITS header
- VizieR star catalog overlay (UCAC4, Gaia DR3) with SQLite local cache
- Known Object Overlay (KOO) engine via IMCCE SkyBoT
- Session comparison dialog

### 🇧🇷 Português

**Fase 2: Astrometria online, detecção de estrelas, plate solving, sobreposição
de catálogo, motor de objetos conhecidos.**

#### Adicionado

- Detecção automática de estrelas via SEP (biblioteca C do Source Extractor)
- Plate solving online via API REST do astrometry.net
- Pipeline de projeção WCS TAN; solução WCS gravada de volta no cabeçalho FITS
- Sobreposição do catálogo de estrelas VizieR (UCAC4, Gaia DR3) com cache local
  SQLite
- Motor de Objetos Conhecidos (KOO) via IMCCE SkyBoT
- Diálogo de comparação de sessão

---

## [0.2.0] - 2025-12-01

### 🇬🇧 English

**Phase 1: Blink comparison workflow and image navigation.**

#### Added

- Multi-image blink engine (BlinkWidget) with configurable interval
- Thumbnail navigator bar for rapid frame selection
- Background/range dialog for interactive display adjustment
- Log panel (scrollable info/warning/error messages)
- B&W vs Color image type warning with "Do not show again" checkbox

### 🇧🇷 Português

**Fase 1: Fluxo de comparação blink e navegação entre imagens.**

#### Adicionado

- Motor de blink multi-imagem (BlinkWidget) com intervalo configurável
- Barra de miniaturas para seleção rápida de frames
- Diálogo de fundo/alcance para ajuste interativo de exibição
- Painel de log (mensagens info/aviso/erro com rolagem)
- Aviso de tipo de imagem B&W vs Cor com checkbox "Não mostrar novamente"

---

## [0.1.0] - 2025-11-01

### 🇬🇧 English

**Phase 0: Qt6/C++ skeleton with FITS image viewer.**

#### Added

- Qt6 application skeleton with MDI sub-window layout
- FITS file loading (cfitsio): single HDU, primary image
- FITS image viewer: zoom, pan, invert, flip horizontal/vertical
- Bilingual menu structure (English / Brazilian Portuguese stubs)
- CMake build system (Qt6, cfitsio, spdlog, nlohmann/json via FetchContent)
- PolyForm Noncommercial 1.0.0 license (historical: superseded by AGPL-3.0 in
  v0.9.0, see above)

### 🇧🇷 Português

**Fase 0: Esqueleto Qt6/C++ com visualizador de imagens FITS.**

#### Adicionado

- Esqueleto de aplicativo Qt6 com layout MDI de sub-janelas
- Carregamento de arquivos FITS (cfitsio): HDU único, imagem primária
- Visualizador de imagens FITS: zoom, pan, inverter, espelhar
  horizontal/vertical
- Estrutura de menus bilíngue (inglês / português brasileiro, stubs)
- Sistema de build CMake (Qt6, cfitsio, spdlog, nlohmann/json via FetchContent)
- Licença PolyForm Noncommercial 1.0.0 (histórico: substituída pela AGPL-3.0 na
  v0.9.0, ver acima)
