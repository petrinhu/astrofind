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
> **Last reviewed / Última revisão:** 2026-09-25
> **Owner:** Petrus Silva Costa
> **Applies to / Aplica-se a:** AstroFind v1.2.0+

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
followed by a 4-byte little-endian XML header length and 4 reserved bytes; the XML
block starts at offset 16. The first `<Image>` element carries:

```xml
<Image geometry="W:H:channels" sampleFormat="Float32" location="attachment:offset:size"/>
```

Pixel data at `offset` bytes from file start (`location="embedded"` with a base64
`<Data>` child is also accepted). AstroFind reads `sampleFormat` UInt8, UInt16 (the
default when the attribute is missing), UInt32, Float32 and Float64, with 1 or 3
channels; 3 channels are stored planar (all R, then G, then B).

**DSLR RAW (LibRaw, optional):**

Camera RAW files (CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2, PEF, …) are decoded with LibRaw
(`open_file` → `unpack`). For a plain RGB Bayer sensor AstroFind copies the visible area of the
colour filter array (CFA) *without* gamma, white balance or black-level subtraction, so the
values stay linear ADU like a raw FITS. The detection/centroiding plane is the 2×2 superpixel
luminance of each Bayer block:

$$L = \frac{R + 2G + B}{4}, \qquad G = \frac{G_1 + G_2}{2}$$

which removes the colour checkerboard a raw CFA frame would otherwise feed into star
detection. The Bayer pattern (RGGB/GRBG/GBRG/BGGR) is read from LibRaw's colour index of the
top-left 2×2 block. Non-Bayer sensors (Fuji X-Trans, Foveon, linear DNG) go through LibRaw's
linear 16-bit processing (`gamm = 1,1`, `no_auto_bright`, raw colour space). EXIF gives the
exposure time and the timestamp; the timestamp is the camera's wall clock (time zone unknown),
so the image is flagged `dateObsAmbiguous` and JD is computed at mid-exposure.

**NASA PDS3 / PDS4:**

*PDS3* labels use ODL (`KEY = VALUE`, ending at `END`). AstroFind reads the first top-level
`OBJECT = IMAGE`: `LINES` (height), `LINE_SAMPLES` (width), `SAMPLE_TYPE` + `SAMPLE_BITS`
(MSB/LSB signed/unsigned integers of 8–64 bits, IEEE reals of 32/64 bits), `SCALING_FACTOR`,
`OFFSET`, `MISSING_CONSTANT` (→ NaN), `LINE_PREFIX_BYTES`/`LINE_SUFFIX_BYTES`. The data start
is given by the `^IMAGE` pointer:

| `^IMAGE` form | Byte offset of the first line |
|---|---|
| `n` | $(n-1)\cdot$`RECORD_BYTES` |
| `n <BYTES>` | $n-1$ |
| `"FILE.IMG"` | 0 in that file |
| `("FILE.IMG", n)` | $(n-1)\cdot$`RECORD_BYTES` in that file |

*PDS4* labels are XML. AstroFind reads the first `Array_2D_Image` of the
`File_Area_Observational`: `offset` (bytes), `axis_index_order` ("Last Index Fastest"),
`Element_Array/data_type` (e.g. `SignedMSB2`, `UnsignedLSB4`, `IEEE754MSBSingle`),
`scaling_factor`, `value_offset`, `Axis_Array/elements` with `sequence_number` 1 = lines and
2 = samples, and `Special_Constants/missing_constant`. A label whose data file is FITS is
handed to the FITS loader.

In both cases the physical value is $v = \text{raw}\cdot\text{scale} + \text{offset}$, and
before anything is allocated the declared size is checked against the shared loader ceiling
and against the file: `offset + lines·(prefix + samples·bytes + suffix) ≤ file size`.

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
cabeçalho XML little-endian de 4 bytes e 4 bytes reservados; o bloco XML começa no
offset 16. O primeiro elemento `<Image>` carrega:

```xml
<Image geometry="W:H:channels" sampleFormat="Float32" location="attachment:offset:size"/>
```

Os dados de pixel ficam no offset `offset` a partir do início do arquivo
(`location="embedded"` com um filho `<Data>` em base64 também é aceito). O
AstroFind lê `sampleFormat` UInt8, UInt16 (o padrão quando o atributo falta),
UInt32, Float32 e Float64, com 1 ou 3 canais; 3 canais ficam em planos separados
(todo o R, depois o G, depois o B).

**RAW de DSLR (LibRaw, opcional):**

Arquivos RAW de câmera (CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2, PEF, …) são decodificados com
o LibRaw (`open_file` → `unpack`). Para um sensor Bayer RGB comum, o AstroFind copia a área
visível do mosaico de filtros de cor (CFA) *sem* gama, balanço de branco ou subtração do nível
de preto, então os valores continuam em ADU linear como num FITS bruto. O plano usado para
detecção e centroide é a luminância por superpixel 2×2 de cada bloco Bayer:

$$L = \frac{R + 2G + B}{4}, \qquad G = \frac{G_1 + G_2}{2}$$

o que elimina o "xadrez" de cores que um CFA bruto passaria para a detecção de estrelas. O
padrão Bayer (RGGB/GRBG/GBRG/BGGR) vem do índice de cor do LibRaw no bloco 2×2 do canto
superior esquerdo. Sensores não Bayer (Fuji X-Trans, Foveon, DNG linear) passam pelo
processamento linear de 16 bits do LibRaw (`gamm = 1,1`, `no_auto_bright`, espaço de cor da
câmera). O EXIF fornece o tempo de exposição e o horário; o horário é o relógio da câmera (fuso
desconhecido), então a imagem é marcada `dateObsAmbiguous` e a JD é calculada no meio da
exposição.

