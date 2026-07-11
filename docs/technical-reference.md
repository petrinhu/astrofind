# AstroFind: Technical Reference / Referência Técnica

**Algorithms, Coordinate Systems, and Numerical Methods / Algoritmos, Sistemas de
Coordenadas e Métodos Numéricos**

> **Audience / Público-alvo:** Contributors and advanced users who need to
> understand the mathematical underpinning of AstroFind's reduction pipeline. /
> Contribuidores e usuários avançados que precisam entender a base matemática do
> pipeline de redução do AstroFind.
> All equations use SI or astronomical units as noted. / Todas as equações usam
> unidades SI ou astronômicas, conforme indicado.
>
> **Last reviewed / Última revisão:** 2026-07-10
> **Owner:** Petrus Silva Costa
> **Applies to / Aplica-se a:** AstroFind v0.9.0+

---

## Table of Contents / Índice

1. [FITS File Structure / Estrutura de Arquivo FITS](#1-fits-file-structure--estrutura-de-arquivo-fits)
2. [Supported Image Formats / Formatos de Imagem Suportados](#2-supported-image-formats--formatos-de-imagem-suportados)
3. [WCS Pipeline: Pixel to Sky / Pipeline WCS: Pixel para Céu](#3-wcs-pipeline-pixel-to-sky--pipeline-wcs-pixel-para-céu)
4. [WCS Projections / Projeções WCS](#4-wcs-projections--projeções-wcs)
5. [Atmospheric Refraction / Refração Atmosférica](#5-atmospheric-refraction--refração-atmosférica)
6. [Coordinate Chain: ICRS → CIRS → Topocentric / Cadeia de Coordenadas: ICRS → CIRS → Topocêntrico](#6-coordinate-chain-icrs--cirs--topocentric--cadeia-de-coordenadas-icrs--cirs--topocêntrico)
7. [Annual Aberration / Aberração Anual](#7-annual-aberration--aberração-anual)
8. [Precession and Nutation / Precessão e Nutação](#8-precession-and-nutation--precessão-e-nutação)
9. [Elliptical PSF Fitting (Levenberg-Marquardt) / Ajuste de PSF Elíptica (Levenberg-Marquardt)](#9-elliptical-psf-fitting-levenberg-marquardt--ajuste-de-psf-elíptica-levenberg-marquardt)
10. [Aperture Photometry and Zero-Point / Fotometria de Abertura e Zero-Point](#10-aperture-photometry-and-zero-point--fotometria-de-abertura-e-zero-point)
11. [Differential Photometry / Fotometria Diferencial](#11-differential-photometry--fotometria-diferencial)
12. [Airmass / Massa de Ar](#12-airmass--massa-de-ar)
13. [Julian Date / Data Juliana](#13-julian-date--data-juliana)
14. [Ecliptic and Galactic Coordinates / Coordenadas Eclípticas e Galácticas](#14-ecliptic-and-galactic-coordinates--coordenadas-eclípticas-e-galácticas)
15. [FFT Image Registration (Phase Correlation) / Registro de Imagem por FFT (Correlação de Fase)](#15-fft-image-registration-phase-correlation--registro-de-imagem-por-fft-correlação-de-fase)
16. [Bad Pixel Correction / Correção de Pixels Ruins](#16-bad-pixel-correction--correção-de-pixels-ruins)
17. [Streak Detection by Blob Elongation / Detecção de Traço por Elongação de Blob](#17-streak-detection-by-blob-elongation--detecção-de-traço-por-elongação-de-blob)
18. [ClumpFind: Blended Source Separation / ClumpFind: Separação de Fontes Blendadas](#18-clumpfind-blended-source-separation--clumpfind-separação-de-fontes-blendadas)
19. [Sliding-Median Background Estimator / Estimador de Fundo por Mediana Deslizante](#19-sliding-median-background-estimator--estimador-de-fundo-por-mediana-deslizante)
20. [2-D Power Spectrum / Espectro de Potência 2-D](#20-2-d-power-spectrum--espectro-de-potência-2-d)
21. [ADES / MPC Report Format / Formato de Relatório ADES / MPC](#21-ades--mpc-report-format--formato-de-relatório-ades--mpc)

---

## 1. FITS File Structure / Estrutura de Arquivo FITS

### 🇬🇧 English

A FITS file ("FITS" = Flexible Image Transport System, the standard file format
for astronomical images) consists of one or more **Header/Data Units (HDUs)**. The
primary HDU is at offset 0; additional extensions begin on 2880-byte block
boundaries.

**Header:**

Keywords are 80-character records of the form `KEY     = value / comment`.
AstroFind reads:

| Keyword | Purpose |
|---------|---------|
| `NAXIS`, `NAXIS1`, `NAXIS2`, `NAXIS3` | Image dimensions |
| `BITPIX` | Pixel data type (8, 16, 32, -32, -64) |
| `BSCALE`, `BZERO` | Physical value: $p = \text{BSCALE} \times \text{raw} + \text{BZERO}$ |
| `DATE-OBS` | ISO 8601 observation start (UTC) |
| `EXPTIME` / `EXPOSURE` | Integration time in seconds |
| `CRPIX1/2` | Reference pixel (1-based) |
| `CRVAL1/2` | Sky coordinate at reference pixel (degrees) |
| `CD1_1`, `CD1_2`, `CD2_1`, `CD2_2` | CD matrix (degrees/pixel): the linear transform from pixel offsets to sky-plane offsets |
| `CTYPE1/2` | Projection type (e.g., `RA---TAN`, `DEC--TAN`) |
| `GAIN` | Electrons per ADU (ADU = Analog-to-Digital Unit, the raw integer count a sensor reports) |
| `RDNOISE` | Read noise in electrons |

**Cube handling:**

When `NAXIS3 > 3`, each plane $k$ is read with cfitsio `fpixel[3] = {1, 1, k+1}`. A
`cubeDepth` field stores $\text{NAXIS3}$. RGB images have `NAXIS3 = 3`.

### 🇧🇷 Português

Um arquivo FITS ("FITS" = Flexible Image Transport System, o formato de arquivo
padrão para imagens astronômicas) consiste em um ou mais **Header/Data Units
(HDUs, unidades de cabeçalho/dados)**. O HDU primário está no offset 0; extensões
adicionais começam em limites de bloco de 2880 bytes.

**Cabeçalho:**

As keywords (palavras-chave) são registros de 80 caracteres no formato
`KEY     = valor / comentário`. O AstroFind lê:

| Keyword | Propósito |
|---------|---------|
| `NAXIS`, `NAXIS1`, `NAXIS2`, `NAXIS3` | Dimensões da imagem |
| `BITPIX` | Tipo de dado do pixel (8, 16, 32, -32, -64) |
| `BSCALE`, `BZERO` | Valor físico: $p = \text{BSCALE} \times \text{raw} + \text{BZERO}$ |
| `DATE-OBS` | Início da observação em ISO 8601 (UTC) |
| `EXPTIME` / `EXPOSURE` | Tempo de integração em segundos |
| `CRPIX1/2` | Pixel de referência (baseado em 1) |
| `CRVAL1/2` | Coordenada celeste no pixel de referência (graus) |
| `CD1_1`, `CD1_2`, `CD2_1`, `CD2_2` | Matriz CD (graus/pixel): a transformação linear de deslocamentos de pixel para deslocamentos no plano do céu |
| `CTYPE1/2` | Tipo de projeção (ex.: `RA---TAN`, `DEC--TAN`) |
| `GAIN` | Elétrons por ADU (ADU = Analog-to-Digital Unit, a contagem inteira bruta que um sensor reporta) |
| `RDNOISE` | Ruído de leitura em elétrons |

**Manipulação de cubo:**

Quando `NAXIS3 > 3`, cada plano $k$ é lido com `fpixel[3] = {1, 1, k+1}` do cfitsio.
Um campo `cubeDepth` armazena $\text{NAXIS3}$. Imagens RGB têm `NAXIS3 = 3`.

---

## 2. Supported Image Formats / Formatos de Imagem Suportados

### 🇬🇧 English

**SER:**

Binary video format. Header is 178 bytes:

```
Offset 14: ColorID (uint32) - 0=mono, 8=Bayer, 100=RGB
Offset 26: Width  (uint32)
Offset 30: Height (uint32)
Offset 34: PixelDepthPerPlane (uint32) - 8 or 16
Offset 38: FrameCount (uint32)
```

Frame data starts at offset 178. Each frame is `Width x Height x (PixelDepth/8)`
bytes. 16-bit frames are little-endian.

**XISF:**

XML-based format (PixInsight). The file begins with an 8-byte magic `XISF0100`,
followed by a 4-byte little-endian XML header length, then the XML block. The
`<Image>` element carries:

```xml
<Image geometry="W:H:channels" sampleFormat="Float32" location="attachment:offset:size"/>
```

Pixel data at `offset` bytes from file start. AstroFind reads Float32 and UInt16.

### 🇧🇷 Português

**SER:**

Formato binário de vídeo. O cabeçalho tem 178 bytes:

```
Offset 14: ColorID (uint32) - 0=mono, 8=Bayer, 100=RGB
Offset 26: Width  (uint32)
Offset 30: Height (uint32)
Offset 34: PixelDepthPerPlane (uint32) - 8 ou 16
Offset 38: FrameCount (uint32)
```

Os dados de frame começam no offset 178. Cada frame tem
`Width x Height x (PixelDepth/8)` bytes. Frames de 16 bits são little-endian
(byte menos significativo primeiro).

**XISF:**

Formato baseado em XML (PixInsight). O arquivo começa com um "magic number" (marca
de identificação do formato) de 8 bytes `XISF0100`, seguido por um comprimento de
cabeçalho XML little-endian de 4 bytes, depois o bloco XML. O elemento `<Image>`
carrega:

```xml
<Image geometry="W:H:channels" sampleFormat="Float32" location="attachment:offset:size"/>
```

Os dados de pixel ficam no offset `offset` a partir do início do arquivo. O
AstroFind lê Float32 e UInt16.

---

## 3. WCS Pipeline: Pixel to Sky / Pipeline WCS: Pixel para Céu

### 🇬🇧 English

"WCS" = World Coordinate System, the map that links a pixel position in the image
to a real position on the sky (right ascension/declination). The three-step
Calabretta & Greisen (2002) pipeline:

**Step 1: Pixel → Intermediate World Coordinates (IWC)**

$$
\begin{pmatrix} x \\ y \end{pmatrix} =
\begin{pmatrix} CD_{1,1} & CD_{1,2} \\ CD_{2,1} & CD_{2,2} \end{pmatrix}
\begin{pmatrix} p_1 - CRPIX_1 \\ p_2 - CRPIX_2 \end{pmatrix}
$$

where $(p_1, p_2)$ are 1-based pixel coordinates and $(x, y)$ are in degrees.

**Step 2: IWC → Native Spherical Coordinates $(\phi, \theta)$**

Projection-specific (see §4). Result is native longitude $\phi$ and latitude
$\theta$ in degrees.

**Step 3: Native → Celestial (RA, Dec)**

Spherical rotation using the reference point $(\alpha_0, \delta_0) = (CRVAL_1,
CRVAL_2)$ and the native latitude of the fiducial point $\theta_0$ (typically 90°
for zenithal projections):

$$
\alpha = \alpha_0 + \arctan\!\left(
  \frac{-\cos\theta\,\sin(\phi - \phi_p)}
       {\sin\theta\,\cos\delta_0 - \cos\theta\,\cos(\phi - \phi_p)\,\sin\delta_0}
\right)
$$

$$
\delta = \arcsin\!\left(
  \sin\theta\,\sin\delta_0 + \cos\theta\,\cos(\phi - \phi_p)\,\cos\delta_0
\right)
$$

where $\phi_p = 180°$ for $\delta_0 \geq \theta_0$, else $0°$.

### 🇧🇷 Português

"WCS" = World Coordinate System (Sistema de Coordenadas Mundiais), o mapa que liga
uma posição de pixel na imagem a uma posição real no céu (ascensão reta/
declinação). O pipeline de três etapas de Calabretta & Greisen (2002):

**Etapa 1: Pixel → Coordenadas Mundiais Intermediárias (IWC)**

$$
\begin{pmatrix} x \\ y \end{pmatrix} =
\begin{pmatrix} CD_{1,1} & CD_{1,2} \\ CD_{2,1} & CD_{2,2} \end{pmatrix}
\begin{pmatrix} p_1 - CRPIX_1 \\ p_2 - CRPIX_2 \end{pmatrix}
$$

onde $(p_1, p_2)$ são coordenadas de pixel baseadas em 1 e $(x, y)$ estão em graus.

**Etapa 2: IWC → Coordenadas Esféricas Nativas $(\phi, \theta)$**

Específico da projeção (ver §4). O resultado é a longitude nativa $\phi$ e a
latitude $\theta$ em graus.

**Etapa 3: Nativa → Celeste (RA, Dec)**

Rotação esférica usando o ponto de referência $(\alpha_0, \delta_0) = (CRVAL_1,
CRVAL_2)$ e a latitude nativa do ponto fiducial $\theta_0$ (tipicamente 90° para
projeções zenitais):

$$
\alpha = \alpha_0 + \arctan\!\left(
  \frac{-\cos\theta\,\sin(\phi - \phi_p)}
       {\sin\theta\,\cos\delta_0 - \cos\theta\,\cos(\phi - \phi_p)\,\sin\delta_0}
\right)
$$

$$
\delta = \arcsin\!\left(
  \sin\theta\,\sin\delta_0 + \cos\theta\,\cos(\phi - \phi_p)\,\cos\delta_0
\right)
$$

onde $\phi_p = 180°$ para $\delta_0 \geq \theta_0$, senão $0°$.

---

## 4. WCS Projections / Projeções WCS

### 🇬🇧 English

**TAN, Gnomonic (default):**

$$
\phi = \arctan\!\left(\frac{x}{-y}\right), \quad
\theta = \arctan\!\left(\frac{180°}{\pi\,\sqrt{x^2 + y^2}}\right)
$$

Valid for small fields of view. Straight lines in the image correspond to great
circles.

**SIN, Slant Orthographic:**

$$
\phi = \arctan\!\left(\frac{x}{-y}\right), \quad
\theta = \arccos\!\left(\frac{\pi\,\sqrt{x^2+y^2}}{180°}\right)
$$

Used by radio interferometry (VLBI images).

**ARC, Zenithal Equidistant:**

$$
R_\theta = \frac{180°}{\pi}\,(90° - \theta), \quad
\phi = \arctan\!\left(\frac{x}{-y}\right)
$$

Preserves great-circle distances from the projection centre.

**STG, Stereographic:**

$$
R_\theta = \frac{360°}{\pi}\,\tan\!\left(\frac{90° - \theta}{2}\right)
$$

Conformal ("conformal" = preserves local angles/shapes); circles on the sphere map
to circles in the plane.

**MER, Mercator:**

$$
x = \phi - \phi_0, \quad y = \frac{180°}{\pi}\,\ln\!\left(\tan\!\left(\frac{90°+\delta}{2}\right)\right)
$$

Cylindrical conformal. Used for full-sky survey maps.

**GLS / CAR, Global Sinusoidal / Plate Carrée:**

For GLS: $x = (\alpha - \alpha_0)\cos\delta$, $y = \delta - \delta_0$.
For CAR (special case $\delta_0 = 0$): equirectangular.

**AIT, Hammer-Aitoff:**

$$
z = \sqrt{1 + \cos\delta\,\cos(\alpha/2)}, \quad
x = \frac{2\cos\delta\,\sin(\alpha/2)}{z}, \quad
y = \frac{\sin\delta}{z}
$$

Equal-area full-sky projection. Used for CMB and survey coverage maps.

### 🇧🇷 Português

**TAN, Gnomônica (padrão):**

$$
\phi = \arctan\!\left(\frac{x}{-y}\right), \quad
\theta = \arctan\!\left(\frac{180°}{\pi\,\sqrt{x^2 + y^2}}\right)
$$

Válida para campos de visão pequenos. Linhas retas na imagem correspondem a
grandes círculos.

**SIN, Ortográfica Inclinada:**

$$
\phi = \arctan\!\left(\frac{x}{-y}\right), \quad
\theta = \arccos\!\left(\frac{\pi\,\sqrt{x^2+y^2}}{180°}\right)
$$

Usada em interferometria de rádio (imagens VLBI).

**ARC, Zenital Equidistante:**

$$
R_\theta = \frac{180°}{\pi}\,(90° - \theta), \quad
\phi = \arctan\!\left(\frac{x}{-y}\right)
$$

Preserva distâncias de grande círculo a partir do centro da projeção.

**STG, Estereográfica:**

$$
R_\theta = \frac{360°}{\pi}\,\tan\!\left(\frac{90° - \theta}{2}\right)
$$

Conforme ("conforme" = preserva ângulos/formas locais); círculos na esfera mapeiam
para círculos no plano.

**MER, Mercator:**

$$
x = \phi - \phi_0, \quad y = \frac{180°}{\pi}\,\ln\!\left(\tan\!\left(\frac{90°+\delta}{2}\right)\right)
$$

Cilíndrica conforme. Usada em mapas de levantamento de céu completo.

**GLS / CAR, Senoidal Global / Plate Carrée:**

Para GLS: $x = (\alpha - \alpha_0)\cos\delta$, $y = \delta - \delta_0$.
Para CAR (caso especial $\delta_0 = 0$): equiretangular.

**AIT, Hammer-Aitoff:**

$$
z = \sqrt{1 + \cos\delta\,\cos(\alpha/2)}, \quad
x = \frac{2\cos\delta\,\sin(\alpha/2)}{z}, \quad
y = \frac{\sin\delta}{z}
$$

Projeção de céu completo de igual área. Usada em mapas de CMB (radiação cósmica de
fundo) e cobertura de levantamento.

---

## 5. Atmospheric Refraction / Refração Atmosférica

### 🇬🇧 English

> **Last reviewed:** 2026-07-10 (AUD-DOC-1). This section previously described a
> two-branch model (pressure/temperature-corrected formula above 15°, Bennett
> below 15°) that does not match the implementation. It has been rewritten to
> describe `applyRefractionCorrection()` (`src/core/Astronomy.cpp`) as it
> actually exists: the Bennett (1982) formula, applied unconditionally down to a
> fixed altitude gate, with no pressure/temperature input.

AstroFind uses a single formula for atmospheric refraction, the Bennett (1982)
approximation, applied uniformly at all altitudes (no separate high-altitude
branch):

$$
R = \frac{1.02}{\tan\!\left(a + \frac{10.3}{a + 5.11}\right)}
$$

where $a$ is the apparent (observed) altitude in degrees, giving $R$ in
arcminutes.

**Altitude gate:** the correction is skipped entirely (returns 0) when the
apparent altitude is below 1°, the formula becomes numerically unstable near the
horizon. There is no 15° branch and no separate low-altitude formula; the same
Bennett expression is used from 1° up to the zenith.

**No pressure/temperature input:** unlike some refraction models, this
implementation does not accept, and its function signature does not expose,
atmospheric pressure or temperature parameters. It is a fixed-atmosphere
approximation (Bennett's formula already folds standard sea-level conditions into
its empirical constants). If site-specific P/T correction is ever desired, it
would need to be implemented as a new parameter and formula (e.g. the $R = R_0
\cdot P/P_0 \cdot T_0/T \cdot \tan z$ style correction), not assumed to already
exist.

**Space telescopes:** the caller (`MainWindow_measurement.cpp`) gates the call
behind `isSpaceTelescope`, refraction is only applied for ground-based
observations, since there is no atmosphere to correct for from orbit.

The correction is applied to each measured RA/Dec before writing to the ADES
report. The log panel shows the applied correction as `Refraction correction: X"
(R=Y')`.

### 🇧🇷 Português

> **Última revisão:** 2026-07-10 (AUD-DOC-1). Esta seção antes descrevia um
> modelo de dois ramos (fórmula corrigida por pressão/temperatura acima de 15°,
> Bennett abaixo de 15°) que não correspondia à implementação. Foi reescrita para
> descrever `applyRefractionCorrection()` (`src/core/Astronomy.cpp`) como ela
> realmente existe: a fórmula de Bennett (1982), aplicada incondicionalmente até
> um teto fixo de altitude, sem entrada de pressão/temperatura.

O AstroFind usa uma única fórmula para refração atmosférica, a aproximação de
Bennett (1982), aplicada uniformemente em todas as altitudes (sem ramo separado
para alta altitude):

$$
R = \frac{1.02}{\tan\!\left(a + \frac{10.3}{a + 5.11}\right)}
$$

onde $a$ é a altitude aparente (observada) em graus, dando $R$ em arcominutos.

**Teto de altitude:** a correção é completamente pulada (retorna 0) quando a
altitude aparente está abaixo de 1°, a fórmula fica numericamente instável perto
do horizonte. Não há ramo de 15° nem fórmula separada de baixa altitude; a mesma
expressão de Bennett é usada de 1° até o zênite.

**Sem entrada de pressão/temperatura:** ao contrário de alguns modelos de
refração, esta implementação não aceita, e a assinatura de sua função não expõe,
parâmetros de pressão ou temperatura atmosférica. É uma aproximação de atmosfera
fixa (a fórmula de Bennett já embute condições padrão ao nível do mar em suas
constantes empíricas). Se uma correção P/T específica do local for desejada no
futuro, precisaria ser implementada como um novo parâmetro e fórmula (ex.: a
correção estilo $R = R_0 \cdot P/P_0 \cdot T_0/T \cdot \tan z$), não presumida
como já existente.

**Telescópios espaciais:** o chamador (`MainWindow_measurement.cpp`) protege a
chamada atrás de `isSpaceTelescope`; a refração só é aplicada para observações
terrestres, já que não há atmosfera a corrigir a partir da órbita.

A correção é aplicada a cada RA/Dec medido antes de escrever no relatório ADES. O
painel de log mostra a correção aplicada como `Refraction correction: X" (R=Y')`.

---

## 6. Coordinate Chain: ICRS → CIRS → Topocentric / Cadeia de Coordenadas: ICRS → CIRS → Topocêntrico

### 🇬🇧 English

> **Updated during audit remediation (AUD-CORR-4, Onda 2, 2026-07-10).** The
> chain described in earlier revisions of this document (precession → nutation →
> aberration → topocentric) was **never actually wired into the pipeline**:
> `applyPrecessionJ2000ToDate()` and `applyNutation()` had zero callers anywhere
> in `src/` and have since been **removed** as dead code. What AstroFind actually
> does is described below.

"ICRS" (International Celestial Reference System) is the modern, essentially
fixed reference frame for star catalogs. "CIRS" (Celestial Intermediate Reference
System) is an intermediate frame used in rigorous coordinate reductions.
"Topocentric" means as seen from the observer's actual location on Earth's
surface, rather than from Earth's center.

AstroFind's plate solution (from `astrometry.net`, or an equivalent external
solver, read from the FITS WCS header) is fit directly against a modern
astrometric catalog, Gaia DR2/DR3, UCAC4, or 2MASS, **at the epoch of
observation**. That fit absorbs, as a single field-wide systematic, everything a
classical precession/nutation/aberration chain would otherwise need to apply
explicitly:

- **Precession/nutation** ("precession" = the slow, ~26,000-year wobble of
  Earth's rotation axis; "nutation" = a smaller, faster wobble on top of it): the
  catalog itself is expressed in the ICRS frame, and the plate solve maps image
  pixels straight to ICRS sky coordinates, there is no separate "mean equinox of
  J2000" intermediate frame in this pipeline that would need precessing to the
  date of observation.
- **Annual aberration** (the apparent shift of a star's position caused by
  Earth's orbital motion, like slanted rain seen from a moving car): a WCS
  solution calibrated against ICRS reference stars already contains the
  aberration displacement of those very reference stars at the moment of
  exposure, so the target's measured position inherits the same (correct)
  aberration automatically.

Applying precession, nutation, or aberration **again** in post-processing on top
of an already-astrometrically-solved position would **double-correct** the
coordinate, moving it away from truth by roughly the size of the correction
itself (up to ~20″ for aberration, tens of arcseconds per century for
precession). This is why AstroFind does not implement those three steps as active
corrections.

What AstroFind **does** apply to the measured position before it is written to
the ADES/MPC report:

1. **WCS pixel → sky** (§3/§4): gives the ICRS-aligned apparent position directly
   from the plate solution.
2. **Catalog proper-motion propagation** (`applyProperMotion()`): used when
   matching/calibrating against catalog stars at the image epoch, not applied to
   the target itself.
3. **Atmospheric refraction removal** (`applyRefractionCorrection()`, §5): the
   one frame correction that IS real and applied: it converts the apparent
   (refracted) altitude/azimuth back to the true topocentric direction. Skipped
   for space telescopes (`isSpaceTelescope`, no atmosphere).

`annualAberrationComponents()` (§7) is retained and still has one real call site
(`MainWindow::runMeasurePipeline`), but purely to **log** the aberration magnitude
at the epoch as a diagnostic for the observer, its result is never added to or
subtracted from the reported coordinate.

The ADES XML output tags coordinates with `<sys>ICRF</sys>` because the WCS plate
solution is itself calibrated against an ICRS-aligned catalog; this is not a
claim that AstroFind applies its own separate aberration/precession/nutation
correction.

### 🇧🇷 Português

> **Atualizado durante a remediação da auditoria (AUD-CORR-4, Onda 2,
> 2026-07-10).** A cadeia descrita em revisões anteriores deste documento
> (precessão → nutação → aberração → topocêntrico) **nunca foi de fato conectada
> ao pipeline**: `applyPrecessionJ2000ToDate()` e `applyNutation()` não tinham
> nenhum chamador em lugar nenhum de `src/` e desde então foram **removidas** como
> código morto. O que o AstroFind realmente faz está descrito abaixo.

"ICRS" (International Celestial Reference System, Sistema de Referência Celeste
Internacional) é o referencial moderno e essencialmente fixo usado por catálogos
de estrelas. "CIRS" (Celestial Intermediate Reference System) é um referencial
intermediário usado em reduções de coordenadas rigorosas. "Topocêntrico" significa
visto a partir da localização real do observador na superfície da Terra, em vez
do centro da Terra.

A solução de plate-solve do AstroFind (do `astrometry.net`, ou um solver externo
equivalente, lido do cabeçalho WCS do FITS) é ajustada diretamente contra um
catálogo astrométrico moderno, Gaia DR2/DR3, UCAC4, ou 2MASS, **na época da
observação**. Esse ajuste absorve, como um único sistemático de todo o campo,
tudo que uma cadeia clássica de precessão/nutação/aberração de outra forma
precisaria aplicar explicitamente:

- **Precessão/nutação** ("precessão" = a oscilação lenta, de ~26.000 anos, do
  eixo de rotação da Terra; "nutação" = uma oscilação menor e mais rápida sobre
  ela): o próprio catálogo é expresso no referencial ICRS, e o plate-solve mapeia
  pixels da imagem diretamente para coordenadas celestes ICRS; não há um
  referencial intermediário separado de "equinócio médio de J2000" neste pipeline
  que precisaria ser precessionado até a data da observação.
- **Aberração anual** (o deslocamento aparente da posição de uma estrela causado
  pelo movimento orbital da Terra, como chuva inclinada vista de um carro em
  movimento): uma solução WCS calibrada contra estrelas de referência ICRS já
  contém o deslocamento de aberração dessas mesmas estrelas de referência no
  momento da exposição, então a posição medida do alvo herda automaticamente a
  mesma aberração (correta).

Aplicar precessão, nutação ou aberração **novamente** em pós-processamento sobre
uma posição já resolvida astrometricamente causaria **dupla correção** da
coordenada, afastando-a da verdade em aproximadamente o tamanho da própria
correção (até ~20″ para aberração, dezenas de arcossegundos por século para
precessão). É por isso que o AstroFind não implementa essas três etapas como
correções ativas.

O que o AstroFind **de fato** aplica à posição medida antes de escrevê-la no
relatório ADES/MPC:

1. **WCS pixel → céu** (§3/§4): dá a posição aparente alinhada ao ICRS
   diretamente da solução de plate-solve.
2. **Propagação de movimento próprio de catálogo** (`applyProperMotion()`): usada
   ao casar/calibrar contra estrelas de catálogo na época da imagem, não aplicada
   ao próprio alvo.
3. **Remoção de refração atmosférica** (`applyRefractionCorrection()`, §5): a
   única correção de referencial que É real e aplicada: converte a
   altitude/azimute aparente (refratado) de volta à direção topocêntrica
   verdadeira. Pulada para telescópios espaciais (`isSpaceTelescope`, sem
   atmosfera).

`annualAberrationComponents()` (§7) foi mantida e ainda tem um único ponto de
chamada real (`MainWindow::runMeasurePipeline`), mas puramente para **registrar
em log** a magnitude da aberração na época como diagnóstico para o observador; seu
resultado nunca é somado ou subtraído da coordenada reportada.

A saída XML do ADES marca coordenadas com `<sys>ICRF</sys>` porque a própria
solução de plate-solve WCS é calibrada contra um catálogo alinhado ao ICRS; isso
não é uma alegação de que o AstroFind aplica sua própria correção separada de
aberração/precessão/nutação.

---

## 7. Annual Aberration / Aberração Anual

### 🇬🇧 English

The stellar aberration due to Earth's orbital velocity $\mathbf{v}_\oplus$:

$$
\Delta\alpha = \frac{1}{c}\left(-v_x\sin\alpha + v_y\cos\alpha\right) / \cos\delta
$$

$$
\Delta\delta = \frac{1}{c}\left(-v_x\cos\alpha\sin\delta - v_y\sin\alpha\sin\delta + v_z\cos\delta\right)
$$

where $v_x, v_y, v_z$ are the components of Earth's velocity in the equatorial
frame, derived from the Sun's ecliptic longitude $\lambda_\odot$:

$$
v_x = -\kappa\,\sin\lambda_\odot, \quad
v_y =  \kappa\,\cos\lambda_\odot\,\cos\varepsilon, \quad
v_z =  \kappa\,\cos\lambda_\odot\,\sin\varepsilon
$$

The constant of aberration $\kappa = 20.49552''$; $\varepsilon$ is the obliquity
of the ecliptic (the tilt of Earth's rotation axis relative to its orbital
plane).

The Sun's ecliptic longitude is computed from the Julian date via
`sunEclipticPosition()`.

> **Not applied to the reported coordinate** (see §6): `annualAberrationComponents()`
> is computed only for the diagnostic log line `Frame: ICRF, annual aberration at
> epoch: ...`. The WCS plate solution already absorbs this shift; applying it
> again would double-correct.

### 🇧🇷 Português

A aberração estelar devida à velocidade orbital da Terra $\mathbf{v}_\oplus$:

$$
\Delta\alpha = \frac{1}{c}\left(-v_x\sin\alpha + v_y\cos\alpha\right) / \cos\delta
$$

$$
\Delta\delta = \frac{1}{c}\left(-v_x\cos\alpha\sin\delta - v_y\sin\alpha\sin\delta + v_z\cos\delta\right)
$$

onde $v_x, v_y, v_z$ são as componentes da velocidade da Terra no referencial
equatorial, derivadas da longitude eclíptica do Sol $\lambda_\odot$:

$$
v_x = -\kappa\,\sin\lambda_\odot, \quad
v_y =  \kappa\,\cos\lambda_\odot\,\cos\varepsilon, \quad
v_z =  \kappa\,\cos\lambda_\odot\,\sin\varepsilon
$$

A constante de aberração $\kappa = 20.49552''$; $\varepsilon$ é a obliquidade da
eclíptica (a inclinação do eixo de rotação da Terra em relação ao seu plano
orbital).

A longitude eclíptica do Sol é calculada a partir da data juliana via
`sunEclipticPosition()`.

> **Não aplicada à coordenada reportada** (ver §6): `annualAberrationComponents()`
> é calculada só para a linha de log de diagnóstico `Frame: ICRF, annual
> aberration at epoch: ...`. A solução de plate-solve WCS já absorve esse
> deslocamento; aplicá-lo de novo causaria dupla correção.

---

## 8. Precession and Nutation / Precessão e Nutação

### 🇬🇧 English

> **Removed (AUD-CORR-4, audit remediation Onda 2, 2026-07-10).** Earlier
> revisions of this document described `applyPrecessionJ2000ToDate()` (IAU
> 1976/Lieske rotation angles) and `applyNutation()` (9-term IAU 1980 series) as
> part of the coordinate chain. A grep across `src/` confirmed **zero callers**
> for either function outside their own declaration/definition, dead code that
> had never been wired into any pipeline, and had zero test coverage. Both
> functions were deleted from `src/core/Astronomy.{h,cpp}`.
>
> AstroFind does **not** implement precession or nutation as separate
> corrections. See §6 for why: the WCS plate solution is fit directly against an
> ICRS catalog at the epoch of observation, which makes a standalone
> precession/nutation step both unnecessary and, if added naively on top of the
> plate-solved position, a double-correction.
>
> If a future need arises to support historical J2000.0 mean-equinox catalog
> input that is **not** run through a modern epoch-of-date plate solve, the
> precession/nutation formulas (Lieske 1977 rotation angles; 9-term IAU 1980
> nutation series) can be reintroduced, but must come with unit tests against an
> external oracle (Meeus/SOFA/Horizons) from the start, and a clearly gated call
> site so they are never applied downstream of an already-solved WCS position.

### 🇧🇷 Português

> **Removida (AUD-CORR-4, remediação de auditoria Onda 2, 2026-07-10).**
> Revisões anteriores deste documento descreviam `applyPrecessionJ2000ToDate()`
> (ângulos de rotação IAU 1976/Lieske) e `applyNutation()` (série de 9 termos IAU
> 1980) como parte da cadeia de coordenadas. Uma busca em todo `src/` confirmou
> **zero chamadores** para qualquer uma das duas funções fora de sua própria
> declaração/definição: código morto que nunca havia sido conectado a nenhum
> pipeline, e tinha zero cobertura de teste. Ambas as funções foram removidas de
> `src/core/Astronomy.{h,cpp}`.
>
> O AstroFind **não** implementa precessão ou nutação como correções separadas.
> Ver §6 para o motivo: a solução WCS de plate-solve é ajustada diretamente
> contra um catálogo ICRS na época da observação, o que torna uma etapa isolada
> de precessão/nutação tanto desnecessária quanto, se adicionada ingenuamente
> sobre a posição já resolvida, uma dupla correção.
>
> Se surgir uma necessidade futura de suportar entrada de catálogo de equinócio
> médio J2000.0 histórico que **não** passa por um plate-solve moderno na época
> de observação, as fórmulas de precessão/nutação (ângulos de rotação Lieske
> 1977; série de nutação IAU 1980 de 9 termos) podem ser reintroduzidas, mas
> precisam vir com testes unitários contra um oráculo externo (Meeus/SOFA/
> Horizons) desde o início, e um ponto de chamada claramente protegido para que
> nunca sejam aplicadas depois de uma posição WCS já resolvida.

---

## 9. Elliptical PSF Fitting (Levenberg-Marquardt) / Ajuste de PSF Elíptica (Levenberg-Marquardt)

### 🇬🇧 English

"PSF" = Point Spread Function, the blur pattern a single point of light (a star)
makes on the sensor. The 2-D elliptical Gaussian model with 6 free parameters:

$$
I(x, y) = A\,\exp\!\left(-\frac{1}{2}\left[
  \left(\frac{x'(x,y)}{\sigma_x}\right)^2 +
  \left(\frac{y'(x,y)}{\sigma_y}\right)^2
\right]\right) + B
$$

where the rotated coordinates are:

$$
x'(x,y) = (x - x_0)\cos\theta + (y - y_0)\sin\theta
$$
$$
y'(x,y) = -(x - x_0)\sin\theta + (y - y_0)\cos\theta
$$

Parameters: amplitude $A$, centroid $(x_0, y_0)$, widths $(\sigma_x, \sigma_y)$,
rotation $\theta$, and sky background $B$.

The **Levenberg-Marquardt** iteration (an optimization algorithm that blends two
simpler methods to converge reliably and fast) minimises:

$$
\chi^2 = \sum_{i,j}\frac{\left(I_\text{obs}(x_i, y_j) - I_\text{model}(x_i, y_j)\right)^2}{\sigma_{ij}^2}
$$

The update step blends gradient descent and Gauss-Newton:

$$
\left(J^T W J + \lambda\,\text{diag}(J^T W J)\right)\Delta\mathbf{p} = J^T W \mathbf{r}
$$

Convergence: $\|\Delta\mathbf{p}\| < 10^{-6}$ or 200 iterations. The FWHM values
("FWHM" = Full Width at Half Maximum, the width of the star's light profile at
half its peak brightness, the standard sharpness/focus metric) are:

$$
\text{FWHM}_{a,b} = 2\sqrt{2\ln 2}\,\sigma_{x,y}
$$

### 🇧🇷 Português

"PSF" = Point Spread Function (Função de Espalhamento de Ponto), o padrão de
borrão que um único ponto de luz (uma estrela) faz no sensor. O modelo gaussiano
elíptico 2-D com 6 parâmetros livres:

$$
I(x, y) = A\,\exp\!\left(-\frac{1}{2}\left[
  \left(\frac{x'(x,y)}{\sigma_x}\right)^2 +
  \left(\frac{y'(x,y)}{\sigma_y}\right)^2
\right]\right) + B
$$

onde as coordenadas rotacionadas são:

$$
x'(x,y) = (x - x_0)\cos\theta + (y - y_0)\sin\theta
$$
$$
y'(x,y) = -(x - x_0)\sin\theta + (y - y_0)\cos\theta
$$

Parâmetros: amplitude $A$, centroide $(x_0, y_0)$, larguras $(\sigma_x,
\sigma_y)$, rotação $\theta$, e fundo de céu $B$.

A iteração **Levenberg-Marquardt** (um algoritmo de otimização que combina dois
métodos mais simples para convergir de forma confiável e rápida) minimiza:

$$
\chi^2 = \sum_{i,j}\frac{\left(I_\text{obs}(x_i, y_j) - I_\text{model}(x_i, y_j)\right)^2}{\sigma_{ij}^2}
$$

O passo de atualização combina gradiente descendente e Gauss-Newton:

$$
\left(J^T W J + \lambda\,\text{diag}(J^T W J)\right)\Delta\mathbf{p} = J^T W \mathbf{r}
$$

Convergência: $\|\Delta\mathbf{p}\| < 10^{-6}$ ou 200 iterações. Os valores de
FWHM ("FWHM" = Full Width at Half Maximum, largura a meia altura, a largura do
perfil de luz da estrela na metade de seu brilho de pico, a métrica padrão de
nitidez/foco) são:

$$
\text{FWHM}_{a,b} = 2\sqrt{2\ln 2}\,\sigma_{x,y}
$$

---

## 10. Aperture Photometry and Zero-Point / Fotometria de Abertura e Zero-Point

### 🇬🇧 English

"Aperture photometry" measures a star's brightness by summing pixel values
inside a circular aperture around it. Source flux in an aperture of radius $r$:

$$
F_\text{src} = \sum_{(x,y): r_{xy} \leq r} I(x,y) - N_\text{ap}\,\bar{S}
$$

where $\bar{S}$ is the median sky per pixel estimated from an annulus (ring)
$(r_\text{in}, r_\text{out})$ and $N_\text{ap} = \pi r^2$ is the aperture area.

**Instrumental magnitude:**

$$
m_\text{inst} = -2.5\,\log_{10}(F_\text{src}\,/\,t_\text{exp})
$$

**Calibrated magnitude:**

$$
m_\text{cal} = m_\text{inst} + Z
$$

where the zero-point $Z$ (the additive constant that converts a raw
instrumental brightness into a real astronomical magnitude) is determined from
reference stars in the field:

$$
Z = \langle m_\text{catalog,i} - m_\text{inst,i} \rangle
$$

The standard error of the zero-point:

$$
\sigma_Z = \frac{\text{MAD}(Z_i)}{0.6745\sqrt{N_\text{ref}}}
$$

### 🇧🇷 Português

"Fotometria de abertura" mede o brilho de uma estrela somando os valores de pixel
dentro de uma abertura circular ao redor dela. Fluxo da fonte numa abertura de
raio $r$:

$$
F_\text{src} = \sum_{(x,y): r_{xy} \leq r} I(x,y) - N_\text{ap}\,\bar{S}
$$

onde $\bar{S}$ é o céu mediano por pixel, estimado de um anel
$(r_\text{in}, r_\text{out})$, e $N_\text{ap} = \pi r^2$ é a área da abertura.

**Magnitude instrumental:**

$$
m_\text{inst} = -2.5\,\log_{10}(F_\text{src}\,/\,t_\text{exp})
$$

**Magnitude calibrada:**

$$
m_\text{cal} = m_\text{inst} + Z
$$

onde o zero-point $Z$ (a constante aditiva que converte um brilho instrumental
bruto numa magnitude astronômica real) é determinado a partir das estrelas de
referência no campo:

$$
Z = \langle m_\text{catalog,i} - m_\text{inst,i} \rangle
$$

O erro padrão do zero-point:

$$
\sigma_Z = \frac{\text{MAD}(Z_i)}{0.6745\sqrt{N_\text{ref}}}
$$

---

## 11. Differential Photometry / Fotometria Diferencial

### 🇬🇧 English

For each science frame, a differential zero-point is computed from the stable
reference stars (those not flagged as variable, blended, or near the detection
limit):

$$
Z_\text{diff} = \text{median}\!\left(m_\text{catalog,i} - m_\text{inst,i}\right)
$$

The uncertainty of the measured object's magnitude:

$$
\sigma_m = \sqrt{\sigma_Z^2 + \left(\frac{2.5}{\ln 10}\right)^2 \frac{F_\text{src} + N_\text{ap}\,\bar{S} + N_\text{ap}\,\sigma_\text{rn}^2/g^2}{F_\text{src}^2}}
$$

where $g$ = gain (e⁻/ADU) and $\sigma_\text{rn}$ = read noise (e⁻).

### 🇧🇷 Português

Para cada frame científico, um zero-point diferencial é calculado a partir das
estrelas de referência estáveis (aquelas não marcadas como variáveis, blendadas,
ou perto do limite de detecção):

$$
Z_\text{diff} = \text{median}\!\left(m_\text{catalog,i} - m_\text{inst,i}\right)
$$

A incerteza da magnitude do objeto medido:

$$
\sigma_m = \sqrt{\sigma_Z^2 + \left(\frac{2.5}{\ln 10}\right)^2 \frac{F_\text{src} + N_\text{ap}\,\bar{S} + N_\text{ap}\,\sigma_\text{rn}^2/g^2}{F_\text{src}^2}}
$$

onde $g$ = ganho (e⁻/ADU) e $\sigma_\text{rn}$ = ruído de leitura (e⁻).

---

## 12. Airmass / Massa de Ar

### 🇬🇧 English

"Airmass" measures how much atmosphere the light passed through (1.0 = straight
overhead, higher near the horizon). For altitudes above 10°, the plane-parallel
formula:

$$
X = \sec z = \frac{1}{\sin a}
$$

For lower altitudes, the Young & Irvine (1967) formula:

$$
X = \sec z \left(1 - 0.0012\,(\sec^2 z - 1)\right)
$$

where $z$ is the true zenith angle and $a$ is the true altitude.

The altitude is computed from the hour angle $H$, declination $\delta$, and
observer latitude $\varphi$:

$$
\sin a = \sin\varphi\,\sin\delta + \cos\varphi\,\cos\delta\,\cos H
$$

### 🇧🇷 Português

"Massa de ar" mede quanta atmosfera a luz atravessou (1,0 = direto do zênite,
maior perto do horizonte). Para altitudes acima de 10°, a fórmula plano-paralela:

$$
X = \sec z = \frac{1}{\sin a}
$$

Para altitudes mais baixas, a fórmula de Young & Irvine (1967):

$$
X = \sec z \left(1 - 0.0012\,(\sec^2 z - 1)\right)
$$

onde $z$ é o ângulo zenital verdadeiro e $a$ é a altitude verdadeira.

A altitude é calculada a partir do ângulo horário $H$, declinação $\delta$, e
latitude do observador $\varphi$:

$$
\sin a = \sin\varphi\,\sin\delta + \cos\varphi\,\cos\delta\,\cos H
$$

---

## 13. Julian Date / Data Juliana

### 🇬🇧 English

The "Julian Date" (JD) is a continuous day count used in astronomy, avoiding
calendar/timezone ambiguity. The Julian Date of a given UTC time
$(Y, M, D, h, m, s)$:

$$
JD = 367Y - \lfloor 7(Y + \lfloor(M+9)/12\rfloor)/4 \rfloor
   + \lfloor 275M/9 \rfloor + D + 1721013.5
   + \frac{h + m/60 + s/3600}{24}
$$

Valid for dates after 1582-10-15 (Gregorian calendar).

The Modified Julian Date: $\text{MJD} = JD - 2400000.5$.

The Julian century from J2000.0: $t = (JD - 2451545.0) / 36525$.

### 🇧🇷 Português

A "Data Juliana" (JD) é uma contagem contínua de dias usada em astronomia,
evitando ambiguidade de calendário/fuso horário. A Data Juliana de um dado
horário UTC $(Y, M, D, h, m, s)$:

$$
JD = 367Y - \lfloor 7(Y + \lfloor(M+9)/12\rfloor)/4 \rfloor
   + \lfloor 275M/9 \rfloor + D + 1721013.5
   + \frac{h + m/60 + s/3600}{24}
$$

Válida para datas após 1582-10-15 (calendário gregoriano).

A Data Juliana Modificada: $\text{MJD} = JD - 2400000.5$.

O século juliano a partir de J2000.0: $t = (JD - 2451545.0) / 36525$.

---

## 14. Ecliptic and Galactic Coordinates / Coordenadas Eclípticas e Galácticas

### 🇬🇧 English

**Equatorial → Ecliptic** (mean ecliptic of date, "ecliptic" = the plane of
Earth's orbit around the Sun):

$$
\lambda = \arctan\!\left(\frac{\sin\alpha\,\cos\varepsilon + \tan\delta\,\sin\varepsilon}{\cos\alpha}\right)
$$
$$
\beta = \arcsin\!\left(\sin\delta\,\cos\varepsilon - \cos\delta\,\sin\varepsilon\,\sin\alpha\right)
$$

**Equatorial (J2000) → Galactic** ("galactic" = coordinates aligned with the
plane of the Milky Way):

The IAU 1958 galactic pole at $(\alpha_G, \delta_G) = (192.25°, 27.40°)$,
ascending node $l_\Omega = 33°$:

$$
b = \arcsin\!\left(\cos\delta\,\cos\delta_G\,\cos(\alpha - \alpha_G) + \sin\delta\,\sin\delta_G\right)
$$
$$
l = \arctan\!\left(\frac{\sin\delta - \sin b\,\sin\delta_G}{\cos\delta\,\sin(\alpha - \alpha_G)\,\cos\delta_G}\right) + l_\Omega
$$

AstroFind warns when $|b| < 10°$ because dense stellar fields near the Galactic
plane increase false-positive detection rates.

### 🇧🇷 Português

**Equatorial → Eclíptica** (eclíptica média da data; "eclíptica" = o plano da
órbita da Terra ao redor do Sol):

$$
\lambda = \arctan\!\left(\frac{\sin\alpha\,\cos\varepsilon + \tan\delta\,\sin\varepsilon}{\cos\alpha}\right)
$$
$$
\beta = \arcsin\!\left(\sin\delta\,\cos\varepsilon - \cos\delta\,\sin\varepsilon\,\sin\alpha\right)
$$

**Equatorial (J2000) → Galáctica** ("galáctica" = coordenadas alinhadas com o
plano da Via Láctea):

O polo galáctico IAU 1958 em $(\alpha_G, \delta_G) = (192.25°, 27.40°)$, nó
ascendente $l_\Omega = 33°$:

$$
b = \arcsin\!\left(\cos\delta\,\cos\delta_G\,\cos(\alpha - \alpha_G) + \sin\delta\,\sin\delta_G\right)
$$
$$
l = \arctan\!\left(\frac{\sin\delta - \sin b\,\sin\delta_G}{\cos\delta\,\sin(\alpha - \alpha_G)\,\cos\delta_G}\right) + l_\Omega
$$

O AstroFind avisa quando $|b| < 10°$ porque campos estelares densos perto do
plano galáctico aumentam as taxas de detecção de falsos positivos.

---

## 15. FFT Image Registration (Phase Correlation) / Registro de Imagem por FFT (Correlação de Fase)

### 🇬🇧 English

"FFT" = Fast Fourier Transform, an algorithm that converts an image between the
spatial domain and the frequency domain very efficiently. Sub-pixel translation
between frames $f_1$ and $f_2$:

$$
G = \frac{F_1 \cdot \overline{F_2}}{|F_1 \cdot \overline{F_2}|}
$$

The cross-power spectrum is normalised (i.e., only phase information is
retained). The inverse FFT of $G$ gives a sharp delta-like peak at the shift
vector $(\Delta x, \Delta y)$.

Sub-pixel accuracy is achieved by fitting a 2-D Gaussian to the peak:

$$
(\Delta x_\text{sub}, \Delta y_\text{sub}) = \arg\max \text{Gaussian fit to peak neighbourhood}
$$

AstroFind uses FFTW3's real-to-complex `r2c` plan for efficiency. Frames are
zero-padded to the next power-of-two size to avoid circular aliasing.

### 🇧🇷 Português

"FFT" = Fast Fourier Transform (Transformada Rápida de Fourier), um algoritmo que
converte uma imagem entre o domínio espacial e o domínio de frequência com muita
eficiência. Translação sub-pixel entre os frames $f_1$ e $f_2$:

$$
G = \frac{F_1 \cdot \overline{F_2}}{|F_1 \cdot \overline{F_2}|}
$$

O espectro de potência cruzada é normalizado (isto é, só a informação de fase é
retida). A FFT inversa de $G$ dá um pico nítido, semelhante a um delta, no vetor
de deslocamento $(\Delta x, \Delta y)$.

A precisão sub-pixel é obtida ajustando uma gaussiana 2-D ao pico:

$$
(\Delta x_\text{sub}, \Delta y_\text{sub}) = \arg\max \text{ajuste gaussiano à vizinhança do pico}
$$

O AstroFind usa o plano real-para-complexo `r2c` da FFTW3 por eficiência. Os
frames recebem zero-padding (preenchimento com zeros) até o próximo tamanho
potência de dois, para evitar aliasing circular.

---

## 16. Bad Pixel Correction / Correção de Pixels Ruins

### 🇬🇧 English

For each pixel $(i, j)$ across a stack of $N$ calibrated frames:

1. Compute the local median $\tilde{p}$ and MAD (Median Absolute Deviation)
   $= \text{median}(|p_k - \tilde{p}|)$.
2. The normalised deviation is $s = |p_k - \tilde{p}| / (1.4826 \cdot \text{MAD})$.
3. Pixels with $s > \sigma_\text{threshold}$ (default $\sigma = 5$) are flagged.
4. Flagged pixels are replaced by bilinear interpolation from the 4 nearest valid
   neighbours:

$$
p_{i,j} = \frac{w_{+x}\,p_{i+1,j} + w_{-x}\,p_{i-1,j} + w_{+y}\,p_{i,j+1} + w_{-y}\,p_{i,j-1}}
               {w_{+x} + w_{-x} + w_{+y} + w_{-y}}
$$

where weights are 1 for valid pixels, 0 for flagged ones.

### 🇧🇷 Português

Para cada pixel $(i, j)$ ao longo de uma pilha de $N$ frames calibrados:

1. Calcular a mediana local $\tilde{p}$ e o MAD (Desvio Absoluto Mediano)
   $= \text{median}(|p_k - \tilde{p}|)$.
2. O desvio normalizado é $s = |p_k - \tilde{p}| / (1.4826 \cdot \text{MAD})$.
3. Pixels com $s > \sigma_\text{threshold}$ (padrão $\sigma = 5$) são marcados.
4. Pixels marcados são substituídos por interpolação bilinear a partir dos 4
   vizinhos válidos mais próximos:

$$
p_{i,j} = \frac{w_{+x}\,p_{i+1,j} + w_{-x}\,p_{i-1,j} + w_{+y}\,p_{i,j+1} + w_{-y}\,p_{i,j-1}}
               {w_{+x} + w_{-x} + w_{+y} + w_{-y}}
$$

onde os pesos são 1 para pixels válidos, 0 para os marcados.

---

## 17. Streak Detection by Blob Elongation / Detecção de Traço por Elongação de Blob

### 🇬🇧 English

After SEP source extraction ("SEP" = a fork of the Source Extractor C library
used to find stars/sources in an image), each detected source has semi-major and
semi-minor axes $a$ and $b$ (from the second moments of the intensity
distribution):

$$
e = a / b
$$

Sources with $e > e_\text{threshold}$ (default 4.0) are classified as
streaks/trails (long elongated blobs, typical of a fast-moving asteroid or
satellite crossing the frame during the exposure) and shown with a dashed-line
overlay. Their position angle $\theta_\text{PA}$ is stored in the measurement
record for archival purposes.

The second moments from SEP:

$$
\bar{x^2} = \frac{\sum I(x,y)(x - \bar{x})^2}{\sum I(x,y)}, \quad
\bar{y^2} = \frac{\sum I(x,y)(y - \bar{y})^2}{\sum I(x,y)}, \quad
\bar{xy}  = \frac{\sum I(x,y)(x - \bar{x})(y - \bar{y})}{\sum I(x,y)}
$$

$$
a = \sqrt{\frac{\bar{x^2}+\bar{y^2}}{2} + \sqrt{\left(\frac{\bar{x^2}-\bar{y^2}}{2}\right)^2 + \bar{xy}^2}}
$$

### 🇧🇷 Português

Após a extração de fontes via SEP ("SEP" = um fork da biblioteca C Source
Extractor usado para encontrar estrelas/fontes numa imagem), cada fonte detectada
tem eixos semi-maior e semi-menor $a$ e $b$ (a partir dos momentos de segunda
ordem da distribuição de intensidade):

$$
e = a / b
$$

Fontes com $e > e_\text{threshold}$ (padrão 4,0) são classificadas como
traços/rastros (blobs longos e alongados, típicos de um asteroide ou satélite se
movendo rápido pelo campo durante a exposição) e mostradas com um overlay de
linha tracejada. O ângulo de posição $\theta_\text{PA}$ é armazenado no registro
de medição para fins de arquivo.

Os momentos de segunda ordem do SEP:

$$
\bar{x^2} = \frac{\sum I(x,y)(x - \bar{x})^2}{\sum I(x,y)}, \quad
\bar{y^2} = \frac{\sum I(x,y)(y - \bar{y})^2}{\sum I(x,y)}, \quad
\bar{xy}  = \frac{\sum I(x,y)(x - \bar{x})(y - \bar{y})}{\sum I(x,y)}
$$

$$
a = \sqrt{\frac{\bar{x^2}+\bar{y^2}}{2} + \sqrt{\left(\frac{\bar{x^2}-\bar{y^2}}{2}\right)^2 + \bar{xy}^2}}
$$

---

## 18. ClumpFind: Blended Source Separation / ClumpFind: Separação de Fontes Blendadas

### 🇬🇧 English

When SEP marks a source island as "blended" (two or more overlapping stars/
objects detected as a single connected region), AstroFind runs a multi-peak scan:

1. Identify the bounding box of the blended island.
2. Compute the local maximum $I_\text{max}$.
3. Scan isophotal contours (contours of equal intensity) from $I_\text{max}$ down
   to $I_\text{threshold}$ in steps of $\Delta I = 0.1\,(I_\text{max} -
   I_\text{threshold})$.
4. At each level, count disconnected regions; a new region appearing for the
   first time is a new sub-peak.
5. Each sub-peak is assigned the pixels it uniquely captures as the level
   descends.

Blended detections are drawn with a magenta double-circle overlay (inner circle
= first sub-peak, outer = second). Photometry of blended sources carries an
uncertainty flag.

### 🇧🇷 Português

Quando o SEP marca uma ilha de fonte como "blendada" (duas ou mais estrelas/
objetos sobrepostos detectados como uma única região conectada), o AstroFind
executa uma varredura multi-pico:

1. Identificar a caixa delimitadora da ilha blendada.
2. Calcular o máximo local $I_\text{max}$.
3. Varrer contornos isofotais (contornos de intensidade igual) de $I_\text{max}$
   até $I_\text{threshold}$ em passos de $\Delta I = 0.1\,(I_\text{max} -
   I_\text{threshold})$.
4. Em cada nível, contar regiões desconectadas; uma nova região que aparece pela
   primeira vez é um novo sub-pico.
5. Cada sub-pico recebe os pixels que captura exclusivamente conforme o nível
   desce.

Detecções blendadas são desenhadas com um overlay de círculo duplo magenta
(círculo interno = primeiro sub-pico, externo = segundo). A fotometria de fontes
blendadas carrega uma flag de incerteza.

---

## 19. Sliding-Median Background Estimator / Estimador de Fundo por Mediana Deslizante

### 🇬🇧 English

The image is divided into a grid of $N_x \times N_y$ tiles of size $w \times w$
pixels. For each tile:

1. Compute the 3σ-clipped median $\tilde{S}$ (iteratively remove values more
   than $3\sigma$ from the median until convergence, "sigma-clipping": a way to
   discard outliers like stars before estimating the smooth sky background).
2. Smooth the grid using bilinear interpolation to produce a continuous sky map
   $S(x,y)$.

The background-subtracted image: $I'(x,y) = I(x,y) - S(x,y)$.

Default tile size: 64 pixels. Smaller tiles track finer gradients but may
confuse extended nebulosity with background.

### 🇧🇷 Português

A imagem é dividida numa grade de $N_x \times N_y$ tiles (blocos) de tamanho
$w \times w$ pixels. Para cada tile:

1. Calcular a mediana com sigma-clipping de 3σ $\tilde{S}$ (remover
   iterativamente valores mais de $3\sigma$ distantes da mediana até convergir;
   "sigma-clipping": uma forma de descartar outliers, como estrelas, antes de
   estimar o fundo de céu suave).
2. Suavizar a grade usando interpolação bilinear para produzir um mapa de céu
   contínuo $S(x,y)$.

A imagem com fundo subtraído: $I'(x,y) = I(x,y) - S(x,y)$.

Tamanho de tile padrão: 64 pixels. Tiles menores acompanham gradientes mais
finos, mas podem confundir nebulosidade estendida com fundo.

---

## 20. 2-D Power Spectrum / Espectro de Potência 2-D

### 🇬🇧 English

The discrete 2-D Hann-windowed power spectrum ("power spectrum" = shows how much
signal energy is at each spatial frequency, useful to diagnose periodic noise or
focus quality):

**Hann window:**

$$
W(x, y) = \sin^2\!\left(\frac{\pi x}{N_x}\right)\sin^2\!\left(\frac{\pi y}{N_y}\right)
$$

**Real-to-complex FFT:** $F = \text{FFT2D}(I \cdot W)$

**Power spectrum (log scale, quadrant-shifted):**

$$
P(u, v) = \log_e\!\left(1 + |F(u, v)|^2\right)
$$

The quadrant shift places $u = v = 0$ at the image centre.

**Spatial frequency to period conversion:**

$$
\nu = \sqrt{u^2 + v^2}\,/\,N_x \quad [\text{cycles/pixel}]
$$
$$
T_\text{arcsec} = \frac{\text{plate\_scale}}{\nu} \quad [\text{arcsec}]
$$

### 🇧🇷 Português

O espectro de potência discreto 2-D com janela de Hann ("espectro de potência" =
mostra quanta energia de sinal existe em cada frequência espacial, útil para
diagnosticar ruído periódico ou qualidade de foco):

**Janela de Hann:**

$$
W(x, y) = \sin^2\!\left(\frac{\pi x}{N_x}\right)\sin^2\!\left(\frac{\pi y}{N_y}\right)
$$

**FFT real-para-complexo:** $F = \text{FFT2D}(I \cdot W)$

**Espectro de potência (escala log, com deslocamento de quadrante):**

$$
P(u, v) = \log_e\!\left(1 + |F(u, v)|^2\right)
$$

O deslocamento de quadrante coloca $u = v = 0$ no centro da imagem.

**Conversão de frequência espacial para período:**

$$
\nu = \sqrt{u^2 + v^2}\,/\,N_x \quad [\text{ciclos/pixel}]
$$
$$
T_\text{arcsec} = \frac{\text{plate\_scale}}{\nu} \quad [\text{arcsec}]
$$

---

## 21. ADES / MPC Report Format / Formato de Relatório ADES / MPC

### 🇬🇧 English

"ADES" = Astrometry Data Exchange Standard, the format the Minor Planet Center
(MPC) uses to receive asteroid position reports.

**ADES 2017 XML Structure:**

```xml
<ades version="2017">
  <obsBlock>
    <obsContext>
      <observatory>
        <mpcCode>T05</mpcCode>
      </observatory>
      <submitter><name>Smith, J.</name></submitter>
      <observers><name>Smith, J.</name></observers>
      <measurers><name>Smith, J.</name></measurers>
      <telescope>
        <design>Reflector</design>
        <aperture>0.40</aperture>
        <fRatio>8</fRatio>
        <detector>CMOS</detector>
      </telescope>
    </obsContext>
    <obsData>
      <optical>
        <trkSub>2024ABC</trkSub>
        <mode>CCD</mode>
        <stn>T05</stn>
        <obsTime>2024-03-15T22:14:37.50Z</obsTime>
        <ra>185.4321</ra>
        <dec>+12.3456</dec>
        <rmsRA>0.15</rmsRA>
        <rmsDec>0.15</rmsDec>
        <mag>18.4</mag>
        <band>V</band>
        <sys>ICRF</sys>
        <ctr>399</ctr>
      </optical>
    </obsData>
  </obsBlock>
</ades>
```

The `<sys>ICRF</sys>` tag indicates the reported position is in the ICRF frame:
this comes from the WCS plate solution being calibrated against an ICRS-aligned
catalog (Gaia/UCAC4/2MASS), which absorbs annual aberration and precession/
nutation as a field-wide systematic (see §6), AstroFind does not apply a
separate aberration/precession/nutation step. The one frame correction AstroFind
does apply explicitly is atmospheric refraction removal (§5). The
`<ctr>399</ctr>` tag is the NAIF body code for Earth (NAIF = the numbering
scheme JPL uses to identify solar-system bodies).

**PSV Format:**

Pipe-Separated Values, one observation per line:

```
permID |provID |trkSub|mode|stn|obsTime                |ra        |dec      |rmsRA|rmsDec|mag|rmsMag|band|sys |ctr
       |       |A001  |CCD |T05|2024-03-15T22:14:37.50Z|185.432100|+12.34560|0.15 |0.15  |18.4|0.3  |V   |ICRF|399
```

### 🇧🇷 Português

"ADES" = Astrometry Data Exchange Standard (Padrão de Troca de Dados de
Astrometria), o formato que o Minor Planet Center (MPC) usa para receber
relatórios de posição de asteroides.

**Estrutura XML ADES 2017:**

```xml
<ades version="2017">
  <obsBlock>
    <obsContext>
      <observatory>
        <mpcCode>T05</mpcCode>
      </observatory>
      <submitter><name>Smith, J.</name></submitter>
      <observers><name>Smith, J.</name></observers>
      <measurers><name>Smith, J.</name></measurers>
      <telescope>
        <design>Reflector</design>
        <aperture>0.40</aperture>
        <fRatio>8</fRatio>
        <detector>CMOS</detector>
      </telescope>
    </obsContext>
    <obsData>
      <optical>
        <trkSub>2024ABC</trkSub>
        <mode>CCD</mode>
        <stn>T05</stn>
        <obsTime>2024-03-15T22:14:37.50Z</obsTime>
        <ra>185.4321</ra>
        <dec>+12.3456</dec>
        <rmsRA>0.15</rmsRA>
        <rmsDec>0.15</rmsDec>
        <mag>18.4</mag>
        <band>V</band>
        <sys>ICRF</sys>
        <ctr>399</ctr>
      </optical>
    </obsData>
  </obsBlock>
</ades>
```

A tag `<sys>ICRF</sys>` indica que a posição reportada está no referencial ICRF:
isso vem da solução de plate-solve WCS ser calibrada contra um catálogo alinhado
ao ICRS (Gaia/UCAC4/2MASS), que absorve a aberração anual e a precessão/nutação
como um sistemático de todo o campo (ver §6); o AstroFind não aplica uma etapa
separada de aberração/precessão/nutação. A única correção de referencial que o
AstroFind de fato aplica explicitamente é a remoção de refração atmosférica
(§5). A tag `<ctr>399</ctr>` é o código de corpo NAIF para a Terra (NAIF = o
esquema de numeração que o JPL usa para identificar corpos do Sistema Solar).

**Formato PSV:**

Pipe-Separated Values (Valores Separados por Pipe), uma observação por linha:

```
permID |provID |trkSub|mode|stn|obsTime                |ra        |dec      |rmsRA|rmsDec|mag|rmsMag|band|sys |ctr
       |       |A001  |CCD |T05|2024-03-15T22:14:37.50Z|185.432100|+12.34560|0.15 |0.15  |18.4|0.3  |V   |ICRF|399
```

---

*AstroFind. Last updated / Última atualização: 2026-07-10, AstroFind v0.9.0.*
*Author / Autor: Petrus Silva Costa.*
