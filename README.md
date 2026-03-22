# AstroFind

![Version](https://img.shields.io/badge/version-0.9.0--beta-orange)
![Language](https://img.shields.io/badge/language-C%2B%2B23-blue)
![Qt](https://img.shields.io/badge/Qt-6.4%2B-41cd52?logo=qt)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?logo=cmake)
![Platform](https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black)
![Tests](https://img.shields.io/badge/tests-116%20passed-brightgreen)
![License](https://img.shields.io/badge/license-PolyForm%20NC%201.0-lightgrey)

**English** | [Português](#português)

---

## English

Modern asteroid detection software for citizen science and educational use.
Built for Linux, compatible with the IASC / MPC workflow.
Outputs [ADES 2022](https://minorplanetcenter.net/ades) format reports.

> **⚠️ Beta software — not yet validated with real observatory FITS files.**
> All features are implemented and the test suite passes on 11 Linux distributions,
> but end-to-end validation with real telescope imagery has not been performed.
> Results (astrometry, photometry, ADES reports) should be independently verified
> before submission to the MPC. Feedback and real-world test reports are very welcome.

> *"To my son Guga, a Linux user like me*
> *who had no good option for finding asteroids."*

---

### Features

#### Image display
- FITS viewer with zoom, pan, invert, flip horizontal/flip vertical
- Display stretch: Linear, Logarithmic, Square-root, Asinh, Histogram Equalization
- False-colour LUTs: Grayscale, Hot, Cool, Viridis
- NaN pixels rendered in a distinct colour
- Multi-image blink comparison with sharpening (Laplacian / Unsharp-mask)
- Thumbnail navigator for rapid frame selection
- Background and range dialog with interactive stretch selector
- Histogram panel with Gaussian sky-noise fit
- 2-D power spectrum (FFT) for periodic noise diagnosis
- FITS cube (NAXIS3 > 3) temporal animation
- 1-D spectrum plot (NAXIS=1 FITS files) via Qt6::Charts

#### File formats
- FITS (`.fits`, `.fit`, `.fts`) — single HDU, multi-HDU, RGB (NAXIS3=3), temporal cubes
- SER video (`.ser`) — first frame
- XISF — PixInsight (`.xisf`)
- TIFF 16-bit, PNG, BMP, JPEG
- ZIP archives — automatic extraction on open
- TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR — via libarchive (optional)
- HDU navigation bar for multi-extension FITS files

#### Astrometry
- Automated star detection (SEP / Source Extractor)
- Plate solving online via [astrometry.net](https://nova.astrometry.net)
- Plate solving offline via [ASTAP](https://www.hnsky.org/astap.htm)
- Full WCS pipeline: TAN, SIN, ARC, STG, CAR, MER, GLS/SFL, AIT (Calabretta & Greisen 2002)
- Write WCS solution back to FITS header
- VizieR star catalog overlay (UCAC4, Gaia DR3)
- Local star catalog from FITS BINTABLE (offline, optional)
- Ecliptic and galactic plane overlay with extinction warning
- Atmospheric refraction correction (topocentric, ignores space telescopes)
- ICRS → CIRS → topocentric conversion for sub-arcsecond ADES residuals
  (annual aberration, precession J2000→date, nutation)

#### Source detection
- Background model by sliding-median filter (tile grid + sigma-clipping)
- Bad pixel mask with nearest-neighbour interpolation
- Streak/trail detection by blob elongation (fast asteroids, satellites)
- PSF fitting: elliptical Gaussian with Levenberg–Marquardt
- ClumpFind: contour-based blended source separation

#### Photometry
- Aperture photometry (single and multi-aperture)
- Differential photometry with zero-point calibration
- Airmass correction (extinction coefficient)
- Light curve dialog
- Growth curve dialog
- Region statistics panel

#### Moving object detection
- Moving Object Detection (MOD) across ≥3 images
- Track & Stack with asteroid-tracking alignment
- Image stacker: Add, Average, Median, Sigma-clipped; sub-pixel FFT registration

#### Calibration
- Bias, dark, flat-field calibration
- Calibration wizard (step-by-step)
- Master dark / master flat builder
- Drag-and-drop calibration frames

#### Reporting
- ADES 2022 XML and PSV report generation
- `<sys>ICRF</sys>` flag for precise reference frame annotation
- MPC report preview dialog with HTML rendering
- MPC direct submission endpoint
- Solar System Body identification via [IMCCE SkyBoT](https://vo.imcce.fr/webservices/skybot/)
- JPL Horizons ephemeris queries
- Magnitude band selector, proper motion, observer site from FITS headers

#### Data import / export
- Export image as JPEG/PNG
- Import DAOPHOT/SExtractor FITS BINTABLE as detected-star overlay
- Import IRAF/Astropy reduction tables (XCENTER/YCENTER, xcentroid/ycentroid, sky coords)
- Export/import school configuration
- Save project (`.gus` JSON format)

#### Interface
- Day / Night / Auto UI theme (Ctrl+Shift+T)
- Full bilingual interface: English and Brazilian Portuguese (1007+ strings)
- School workflow: step-by-step guided mode for students
- Toast notifications (non-blocking)
- Undo/Redo stack
- Secure API key storage (Qt Keychain, optional)
- Session comparison dialog

---

### Project Structure

```
astrometrica/
├── cmake/
│   ├── dependencies.cmake       # Third-party library detection (pkg-config + FetchContent)
│   ├── audit.cmake              # Numerical audit targets (38.1–38.5)
│   └── valgrind.supp            # Valgrind suppressions for Qt/glibc noise
├── docs/
│   └── technical-reference.md  # Technical reference (WCS, refraction, PSF, ADES…)
├── i18n/
│   ├── astrofind_en.ts          # English translation source
│   ├── astrofind_pt_BR.ts       # Brazilian Portuguese translation source
│   ├── astrofind_en.qm          # Compiled English strings
│   └── astrofind_pt_BR.qm       # Compiled Portuguese strings
├── originals/
│   └── CCfits.tar.gz            # CCfits 2.7 source archive — used at build time only
├── resources/
│   ├── help/
│   │   ├── help_en.html         # Built-in help — English
│   │   └── help_pt_br.html      # Built-in help — Portuguese
│   ├── icons/
│   │   ├── astrofind-16.png     # Application icon 16×16
│   │   ├── astrofind-32.png
│   │   ├── astrofind-48.png
│   │   ├── astrofind-64.png
│   │   ├── astrofind-128.png
│   │   ├── astrofind-256.png
│   │   ├── astrofind-512.png
│   │   └── astrofind.svg        # Vector source
│   └── resources.qrc            # Qt resource manifest
├── src/
│   ├── core/                    # Business logic — no Qt UI dependencies
│   │   ├── AdesReport.cpp/h
│   │   ├── ApiKeyStore.cpp/h
│   │   ├── AstapSolver.cpp/h
│   │   ├── AstrometryClient.cpp/h
│   │   ├── Astronomy.cpp/h
│   │   ├── Calibration.cpp/h
│   │   ├── CatalogClient.cpp/h
│   │   ├── Centroid.cpp/h
│   │   ├── Ephemeris.cpp/h
│   │   ├── ExportImage.cpp/h
│   │   ├── FitsImage.cpp/h
│   │   ├── FitsTableReader.cpp/h
│   │   ├── GusProject.cpp/h
│   │   ├── HorizonsClient.cpp/h
│   │   ├── ImageLoader.cpp/h
│   │   ├── ImageSession.cpp/h
│   │   ├── ImageStacker.cpp/h
│   │   ├── KooEngine.cpp/h
│   │   ├── Measurement.h
│   │   ├── MovingObjectDetector.cpp/h
│   │   ├── MpcOrb.cpp/h
│   │   ├── MpcSubmit.cpp/h
│   │   ├── ObservatoryDatabase.h
│   │   ├── Overlay.h
│   │   ├── Photometry.cpp/h
│   │   ├── SessionComparator.cpp/h
│   │   ├── Spectrum1D.cpp/h
│   │   ├── StarDetector.cpp/h
│   │   └── CMakeLists.txt
│   ├── ui/                      # Qt6 widgets — depends on astrofind_core
│   │   ├── AboutDialog.cpp/h
│   │   ├── AppIcons.cpp/h
│   │   ├── BackgroundRangeDialog.cpp/h
│   │   ├── BlinkWidget.cpp/h
│   │   ├── CalibrationPanel.cpp/h
│   │   ├── CalibrationWizard.cpp/h
│   │   ├── DockTitleBar.cpp/h
│   │   ├── EditImageParametersDialog.cpp/h
│   │   ├── FitsImageView.cpp/h
│   │   ├── FitsSubWindow.cpp/h
│   │   ├── GrowthCurveDialog.cpp/h
│   │   ├── HelpDialog.cpp/h
│   │   ├── HistogramDialog.cpp/h
│   │   ├── HorizonsQueryDialog.cpp/h
│   │   ├── ImageCatalogTable.cpp/h
│   │   ├── LightCurveDialog.cpp/h
│   │   ├── LogPanel.cpp/h
│   │   ├── MainWindow.cpp/h
│   │   ├── MainWindow_io.cpp
│   │   ├── MainWindow_measurement.cpp
│   │   ├── MainWindow_p.h
│   │   ├── MainWindow_reduction.cpp
│   │   ├── MainWindow_settings.cpp
│   │   ├── MasterFrameDialog.cpp/h
│   │   ├── MeasurementTable.cpp/h
│   │   ├── PowerSpectrumDialog.cpp/h
│   │   ├── RegionStatsPanel.cpp/h
│   │   ├── ReportPreviewDialog.cpp/h
│   │   ├── SelectMarkingsDialog.cpp/h
│   │   ├── SessionCompareDialog.cpp/h
│   │   ├── SessionInfoBar.cpp/h
│   │   ├── SettingsDialog.cpp/h
│   │   ├── SetupWizard.cpp/h
│   │   ├── SpectrumDialog.cpp/h
│   │   ├── Theme.cpp/h
│   │   ├── ThumbnailBar.cpp/h
│   │   ├── Toast.cpp/h
│   │   ├── VerificationDialog.cpp/h
│   │   ├── WorkflowPanel.cpp/h
│   │   └── CMakeLists.txt
│   ├── CMakeLists.txt
│   └── main.cpp
├── tests/                       # Unit / integration tests — optional for app-only builds
│   ├── test_ades_report.cpp     # ADES XML/PSV generation, ICRS flag (10 cases)
│   ├── test_astronomy.cpp       # Refraction, aberration, precession, nutation, CIRS (17 cases)
│   ├── test_calibration.cpp     # Dark/flat/bias, background, bad pixels (8 cases)
│   ├── test_calibration_panel.cpp # CalibrationPanel widget state (9 cases)  [UI]
│   ├── test_centroid.cpp        # Moments, Gaussian PSF, elliptical LM PSF (6 cases)
│   ├── test_fits_functional.cpp # Multi-HDU, multi-ext RGB, SER, XISF (10 cases)
│   ├── test_fits_loader.cpp     # FITS open, metadata, invalid files (2 cases)
│   ├── test_image_session.cpp   # Session create/add/persist (3 cases)
│   ├── test_image_stacker.cpp   # Add/Average/Median, FFT align, Track&Stack (8 cases)
│   ├── test_moving_object_detector.cpp  # Tracklets, tolerance, SNR (5 cases)
│   ├── test_mpc_orb.cpp         # Parsing, lookup, update (4 cases)
│   ├── test_photometry.cpp      # Differential photometry, zero-point, growth curve (18 cases)
│   ├── test_robustness.cpp      # Degenerate images, overflow, extremes, NaN (22 cases)
│   ├── test_session_signals.cpp # MainWindow/WorkflowPanel/BlinkWidget signals (12 cases) [UI]
│   ├── test_settings_dialog.cpp # SettingsDialog load/save/reset (2 cases) [UI]
│   ├── test_star_detector.cpp   # Detection, threshold, star limit (3 cases)
│   └── CMakeLists.txt
├── astrofind.desktop            # XDG desktop entry (Linux install)
├── CHANGELOG.md
├── CLAUDE.md                    # AI assistant instructions and feature backlog
├── CMakeLists.txt               # Root build configuration
├── CONTRIBUTING.md
├── INSTALL.md                   # Per-distro installation instructions
├── LICENSE                      # PolyForm Noncommercial 1.0.0
└── README.md
```

> **Note on `originals/`:** this directory contains only `CCfits.tar.gz`, a C++ FITS library used
> at build time. It is not needed to run the application and can be ignored by end users.
> CMake extracts it automatically during configuration.
>
> **Note on `tests/`:** the test suite (116 core + 23 UI cases) is part of the repository but
> is not required to build the application. See [Building](#building) for how to compile
> the app only.

---

### Source Files

#### `src/core/` — Business logic (no Qt UI dependencies)

| File | Description |
|------|-------------|
| `AdesReport.cpp/h` | ADES 2022 XML and PSV report generation; `<sys>ICRF</sys>` annotation |
| `ApiKeyStore.cpp/h` | API key credential storage (Qt Keychain when available, QSettings fallback) |
| `AstapSolver.cpp/h` | ASTAP offline plate solver — subprocess wrapper |
| `AstrometryClient.cpp/h` | astrometry.net REST client (async, QNetworkAccessManager) |
| `Astronomy.cpp/h` | Atmospheric refraction, annual aberration, precession J2000→date, nutation, ICRS→CIRS→topocentric, ecliptic/galactic coordinates, Julian Date |
| `Calibration.cpp/h` | Bias/dark/flat calibration pipeline; bad pixel mask + interpolation; background subtraction |
| `CatalogClient.cpp/h` | VizieR TAP query (UCAC4, Gaia DR3); SQLite local cache |
| `Centroid.cpp/h` | Moment centroids; Gaussian PSF; elliptical PSF with Levenberg–Marquardt |
| `Ephemeris.cpp/h` | Ephemeris calculations (position of Solar System bodies) |
| `ExportImage.cpp/h` | Export FITS image data as JPEG/PNG via Qt |
| `FitsImage.cpp/h` | FITS file loader (cfitsio); `PlateSolution` WCS pipeline (all 8 projections); `loadFitsCube()` for temporal cubes; `loadSpectrum1D()` for 1-D spectra |
| `FitsTableReader.cpp/h` | FITS BINTABLE reader (CCfits); `importDaophotTable()` (SExtractor/DAOPHOT); `readLocalCatalogTable()` (offline catalogs); `importReductionTable()` (IRAF/Astropy) |
| `GusProject.cpp/h` | `.gus` project file save/load (nlohmann/json) |
| `HorizonsClient.cpp/h` | JPL Horizons API client for ephemeris queries |
| `ImageLoader.cpp/h` | Unified loader dispatching to FITS/SER/XISF/Qt formats by extension |
| `ImageSession.cpp/h` | Multi-image session container with metadata |
| `ImageStacker.cpp/h` | Add / Average / Median / Sigma-clipped stacking; FFT sub-pixel alignment; Track & Stack |
| `KooEngine.cpp/h` | Known Object Overlay using IMCCE SkyBoT REST API |
| `Measurement.h` | `MeasurementResult` data structure (centroid, photometry, astrometry) |
| `MovingObjectDetector.cpp/h` | Moving object detection across ≥3 images; tracklet linking |
| `MpcOrb.cpp/h` | MPCORB.DAT parser and lookup; incremental update |
| `MpcSubmit.cpp/h` | MPC direct report submission (HTTP POST) |
| `ObservatoryDatabase.h` | Static MPC observatory code + coordinates database |
| `Overlay.h` | `DetectedStar`, `CatalogStar`, `KooObject` data structures |
| `Photometry.cpp/h` | Aperture photometry; differential zero-point; multi-aperture; airmass correction |
| `SessionComparator.cpp/h` | Session comparison (photometry residuals, star matching across sessions) |
| `Spectrum1D.cpp/h` | 1-D spectrum loader from FITS NAXIS=1 files; WCS axis computation |
| `StarDetector.cpp/h` | SEP (Source Extractor) integration; streak detection; ClumpFind blended sources |

#### `src/ui/` — Qt6 widgets

| File | Description |
|------|-------------|
| `AboutDialog.cpp/h` | About / license dialog |
| `AppIcons.cpp/h` | All application icons drawn programmatically with QPainter |
| `BackgroundRangeDialog.cpp/h` | Interactive stretch selector (display min/max + transfer function) |
| `BlinkWidget.cpp/h` | Multi-frame blink widget; Laplacian / Unsharp-Mask sharpening toggle |
| `CalibrationPanel.cpp/h` | Dark/flat/bias status dock panel |
| `CalibrationWizard.cpp/h` | Step-by-step calibration frame wizard |
| `DockTitleBar.cpp/h` | Custom styled dock title bar |
| `EditImageParametersDialog.cpp/h` | Manual override of image header parameters (RA, Dec, JD, scale…) |
| `FitsImageView.cpp/h` | FITS image rendering widget (OpenGL, zoom, pan, overlay painting, colour LUTs) |
| `FitsSubWindow.cpp/h` | MDI child wrapping FitsImageView; HDU navigation bar; context menu |
| `GrowthCurveDialog.cpp/h` | Aperture growth curve plot |
| `HelpDialog.cpp/h` | Bilingual built-in help (HTML) |
| `HistogramDialog.cpp/h` | Pixel histogram with Gaussian sky-noise fit |
| `HorizonsQueryDialog.cpp/h` | JPL Horizons ephemeris query UI |
| `ImageCatalogTable.cpp/h` | Reference star catalog table (loaded images tab) |
| `LightCurveDialog.cpp/h` | Light curve plot across session images |
| `LogPanel.cpp/h` | Scrollable log panel (info / warning / error) |
| `MainWindow.cpp/h` | Main application window; menus; toolbar; dock layout |
| `MainWindow_io.cpp` | File I/O: load images, calibration frames, project save/load, archive extraction (ZIP + libarchive), spectrum/cube handling, DAOPHOT/IRAF import |
| `MainWindow_measurement.cpp` | Measurement mode: pixel click → centroid + photometry pipeline |
| `MainWindow_p.h` | Private shared header included by all MainWindow_*.cpp translation units |
| `MainWindow_reduction.cpp` | Data reduction: star detection, plate solving, VizieR/local catalog, KOO overlay |
| `MainWindow_settings.cpp` | Settings load/save/apply; observer location; camera parameters |
| `MasterFrameDialog.cpp/h` | Master dark / master flat builder dialog |
| `MeasurementTable.cpp/h` | Measurement results table (RA, Dec, mag, errors) |
| `PowerSpectrumDialog.cpp/h` | 2-D power spectrum (fftw3 r2c, Hann window, fftshift, frequency tooltip) |
| `RegionStatsPanel.cpp/h` | Floating region statistics overlay (mean, median, σ, min/max) |
| `ReportPreviewDialog.cpp/h` | ADES/MPC report preview with HTML rendering and submission buttons |
| `SelectMarkingsDialog.cpp/h` | Object overlay visibility selector |
| `SessionCompareDialog.cpp/h` | Side-by-side session photometry comparison |
| `SessionInfoBar.cpp/h` | Top status bar showing current session step and image counts |
| `SettingsDialog.cpp/h` | Full settings dialog (observer, camera, connections, detection, display, legacy) |
| `SetupWizard.cpp/h` | First-run setup wizard (API key, site location, pixel scale) |
| `SpectrumDialog.cpp/h` | 1-D spectrum plot (Qt6::Charts, dark theme, wavelength/flux axes) |
| `Theme.cpp/h` | Day / Night / Auto theme engine (palette + stylesheet) |
| `ThumbnailBar.cpp/h` | Bottom thumbnail strip for multi-image navigation |
| `Toast.cpp/h` | Transient non-blocking notification toasts (success/info/warning/error) |
| `VerificationDialog.cpp/h` | Measurement verification: star match, residuals, known object check |
| `WorkflowPanel.cpp/h` | Step-by-step guided workflow panel (school mode) |

#### `src/` — Entry point

| File | Description |
|------|-------------|
| `main.cpp` | Application entry point; locale, translation loader, QApplication setup |

---

### Requirements

#### Required

| Dependency | Version | Notes |
|------------|---------|-------|
| Qt | ≥ 6.4 | Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml |
| cfitsio | any | FITS I/O (system package or FetchContent) |
| FFTW3 | any | FFT for image stacking and power spectrum |
| SEP | ≥ 1.2 | Source Extractor C library (bundled via FetchContent) |
| spdlog | ≥ 1.11 | Logging (bundled via FetchContent) |
| nlohmann/json | ≥ 3.11 | Project file serialisation (bundled) |
| CMake | ≥ 3.20 | Build system |
| C++ | 23 | Compiler: GCC ≥ 13 or Clang ≥ 16 |

#### Optional

| Dependency | Notes |
|------------|-------|
| QuaZip + Qt6Core5Compat | ZIP archive extraction; `dnf install qt6-qt5compat-devel` |
| libarchive | TAR.GZ/BZ2/XZ, 7Z, RAR extraction; `dnf install libarchive-devel` |
| Qt6Keychain | Secure API key storage; `dnf install qtkeychain-qt6-devel libsecret-devel` |
| Qt6LinguistTools | Compile `.ts` translations; `dnf install qt6-linguist` |
| ASTAP | Offline plate solver; download from [hnsky.org](https://www.hnsky.org/astap.htm) |

---

### Building

For per-distro installation commands see **[INSTALL.md](INSTALL.md)**.

```bash
# Configure and build (application only)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)

# Run
./build/bin/AstroFind

# Build everything including tests (optional)
cmake --build build -j$(nproc)
```

Optional: install to system (Linux FHS)

```bash
sudo cmake --install build
```

This installs the binary to `/usr/local/bin`, the `.desktop` file to
`/usr/local/share/applications`, and icons to the hicolor icon theme.

---

### Testing

```bash
# Core tests (116 cases)
./build/bin/astrofind_tests

# UI tests (23 cases — requires display)
./build/bin/astrofind_ui_tests
```

---

### License

[PolyForm Noncommercial License 1.0.0](LICENSE) — free for personal, educational,
and non-commercial use.
Copyright (c) 2024–2026 Petrus Silva Costa.

---

### Acknowledgments

AstroFind was inspired by and learned from these excellent projects:

| Project | Author(s) | What we learned |
|---------|-----------|-----------------|
| [Astrometrica](http://www.astrometrica.at/) | Herbert Raab | Primary UX inspiration — the standard Windows tool for IASC/MPC work |
| [find_orb](https://github.com/Bill-Gray/find_orb) | Bill Gray (Project Pluto) | ADES output format, MPC submission, orbit determination |
| [umbrella2](https://github.com/mostanes/umbrella2) | CSCF | Moving object detection algorithms, tracklet linking |
| [Siril](https://gitlab.com/free-astro/siril) | Free-Astro team | PSF fitting, background extraction, photometry pipeline |
| [astrometry.net](https://github.com/dstndstn/astrometry.net) | Dustin Lang et al. | Plate-solving REST API used directly |
| [IRAF](https://github.com/iraf-community/iraf) | NOAO / community | Aperture photometry concepts and table column conventions |
| [GDL](https://github.com/gnudatalanguage/gdl) | GDL team | PSF elliptical / MPFIT, sliding median, FFT algorithms |
| [NEMO](https://github.com/teuben/nemo) | P. Teuben | Bad pixel masks, blob/streak, ClumpFind, histogram, Laplacian, WCS |
| [Boost.Astronomy](https://github.com/BoostGSoC19/astronomy) | GSoC 2019 | Atmospheric refraction, CIRS/ADES, ecliptic/galactic overlays |

Libraries used:
[Qt6](https://github.com/qt/qtbase) ·
[cfitsio](https://github.com/HEASARC/cfitsio) ·
[CCfits 2.7](https://heasarc.gsfc.nasa.gov/fitsio/CCfits/) ·
[FFTW3](https://github.com/FFTW/fftw3) ·
[SEP](https://github.com/kbarbary/sep) ·
[spdlog](https://github.com/gabime/spdlog) ·
[nlohmann/json](https://github.com/nlohmann/json) ·
[Catch2](https://github.com/catchorg/Catch2) ·
[QuaZip](https://github.com/stachenov/quazip) ·
[qt-keychain](https://github.com/frankosterfeld/qtkeychain) ·
[libarchive](https://www.libarchive.org/)

---

## Português

![Versão](https://img.shields.io/badge/versão-0.9.0--beta-orange)
![Language](https://img.shields.io/badge/linguagem-C%2B%2B23-blue)
![Qt](https://img.shields.io/badge/Qt-6.4%2B-41cd52?logo=qt)
![Plataforma](https://img.shields.io/badge/plataforma-Linux-FCC624?logo=linux&logoColor=black)
![Testes](https://img.shields.io/badge/testes-116%20aprovados-brightgreen)
![Licença](https://img.shields.io/badge/licença-PolyForm%20NC%201.0-lightgrey)

Software moderno de detecção de asteroides para ciência cidadã e uso educacional.
Desenvolvido para Linux, compatível com o fluxo de trabalho IASC / MPC.
Gera relatórios no formato [ADES 2022](https://minorplanetcenter.net/ades).

> **⚠️ Software em fase beta — ainda não validado com arquivos FITS reais de observatório.**
> Todas as funcionalidades estão implementadas e a suite de testes passa em 11 distribuições Linux,
> mas a validação ponta-a-ponta com imagens reais de telescópio ainda não foi realizada.
> Os resultados (astrometria, fotometria, relatórios ADES) devem ser verificados de forma
> independente antes de qualquer envio ao MPC. Relatos de testes com dados reais são muito bem-vindos.

> *"Para meu filho Guga, usuário de Linux como eu*
> *que estava sem uma boa opção para buscar asteroides."*

---

### Funcionalidades

#### Visualização de imagens
- Visualizador FITS com zoom, pan, inverter, espelhar horizontal/vertical
- Esticamento de display: Linear, Logarítmico, Raiz Quadrada, Asinh, Equalização de Histograma
- LUTs de falsa cor: Escala de Cinza, Hot, Cool, Viridis
- Pixels NaN renderizados em cor distinta
- Comparação de múltiplas imagens em blink com realce (Laplaciano / Unsharp-mask)
- Barra de miniaturas para seleção rápida de frames
- Diálogo de fundo e alcance com seletor de esticamento interativo
- Painel de histograma com ajuste gaussiano de ruído de céu
- Espectro de potência 2-D (FFT) para diagnóstico de ruído periódico
- Animação temporal de cubo FITS (NAXIS3 > 3)
- Plot de espectro 1-D (arquivos FITS NAXIS=1) via Qt6::Charts

#### Formatos de arquivo
- FITS (`.fits`, `.fit`, `.fts`) — HDU único, multi-HDU, RGB (NAXIS3=3), cubos temporais
- Vídeo SER (`.ser`) — primeiro frame
- XISF — PixInsight (`.xisf`)
- TIFF 16-bit, PNG, BMP, JPEG
- Arquivos ZIP — extração automática ao abrir
- TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR — via libarchive (opcional)
- Barra de navegação HDU para arquivos FITS multi-extensão

#### Astrometria
- Detecção automática de estrelas (SEP / Source Extractor)
- Plate solving online via [astrometry.net](https://nova.astrometry.net)
- Plate solving offline via [ASTAP](https://www.hnsky.org/astap.htm)
- Pipeline WCS completo: TAN, SIN, ARC, STG, CAR, MER, GLS/SFL, AIT (Calabretta & Greisen 2002)
- Gravar solução WCS de volta no cabeçalho FITS
- Sobreposição do catálogo de estrelas VizieR (UCAC4, Gaia DR3)
- Catálogo local de estrelas em FITS BINTABLE (offline, opcional)
- Sobreposição de planos eclíptico e galáctico com aviso de extinção
- Correção de refração atmosférica (topocêntrica, ignora telescópios espaciais)
- Conversão ICRS → CIRS → topocêntrico para resíduos ADES sub-arcseg
  (aberração anual, precessão J2000→data, nutação)

#### Detecção de fontes
- Modelo de fundo por filtro de mediana deslizante (grade de tiles + sigma-clipping)
- Máscara de pixels ruins com interpolação por vizinhos mais próximos
- Detecção de traço/streak por elongação de blob (asteroides rápidos, satélites)
- Ajuste de PSF: Gaussiana elíptica com Levenberg–Marquardt
- ClumpFind: separação de fontes blendadas por contorno de intensidade

#### Fotometria
- Fotometria de abertura (simples e multi-abertura)
- Fotometria diferencial com calibração de zero-point
- Correção de massa de ar (coeficiente de extinção)
- Diálogo de curva de luz
- Diálogo de curva de crescimento
- Painel de estatísticas de região

#### Detecção de objetos em movimento
- Detecção de Objeto em Movimento (MOD) em ≥3 imagens
- Track & Stack com alinhamento por rastreamento de asteroide
- Empilhador de imagens: Adição, Média, Mediana, Sigma-clipped; registro sub-pixel por FFT

#### Calibração
- Calibração bias/dark/flat
- Assistente de calibração (passo a passo)
- Construtor de master dark / master flat
- Frames de calibração por arrastar e soltar

#### Relatórios
- Geração de relatório ADES 2022 XML e PSV
- Flag `<sys>ICRF</sys>` para anotação de sistema de referência preciso
- Diálogo de pré-visualização de relatório MPC com renderização HTML
- Endpoint de submissão direta ao MPC
- Identificação de corpos do Sistema Solar via [IMCCE SkyBoT](https://vo.imcce.fr/webservices/skybot/)
- Consultas de efemérides JPL Horizons
- Seletor de banda de magnitude, movimento próprio, localização do observador a partir de cabeçalhos FITS

#### Importação / exportação de dados
- Exportar imagem como JPEG/PNG
- Importar tabela FITS BINTABLE DAOPHOT/SExtractor como sobreposição de estrelas detectadas
- Importar tabelas de redução IRAF/Astropy (XCENTER/YCENTER, xcentroid/ycentroid, coordenadas de céu)
- Exportar/importar configuração da escola
- Salvar projeto (formato JSON `.gus`)

#### Interface
- Tema de UI Dia / Noite / Automático (Ctrl+Shift+T)
- Interface bilíngue completa: inglês e português brasileiro (1007+ strings)
- Fluxo escolar: modo guiado passo a passo para alunos
- Notificações toast (não bloqueantes)
- Pilha Desfazer/Refazer
- Armazenamento seguro de chave API (Qt Keychain, opcional)
- Diálogo de comparação de sessão

---

### Estrutura do projeto

*Veja a seção [Project Structure](#project-structure) acima — a estrutura é idêntica.*

> **Nota sobre `originals/`:** este diretório contém apenas `CCfits.tar.gz`, uma biblioteca C++
> de FITS usada somente em tempo de compilação. Não é necessária para executar o aplicativo e
> pode ser ignorada por usuários finais. O CMake a extrai automaticamente durante a configuração.
>
> **Nota sobre `tests/`:** a suíte de testes (116 casos core + 23 UI) faz parte do repositório
> mas não é necessária para compilar o aplicativo. Veja [Compilação](#compilação) para compilar
> somente o app.

---

### Requisitos

#### Obrigatórios

| Dependência | Versão | Notas |
|-------------|--------|-------|
| Qt | ≥ 6.4 | Widgets, OpenGL, Charts, Concurrent, Network, Sql, Xml |
| cfitsio | qualquer | I/O FITS (pacote do sistema ou FetchContent) |
| FFTW3 | qualquer | FFT para empilhamento de imagens e espectro de potência |
| SEP | ≥ 1.2 | Biblioteca C do Source Extractor (bundled via FetchContent) |
| spdlog | ≥ 1.11 | Log (bundled via FetchContent) |
| nlohmann/json | ≥ 3.11 | Serialização de arquivo de projeto (bundled) |
| CMake | ≥ 3.20 | Sistema de build |
| C++ | 23 | Compilador: GCC ≥ 13 ou Clang ≥ 16 |

#### Opcionais

| Dependência | Notas |
|-------------|-------|
| QuaZip + Qt6Core5Compat | Extração de arquivos ZIP; `dnf install qt6-qt5compat-devel` |
| libarchive | Extração TAR.GZ/BZ2/XZ, 7Z, RAR; `dnf install libarchive-devel` |
| Qt6Keychain | Armazenamento seguro de chave API; `dnf install qtkeychain-qt6-devel libsecret-devel` |
| Qt6LinguistTools | Compilar traduções `.ts`; `dnf install qt6-linguist` |
| ASTAP | Plate solver offline; baixar em [hnsky.org](https://www.hnsky.org/astap.htm) |

---

### Compilação

Para comandos de instalação por distribuição, veja **[INSTALL.md](INSTALL.md)**.

```bash
# Configurar e compilar (somente o aplicativo)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)

# Executar
./build/bin/AstroFind

# Compilar tudo incluindo testes (opcional)
cmake --build build -j$(nproc)
```

Opcional: instalar no sistema (Linux FHS)

```bash
sudo cmake --install build
```

---

### Testes

```bash
# Testes do núcleo (116 casos)
./build/bin/astrofind_tests

# Testes de UI (23 casos — requer display)
./build/bin/astrofind_ui_tests
```

---

### Licença

[PolyForm Noncommercial License 1.0.0](LICENSE) — livre para uso pessoal, educacional e não comercial.
Copyright (c) 2024–2026 Petrus Silva Costa.

---

### Agradecimentos

O AstroFind foi inspirado e aprendeu com estes excelentes projetos:

| Projeto | Autor(es) | O que aprendemos |
|---------|-----------|------------------|
| [Astrometrica](http://www.astrometrica.at/) | Herbert Raab | Inspiração principal de UX — ferramenta padrão Windows para IASC/MPC |
| [find_orb](https://github.com/Bill-Gray/find_orb) | Bill Gray (Project Pluto) | Formato de saída ADES, submissão MPC, determinação de órbita |
| [umbrella2](https://github.com/mostanes/umbrella2) | CSCF | Algoritmos de detecção de objetos em movimento, ligação de tracklets |
| [Siril](https://gitlab.com/free-astro/siril) | Free-Astro team | Ajuste de PSF, extração de fundo, pipeline de fotometria |
| [astrometry.net](https://github.com/dstndstn/astrometry.net) | Dustin Lang et al. | API REST de plate-solving usada diretamente |
| [IRAF](https://github.com/iraf-community/iraf) | NOAO / comunidade | Conceitos de fotometria de abertura e convenções de colunas de tabela |
| [GDL](https://github.com/gnudatalanguage/gdl) | equipe GDL | PSF elíptica / MPFIT, mediana deslizante, algoritmos FFT |
| [NEMO](https://github.com/teuben/nemo) | P. Teuben | Máscaras de pixels ruins, blob/streak, ClumpFind, histograma, Laplaciano, WCS |
| [Boost.Astronomy](https://github.com/BoostGSoC19/astronomy) | GSoC 2019 | Refração atmosférica, CIRS/ADES, sobreposições eclíptica/galáctica |

Bibliotecas utilizadas:
[Qt6](https://github.com/qt/qtbase) ·
[cfitsio](https://github.com/HEASARC/cfitsio) ·
[CCfits 2.7](https://heasarc.gsfc.nasa.gov/fitsio/CCfits/) ·
[FFTW3](https://github.com/FFTW/fftw3) ·
[SEP](https://github.com/kbarbary/sep) ·
[spdlog](https://github.com/gabime/spdlog) ·
[nlohmann/json](https://github.com/nlohmann/json) ·
[Catch2](https://github.com/catchorg/Catch2) ·
[QuaZip](https://github.com/stachenov/quazip) ·
[qt-keychain](https://github.com/frankosterfeld/qtkeychain) ·
[libarchive](https://www.libarchive.org/)