**NASA PDS3 / PDS4:**

Rótulos *PDS3* usam ODL (`CHAVE = VALOR`, terminando em `END`). O AstroFind lê o primeiro
`OBJECT = IMAGE` de nível superior: `LINES` (altura), `LINE_SAMPLES` (largura), `SAMPLE_TYPE` +
`SAMPLE_BITS` (inteiros MSB/LSB com e sem sinal de 8 a 64 bits, reais IEEE de 32/64 bits),
`SCALING_FACTOR`, `OFFSET`, `MISSING_CONSTANT` (→ NaN), `LINE_PREFIX_BYTES`/`LINE_SUFFIX_BYTES`.
O início dos dados vem do ponteiro `^IMAGE`:

| Forma do `^IMAGE` | Offset em bytes da primeira linha |
|---|---|
| `n` | $(n-1)\cdot$`RECORD_BYTES` |
| `n <BYTES>` | $n-1$ |
| `"ARQ.IMG"` | 0 nesse arquivo |
| `("ARQ.IMG", n)` | $(n-1)\cdot$`RECORD_BYTES` nesse arquivo |

Rótulos *PDS4* são XML. O AstroFind lê o primeiro `Array_2D_Image` da
`File_Area_Observational`: `offset` (bytes), `axis_index_order` ("Last Index Fastest"),
`Element_Array/data_type` (ex.: `SignedMSB2`, `UnsignedLSB4`, `IEEE754MSBSingle`),
`scaling_factor`, `value_offset`, `Axis_Array/elements` com `sequence_number` 1 = linhas e
2 = amostras, e `Special_Constants/missing_constant`. Um rótulo cujo arquivo de dados é FITS é
repassado ao leitor FITS.

Nos dois casos o valor físico é $v = \text{bruto}\cdot\text{escala} + \text{offset}$, e
antes de alocar qualquer coisa o tamanho declarado é conferido contra o teto comum dos leitores
e contra o arquivo: `offset + linhas·(prefixo + amostras·bytes + sufixo) ≤ tamanho do arquivo`.

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

where $\phi_p = 180°$ for $\delta_0 \geq \theta_0$, else $0°$. **AUD-CORR-10:**
this default rule only applies when the header has no `LONPOLE` (or its `PV1_3`
alias) card; when present, $\phi_p$ is taken from that card instead, and
`LATPOLE` (or `PV1_4`) can additionally fix $\delta_p$ for the ambiguous case
of a non-zenithal projection whose fiducial point is not on the equator
($\delta_0 \neq 0°$), following Calabretta & Greisen (2002) §2.4, eqs. 8-10. A
`LONPOLE`/`LATPOLE` pair that admits no valid celestial pole is logged as a
warning and the parser falls back to the default above; `PV1_1`/`PV1_2` (a
native fiducial point away from $(\phi_0,\theta_0)=(0°,0°)$) are not
supported and are also logged.

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

onde $\phi_p = 180°$ para $\delta_0 \geq \theta_0$, senão $0°$. **AUD-CORR-10:**
essa regra padrão só vale quando o cabeçalho não tem o cartão `LONPOLE` (ou seu
alias `PV1_3`); quando presente, $\phi_p$ vem desse cartão, e `LATPOLE` (ou
`PV1_4`) pode adicionalmente fixar $\delta_p$ no caso ambíguo de uma projeção
não zenital cujo ponto fiducial não está no equador ($\delta_0 \neq 0°$),
seguindo Calabretta & Greisen (2002) §2.4, eqs. 8-10. Um par
`LONPOLE`/`LATPOLE` que não admite nenhum polo celeste válido gera um aviso no
log e o parser volta à regra padrão acima; `PV1_1`/`PV1_2` (um ponto fiducial
nativo fora de $(\phi_0,\theta_0)=(0°,0°)$) não são suportados e também geram
aviso.

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

**AUD-CORR-11 (sky → pixel, catalog overlay):** the *inverse* mapping used to
place catalog stars and known objects back onto the image (celestial → native
→ IWC → pixel) computes the native longitude $\phi$ with `atan2`, which
returns a value in $(-180°,180°]$ *before* the fiducial-longitude offset
$\phi_p$ is added. For the cylindrical/pseudocylindrical projections whose
inverse uses $\phi$ directly ($x \propto \phi$: **CAR, MER, GLS, AIT**), this
came out 360° off whenever the field's $\phi_p$ pushed the result outside
$(-180°,180°]$ (in practice, every southern-declination field for these
projections), landing the reprojected pixel roughly $1.3\times10^6$ px away
at 1″/px, i.e. off the image entirely. Fixed by reducing $\phi$ back into
$(-180°,180°]$ (`std::remainder`) after adding $\phi_p$. Zenithal projections
(TAN, SIN, ARC, STG) only use $\sin\phi/\cos\phi$ and were never affected.

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

**AUD-CORR-11 (céu → pixel, sobreposição de catálogo):** o mapeamento
*inverso*, usado para posicionar estrelas de catálogo e objetos conhecidos de
volta na imagem (celeste → nativo → IWC → pixel), calcula a longitude nativa
$\phi$ com `atan2`, que devolve um valor em $(-180°,180°]$ *antes* de somar o
deslocamento de longitude fiducial $\phi_p$. Para as projeções
cilíndricas/pseudocilíndricas cuja inversa usa $\phi$ diretamente
($x \propto \phi$: **CAR, MER, GLS, AIT**), isso saía 360° errado sempre que o
$\phi_p$ do campo empurrava o resultado para fora de $(-180°,180°]$, na
prática todo campo de declinação sul nessas projeções, levando o pixel
reprojetado a cerca de $1,3\times10^6$ px de distância a 1″/px, ou seja, fora
da imagem inteiramente. Corrigido reduzindo $\phi$ de volta a $(-180°,180°]$
(`std::remainder`) depois de somar $\phi_p$. As projeções zenitais (TAN, SIN,
ARC, STG) usam só $\sin\phi/\cos\phi$ e nunca foram afetadas.

