# Advanced / Avançado

🇬🇧 **Who this page is for.** Experienced observers (Astrometrica / MPC users) and developers
who want to know *how* AstroFind computes its numbers, where its limits are, how to get
MPC-grade data out of it, and how to build, test and audit it from source. Most of the page is
🔴 Advanced, but every section starts with a two-line plain-language summary, so an intermediate
user can follow. Unknown words are in the [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary). The full maths is in the repository file
[`docs/technical-reference.md`](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md)
(we call it "TR" below). Where TR and the code disagree, this page says so and describes what
the **code** does (AstroFind v1.1.0; where the next version (after 1.1.0) already behaves
differently, for example in timing, the page says so).

🇧🇷 **Para quem é esta página.** Observadores experientes (usuários do Astrometrica / MPC) e
desenvolvedores que querem saber *como* o AstroFind calcula seus números, quais são os limites,
como tirar dele dados com qualidade de MPC e como compilar, testar e auditar a partir do código.
A maior parte é 🔴 Avançado, mas toda seção começa com um resumo de duas linhas em linguagem
simples, para que um usuário intermediário acompanhe. Palavras desconhecidas estão no
[Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary). A matemática completa está no arquivo do repositório
[`docs/technical-reference.md`](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md)
(chamado de "TR" abaixo). Onde o TR e o código divergem, esta página avisa e descreve o que o
**código** faz (AstroFind v1.1.0; onde a próxima versão (depois da 1.1.0) já se comporta
diferente, por exemplo no tempo, a página avisa).

> ⚠️ **Watch out / Atenção** — 🇬🇧 AstroFind has **not yet been validated end to end with real
> observatory data**. Check astrometry, photometry and the ADES report independently before
> submitting to the MPC. 🇧🇷 O AstroFind **ainda não foi validado de ponta a ponta com dados reais
> de observatório**. Confira astrometria, fotometria e o relatório ADES de forma independente
> antes de enviar ao MPC.

**Contents / Conteúdo**

