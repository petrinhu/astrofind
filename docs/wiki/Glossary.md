# Glossary / Glossário

🇬🇧 **English**
**Who this page is for:** anyone who meets a word they don't know in AstroFind or in this wiki.
Terms are in alphabetical order (by the English name). Each entry has a plain explanation and,
where useful, a note on how AstroFind uses it. The dot after each title shows the level:
🟢 Beginner, 🟡 Intermediate, 🔴 Advanced. The full mathematics is in
`docs/technical-reference.md` in the repository.

🇧🇷 **Português**
**Para quem é esta página:** para quem encontra uma palavra desconhecida no AstroFind ou nesta
wiki. Os termos estão em ordem alfabética (pelo nome em inglês). Cada item tem uma explicação
simples e, quando útil, uma nota sobre como o AstroFind usa o termo. A bolinha depois de cada
título mostra o nível: 🟢 Iniciante, 🟡 Intermediário, 🔴 Avançado. A matemática completa está em
`docs/technical-reference.md` no repositório.

**Index / Índice:** [A](#aberration-annual--aberração-anual) · [B](#background-sky--fundo-do-céu) ·
[C](#catalog-star--estrela-de-catálogo) · [D](#dark-current--corrente-de-escuro) ·
[E](#ecliptic--eclíptica) · [F](#fits--flexible-image-transport-system) ·
[G](#gaia-dr3) · [H](#hdu--headerdata-unit) · [I](#iasc--international-astronomical-search-collaboration) ·
[J](#jd--mjd--julian-date--modified-julian-date) · [K](#koo--known-object-overlay--sobreposição-de-objetos-conhecidos) ·
[L](#label-pds--rótulo-pds) · [M](#magnitude--magnitude) · [N](#nan--not-a-number) ·
[O](#observatory-code--código-de-observatório) · [P](#pds--pds3--pds4) ·
[R](#ra--dec--ar--dec) · [S](#saturation--saturação) · [T](#time-zone--fuso-horário) ·
[U](#ucac4) · [V](#vizier) · [W](#wcs--world-coordinate-system) · [X](#xisf) ·
[Z](#zero-point--ponto-zero)

---

### Aberration (annual) / Aberração (anual)

🔴

🇬🇧 **English**
A tiny apparent shift of star positions (up to about 20 arcseconds) caused by the Earth's motion
around the Sun, like rain that seems to fall at an angle when you run. AstroFind computes it and
writes it in the log for information only. It does **not** apply it, because measuring against
catalog stars in the same image already includes it.

🇧🇷 **Português**
Um pequeno deslocamento aparente das posições das estrelas (até cerca de 20 segundos de arco)
causado pelo movimento da Terra em volta do Sol, como a chuva que parece cair inclinada quando
você corre. O AstroFind calcula e escreve no log só como informação. Ele **não** aplica, porque
medir em relação às estrelas de catálogo da mesma imagem já inclui esse efeito.

---

### ADES — Astrometry Data Exchange Standard

🟡

🇬🇧 **English**
The modern standard format for reporting asteroid and comet observations to the MPC. It replaces
the old fixed-width 80-column format and carries more information, such as position
uncertainties, the catalog used and the telescope. It comes in two forms: XML and PSV. AstroFind
writes ADES version 2022 only; it has no 80-column output. *(advanced: see
`docs/technical-reference.md`)*

🇧🇷 **Português**
O formato padrão moderno para reportar observações de asteroides e cometas ao MPC. Ele substitui
o antigo formato de 80 colunas de largura fixa e leva mais informação, como incertezas de posição,
o catálogo usado e o telescópio. Existe em duas formas: XML e PSV. O AstroFind escreve só ADES
versão 2022; não há saída de 80 colunas. *(avançado: veja `docs/technical-reference.md`)*

---

### ADU — Analog-to-Digital Unit / Unidade analógico-digital

🟡

🇬🇧 **English**
The raw number a camera stores for each pixel. Brighter light gives more ADU. AstroFind's
"Saturation Level:" setting (default 60000) is in ADU.

🇧🇷 **Português**
O número bruto que a câmera guarda para cada pixel. Luz mais forte dá mais ADU. A configuração
"Nível de saturação:" do AstroFind (padrão 60000) é em ADU.

---

### Airmass / Massa de ar

🟡

🇬🇧 **English**
A number saying how much air the starlight passes through, compared with looking straight up
(airmass = 1 at the zenith, larger near the horizon). More airmass means more dimming
(extinction). AstroFind always computes it; it corrects the brightness only when "Extinction
coeff k:" is above 0.

🇧🇷 **Português**
Um número que diz quanto ar a luz da estrela atravessa, em comparação com olhar direto para cima
(massa de ar = 1 no zênite, maior perto do horizonte). Mais massa de ar significa mais
escurecimento (extinção). O AstroFind sempre calcula; ele só corrige o brilho quando "Coef. de
extinção k:" é maior que 0.

---

### Annulus / Anel

🔴

🇬🇧 **English**
A ring around the photometric aperture. The sky brightness is measured in the ring and
subtracted, so only the star's own light is counted.

🇧🇷 **Português**
Um anel em volta da abertura fotométrica. O brilho do céu é medido no anel e subtraído, para que
só a luz da própria estrela seja contada.

---

### Aperture / Abertura

🟡

🇬🇧 **English**
A circle drawn around a star to add up its light (photometry). In AstroFind the radius is
automatic (2 × FWHM, at least 3 px) or fixed ("Fixed:" in **Settings → Detection**). Do not
confuse it with the **Aperture Tool** (`A`), which is the measuring tool. See also *Annulus*.

🇧🇷 **Português**
Um círculo desenhado em volta de uma estrela para somar a luz dela (fotometria). No AstroFind o
raio é automático (2 × FWHM, no mínimo 3 px) ou fixo ("Fixo:" em **Configurações → Detecção**).
Não confunda com a **Ferramenta Abertura** (`A`), que é a ferramenta de medição. Veja também
*Anel*.

---

### API key / Chave de API

🟢

🇬🇧 **English**
A personal code, like a password, that lets a program use an online service. AstroFind needs a
free astrometry.net API key for online plate solving. You get it at nova.astrometry.net and type
it in **Settings → Connections → API Key:**.

🇧🇷 **Português**
Um código pessoal, como uma senha, que permite a um programa usar um serviço online. O AstroFind
precisa de uma chave de API gratuita do astrometry.net para o plate solving online. Você a obtém
em nova.astrometry.net e digita em **Configurações → Conexões → Chave de API:**.

---

### Arcsecond, arcminute / Segundo de arco, minuto de arco

🟢

🇬🇧 **English**
Units for small angles on the sky. One degree has 60 arcminutes (′); one arcminute has 60
arcseconds (″ or "). The full Moon is about 30′ wide. Asteroid positions are measured to a
fraction of an arcsecond.

🇧🇷 **Português**
Unidades para ângulos pequenos no céu. Um grau tem 60 minutos de arco (′); um minuto de arco tem
60 segundos de arco (″ ou "). A Lua cheia tem cerca de 30′ de largura. As posições de asteroides
são medidas com fração de segundo de arco.

---

### Archive (compressed file) / Arquivo compactado

🟢

🇬🇧 **English**
One file that packs many files together, usually smaller: ZIP, TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR.
AstroFind extracts the images inside automatically. ZIP needs the system `unzip` program; the
others need AstroFind built with libarchive.

🇧🇷 **Português**
Um arquivo que junta muitos arquivos, normalmente menores: ZIP, TAR.GZ, TAR.BZ2, TAR.XZ, 7Z,
RAR. O AstroFind extrai as imagens de dentro automaticamente. O ZIP precisa do programa `unzip`
do sistema; os outros precisam do AstroFind compilado com libarchive.

---

### ASan / UBSan — AddressSanitizer / UndefinedBehaviorSanitizer *(computing term)*

🔴

🇬🇧 **English**
Tools built into the compiler (GCC/Clang) that catch memory bugs (ASan: reading or writing
outside allocated memory, use after free) and undefined behaviour (UBSan: for example integer
overflow or invalid casts) while the program runs. AstroFind's developers run the test suite
under both as part of the project's audit. You never need them just to *use* the app.

🇧🇷 **Português**
Ferramentas embutidas no compilador (GCC/Clang) que pegam bugs de memória (ASan: leitura ou
escrita fora da memória reservada, uso depois de liberar) e comportamento indefinido (UBSan: por
exemplo estouro de inteiro ou conversões inválidas) enquanto o programa roda. Os desenvolvedores
do AstroFind rodam a suíte de testes com os dois como parte da auditoria do projeto. Você nunca
precisa deles só para *usar* o aplicativo.

---

### ASTAP

🟡

🇬🇧 **English**
A free plate-solving program (by Han Kleijn) that runs on your own computer, without internet.
It needs the program **and** one of its star databases, both from
[hnsky.org](https://www.hnsky.org/astap.htm). In AstroFind choose "ASTAP (local, offline)" in
**Settings → Connections → Backend:** and fill "Executável ASTAP:".

🇧🇷 **Português**
Um programa gratuito de plate solving (de Han Kleijn) que roda no seu próprio computador, sem
internet. Precisa do programa **e** de um dos bancos de estrelas dele, ambos em
[hnsky.org](https://www.hnsky.org/astap.htm). No AstroFind escolha "ASTAP (local, offline)" em
**Configurações → Conexões → Backend:** e preencha "Executável ASTAP:".

---

### Asteroid / Asteroide

🟢

🇬🇧 **English**
A small rocky body orbiting the Sun, too small to be a planet. Most known asteroids orbit
between Mars and Jupiter (the "main belt"). Some come close to the Earth (near-Earth asteroids,
NEAs) and can move fast enough to leave a short trail in one exposure.

🇧🇷 **Português**
Um pequeno corpo rochoso que orbita o Sol, pequeno demais para ser planeta. A maioria dos
asteroides conhecidos orbita entre Marte e Júpiter (o "cinturão principal"). Alguns passam perto
da Terra (asteroides próximos da Terra, NEAs) e podem andar rápido o bastante para deixar um
rastro curto numa única exposição.

---

### Astrometry / Astrometria

🟢

🇬🇧 **English**
Measuring the precise *positions* of objects on the sky (as opposed to **photometry**, which
measures brightness). AstroFind's main job is the astrometry of moving objects.

🇧🇷 **Português**
Medir com precisão as *posições* de objetos no céu (em oposição à **fotometria**, que mede
brilho). A tarefa principal do AstroFind é a astrometria de objetos em movimento.

---

### astrometry.net

🟡

🇬🇧 **English**
A free online plate-solving service at nova.astrometry.net. It is AstroFind's default solver.
You need a free API key. AstroFind uploads each image as private ("publicly visible: no").

🇧🇷 **Português**
Um serviço online e gratuito de plate solving em nova.astrometry.net. É o solver padrão do
AstroFind. Você precisa de uma chave de API gratuita. O AstroFind envia cada imagem como privada
("publicly visible: no").

---

### Background (sky) / Fundo do céu

🟡

🇬🇧 **English**
The faint glow of the sky behind the stars (light pollution, moonlight, airglow). It must be
subtracted before measuring. AstroFind can also remove uneven background (gradients) before star
detection: "Subtrair modelo de fundo antes da detecção" in **Settings → Detection**.

🇧🇷 **Português**
O brilho fraco do céu atrás das estrelas (poluição luminosa, luar, luminescência do ar). Ele
precisa ser subtraído antes de medir. O AstroFind também pode remover fundo irregular
(gradientes) antes da detecção de estrelas: "Subtrair modelo de fundo antes da detecção" em
**Configurações → Detecção**.

---

### Bad pixel / Pixel ruim

🟡

🇬🇧 **English**
A sensor pixel that is always too bright ("hot pixel"), always dark, or part of a defective
column. AstroFind replaces such pixels with a value estimated from the neighbours (a 3×3 median)
so they are not mistaken for stars. It is on by default ("Corrigir pixels ruins
automaticamente" in **Settings → Camera**).

🇧🇷 **Português**
Um pixel do sensor que está sempre claro demais ("hot pixel"), sempre escuro, ou numa coluna
defeituosa. O AstroFind troca esses pixels por um valor estimado a partir dos vizinhos (uma
mediana 3×3) para que não sejam confundidos com estrelas. Vem ligado por padrão ("Corrigir
pixels ruins automaticamente" em **Configurações → Câmera**).

---

### Band (photometric) / Banda (fotométrica)

🟡

🇬🇧 **English**
The range of colours (wavelengths) a filter lets through, named by a letter such as V, R, B, g,
r, i. "C" means clear (no filter). AstroFind takes the band from the FITS `FILTER` keyword, or
from "Default band:" in **Settings → Detection**.

🇧🇷 **Português**
A faixa de cores (comprimentos de onda) que um filtro deixa passar, indicada por uma letra como
V, R, B, g, r, i. "C" quer dizer claro (sem filtro). O AstroFind pega a banda da palavra-chave
`FILTER` do FITS, ou de "Banda padrão:" em **Configurações → Detecção**.

---

### Bayer mosaic / Mosaico de Bayer

See / Veja *CFA*.

---

### Bias frame / Quadro de bias

🟡

🇬🇧 **English**
A zero-length (or shortest possible) exposure with the shutter closed. It records only the
camera's fixed electronic offset. AstroFind has no separate bias input: a dark frame with the
same exposure time as the images already contains the bias.

🇧🇷 **Português**
Uma exposição de duração zero (ou a mais curta possível) com o obturador fechado. Ela registra só
o deslocamento eletrônico fixo da câmera. O AstroFind não tem entrada separada para bias: um dark
com o mesmo tempo de exposição das imagens já contém o bias.

---

### BINTABLE (FITS binary table) / BINTABLE (tabela binária FITS)

🔴

🇬🇧 **English**
A table stored inside a FITS file, with named columns (for example RA, Dec, magnitude).
AstroFind reads BINTABLEs as a local star catalog and to import results of other programs
(DAOPHOT, SExtractor, IRAF, Astropy).

🇧🇷 **Português**
Uma tabela guardada dentro de um arquivo FITS, com colunas com nome (por exemplo AR, Dec,
magnitude). O AstroFind lê BINTABLEs como catálogo local de estrelas e para importar resultados
de outros programas (DAOPHOT, SExtractor, IRAF, Astropy).

---

### Blended source / Fonte blendada (sobreposta)

🟡

🇬🇧 **English**
Two or more objects so close in the image that their light overlaps into one blob. AstroFind
separates them with a ClumpFind-style method and draws them as magenta double circles.

🇧🇷 **Português**
Dois ou mais objetos tão próximos na imagem que a luz deles se junta numa só mancha. O AstroFind
separa esses objetos com um método no estilo ClumpFind e os desenha como círculos duplos
magenta.

---

### Blink / Piscar

🟢

🇬🇧 **English**
Showing several images of the same field one after the other, in the same place on the screen,
so that anything that moves (an asteroid) jumps while the stars stay still. The name comes from
the "blink comparator" instrument, used for example to discover Pluto in 1930. In AstroFind:
**Utilities → Begin Blink Mode** (`Ctrl+B`).

🇧🇷 **Português**
Mostrar várias imagens do mesmo campo uma depois da outra, no mesmo lugar da tela, para que
qualquer coisa que se mova (um asteroide) pule enquanto as estrelas ficam paradas. O nome vem do
instrumento "comparador de piscar" (blink comparator), usado por exemplo na descoberta de Plutão
em 1930. No AstroFind: **Utilitários → Ativar Modo de Piscagem** (`Ctrl+B`).

---

### Catalog star / Estrela de catálogo

🟢

🇬🇧 **English**
A star whose precise position and brightness are listed in a catalog (UCAC4 or Gaia DR3). AstroFind
uses these reference stars to turn pixel positions into sky coordinates and to calibrate
magnitudes. They appear as yellow crosses and as "Estrela Guia" in the Image Catalog.

🇧🇷 **Português**
Uma estrela cuja posição e brilho precisos estão numa lista (catálogo), como o UCAC4 ou o Gaia
DR3. O AstroFind usa essas estrelas de referência para transformar posições em pixels em
coordenadas do céu e para calibrar magnitudes. Elas aparecem como cruzes amarelas e como
"Estrela Guia" no Catálogo de Imagem.

---

### Centroid / Centroide

🟡

🇬🇧 **English**
The exact centre of a star's light, found to a fraction of a pixel. When you click an object,
AstroFind fits an elliptical PSF model (see *PSF*) to find it. If there is no star-like dot
there, the log says "Centroid failed — no source found at click position".

🇧🇷 **Português**
O centro exato da luz de uma estrela, encontrado com fração de pixel. Quando você clica num
objeto, o AstroFind ajusta um modelo de PSF elíptica (veja *PSF*) para encontrá-lo. Se não houver
um ponto parecido com estrela ali, o log diz "Centroide falhou — nenhuma fonte encontrada na
posição clicada".

---

### CFA / Bayer mosaic — Colour Filter Array / Mosaico de Bayer

🟡

🇬🇧 **English**
The pattern of tiny red, green and blue filters over the pixels of a colour camera sensor
(usually repeating 2×2 groups: one red, two green, one blue). Each pixel sees only one colour.
AstroFind reads DSLR RAW files as this raw mosaic. See also *Demosaic* and *Superpixel*.

🇧🇷 **Português**
O padrão de minúsculos filtros vermelhos, verdes e azuis sobre os pixels do sensor de uma câmera
colorida (normalmente grupos 2×2 repetidos: um vermelho, dois verdes, um azul). Cada pixel vê só
uma cor. O AstroFind lê arquivos RAW de DSLR como esse mosaico bruto. Veja também *Demosaico* e
*Superpixel*.

---

### CIRS — Celestial Intermediate Reference System

🔴

🇬🇧 **English**
An intermediate coordinate frame tied to the Earth's equator and rotation at a given moment. It
is a step between the fixed ICRS frame and the observer's local (topocentric) view of the sky.
*(advanced: see `docs/technical-reference.md`)*

🇧🇷 **Português**
Um sistema de coordenadas intermediário ligado ao equador e à rotação da Terra num dado instante.
É um passo entre o referencial fixo ICRS e a visão local (topocêntrica) do céu pelo observador.
*(avançado: veja `docs/technical-reference.md`)*

---

### ClumpFind

🔴

🇬🇧 **English**
A source-detection method (Williams, de Geus & Blitz, 1994) that finds separate peaks inside one
connected patch of bright pixels. AstroFind uses a ClumpFind-style pass to split blended
sources.

🇧🇷 **Português**
Um método de detecção de fontes (Williams, de Geus & Blitz, 1994) que encontra picos separados
dentro de uma única mancha contínua de pixels brilhantes. O AstroFind usa uma etapa no estilo
ClumpFind para separar fontes blendadas.

---

### Coma

🟡

🇬🇧 **English**
An optical defect that makes stars near the edge of the image look like little comets instead
of round dots. It stretches the PSF. AstroFind warns in the log when a measured object is
strongly elongated: "PSF elongation=… — check tracking, focus, or coma".

🇧🇷 **Português**
Um defeito óptico que faz as estrelas perto da borda da imagem parecerem pequenos cometas em vez
de pontos redondos. Ele estica a PSF. O AstroFind avisa no log quando um objeto medido está muito
alongado: "Elongação PSF=… — verifique rastreamento, foco ou coma".

---

### CRVAL / CRPIX / CD matrix / Matriz CD

🔴

🇬🇧 **English**
FITS header keywords that define the WCS solution. **CRPIX** is the reference pixel, **CRVAL**
is the sky coordinate (RA/Dec) at that pixel, and the **CD matrix** (4 numbers) holds the
rotation, scale and any skew of the image, in degrees per pixel.

🇧🇷 **Português**
Palavras-chave do cabeçalho FITS que definem a solução WCS. **CRPIX** é o pixel de referência,
**CRVAL** é a coordenada do céu (AR/Dec) nesse pixel, e a **matriz CD** (4 números) guarda a
rotação, a escala e qualquer inclinação da imagem, em graus por pixel.

---

### Dark current / Corrente de escuro

🟡

🇬🇧 **English**
Signal that builds up in a camera sensor over time even with no light, caused by heat. It is
measured with a **dark frame** and subtracted.

🇧🇷 **Português**
Sinal que se acumula no sensor da câmera com o tempo, mesmo sem luz, causado pelo calor. Ele é
medido com um **dark frame** e subtraído.

---

### Dark frame / Dark frame (quadro escuro)

🟢

🇬🇧 **English**
An exposure taken with the telescope covered, with the same exposure time and temperature as the
images. It records dark current, bias and hot pixels, which AstroFind subtracts. Load it with
**File → Use Dark Frame for Calibration…**. It must be a FITS file with the same size as the images; otherwise
the log says "Image …: dark frame size mismatch — skipped".

🇧🇷 **Português**
Uma exposição feita com o telescópio tampado, com o mesmo tempo de exposição e temperatura das
imagens. Ela registra a corrente de escuro, o bias e os hot pixels, que o AstroFind subtrai.
Carregue em **Arquivo → Usar Dark de Calibração…**. Precisa ser um arquivo FITS com o mesmo tamanho
das imagens; senão o log diz "Imagem …: tamanho do dark frame incompatível — ignorado".

---

### DATE-OBS / TIMESYS

🟡

🇬🇧 **English**
FITS header keywords. `DATE-OBS` holds the date and time of the observation; `TIMESYS` says
which time scale it uses (for example UTC). When `DATE-OBS` has no time zone and there is no
`TIMESYS=UTC`, AstroFind assumes UTC and warns "DATE-OBS sem fuso horário e sem TIMESYS —
assumido UTC; verifique se a câmera grava hora local." See the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

🇧🇷 **Português**
Palavras-chave do cabeçalho FITS. `DATE-OBS` guarda a data e hora da observação; `TIMESYS` diz
qual escala de tempo ela usa (por exemplo UTC). Quando o `DATE-OBS` não tem fuso e não existe
`TIMESYS=UTC`, o AstroFind assume UTC e avisa "DATE-OBS sem fuso horário e sem TIMESYS —
assumido UTC; verifique se a câmera grava hora local." Veja o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

---

### Declination / Declinação

See / Veja *RA / Dec*.

---

### Delta T (ΔT) / TT

🔴

🇬🇧 **English**
TT (Terrestrial Time) is a smooth time scale used for calculations; UTC follows the Earth's
slightly irregular rotation. ΔT is the difference TT − UTC, about 68–69 seconds today. AstroFind
has it in **Settings → Camera** ("ΔT (TT − UTC):", default 68.0 s). It is used **only** to
compute known-asteroid positions from the offline MPCORB file (orbits are given in TT). Image
times and the report time (`obsTime`) stay in UTC; ΔT is not added to them. (Version 1.1.0 and
earlier wrongly added ΔT to the image times; see [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).)

🇧🇷 **Português**
O TT (Tempo Terrestre) é uma escala de tempo uniforme usada nos cálculos; o UTC acompanha a
rotação um pouco irregular da Terra. O ΔT é a diferença TT − UTC, cerca de 68–69 segundos hoje. O
AstroFind tem esse valor em **Configurações → Câmera** ("ΔT (TT − UTC):", padrão 68,0 s). Ele é
usado **só** para calcular a posição de asteroides conhecidos pelo arquivo offline MPCORB (as
órbitas são dadas em TT). Os horários das imagens e do relatório (`obsTime`) ficam em UTC; o ΔT
não é somado a eles. (A versão 1.1.0 e anteriores somavam o ΔT aos horários das imagens, o que
era um erro; veja [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).)

---

### Demosaic / Demosaico

🟡

🇬🇧 **English**
Rebuilding a full-colour picture from a CFA (Bayer) mosaic by estimating the two missing colours
at each pixel. AstroFind uses a simple demosaic only to show RAW and colour SER images on screen;
measurements use the linear data.

🇧🇷 **Português**
Reconstruir uma imagem colorida completa a partir do mosaico CFA (Bayer), estimando as duas cores
que faltam em cada pixel. O AstroFind usa um demosaico simples só para mostrar na tela imagens RAW
e SER coloridas; as medições usam os dados lineares.

---

### Differential photometry / Fotometria diferencial

🟡

🇬🇧 **English**
Measuring an object's brightness by comparing it with catalog stars in the same image. Because
all of them pass through the same air and optics, most errors cancel. AstroFind uses nearby
catalog stars for this, then a whole-image zero-point, and as a last resort the instrumental
magnitude.

🇧🇷 **Português**
Medir o brilho de um objeto comparando-o com estrelas de catálogo da mesma imagem. Como todos
atravessam o mesmo ar e a mesma óptica, a maioria dos erros se cancela. O AstroFind usa para isso
estrelas de catálogo próximas, depois um ponto zero da imagem inteira e, em último caso, a
magnitude instrumental.

---

### Distribution (distro) / Distribuição (distro)

🟢

🇬🇧 **English**
A particular flavour of Linux, such as Ubuntu, Fedora, Debian or Arch. Each has its own package
manager and package names, which is why installation commands differ. See [Installation](https://github.com/petrinhu/astrofind/wiki/Installation).

🇧🇷 **Português**
Uma variante específica do Linux, como Ubuntu, Fedora, Debian ou Arch. Cada uma tem seu
gerenciador de pacotes e seus nomes de pacote, por isso os comandos de instalação mudam. Veja
[Installation](https://github.com/petrinhu/astrofind/wiki/Installation).

---

### Dock / Painel (dock)

🟢

🇬🇧 **English**
A panel attached to the side or bottom of the main window that you can move, hide or show.
AstroFind's docks are Workflow, Calibration, Images, Log, Observations and Image Catalog. Show
them again with the **Window → View …** items.

🇧🇷 **Português**
Um painel preso na lateral ou embaixo da janela principal, que você pode mover, esconder ou
mostrar. Os painéis do AstroFind são Fluxo de Trabalho, Calibração, Imagens, Registro (Log),
Observações e Catálogo de Imagem. Mostre de novo pelos itens **Janela → Exibir …**.

---

### Ecliptic / Eclíptica

🟢

🇬🇧 **English**
The path the Sun seems to follow across the sky during the year. The planets and most asteroids
orbit close to this plane, so fields near the ecliptic have more asteroids. AstroFind can draw it
with **Utilities → Ecliptic / Galactic Overlay** (`Ctrl+E`).

🇧🇷 **Português**
O caminho que o Sol parece fazer no céu ao longo do ano. Os planetas e a maioria dos asteroides
orbitam perto desse plano, então campos perto da eclíptica têm mais asteroides. O AstroFind pode
desenhá-la com **Utilitários → Sobreposição Eclíptica / Galáctica** (`Ctrl+E`).

---

### Elongation / Alongamento

🟡

🇬🇧 **English**
How stretched a star image is: the long axis divided by the short axis (1 = perfectly round).
AstroFind marks sources with elongation of at least 3 (setting "Streak threshold (a/b):") as
streaks, and warns about stretched stars when measuring.

🇧🇷 **Português**
O quanto a imagem de uma estrela está esticada: o eixo maior dividido pelo menor (1 = redonda).
O AstroFind marca como traços as fontes com alongamento de pelo menos 3 (configuração "Limiar de
traço (a/b):") e avisa sobre estrelas esticadas ao medir.

---

### Ephemeris / Efeméride

🟡

🇬🇧 **English**
A prediction of where a known object will be on the sky at given times. AstroFind can ask JPL
Horizons for one with **Internet → Query Horizons…**.

🇧🇷 **Português**
Uma previsão de onde um objeto conhecido estará no céu em determinados momentos. O AstroFind pode
pedir uma ao JPL Horizons com **Internet → Consultar Horizons…**.

---

### EXIF

🟢

🇬🇧 **English**
Information that digital cameras store inside each photo: camera model, exposure time, date and
time, and more. For DSLR RAW files AstroFind reads the exposure time, the camera name and the
time from EXIF. That time comes from the camera clock, so it is flagged as ambiguous.

🇧🇷 **Português**
Informações que câmeras digitais guardam dentro de cada foto: modelo da câmera, tempo de
exposição, data e hora, e outras. Para arquivos RAW de DSLR o AstroFind lê do EXIF o tempo de
exposição, o nome da câmera e o horário. Esse horário vem do relógio da câmera, por isso é marcado
como ambíguo.

---

### Extinction / Extinção

🟡

🇬🇧 **English**
The dimming of starlight by the atmosphere; it grows with airmass. AstroFind corrects it only
when "Extinction coeff k:" in **Settings → Detection** is above 0 (default 0, no correction).

🇧🇷 **Português**
O enfraquecimento da luz das estrelas pela atmosfera; ele cresce com a massa de ar. O AstroFind
só corrige quando "Coef. de extinção k:" em **Configurações → Detecção** é maior que 0 (padrão 0,
sem correção).

---

### FITS — Flexible Image Transport System

🟢

🇬🇧 **English**
The standard file format for astronomical images and data since 1981, maintained under the
International Astronomical Union (IAU). It stores the pixel data together with a text header of
metadata (when and where the image was taken, exposure time, telescope…). Extensions: `.fits`,
`.fit`, `.fts`.

🇧🇷 **Português**
O formato de arquivo padrão para imagens e dados astronômicos desde 1981, mantido no âmbito da
União Astronômica Internacional (IAU). Ele guarda os dados dos pixels junto com um cabeçalho de
texto com metadados (quando e onde a imagem foi feita, tempo de exposição, telescópio…).
Extensões: `.fits`, `.fit`, `.fts`.

---

### Flat field

🟢

🇬🇧 **English**
A calibration exposure of an evenly lit target (twilight sky or a light panel). It measures and
corrects differences in pixel sensitivity, vignetting (darker image corners) and dust shadows.
AstroFind divides the images by it. Load it with **File → Use Flat Field for Calibration…** (FITS, same size as
the images).

🇧🇷 **Português**
Uma exposição de calibração de um alvo iluminado por igual (céu do crepúsculo ou painel de luz).
Ela mede e corrige diferenças de sensibilidade entre pixels, vinhetagem (cantos mais escuros) e
sombras de poeira. O AstroFind divide as imagens por ela. Carregue em **Arquivo → Usar Flat de
Calibração…** (FITS, mesmo tamanho das imagens).

---

### FWHM — Full Width at Half Maximum / Largura a meia altura

🟡

🇬🇧 **English**
A measure of how big a star looks: the width of its light profile at half the peak brightness.
It depends on the air turbulence ("seeing"), the optics and the focus. Typically 1–4 arcseconds
for ground-based telescopes. AstroFind shows it in the Observations and Image Catalog tables
(`FWHM"`).

🇧🇷 **Português**
Uma medida de quão grande uma estrela parece: a largura do perfil de luz na metade do brilho
máximo. Depende da turbulência do ar ("seeing"), da óptica e do foco. Tipicamente 1–4 segundos de
arco em telescópios no solo. O AstroFind mostra esse valor nas tabelas de Observações e do
Catálogo de Imagem (`FWHM"`).

---

### Gaia DR3

🟡

🇬🇧 **English**
The third data release (2022) of the European Space Agency's Gaia mission: very precise positions
and motions of more than a billion stars, at epoch J2016.0, with magnitudes in Gaia's own G band.
One of AstroFind's two reference catalogs ("Gaia DR3" in **Settings → Connections → Catalog:**).

🇧🇷 **Português**
A terceira publicação de dados (2022) da missão Gaia, da Agência Espacial Europeia: posições e
movimentos muito precisos de mais de um bilhão de estrelas, na época J2016.0, com magnitudes na
banda G do próprio Gaia. Um dos dois catálogos de referência do AstroFind ("Gaia DR3" em
**Configurações → Conexões → Catálogo:**).

---

### Galactic plane / Plano galáctico

🟡

🇬🇧 **English**
The band of the Milky Way on the sky. It is crowded with stars and dust, which dims objects
behind it (high extinction). With **Utilities → Ecliptic / Galactic Overlay** on, AstroFind shows the
badge "⚠  Plano galáctico — alta extinção interestelar" when the field is within 15° of it.

🇧🇷 **Português**
A faixa da Via Láctea no céu. Ela tem muitas estrelas e poeira, que enfraquece os objetos atrás
dela (alta extinção). Com **Utilitários → Sobreposição Eclíptica / Galáctica** ligada, o AstroFind
mostra o aviso "⚠  Plano galáctico — alta extinção interestelar" quando o campo está a menos de
15° dele.

---

### Geocentric / Topocentric / Geocêntrico / Topocêntrico

🔴

🇬🇧 **English**
*Geocentric* means "as seen from the centre of the Earth"; *topocentric* means "as seen from your
place on the Earth's surface". For nearby objects the two positions differ. AstroFind needs your
real location to compute topocentric corrections; if it is 0°, 0° it warns "Localização não
configurada". The **Query Horizons…** result is geocentric.

🇧🇷 **Português**
*Geocêntrico* quer dizer "visto do centro da Terra"; *topocêntrico* quer dizer "visto do seu
lugar na superfície da Terra". Para objetos próximos, as duas posições são diferentes. O
AstroFind precisa da sua localização real para as correções topocêntricas; se ela for 0°, 0°, ele
avisa "Localização não configurada". O resultado de **Consultar Horizons…** é geocêntrico.

---

### Growth curve / Curva de crescimento

🔴

🇬🇧 **English**
A plot of how much of a star's light falls inside apertures of growing radius. It helps choose
the best aperture size. In AstroFind: **Utilities → Growth Curve…** (`Ctrl+Shift+G`).

🇧🇷 **Português**
Um gráfico de quanta luz de uma estrela cai dentro de aberturas com raio crescente. Ajuda a
escolher o melhor tamanho de abertura. No AstroFind: **Utilitários → Curva de Crescimento…**
(`Ctrl+Shift+G`).

---

### HDU — Header/Data Unit

🟡

🇬🇧 **English**
A self-contained block inside a FITS file: a text header plus (optionally) data. One FITS file can
hold several HDUs, for example one per colour channel. When a file has more than one image HDU,
AstroFind shows an HDU navigation bar (`‹`, a list, `›`) in the image window.

🇧🇷 **Português**
Um bloco independente dentro de um arquivo FITS: um cabeçalho de texto mais (opcionalmente)
dados. Um arquivo FITS pode ter vários HDUs, por exemplo um por canal de cor. Quando o arquivo tem
mais de um HDU de imagem, o AstroFind mostra uma barra de navegação de HDU (`‹`, uma lista, `›`)
na janela da imagem.

---

### Header / Cabeçalho

🟢

🇬🇧 **English**
The text part of a FITS file with keywords such as `DATE-OBS`, `EXPTIME`, `FILTER`, `TELESCOP`.
AstroFind reads many settings from it automatically. View it with **Image Tools → View FITS Header…**.

🇧🇷 **Português**
A parte de texto de um arquivo FITS, com palavras-chave como `DATE-OBS`, `EXPTIME`, `FILTER`,
`TELESCOP`. O AstroFind lê muitas configurações dele automaticamente. Veja com **Ferramentas de Imagem → Ver
Cabeçalho FITS…**.

---

### Histogram / Histograma

🟡

🇬🇧 **English**
A chart of how many pixels have each brightness value. It shows the sky level and the noise.
In AstroFind right-click an image → **Show Histogram**.

🇧🇷 **Português**
Um gráfico de quantos pixels têm cada valor de brilho. Ele mostra o nível do céu e o ruído. No
AstroFind, clique com o botão direito numa imagem → **Exibir Histograma**.

---

### Horizons (JPL)

🟡

🇬🇧 **English**
NASA JPL's online service that computes positions of solar-system objects. AstroFind queries it
with **Internet → Query Horizons…** and can add the result to the image overlay.

🇧🇷 **Português**
O serviço online do JPL/NASA que calcula posições de objetos do Sistema Solar. O AstroFind
consulta com **Internet → Consultar Horizons…** e pode acrescentar o resultado à sobreposição da
imagem.

---

### IASC — International Astronomical Search Collaboration

🟢

🇬🇧 **English**
An educational program that gives schools and the public real telescope images to search for
asteroids. AstroFind's workflow and school features suit this kind of campaign.

🇧🇷 **Português**
Um programa educacional que fornece a escolas e ao público imagens reais de telescópio para
procurar asteroides. O fluxo de trabalho e os recursos escolares do AstroFind servem para esse
tipo de campanha.

---

### ICRS / ICRF — International Celestial Reference System / Frame

🔴

🇬🇧 **English**
The modern standard reference frame for sky coordinates, tied to very distant quasars so it does
not drift. The ICRF is its practical realisation. AstroFind marks every ADES position with
`sys=ICRF`.

🇧🇷 **Português**
O referencial padrão moderno para coordenadas do céu, amarrado a quasares muito distantes para
não se deslocar. O ICRF é a sua realização prática. O AstroFind marca toda posição ADES com
`sys=ICRF`.

---

### Instrumental magnitude / Magnitude instrumental

🟡

🇬🇧 **English**
A brightness computed only from the counts in your image (−2.5 × log₁₀ of the flux), before
calibration with catalog stars. It is useful for comparisons but is not on the standard scale.
AstroFind falls back to it when no catalog star can be used ("Photometry: no catalog stars
matched — using instrumental mag").

🇧🇷 **Português**
Um brilho calculado só com as contagens da sua imagem (−2,5 × log₁₀ do fluxo), antes da calibração
com estrelas de catálogo. Serve para comparações, mas não está na escala padrão. O AstroFind usa
esse valor quando nenhuma estrela de catálogo pode ser usada ("Fotometria: nenhuma estrela do
catálogo correspondida — usando magnitude instrumental").

---

### JD / MJD — Julian Date / Modified Julian Date

🟡

🇬🇧 **English**
A continuous count of days used by astronomers, without months or leap years. JD counts days from
noon of 1 January 4713 BC; MJD = JD − 2400000.5. J2000.0, a common reference epoch, is JD
2451545.0. AstroFind uses the JD of mid-exposure; you can edit it in **Image Tools → Edit Image
Settings…** ("Julian Date:").

🇧🇷 **Português**
Uma contagem contínua de dias usada por astrônomos, sem meses nem anos bissextos. O JD conta dias
a partir do meio-dia de 1º de janeiro de 4713 a.C.; MJD = JD − 2400000,5. J2000.0, uma época de
referência comum, é o JD 2451545,0. O AstroFind usa o JD do meio da exposição; você pode editá-lo
em **Ferramentas de Imagem → Editar Configurações da Imagem…** ("Data Juliana:").

---

### KOO — Known Object Overlay / Sobreposição de Objetos Conhecidos

🟢

🇬🇧 **English**
AstroFind's feature that marks already-catalogued asteroids, comets and planets predicted to be in
your image: **Utilities → Show Known Objects** (`Ctrl+K`). It asks the IMCCE SkyBoT service, or uses
MPCORB.DAT when SkyBoT cannot be reached. Asteroids are green circles, planets light blue, comets
orange.

🇧🇷 **Português**
O recurso do AstroFind que marca asteroides, cometas e planetas já catalogados que deveriam estar
na sua imagem: **Utilitários → Mostrar Objetos Conhecidos** (`Ctrl+K`). Ele consulta o
serviço SkyBoT do IMCCE, ou usa o MPCORB.DAT quando o SkyBoT não responde. Asteroides são
círculos verdes, planetas azul-claro, cometas laranja.

---

### Label (PDS) / Rótulo (PDS)

🟡

🇬🇧 **English**
The text that describes a PDS data file: image size, number format, time, target and so on. In
PDS3 it is inside the `.img` file ("attached") or in a separate `.lbl` file ("detached"). In PDS4
it is an `.xml` file. AstroFind needs the label to read the image.

🇧🇷 **Português**
O texto que descreve um arquivo de dados PDS: tamanho da imagem, formato dos números, horário,
alvo e assim por diante. No PDS3 ele fica dentro do `.img` ("anexado") ou num arquivo `.lbl`
separado ("destacado"). No PDS4 é um arquivo `.xml`. O AstroFind precisa do rótulo para ler a
imagem.

---

### LibRaw

🟡

🇬🇧 **English**
A free software library that reads camera RAW files from many brands. AstroFind uses it,
optionally, to open DSLR RAW files. If AstroFind was compiled without it, RAW files are refused
with "DSLR RAW support is not available in this build of AstroFind (compiled without LibRaw): …".

🇧🇷 **Português**
Uma biblioteca de software livre que lê arquivos RAW de câmeras de muitas marcas. O AstroFind a
usa, opcionalmente, para abrir arquivos RAW de DSLR. Se o AstroFind foi compilado sem ela, os
arquivos RAW são recusados com "DSLR RAW support is not available in this build of AstroFind
(compiled without LibRaw): …".

---

### Light curve / Curva de luz

🟡

🇬🇧 **English**
A plot of an object's brightness over time. It can show an asteroid's rotation. In AstroFind:
**Utilities → Light Curve…** (`Ctrl+Shift+L`; in 1.1.0 and earlier its `Ctrl+L` clashed with Import
Images, so use the menu there).

🇧🇷 **Português**
Um gráfico do brilho de um objeto ao longo do tempo. Pode mostrar a rotação de um asteroide. No
AstroFind: **Utilitários → Curva de Luz…** (`Ctrl+Shift+L`; na 1.1.0 e anteriores o atalho
`Ctrl+L` conflitava com Carregar Imagens, então lá use o menu).

---

### LUT — Look-Up Table / Tabela de cores

🟢

🇬🇧 **English**
A recipe that turns each brightness value into a colour, used to show grey data in "false
colour" (Hot, Cool, Viridis) so faint detail is easier to see. It changes only the display. In
AstroFind: "Cor falsa (LUT):" in **Image Tools → Adjust Black Point and Contrast…**.

🇧🇷 **Português**
Uma receita que transforma cada valor de brilho numa cor, usada para mostrar dados cinza em "cor
falsa" (Quente, Frio, Viridis) e facilitar ver detalhes fracos. Muda só a visualização. No
AstroFind: "Cor falsa (LUT):" em **Ferramentas de Imagem → Ajustar Ponto Preto e Contraste…**.

---

### Magnitude / Magnitude

🟢

🇬🇧 **English**
The astronomical brightness scale. **Smaller numbers mean brighter** objects; 5 magnitudes are a
factor of 100 in brightness. The faintest stars seen by eye are about magnitude 6; amateur
telescope images often reach magnitude 18–20.

🇧🇷 **Português**
A escala de brilho da astronomia. **Números menores significam objetos mais brilhantes**; 5
magnitudes são um fator de 100 no brilho. As estrelas mais fracas vistas a olho nu têm magnitude
por volta de 6; imagens de telescópios amadores costumam chegar à magnitude 18–20.

---

### Master frame / Quadro mestre

🟡

🇬🇧 **English**
One calibration frame (dark or flat) made by combining many individual frames (average or
median), which lowers random noise and removes cosmic-ray hits. In AstroFind: the **"Build
Master…"** button in the Calibration dock.

🇧🇷 **Português**
Um único quadro de calibração (dark ou flat) feito combinando muitos quadros individuais (média ou
mediana), o que reduz o ruído aleatório e remove impactos de raios cósmicos. No AstroFind: o botão
**"Construir Master…"** no painel de Calibração.

---

### MOD — Moving Object Detection / Detecção de Objetos em Movimento

🟡

🇬🇧 **English**
AstroFind's automatic search for objects that move steadily across the images:
**Astrometry Tools → Detect Moving Objects…** (`Ctrl+M`). Candidates are marked "Cand #N". By default
an object must appear in at least 3 images.

🇧🇷 **Português**
A busca automática do AstroFind por objetos que andam de forma regular pelas imagens:
**Ferramentas de Astrometria → Detectar Objetos em Movimento…** (`Ctrl+M`). Os candidatos são marcados
"Cand #N". Por padrão, um objeto precisa aparecer em pelo menos 3 imagens.

---

### MPC — Minor Planet Center

🟢

🇬🇧 **English**
The official body, working under the International Astronomical Union and based at the Center for
Astrophysics | Harvard & Smithsonian, that collects asteroid and comet observations from all over
the world and computes their orbits. AstroFind's reports are made for the MPC.

🇧🇷 **Português**
O órgão oficial, ligado à União Astronômica Internacional e sediado no Center for Astrophysics |
Harvard & Smithsonian, que reúne observações de asteroides e cometas do mundo todo e calcula suas
órbitas. Os relatórios do AstroFind são feitos para o MPC.

---

### MPCORB (MPCORB.DAT)

🟡

🇬🇧 **English**
The MPC's file with the orbits of all known asteroids. It is large (AstroFind says about 200 MB).
Download it with **Internet → Download MPCOrb Database** and update it with **Internet → Update MPCOrb Database**.
AstroFind uses it to find known objects offline when SkyBoT cannot be reached.

🇧🇷 **Português**
O arquivo do MPC com as órbitas de todos os asteroides conhecidos. Ele é grande (o AstroFind diz
cerca de 200 MB). Baixe com **Internet → Baixar Banco MPCOrb** e atualize com **Internet → Atualizar
Banco MPCOrb**. O AstroFind o usa para achar objetos conhecidos offline quando o SkyBoT não responde.

---

### NaN — Not a Number

🟡

🇬🇧 **English**
A special value meaning "no valid number here", for example pixels outside the camera's area or
marked as missing in a PDS file. AstroFind draws NaN pixels in **magenta** and ignores them in
calculations.

🇧🇷 **Português**
Um valor especial que quer dizer "não há número válido aqui", por exemplo pixels fora da área da
câmera ou marcados como ausentes num arquivo PDS. O AstroFind desenha pixels NaN em **magenta** e
os ignora nos cálculos.

---

### Observatory code / Código de observatório

🟢

🇬🇧 **English**
A 3-character code the MPC gives to one observing site (for example `568`, `W49`). It is needed for
formal submissions. In AstroFind: **Settings → Observer → "MPC Station Code:"**. With no code the
report uses `XXX` ("no code yet"). Do not use `500` as a placeholder: it is the real code for the
centre of the Earth. See the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

🇧🇷 **Português**
Um código de 3 caracteres que o MPC dá a um local de observação (por exemplo `568`, `W49`). É
necessário para envios formais. No AstroFind: **Configurações → Observador → "Código de estação
MPC:"**. Sem código, o relatório usa `XXX` ("ainda sem código"). Não use `500` como marcador: é o
código real do centro da Terra. Veja o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

---

### PDS / PDS3 / PDS4

🟡

🇬🇧 **English**
The **Planetary Data System**, NASA's archive of planetary mission data, and its file formats.
**PDS3** is the older one (an `.img` file with a text label inside or in a `.lbl` file); **PDS4**
is the newer one (an `.xml` label that points to the data). AstroFind opens images in both. See
[File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

🇧🇷 **Português**
O **Planetary Data System**, o arquivo da NASA com dados de missões planetárias, e seus formatos
de arquivo. **PDS3** é o mais antigo (um arquivo `.img` com rótulo de texto dentro ou num `.lbl`);
**PDS4** é o mais novo (um rótulo `.xml` que aponta para os dados). O AstroFind abre imagens dos
dois. Veja [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

---

### Photometry / Fotometria

🟢

🇬🇧 **English**
Measuring the *brightness* of objects (as opposed to **astrometry**, which measures position).

🇧🇷 **Português**
Medir o *brilho* de objetos (em oposição à **astrometria**, que mede posição).

---

### Pixel scale / Escala de pixel

See / Veja *Plate scale*.

---

### Plate scale / Escala de placa (escala de pixel)

🟡

🇬🇧 **English**
How much sky one pixel covers, usually in arcseconds per pixel. It depends on the focal length
and the pixel size. AstroFind gets it from the plate solution or the header. In
**Settings → Camera** the field is in **″/px** (arcsec per pixel, without binning); leave it at 0
("Auto") unless you know it. (In 1.1.0 and earlier the field was wrongly labelled "°/px".)

🇧🇷 **Português**
Quanto céu um pixel cobre, normalmente em segundos de arco por pixel. Depende da distância focal e
do tamanho do pixel. O AstroFind obtém esse valor da solução de placa ou do cabeçalho. Em
**Configurações → Câmera** o campo é em **″/px** (segundos de arco por pixel, sem binning); deixe
em 0 ("Automático"), a menos que você saiba o valor. (Na 1.1.0 e anteriores o campo tinha o
rótulo errado "°/px".)

---

### Plate solving / Solução de placa (plate solving)

🟢

🇬🇧 **English**
Working out, from the star pattern alone, exactly which part of the sky an image shows: centre,
rotation and scale. The result is the **WCS**. AstroFind does it inside **Astrometry Tools → Run Data
Reduction…** (`Ctrl+A`), online with astrometry.net or offline with ASTAP.

🇧🇷 **Português**
Descobrir, só pelo padrão das estrelas, exatamente qual parte do céu uma imagem mostra: centro,
rotação e escala. O resultado é o **WCS**. O AstroFind faz isso dentro de **Ferramentas de Astrometria → Executar
Redução de Dados…** (`Ctrl+A`), online com o astrometry.net ou offline com o ASTAP.

---

### Power spectrum / Espectro de potência

🔴

🇬🇧 **English**
A picture of which spatial patterns (frequencies) are present in an image, computed with an FFT
(Fast Fourier Transform). It reveals repeating electronic noise and focus problems. In AstroFind
right-click an image → **Show Power Spectrum**.

🇧🇷 **Português**
Uma imagem de quais padrões espaciais (frequências) existem numa imagem, calculada com uma FFT
(Transformada Rápida de Fourier). Revela ruído eletrônico repetitivo e problemas de foco. No
AstroFind, clique com o botão direito numa imagem → **Mostrar Espectro de Potência**.

---

### Precession / Nutation / Precessão / Nutação

🔴

🇬🇧 **English**
Slow wobbles of the Earth's axis: precession is a 26 000-year cycle, nutation a small 18.6-year
nodding. They shift the coordinate grid over time. Because AstroFind measures positions against
catalog stars in the same image, the plate solution already includes these effects.

🇧🇷 **Português**
Oscilações lentas do eixo da Terra: a precessão é um ciclo de 26 000 anos, a nutação um pequeno
balanço de 18,6 anos. Elas deslocam a grade de coordenadas com o tempo. Como o AstroFind mede as
posições em relação às estrelas de catálogo da mesma imagem, a solução de placa já inclui esses
efeitos.

---

### Project (.gus) / Projeto (.gus)

🟢

🇬🇧 **English**
A file that saves your work: which images, their plate solutions, detected stars, known objects,
display settings and measurements. It stores the image **paths**, not the pixels, so keep the
images where they are. Use **File → Save Project** (`Ctrl+S`) and **File → Open Project…**
(`Ctrl+O`).

🇧🇷 **Português**
Um arquivo que salva o seu trabalho: quais imagens, suas soluções de placa, estrelas detectadas,
objetos conhecidos, ajustes de exibição e medições. Ele guarda os **caminhos** das imagens, não os
pixels, então mantenha as imagens onde estão. Use **Arquivo → Salvar Projeto** (`Ctrl+S`) e
**Arquivo → Abrir Projeto…** (`Ctrl+O`).

---

### Projection / Projeção

🔴

🇬🇧 **English**
A mathematical way to draw the curved sky on a flat image, like map projections of the round
Earth. AstroFind supports TAN (the default, fine for normal telescope fields), SIN, ARC, STG, CAR,
MER, GLS/SFL and AIT.

🇧🇷 **Português**
Um jeito matemático de desenhar o céu curvo numa imagem plana, como as projeções de mapa da Terra
redonda. O AstroFind aceita TAN (o padrão, bom para campos normais de telescópio), SIN, ARC, STG,
CAR, MER, GLS/SFL e AIT.

---

### PSF — Point Spread Function / Função de espalhamento de ponto

🔴

🇬🇧 **English**
The 2-D pattern of light a single point (a star) makes on the detector after diffraction, optics
and air have spread it. Fitting a curve (a Gaussian, possibly elliptical and rotated) to it gives
a precise centre and brightness. AstroFind fits an elliptical Gaussian and reports FWHM and
elongation.

🇧🇷 **Português**
O padrão 2-D de luz que um único ponto (uma estrela) forma no detector depois de espalhado pela
difração, pela óptica e pelo ar. Ajustar uma curva (uma gaussiana, possivelmente elíptica e
girada) a esse padrão dá um centro e um brilho precisos. O AstroFind ajusta uma gaussiana elíptica
e informa o FWHM e o alongamento.

---

### PSV — Pipe-Separated Values

🟡

🇬🇧 **English**
A text table where columns are separated by the `|` character. It is one of the two ADES forms
(the other is XML). AstroFind sends the PSV form to the MPC.

🇧🇷 **Português**
Uma tabela de texto em que as colunas são separadas pelo caractere `|`. É uma das duas formas do
ADES (a outra é XML). O AstroFind envia a forma PSV ao MPC.

---

### RA / Dec / AR / Dec

🟢

🇬🇧 **English**
Right Ascension (RA) and Declination (Dec): the "longitude" and "latitude" of the sky. RA is
measured eastward along the celestial equator, Dec north (+) or south (−) of it. AstroFind shows
both in decimal degrees in the status bar and tables.

🇧🇷 **Português**
Ascensão Reta (AR) e Declinação (Dec): a "longitude" e a "latitude" do céu. A AR é medida para
leste ao longo do equador celeste, a Dec ao norte (+) ou ao sul (−) dele. O AstroFind mostra as
duas em graus decimais na barra de status e nas tabelas.

---

### RAW (camera) / RAW (câmera)

🟢

🇬🇧 **English**
The unprocessed file a digital camera can save (`.cr2`, `.cr3`, `.nef`, `.arw`, `.dng`, `.raf`,
`.orf`, `.rw2`, `.pef` and others). It keeps the linear sensor data, without the processing of a
JPEG, which is what measurements need. AstroFind opens RAW files when built with LibRaw.

🇧🇷 **Português**
O arquivo sem processamento que uma câmera digital pode salvar (`.cr2`, `.cr3`, `.nef`, `.arw`,
`.dng`, `.raf`, `.orf`, `.rw2`, `.pef` e outros). Ele guarda os dados lineares do sensor, sem o
processamento de um JPEG, que é o que as medições precisam. O AstroFind abre arquivos RAW quando
compilado com LibRaw.

---

### Refraction / Refração (atmosférica)

🟡

🇬🇧 **English**
The bending of starlight by the atmosphere, which lifts objects slightly. It is zero straight
overhead, about 1 arcminute at 45° altitude, and about 34 arcminutes at the horizon. AstroFind
skips the correction for space telescopes, below 1° altitude, and (AUD-CORR-7) for **any position
already derived from a catalog plate solution** (`astrometry.net`/ASTAP against Gaia/UCAC4/2MASS):
that fit is refracted along with its reference stars, so it already absorbs the mean refraction,
and correcting it again would double it (up to ~1.7′ at 30° altitude). In practice this means the
Bennett formula does not run on today's typical measured position, since those come from a plate
solution; it would only apply to a ground-based position obtained some other way (e.g. raw
pointing/mount coordinates). *In version 1.1.0 and earlier, Bennett refraction was applied to
every ground-based measurement regardless of how the sky position was obtained.*

🇧🇷 **Português**
A curvatura da luz das estrelas pela atmosfera, que levanta um pouco os objetos. É zero bem acima
da cabeça, cerca de 1 minuto de arco a 45° de altura e cerca de 34 minutos de arco no horizonte. O
AstroFind pula a correção para telescópios espaciais, abaixo de 1° de altura e (AUD-CORR-7) para
**qualquer posição já derivada de uma solução de plate-solve por catálogo** (`astrometry.net`/ASTAP
contra Gaia/UCAC4/2MASS): esse ajuste é refratado junto com suas estrelas de referência, então já
absorve a refração média, e corrigi-la de novo dobraria o efeito (até ~1,7′ a 30° de altitude). Na
prática isso significa que a fórmula de Bennett não roda sobre a posição medida típica de hoje, já
que estas vêm de uma solução de plate-solve; ela só se aplicaria a uma posição terrestre obtida de
outro jeito (ex.: coordenadas brutas de apontamento/montagem). *Na versão 1.1.0 e anteriores, a
refração de Bennett era aplicada a toda medição terrestre, independente de como a posição celeste
foi obtida.*

---

### Residual / Resíduo

🟡

🇬🇧 **English**
What is left after a comparison. (1) In astrometry: the difference between a star's measured
position and its catalog position; AstroFind summarises these as the "WCS RMS" in the log, and
uses that value as the position uncertainty in the report. (2) In **Utilities → Compare Sessions…**:
the "Residual" image, what remains after subtracting a reference image, where new or moving
objects stand out.

🇧🇷 **Português**
O que sobra depois de uma comparação. (1) Em astrometria: a diferença entre a posição medida de
uma estrela e a posição dela no catálogo; o AstroFind resume isso como o "WCS RMS" no log e usa
esse valor como incerteza da posição no relatório. (2) Em **Utilitários → Comparar Sessões…**: a
imagem "Residual", o que sobra depois de subtrair uma imagem de referência, onde objetos novos ou
em movimento se destacam.

---

### Saturation / Saturação

🟡

🇬🇧 **English**
The brightness level at which a pixel cannot record more light. Saturated stars have a flat top
and give poor measurements. AstroFind's "Saturation Level:" (default 60000 ADU) is in
**Settings → Camera**.

🇧🇷 **Português**
O nível de brilho em que um pixel não consegue registrar mais luz. Estrelas saturadas ficam com o
topo achatado e dão medições ruins. O "Nível de saturação:" do AstroFind (padrão 60000 ADU) fica
em **Configurações → Câmera**.

---

### SEP — Source Extraction and Photometry

🔴

🇬🇧 **English**
A library based on the well-known SExtractor program. AstroFind uses it to find stars in each
image (at most the 500 brightest).

🇧🇷 **Português**
Uma biblioteca baseada no conhecido programa SExtractor. O AstroFind a usa para encontrar as
estrelas em cada imagem (no máximo as 500 mais brilhantes).

---

### SER

🟡

🇬🇧 **English**
A simple video file format used for planetary and fast imaging. AstroFind reads mono, colour and
Bayer SER files.

🇧🇷 **Português**
Um formato simples de vídeo usado para imagens planetárias e rápidas. O AstroFind lê arquivos SER
monocromáticos, coloridos e Bayer.

---

### Session / Sessão

🟢

🇬🇧 **English**
The set of images you are working on now, with everything computed for them. By default a
session holds up to 20 images. Save it as a project (.gus). **File → Close Project** ends it;
`Ctrl+R` and `Ctrl+W` clear it without asking.

🇧🇷 **Português**
O conjunto de imagens com que você está trabalhando agora, com tudo o que foi calculado para elas.
Por padrão uma sessão tem até 20 imagens. Salve como projeto (.gus). **Arquivo → Fechar Projeto**
encerra a sessão; `Ctrl+R` e `Ctrl+W` limpam sem perguntar.

---

### SkyBoT

🟡

🇬🇧 **English**
An online service of the IMCCE (Paris Observatory) that lists the known solar-system objects in a
given patch of sky at a given time. AstroFind uses it when you Show Known Objects.

🇧🇷 **Português**
Um serviço online do IMCCE (Observatório de Paris) que lista os objetos conhecidos do Sistema
Solar numa região do céu num dado momento. O AstroFind o usa na Sobreposição de Objetos
Conhecidos.

---

### SNR — Signal-to-Noise Ratio / Razão sinal-ruído

🟡

🇬🇧 **English**
How strong a signal is compared with the random noise around it. Higher is more reliable; SNR 5
or more is usually a solid detection. Stacking N images improves SNR by about √N. AstroFind's
Detect Moving Objects uses "MOD min. SNR:" (default 5).

🇧🇷 **Português**
Quão forte um sinal é em comparação com o ruído aleatório em volta. Quanto maior, mais confiável;
SNR 5 ou mais costuma ser uma detecção sólida. Empilhar N imagens melhora o SNR por cerca de √N. A
Detecção de Objetos em Movimento do AstroFind usa "MOD SNR mín.:" (padrão 5).

---

### Stacking / Empilhamento

🟡

🇬🇧 **English**
Combining several aligned exposures into one to raise the signal-to-noise ratio and show fainter
objects. In AstroFind: **Image Tools → Rebuild Stack** (aligned on the stars, or by FFT when stars are missing). See also *Track and
stack*.

🇧🇷 **Português**
Combinar várias exposições alinhadas numa só para aumentar a razão sinal-ruído e mostrar objetos
mais fracos. No AstroFind: **Ferramentas de Imagem → Reconstruir Empilhamento** (alinhado pelas estrelas, ou por FFT quando faltam estrelas). Veja
também *Track and stack*.

---

### Streak / Traço

🟡

🇬🇧 **English**
A short elongated trail in one exposure instead of a round dot, made by an object that moved
during the exposure: a fast near-Earth asteroid or a satellite. AstroFind draws streaks as orange
ellipses.

🇧🇷 **Português**
Um rastro curto e alongado numa única exposição em vez de um ponto redondo, feito por um objeto
que se moveu durante a exposição: um asteroide rápido próximo da Terra ou um satélite. O AstroFind
desenha os traços como elipses laranja.

---

### Stretch / Transfer function / Esticamento / Função de transferência

🟢

🇬🇧 **English**
The rule that maps pixel values to screen brightness. Choosing a good stretch makes faint stars
visible. In **Image Tools → Adjust Black Point and Contrast…** AstroFind offers "Linear", "Logarítmica", "Raiz
Quadrada (Sqrt)", "Asinh" and "Equalização de Histograma", plus the "⟳ Auto" button. It changes
only the display.

🇧🇷 **Português**
A regra que transforma os valores dos pixels em brilho na tela. Um bom esticamento torna visíveis
as estrelas fracas. Em **Ferramentas de Imagem → Ajustar Ponto Preto e Contraste…** o AstroFind oferece "Linear",
"Logarítmica", "Raiz Quadrada (Sqrt)", "Asinh" e "Equalização de Histograma", mais o botão
"⟳ Auto". Muda só a visualização.

---

### Superpixel

🟡

🇬🇧 **English**
One value made from a 2×2 group of CFA pixels (one red, two green, one blue). AstroFind uses the
brightness (R + 2G + B)/4 of each group to detect and measure stars in DSLR RAW images, so no
colour guessing affects the measurement.

🇧🇷 **Português**
Um valor feito de um grupo 2×2 de pixels do CFA (um vermelho, dois verdes, um azul). O AstroFind
usa o brilho (R + 2G + B)/4 de cada grupo para detectar e medir estrelas em imagens RAW de DSLR,
para que nenhuma estimativa de cor afete a medição.

---

### Time zone / Fuso horário

🟢

🇬🇧 **English**
The difference between local clock time and UTC (for example UTC−3 in Brasília). Observation
times must be in UTC. AstroFind's "Time Zone:" setting is stored but does **not** change image
times; correct a wrong time in **Image Tools → Edit Image Settings…**. See the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

🇧🇷 **Português**
A diferença entre a hora local do relógio e o UTC (por exemplo UTC−3 em Brasília). Os horários de
observação precisam estar em UTC. A configuração "Fuso horário:" do AstroFind é guardada, mas
**não** muda o horário das imagens; corrija um horário errado em **Ferramentas de Imagem → Editar Configurações da
Imagem…**. Veja o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

---

### Track and stack / Track & Stack

🔴

🇬🇧 **English**
Stacking images while shifting each one by the expected motion of a moving object, so the object
adds up into a sharp dot while the stars smear. It reveals objects too faint for one image. In
AstroFind: **Astrometry Tools → Stack Images…** (`Ctrl+T`), where you type the shift per frame (dX, dY)
in pixels.

🇧🇷 **Português**
Empilhar imagens deslocando cada uma pelo movimento esperado de um objeto, para que o objeto se
some num ponto nítido enquanto as estrelas se espalham. Revela objetos fracos demais para uma
imagem só. No AstroFind: **Ferramentas de Astrometria → Empilhar Imagens…** (`Ctrl+T`), onde você digita o
deslocamento por quadro (dX, dY) em pixels.

---

### Tracklet

🔴

🇬🇧 **English**
A short series of positions of one object from a single night, consistent with steady motion.
It is the basic unit the MPC links into orbits. AstroFind's Detect Moving Objects builds such
tracks across the images; in the ADES report an unidentified object gets a temporary name
(`trkSub`, "UNKN" by default).

🇧🇷 **Português**
Uma série curta de posições de um objeto numa mesma noite, compatível com um movimento regular. É
a unidade básica que o MPC liga em órbitas. A Detecção de Objetos em Movimento do AstroFind monta
esses trajetos nas imagens; no relatório ADES um objeto não identificado recebe um nome
provisório (`trkSub`, "UNKN" por padrão).

---

### UCAC4

🟡

🇬🇧 **English**
The fourth U.S. Naval Observatory CCD Astrograph Catalog (2012): positions, proper motions and
magnitudes of about 113 million stars, down to about magnitude 16. It is AstroFind's default
reference catalog; AstroFind moves each star to the image date using its proper motion.

🇧🇷 **Português**
O quarto catálogo do Astrógrafo CCD do Observatório Naval dos EUA (2012): posições, movimentos
próprios e magnitudes de cerca de 113 milhões de estrelas, até mais ou menos a magnitude 16. É o
catálogo de referência padrão do AstroFind; o AstroFind leva cada estrela até a data da imagem
usando o movimento próprio dela.

---

### UTC — Coordinated Universal Time / Tempo Universal Coordenado

🟢

🇬🇧 **English**
The world's reference clock time, the same everywhere (no time zones, no daylight saving). All
observation times sent to the MPC are in UTC. Set your camera or capture software to UTC to avoid
mistakes.

🇧🇷 **Português**
O horário de referência do mundo, o mesmo em todo lugar (sem fusos, sem horário de verão). Todos
os horários de observação enviados ao MPC estão em UTC. Deixe a câmera ou o programa de captura
em UTC para evitar erros.

---

### VizieR

🟡

🇬🇧 **English**
The online catalog library of the CDS (Strasbourg). AstroFind downloads UCAC4 or Gaia DR3 stars
from it for the field of the image and keeps a local copy (cache) to reuse later.

🇧🇷 **Português**
A biblioteca online de catálogos do CDS (Estrasburgo). O AstroFind baixa dela as estrelas do UCAC4
ou do Gaia DR3 do campo da imagem e guarda uma cópia local (cache) para reusar depois.

---

### WCS — World Coordinate System

🟡

🇬🇧 **English**
The FITS standard (Greisen & Calabretta 2002) that links each pixel to a sky coordinate
(RA/Dec). It is the result of plate solving and is stored in the header as CRVAL/CRPIX/CD
keywords plus a projection type. Images that already have a WCS are not solved again.

🇧🇷 **Português**
O padrão FITS (Greisen & Calabretta 2002) que liga cada pixel a uma coordenada do céu (AR/Dec). É o
resultado do plate solving e fica no cabeçalho como palavras-chave CRVAL/CRPIX/CD mais um tipo de
projeção. Imagens que já têm WCS não são resolvidas de novo.

---

### XISF

🟡

🇬🇧 **English**
The Extensible Image Serialization Format, the native image format of the PixInsight program.
AstroFind reads XISF files with 1 or 3 channels.

🇧🇷 **Português**
O Extensible Image Serialization Format, o formato de imagem nativo do programa PixInsight. O
AstroFind lê arquivos XISF com 1 ou 3 canais.

---

### Zero-point / Ponto zero

🔴

🇬🇧 **English**
The constant that turns instrumental magnitudes into the standard magnitude scale. It is found by
comparing measured stars with catalog stars of known magnitude. AstroFind logs it as "ZP=…".

🇧🇷 **Português**
A constante que transforma magnitudes instrumentais na escala padrão de magnitude. Ela é
encontrada comparando estrelas medidas com estrelas de catálogo de magnitude conhecida. O
AstroFind escreve no log como "ZP=…".

---

🇬🇧 **Missing a word?** If you find a term in AstroFind or this wiki that isn't listed here,
please open an issue at <https://github.com/petrinhu/astrofind/issues> so we can add it.

🇧🇷 **Faltou uma palavra?** Se você encontrar um termo no AstroFind ou nesta wiki que não está
aqui, abra uma issue em <https://github.com/petrinhu/astrofind/issues> para acrescentarmos.

**See also / Veja também:** [Home](https://github.com/petrinhu/astrofind/wiki/Home) · [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) · [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) · [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ) ·
[Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) · [Manual](https://github.com/petrinhu/astrofind/wiki/Manual)