---

## 5. Atmospheric Refraction / Refração Atmosférica

### 🇬🇧 English

> **Last reviewed:** 2026-09-24 (AUD-CORR-7). This section previously described a
> two-branch model (pressure/temperature-corrected formula above 15°, Bennett
> below 15°) that does not match the implementation (fixed 2026-07-10,
> AUD-DOC-1). It has since been updated again: the formula below
> (`applyRefractionCorrection()`) is unchanged, but the *policy* for when it
> runs on a reported position changed (`shouldApplyRefraction()`,
> `src/core/Astronomy.cpp`): see "When the correction is applied" below.

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

**When the correction is applied (AUD-CORR-7).** `shouldApplyRefraction()`
gates every call from the measurement pipeline
(`MainWindow::runMeasurePipeline`) and returns `true` only when **all** of
the following hold:

1. **Not a space telescope** (`isSpaceTelescope`): no atmosphere, so the
   correction never applies from orbit.
2. **The position was not derived from a catalog plate solution**
   (`FitsImage::wcs.solved`, i.e. a WCS fitted by `astrometry.net` or an
   equivalent solver against Gaia/UCAC4/2MASS star positions in the same
   exposure). Such a fit maps pixels straight onto catalog (ICRS)
   coordinates: the reference stars are refracted by the same atmosphere as
   the target, so the fit already absorbs the mean refraction and, to first
   order, its variation across the field. Applying Bennett on top of that
   would correct refraction **twice** (up to ~1.7′ at 30° altitude).
3. **The Julian Date is a real epoch** (`jd > 2400000`), needed to convert
   RA/Dec to alt-az.

In practice, essentially every measured position in AstroFind today comes
from a WCS plate solution, so Bennett refraction (§5's formula) does **not**
run on the reported RA/Dec; it would only apply to a ground-based position
derived some other way (e.g. raw pointing/mount coordinates without a plate
solve). This is a policy change from the pre-AUD-CORR-7 behaviour, which
applied Bennett to every ground-based measurement regardless of how the sky
position was obtained. When the correction is skipped for this reason, the
log panel says "Refraction: not applied (absorbed by the catalog plate
solution)"; when it does run, it still shows `Refraction correction: X"
(R=Y')`.

### 🇧🇷 Português

> **Última revisão:** 2026-09-24 (AUD-CORR-7). Esta seção antes descrevia um
> modelo de dois ramos (fórmula corrigida por pressão/temperatura acima de 15°,
> Bennett abaixo de 15°) que não correspondia à implementação (corrigido em
> 2026-07-10, AUD-DOC-1). Desde então foi atualizada de novo: a fórmula abaixo
> (`applyRefractionCorrection()`) não mudou, mas a *política* de quando ela roda
> sobre uma posição reportada mudou (`shouldApplyRefraction()`,
> `src/core/Astronomy.cpp`), ver "Quando a correção é aplicada" abaixo.

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

**Quando a correção é aplicada (AUD-CORR-7).** `shouldApplyRefraction()`
protege toda chamada feita a partir do pipeline de medição
(`MainWindow::runMeasurePipeline`) e só devolve `true` quando **todas** estas
condições valem:

1. **Não é telescópio espacial** (`isSpaceTelescope`): sem atmosfera, a
   correção nunca se aplica a partir da órbita.
2. **A posição não veio de uma solução de plate-solve por catálogo**
   (`FitsImage::wcs.solved`, isto é, um WCS ajustado pelo `astrometry.net` ou
   um solver equivalente contra posições de estrelas Gaia/UCAC4/2MASS na
   mesma exposição). Esse ajuste mapeia pixels diretamente para coordenadas
   de catálogo (ICRS): as estrelas de referência são refratadas pela mesma
   atmosfera que o alvo, então o ajuste já absorve a refração média e, em
   primeira ordem, sua variação pelo campo. Aplicar Bennett em cima disso
   corrigiria a refração **duas vezes** (até ~1,7′ a 30° de altitude).
3. **A Data Juliana é uma época real** (`jd > 2400000`), necessária para
   converter RA/Dec para alt-az.

Na prática, hoje praticamente toda posição medida no AstroFind vem de uma
solução WCS por plate-solve, então a refração de Bennett (fórmula da §5)
**não** roda sobre o RA/Dec reportado; ela só se aplicaria a uma posição
terrestre obtida de outro jeito (ex.: coordenadas brutas de apontamento/
montagem sem plate-solve). Esta é uma mudança de política em relação ao
comportamento anterior ao AUD-CORR-7, que aplicava Bennett a toda medição
terrestre independente de como a posição celeste foi obtida. Quando a
correção é pulada por esse motivo, o painel de log mostra "Refraction: not
applied (absorbed by the catalog plate solution)"; quando ela roda, continua
mostrando `Refraction correction: X" (R=Y')`.

---

## 6. Coordinate Chain: ICRS → CIRS → Topocentric / Cadeia de Coordenadas: ICRS → CIRS → Topocêntrico

### 🇬🇧 English

> **Updated during audit remediation (AUD-CORR-4, Onda 2, 2026-07-10; refraction
> policy revised 2026-09-24, AUD-CORR-7/AUD-DOC-8).** The chain described in
> earlier revisions of this document (precession → nutation → aberration →
> topocentric) was **never actually wired into the pipeline**:
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
   one frame correction that CAN be real and applied, gated by
   `shouldApplyRefraction()` (AUD-CORR-7). It converts the apparent (refracted)
   altitude/azimuth back to the true topocentric direction, but **only** for a
   ground-based position **not** derived from a catalog plate solution, which,
   for the reason given above, already absorbs refraction as part of its
   field-wide systematic. Since essentially every measured position today
   comes from such a plate solution, this step does not run in practice on
   the reported RA/Dec; it is skipped both for space telescopes
   (`isSpaceTelescope`, no atmosphere) and for plate-solved ground-based
   positions.

`annualAberrationComponents()` (§7) is retained and still has one real call site
(`MainWindow::runMeasurePipeline`), but purely to **log** the aberration magnitude
at the epoch as a diagnostic for the observer, its result is never added to or
subtracted from the reported coordinate.

The ADES XML output tags coordinates with `<sys>ICRF</sys>` (AUD-CORR-14/
AUD-DOC-8) because the WCS plate solution is itself calibrated against an
ICRS-aligned catalog; this is not a claim that AstroFind applies its own
separate aberration/precession/nutation correction; those two are only ever
logged (§7, §8), never applied to the exported RA/Dec.

### 🇧🇷 Português

> **Atualizado durante a remediação da auditoria (AUD-CORR-4, Onda 2,
> 2026-07-10; política de refração revisada em 2026-09-24, AUD-CORR-7/
> AUD-DOC-8).** A cadeia descrita em revisões anteriores deste documento
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
   única correção de referencial que PODE ser real e aplicada, protegida por
   `shouldApplyRefraction()` (AUD-CORR-7). Ela converte a altitude/azimute
   aparente (refratado) de volta à direção topocêntrica verdadeira, mas
   **só** para uma posição terrestre **não** derivada de uma solução de
   plate-solve por catálogo, que, pelo motivo explicado acima, já absorve a
   refração como parte do seu sistemático de todo o campo. Como praticamente
   toda posição medida hoje vem de tal solução, essa etapa não roda na
   prática sobre o RA/Dec reportado; ela é pulada tanto para telescópios
   espaciais (`isSpaceTelescope`, sem atmosfera) quanto para posições
   terrestres resolvidas por plate-solve.

`annualAberrationComponents()` (§7) foi mantida e ainda tem um único ponto de
chamada real (`MainWindow::runMeasurePipeline`), mas puramente para **registrar
em log** a magnitude da aberração na época como diagnóstico para o observador; seu
resultado nunca é somado ou subtraído da coordenada reportada.

A saída XML do ADES marca coordenadas com `<sys>ICRF</sys>` (AUD-CORR-14/
AUD-DOC-8) porque a própria solução de plate-solve WCS é calibrada contra um
catálogo alinhado ao ICRS; isso não é uma alegação de que o AstroFind aplica
sua própria correção separada de aberração/precessão/nutação; as duas só
são registradas em log (§7, §8), nunca aplicadas ao RA/Dec exportado.

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
\right]\right)
$$