1. [How AstroFind computes things / Como o AstroFind calcula](#1-how-astrofind-computes-things--como-o-astrofind-calcula)
2. [Timing / Tempo](#2-timing--tempo)
3. [Data quality for MPC-grade astrometry / Qualidade de dados para astrometria nível MPC](#3-data-quality-for-mpc-grade-astrometry--qualidade-de-dados-para-astrometria-nível-mpc)
4. [Command line and files / Linha de comando e arquivos](#4-command-line-and-files--linha-de-comando-e-arquivos)
5. [Security and robustness / Segurança e robustez](#5-security-and-robustness--segurança-e-robustez)
6. [Contributing and real-data reports / Contribuindo e relatos com dados reais](#6-contributing-and-real-data-reports--contribuindo-e-relatos-com-dados-reais)

---

## 1. How AstroFind computes things / Como o AstroFind calcula

🔴 Advanced / Avançado

### 1.1 Star detection (SEP) / Detecção de estrelas (SEP)

🇬🇧 **English**
*In short: AstroFind finds every star-like spot in each image with a well-known library (SEP).
The strength of a spot compared to the noise decides if it counts.*

- Library: **SEP** (the C library version of Source Extractor), run in parallel on all images
  during **Astrometry → Data Reduction...** (`Ctrl+A`).
- Parameters: threshold = **Settings → Detection → Detection threshold:** (`detection/sigmaLimit`,
  default 4σ; σ = sky noise), minimum area 5 connected pixels, deblending on, 3×3 matched
  (convolution) filter, at most **500** sources kept (the brightest by flux).
- After SEP, the **Minimum FWHM:** filter removes sources that are too sharp (hot pixels,
  cosmic rays).
  The value is in **arcseconds** (the field shows ″); the code divides it by the pixel scale.
  (In 1.1.0 and earlier the field was wrongly labelled "px".)
- Log line: `Image N: M stars found`.

🇧🇷 **Português**
*Resumindo: o AstroFind acha todo ponto parecido com estrela em cada imagem usando uma biblioteca
conhecida (SEP). A força do ponto em relação ao ruído decide se ele conta.*

- Biblioteca: **SEP** (a versão em biblioteca C do Source Extractor), rodando em paralelo em
  todas as imagens durante **Astrometria → Redução de Dados...** (`Ctrl+A`).
- Parâmetros: limiar = **Configurações → Detecção → Limiar de detecção:** (`detection/sigmaLimit`,
  padrão 4σ; σ = ruído do céu), área mínima de 5 pixels conectados, deblending ligado, filtro
  casado (convolução) 3×3, no máximo **500** fontes mantidas (as mais brilhantes em fluxo).
- Depois do SEP, o filtro **FWHM mínimo:** remove fontes finas demais (pixels quentes, raios
  cósmicos).
  O valor está em **segundos de arco** (o campo mostra ″); o código divide pela escala de
  pixel. (Na 1.1.0 e anteriores o campo tinha o rótulo errado "px".)
- Linha de log: `Image N: M stars found`.

### 1.2 Centroid and elliptical PSF / Centroide e PSF elíptica

🇬🇧 **English**
*In short: when you click an object, AstroFind fits a tilted oval "bell" shape to the light. The
centre of that shape is the position it reports, to a fraction of a pixel.*

- Called by the measurement pipeline (Aperture tool `A`, Measure button `M`, or a click in the
  blink view): `findCentroidElliptical`.
- Seed: an intensity-weighted centroid, then a circular Gaussian fit. Final: a **6-parameter
  elliptical Gaussian** (x₀, y₀, σ_a, σ_b, θ, amplitude) fitted by **Levenberg–Marquardt** (a
  robust least-squares method).
  - In the code the sky is removed first, not fitted.
  - The loop stops after at most 60 iterations, or earlier if it diverges.
  - TR §9 shows a model with a background term B and "200 iterations"; the code is as described
    here.
- FWHM = 2√(2 ln 2)·σ for each axis.
- The log shows `PSF: FWHM=…"`. When elongation > 1.15 it shows FWHM_a, FWHM_b, θ and elongation.
  When elongation > 1.5 it warns `PSF elongation=… — check tracking, focus, or coma`.
- If no source is found at the click, the log shows `Centroid failed — no source found at click position`.
- TR: [§9 Elliptical PSF](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#9-elliptical-psf-fitting-levenberg-marquardt--ajuste-de-psf-elíptica-levenberg-marquardt).

🇧🇷 **Português**
*Resumindo: quando você clica num objeto, o AstroFind ajusta à luz um "sino" ovalado e inclinado.
O centro desse formato é a posição informada, com precisão de fração de pixel.*

- Chamado pela medição (ferramenta Abertura `A`, botão Medir `M` ou clique na visão de piscar):
  `findCentroidElliptical`.
- Semente: centroide ponderado pela intensidade, depois um ajuste gaussiano circular. Final:
  **gaussiana elíptica de 6 parâmetros** (x₀, y₀, σ_a, σ_b, θ, amplitude) ajustada por
  **Levenberg–Marquardt** (um método robusto de mínimos quadrados).
  - No código, o céu é removido antes, e não ajustado.
  - O laço para em no máximo 60 iterações, ou antes se divergir.
  - O TR §9 mostra um modelo com termo de fundo B e "200 iterações"; o código é como descrito aqui.
- FWHM = 2√(2 ln 2)·σ em cada eixo.
- O log mostra `PSF: FWHM=…"`. Com elongação > 1,15 mostra FWHM_a, FWHM_b, θ e elongação. Com
  elongação > 1,5 avisa `PSF elongation=… — check tracking, focus, or coma`.
- Se nenhuma fonte for achada no clique: `Centroid failed — no source found at click position`.
- TR: [§9 PSF elíptica](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#9-elliptical-psf-fitting-levenberg-marquardt--ajuste-de-psf-elíptica-levenberg-marquardt).

### 1.3 WCS pipeline and projections / Pipeline WCS e projeções

🇬🇧 **English**
*In short: a "plate solution" (WCS) is the map from pixel to sky position. AstroFind gets it from
an external solver, or from the file itself, and then only applies it.*

- Source of the WCS:
  - A WCS already in the header (CD matrix) is used and solving is skipped
    (`WCS pré-existente — plate solving ignorado`).
  - Otherwise the image is solved by **astrometry.net** (online, the default) or **ASTAP**
    (local) — **Settings → Connections → Backend:**.
- AstroFind **does not refit plate constants** against UCAC4/Gaia. The catalog you choose is
  used to *check* the solution (RMS), for photometry, and for the `astCat` label (see §1.12).
- Pixel → sky follows Calabretta & Greisen (2002): CD matrix → intermediate coordinates →
  native spherical (per projection) → celestial rotation.
- Projections read from `CTYPE1`: **TAN** (default), **SIN**, **ARC**, **STG**, **CAR**,
  **MER**, **GLS/SFL**, **AIT**.
- **Fixed (AUD-CORR-10):** `LONPOLE`/`LATPOLE` (and their `PV1_3`/`PV1_4` aliases) are now read
  and override the default pole per Calabretta & Greisen (2002) §2.4, eqs. 8-10, when present; a
  pair that admits no valid celestial pole is logged as a warning and falls back to the default.
  `PV1_1`/`PV1_2` (moving the native fiducial point) are still not supported and are logged.
  *Version 1.1.0 and earlier always assumed the default pole, ignoring these cards.* This matters
  only for unusual wide-field headers.
- TR: [§3 WCS pipeline](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#3-wcs-pipeline-pixel-to-sky--pipeline-wcs-pixel-para-céu),
  [§4 Projections](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#4-wcs-projections--projeções-wcs).

🇧🇷 **Português**
*Resumindo: a "solução de placa" (WCS) é o mapa de pixel para posição no céu. O AstroFind a
obtém de um resolvedor externo, ou do próprio arquivo, e depois só a aplica.*

- Origem do WCS:
  - Um WCS que já está no cabeçalho (matriz CD) é usado e a solução é pulada
    (`WCS pré-existente — plate solving ignorado`).
  - Senão, a imagem é resolvida pelo **astrometry.net** (online, padrão) ou pelo **ASTAP**
    (local) — **Configurações → Conexões → Backend:**.
- O AstroFind **não reajusta as constantes de placa** contra UCAC4/Gaia. O catálogo escolhido
  serve para *conferir* a solução (RMS), para a fotometria e para o rótulo `astCat` (veja §1.12).
- Pixel → céu segue Calabretta & Greisen (2002): matriz CD → coordenadas intermediárias →
  esféricas nativas (por projeção) → rotação celeste.
- Projeções lidas do `CTYPE1`: **TAN** (padrão), **SIN**, **ARC**, **STG**, **CAR**, **MER**,
  **GLS/SFL**, **AIT**.
- **Corrigido (AUD-CORR-10):** `LONPOLE`/`LATPOLE` (e seus aliases `PV1_3`/`PV1_4`) agora são
  lidos e sobrescrevem o polo padrão conforme Calabretta & Greisen (2002) §2.4, eqs. 8-10, quando
  presentes; um par que não admite nenhum polo celeste válido gera aviso no log e volta ao padrão.
  `PV1_1`/`PV1_2` (deslocar o ponto fiducial nativo) ainda não são suportados e também geram aviso.
  *Na versão 1.1.0 e anteriores, o polo padrão era sempre assumido, ignorando esses cartões.* Só
  importa em cabeçalhos de campo largo incomuns.
- TR: [§3 Pipeline WCS](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#3-wcs-pipeline-pixel-to-sky--pipeline-wcs-pixel-para-céu),
  [§4 Projeções](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#4-wcs-projections--projeções-wcs).

### 1.4 Refraction (Bennett) / Refração (Bennett)

🇬🇧 **English**
*In short: air bends starlight upward. AstroFind removes a standard amount of that bending from
a measured position, but only when the position was not already fitted against catalog stars
(which are bent by the same air).*

- Formula: **Bennett (1982)**, R = 1.02 / tan(a + 10.3/(a + 5.11)) arcmin, with a = apparent
  altitude. There is no pressure or temperature input (a standard atmosphere is assumed).
- Skipped below 1° altitude.
- **Fixed (AUD-CORR-7).** `shouldApplyRefraction()` now gates the call and returns `true` only
  when **all** of these are true:
  - the image is **not** a space telescope (`isSpaceTelescope`);
  - the position was **not** derived from a catalog plate solution (`FitsImage::wcs.solved`,
    i.e. fitted by `astrometry.net`/ASTAP against Gaia/UCAC4/2MASS): such a fit is refracted along
    with its reference stars and already absorbs the mean refraction, so correcting it again would
    double it (up to ~1.7′ at 30° altitude);
  - the JD is valid.
- In practice, essentially every measured position today comes from a plate solution, so Bennett
  does not run on the reported RA/Dec; it would only apply to a ground-based position obtained
  some other way (e.g. raw pointing/mount coordinates). Log when skipped for this reason:
  `Refraction: not applied (absorbed by the catalog plate solution)`. Log when applied:
  `Refraction correction: X" (R=Y')`.
- *Version 1.1.0 and earlier applied the full Bennett term on top of every ground-based plate
  solution regardless, double-correcting refraction by up to ~1.7′ at 30° altitude.*
- TR: [§5 Refraction](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#5-atmospheric-refraction--refração-atmosférica).

🇧🇷 **Português**
*Resumindo: o ar desvia a luz das estrelas para cima. O AstroFind remove uma quantidade padrão
desse desvio de uma posição medida, mas só quando ela ainda não foi ajustada contra estrelas de
catálogo (desviadas pelo mesmo ar).*

- Fórmula: **Bennett (1982)**, R = 1,02 / tan(a + 10,3/(a + 5,11)) arcmin, com a = altitude
  aparente. Não há entrada de pressão ou temperatura (assume atmosfera padrão).
- Pulada abaixo de 1° de altitude.
- **Corrigido (AUD-CORR-7).** `shouldApplyRefraction()` agora protege a chamada e só devolve
  `true` quando **todas** estas condições valem:
  - a imagem **não** é de telescópio espacial (`isSpaceTelescope`);
  - a posição **não** veio de uma solução de plate-solve por catálogo (`FitsImage::wcs.solved`,
    ajustada pelo `astrometry.net`/ASTAP contra Gaia/UCAC4/2MASS): esse ajuste é refratado junto
    com suas estrelas de referência e já absorve a refração média, então corrigi-la de novo
    dobraria o efeito (até ~1,7′ a 30° de altitude);
  - o JD é válido.
- Na prática, praticamente toda posição medida hoje vem de uma solução de plate-solve, então
  Bennett não roda sobre o RA/Dec reportado; ele só se aplicaria a uma posição terrestre obtida
  de outro jeito (ex.: coordenadas brutas de apontamento/montagem). Log quando pulada por esse
  motivo: `Refraction: not applied (absorbed by the catalog plate solution)`. Log quando aplicada:
  `Refraction correction: X" (R=Y')`.
- *Na versão 1.1.0 e anteriores, o termo de Bennett inteiro era aplicado por cima de toda solução
  de plate-solve terrestre, corrigindo a refração duas vezes em até ~1,7′ a 30° de altitude.*
- TR: [§5 Refração](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#5-atmospheric-refraction--refração-atmosférica).

### 1.5 ICRS → CIRS → topocentric: what is really applied / O que é realmente aplicado

🇬🇧 **English**
*In short: the reported RA/Dec is the plate-solution position as-is (refraction removal only
applies when the position did NOT come from that plate solution, see §1.4). AstroFind does not
add its own aberration, precession or nutation corrections.*

What happens to a measured position before it goes into the ADES report:

| Step | Applied to the output? |
|---|---|
| WCS pixel → sky (ICRS-aligned, from the plate solution) | ✅ yes |
| Atmospheric refraction removal (Bennett) | conditional: only ground-based **and** not from a catalog plate solution (see §1.4, AUD-CORR-7) |
| Proper motion of catalog stars to the image epoch | only to *catalog stars* used for checks and photometry, never to the target. UCAC4 only: the Gaia DR3 query does not fetch proper motions, so Gaia stars stay at epoch J2016 |
| Annual aberration | ❌ computed and **logged only** (`Frame: ICRF — annual aberration at epoch: …`) |
| Precession / nutation | ❌ not implemented (the dead code was removed in the audit, AUD-CORR-4) |
| Diurnal (topocentric) parallax | ❌ not applied; the MPC applies it from your station code |

- The rationale for skipping aberration/precession/nutation: a plate solution fitted against an
  ICRS catalog at the epoch of observation already contains them, so applying them again would
  double-correct.
- ADES tags positions with `sys=ICRF` for that reason. It does **not** mean AstroFind ran a full
  ICRS→CIRS chain.
- **Fixed (AUD-DOC-8):** the in-app Help no longer describes a "complete chain"; it now matches
  this page and TR §6.
- Why the site still matters: refraction and airmass use latitude/longitude, and the MPC code in
  the report is what the MPC uses for parallax. If the site is 0°, 0°, Data Reduction warns
  `Localização não configurada`.
- TR: [§6 Coordinate chain](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#6-coordinate-chain-icrs--cirs--topocentric--cadeia-de-coordenadas-icrs--cirs--topocêntrico),
  [§7 Aberration](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#7-annual-aberration--aberração-anual),
  [§8 Precession/nutation](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#8-precession-and-nutation--precessão-e-nutação).

🇧🇷 **Português**
*Resumindo: o RA/Dec informado é a posição da solução de placa como está (a remoção de refração
só se aplica quando a posição NÃO veio dessa solução de placa, veja §1.4). O AstroFind não soma
correções próprias de aberração, precessão ou nutação.*

O que acontece com uma posição medida antes de ir para o relatório ADES:

| Etapa | Aplicada na saída? |
|---|---|
| WCS pixel → céu (alinhado ao ICRS, da solução de placa) | ✅ sim |
| Remoção da refração atmosférica (Bennett) | condicional: só em solo **e** não vinda de plate-solve por catálogo (veja §1.4, AUD-CORR-7) |
| Movimento próprio das estrelas de catálogo até a época da imagem | só nas *estrelas de catálogo* usadas para conferência e fotometria, nunca no alvo. Só UCAC4: a consulta ao Gaia DR3 não traz movimentos próprios, então as estrelas Gaia ficam na época J2016 |
| Aberração anual | ❌ calculada e **só registrada no log** (`Frame: ICRF — annual aberration at epoch: …`) |
| Precessão / nutação | ❌ não implementadas (o código morto foi removido na auditoria, AUD-CORR-4) |
| Paralaxe diurna (topocêntrica) | ❌ não aplicada; o MPC aplica a partir do seu código de estação |

- O motivo para não aplicar aberração/precessão/nutação: uma solução de placa ajustada contra um
  catálogo ICRS na época da observação já as contém, e aplicá-las de novo seria corrigir duas
  vezes.
- Por isso o ADES marca as posições com `sys=ICRF`. Isso **não** quer dizer que o AstroFind rodou
  uma cadeia ICRS→CIRS completa.
- **Corrigido (AUD-DOC-8):** a Ajuda do programa não descreve mais uma "cadeia completa"; agora
  corresponde a esta página e ao TR §6.
- Por que o local ainda importa: refração e massa de ar usam latitude/longitude, e o código MPC
  do relatório é o que o MPC usa para a paralaxe. Se o local for 0°, 0°, a Redução de Dados avisa
  `Localização não configurada`.
- TR: [§6 Cadeia de coordenadas](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#6-coordinate-chain-icrs--cirs--topocentric--cadeia-de-coordenadas-icrs--cirs--topocêntrico),
  [§7 Aberração](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#7-annual-aberration--aberração-anual),
  [§8 Precessão/nutação](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#8-precession-and-nutation--precessão-e-nutação).

### 1.6 Photometry, zero-point, airmass, extinction / Fotometria, ponto zero, massa de ar, extinção

🇬🇧 **English**
*In short: AstroFind adds up the light inside a small circle around the object and compares it
with catalog stars of known brightness. That gives a magnitude, which is useful but not
precision photometry.*

- **Aperture.** Radius = max(3, 2×FWHM) px in automatic mode, or the fixed value
  (**Settings → Detection → Aperture Radius**).
  - The sky is the median of an annulus (ring) from 1.5× to 2.5× the aperture radius.
- **Instrumental magnitude**: m = −2.5 log₁₀(flux / exposure).
- **Zero-point** (the constant that turns instrumental into catalog magnitude):
  - First choice: a *differential* zero-point from nearby catalog stars.
  - Fallback: a global zero-point.
  - Last resort: the instrumental magnitude (`Photometry: no catalog stars matched — using instrumental mag`).
- Catalog magnitudes: UCAC4 `Vmag` or Gaia `Gmag`. The reported `band` is the FITS `FILTER`, or
  **Default band:** — so an unfiltered (C) image calibrated on V or G is only approximate.
- **Airmass** is always computed and logged. The code uses the **Pickering (2002)** formula; TR
  §12 describes sec z / Young & Irvine, which is not what the code does.
- **Extinction**: −k·X is added only when **Extinction coeff k:** > 0 (default 0 = off).
- To tune the aperture, use **Tools → Growth Curve…** (`Ctrl+Shift+G`) / **Ferramentas → Curva de Crescimento…**.
- TR: [§10](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#10-aperture-photometry-and-zero-point--fotometria-de-abertura-e-zero-point),
  [§11](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#11-differential-photometry--fotometria-diferencial),
  [§12](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#12-airmass--massa-de-ar).

🇧🇷 **Português**
*Resumindo: o AstroFind soma a luz dentro de um pequeno círculo ao redor do objeto e compara com
estrelas de catálogo de brilho conhecido. Isso dá uma magnitude útil, mas não é fotometria de
precisão.*

- **Abertura.** Raio = max(3, 2×FWHM) px no modo automático, ou o valor fixo
  (**Configurações → Detecção → Raio da abertura**).
  - O céu é a mediana de um anel de 1,5× a 2,5× o raio da abertura.
- **Magnitude instrumental**: m = −2,5 log₁₀(fluxo / exposição).
- **Ponto zero** (a constante que transforma magnitude instrumental em magnitude de catálogo):
  - Primeira opção: um ponto zero *diferencial* com estrelas de catálogo próximas.
  - Alternativa: um ponto zero global.
  - Último recurso: a magnitude instrumental (`Photometry: no catalog stars matched — using instrumental mag`).
- Magnitudes de catálogo: `Vmag` do UCAC4 ou `Gmag` do Gaia. A `band` informada é o `FILTER` do
  FITS, ou **Banda padrão:** — então uma imagem sem filtro (C) calibrada em V ou G é só
  aproximada.
- **Massa de ar** é sempre calculada e registrada. O código usa a fórmula de **Pickering (2002)**;
  o TR §12 descreve sec z / Young & Irvine, que não é o que o código faz.
- **Extinção**: −k·X só é somado quando **Coef. de extinção k:** > 0 (padrão 0 = desligado).
- Para ajustar a abertura, use **Ferramentas → Curva de Crescimento…** (`Ctrl+Shift+G`).
- TR: [§10](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#10-aperture-photometry-and-zero-point--fotometria-de-abertura-e-zero-point),
  [§11](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#11-differential-photometry--fotometria-diferencial),
  [§12](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#12-airmass--massa-de-ar).

### 1.7 FFT registration and stacking / Registro por FFT e empilhamento

🇬🇧 **English**
*In short: to stack images, AstroFind first lines them up, using the stars or a frequency-domain
trick (FFT). Then it averages, medians or adds them.*

- **Images → Re-Stack Images** / **Imagens → Re-empilhar Imagens**: aligns on the star list when
  every image has detected stars; otherwise it uses **FFT phase correlation** (fftw3). The
  output is `stacked.fits`.
  - In the code the sub-pixel peak is found by parabolic interpolation (TR §15 says Gaussian fit).
- **Astrometry → Stack Images...** (`Ctrl+T`) / **Astrometria → Empilhar Imagens...**: *Track &
  Stack*. You type the object's motion as dX/dY per frame (−500…500 px). The output is
  `track_stacked.fits`. Use it for objects too faint in single frames.
- Modes: **Average**, **Median** (rejects outliers such as satellites), **Add** (keeps total
  counts).
- TR: [§15 FFT registration](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#15-fft-image-registration-phase-correlation--registro-de-imagem-por-fft-correlação-de-fase).

🇧🇷 **Português**
*Resumindo: para empilhar, o AstroFind primeiro alinha as imagens, pelas estrelas ou por um
truque no domínio de frequência (FFT). Depois tira a média, a mediana ou soma.*

- **Imagens → Re-empilhar Imagens**: alinha pela lista de estrelas quando toda imagem tem estrelas
  detectadas; senão usa **correlação de fase por FFT** (fftw3). A saída é `stacked.fits`.
  - No código, o pico subpixel é achado por interpolação parabólica (o TR §15 diz ajuste gaussiano).
- **Astrometria → Empilhar Imagens...** (`Ctrl+T`): *Track & Stack*. Você digita o movimento do
  objeto como dX/dY por quadro (−500…500 px). A saída é `track_stacked.fits`. Use para objetos
  fracos demais em quadros isolados.
- Modos: **Average** (média), **Median** (mediana; rejeita discrepantes como satélites), **Add**
  (soma; preserva a contagem total).
- TR: [§15 Registro por FFT](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#15-fft-image-registration-phase-correlation--registro-de-imagem-por-fft-correlação-de-fase).

### 1.8 Streaks and ClumpFind / Traços e ClumpFind

🇬🇧 **English**
*In short: very long spots are flagged as trails (fast asteroids, satellites). Two stars merged
into one spot are split apart.*

- **Streaks**: elongation a/b (from SEP second moments) ≥ **Settings → Detection → Streak
  threshold (a/b):** (`detection/streakElongation`, default **3.0**; TR §17 says 4.0, the code
  uses 3.0). They are drawn as orange rotated ellipses, and the log adds
  `(N streak/trail candidate(s) — shown in orange)`.
- **Blended sources**: sources that SEP marks as merged, plus a ClumpFind-style multi-peak scan
  (isophote levels stepping down from the peak). They are drawn as magenta double circles.
- TR: [§17](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#17-streak-detection-by-blob-elongation--detecção-de-traço-por-elongação-de-blob),
  [§18](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#18-clumpfind-blended-source-separation--clumpfind-separação-de-fontes-blendadas).

🇧🇷 **Português**
*Resumindo: manchas muito compridas são marcadas como traços (asteroides rápidos, satélites).
Duas estrelas fundidas numa mancha só são separadas.*

- **Traços**: elongação a/b (dos segundos momentos do SEP) ≥ **Configurações → Detecção → Limiar de
  traço (a/b):** (`detection/streakElongation`, padrão **3,0**; o TR §17 diz 4,0, o código usa
  3,0). São desenhados como elipses laranja giradas, e o log acrescenta
  `(N streak/trail candidate(s) — shown in orange)`.
- **Fontes blendadas**: as que o SEP marca como fundidas, mais uma varredura multipico estilo
  ClumpFind (níveis de isofota descendo a partir do pico). São desenhadas como círculos duplos
  magenta.
- TR: [§17](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#17-streak-detection-by-blob-elongation--detecção-de-traço-por-elongação-de-blob),
  [§18](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#18-clumpfind-blended-source-separation--clumpfind-separação-de-fontes-blendadas).

### 1.9 Background, bad pixels, power spectrum / Fundo, pixels ruins, espectro de potência

🇬🇧 **English**
*In short: before detection AstroFind can flatten a sky gradient and repair isolated bad pixels.
A frequency plot helps you spot periodic noise and focus problems.*

- **Sliding-median background** (off by default: **Settings → Detection → Subtrair modelo de
  fundo antes da detecção**):
  - The image is cut into tiles (default 64 px). Each tile gets a 3σ-clipped median, and the
    tile values are interpolated bilinearly.
  - Small tiles follow gradients better, but they can eat nebulosity.
  - TR: [§19](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#19-sliding-median-background-estimator--estimador-de-fundo-por-mediana-deslizante).
- **Bad-pixel mask** (on by default, σ = 5):
  - Works on each image on its own: each pixel is compared with its 3×3 median, using noise
    σ = 1.4826·MAD.
  - Flagged pixels are replaced by the median of their valid 3×3 neighbours.
  - This runs after dark/flat and before detection.
  - TR §16 describes a multi-frame stack with 4-neighbour interpolation; the code is as described
    here.
  - TR: [§16](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#16-bad-pixel-correction--correção-de-pixels-ruins).
- **Power spectrum** (right-click on the image → **Show Power Spectrum** / **Mostrar Espectro de
  Potência**):
  - Hann-windowed 2-D FFT shown as log(1+|F|²), computed on at most 1024×1024 px.
  - Bright off-centre spots mean periodic noise, for example readout pattern noise.
  - TR: [§20](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#20-2-d-power-spectrum--espectro-de-potência-2-d).

🇧🇷 **Português**
*Resumindo: antes da detecção, o AstroFind pode achatar um gradiente de céu e consertar pixels
ruins isolados. Um gráfico de frequências ajuda a achar ruído periódico e problemas de foco.*

- **Fundo por mediana deslizante** (desligado por padrão: **Configurações → Detecção → Subtrair
  modelo de fundo antes da detecção**):
  - A imagem é dividida em blocos (padrão 64 px). Cada bloco recebe uma mediana com corte 3σ, e
    os valores dos blocos são interpolados bilinearmente.
  - Blocos pequenos seguem melhor os gradientes, mas podem "comer" nebulosidade.
  - TR: [§19](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#19-sliding-median-background-estimator--estimador-de-fundo-por-mediana-deslizante).
- **Máscara de pixels ruins** (ligada por padrão, σ = 5):
  - Funciona em cada imagem isoladamente: cada pixel é comparado com a mediana 3×3, usando ruído
    σ = 1,4826·MAD.
  - Pixels marcados são trocados pela mediana dos vizinhos 3×3 válidos.
  - Roda depois de dark/flat e antes da detecção.
  - O TR §16 descreve uma pilha multiquadro com interpolação de 4 vizinhos; o código é como
    descrito aqui.
  - TR: [§16](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#16-bad-pixel-correction--correção-de-pixels-ruins).
- **Espectro de potência** (clique direito na imagem → **Mostrar Espectro de Potência**):
  - FFT 2-D com janela de Hann, mostrada como log(1+|F|²), calculada em no máximo 1024×1024 px.
  - Pontos brilhantes fora do centro indicam ruído periódico, por exemplo padrão de leitura.
  - TR: [§20](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#20-2-d-power-spectrum--espectro-de-potência-2-d).

### 1.10 Moving-object detection / Detecção de objetos em movimento

🇬🇧 **English**
*In short: AstroFind looks for sources that move in a straight line at a steady rate across your
frames.*

- **Astrometry → Moving Object Detection...** (`Ctrl+M`) / **Astrometria → Detecção de Objetos em
  Movimento...** needs at least 2 images with detected stars.
- It pairs sources across frames and groups motion vectors that agree within
  `detection/modTolerance` (2 px). It keeps tracks seen in ≥ `detection/modMinFrames` (3) frames
  with SNR ≥ **MOD min. SNR:** (5).
- Tolerance and minimum frames have no field in the Settings window. They can only be changed
  in the settings file (§4.4).
- Candidates appear as `Cand #N`.

🇧🇷 **Português**
*Resumindo: o AstroFind procura fontes que andam em linha reta, com velocidade constante, ao
longo dos seus quadros.*

- **Astrometria → Detecção de Objetos em Movimento...** (`Ctrl+M`) precisa de pelo menos 2 imagens
  com estrelas detectadas.
- Pareia fontes entre quadros e agrupa vetores de movimento que concordam dentro de
  `detection/modTolerance` (2 px). Mantém trilhas vistas em ≥ `detection/modMinFrames` (3)
  quadros com SNR ≥ **MOD SNR mín.:** (5).
- Tolerância e mínimo de quadros não têm campo na janela de Configurações. Só dá para mudá-los no
  arquivo de configurações (§4.4).
- Candidatos aparecem como `Cand #N`.

### 1.11 Ecliptic / galactic overlay / Sobreposição eclíptica / galáctica

🇬🇧 **English**
*In short: AstroFind can draw the ecliptic and the Milky Way plane on the image, and it warns
you when the field is crowded and dimmed by dust.*

- **Tools → Ecliptic / Galactic Overlay** (`Ctrl+E`).
- The warning badge appears when |b| < **15°** (the code value; TR §14 says 10°).
- TR: [§14](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#14-ecliptic-and-galactic-coordinates--coordenadas-eclípticas-e-galácticas).

🇧🇷 **Português**
*Resumindo: o AstroFind pode desenhar a eclíptica e o plano da Via Láctea na imagem, e avisa
quando o campo está lotado e apagado pela poeira.*

- **Ferramentas → Sobreposição Eclíptica / Galáctica** (`Ctrl+E`).
- O selo de aviso aparece quando |b| < **15°** (valor do código; o TR §14 diz 10°).
- TR: [§14](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#14-ecliptic-and-galactic-coordinates--coordenadas-eclípticas-e-galácticas).

### 1.12 ADES XML / PSV fields / Campos ADES XML / PSV

🇬🇧 **English**
*In short: the report is the standard MPC ADES 2022 format, as XML or as a pipe-separated table.
The table below says where each field comes from, so you know which ones to double-check.*

| Field | Source in AstroFind |
|---|---|
| `permID` / `provID` / `trkSub` | Object name from the Verification dialog or the Observations table: numbered → permID, provisional → provID, otherwise trkSub (default `UNKN`) |
| `mode`, `stn` | `CCD`; MPC code from Settings (or `XXX` if empty) |
| `obsTime` | Image JD → ISO 8601 with `Z`. The number of decimals comes from **Time Precision:** read as an integer (see §2) |
| `ra`, `dec` | 9 decimals, degrees |
| `sys` | always `ICRF` (see §1.5) |
| `rmsRA`, `rmsDec` | The same value in both: the image's **WCS RMS** (radial RMS of nearest-neighbour matches within 5 px, ≥ 3 stars). Default 0.5″. This is a field-level fit figure, not a per-object error |
| `rmsCorr` | always `0.0000` |
| `astCat` | `UCAC4` or `GaiaDR3`, from **Catalog:**. ⚠️ The astrometric solution itself came from astrometry.net/ASTAP and their own reference index, so this label shows the catalog you *checked* against, which may not be the one the plate was *fitted* against |
| `mag`, `rmsMag`, `band`, `photCat` | Only when a magnitude exists and **Include magnitude in ADES report** is on |
| `notes` | Comet notes N/T/C when applicable |
| `obsContext` | observatory/mpcCode, submitter, observers, measurers, telescope, software `AstroFind`; contact only if **Include contact info in ADES report** is on |

- The PSV header is `# version=2022`.
- There is **no MPC 80-column** output.
- The XML example in TR §21 is an illustrative ADES 2017 block with `<ctr>` and telescope
  details; AstroFind does not write those.
- TR: [§21](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#21-ades--mpc-report-format--formato-de-relatório-ades--mpc).

🇧🇷 **Português**
*Resumindo: o relatório é o formato padrão ADES 2022 do MPC, em XML ou em tabela separada por
barras. A tabela abaixo diz de onde vem cada campo, para você saber quais conferir.*

| Campo | Origem no AstroFind |
|---|---|
| `permID` / `provID` / `trkSub` | Nome do objeto no diálogo Verificação ou na tabela Observações: numerado → permID, provisório → provID, senão trkSub (padrão `UNKN`) |
| `mode`, `stn` | `CCD`; código MPC das Configurações (ou `XXX` se vazio) |
| `obsTime` | JD da imagem → ISO 8601 com `Z`. O número de decimais vem de **Precisão de tempo:** lida como inteiro (veja §2) |
| `ra`, `dec` | 9 decimais, graus |
| `sys` | sempre `ICRF` (veja §1.5) |
| `rmsRA`, `rmsDec` | O mesmo valor nos dois: o **RMS do WCS** da imagem (RMS radial dos pares mais próximos até 5 px, ≥ 3 estrelas). Padrão 0,5″. É um número do ajuste do campo, não o erro de cada objeto |
| `rmsCorr` | sempre `0.0000` |
| `astCat` | `UCAC4` ou `GaiaDR3`, de **Catálogo:**. ⚠️ A solução astrométrica em si veio do astrometry.net/ASTAP e do índice de referência deles, então esse rótulo mostra o catálogo com que você *conferiu*, que pode não ser aquele contra o qual a placa foi *ajustada* |
| `mag`, `rmsMag`, `band`, `photCat` | Só quando existe magnitude e **Incluir magnitude no relatório ADES** está ligado |
| `notes` | Notas de cometa N/T/C quando aplicável |
| `obsContext` | observatory/mpcCode, submitter, observers, measurers, telescope, software `AstroFind`; contato só se **Incluir informações de contato no relatório ADES** estiver ligado |

- O cabeçalho do PSV é `# version=2022`.
- **Não há** saída no formato MPC de 80 colunas.
- O exemplo XML do TR §21 é um bloco ilustrativo ADES 2017 com `<ctr>` e detalhes do telescópio;
  o AstroFind não escreve esses campos.
- TR: [§21](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md#21-ades--mpc-report-format--formato-de-relatório-ades--mpc).

---

## 2. Timing / Tempo

🔴 Advanced / Avançado

🇬🇧 **English**
*In short: an asteroid moves, so a wrong clock means a wrong position. Read this section fully
before you submit anything. Version 1.1.0 had time-handling traps that the next version (after
1.1.0) fixes; a note below says what changed.*

**How the image time is read** (FITS):

1. If the header has a `JD` keyword, it is used **as-is**. AstroFind assumes it is mid-exposure
   and does not shift it. If your software writes the JD at the *start* of the exposure, your
   times will be early by EXPTIME/2.
2. Otherwise JD = mid-exposure of `MJD-OBS` (preferred) or `DATE-OBS`, plus half of `EXPTIME`.
   Only `EXPTIME` is read, not `EXPOSURE`. **Fixed (AUD-CORR-13):** `MJD-OBS` is now read and
   preferred over `DATE-OBS` when finite and > 0; a warning is logged if the two disagree by more
   than 1 s. `DATE-AVG` is still not read. *Version 1.1.0 and earlier ignored `MJD-OBS` entirely
   and always used `DATE-OBS`.*
3. The time scale of `DATE-OBS`:
   - With `Z` → UTC.
   - With a `±HH:MM` offset → converted to UTC.
   - No designator but `TIMESYS=UTC`/`UT` → UTC.
   - No designator and `TIMESYS` missing or not UTC (TT, TAI…) → **assumed UTC and flagged
     `dateObsAmbiguous`**.
4. **DSLR RAW**: the EXIF time is the camera's wall clock, with an unknown time zone, so it is
   **always flagged** as ambiguous.
5. When an image is flagged, the log shows: `DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.`
   This Portuguese message appears in both UI languages.
6. **Fixed (AUD-CORR-13):** the conversion now keeps the **milliseconds** of the input instant
   (`QDateTime::msecsTo`, not the truncating `secsTo`). *Version 1.1.0 and earlier kept whole
   seconds only, dropping the fractional part of DATE-OBS.*

**What Data Reduction then does to the JD:**

- It adds **Settings → Observer → Time Offset:** as *seconds* (a camera-clock correction).
  This is done **once** per image: AstroFind remembers the correction already applied (it is
  also saved in the `.gus` project), so re-running Data Reduction or re-opening a project
  never adds it again. If you change the value, only the difference is applied.
- It does **not** add ΔT. The JD stays **UTC**, and the ADES `obsTime` is written from it with
  `Z`, as ADES expects.
- **Settings → Camera → ΔT (TT − UTC):** (default **68 s**) is used only for the **offline**
  MPCORB scan of known objects: the orbits are propagated in TT, so AstroFind adds ΔT to the
  UTC JD for that calculation only. The online SkyBoT query is sent in UTC (as the IMCCE
  documentation asks).
- **Fixed (AUD-CORR-12):** that offline propagator itself had two accuracy bugs: the MPCORB
  packed epoch was read half a day late, and the Sun position used to place Earth was left in the
  wrong equinox. Fixed; error against JPL Horizons dropped from 348″ to 13″ for Ceres and from
  2952″ to 49″ for Eros. The propagator still does not model light-time or use a high-precision
  Sun, so always cross-check against Horizons before submitting (§3).
- Time Offset is **never filled automatically**. On the first start of the next version
  (after 1.1.0), a stored non-zero Time Offset is reset to 0 once, with a warning in the log,
  because older versions filled it with a wrong value.
- **Time Zone:** (`observer/timeZone`) is saved, but nothing in the pipeline uses it. It does
  **not** convert a local-time DATE-OBS.

> ⚠️ **Version 1.1.0 and earlier (AUD-CORR-15).** There, Data Reduction added ΔT (68 s) and
> Time Offset to the JD **on every run**, and the report still labelled the result UTC, so
> `obsTime` came out about 68 s late (more after each re-run). Time Offset was also auto-filled
> with *longitude / 15* (hours, then read as seconds; longitude −35° gave −2.3 s). If you are
> still on 1.1.0: set **ΔT** and **Time Offset** to `0` before the first Data Reduction, run it
> only once per loaded session, and reload the images if you already ran it.

**Recommended procedure:**

1. Make sure your acquisition software writes `DATE-OBS` in UTC with `Z` (or `TIMESYS='UTC'`).
   This is the only fully reliable path. If your camera writes local time, fix the headers
   before loading.
2. In **File → Settings...** (`Ctrl+,`) / **Arquivo → Configurações...**, leave
   **Camera → ΔT (TT − UTC):** at its default, and set **Observer → Time Offset:** to `0`, or
   to a real clock error in seconds if you know one. The range is −999…999 s.
3. Run **Data Reduction** (`Ctrl+A`). Running it again is safe.
4. For a single image, or a DSLR with a known clock error, you can type the correct
   mid-exposure JD in **Images → Edit Image Parameters...** / **Imagens → Editar Parâmetros da
   Imagem...** (field "Julian Date:", 6 decimals ≈ 0.09 s). If the JD you type is already
   corrected, keep Time Offset at 0.
5. Before submitting, compare the `obsTime` in the ADES preview with the mid-exposure UTC you
   expect.

**What you should see:** the ADES `obsTime` equals DATE-OBS (UTC) + EXPTIME/2, to the precision
you chose.

> 💡 **Tip — DSLR clocks.** Camera clocks drift by seconds per week and are often set to local
> time or DST. Before a session, photograph a GPS/NTP-synced clock on a phone screen and note the
> difference. Then correct each frame's JD as in step 4. A 1 s error on an asteroid moving
> 1″/s gives a 1″ error along its path.

> 💡 **Time Precision.** A whole number 0–3: the **number of decimal places** of the seconds in
> `obsTime` (0 whole seconds, 1 tenths, 2 hundredths, 3 milliseconds). The default 1 gives
> tenths of a second. Use 1 or 2; more digits than your clock accuracy only look precise. (In
> 1.1.0 and earlier the field was wrongly labelled "h", with the same meaning.)

🇧🇷 **Português**
*Resumindo: um asteroide se move, então relógio errado = posição errada. Leia esta seção inteira
antes de enviar qualquer coisa. A versão 1.1.0 tinha armadilhas no tratamento de tempo que a
próxima versão (depois da 1.1.0) corrige; uma nota abaixo diz o que mudou.*

**Como o horário da imagem é lido** (FITS):

1. Se o cabeçalho tem a palavra-chave `JD`, ela é usada **como está**. O AstroFind supõe que é o
   meio da exposição e não a desloca. Se o seu software grava o JD no *início* da exposição, seus
   horários vão ficar adiantados em EXPTIME/2.
2. Senão, JD = meio da exposição a partir de `MJD-OBS` (preferido) ou `DATE-OBS`, mais metade do
   `EXPTIME`. Só `EXPTIME` é lido, não `EXPOSURE`. **Corrigido (AUD-CORR-13):** `MJD-OBS` agora é
   lido e preferido sobre `DATE-OBS` quando finito e > 0; um aviso é registrado no log se os dois
   discordarem em mais de 1 s. `DATE-AVG` continua não sendo lido. *Na versão 1.1.0 e anteriores,
   `MJD-OBS` era completamente ignorado e o `DATE-OBS` sempre era usado.*
3. A escala de tempo do `DATE-OBS`:
   - Com `Z` → UTC.
   - Com deslocamento `±HH:MM` → convertido para UTC.
   - Sem designador, mas com `TIMESYS=UTC`/`UT` → UTC.
   - Sem designador e com `TIMESYS` ausente ou não UTC (TT, TAI…) → **assumido UTC e marcado
     `dateObsAmbiguous`**.
4. **RAW de DSLR**: o horário EXIF é o relógio da câmera, com fuso desconhecido, então é **sempre
   marcado** como ambíguo.
5. Quando uma imagem é marcada, o log mostra: `DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.`
   Essa mensagem aparece nos dois idiomas da interface.
6. **Corrigido (AUD-CORR-13):** a conversão agora guarda os **milissegundos** do instante de
   entrada (`QDateTime::msecsTo`, não o `secsTo`, que trunca). *Na versão 1.1.0 e anteriores só
   segundos inteiros eram guardados, descartando a fração de segundo do DATE-OBS.*

**O que a Redução de Dados faz depois com o JD:**

- Soma **Configurações → Observador → Deslocamento de tempo:** como *segundos* (uma correção
  do relógio da câmera). Isso é feito **uma vez** por imagem: o AstroFind lembra a correção já
  aplicada (ela também fica salva no projeto `.gus`), então rodar a Redução de Dados de novo ou
  reabrir um projeto nunca soma outra vez. Se você mudar o valor, só a diferença é aplicada.
- **Não** soma o ΔT. O JD fica em **UTC**, e o `obsTime` do ADES é escrito a partir dele com
  `Z`, como o ADES espera.
- **Configurações → Câmera → ΔT (TT − UTC):** (padrão **68 s**) é usado só na busca
  **offline** de objetos conhecidos pelo MPCORB: as órbitas são propagadas em TT, então o
  AstroFind soma o ΔT ao JD UTC só para esse cálculo. A consulta online ao SkyBoT é enviada em
  UTC (como pede a documentação do IMCCE).
- **Corrigido (AUD-CORR-12):** esse propagador offline tinha dois erros de exatidão: a época
  empacotada do MPCORB era lida meio dia atrasada, e a posição do Sol usada para localizar a
  Terra ficava no equinócio errado. Corrigidos; o erro contra o JPL Horizons caiu de 348″ para
  13″ para Ceres e de 2952″ para 49″ para Eros. O propagador ainda não modela tempo-luz nem usa
  uma posição de alta precisão do Sol, então sempre confira contra o Horizons antes de enviar
  (§3).
- O Deslocamento de tempo **nunca é preenchido sozinho**. Na primeira vez que a próxima versão
  (depois da 1.1.0) abre, um Deslocamento de tempo diferente de 0 é zerado uma vez, com um
  aviso no log, porque as versões antigas o preenchiam com um valor errado.
- **Fuso horário:** (`observer/timeZone`) é salvo, mas nada no pipeline o usa. Ele **não**
  converte um DATE-OBS em hora local.

> ⚠️ **Versão 1.1.0 e anteriores (AUD-CORR-15).** Nelas, a Redução de Dados somava o ΔT (68 s)
> e o Deslocamento de tempo ao JD **a cada execução**, e o relatório ainda marcava o resultado
> como UTC, então o `obsTime` saía cerca de 68 s atrasado (mais a cada nova execução). O
> Deslocamento de tempo também era preenchido sozinho com *longitude / 15* (horas, lidas depois
> como segundos; longitude −35° dava −2,3 s). Se ainda usa a 1.1.0: ponha **ΔT** e
> **Deslocamento de tempo** em `0` antes da primeira Redução de Dados, rode só uma vez por
> sessão carregada e recarregue as imagens se já rodou.

**Procedimento recomendado:**

1. Garanta que o software de aquisição grave `DATE-OBS` em UTC com `Z` (ou `TIMESYS='UTC'`). Esse
   é o único caminho totalmente confiável. Se a câmera grava hora local, corrija os cabeçalhos
   antes de carregar.
2. Em **Arquivo → Configurações...** (`Ctrl+,`), deixe **Câmera → ΔT (TT − UTC):** no padrão,
   e ponha **Observador → Deslocamento de tempo:** em `0`, ou num erro real de relógio em
   segundos, se você souber. A faixa é −999…999 s.
3. Rode a **Redução de Dados** (`Ctrl+A`). Rodar de novo não causa problema.
4. Para uma imagem só, ou uma DSLR com erro de relógio conhecido, você pode digitar o JD correto
   do meio da exposição em **Imagens → Editar Parâmetros da Imagem...** (campo "Julian Date:", 6
   decimais ≈ 0,09 s). Se o JD digitado já está corrigido, mantenha o Deslocamento de tempo em 0.
5. Antes de enviar, compare o `obsTime` na pré-visualização ADES com o UTC do meio da exposição
   que você espera.

**O que você deve ver:** o `obsTime` do ADES igual a DATE-OBS (UTC) + EXPTIME/2, na precisão
escolhida.

> 💡 **Dica — relógios de DSLR.** Relógios de câmera derivam segundos por semana e muitas vezes
> estão em hora local ou horário de verão. Antes da sessão, fotografe um relógio sincronizado por
> GPS/NTP na tela do celular e anote a diferença. Depois corrija o JD de cada quadro como no passo
> 4. Um erro de 1 s num asteroide que anda 1″/s vira 1″ de erro ao longo da trajetória.

> 💡 **Precisão de tempo.** Um número inteiro de 0 a 3: o **número de casas decimais** dos
> segundos no `obsTime` (0 segundos inteiros, 1 décimos, 2 centésimos, 3 milésimos). O padrão 1
> dá décimos de segundo. Use 1 ou 2; mais dígitos do que a exatidão do seu relógio só parecem
> precisos. (Na 1.1.0 e anteriores o campo tinha o rótulo errado "h", com o mesmo significado.)

---

## 3. Data quality for MPC-grade astrometry / Qualidade de dados para astrometria nível MPC

🔴 Advanced / Avançado

🇬🇧 **English**
*In short: good positions need round, well-exposed images, a good time stamp and a sanity check
against known asteroids. This checklist is general good practice plus AstroFind-specific
checks.*

1. **Exposure vs motion.** Keep the trail during one exposure shorter than about one FWHM:
   t_max ≈ FWHM / rate.
   - Example: seeing 3″ and a main-belt asteroid at ~0.5″/min → up to ~6 min.
   - A fast NEO at 30″/min → ~6 s.
   - If the object is still too faint, take many short frames and use Track & Stack (§1.7).
2. **SNR.** The MOD default is SNR ≥ 5, which is enough to *find* an object. For astrometry, aim
   for SNR ≥ 10–20 on the target. Check the `SNR` column of the Observations dock.
3. **PSF shape.** If the log reports elongation > 1.5 on field stars, fix tracking, focus or coma
   first. An elongated PSF biases the centroid along the elongation.
4. **Reference catalog.**
   - The plate is fitted by astrometry.net/ASTAP, not by AstroFind (§1.3).
   - **Settings → Connections → Catalog:** affects the RMS check, the photometry and `astCat`.
   - Gaia DR3 has the better astrometry, but AstroFind fetches no Gaia proper motions (§1.5).
     UCAC4 is proper-motion corrected, but it is older and less accurate.
   - Keep **Catalog mag (bright):** so that saturated stars are excluded, and check
     **Saturation Level:**.
5. **Residual checks.**
   - Read `WCS RMS = …" (N matched stars)` in the log after **Tools → Known Object Overlay**
     (`Ctrl+K`). Values much above ~1″, or N below ~10, mean the solution or the matching is
     poor. Do not submit.
   - Measure one or two **known** asteroids in the same frames and compare them with an
     ephemeris. **Internet → Query Horizons…** is **geocentric** in AstroFind, so for near-Earth
     objects compare against a topocentric ephemeris for your station (for example the MPC or
     JPL web services).
   - Differences of a few tenths of an arcsecond are normal. Several arcseconds point to a time,
     refraction (§1.4) or solution problem.
6. **Site and station.** Set a real MPC code and site. A 0°, 0° site gives the geocentre warning,
   and the report uses `XXX` without a code.
7. **Before submitting**, re-read §2, check the ADES preview (`obsTime`, `stn`, `astCat`, `band`)
   and keep your raw FITS files.

🇧🇷 **Português**
*Resumindo: boas posições precisam de imagens redondas e bem expostas, de um bom registro de
horário e de uma conferência com asteroides conhecidos. Esta lista junta boas práticas gerais
com verificações próprias do AstroFind.*

1. **Exposição vs movimento.** Mantenha o traço durante uma exposição menor que cerca de um FWHM:
   t_max ≈ FWHM / taxa.
   - Exemplo: seeing 3″ e asteroide do cinturão a ~0,5″/min → até ~6 min.
   - Um NEO rápido a 30″/min → ~6 s.
   - Se o objeto ainda ficar fraco demais, faça muitos quadros curtos e use Track & Stack (§1.7).
2. **SNR.** O padrão do MOD é SNR ≥ 5, o que basta para *achar* um objeto. Para astrometria, busque
   SNR ≥ 10–20 no alvo. Veja a coluna `SNR` do painel Observações.
3. **Formato da PSF.** Se o log acusar elongação > 1,5 em estrelas de campo, corrija primeiro
   rastreamento, foco ou coma. PSF alongada puxa o centroide na direção da elongação.
4. **Catálogo de referência.**
   - A placa é ajustada pelo astrometry.net/ASTAP, não pelo AstroFind (§1.3).
   - **Configurações → Conexões → Catálogo:** afeta a conferência de RMS, a fotometria e o `astCat`.
   - O Gaia DR3 tem a melhor astrometria, mas o AstroFind não busca movimentos próprios do Gaia
     (§1.5). O UCAC4 tem correção de movimento próprio, mas é mais antigo e menos preciso.
   - Ajuste **Magnitude do catálogo (brilhante):** para excluir estrelas saturadas, e confira
     **Nível de saturação:**.
5. **Conferência de resíduos.**
   - Leia `WCS RMS = …" (N matched stars)` no log depois de **Ferramentas → Sobreposição de Objetos
     Conhecidos** (`Ctrl+K`). Valores bem acima de ~1″, ou N abaixo de ~10, indicam solução ou
     pareamento ruim. Não envie.
   - Meça um ou dois asteroides **conhecidos** nos mesmos quadros e compare com uma efeméride.
     **Internet → Consultar Horizons…** é **geocêntrico** no AstroFind, então para objetos
     próximos da Terra compare com uma efeméride topocêntrica para sua estação (por exemplo os
     serviços web do MPC ou do JPL).
   - Diferenças de alguns décimos de segundo de arco são normais. Vários segundos de arco indicam
     problema de horário, de refração (§1.4) ou de solução.
6. **Local e estação.** Configure um código MPC e um local reais. Local 0°, 0° dá o aviso de
   geocentro, e sem código o relatório usa `XXX`.
7. **Antes de enviar**, releia a §2, confira a pré-visualização ADES (`obsTime`, `stn`, `astCat`,
   `band`) e guarde seus FITS originais.

---

## 4. Command line and files / Linha de comando e arquivos

🔴 Advanced / Avançado

### 4.1 Build from source / Compilar do código

🇬🇧 **English**
*In short: you download the code and let CMake compile it. Optional libraries are detected
automatically. If one is missing you only lose that feature.*

Install the build dependencies for your distro from
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md) (the single source of
truth for package names). Then:

```bash
git clone https://github.com/petrinhu/astrofind.git && cd astrofind
```
Downloads the source code and enters its folder.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
Configures the build into `build/`. The output lists which optional features were found.

```bash
cmake --build build --target AstroFind -j$(nproc)
```
Compiles the app on all CPU cores. Leave out `--target AstroFind` to also build the tests.

```bash
./build/bin/AstroFind
```
Starts the program. Debug messages (spdlog) go to this terminal; there is no log file.

Required: Qt ≥ 6.4 (a C++23 compiler), cfitsio, Eigen3, fftw3. CMake detects these optional
dependencies:

| Optional dependency | CMake message / flag | Without it |
|---|---|---|
| LibRaw | `Found LibRaw … — DSLR RAW loading enabled` → `ASTROFIND_HAS_LIBRAW` | RAW files are refused with a clear message |
| libarchive | → `ASTROFIND_HAS_LIBARCHIVE` | TAR.GZ/BZ2/XZ, 7Z, RAR cannot be opened |
| Qt6Keychain (+ libsecret) | `Qt6Keychain found — secure API key storage enabled` → `ASTROFIND_HAS_KEYCHAIN` | API key stored in **plain text** in the settings file |
| Qt6 Core5Compat | found → fetches QuaZip | (ZIP files are extracted by the system `unzip` command in either case) |
| Qt6 LinguistTools | `Qt6 LinguistTools found — translations will be compiled` | `.qm` translations are not rebuilt |

Build option: `-DASTROFIND_ASAN=ON` adds AddressSanitizer + UndefinedBehaviorSanitizer (use a
separate build folder with `-DCMAKE_BUILD_TYPE=Debug`).

🇧🇷 **Português**
*Resumindo: você baixa o código e deixa o CMake compilar. As bibliotecas opcionais são detectadas
sozinhas. Se faltar uma, você só perde aquele recurso.*

Instale as dependências de build da sua distro pelo
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md) (a fonte única dos nomes
de pacotes). Depois:

```bash
git clone https://github.com/petrinhu/astrofind.git && cd astrofind
```
Baixa o código-fonte e entra na pasta.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
Configura o build em `build/`. A saída lista quais recursos opcionais foram encontrados.

```bash
cmake --build build --target AstroFind -j$(nproc)
```
Compila o programa usando todos os núcleos. Tire o `--target AstroFind` para compilar também os
testes.

```bash
./build/bin/AstroFind
```
Abre o programa. As mensagens de depuração (spdlog) saem neste terminal; não há arquivo de log.

Obrigatórios: Qt ≥ 6.4 (compilador C++23), cfitsio, Eigen3, fftw3. O CMake detecta estas
dependências opcionais:

| Dependência opcional | Mensagem / flag do CMake | Sem ela |
|---|---|---|
| LibRaw | `Found LibRaw … — DSLR RAW loading enabled` → `ASTROFIND_HAS_LIBRAW` | Arquivos RAW são recusados com mensagem clara |
| libarchive | → `ASTROFIND_HAS_LIBARCHIVE` | TAR.GZ/BZ2/XZ, 7Z, RAR não abrem |
| Qt6Keychain (+ libsecret) | `Qt6Keychain found — secure API key storage enabled` → `ASTROFIND_HAS_KEYCHAIN` | Chave de API guardada em **texto puro** no arquivo de configurações |
| Qt6 Core5Compat | encontrado → baixa o QuaZip | (arquivos ZIP são extraídos pelo comando `unzip` do sistema em qualquer caso) |
| Qt6 LinguistTools | `Qt6 LinguistTools found — translations will be compiled` | As traduções `.qm` não são recompiladas |

Opção de build: `-DASTROFIND_ASAN=ON` liga AddressSanitizer + UndefinedBehaviorSanitizer (use uma
pasta de build separada com `-DCMAKE_BUILD_TYPE=Debug`).

### 4.2 Test suites / Suítes de teste

🇬🇧 **English**
*In short: two test programs check the maths and the windows automatically. The UI tests can
run without a screen.*

```bash
cmake --build build -j$(nproc)
```
Builds the app and both test programs.

```bash
cd build && ctest --output-on-failure
```
Runs both suites and prints details only for failures.

```bash
build/bin/astrofind_tests
```
Runs the **179** core test cases (Catch2) directly.

```bash
QT_QPA_PLATFORM=offscreen build/bin/astrofind_ui_tests
```
Runs the **27** UI test cases without a display.

🇧🇷 **Português**
*Resumindo: dois programas de teste conferem a matemática e as janelas automaticamente. Os
testes de interface rodam sem tela.*

```bash
cmake --build build -j$(nproc)
```
Compila o programa e os dois executáveis de teste.

```bash
cd build && ctest --output-on-failure
```
Roda as duas suítes e mostra detalhes só das falhas.

```bash
build/bin/astrofind_tests
```
Roda diretamente os **179** casos de teste do núcleo (Catch2).

```bash
QT_QPA_PLATFORM=offscreen build/bin/astrofind_ui_tests
```
Roda os **27** casos de teste de interface sem monitor.

### 4.3 Audit target / Alvo de auditoria

🇬🇧 **English**
*In short: one command runs four bug-hunting tools and writes their reports in a folder.*

```bash
cmake --build build --target audit
```
Runs all four tools. The reports go to `build/audit/`:

| Report | Tool | What it finds | Sub-target |
|---|---|---|---|
| `cppcheck.xml` | cppcheck `--enable=all` (XML v2) | Static issues in `src/core` and `src/ui` | `audit-cppcheck` |
| `clang_tidy.txt` (+ `clang_tidy_fixes.yaml` with run-clang-tidy) | clang-tidy: bugprone, cert, cppcoreguidelines, misc, performance, narrowing | Risky C++ patterns | `audit-clang-tidy` |
| `valgrind.xml` | Valgrind memcheck on `astrofind_tests` | Leaks and invalid memory accesses | `audit-valgrind` |
| `asan_tests.txt` | Configures `build_asan/` with `ASTROFIND_ASAN=ON`, builds and runs `astrofind_tests` | Memory errors and undefined behaviour at run time | `audit-asan` |

- A missing tool is skipped, with a hint of the package to install.
- In CI the same audit runs on Fedora 44, CachyOS, Arch, Ubuntu 24.04 and Debian 12, for pull
  requests into `main` and on release tags.

🇧🇷 **Português**
*Resumindo: um comando roda quatro ferramentas caçadoras de bugs e grava os relatórios numa pasta.*

```bash
cmake --build build --target audit
```
Roda as quatro ferramentas. Os relatórios vão para `build/audit/`:

| Relatório | Ferramenta | O que encontra | Subalvo |
|---|---|---|---|
| `cppcheck.xml` | cppcheck `--enable=all` (XML v2) | Problemas estáticos em `src/core` e `src/ui` | `audit-cppcheck` |
| `clang_tidy.txt` (+ `clang_tidy_fixes.yaml` com run-clang-tidy) | clang-tidy: bugprone, cert, cppcoreguidelines, misc, performance, narrowing | Padrões arriscados de C++ | `audit-clang-tidy` |
| `valgrind.xml` | Valgrind memcheck sobre `astrofind_tests` | Vazamentos e acessos inválidos à memória | `audit-valgrind` |
| `asan_tests.txt` | Configura `build_asan/` com `ASTROFIND_ASAN=ON`, compila e roda `astrofind_tests` | Erros de memória e comportamento indefinido em execução | `audit-asan` |

- Uma ferramenta ausente é pulada, com a dica do pacote a instalar.
- Na CI, a mesma auditoria roda em Fedora 44, CachyOS, Arch, Ubuntu 24.04 e Debian 12, em pull
  requests para `main` e em tags de release.

### 4.4 Where things live / Onde as coisas ficam

🇬🇧 **English**
*In short: settings live in one text file in your home folder. Downloads and caches live in
another folder. Projects and reports go where you choose.*

| What | Where (Linux) |
|---|---|
| Settings (QSettings, org/app "AstroFind") | `~/.config/AstroFind/AstroFind.conf` (INI text; keys like `observer/mpcCode`, `detection/modTolerance`) |
| API key | System keychain, service "AstroFind" (KWallet / Secret Service); **or** `astrometry/apiKey` in the settings file when built without Qt6Keychain |
| MPCORB.DAT, extra translations | `~/.local/share/AstroFind/AstroFind/` (AppDataLocation) |
| Catalog cache `catalog_cache.db` (SQLite) | AppLocalDataLocation (the same folder on Linux) |
| Projects `.gus` (JSON, no pixel data) | Chosen in the dialog; default `paths/projectsDir` = `~/projects` |
| ADES auto-save `ades_report.xml/.psv` | **Settings → Connections → Report output folder:** (`report/outputDir`) |
| "Save to Reports Folder" | `paths/reportDir` (default home; a different key, not editable in the UI) |
| Log | Only the in-app Log panel, plus the terminal output; **File → View Log File** just shows the panel |

> 💡 **Tip.** **File → Exportar Configuração da Escola...** writes *all* settings to an `.ini`
> file. This is handy for backups, or for giving a class the same setup. Close AstroFind before
> you edit `AstroFind.conf` by hand.

🇧🇷 **Português**
*Resumindo: as configurações ficam num arquivo de texto na sua pasta pessoal. Downloads e cache
ficam em outra pasta. Projetos e relatórios vão para onde você escolher.*

| O quê | Onde (Linux) |
|---|---|
| Configurações (QSettings, org/app "AstroFind") | `~/.config/AstroFind/AstroFind.conf` (texto INI; chaves como `observer/mpcCode`, `detection/modTolerance`) |
| Chave de API | Chaveiro do sistema, serviço "AstroFind" (KWallet / Secret Service); **ou** `astrometry/apiKey` no arquivo de configurações quando compilado sem Qt6Keychain |
| MPCORB.DAT, traduções extras | `~/.local/share/AstroFind/AstroFind/` (AppDataLocation) |
| Cache de catálogo `catalog_cache.db` (SQLite) | AppLocalDataLocation (a mesma pasta no Linux) |
| Projetos `.gus` (JSON, sem pixels) | Escolhidos no diálogo; padrão `paths/projectsDir` = `~/projects` |
| Salvamento automático ADES `ades_report.xml/.psv` | **Configurações → Conexões → Pasta de saída de relatórios:** (`report/outputDir`) |
| "Save to Reports Folder" | `paths/reportDir` (padrão: pasta pessoal; outra chave, não editável na interface) |
| Log | Só o painel Registro do programa, mais a saída do terminal; **Arquivo → Ver Arquivo de Log** só mostra o painel |

> 💡 **Dica.** **Arquivo → Exportar Configuração da Escola...** grava *todas* as configurações num
> `.ini`. Serve para backup, ou para dar a mesma configuração a uma turma. Feche o AstroFind antes
> de editar o `AstroFind.conf` à mão.

### 4.5 Translations / Traduções

🇬🇧 **English**
*In short: all interface text is in two translation files. After you change the text in the
code, you refresh those files and compile them.*

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```
Scans the code for `tr()` strings and updates the `.ts` files (XML).

```bash
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```
Compiles them into the `.qm` files the app loads.

- Fill in the `<translation>` tags in `astrofind_pt_BR.ts` before you run `lrelease`.
- With Qt6 LinguistTools installed, the CMake build also compiles the `.qm` files.
- At run time, `astrofind_<lang>.qm` is searched in `<appdir>/translations`, then
  `<AppData>/translations`, then the copy embedded in the program.

🇧🇷 **Português**
*Resumindo: todo texto da interface está em dois arquivos de tradução. Depois de mudar o texto no
código, você atualiza esses arquivos e os compila.*

```bash
lupdate-qt6 src/ -ts i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```
Varre o código atrás de strings `tr()` e atualiza os arquivos `.ts` (XML).

```bash
lrelease-qt6 i18n/astrofind_pt_BR.ts i18n/astrofind_en.ts
```
Compila-os nos arquivos `.qm` que o programa carrega.

- Preencha as tags `<translation>` do `astrofind_pt_BR.ts` antes de rodar o `lrelease`.
- Com o Qt6 LinguistTools instalado, o build do CMake também compila os `.qm`.
- Em execução, o `astrofind_<lang>.qm` é procurado em `<appdir>/translations`, depois em
  `<AppData>/translations`, depois na cópia embutida no programa.

---

## 5. Security and robustness / Segurança e robustez

🔴 Advanced / Avançado

🇬🇧 **English**
*In short: a damaged or malicious file should be refused, not crash the program. Your
astrometry.net key is kept in the system keychain when possible.*

- **Hostile files are validated before memory is allocated.**
  - Every image loader (FITS, SER, XISF, PDS3/PDS4, RAW…) checks the declared size against a
    shared ceiling: 20000 px per axis, 200 million pixels, 100000 cube planes.
  - It also checks the size against the real file size: a header cannot claim more data than
    the file holds.
  - For PDS, `offset + lines·(prefix + samples·bytes + suffix)` must fit in the file.
- **Archives**: symlinks, devices and FIFOs are rejected. Only image extensions are extracted,
  into a unique temporary folder, and directory trees are flattened.
- **Network**: the astrometry.net server URL and the MPC submit URL accept only `https`, or
  `http` to localhost.
- **API key**: kept in the system keychain (Qt6Keychain) when available. Otherwise it is kept in
  plain text, and Settings shows the badge "⚠ Stored in plain text — install qtkeychain-qt6-devel + libsecret-devel for secure storage".
- Hardened linker flags are used for the executable.
- Not every audit finding is fixed yet. Read before you rely on a specific point:
  - [SECURITY.md](https://github.com/petrinhu/astrofind/blob/main/SECURITY.md) — how to report a
    vulnerability **privately** (do not open a public issue).
  - [AUDIT_FIND.md](https://github.com/petrinhu/astrofind/blob/main/AUDIT_FIND.md) — every audit
    finding and its status.
  - [TODO.md](https://github.com/petrinhu/astrofind/blob/main/TODO.md) — the open items (⏳/🟡)
    and their priority.

🇧🇷 **Português**
*Resumindo: um arquivo danificado ou malicioso deve ser recusado, e não derrubar o programa. Sua
chave do astrometry.net fica no chaveiro do sistema quando possível.*

- **Arquivos hostis são validados antes de alocar memória.**
  - Todo carregador de imagem (FITS, SER, XISF, PDS3/PDS4, RAW…) confere o tamanho declarado
    contra um teto comum: 20000 px por eixo, 200 milhões de pixels, 100000 planos de cubo.
  - Também confere o tamanho contra o tamanho real do arquivo: um cabeçalho não pode declarar
    mais dados do que o arquivo contém.
  - No PDS, `offset + lines·(prefix + samples·bytes + suffix)` precisa caber no arquivo.
- **Arquivos compactados**: links simbólicos, dispositivos e FIFOs são rejeitados. Só extensões
  de imagem são extraídas, numa pasta temporária única, e as árvores de diretório são achatadas.
- **Rede**: a URL do servidor astrometry.net e a URL de envio ao MPC só aceitam `https`, ou
  `http` para localhost.
- **Chave de API**: guardada no chaveiro do sistema (Qt6Keychain) quando disponível. Senão fica
  em texto puro, e as Configurações mostram o selo "⚠ Stored in plain text — install qtkeychain-qt6-devel + libsecret-devel for secure storage".
- O executável usa flags de link endurecidas.
- Nem todo achado de auditoria está corrigido. Leia antes de depender de um ponto específico:
  - [SECURITY.md](https://github.com/petrinhu/astrofind/blob/main/SECURITY.md) — como relatar uma
    vulnerabilidade **em privado** (não abra issue pública).
  - [AUDIT_FIND.md](https://github.com/petrinhu/astrofind/blob/main/AUDIT_FIND.md) — todos os
    achados da auditoria e seu status.
  - [TODO.md](https://github.com/petrinhu/astrofind/blob/main/TODO.md) — os itens abertos (⏳/🟡)
    e sua prioridade.

---

## 6. Contributing and real-data reports / Contribuindo e relatos com dados reais

🟡 Intermediate / Intermediário · 🔴 Advanced / Avançado

🇬🇧 **English**
*In short: code, translations and bug reports are welcome. Reports from real observing nights
are the most valuable help right now.*

- Code and translations: see
  [CONTRIBUTING.md](https://github.com/petrinhu/astrofind/blob/main/CONTRIBUTING.md). It covers
  code style, adding files and `tr()` strings, the `pre-commit` hook, tests, and updating
  `CHANGELOG.md` under `[Unreleased]`.
- **Real-data validation results are very welcome.** Open an issue at
  <https://github.com/petrinhu/astrofind/issues> and include:
  1. AstroFind version, distro, and how you installed it (package or source; optional libraries).
  2. Telescope, camera or DSLR model, pixel scale, MPC station code.
  3. The header lines that matter: `DATE-OBS`, `TIMESYS`, `EXPTIME`, `JD` (if present), site
     keywords, and whether a WCS was already present.
  4. Which solver (astrometry.net / ASTAP) and which catalog you used, and the `WCS RMS` lines
     from the log.
  5. Your AstroFind positions of **known** objects, compared with an ephemeris or with another
     program (Astrometrica, Find_Orb…). Give the differences in RA/Dec (″) and in time.
  6. If you submitted: the MPC's response or residuals.
  7. The ADES PSV, if you are comfortable sharing it. Remove contact details you do not want
     public.
- Security problems go **by email** as described in SECURITY.md, never in a public issue.

🇧🇷 **Português**
*Resumindo: código, traduções e relatos de bugs são bem-vindos. Relatos de noites reais de
observação são a ajuda mais valiosa agora.*

- Código e traduções: veja o
  [CONTRIBUTING.md](https://github.com/petrinhu/astrofind/blob/main/CONTRIBUTING.md). Ele cobre
  estilo de código, como adicionar arquivos e strings `tr()`, o hook `pre-commit`, os testes e a
  atualização do `CHANGELOG.md` em `[Unreleased]`.
- **Resultados de validação com dados reais são muito bem-vindos.** Abra uma issue em
  <https://github.com/petrinhu/astrofind/issues> e inclua:
  1. Versão do AstroFind, distro e forma de instalação (pacote ou código; bibliotecas opcionais).
  2. Telescópio, câmera ou modelo da DSLR, escala de pixel, código de estação MPC.
  3. As linhas do cabeçalho que importam: `DATE-OBS`, `TIMESYS`, `EXPTIME`, `JD` (se houver),
     palavras-chave de local, e se já havia WCS.
  4. Qual resolvedor (astrometry.net / ASTAP) e qual catálogo você usou, e as linhas `WCS RMS` do
     log.
  5. Suas posições no AstroFind de objetos **conhecidos**, comparadas com uma efeméride ou com
     outro programa (Astrometrica, Find_Orb…). Dê as diferenças em RA/Dec (″) e em tempo.
  6. Se você enviou: a resposta ou os resíduos do MPC.
  7. O PSV do ADES, se você se sentir à vontade para compartilhar. Tire os dados de contato que
     não quer públicos.
- Problemas de segurança vão **por e-mail**, como descrito no SECURITY.md, nunca em issue pública.

---

See also / Veja também: [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) · [Settings](https://github.com/petrinhu/astrofind/wiki/Settings) · [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats) · [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) ·
[FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ) · [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)
