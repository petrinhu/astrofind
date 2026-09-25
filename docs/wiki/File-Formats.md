# File Formats / Formatos de Arquivo

🇬🇧 **Who this page is for.** Anyone who wants to know *which files AstroFind can open, what it
reads from them, and which format to choose*. Beginners should read the summary table and
"Which format should I use?"; DSLR and NASA PDS users have their own step-by-step sections. For
the full workflow see [Manual](https://github.com/petrinhu/astrofind/wiki/Manual); for unknown words see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Para quem é esta página.** Para quem quer saber *quais arquivos o AstroFind abre, o que ele
lê de cada um e qual formato escolher*. Iniciantes devem ler a tabela-resumo e "Qual formato
devo usar?"; quem usa DSLR ou dados NASA PDS tem seções passo a passo próprias. Para o fluxo
completo veja o [Manual](https://github.com/petrinhu/astrofind/wiki/Manual); para palavras desconhecidas, o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

**Contents / Conteúdo**

1. [Summary table / Tabela-resumo](#summary-table--tabela-resumo)
2. [Which format should I use? / Qual formato devo usar?](#which-format-should-i-use--qual-formato-devo-usar)
3. [How to open files / Como abrir arquivos](#how-to-open-files--como-abrir-arquivos)
4. [FITS](#fits--fits)
5. [SER](#ser-video--vídeo-ser)
6. [XISF](#xisf-pixinsight--xisf-pixinsight)
7. [TIFF / PNG / BMP / JPEG](#tiff--png--bmp--jpeg--tiff--png--bmp--jpeg)
8. [DSLR RAW](#dslr-raw-files--arquivos-raw-de-dslr)
9. [NASA PDS3 / PDS4](#nasa-pds3--pds4--nasa-pds3--pds4)
10. [Archives / Arquivos compactados](#archives-zip-tar-7z-rar--arquivos-compactados-zip-tar-7z-rar)
11. [Tables / Tabelas](#tables-catalogs-and-external-detections--tabelas-catálogos-e-detecções-externas)
12. [Projects (.gus) / Projetos (.gus)](#projects-gus--projetos-gus)
13. [Export formats / Formatos de exportação](#export-formats--formatos-de-exportação)

---

## Summary table / Tabela-resumo

🟢 **Beginner / Iniciante**

🇬🇧 **English** / 🇧🇷 **Português** — the table is bilingual: EN / PT in each cell.

| Format / Formato | Extensions / Extensões | Colour? / Cor? | Metadata read / Metadados lidos | Notes / Observações |
|---|---|---|---|---|
| **FITS** (2-D) | `.fits` `.fit` `.fts` | No / Não | Time (`DATE-OBS`, `JD`, `TIMESYS`), `EXPTIME`, WCS, site, MPC code, filter, pixel scale, saturation, binning, object, telescope / Horário, exposição, WCS, local, código MPC, filtro, escala, saturação, binning, objeto, telescópio | Best format. Compressed FITS via cfitsio if the name ends in `.fits`. / Melhor formato. FITS comprimido via cfitsio se o nome terminar em `.fits`. |
| **FITS colour** / **FITS colorido** | same / idem | Yes (`NAXIS3 = 3`) / Sim | as FITS / como FITS | Detection uses a luminance mix. / A detecção usa uma mistura de luminância. |
| **FITS multi-extension** / **multi-extensão** | same / idem | Yes if exactly 3 same-size image HDUs / Sim se houver exatamente 3 HDUs de imagem do mesmo tamanho | as FITS / como FITS | Otherwise an HDU navigation bar appears. / Senão aparece uma barra de navegação de HDU. |
| **FITS cube** / **cubo FITS** | same / idem | No / Não | as FITS / como FITS | `NAXIS3 > 3`: first plane shown; right-click → **Animate Cube**. / Primeiro plano exibido; botão direito → **Animar Cubo**. |
| **FITS 1-D spectrum** / **espectro 1-D** | same / idem | — | — | Opens a Spectrum plot, not an image. / Abre um gráfico de espectro, não uma imagem. |
| **SER** video / vídeo | `.ser` | Mono, RGB or Bayer / Mono, RGB ou Bayer | Observer, telescope / Observador, telescópio | **Only frame 1** is loaded; **no time** is read. / **Só o quadro 1**; **sem horário**. |
| **XISF** (PixInsight) | `.xisf` | 1 or 3 channels / 1 ou 3 canais | `OBJECT`, `EXPTIME`, `GAIN`, `FILTER`, `TELESCOP`, `OBSERVER`, `JD`, `RA`, `DEC`, `DATE-OBS` | No WCS read → plate-solve. Uncompressed only. / WCS não lido → faça plate solving. Só sem compressão. |
| **TIFF / PNG / BMP / JPEG** | `.tif` `.tiff` `.png` `.bmp` `.jpg` `.jpeg` | Yes if the file is colour / Sim se o arquivo for colorido | **None** / **Nenhum** | 16-bit grayscale kept; colour is reduced to 8 bits per channel. Time must be typed in. / Cinza 16 bits preservado; cor vira 8 bits por canal. Horário precisa ser digitado. |
| **DSLR RAW** (needs LibRaw / precisa da LibRaw) | `.cr2` `.cr3` `.crw` `.nef` `.nrw` `.arw` `.srf` `.sr2` `.orf` `.rw2` `.raf` `.pef` `.dng` `.srw` `.3fr` `.erf` `.kdc` `.mrw` `.x3f` `.iiq` `.mef` `.mos` `.rwl` | Yes (display) / Sim (exibição) | EXIF exposure, camera make/model, saturation, camera-clock time (**flagged ambiguous**) / Exposição EXIF, marca/modelo, saturação, horário do relógio da câmera (**marcado ambíguo**) | Linear data; no WCS, no site → set location and plate-solve. / Dados lineares; sem WCS nem local → configure o local e faça plate solving. |
| **NASA PDS3** | `.img` (+ `.lbl`) | No / Não | `START_TIME`/`STOP_TIME`, `EXPOSURE_DURATION`, `TARGET_NAME`, instrument, filter, RA/Dec / instrumento, filtro, AR/Dec | First band only; missing values → NaN (magenta). Plate-solve first. / Só a primeira banda; valores ausentes → NaN (magenta). Faça plate solving antes. |
| **NASA PDS4** | `.xml` (label / rótulo) | No / Não | `start_date_time`/`stop_date_time`, `exposure_duration`, target, observing system / alvo, sistema de observação | A label pointing to FITS opens the FITS. / Um rótulo que aponta para FITS abre o FITS. |
| **Archives** / **Compactados** | `.zip` `.tar.gz` `.tgz` `.tar.bz2` `.tbz2` `.tar.xz` `.txz` `.7z` `.rar` | — | — | ZIP uses the system `unzip`; the others need libarchive. / ZIP usa o `unzip` do sistema; os outros precisam da libarchive. |
| **Project** / **Projeto** | `.gus` | — | Session state (no pixels) / Estado da sessão (sem pixels) | **File → Open Project...** / **Arquivo → Abrir Projeto...** |

---

## Which format should I use? / Qual formato devo usar?

🟢 **Beginner / Iniciante**

🇬🇧 **English**

1. **FITS, whenever you can.** It is the only format that carries everything an MPC report
   needs: the exact time with its time system, the exposure, and often the site and a plate
   solution. Most astronomy capture programs can save FITS.
2. **DSLR owners:** open your **RAW** files directly — do *not* convert to JPEG or TIFF first.
   RAW keeps the linear sensor data. Just set the camera clock to UTC (see below).
3. **XISF or SER** are fine if that is what your software writes, but you will have to check or
   type the time (SER has none) and plate-solve.
4. **TIFF/PNG/BMP/JPEG** only as a last resort, for practice: they carry no time and no position.
   JPEG is lossy (it changes pixel values) and 8-bit; avoid it for measurement.
5. **Calibration frames (dark, flat) must be FITS**, whatever format your images are in.

🇧🇷 **Português**

1. **FITS, sempre que puder.** É o único formato que leva tudo o que um relatório ao MPC precisa:
   o horário exato com seu sistema de tempo, a exposição e muitas vezes o local e uma solução de
   placa. A maioria dos programas de captura astronômica salva em FITS.
2. **Quem tem DSLR:** abra os arquivos **RAW** direto — *não* converta para JPEG ou TIFF antes. O
   RAW mantém os dados lineares do sensor. Só acerte o relógio da câmera em UTC (veja abaixo).
3. **XISF ou SER** servem se é isso que o seu programa grava, mas você terá de conferir ou
   digitar o horário (o SER não tem) e fazer plate solving.
4. **TIFF/PNG/BMP/JPEG** só em último caso, para praticar: não levam horário nem posição. O JPEG
   perde informação (muda os valores dos pixels) e tem 8 bits; evite-o para medir.
5. **Frames de calibração (dark, flat) precisam ser FITS**, seja qual for o formato das imagens.

---

## How to open files / Como abrir arquivos

🟢 **Beginner / Iniciante**

🇬🇧 **English**

1. **File → Import Images...** / **Arquivo → Importar Imagens...**. The dialog "Load Astronomical
   Images" has filters: *Astronomical Images* (everything), *FITS*, *SER video*, *XISF —
   PixInsight*, *NASA PDS3 / PDS4*, *DSLR RAW*, *TIFF / PNG / BMP / JPEG*, *ZIP archives*,
   *Compressed archives*, *All files*.
2. Select several files at once with `Ctrl`+click or `Shift`+click.

- **Drag and drop** accepts only `.fits`, `.fit`, `.fts`, `.zip` and the libarchive formats.
  Other files dropped on the window are ignored: use the menu.
- **File → Open Recent...** reopens a folder and loads FITS, SER, XISF, TIFF, PNG, PDS3
  (`.img`/`.lbl`) and the common RAW types in it — not archives, BMP, JPEG or PDS4 `.xml`.
- **Image Tools → View FITS Header...** shows the FITS header; for files that are not FITS it does
  nothing.
- An unknown extension gives: "Unsupported image format '.xyz' — supported: fits, ser, xisf,
  tiff, tif, png, bmp, jpg, PDS3 (img/lbl), PDS4 (xml), DSLR RAW".

🇧🇷 **Português**

1. **Arquivo → Importar Imagens...** / **File → Import Images...**. A janela "Carregar Imagens
   Astronômicas" tem filtros (em inglês): *Astronomical Images* (tudo), *FITS*, *SER video*,
   *XISF — PixInsight*, *NASA PDS3 / PDS4*, *DSLR RAW*, *TIFF / PNG / BMP / JPEG*, *ZIP
   archives*, *Compressed archives*, *All files*.
2. Selecione vários arquivos com `Ctrl`+clique ou `Shift`+clique.

- **Arrastar e soltar** aceita só `.fits`, `.fit`, `.fts`, `.zip` e os formatos da libarchive.
  Outros arquivos soltos na janela são ignorados: use o menu.
- **Arquivo → Abrir Recente...** reabre uma pasta e carrega FITS, SER, XISF, TIFF, PNG, PDS3
  (`.img`/`.lbl`) e os tipos RAW comuns — não compactados, BMP, JPEG nem `.xml` PDS4.
- **Ferramentas de Imagem → Ver Cabeçalho FITS...** mostra o cabeçalho FITS; para arquivos que não são FITS não
  faz nada.
- Uma extensão desconhecida dá: "Unsupported image format '.xyz' — supported: fits, ser, xisf,
  tiff, tif, png, bmp, jpg, PDS3 (img/lbl), PDS4 (xml), DSLR RAW".

---

## FITS / FITS

🟢 **Beginner / Iniciante** · 🟡 **Intermediate / Intermediário** (variants / variantes)

🇬🇧 **English**

**FITS** (Flexible Image Transport System, see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)) is the standard astronomy format:
a text **header** of keywords followed by the pixel data. One file can hold several
**HDUs** (header/data units). AstroFind reads the file with the cfitsio library.

**Keywords AstroFind uses:**

| Purpose | Keywords |
|---|---|
| Time | `DATE-OBS`, `TIMESYS`, `JD` (if present it wins), `EXPTIME` |
| Plate solution (WCS) | `CTYPE1`, `CRVAL1/2`, `CRPIX1/2`, `CD1_1…CD2_2` or `CDELT1/2` + `CROTA1/2` |
| Pointing | `RA`, `DEC`, `OBJCTRA`, `OBJCTDEC` |
| Site | `SITELAT`/`SITELONG`/`SITEELEV`, `LAT-OBS`/`LONG-OBS`/`ALT-OBS`, `LATITUDE`/`LONGITUD`/`ALTITUDE`, `OBSGEO-B/L/H` |
| Station | `MPCCODE`, `OBSCODE`, `OBSERVAT`; also `TELESCOP`/`ORIGIN` for known telescopes and space telescopes |
| Camera | `PIXSCALE`, `XBINNING`/`YBINNING`, `GAIN`, `SATURATE`/`SATLEVEL`/`DATAMAX` |
| Other | `OBJECT`, `FILTER`, `OBSERVER` |

Projections read from `CTYPE1`: TAN (default), SIN, ARC, STG, CAR, MER, GLS/SFL, AIT.

**Variants:**

- **Compressed FITS.** cfitsio reads fpack tile-compressed and gzip-compressed FITS. But
  AstroFind chooses the reader by the *last* extension, so `image.fits.fz` or `image.fits.gz` is
  refused. Decompress it (`funpack`, `gunzip`) or rename it so it ends in `.fits`.
- **Colour (`NAXIS3 = 3`).** Three planes are read as R, G, B. The "Imagens carregadas" notice
  says "🎨 Coloridas (NAXIS3 = 3)". Stars are detected on a luminance mix. ⚠️ A 3-plane file is
  *always* treated as colour, even if the planes are really three time frames.
- **Multi-extension colour.** A file with **exactly three** 2-D image HDUs of the same size is
  combined into colour. The order comes from `EXTNAME` or `FILTER` (R/RED, G/GREEN/V, B/BLUE),
  otherwise it is first = R, second = G, third = B.
- **Multi-HDU navigation.** Any other file with several image HDUs shows a bar at the top of the
  image window: `‹`, a list "n: NAME (W×H)", `›`. The info line starts with `HDU n/N |`.
- **Cubes (`NAXIS3 > 3`)** — e.g. a time series. The first plane is shown. Right-click →
  **Animate Cube (N frames)…** / **Animar Cubo (N quadros)…** plays it in a blink-style window
  "Cube: …". Up to 100 000 planes.
- **1-D spectra (`NAXIS = 1`).** When a FITS is not an image but a spectrum, a **Spectrum**
  window opens automatically (axes "Channel" and "Flux").
- **NaN pixels** (undefined values) are allowed and drawn in magenta.

🇧🇷 **Português**

**FITS** (Flexible Image Transport System, veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)) é o formato padrão da
astronomia: um **cabeçalho** de texto com palavras-chave seguido dos pixels. Um arquivo pode ter
várias **HDUs** (unidades cabeçalho/dados). O AstroFind lê o arquivo com a biblioteca cfitsio.

**Palavras-chave que o AstroFind usa:**

| Para quê | Palavras-chave |
|---|---|
| Horário | `DATE-OBS`, `TIMESYS`, `JD` (se existir, vale ele), `EXPTIME` |
| Solução de placa (WCS) | `CTYPE1`, `CRVAL1/2`, `CRPIX1/2`, `CD1_1…CD2_2` ou `CDELT1/2` + `CROTA1/2` |
| Apontamento | `RA`, `DEC`, `OBJCTRA`, `OBJCTDEC` |
| Local | `SITELAT`/`SITELONG`/`SITEELEV`, `LAT-OBS`/`LONG-OBS`/`ALT-OBS`, `LATITUDE`/`LONGITUD`/`ALTITUDE`, `OBSGEO-B/L/H` |
| Estação | `MPCCODE`, `OBSCODE`, `OBSERVAT`; também `TELESCOP`/`ORIGIN` para telescópios conhecidos e espaciais |
| Câmera | `PIXSCALE`, `XBINNING`/`YBINNING`, `GAIN`, `SATURATE`/`SATLEVEL`/`DATAMAX` |
| Outros | `OBJECT`, `FILTER`, `OBSERVER` |

Projeções lidas de `CTYPE1`: TAN (padrão), SIN, ARC, STG, CAR, MER, GLS/SFL, AIT.

**Variantes:**

- **FITS comprimido.** A cfitsio lê FITS comprimido em blocos (fpack) e com gzip. Mas o
  AstroFind escolhe o leitor pela *última* extensão, então `imagem.fits.fz` ou `imagem.fits.gz`
  é recusado. Descomprima (`funpack`, `gunzip`) ou renomeie para terminar em `.fits`.
- **Colorido (`NAXIS3 = 3`).** Os três planos são lidos como R, G, B. O aviso "Imagens
  carregadas" diz "🎨 Coloridas (NAXIS3 = 3)". As estrelas são detectadas numa mistura de
  luminância. ⚠️ Um arquivo com 3 planos é *sempre* tratado como colorido, mesmo que os planos
  sejam três quadros no tempo.
- **Cor em multi-extensão.** Um arquivo com **exatamente três** HDUs de imagem 2-D do mesmo
  tamanho vira uma imagem colorida. A ordem vem de `EXTNAME` ou `FILTER` (R/RED, G/GREEN/V,
  B/BLUE); senão é primeira = R, segunda = G, terceira = B.
- **Navegação de HDUs.** Qualquer outro arquivo com várias HDUs de imagem mostra uma barra no
  alto da janela da imagem: `‹`, uma lista "n: NOME (L×A)", `›`. A linha de informação começa
  com `HDU n/N |`.
- **Cubos (`NAXIS3 > 3`)** — por exemplo uma série temporal. O primeiro plano é exibido. Botão
  direito → **Animar Cubo (N quadros)…** / **Animate Cube (N frames)…** toca o cubo numa janela
  tipo blink "Cube: …". Até 100 000 planos.
- **Espectros 1-D (`NAXIS = 1`).** Quando um FITS não é imagem, e sim espectro, abre
  automaticamente uma janela **Espectro** (eixos "Canal" e "Fluxo").
- **Pixels NaN** (valores indefinidos) são aceitos e desenhados em magenta.

---

## SER video / Vídeo SER

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

SER is a simple video format from planetary/lucky-imaging cameras. AstroFind reads mono, RGB/BGR
and Bayer (RGGB, GRBG, GBRG, BGGR) SER files with 8 or 16 bits. Bayer frames are shown in colour
(simple 2×2 demosaic) while measurement uses the raw mono values.

- ⚠️ **Only the first frame** is loaded (the log warns "SER file has N frames — only frame 1
  loaded").
- ⚠️ **The time stamp is not read.** Enter the mid-exposure time in **Image Tools → Edit Image
  Settings...** → **Julian Date:**, and the exposure in **Exposure time:**.
- Observer and telescope names are read from the header.

🇧🇷 **Português**

SER é um formato de vídeo simples de câmeras planetárias/"lucky imaging". O AstroFind lê SER mono,
RGB/BGR e Bayer (RGGB, GRBG, GBRG, BGGR) de 8 ou 16 bits. Quadros Bayer são exibidos coloridos
(demosaico simples 2×2) enquanto a medição usa os valores mono brutos.

- ⚠️ **Só o primeiro quadro** é carregado (o registro avisa "SER file has N frames — only frame 1
  loaded").
- ⚠️ **O horário não é lido.** Digite o horário de meio da exposição em **Ferramentas de Imagem → Editar
  Configurações da Imagem...** → **Data Juliana:**, e a exposição em **Tempo de exposição:**.
- Os nomes de observador e telescópio são lidos do cabeçalho.

---

## XISF (PixInsight) / XISF (PixInsight)

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

XISF is PixInsight's format. AstroFind reads the first image, with 1 channel (mono) or 3
channels (colour), samples UInt8/16/32 or Float32/64, stored as attachment or embedded.

- It reads these FITS keywords stored in the XISF: `OBJECT`, `EXPTIME`, `GAIN`, `FILTER`,
  `TELESCOP`, `OBSERVER`, `JD`, `RA`, `DEC`, `DATE-OBS` (`DATE-OBS` is taken as UTC).
- The plate solution is **not** read: Run Data Reduction will plate-solve it.
- Save the XISF **without compression** in PixInsight; compressed data is not decoded.

🇧🇷 **Português**

XISF é o formato do PixInsight. O AstroFind lê a primeira imagem, com 1 canal (mono) ou 3 canais
(cor), amostras UInt8/16/32 ou Float32/64, gravadas como anexo ou embutidas.

- Ele lê estas palavras-chave FITS guardadas no XISF: `OBJECT`, `EXPTIME`, `GAIN`, `FILTER`,
  `TELESCOP`, `OBSERVER`, `JD`, `RA`, `DEC`, `DATE-OBS` (`DATE-OBS` é tomado como UTC).
- A solução de placa **não** é lida: a Redução de Dados fará o plate solving.
- Salve o XISF **sem compressão** no PixInsight; dados comprimidos não são decodificados.

---

## TIFF / PNG / BMP / JPEG / TIFF / PNG / BMP / JPEG

🟢 **Beginner / Iniciante**

🇬🇧 **English**

Ordinary image files, read through Qt.

- **16-bit grayscale** TIFF/PNG keeps its full 0–65535 range — the best case for these formats.
- 8-bit grayscale is read as is.
- **Colour** files are converted to 8 bits per channel (even a 16-bit colour TIFF loses
  precision). Prefer grayscale 16-bit if you must use TIFF/PNG.
- **No metadata is read**: no time, exposure, position or site. Type the time and exposure in
  **Image Tools → Edit Image Settings...** / **Ferramentas de Imagem → Editar Configurações da Imagem...**, set the
  site in Settings, and let Run Data Reduction plate-solve.
- Archives do not extract BMP or JPEG files.

🇧🇷 **Português**

Arquivos de imagem comuns, lidos pelo Qt.

- TIFF/PNG **cinza de 16 bits** mantém a faixa completa 0–65535 — o melhor caso nesses formatos.
- Cinza de 8 bits é lido como está.
- Arquivos **coloridos** são convertidos para 8 bits por canal (até um TIFF colorido de 16 bits
  perde precisão). Prefira cinza 16 bits se precisar usar TIFF/PNG.
- **Nenhum metadado é lido**: sem horário, exposição, posição ou local. Digite horário e
  exposição em **Ferramentas de Imagem → Editar Configurações da Imagem...** / **Image Tools → Edit Image
  Settings...**, configure o local nas Configurações e deixe a Redução de Dados fazer o plate
  solving.
- Arquivos compactados não extraem BMP nem JPEG.

---

## DSLR RAW files / Arquivos RAW de DSLR

🟢 **Beginner / Iniciante** (steps / passos) · 🟡 **Intermediate / Intermediário** (how it works / como funciona)

🇬🇧 **English**

New in 1.1.0. A **RAW** file is the untouched data from a digital camera sensor. AstroFind reads
it with the **LibRaw** library.

**Which cameras / extensions:**

| Brand | Extensions |
|---|---|
| Canon | `.cr2` `.cr3` `.crw` |
| Nikon | `.nef` `.nrw` |
| Sony | `.arw` `.srf` `.sr2` |
| Olympus / OM System | `.orf` |
| Panasonic / Lumix | `.rw2` |
| Fujifilm | `.raf` |
| Pentax | `.pef` (and `.dng`) |
| Adobe DNG (many brands, phones) | `.dng` |
| Samsung | `.srw` |
| Hasselblad | `.3fr` |
| Epson | `.erf` |
| Kodak | `.kdc` |
| Minolta | `.mrw` |
| Sigma (Foveon) | `.x3f` |
| Phase One | `.iiq` |
| Mamiya | `.mef` |
| Leaf | `.mos` |
| Leica | `.rwl` |

**Step by step:**

1. **Before observing, set the camera clock to UTC** (Universal Time, not your local time) and
   set it accurately. The RAW file stores only what the clock showed, with no time zone.
2. Shoot in RAW (not JPEG only). Take several frames of the same field.
3. **File → Import Images...** → filter *DSLR RAW* → select the files.
4. The Log shows "DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera
   grava hora local." for each file. This is expected for RAW: AstroFind *assumed* the clock was
   UTC. If it was not, correct the **Julian Date:** in **Image Tools → Edit Image Settings...**
   before Run Data Reduction (see [Manual](https://github.com/petrinhu/astrofind/wiki/Manual), Part 11).
5. Set your **location** in **File → Settings... → Observer** — RAW files do not contain it.
6. Run **Astrometry Tools → Run Data Reduction...** (`Ctrl+A`) to plate-solve; RAW files have no plate
   solution.

**Why linear and "no demosaic" matter.** A colour sensor has a mosaic of red, green and blue
filters (the Bayer pattern, 2×2: one red, two green, one blue). Normal photo software
"demosaics" (interpolates the missing colours), applies white balance and a brightness curve
(gamma). All of that changes pixel values in ways that are not proportional to the light, which
spoils measurements. AstroFind keeps the data **linear** (no white balance, no gamma, no black
subtraction) — like a FITS from an astronomy camera.

**Superpixel luminance.** For star detection and measurement, each 2×2 Bayer block is combined
into one luminance value, **(R + 2G + B) / 4**. This removes the colour checkerboard that would
confuse the detector. The image keeps its pixel count (each 2×2 block holds the same value), so
the effective resolution is 2 pixels. A simple demosaic is used only to show the image in
colour. Fuji X-Trans, Sigma Foveon and linear DNG files are not plain Bayer: LibRaw converts them
to linear 16-bit instead.

**What is read:** exposure time (EXIF), camera make and model, the sensor maximum (used as the
saturation level) and the capture time. Not read: plate solution, site, object, filter.

**Without LibRaw.** LibRaw is an *optional* build dependency. If your AstroFind was built without
it, RAW files are refused with: "DSLR RAW support is not available in this build of AstroFind
(compiled without LibRaw): <file>". Install the LibRaw development package and rebuild — see
[Installation](https://github.com/petrinhu/astrofind/wiki/Installation).

> ⚠️ **Watch out / Atenção** — Dark and flat frames must be **FITS**. RAW darks/flats cannot be
> loaded as calibration frames; convert them to FITS in another program first. Archives extract
> only `.cr2 .cr3 .nef .arw .dng .raf .orf .rw2 .pef` RAW files.

🇧🇷 **Português**

Novidade da 1.1.0. Um arquivo **RAW** são os dados intactos do sensor de uma câmera digital. O
AstroFind o lê com a biblioteca **LibRaw**.

**Quais câmeras / extensões:** veja a tabela acima (Canon `.cr2` `.cr3` `.crw`, Nikon `.nef`
`.nrw`, Sony `.arw` `.srf` `.sr2`, Olympus/OM System `.orf`, Panasonic `.rw2`, Fujifilm `.raf`,
Pentax `.pef`, Adobe DNG `.dng` (várias marcas e celulares), Samsung `.srw`, Hasselblad `.3fr`,
Epson `.erf`, Kodak `.kdc`, Minolta `.mrw`, Sigma `.x3f`, Phase One `.iiq`, Mamiya `.mef`, Leaf
`.mos`, Leica `.rwl`).

**Passo a passo:**

1. **Antes de observar, acerte o relógio da câmera em UTC** (Tempo Universal, não a hora local)
   e com precisão. O RAW guarda só o que o relógio mostrava, sem fuso horário.
2. Fotografe em RAW (não só em JPEG). Faça vários quadros do mesmo campo.
3. **Arquivo → Importar Imagens...** → filtro *DSLR RAW* → selecione os arquivos.
4. O Registro mostra "DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a
   câmera grava hora local." para cada arquivo. Isso é esperado em RAW: o AstroFind *supôs* que o
   relógio estava em UTC. Se não estava, corrija a **Data Juliana:** em **Ferramentas de Imagem → Editar
   Configurações da Imagem...** antes da Redução de Dados (veja o [Manual](https://github.com/petrinhu/astrofind/wiki/Manual), Parte 11).
5. Configure sua **localização** em **Arquivo → Configurações... → Observador** — arquivos RAW
   não a contêm.
6. Rode **Ferramentas de Astrometria → Executar Redução de Dados...** (`Ctrl+A`) para fazer o plate solving; arquivos RAW
   não têm solução de placa.

**Por que "linear" e "sem demosaico" importam.** Um sensor colorido tem um mosaico de filtros
vermelho, verde e azul (o padrão Bayer, 2×2: um vermelho, dois verdes, um azul). Programas de
foto comuns fazem o "demosaico" (interpolam as cores que faltam), aplicam balanço de branco e uma
curva de brilho (gama). Tudo isso muda os valores dos pixels de forma não proporcional à luz, o
que estraga as medições. O AstroFind mantém os dados **lineares** (sem balanço de branco, sem
gama, sem subtração de preto) — como um FITS de câmera astronômica.

**Luminância por superpixel.** Para detectar e medir estrelas, cada bloco Bayer 2×2 vira um valor
de luminância, **(R + 2G + B) / 4**. Isso tira o "xadrez" de cores que confundiria o detector. A
imagem mantém o número de pixels (cada bloco 2×2 tem o mesmo valor), então a resolução efetiva é
de 2 pixels. Um demosaico simples é usado só para mostrar a imagem colorida. Arquivos Fuji
X-Trans, Sigma Foveon e DNG linear não são Bayer simples: a LibRaw os converte em 16 bits
lineares.

**O que é lido:** tempo de exposição (EXIF), marca e modelo da câmera, o máximo do sensor (usado
como nível de saturação) e o horário da captura. Não é lido: solução de placa, local, objeto,
filtro.

**Sem a LibRaw.** A LibRaw é uma dependência *opcional* de compilação. Se o seu AstroFind foi
compilado sem ela, arquivos RAW são recusados com: "DSLR RAW support is not available in this
build of AstroFind (compiled without LibRaw): <arquivo>". Instale o pacote de desenvolvimento da
LibRaw e recompile — veja [Installation](https://github.com/petrinhu/astrofind/wiki/Installation).

> ⚠️ **Atenção** — Darks e flats precisam ser **FITS**. Darks/flats em RAW não podem ser
> carregados como calibração; converta-os para FITS em outro programa antes. Arquivos
> compactados só extraem RAW `.cr2 .cr3 .nef .arw .dng .raf .orf .rw2 .pef`.

---

## NASA PDS3 / PDS4 / NASA PDS3 / PDS4

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

New in 1.1.0. The **PDS** (Planetary Data System) is NASA's public archive of data from
planetary missions and some telescopes. Its images are raw binary files described by a text
**label**. There are two generations:

- **PDS3**: the label is ODL text. It is either **attached** (at the start of the `.img` file) or
  **detached** (a separate `.lbl` file with the same name next to the `.img`).
- **PDS4**: the label is an **XML** file (`.xml`) that points to the data file.

**How to open:**

1. Put the label and the data file in the **same folder**, with their original names
   (upper/lower case does not matter).
2. **File → Import Images...** → filter *NASA PDS3 / PDS4* and select:
   - PDS3 with attached label: the `.img`.
   - PDS3 with detached label: the `.img` **or** the `.lbl` (if you select both, the extra `.lbl`
     is dropped).
   - PDS4: the `.xml` label. If the label points to a FITS file, that FITS is opened.
3. Run **Astrometry Tools → Run Data Reduction...** before measuring: mission images **rarely have a plate
   solution**.

**What is read:** the first band of the image (other bands are skipped with a log warning);
8/16/32-bit integers and 32/64-bit floating point; scaling factor and offset. The label's
**missing value** (`MISSING_CONSTANT` / `missing_constant`) becomes NaN, drawn in **magenta** —
those pixels are not bad data, they are "no data". Time (start/stop), exposure, target,
instrument, filter and, in PDS3, RA/Dec are read; PDS times are taken as UTC.

**Limits and errors:** "VAX floating point PDS3 images are not supported"; "Unsupported PDS3
SAMPLE_TYPE '…'"; "Not a PDS3 file (no attached or detached label)" (the `.lbl` is missing); a
PDS4 `.xml` must be an observational product (`Product_Observational`) with a 2-D image; PDS4
`.xml` is not extracted from archives and not scanned by **Open Recent...**.

> ⚠️ **Watch out / Atenção** — Images taken from a spacecraft are great for learning. An MPC
> report from a spacecraft also needs the spacecraft's position, which AstroFind does not write.

🇧🇷 **Português**

Novidade da 1.1.0. O **PDS** (Planetary Data System) é o arquivo público da NASA com dados de
missões planetárias e de alguns telescópios. Suas imagens são arquivos binários descritos por um
**rótulo** (label) de texto. Há duas gerações:

- **PDS3**: o rótulo é texto ODL. Ele é **anexado** (no começo do arquivo `.img`) ou **separado**
  (um arquivo `.lbl` com o mesmo nome ao lado do `.img`).
- **PDS4**: o rótulo é um arquivo **XML** (`.xml`) que aponta para o arquivo de dados.

**Como abrir:**

1. Deixe o rótulo e o arquivo de dados na **mesma pasta**, com os nomes originais (maiúsculas ou
   minúsculas não importam).
2. **Arquivo → Importar Imagens...** → filtro *NASA PDS3 / PDS4* e selecione:
   - PDS3 com rótulo anexado: o `.img`.
   - PDS3 com rótulo separado: o `.img` **ou** o `.lbl` (se selecionar os dois, o `.lbl` extra é
     descartado).
   - PDS4: o rótulo `.xml`. Se o rótulo apontar para um arquivo FITS, esse FITS é aberto.
3. Rode **Ferramentas de Astrometria → Executar Redução de Dados...** antes de medir: imagens de missão **raramente têm
   solução de placa**.

**O que é lido:** a primeira banda da imagem (as outras são puladas com aviso no registro);
inteiros de 8/16/32 bits e ponto flutuante de 32/64 bits; fator de escala e deslocamento. O
**valor ausente** do rótulo (`MISSING_CONSTANT` / `missing_constant`) vira NaN, desenhado em
**magenta** — esses pixels não são defeito, são "sem dados". Horário (início/fim), exposição,
alvo, instrumento, filtro e, no PDS3, AR/Dec são lidos; os horários PDS são tomados como UTC.

**Limites e erros:** "VAX floating point PDS3 images are not supported"; "Unsupported PDS3
SAMPLE_TYPE '…'"; "Not a PDS3 file (no attached or detached label)" (falta o `.lbl`); um `.xml`
PDS4 precisa ser um produto observacional (`Product_Observational`) com imagem 2-D; o `.xml`
PDS4 não é extraído de arquivos compactados nem varrido por **Abrir Recente...**.

> ⚠️ **Atenção** — Imagens feitas por uma sonda são ótimas para aprender. Um relatório ao MPC
> feito de uma sonda também precisa da posição da sonda, que o AstroFind não grava.

---

## Archives: ZIP, TAR, 7Z, RAR / Arquivos compactados: ZIP, TAR, 7Z, RAR

🟢 **Beginner / Iniciante**

🇬🇧 **English**

You can load a whole observing night packed in one archive. AstroFind extracts it to a temporary
folder and loads the images inside.

| Archive | Extracted with | If missing |
|---|---|---|
| `.zip` | the system `unzip` command | install `unzip` with your package manager |
| `.tar.gz` `.tgz` `.tar.bz2` `.tbz2` `.tar.xz` `.txz` `.7z` `.rar` | **libarchive** (optional at build time) | "Cannot extract '…': libarchive not available. Install libarchive-devel and recompile." — see [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) |

- Only these files are taken from an archive: `.fits .fit .fts .ser .xisf .tiff .tif .png .img
  .lbl .cr2 .cr3 .nef .arw .dng .raf .orf .rw2 .pef`. Folders inside are flattened; links and
  special files are rejected.
- A project remembers the archive and re-extracts it when you open the project.

🇧🇷 **Português**

Você pode carregar uma noite inteira de observação num único arquivo compactado. O AstroFind o
extrai numa pasta temporária e carrega as imagens de dentro.

| Arquivo | Extraído com | Se faltar |
|---|---|---|
| `.zip` | o comando `unzip` do sistema | instale o `unzip` pelo gerenciador de pacotes |
| `.tar.gz` `.tgz` `.tar.bz2` `.tbz2` `.tar.xz` `.txz` `.7z` `.rar` | **libarchive** (opcional na compilação) | "Cannot extract '…': libarchive not available. Install libarchive-devel and recompile." — veja [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) |

- Só estes arquivos são tirados do compactado: `.fits .fit .fts .ser .xisf .tiff .tif .png .img
  .lbl .cr2 .cr3 .nef .arw .dng .raf .orf .rw2 .pef`. Pastas internas são achatadas; links e
  arquivos especiais são rejeitados.
- Um projeto lembra do arquivo compactado e o extrai de novo quando você abre o projeto.

---

## Tables: catalogs and external detections / Tabelas: catálogos e detecções externas

🔴 **Advanced / Avançado**

🇬🇧 **English**

AstroFind also reads **FITS BINTABLE** (binary table) files — not images, but lists of stars:

- **Local star catalog** (offline reference stars: USNO-B, UCAC, Gaia exports): **Settings →
  Connections → Source: Local FITS BINTABLE** → **Local catalog:**.
- **Astrometry Tools → Import Detected Stars (DAOPHOT/SExtractor)…** (`.fits .fit .fts .cat`) replaces
  the detected stars of the active image.
- **Astrometry Tools → Import Reduction Table (IRAF/Astropy)…** does the same for external reduction
  tables; rows with RA/Dec are marked as matched.

🇧🇷 **Português**

O AstroFind também lê arquivos **FITS BINTABLE** (tabela binária) — não são imagens, e sim listas
de estrelas:

- **Catálogo local de estrelas** (estrelas de referência sem internet: exportações USNO-B, UCAC,
  Gaia): **Configurações → Conexões → Fonte: Local FITS BINTABLE** → **Catálogo local:**.
- **Ferramentas de Astrometria → Importar Estrelas Detectadas (DAOPHOT/SExtractor)…** (`.fits .fit .fts .cat`)
  substitui as estrelas detectadas da imagem ativa.
- **Ferramentas de Astrometria → Importar Tabela de Redução (IRAF/Astropy)…** faz o mesmo com tabelas de redução
  externas; linhas com AR/Dec são marcadas como casadas.

---

## Projects (.gus) / Projetos (.gus)

🟢 **Beginner / Iniciante**

🇬🇧 **English**

A `.gus` file ("AstroFind Project") is a JSON text file with your session: image paths and
metadata, plate solutions, detected and catalog stars, known objects, display range and
observations. It does **not** contain the pixels: keep the image files (or the archive) where
they were. Save with **File → Save Project** (`Ctrl+S`), open with **File → Open Project...**
(`Ctrl+O`). See [Manual](https://github.com/petrinhu/astrofind/wiki/Manual), Part 13.

🇧🇷 **Português**

Um arquivo `.gus` ("AstroFind Project") é um texto JSON com a sua sessão: caminhos e metadados
das imagens, soluções de placa, estrelas detectadas e de catálogo, objetos conhecidos, faixa de
exibição e observações. Ele **não** contém os pixels: mantenha os arquivos de imagem (ou o
compactado) onde estavam. Salve com **Arquivo → Salvar Projeto** (`Ctrl+S`), abra com
**Arquivo → Abrir Projeto...** (`Ctrl+O`). Veja o [Manual](https://github.com/petrinhu/astrofind/wiki/Manual), Parte 13.

---

## Export formats / Formatos de exportação

🟢 **Beginner / Iniciante**

🇬🇧 **English**

| What | Format | Where |
|---|---|---|
| Image as you see it | JPEG, PNG, BMP | **File → Export Image As...** / right-click → **Export as JPEG/PNG…** |
| Image + plate solution | FITS copy `<name>_wcs.fits` | **File → Save FITS Copy...**, **File → Save all FITS** |
| MPC report | ADES 2022 XML and PSV | Report window: **Save…**, **Save to Reports Folder** |
| Printable report | PDF | Report window: **Export PDF…** |
| Light curve | PNG | **Utilities → Light Curve…** → **Export PNG…** |
| School settings | `.ini` | **File → Exportar Configuração da Escola...** |
| Session | `.gus` | **File → Save Project** |

> ⚠️ **Watch out / Atenção** — **Save FITS Copy** copies the *original file* and writes the WCS
> into the copy. The copy keeps the original pixels (calibration is not saved), and it only works
> for images that came from a FITS file: it does not convert RAW, TIFF or other formats to FITS.
> No MPC 80-column report is produced.

🇧🇷 **Português**

| O quê | Formato | Onde |
|---|---|---|
| Imagem como você a vê | JPEG, PNG, BMP | **Arquivo → Exportar Imagem Como...** / botão direito → **Exportar como JPEG/PNG…** |
| Imagem + solução de placa | cópia FITS `<nome>_wcs.fits` | **Arquivo → Salvar Cópia em FITS...**, **Arquivo → Salvar todos como FITS** |
| Relatório MPC | ADES 2022 XML e PSV | Janela do relatório: **Save…**, **Save to Reports Folder** |
| Relatório para imprimir | PDF | Janela do relatório: **Export PDF…** |
| Curva de luz | PNG | **Utilitários → Curva de Luz…** → **Export PNG…** |
| Configurações da escola | `.ini` | **Arquivo → Exportar Configuração da Escola...** |
| Sessão | `.gus` | **Arquivo → Salvar Projeto** |

> ⚠️ **Atenção** — **Salvar Cópia em FITS** copia o *arquivo original* e grava o WCS na cópia. A
> cópia mantém os pixels originais (a calibração não é salva) e só funciona para imagens que
> vieram de um arquivo FITS: ela não converte RAW, TIFF ou outros formatos para FITS. Nenhum
> relatório MPC de 80 colunas é gerado.

---

**See also / Veja também:** [Home](https://github.com/petrinhu/astrofind/wiki/Home) · [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) · [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) · [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) ·
[Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) · [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary) · [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ)