where the rotated coordinates are:

$$
x'(x,y) = (x - x_0)\cos\theta + (y - y_0)\sin\theta
$$
$$
y'(x,y) = -(x - x_0)\sin\theta + (y - y_0)\cos\theta
$$

Parameters: amplitude $A$, centroid $(x_0, y_0)$, widths $(\sigma_x, \sigma_y)$ and
rotation $\theta$. The sky background is **not** a fit parameter: the median of the
pixels on the border of the fitting box is subtracted first, and negative values are
clamped to 0. The fit is seeded by a symmetric Gaussian fit (`findCentroidPsf`).

The **Levenberg-Marquardt** iteration (an optimization algorithm that blends two
simpler methods to converge reliably and fast) minimises:

$$
\chi^2 = \sum_{i,j}\left(I_\text{obs}(x_i, y_j) - I_\text{model}(x_i, y_j)\right)^2
$$

The update step blends gradient descent and Gauss-Newton:

$$
\left(J^T J + \lambda\,\text{diag}(J^T J)\right)\Delta\mathbf{p} = J^T \mathbf{r}
$$

The sum is unweighted (all pixels count the same). $\lambda$ starts at $10^{-3}$; a step
that lowers $\chi^2$ is accepted and $\lambda$ is divided by 5, otherwise $\lambda$ is
multiplied by 5. The loop stops after 60 iterations, when the system is singular, or when
$\lambda > 10^{8}$; $\sigma_{x,y} \ge 0.3$ px and $A \ge 0$ are enforced. The FWHM values
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
\right]\right)
$$

onde as coordenadas rotacionadas são:

$$
x'(x,y) = (x - x_0)\cos\theta + (y - y_0)\sin\theta
$$
$$
y'(x,y) = -(x - x_0)\sin\theta + (y - y_0)\cos\theta
$$

Parâmetros: amplitude $A$, centroide $(x_0, y_0)$, larguras $(\sigma_x,
\sigma_y)$ e rotação $\theta$. O fundo de céu **não** é parâmetro do ajuste: a
mediana dos pixels da borda da caixa de ajuste é subtraída antes, e valores
negativos viram 0. O ajuste parte de um ajuste gaussiano simétrico
(`findCentroidPsf`).

A iteração **Levenberg-Marquardt** (um algoritmo de otimização que combina dois
métodos mais simples para convergir de forma confiável e rápida) minimiza:

$$
\chi^2 = \sum_{i,j}\left(I_\text{obs}(x_i, y_j) - I_\text{model}(x_i, y_j)\right)^2
$$

O passo de atualização combina gradiente descendente e Gauss-Newton:

$$
\left(J^T J + \lambda\,\text{diag}(J^T J)\right)\Delta\mathbf{p} = J^T \mathbf{r}
$$

A soma não tem pesos (todos os pixels contam igual). $\lambda$ começa em $10^{-3}$; um
passo que reduz o $\chi^2$ é aceito e $\lambda$ é dividido por 5, senão $\lambda$ é
multiplicado por 5. O laço para depois de 60 iterações, quando o sistema é singular, ou
quando $\lambda > 10^{8}$; impõe-se $\sigma_{x,y} \ge 0,3$ px e $A \ge 0$. Os valores de
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
overhead, higher near the horizon). AstroFind uses the Pickering (2002) formula at
every altitude, with the altitude $a$ in degrees:

$$
X = \frac{1}{\sin\!\left(a + \dfrac{244}{165 + 47\,a^{1.1}}\right)}
$$

where $a$ is the geometric (unrefracted) altitude computed below; the angle inside the
sine is in degrees. Near the zenith this reduces to the plane-parallel $X = \sec z =
1/\sin a$; at the horizon it stays finite ($X \approx 38$). For $a \le 0$ the function
returns 0 (below the horizon) and no extinction correction is applied.

> **Note:** up to 1.1.0 the code used the exponent 2.575 instead of Pickering's 1.1. The two
> agree within 1% above 20° altitude but differ by about 3% at 10° and 9% at 5°, so
> extinction-corrected magnitudes of low targets were affected; 1.2.0 uses 1.1.

The altitude is computed from the hour angle $H$, declination $\delta$, and
observer latitude $\varphi$:

$$
\sin a = \sin\varphi\,\sin\delta + \cos\varphi\,\cos\delta\,\cos H
$$

### 🇧🇷 Português

"Massa de ar" mede quanta atmosfera a luz atravessou (1,0 = direto do zênite,
maior perto do horizonte). O AstroFind usa a fórmula de Pickering (2002) em qualquer
altitude, com a altitude $a$ em graus:

$$
X = \frac{1}{\sin\!\left(a + \dfrac{244}{165 + 47\,a^{1.1}}\right)}
$$

onde $a$ é a altitude geométrica (sem refração) calculada abaixo; o ângulo dentro do
seno está em graus. Perto do zênite isso se reduz à plano-paralela $X = \sec z =
1/\sin a$; no horizonte continua finita ($X \approx 38$). Para $a \le 0$ a função
retorna 0 (abaixo do horizonte) e nenhuma correção de extinção é aplicada.

> **Nota:** até a 1.1.0 o código usava o expoente 2,575 em vez do 1,1 de Pickering. Os dois
> concordam em até 1% acima de 20° de altitude, mas diferem cerca de 3% a 10° e 9% a 5°, então
> as magnitudes corrigidas de extinção de alvos baixos eram afetadas; a 1.2.0 usa 1,1.

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

**Time scales in AstroFind.** The image JD is the mid-exposure in **UTC**, plus the
user's camera-clock correction $\Delta t_\text{clock}$ (Settings → Observer → Time
Offset, in seconds), which is applied exactly once per image:

$$
JD_\text{UTC} = JD_\text{mid} + \frac{\Delta t_\text{clock}}{86400}
$$

This $JD_\text{UTC}$ is what the ADES report writes as `obsTime` (with `Z`) and what
is sent to SkyBoT, which expects UTC. $\Delta T = TT - UTC$ (Settings → Camera,
default 68 s) is **not** added to the reported time. It is used only for the offline
MPCORB ephemeris scan, whose orbital elements are propagated in TT:

$$
JD_\text{TT} = JD_\text{UTC} + \frac{\Delta T}{86400}
$$

(Version 1.1.0 and earlier added $\Delta T$ to the image JD during Run Data Reduction, so
`obsTime` was about 68 s late; version 1.2.0 fixes this.)

**Mid-exposure JD and sub-second precision (AUD-CORR-13).** `julianDateUtc()`
keeps the milliseconds of the input instant (`QDateTime::msecsTo`, not
`secsTo`, which truncates). `midExposureJd()` prefers `MJD-OBS` (start of
exposure, same time scale as `DATE-OBS`) over `DATE-OBS` when the header has
a finite, positive `MJD-OBS`: it logs a warning if the two disagree by more
than 1 s, and otherwise falls back to `DATE-OBS` at millisecond resolution.
Half of `EXPTIME`/`EXPOSURE` (when present) is then added to reach the
mid-point. The XISF loader now computes its JD the same way, from
`DATE-OBS`, instead of a coarser method (version 1.1.0 and earlier).

**Offline MPCORB ephemeris accuracy (AUD-CORR-12).** The two-body propagator
used by the offline known-object scan (`Ephemeris.cpp`, `MpcOrb.cpp`) had two
errors that together threw off predicted positions: the MPCORB packed epoch
(a calendar date "at 0h TT") was read as if it were the Julian Day Number of
that date's *noon*, half a day late, and the Sun's ecliptic longitude used to
locate Earth was left in the mean equinox of date instead of being precessed
back to J2000 to match the orbital elements. Both are now fixed (epoch minus
0.5 day; longitude corrected by the IAU 1976 general precession, $p =
5029.0966''\!\cdot T + 1.11113''\!\cdot T^2$). Against JPL Horizons, the
resulting error dropped from 348″ to 13″ for Ceres (epoch 2024) and from
2952″ to 49″ for Eros (epoch 2012). The remaining tens-of-arcsecond
discrepancy comes from what this propagator still does not model: light-time
and the low-precision (~36″ accuracy) Sun position; it is a reason to always
double-check against Horizons before submitting, never a claim of
Horizons-grade accuracy.

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

**Escalas de tempo no AstroFind.** O JD da imagem é o meio da exposição em **UTC**,
mais a correção do relógio da câmera $\Delta t_\text{clock}$ (Configurações →
Observador → Deslocamento de tempo, em segundos), aplicada exatamente uma vez por
imagem:

$$
JD_\text{UTC} = JD_\text{mid} + \frac{\Delta t_\text{clock}}{86400}
$$

Esse $JD_\text{UTC}$ é o que o relatório ADES grava como `obsTime` (com `Z`) e o que é
enviado ao SkyBoT, que espera UTC. O $\Delta T = TT - UTC$ (Configurações → Câmera,
padrão 68 s) **não** é somado ao horário reportado. Ele só é usado na busca offline de
efemérides pelo MPCORB, cujos elementos orbitais são propagados em TT:

$$
JD_\text{TT} = JD_\text{UTC} + \frac{\Delta T}{86400}
$$

(A versão 1.1.0 e anteriores somavam o $\Delta T$ ao JD da imagem na Redução de Dados,
e o `obsTime` saía cerca de 68 s atrasado; a versão 1.2.0 corrige
isso.)

**JD do meio da exposição e precisão sub-segundo (AUD-CORR-13).**
`julianDateUtc()` mantém os milissegundos do instante de entrada
(`QDateTime::msecsTo`, não `secsTo`, que trunca). `midExposureJd()` prefere
`MJD-OBS` (início da exposição, na mesma escala de tempo do `DATE-OBS`) sobre
`DATE-OBS` quando o cabeçalho tem um `MJD-OBS` finito e positivo: registra um
aviso no log se os dois discordarem em mais de 1 s, e senão volta para
`DATE-OBS` com resolução de milissegundos. Metade do `EXPTIME`/`EXPOSURE`
(quando presente) é então somada para chegar ao meio da exposição. O leitor
de XISF agora calcula seu JD do mesmo jeito, a partir do `DATE-OBS`, em vez de
um método mais grosseiro (versão 1.1.0 e anteriores).

**Precisão da efeméride offline do MPCORB (AUD-CORR-12).** O propagador de
dois corpos usado na busca offline de objetos conhecidos (`Ephemeris.cpp`,
`MpcOrb.cpp`) tinha dois erros que juntos desviavam as posições previstas: a
época empacotada do MPCORB (uma data de calendário "às 0h TT") era lida como
se fosse o Número do Dia Juliano do *meio-dia* dessa data, meio dia atrasada,
e a longitude eclíptica do Sol usada para localizar a Terra ficava no
equinócio médio da data em vez de ser precessionada de volta a J2000 para
casar com os elementos orbitais. Os dois já estão corrigidos (época menos 0,5
dia; longitude corrigida pela precessão geral IAU 1976, $p =
5029,0966''\!\cdot T + 1,11113''\!\cdot T^2$). Contra o JPL Horizons, o erro
resultante caiu de 348″ para 13″ para Ceres (época 2024) e de 2952″ para 49″
para Eros (época 2012). A discrepância residual, de dezenas de arcossegundos,
vem do que esse propagador ainda não modela: tempo-luz e a posição de baixa
precisão (~36″ de exatidão) do Sol; é motivo para sempre conferir contra o
Horizons antes de submeter, nunca uma alegação de exatidão no nível do
Horizons.

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

The IAU 1958 galactic north pole in J2000 coordinates,
$(\alpha_G, \delta_G) = (192.859508°, 27.128336°)$, and the galactic longitude of the
north celestial pole $l_\text{NCP} = 122.932°$:

$$
b = \arcsin\!\left(\cos\delta\,\cos\delta_G\,\cos(\alpha - \alpha_G) + \sin\delta\,\sin\delta_G\right)
$$
$$
l = l_\text{NCP} - \operatorname{atan2}\!\left(\cos\delta\,\sin(\alpha - \alpha_G),\;\sin\delta\,\cos\delta_G - \cos\delta\,\sin\delta_G\,\cos(\alpha - \alpha_G)\right)
$$

With the ecliptic / galactic overlay on, AstroFind shows a warning badge when the
field centre has $|b| < 15°$ (high interstellar extinction; dense star fields near the
Galactic plane also raise false-positive detections).

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

O polo norte galáctico IAU 1958 em coordenadas J2000,
$(\alpha_G, \delta_G) = (192,859508°, 27,128336°)$, e a longitude galáctica do polo
norte celeste $l_\text{NCP} = 122,932°$:

$$
b = \arcsin\!\left(\cos\delta\,\cos\delta_G\,\cos(\alpha - \alpha_G) + \sin\delta\,\sin\delta_G\right)
$$
$$
l = l_\text{NCP} - \operatorname{atan2}\!\left(\cos\delta\,\sin(\alpha - \alpha_G),\;\sin\delta\,\cos\delta_G - \cos\delta\,\sin\delta_G\,\cos(\alpha - \alpha_G)\right)
$$

Com a sobreposição eclíptica / galáctica ligada, o AstroFind mostra um aviso quando o
centro do campo tem $|b| < 15°$ (alta extinção interestelar; campos estelares densos
perto do plano galáctico também aumentam as detecções falsas).

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

Sub-pixel accuracy comes from a 3-point parabolic interpolation around the integer
peak $(p_x, p_y)$, done separately along each axis, with $c_{k}$ the correlation value
at offset $k$ from the peak along that axis:

$$
\Delta x_\text{sub} = \frac{1}{2}\,\frac{c_{-1} - c_{+1}}{c_{-1} - 2c_0 + c_{+1}}
$$

(and the same for $y$; the correction is 0 when the denominator is below $10^{-12}$).
The shift is $p + \Delta_\text{sub}$, wrapped into $[-w/2, w/2)$ and $[-h/2, h/2)$.

AstroFind uses FFTW3's real-to-complex `r2c` plan (and `c2r` for the inverse) at the
native frame size $w \times h$. There is no zero-padding, so the correlation is
circular.

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

A precisão sub-pixel vem de uma interpolação parabólica de 3 pontos em volta do pico
inteiro $(p_x, p_y)$, feita separadamente em cada eixo, com $c_{k}$ o valor da
correlação no deslocamento $k$ a partir do pico ao longo desse eixo:

$$
\Delta x_\text{sub} = \frac{1}{2}\,\frac{c_{-1} - c_{+1}}{c_{-1} - 2c_0 + c_{+1}}
$$

(e o mesmo para $y$; a correção é 0 quando o denominador é menor que $10^{-12}$).
O deslocamento é $p + \Delta_\text{sub}$, levado para $[-w/2, w/2)$ e $[-h/2, h/2)$.

O AstroFind usa o plano real-para-complexo `r2c` da FFTW3 (e `c2r` na inversa) no
tamanho nativo do frame $w \times h$. Não há zero-padding (preenchimento com zeros),
então a correlação é circular.

---

## 16. Bad Pixel Correction / Correção de Pixels Ruins

### 🇬🇧 English

The correction works on **one image at a time** (`applyBadPixelCorrection`, run in
Data Reduction when the checkbox "Corrigir pixels ruins automaticamente" is on):

1. For each pixel, compute the median $\tilde{p}_{i,j}$ of its 3×3 neighbourhood
   (itself included; edges are clamped) and the difference
   $d_{i,j} = p_{i,j} - \tilde{p}_{i,j}$.
2. Estimate the image noise from the MAD (Median Absolute Deviation) of **all**
   differences: $\sigma_\text{noise} = 1.4826 \cdot \text{median}(|d|)$. If the MAD is 0
   (uniform image), nothing is done.
3. Pixels with $|d_{i,j}| > k\,\sigma_\text{noise}$ are flagged, where $k$ is
   **Settings → Camera → Bad Pixel Correction** (`camera/badPixelSigma`, default 5, range 2–15).
4. Each flagged pixel is replaced by the **median of its non-flagged 3×3 neighbours**
   (up to 8). If all its neighbours are flagged too, it is left unchanged:

$$
p_{i,j} \leftarrow \text{median}\{\,p_{i+u,\,j+v} : (u,v) \ne (0,0),\ |u|,|v| \le 1,\ \text{not flagged}\,\}
$$

### 🇧🇷 Português

A correção trabalha em **uma imagem por vez** (`applyBadPixelCorrection`, rodada na
Redução de Dados quando "Corrigir pixels ruins automaticamente" está ligado):

1. Para cada pixel, calcular a mediana $\tilde{p}_{i,j}$ da vizinhança 3×3 (incluindo
   o próprio pixel; nas bordas os índices são limitados) e a diferença
   $d_{i,j} = p_{i,j} - \tilde{p}_{i,j}$.
2. Estimar o ruído da imagem pelo MAD (Desvio Absoluto Mediano) de **todas** as
   diferenças: $\sigma_\text{ruído} = 1{,}4826 \cdot \text{mediana}(|d|)$. Se o MAD for 0
   (imagem uniforme), nada é feito.
3. Pixels com $|d_{i,j}| > k\,\sigma_\text{ruído}$ são marcados, onde $k$ vem de
   **Configurações → Câmera → Correção de Pixels Ruins** (`camera/badPixelSigma`, padrão 5,
   faixa 2–15).
4. Cada pixel marcado é substituído pela **mediana dos vizinhos 3×3 não marcados**
   (até 8). Se todos os vizinhos também estão marcados, ele fica como está:

$$
p_{i,j} \leftarrow \text{mediana}\{\,p_{i+u,\,j+v} : (u,v) \ne (0,0),\ |u|,|v| \le 1,\ \text{não marcado}\,\}
$$

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

Sources with $e \ge e_\text{threshold}$ (and $b > 0.1$ px) are classified as
streaks/trails (long elongated blobs, typical of a fast-moving asteroid or
satellite crossing the frame during the exposure). The threshold is
**Settings → Detection → Streak threshold (a/b):** (`detection/streakElongation`,
default 3.0, range 1.5–20; `StarDetectorConfig::streakMinElongation`, where 0
disables the flag). Streaks are drawn as an orange ellipse rotated by the SEP
position angle $\theta$, with a direction tick.

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

Fontes com $e \ge e_\text{threshold}$ (e $b > 0{,}1$ px) são classificadas como
traços/rastros (blobs longos e alongados, típicos de um asteroide ou satélite se
movendo rápido pelo campo durante a exposição). O limiar é
**Configurações → Detecção → Limiar de traço (a/b):** (`detection/streakElongation`,
padrão 3,0, faixa 1,5–20; `StarDetectorConfig::streakMinElongation`, onde 0 desliga a
marcação). Os traços são desenhados como uma elipse laranja girada pelo ângulo de
posição $\theta$ do SEP, com um traço indicando a direção.

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

**ADES 2022 XML structure** (what `generateAdesXml` writes; `obsTime` has "Time Precision:" decimals, 1 by default):

```xml
<ades version="2022">
  <obsBlock>
    <obsContext>
      <observatory>
        <mpcCode>T05</mpcCode>
      </observatory>
      <submitter>
        <name>Smith, J.</name>
      </submitter>
      <observers>
        <name>Smith, J.</name>
      </observers>
      <measurers>
        <name>Smith, J.</name>
      </measurers>
      <telescope>
        <name>0.40-m f/8 reflector + CMOS</name>
      </telescope>
      <software>
        <product>AstroFind</product>
      </software>
    </obsContext>
    <obsData>
      <optical>
        <trkSub>2024ABC</trkSub>
        <mode>CCD</mode>
        <stn>T05</stn>
        <obsTime>2024-03-15T22:14:37.5Z</obsTime>
        <ra>185.432100000</ra>
        <dec>12.345600000</dec>
        <sys>ICRF</sys>
        <rmsRA>0.150</rmsRA>
        <rmsDec>0.150</rmsDec>
        <rmsCorr>0.0000</rmsCorr>
        <astCat>UCAC4</astCat>
        <mag>18.40</mag>
        <rmsMag>0.30</rmsMag>
        <band>V</band>
        <photCat>UCAC4</photCat>
      </optical>
    </obsData>
  </obsBlock>
</ades>
```

The `<sys>ICRF</sys>` tag (AUD-CORR-14/AUD-DOC-8) indicates the reported
position is an **astrometric** ICRF position: this comes from the WCS plate
solution being calibrated against an ICRS-aligned catalog (Gaia/UCAC4/2MASS),
which absorbs annual aberration and precession/nutation as a field-wide
systematic (see §6); AstroFind does not apply a separate aberration/
precession/nutation step to the exported RA/Dec, those are only logged
(§7, §8). Atmospheric refraction removal (§5) is the one frame correction
AstroFind can apply explicitly, but only when `shouldApplyRefraction()`
(AUD-CORR-7) says the position was **not** derived from a catalog plate
solution (in practice, not for a typical measured position, since those come
from a plate solution).

**PSV Format:**

Pipe-Separated Values: a `# version=2022` line, a header line, then one observation
per line (empty fields are left blank):

```
# version=2022
permID|provID|trkSub|mode|stn|obsTime|ra|dec|sys|rmsRA|rmsDec|astCat|mag|rmsMag|band|photCat|notes|observers|measurers|telescope
||2024ABC|CCD|T05|2024-03-15T22:14:37.5Z|185.432100000|12.345600000|ICRF|0.150|0.150|UCAC4|18.40|0.30|V|UCAC4||Smith, J.|Smith, J.|0.40-m f/8 reflector + CMOS
```

### 🇧🇷 Português

"ADES" = Astrometry Data Exchange Standard (Padrão de Troca de Dados de
Astrometria), o formato que o Minor Planet Center (MPC) usa para receber
relatórios de posição de asteroides.

**Estrutura XML ADES 2022** (o que o `generateAdesXml` grava; o `obsTime` tem as casas decimais de "Precisão de tempo:", 1 por padrão):

```xml
<ades version="2022">
  <obsBlock>
    <obsContext>
      <observatory>
        <mpcCode>T05</mpcCode>
      </observatory>
      <submitter>
        <name>Smith, J.</name>
      </submitter>
      <observers>
        <name>Smith, J.</name>
      </observers>
      <measurers>
        <name>Smith, J.</name>
      </measurers>
      <telescope>
        <name>0.40-m f/8 reflector + CMOS</name>
      </telescope>
      <software>
        <product>AstroFind</product>
      </software>
    </obsContext>
    <obsData>
      <optical>
        <trkSub>2024ABC</trkSub>
        <mode>CCD</mode>
        <stn>T05</stn>
        <obsTime>2024-03-15T22:14:37.5Z</obsTime>
        <ra>185.432100000</ra>
        <dec>12.345600000</dec>
        <sys>ICRF</sys>
        <rmsRA>0.150</rmsRA>
        <rmsDec>0.150</rmsDec>
        <rmsCorr>0.0000</rmsCorr>
        <astCat>UCAC4</astCat>
        <mag>18.40</mag>
        <rmsMag>0.30</rmsMag>
        <band>V</band>
        <photCat>UCAC4</photCat>
      </optical>
    </obsData>
  </obsBlock>
</ades>
```

A tag `<sys>ICRF</sys>` (AUD-CORR-14/AUD-DOC-8) indica que a posição reportada
é uma posição **astrométrica** ICRF: isso vem da solução de plate-solve WCS
ser calibrada contra um catálogo alinhado ao ICRS (Gaia/UCAC4/2MASS), que
absorve a aberração anual e a precessão/nutação como um sistemático de todo o
campo (ver §6); o AstroFind não aplica uma etapa separada de aberração/
precessão/nutação ao RA/Dec exportado, elas só são registradas em log (§7,
§8). A remoção de refração atmosférica (§5) é a única correção de referencial
que o AstroFind pode aplicar explicitamente, mas só quando
`shouldApplyRefraction()` (AUD-CORR-7) diz que a posição **não** veio de uma
solução de plate-solve por catálogo, na prática, não para uma posição medida
típica, já que estas vêm de uma solução de plate-solve.

**Formato PSV:**

Pipe-Separated Values (Valores Separados por Pipe): uma linha `# version=2022`, uma
linha de cabeçalho e depois uma observação por linha (campos vazios ficam em branco):

```
# version=2022
permID|provID|trkSub|mode|stn|obsTime|ra|dec|sys|rmsRA|rmsDec|astCat|mag|rmsMag|band|photCat|notes|observers|measurers|telescope
||2024ABC|CCD|T05|2024-03-15T22:14:37.5Z|185.432100000|12.345600000|ICRF|0.150|0.150|UCAC4|18.40|0.30|V|UCAC4||Smith, J.|Smith, J.|0.40-m f/8 reflector + CMOS
```

---

*AstroFind. Last updated / Última atualização: 2026-09-25, AstroFind v1.2.0.*
*Author / Autor: Petrus Silva Costa.*
