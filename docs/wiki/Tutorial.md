# Tutorial — All Features Explained / Tutorial — Todas as Funções Explicadas

🇬🇧 **Who this page is for:** anyone who has done the [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) and wants to know every
AstroFind feature. Each item follows the same pattern: **what it is / what it's for / how to do
it**, with a level badge: 🟢 Beginner, 🟡 Intermediate, 🔴 Advanced. Menu paths are given in
English and Portuguese exactly as the program shows them. Unknown words are in the
[Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary). The full list of menus is in [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference); every option is in [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

🇧🇷 **Para quem é esta página:** quem já fez o [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) e quer conhecer todas as funções
do AstroFind. Cada item segue o mesmo padrão: **o que é / para que serve / como fazer**, com um
selo de nível: 🟢 Iniciante, 🟡 Intermediário, 🔴 Avançado. Os caminhos de menu estão em inglês e
português exatamente como o programa mostra. Palavras desconhecidas estão no
[Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary). A lista completa de menus está em [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference); todas as opções
em [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

> 🇬🇧 **Note on language:** some labels appear **in Portuguese even in the English interface**
> (for example "Fluxo automático", "Projetos Recentes", "&Fechar Imagens", "✓ Aceitar"). This
> page quotes them as the program shows them.
>
> 🇧🇷 **Nota sobre idioma:** alguns rótulos aparecem **em português mesmo na interface em
> inglês** (ex.: "Fluxo automático", "Projetos Recentes", "Fechar Imagens", "✓ Aceitar").

## Contents / Conteúdo

| Part / Parte | Items / Itens |
|---|---|
| A. Start and files / Início e arquivos | 1 Setup and Settings · 2 Loading images · 3 Multi-HDU, colour, cubes, spectra · 4 Projects · 5 Image parameters and header |
| B. Viewing / Visualização | 6 Zoom, pan, windows · 7 Display stretch and LUT · 8 Overlays and markings · 9 Histogram, power spectrum, region statistics · 10 Annotate, copy, export |
| C. Preparing / Preparação | 11 Calibration (dark, flat) · 12 Bad pixels and background · 13 Data Reduction · 14 Import external tables |
| D. Finding / Busca | 15 Known objects and catalogs · 16 Blink · 17 Moving Object Detection · 18 Stacking · 19 Compare Sessions |
| E. Measuring / Medição | 20 Measuring and verification · 21 What happens in a measurement · 22 Growth and light curves |
| F. Reporting / Relatório | 23 ADES report and submission |
| G. Interface | 24 Workflow panel and school mode · 25 Themes, language, panels · 26 Help and About |

---

# A. Start and files / Início e arquivos

## 1. Setup Wizard and Settings / Assistente de Configuração e Configurações

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** the first-run wizard and the main options window.
**What it's for:** AstroFind puts your name, station code and telescope into every report, and
needs your location and an astrometry.net key.
**How to do it:**
- **Help → Setup Wizard…** / **Ajuda → Assistente de Configuração…**: 3 pages (Observer
  Identity, Plate-solving API Key, Asteroid Catalog). It opens by itself at start while
  "Show this wizard every time AstroFind starts" is ticked.
- **File → Settings...** (`Ctrl+,`) / **Arquivo → Configurações...**: tabs **Observer, Camera,
  Connections, Detection, Display, Legacy**. **Reset to Defaults** restores recommended values.
  Tabs with a yellow banner are for advanced users. Details: [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

> 🇬🇧 💡 **Units:** pixel scale is in ″/px (arcseconds per pixel); Minimum FWHM is in ″
> (arcseconds); Time Offset is a camera-clock correction in seconds; Time Precision (0–3) is
> the number of decimal places of the seconds in the report. Leave these at their defaults
> unless you know why. In 1.1.0 and earlier some of these labels were wrong (°/px, px, h). See [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

🇧🇷 **Português**
**O que é:** o assistente da primeira execução e a janela principal de opções.
**Para que serve:** o AstroFind coloca seu nome, código de estação e telescópio em todo
relatório, e precisa da sua localização e de uma chave do astrometry.net.
**Como fazer:**
- **Ajuda → Assistente de Configuração…**: 3 páginas (Identidade do observador, Chave de API,
  Catálogo de asteroides). Abre sozinho ao iniciar enquanto "Mostrar este assistente sempre que o
  AstroFind iniciar" estiver marcado.
- **Arquivo → Configurações...** (`Ctrl+,`): abas **Observador, Câmera, Conexões, Detecção,
  Exibição, Legado**. **Restaurar padrões** volta aos valores recomendados. Abas com faixa
  amarela são para usuários avançados. Detalhes: [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

> 🇧🇷 💡 **Unidades:** a escala de pixel é em ″/px (segundos de arco por pixel); o FWHM mínimo
> é em ″ (segundos de arco); o Deslocamento de tempo é uma correção do relógio da câmera em
> segundos; a Precisão de tempo (0–3) é o número de casas decimais dos segundos no relatório.
> Deixe nos padrões, a não ser que saiba o motivo. Na 1.1.0 e anteriores alguns desses rótulos
> estavam errados (°/px, px, h). Veja [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

---

## 2. Loading images / Carregando imagens

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** opening picture files as a **session** (the set of images you work on together).
**What it's for:** every measurement starts from images of one sky field taken minutes apart.
**How to do it:**
1. **File → Load Images...** / **Arquivo → Carregar Imagens...**. Select all frames at once.
2. Or **File → Open Recent...** / **Arquivo → Abrir Recente...**: lists the last 8 *folders*;
   clicking one loads every supported image in it (archives, BMP, JPEG and PDS4 `.xml` are not
   scanned). **Clear Recent** / **Limpar Recentes** empties the list.
3. Or drag files onto the window. Drag and drop only accepts FITS, ZIP and TAR/7Z/RAR files.
   Dropping one file while images are open shows a menu: **Open as Science Image**, **Use as
   Dark Frame**, **Use as Flat Field**, **Cancel**.

Formats (v1.1.0): FITS, SER video (first frame only), XISF (PixInsight), TIFF/PNG (16-bit
read directly), BMP, JPEG, **DSLR RAW** (CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2, PEF and more,
if the program was built with LibRaw), **NASA PDS3** (`.img`, or `.img` + `.lbl`) and **PDS4**
(`.xml` label). ZIP is always unpacked; TAR.GZ/BZ2/XZ, 7Z and RAR need libarchive. Details:
[File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

**What you should see:** one window per image, thumbnails in the Images bar, the notice
"Imagens carregadas" (B&W or colour; untick "Mostrar sempre este aviso" to hide it), and a bar
of ✓/⚠ chips above the images (pixel scale, WCS, UTC date/time, saturation, location). Settings
such as pixel scale and location are filled from the first image's header when they are empty.

> 🇬🇧 ⚠️ **Watch out:**
> - The shortcut is `Ctrl+L`. (In 1.1.0 and earlier it was also given to Tools → Light
>   Curve…, so the key might do nothing; there, use the menu.)
> - Loading while images are open asks **Acréscimo** (add) or **Novo Projeto** (new project).
>   *Novo Projeto* also resets pixel scale, saturation and location (Time Offset is kept).
> - A session holds at most **20 images**. The message says to raise the limit in Settings, but
>   there is no such field.
> - **DSLR RAW:** the time comes from the camera clock and is marked uncertain. The Log warns
>   "DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC…". Check your camera clock was set
>   to UTC. A build without LibRaw refuses RAW files with a clear message.
> - **PDS (mission) images** rarely contain a sky solution: run Data Reduction (item 13).

🇧🇷 **Português**
**O que é:** abrir arquivos de imagem como uma **sessão** (o conjunto de imagens em que você
trabalha junto).
**Para que serve:** toda medição começa com imagens de um campo do céu tiradas com minutos de
intervalo.
**Como fazer:**
1. **Arquivo → Carregar Imagens...**. Selecione todos os quadros de uma vez.
2. Ou **Arquivo → Abrir Recente...**: lista as 8 últimas *pastas*; clicar numa carrega toda imagem
   suportada dela (compactados, BMP, JPEG e `.xml` PDS4 não são lidos). **Limpar Recentes** esvazia
   a lista.
3. Ou arraste os arquivos para a janela. Arrastar só aceita FITS, ZIP e TAR/7Z/RAR. Soltar um
   arquivo com imagens já abertas mostra um menu: **Abrir como imagem científica**, **Usar como
   dark frame**, **Usar como flat field**, **Cancelar**.

Formatos (v1.1.0): FITS, vídeo SER (só o primeiro quadro), XISF (PixInsight), TIFF/PNG (16 bits
lidos direto), BMP, JPEG, **RAW de DSLR** (CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2, PEF e outros,
se o programa foi compilado com LibRaw), **NASA PDS3** (`.img`, ou `.img` + `.lbl`) e **PDS4**
(rótulo `.xml`). ZIP é sempre extraído; TAR.GZ/BZ2/XZ, 7Z e RAR precisam da libarchive. Detalhes:
[File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

**O que você deve ver:** uma janela por imagem, miniaturas na barra Imagens, o aviso "Imagens
carregadas" (PB ou colorida; desmarque "Mostrar sempre este aviso" para esconder) e uma barra de
etiquetas ✓/⚠ acima das imagens (escala de pixel, WCS, data/hora UTC, saturação, localização).
Configurações como escala de pixel e localização são preenchidas a partir do cabeçalho da
primeira imagem quando estão vazias.

> 🇧🇷 ⚠️ **Atenção:**
> - O atalho é `Ctrl+L`. (Na 1.1.0 e anteriores ele também estava em Ferramentas → Curva de
>   Luz…, então a tecla podia não fazer nada; lá, use o menu.)
> - Carregar com imagens abertas pergunta **Acréscimo** (somar) ou **Novo Projeto**. *Novo
>   Projeto* também zera escala de pixel, saturação e localização (o Deslocamento de tempo é
>   mantido).
> - Uma sessão aceita no máximo **20 imagens**. A mensagem diz para aumentar o limite nas
>   Configurações, mas esse campo não existe.
> - **RAW de DSLR:** a hora vem do relógio da câmera e é marcada como incerta. O Registro avisa
>   "DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC…". Confira se o relógio da câmera
>   estava em UTC. Uma versão sem LibRaw recusa arquivos RAW com uma mensagem clara.
> - **Imagens PDS (de missões)** raramente têm solução de céu: rode a Redução de Dados (item 13).

---

## 3. Multi-HDU files, colour, cubes and spectra / Arquivos multi-HDU, cor, cubos e espectros

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** special FITS layouts. An **HDU** (Header/Data Unit) is one block of data inside
a FITS file; a file can hold several.
**What it's for:** opening mission data, colour images and time series without converting them.
**How to do it:** nothing to set, AstroFind detects them:
- **Several image HDUs:** a bar `‹ [n: NAME (W×H)] ›` appears in the image window to switch.
- **Colour:** `NAXIS3 = 3` or 3 HDUs named by colour/filter are shown in colour.
- **Cube** (more than 3 planes): right-click the image → **Animate Cube (N frames)…** /
  **Animar Cubo (N quadros)…** plays the planes like a blink.
- **1-D spectrum** (`NAXIS = 1`): the "Spectrum" window (Channel × Flux) opens by itself.

🇧🇷 **Português**
**O que é:** formatos FITS especiais. Um **HDU** (Header/Data Unit) é um bloco de dados dentro
do arquivo FITS; um arquivo pode ter vários.
**Para que serve:** abrir dados de missões, imagens coloridas e séries temporais sem converter.
**Como fazer:** nada a configurar, o AstroFind detecta:
- **Vários HDUs de imagem:** aparece uma barra `‹ [n: NOME (L×A)] ›` na janela da imagem para
  trocar.
- **Cor:** `NAXIS3 = 3` ou 3 HDUs nomeados por cor/filtro são mostrados coloridos.
- **Cubo** (mais de 3 planos): clique direito na imagem → **Animar Cubo (N quadros)…** mostra os
  planos em sequência, como no piscar.
- **Espectro 1-D** (`NAXIS = 1`): a janela "Espectro" (Canal × Fluxo) abre sozinha.

---

## 4. Projects: save, open, close / Projetos: salvar, abrir, fechar

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** a `.gus` project file stores your session: image paths, sky solutions, detected
stars, known objects, display levels and measurements. **Pixels are not stored**; images are
reloaded from disk (or re-extracted from their archive).
**What it's for:** stopping and continuing later.
**How to do it:**
- **File → Save Project** (`Ctrl+S`) / **Arquivo → Salvar Projeto**; **File → Save Project
  As...** (`Ctrl+Shift+S`) / **Salvar Projeto Como...**. Default folder `~/projects`.
- **File → Open Project...** (`Ctrl+O`) / **Arquivo → Abrir Projeto...**. If an image is missing
  it offers **Localizar…**, **Pular**, **Cancelar tudo**.
- **File → Projetos Recentes**: the last 5 projects.
- **File → Close Project** / **Arquivo → Fechar Projeto**: offers to save, then clears everything.

> 🇬🇧 ⚠️ **Watch out:** **File → Reset Files** (`Ctrl+R`), **File → Fechar Imagens** (`Ctrl+W`)
> and **Windows → Close all Images** clear the whole session **without asking to save**.
> **Windows → Close all Windows** only closes the windows and keeps the session.

🇧🇷 **Português**
**O que é:** um arquivo de projeto `.gus` guarda sua sessão: caminhos das imagens, soluções de
céu, estrelas detectadas, objetos conhecidos, níveis de exibição e medições. **Os pixels não são
guardados**; as imagens são recarregadas do disco (ou extraídas de novo do compactado).
**Para que serve:** parar e continuar depois.
**Como fazer:**
- **Arquivo → Salvar Projeto** (`Ctrl+S`); **Arquivo → Salvar Projeto Como...**
  (`Ctrl+Shift+S`). Pasta padrão `~/projects`.
- **Arquivo → Abrir Projeto...** (`Ctrl+O`). Se faltar uma imagem, oferece **Localizar…**,
  **Pular**, **Cancelar tudo**.
- **Arquivo → Projetos Recentes**: os 5 últimos projetos.
- **Arquivo → Fechar Projeto**: oferece salvar e depois limpa tudo.

> 🇧🇷 ⚠️ **Atenção:** **Arquivo → Resetar Arquivos** (`Ctrl+R`), **Arquivo → Fechar Imagens**
> (`Ctrl+W`) e **Janelas → Fechar todas as Imagens** limpam a sessão inteira **sem perguntar se
> quer salvar**. **Janelas → Fechar todas as Janelas** só fecha as janelas e mantém a sessão.

---

## 5. Image parameters and FITS header / Parâmetros da imagem e cabeçalho FITS

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** the **header** is the text part of a FITS file (date, exposure, telescope…).
**What it's for:** checking or fixing wrong metadata (for example a missing time or pixel scale)
before measuring.
**How to do it:**
- **Images → Display Header...** / **Imagens → Exibir Cabeçalho...** (or right-click →
  **FITS Header…** / **Cabeçalho FITS…**): read-only. It shows nothing for files that are not
  FITS (RAW, PDS3, SER…).
- **Images → Edit Image Parameters...** / **Imagens → Editar Parâmetros da Imagem...**: object
  name, filter, observer, telescope, RA/Dec, Julian Date of mid-exposure, exposure time, plate
  scale, gain, saturation.

🇧🇷 **Português**
**O que é:** o **cabeçalho** é a parte de texto do arquivo FITS (data, exposição, telescópio…).
**Para que serve:** conferir ou corrigir metadados errados (ex.: hora ou escala de pixel
faltando) antes de medir.
**Como fazer:**
- **Imagens → Exibir Cabeçalho...** (ou clique direito → **Cabeçalho FITS…**): só leitura. Não
  mostra nada para arquivos que não são FITS (RAW, PDS3, SER…).
- **Imagens → Editar Parâmetros da Imagem...**: nome do objeto, filtro, observador, telescópio,
  AR/Dec, Data Juliana do meio da exposição, tempo de exposição, escala de placa, ganho,
  saturação.

---

# B. Viewing / Visualização

## 6. Zoom, pan, flip, magnifier and window layout / Zoom, arrastar, espelhar, lupa e janelas

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** moving around the image.
**What it's for:** looking closely at a faint dot or seeing the whole field.
**How to do it:**
- Mouse wheel zooms around the cursor. Drag with the left or middle button to move (pan).
  Arrow keys also pan.
- **Images → Zoom In** (`Ctrl++`, `+`) / **Imagens → Ampliar Mais**; **Zoom Out** (`Ctrl+-`, `-`)
  / **Ampliar Menos**; **Fit Window Size** (`Ctrl+F`, `0`) / **Ajustar ao Tamanho da Janela**.
- **Images → Invert Display** (`Ctrl+I`) / **Inverter Exibição**: black stars on white.
- **Images → Flip Horizontal / Flip Vertical** / **Espelhar na Horizontal / na Vertical**: mirror
  the view, to match a chart with another orientation.
- **Images → Magnifying Glass** (`Ctrl+G`) / **Imagens → Lupa**: an enlarged view under the
  cursor.
- **Windows → Tile Windows / Cascade Windows / Arrange all Windows** / **Janelas → Organizar em
  Grade / Cascata de Janelas / Organizar todas as Janelas**. Click a thumbnail in the Images bar
  to bring that image forward.
- The status bar shows the pixel value and RA/Dec (decimal degrees) under the cursor.

🇧🇷 **Português**
**O que é:** navegar pela imagem.
**Para que serve:** olhar de perto um ponto fraco ou ver o campo inteiro.
**Como fazer:**
- A roda do mouse dá zoom em torno do cursor. Arraste com o botão esquerdo ou do meio para mover.
  As setas do teclado também movem.
- **Imagens → Ampliar Mais** (`Ctrl++`, `+`); **Ampliar Menos** (`Ctrl+-`, `-`); **Ajustar ao
  Tamanho da Janela** (`Ctrl+F`, `0`).
- **Imagens → Inverter Exibição** (`Ctrl+I`): estrelas pretas em fundo branco.
- **Imagens → Espelhar na Horizontal / Espelhar na Vertical**: espelham a visualização, para
  bater com uma carta de outra orientação.
- **Imagens → Lupa** (`Ctrl+G`): uma visão ampliada sob o cursor.
- **Janelas → Organizar em Grade / Cascata de Janelas / Organizar todas as Janelas**. Clique numa
  miniatura na barra Imagens para trazer aquela imagem para frente.
- A barra de status mostra o valor do pixel e AR/Dec (graus decimais) sob o cursor.

---

## 7. Display stretch and false colour / Ajuste de exibição e cor falsa

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** how pixel numbers become screen brightness. A **transfer function** is the curve
used; a **LUT** (look-up table) paints brightness with colours.
**What it's for:** making faint stars and asteroids visible. It never changes the data used for
measurement.
**How to do it:** **Images → Background and Range...** / **Imagens → Fundo e Intervalo...** (or
the Display toolbar). In the window:
- Sliders **Background (min)** / **Fundo (mín)** and **Range (max)** / **Intervalo (máx)**, with
  a histogram preview.
- **⟳ Auto**: automatic levels from the sky noise.
- "Função de transferência": Linear, Logarítmica, Raiz Quadrada (Sqrt), Asinh, Equalização de
  Histograma.
- "Cor falsa (LUT)": Cinza (Grayscale), Quente (Hot), Frio (Cool), Viridis.
- **Apply to all images in session** / **Aplicar a todas as imagens da sessão**.

Pixels with no value (**NaN**) are always drawn in **magenta**.

🇧🇷 **Português**
**O que é:** como os números dos pixels viram brilho na tela. A **função de transferência** é a
curva usada; uma **LUT** (tabela de cores) pinta o brilho com cores.
**Para que serve:** deixar visíveis estrelas e asteroides fracos. Nunca muda os dados usados na
medição.
**Como fazer:** **Imagens → Fundo e Intervalo...** (ou a barra Exibição). Na janela:
- Controles **Fundo (mín)** e **Intervalo (máx)**, com prévia do histograma.
- **⟳ Auto**: níveis automáticos a partir do ruído do céu.
- "Função de transferência": Linear, Logarítmica, Raiz Quadrada (Sqrt), Asinh, Equalização de
  Histograma.
- "Cor falsa (LUT)": Cinza (Grayscale), Quente (Hot), Frio (Cool), Viridis.
- **Aplicar a todas as imagens da sessão**.

Pixels sem valor (**NaN**) são sempre desenhados em **magenta**.

---

## 8. Overlays and markings / Sobreposições e marcações

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** symbols drawn over the image. Cyan circle = detected star; orange ellipse =
**streak** (a trail); magenta double circle = **blended** source (two overlapping stars); yellow
cross = catalog reference star; green circle = known asteroid (light blue = planet, orange =
comet); "Cand #N" = automatic candidate; red = selected item.
**What it's for:** seeing at a glance what AstroFind found.
**How to do it:**
- **Images → Select Markings...** / **Imagens → Selecionar Marcações...**: choose which types to
  show (detected stars, catalog stars, known objects, object labels). Applies to all windows.
- **Tools → Ecliptic / Galactic Overlay** (`Ctrl+E`) / **Ferramentas → Sobreposição Eclíptica /
  Galáctica**: draws the **ecliptic** (the Sun's path, where most asteroids are) and the plane of
  the Milky Way. If the field is within 15° of the Milky Way, a badge warns
  "Plano galáctico — alta extinção interestelar": many stars and dust make the work harder.
- **Edit → Clear All Markings** / **Editar → Limpar Todas as Marcações**: removes all
  measurements and text notes.

🇧🇷 **Português**
**O que é:** símbolos desenhados sobre a imagem. Círculo ciano = estrela detectada; elipse
laranja = **traço** (rastro); círculo duplo magenta = fonte **blendada** (duas estrelas
sobrepostas); cruz amarela = estrela de referência do catálogo; círculo verde = asteroide conhecido
(azul-claro = planeta, laranja = cometa); "Cand #N" = candidato automático; vermelho = item
selecionado.
**Para que serve:** ver de relance o que o AstroFind achou.
**Como fazer:**
- **Imagens → Selecionar Marcações...**: escolha que tipos mostrar (estrelas detectadas, estrelas
  do catálogo, objetos conhecidos, rótulos). Vale para todas as janelas.
- **Ferramentas → Sobreposição Eclíptica / Galáctica** (`Ctrl+E`): desenha a **eclíptica** (o
  caminho do Sol, onde está a maioria dos asteroides) e o plano da Via Láctea. Se o campo estiver
  a menos de 15° da Via Láctea, um selo avisa "Plano galáctico — alta extinção interestelar":
  muitas estrelas e poeira dificultam o trabalho.
- **Editar → Limpar Todas as Marcações**: apaga todas as medições e anotações de texto.

> 🇬🇧 ⚠️ **Watch out:** Clear All Markings also empties the Observations table. /
> 🇧🇷 ⚠️ **Atenção:** Limpar Todas as Marcações também esvazia a tabela de Observações.

---

## 9. Histogram, power spectrum and region statistics / Histograma, espectro de potência e estatísticas de região

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** three diagnostic tools. The **histogram** counts pixels per brightness and fits
a bell curve to the sky (giving sky level μ and noise σ). The **power spectrum** (a 2-D FFT, a
maths tool that finds repeating patterns) shows periodic noise. **Region statistics** gives
numbers for a rectangle you draw.
**What it's for:** checking noise, banding from the camera readout, and focus quality.
**How to do it:**
- Right-click the image → **Show Histogram** / **Exibir Histograma**.
- Right-click → **Show Power Spectrum** / **Mostrar Espectro de Potência** (large images are
  reduced to 1024 px for speed).
- **Images → Region Statistics** (`Ctrl+Shift+R`) / **Imagens → Estatísticas de Região**: drag a
  rectangle; `Esc` cancels.

🇧🇷 **Português**
**O que é:** três ferramentas de diagnóstico. O **histograma** conta pixels por brilho e ajusta
uma curva de sino ao céu (dando o nível do céu μ e o ruído σ). O **espectro de potência** (FFT
2-D, uma ferramenta matemática que acha padrões repetidos) mostra ruído periódico. As
**estatísticas de região** dão números de um retângulo que você desenha.
**Para que serve:** conferir ruído, faixas causadas pela leitura da câmera e qualidade do foco.
**Como fazer:**
- Clique direito na imagem → **Exibir Histograma**.
- Clique direito → **Mostrar Espectro de Potência** (imagens grandes são reduzidas a 1024 px para
  ficar rápido).
- **Imagens → Estatísticas de Região** (`Ctrl+Shift+R`): arraste um retângulo; `Esc` cancela.

---

## 10. Annotate, copy and export images / Anotar, copiar e exportar imagens

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** text labels on the image, and saving the image as a picture or FITS.
**What it's for:** presentations, school work, sharing with other programs.
**How to do it:**
- **Edit → Annotate Tool** (`N`) / **Editar → Ferramenta Anotação**: click, type the label text;
  it is drawn in magenta. **Edit → Select Tool** (`S`) / **Ferramenta Seleção** returns to normal.
- **Edit → Copy Image** (`Ctrl+C`) / **Editar → Copiar Imagem**: copies the image window to the
  clipboard.
- **File → Export Image to...** / **Arquivo → Exportar Imagem como...** → **JPEG / PNG / BMP…**,
  **JPEG (quick)…** or **PNG…** (also right-click → **Export as JPEG/PNG…**). Saves the image as
  displayed.
- **File → Save as FITS...** / **Salvar como FITS...** and **File → Save all FITS** / **Salvar
  todos como FITS**: write copies named `<name>_wcs.fits`, including the sky solution (WCS) when
  the image is solved. Use this to reuse the solution in other programs.

🇧🇷 **Português**
**O que é:** rótulos de texto sobre a imagem, e salvar a imagem como figura ou FITS.
**Para que serve:** apresentações, trabalhos escolares, compartilhar com outros programas.
**Como fazer:**
- **Editar → Ferramenta Anotação** (`N`): clique e digite o texto; ele é desenhado em magenta.
  **Editar → Ferramenta Seleção** (`S`) volta ao normal.
- **Editar → Copiar Imagem** (`Ctrl+C`): copia a janela da imagem para a área de transferência.
- **Arquivo → Exportar Imagem como...** → **JPEG / PNG / BMP…**, **JPEG (rápido)…** ou **PNG…**
  (também clique direito → **Exportar como JPEG/PNG…**). Salva a imagem como está na tela.
- **Arquivo → Salvar como FITS...** e **Arquivo → Salvar todos como FITS**: gravam cópias
  chamadas `<nome>_wcs.fits`, com a solução de céu (WCS) quando a imagem está resolvida. Use para
  aproveitar a solução em outros programas.

---

# C. Preparing / Preparação

## 11. Calibration: dark frame and flat field / Calibração: dark frame e flat field

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** removing camera defects.
- **Dark frame:** an exposure with the shutter closed; it records the heat noise of the sensor.
  It is **subtracted**.
- **Flat field:** an image of an evenly lit surface; it records dust shadows and darker corners.
  The image is **divided** by it.
- A **master** frame is several darks (or flats) combined into one, which lowers noise.

AstroFind has no separate bias frame step.
**What it's for:** without calibration, defects can look like stars or spoil brightness
measurements. Skip it if your images are already calibrated (common for campaign or mission
data).
**How to do it:**
- **File → Load Dark Frame...** / **Arquivo → Carregar Dark Frame...** and **File → Load Flat
  Field...** / **Carregar Flat Field...** (FITS only). The **Calibration** panel appears.
- **File → Calibration Wizard…** / **Arquivo → Assistente de Calibração…**: Step 1 Dark, Step 2
  Flat, Step 3 Apply. If "Apply calibration to currently loaded images now" is unticked, the frames
  are applied at the next Data Reduction.
- In the **Calibration** panel: **Load…**, **Build Master…** (combine ≥ 2 frames by Average or
  Median), **Clear**, and **Apply to All Images**.
- Right-click one image → **Apply Dark Frame…** / **Apply Flat Field…** for that image only.

> 🇬🇧 ⚠️ **Watch out:** darks and flats must have **the same size** as the images, or they are
> skipped (the Log says "dark frame size mismatch — skipped").

🇧🇷 **Português**
**O que é:** remover defeitos da câmera.
- **Dark frame:** exposição com o obturador fechado; registra o ruído de calor do sensor. É
  **subtraído**.
- **Flat field:** imagem de uma superfície iluminada por igual; registra sombras de poeira e
  cantos mais escuros. A imagem é **dividida** por ele.
- Um frame **mestre** (master) é a combinação de vários darks (ou flats) num só, o que reduz o
  ruído.

O AstroFind não tem uma etapa separada de bias.
**Para que serve:** sem calibração, defeitos podem parecer estrelas ou estragar a medição de
brilho. Pule se suas imagens já vêm calibradas (comum em dados de campanha ou de missões).
**Como fazer:**
- **Arquivo → Carregar Dark Frame...** e **Arquivo → Carregar Flat Field...** (só FITS). O painel
  **Calibração** aparece.
- **Arquivo → Assistente de Calibração…**: Etapa 1 Dark, Etapa 2 Flat, Etapa 3 Aplicar. Se
  "Aplicar calibração às imagens carregadas agora" estiver desmarcado, os frames são aplicados na
  próxima Redução de Dados.
- No painel **Calibração**: **Carregar…**, **Construir Master…** (combina ≥ 2 frames por média ou
  mediana), **Limpar** e **Aplicar a Todas as Imagens**.
- Clique direito numa imagem → **Aplicar Dark Frame…** / **Aplicar Flat Field…** só naquela.

> 🇧🇷 ⚠️ **Atenção:** darks e flats precisam ter **o mesmo tamanho** das imagens, senão são
> ignorados (o Registro diz "dark frame size mismatch — skipped").

---

## 12. Bad pixels and sky background / Pixels ruins e fundo do céu

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** two clean-up steps inside Data Reduction. **Bad-pixel correction** replaces
"hot" pixels (stuck bright) with the median of their neighbours. **Background subtraction**
removes a smooth sky glow or gradient.
**What it's for:** hot pixels can be detected as false stars or false moving objects; gradients
confuse the detection threshold.
**How to do it:** in **File → Settings... → Camera**, "Corrigir pixels ruins automaticamente" is
**on** by default (threshold 5 σ). In **Settings → Detection**, "Subtrair modelo de fundo antes
da detecção" is **off** by default; turn it on for images with strong gradients (moonlight, city
light).

🇧🇷 **Português**
**O que é:** duas etapas de limpeza dentro da Redução de Dados. A **correção de pixels ruins**
troca pixels "quentes" (presos acesos) pela mediana dos vizinhos. A **subtração de fundo** remove
um brilho suave ou gradiente do céu.
**Para que serve:** pixels quentes podem ser detectados como estrelas falsas ou objetos em
movimento falsos; gradientes confundem o limiar de detecção.
**Como fazer:** em **Arquivo → Configurações... → Câmera**, "Corrigir pixels ruins
automaticamente" vem **ligado** (limiar 5 σ). Em **Configurações → Detecção**, "Subtrair modelo
de fundo antes da detecção" vem **desligado**; ligue para imagens com gradiente forte (Lua, luz
da cidade).

---

## 13. Data Reduction: star detection and plate solving / Redução de Dados: detecção de estrelas e plate solving

🟢 Beginner / Iniciante (🟡 for the options / 🟡 nas opções)

🇬🇧 **English**
**What it is:** the main processing step. In order: calibration, bad pixels, background, **star
detection** (with the SEP library, up to 500 brightest stars per image, also marking streaks and
blended sources), then **plate solving**: matching the stars to the sky to get a **WCS** (the
formula from pixel to RA/Dec).
**What it's for:** without a WCS no sky position can be measured.
**How to do it:**
1. **Astrometry → Data Reduction...** (`Ctrl+A`) / **Astrometria → Redução de Dados...**.
2. To cancel: **Astrometry → Stop Data Reduction** (`Ctrl+.`) / **Parar Redução de Dados**.
3. Choose the solver in **Settings → Connections → Backend**: "astrometry.net (online)" (default,
   needs a free API key) or "ASTAP (local, offline)" (needs the separate ASTAP program and its
   path in "Executável ASTAP").

- Images that already have a WCS in the header are skipped ("WCS pré-existente — plate solving
  ignorado").
- If the location is not set, a warning "Localização não configurada" offers to open Settings.
- "Save WCS back to FITS file after plate solve" (Settings → Connections) writes the solution
  **into your original file**. Off by default. File → Save as FITS... is the safer option.
- Supported sky projections read from the header: TAN, SIN, ARC, STG, CAR, MER, GLS/SFL, AIT.
- Detection options (threshold, streak elongation, minimum SNR): **Settings → Detection**.

**What you should see:** "Image N: X stars found" (plus "streak/trail candidate(s) — shown in
orange" if any), "Solved! RA=… Dec=…", and "Redução concluída: …". The **Image Catalog** panel
opens; click a row to highlight that star in red on every image (`Esc` clears).

🇧🇷 **Português**
**O que é:** a etapa principal de processamento. Em ordem: calibração, pixels ruins, fundo,
**detecção de estrelas** (com a biblioteca SEP, até as 500 estrelas mais brilhantes por imagem,
marcando também traços e fontes blendadas) e depois **plate solving**: casar as estrelas com o céu
para obter um **WCS** (a fórmula de pixel para AR/Dec).
**Para que serve:** sem WCS não dá para medir posição no céu.
**Como fazer:**
1. **Astrometria → Redução de Dados...** (`Ctrl+A`).
2. Para cancelar: **Astrometria → Parar Redução de Dados** (`Ctrl+.`).
3. Escolha o resolvedor em **Configurações → Conexões → Backend**: "astrometry.net (online)"
   (padrão, precisa de chave de API gratuita) ou "ASTAP (local, offline)" (precisa do programa
   ASTAP à parte e do caminho em "Executável ASTAP").

- Imagens que já têm WCS no cabeçalho são puladas ("WCS pré-existente — plate solving ignorado").
- Se a localização não estiver configurada, o aviso "Localização não configurada" oferece abrir
  as Configurações.
- "Salvar WCS no arquivo FITS após solução de campo" (Configurações → Conexões) grava a solução
  **no seu arquivo original**. Desligado por padrão. Arquivo → Salvar como FITS... é mais seguro.
- Projeções de céu lidas do cabeçalho: TAN, SIN, ARC, STG, CAR, MER, GLS/SFL, AIT.
- Opções de detecção (limiar, alongamento de traço, SNR mínimo): **Configurações → Detecção**.

**O que você deve ver:** "Image N: X stars found" (mais "streak/trail candidate(s) — shown in
orange" se houver), "Solved! RA=… Dec=…" e "Redução concluída: …". O painel **Catálogo de
Imagem** abre; clique numa linha para destacar a estrela em vermelho em todas as imagens (`Esc`
limpa).

---

## 14. Import external star tables / Importar tabelas de estrelas externas

🔴 Advanced / Avançado

🇬🇧 **English**
**What it is:** reading star lists made by other software, stored as a FITS **BINTABLE** (a table
inside a FITS file).
**What it's for:** reusing a DAOPHOT, SExtractor, IRAF or Astropy reduction instead of AstroFind's
own detection.
**How to do it:** with an image active, **Astrometry → Import Detected Stars
(DAOPHOT/SExtractor)…** or **Astrometry → Import Reduction Table (IRAF/Astropy)…** /
**Astrometria → Importar Estrelas Detectadas (DAOPHOT/SExtractor)…** / **Importar Tabela de
Redução (IRAF/Astropy)…**. The stars replace the active image's detected stars; rows with RA/Dec
count as matched.

🇧🇷 **Português**
**O que é:** ler listas de estrelas feitas por outros programas, guardadas como FITS
**BINTABLE** (uma tabela dentro de um arquivo FITS).
**Para que serve:** reaproveitar uma redução do DAOPHOT, SExtractor, IRAF ou Astropy no lugar da
detecção do AstroFind.
**Como fazer:** com uma imagem ativa, **Astrometria → Importar Estrelas Detectadas
(DAOPHOT/SExtractor)…** ou **Astrometria → Importar Tabela de Redução (IRAF/Astropy)…**. As
estrelas substituem as detectadas da imagem ativa; linhas com AR/Dec contam como casadas.

---

# D. Finding / Busca

## 15. Known objects, reference stars and ephemerides / Objetos conhecidos, estrelas de referência e efemérides

🟢 Beginner / Iniciante (🟡 for MPCOrb and Horizons / 🟡 em MPCOrb e Horizons)

🇬🇧 **English**
**What it is:** **Tools → Known Object Overlay** (`Ctrl+K`) / **Ferramentas → Sobreposição de
Objetos Conhecidos** does two things for the field:
- downloads **reference stars** from a **catalog** (a precise star list): UCAC4 (default) or
  Gaia DR3 from VizieR, or a local FITS catalog for offline use. They are used for the position
  and magnitude of your measurements, and give the "WCS RMS" (accuracy) in the Log;
- asks the **SkyBoT** service which **known** asteroids and comets are in the field. If SkyBoT
  cannot be reached and `MPCORB.DAT` is present, AstroFind computes them itself.

**What it's for:** telling a new object from a known one, and calibrating positions.
**How to do it:**
- Run Ctrl+K after Data Reduction. Catalog choice: **Settings → Connections** ("Catalog",
  "Source", "Local catalog"). Magnitude limits: **Settings → Detection**.
- **Internet → Download MPCOrb** / **Internet → Baixar MPCOrb**: the full asteroid list
  (about 200 MB; the status tip says ~500 MB, the confirmation dialog 200 MB). **Internet →
  Update MPCOrb** / **Atualizar MPCOrb** adds new objects from the daily file. **File → Reload
  MPCOrb** / **Arquivo → Recarregar MPCOrb** reloads the file from disk.
- **Internet → Query Horizons…** / **Internet → Consultar Horizons…**: type a target (e.g. `433`,
  `Eros`, `2023 DW`), click **Query**, then **Add to Overlay** to mark it on every image.
- **Internet → Minor Planet Center…** opens the MPC website.

> 🇬🇧 ⚠️ **Watch out:** the Horizons position is **geocentric** (seen from Earth's centre; your
> site is not sent), so it can differ by a few arcseconds for nearby objects. In the Image Catalog,
> the tooltip of "Estrela Guia" always says UCAC4, even when Gaia DR3 is selected.

🇧🇷 **Português**
**O que é:** **Ferramentas → Sobreposição de Objetos Conhecidos** (`Ctrl+K`) faz duas coisas no
campo:
- baixa **estrelas de referência** de um **catálogo** (lista precisa de estrelas): UCAC4 (padrão)
  ou Gaia DR3 do VizieR, ou um catálogo FITS local para uso offline. Elas servem para a posição e
  a magnitude das suas medições, e dão o "WCS RMS" (precisão) no Registro;
- pergunta ao serviço **SkyBoT** quais asteroides e cometas **conhecidos** estão no campo. Se o
  SkyBoT não responder e o `MPCORB.DAT` existir, o AstroFind calcula sozinho.

**Para que serve:** distinguir um objeto novo de um conhecido, e calibrar as posições.
**Como fazer:**
- Rode Ctrl+K depois da Redução de Dados. Escolha do catálogo: **Configurações → Conexões**
  ("Catálogo", "Fonte", "Catálogo local"). Limites de magnitude: **Configurações → Detecção**.
- **Internet → Baixar MPCOrb**: a lista completa de asteroides (cerca de 200 MB; a dica da barra
  de status diz ~500 MB, a confirmação diz 200 MB). **Internet → Atualizar MPCOrb** acrescenta
  objetos novos do arquivo diário. **Arquivo → Recarregar MPCOrb** relê o arquivo do disco.
- **Internet → Consultar Horizons…**: digite um alvo (ex.: `433`, `Eros`, `2023 DW`), clique em
  **Consultar** e depois em **Adicionar à Sobreposição** para marcá-lo em todas as imagens.
- **Internet → Minor Planet Center…** abre o site do MPC.

> 🇧🇷 ⚠️ **Atenção:** a posição do Horizons é **geocêntrica** (vista do centro da Terra; seu local
> não é enviado), então pode diferir alguns segundos de arco para objetos próximos. No Catálogo de
> Imagem, a dica de "Estrela Guia" sempre diz UCAC4, mesmo com Gaia DR3 selecionado.

---

## 16. Blink / Piscar

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** showing the images one after another in the same place.
**What it's for:** the eye easily spots a dot that jumps while stars stay still. This is the
classic way to find asteroids.
**How to do it:** **Tools → Blink Images** (`Ctrl+B`) / **Ferramentas → Piscar Imagens** (needs ≥ 2
images). **Tools → Stop Blinking** (`Ctrl+F9`) / **Parar Piscar** closes it.
- In the Blink View: `Space` play/pause, `←`/`→` previous/next, `Esc` stop, `+`/`-`/`0` zoom.
- Bottom bar: `◀ ⏸ ▶`, **■ Stop**, **Sharpen** (Off → USM → LoG: unsharp mask and Laplacian
  filters that make faint dots stand out), **Speed** slider (50–2000 ms), and 4 thumbnails.
- Blink toolbar: interval box (×0.1 s), step back (`Shift+Ctrl+F10`), step forward
  (`Ctrl+F10`), play (`Ctrl+F11`). Default interval: **Settings → Display → Blink interval**.

> 🇬🇧 ⚠️ **Watch out:** the Blink View shows at most **4** images.

🇧🇷 **Português**
**O que é:** mostrar as imagens uma depois da outra no mesmo lugar.
**Para que serve:** o olho percebe fácil um ponto que pula enquanto as estrelas ficam paradas. É
o jeito clássico de achar asteroides.
**Como fazer:** **Ferramentas → Piscar Imagens** (`Ctrl+B`) (precisa de ≥ 2 imagens).
**Ferramentas → Parar Piscar** (`Ctrl+F9`) fecha.
- Na Visão de piscar: `Espaço` toca/pausa, `←`/`→` anterior/seguinte, `Esc` para, `+`/`-`/`0`
  zoom.
- Barra de baixo: `◀ ⏸ ▶`, **■ Parar**, **Nitidez** (Desativado → USM → LoG: filtros de máscara
  de nitidez e laplaciano que destacam pontos fracos), controle **Speed** (50–2000 ms) e 4
  miniaturas.
- Barra Piscar: caixa de intervalo (×0,1 s), voltar (`Shift+Ctrl+F10`), avançar (`Ctrl+F10`),
  tocar (`Ctrl+F11`). Intervalo padrão: **Configurações → Exibição → Intervalo de piscar**.

> 🇧🇷 ⚠️ **Atenção:** a Visão de piscar mostra no máximo **4** imagens.

---

## 17. Moving Object Detection (MOD) / Detecção de Objetos em Movimento (MOD)

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** an automatic search. It links detected sources that move in a straight line at a
steady rate across the frames (a **tracklet**).
**What it's for:** finding faint or easy-to-miss candidates in many images.
**How to do it:** after Data Reduction, **Astrometry → Moving Object Detection...** (`Ctrl+M`) /
**Astrometria → Detecção de Objetos em Movimento...**. There is no options dialog: it keeps
tracks seen in ≥ 3 frames with SNR ≥ "MOD min. SNR" (**Settings → Detection**, default 5). A box
lists the candidates (motion dx, dy in px/frame, SNR, frames) and they are marked "Cand #N".
Always **confirm a candidate by blinking** before measuring it.

🇧🇷 **Português**
**O que é:** uma busca automática. Liga fontes detectadas que se movem em linha reta, em ritmo
constante, entre os quadros (um **tracklet**).
**Para que serve:** achar candidatos fracos ou fáceis de perder em muitas imagens.
**Como fazer:** depois da Redução de Dados, **Astrometria → Detecção de Objetos em
Movimento...** (`Ctrl+M`). Não há janela de opções: ele mantém trilhas vistas em ≥ 3 quadros com
SNR ≥ "MOD SNR mín." (**Configurações → Detecção**, padrão 5). Uma caixa lista os candidatos
(movimento dx, dy em px/quadro, SNR, quadros) e eles são marcados "Cand #N". Sempre **confirme um
candidato piscando** antes de medir.

---

## 18. Stacking and Track & Stack / Empilhamento e Track & Stack

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** combining several images into one (**Average**, **Median** or **Add**).
**Track & Stack** shifts each frame by the object's motion, so a moving asteroid adds up into
one sharp dot while the stars trail.
**What it's for:** more signal than noise (**SNR**), so objects too faint for one image appear.
**How to do it:**
- **Images → Re-Stack Images** / **Imagens → Re-empilhar Imagens**: choose the method. Images are
  aligned on their stars (or by FFT if stars are missing). Opens `stacked.fits`.
- **Astrometry → Stack Images...** (`Ctrl+T`) / **Astrometria → Empilhar Imagens...**: this is
  Track & Stack. Type the motion **dX/frame** and **dY/frame** in pixels (e.g. from a MOD
  candidate or Horizons), then the method. Opens `track_stacked.fits`.

🇧🇷 **Português**
**O que é:** combinar várias imagens numa só (**Média**, **Mediana** ou **Soma**). O **Track &
Stack** desloca cada quadro pelo movimento do objeto, então um asteroide em movimento se soma num
ponto nítido enquanto as estrelas viram rastros.
**Para que serve:** mais sinal que ruído (**SNR**), revelando objetos fracos demais para uma
imagem só.
**Como fazer:**
- **Imagens → Re-empilhar Imagens**: escolha o método. As imagens são alinhadas pelas estrelas
  (ou por FFT se faltarem estrelas). Abre `stacked.fits`.
- **Astrometria → Empilhar Imagens...** (`Ctrl+T`): este é o Track & Stack. Digite o movimento
  **dX/frame** e **dY/frame** em pixels (ex.: de um candidato do MOD ou do Horizons) e depois o
  método. Abre `track_stacked.fits`.

---

## 19. Compare Sessions / Comparar Sessões

🔴 Advanced / Avançado

🇬🇧 **English**
**What it is:** subtracting a reference image of the same field from your images; what remains
(the **residual**) is what changed.
**What it's for:** finding new or moving objects against an older image.
**How to do it:** **Tools → Compare Sessions…** (`Ctrl+Shift+C`) / **Ferramentas → Comparar
Sessões…**: pick a reference FITS file or folder (**Browse…**), set the detection threshold,
click **Compare**. A "Residual" window opens and the result line gives the number of residual
sources and the method ("WCS warp" or "direct subtraction").

🇧🇷 **Português**
**O que é:** subtrair das suas imagens uma imagem de referência do mesmo campo; o que sobra (o
**resíduo**) é o que mudou.
**Para que serve:** achar objetos novos ou em movimento comparando com uma imagem mais antiga.
**Como fazer:** **Ferramentas → Comparar Sessões…** (`Ctrl+Shift+C`): escolha um FITS ou pasta de
referência (**Browse…**), ajuste o limiar de detecção e clique em **Compare**. Abre uma janela
"Residual" e a linha de resultado dá o número de fontes residuais e o método ("WCS warp" ou
"direct subtraction").

---

# E. Measuring / Medição

## 20. Measuring and the Verification window / Medindo e a janela de Verificação

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** clicking an object to record its position, time and brightness: one
**observation**.
**What it's for:** observations are what goes into the report.
**How to do it:**
1. Press **`M`** (toolbar **Measure Object**, turns orange) or **Edit → Aperture Tool** (`A`) /
   **Editar → Ferramenta Abertura**. You can also click in the Blink View; if it is playing,
   choose **Parar e Medir** or **Continuar Blink**.
2. Click the object. The **Verification** window shows a 4× zoom, RA, Dec, Mag (band), FWHM, SNR
   and a field **Object** for the name. "Objetos conhecidos próximos" lists nearby known objects
   (double-click to use the name); if a known object is within 10 px the name is filled in.
3. **✓ Aceitar** (window stays open), **✓ Aceitar e Fechar**, or **✗ Rejeitar**.
4. Repeat on every image.

The **Observations** panel (also **Windows → View Observations** or **File → View Photometry
File**) lists `#`, Object, RA, Dec, Mag, Band, FWHM", SNR, Airmass, JD, Image. Double-click
**Object** or **Band** to edit them. **Edit → Undo** (`Ctrl+Z`) / **Redo** (`Ctrl+Shift+Z`)
work on accepted measurements.

> 🇬🇧 ⚠️ **Watch out:** the **Remove** button in the Observations panel cannot be undone.
> "File → View Photometry File" does not open a file: it only shows this panel.

🇧🇷 **Português**
**O que é:** clicar num objeto para registrar posição, horário e brilho: uma **observação**.
**Para que serve:** as observações são o que entra no relatório.
**Como fazer:**
1. Aperte **`M`** (botão **Measure Object** da barra, fica laranja) ou **Editar → Ferramenta
   Abertura** (`A`). Também dá para clicar na Visão de piscar; se estiver tocando, escolha **Parar
   e Medir** ou **Continuar Blink**.
2. Clique no objeto. A janela **Verificação** mostra um zoom 4×, AR, Dec, Mag (banda), FWHM, SNR e
   um campo **Object** para o nome. "Objetos conhecidos próximos" lista objetos conhecidos por
   perto (duplo clique para usar o nome); se houver um objeto conhecido a até 10 px, o nome já vem
   preenchido.
3. **✓ Aceitar** (a janela fica aberta), **✓ Aceitar e Fechar** ou **✗ Rejeitar**.
4. Repita em todas as imagens.

O painel **Observações** (também **Janelas → Exibir observações** ou **Arquivo → Ver Arquivo de
Fotometria**) lista `#`, Object, RA, Dec, Mag, Band, FWHM", SNR, Airmass, JD, Image. Dê duplo
clique em **Object** ou **Band** para editar. **Editar → Desfazer** (`Ctrl+Z`) / **Refazer**
(`Ctrl+Shift+Z`) valem para medições aceitas.

> 🇧🇷 ⚠️ **Atenção:** o botão **Remover** do painel Observações não pode ser desfeito. "Arquivo →
> Ver Arquivo de Fotometria" não abre arquivo: só mostra este painel. Os títulos das colunas
> ficam em inglês.

---

## 21. What happens during a measurement / O que acontece numa medição

🔴 Advanced / Avançado

🇬🇧 **English**
**What it is:** the automatic steps after your click. Nothing to set, but the **Log** shows each
one.
1. **Centroid:** an elliptical Gaussian **PSF** (the shape of a star's light) is fitted
   (Levenberg–Marquardt method) to find the exact centre.
2. **Sky position** from the WCS.
3. **Atmospheric refraction** (the air bends light, about 1-2' at 30° altitude, more near the
   horizon) is corrected only for a ground-based position that did **not** already come from a
   catalog plate solution: such a solution is refracted the same way as its reference stars, so
   it already absorbs the correction, and applying it again would double it. In practice this
   means the correction does not run on a typical measured position today, since those come from a
   plate solution; it is also always skipped for space telescopes and below 1° altitude.
4. **Annual aberration** is only logged: the plate solution against the catalog already includes
   it. Precession and nutation are not applied either, because the catalog stars already give
   ICRF (J2000) coordinates, which is what the report uses (`sys=ICRF`).
5. **PSF shape:** FWHM is logged; if the star is elongated, the Log gives both axes and angle,
   and above 1.5 warns "check tracking, focus, or coma".
6. **Aperture photometry:** light inside a circle of radius max(3, 2×FWHM) px, minus the sky
   measured in a ring around it. Fixed radius: **Settings → Detection → Aperture Radius**.
7. **Magnitude** from nearby catalog stars (differential zero-point). If none match, the Log says
   "using instrumental mag".
8. **Airmass** (how much air the light crossed) is always computed; the extinction correction
   runs only if "Extinction coeff k" > 0.

Formulas: `docs/technical-reference.md` in the repository, and [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced).

🇧🇷 **Português**
**O que é:** os passos automáticos depois do seu clique. Nada a configurar, mas o **Registro**
mostra cada um.
1. **Centroide:** uma **PSF** gaussiana elíptica (a forma da luz de uma estrela) é ajustada
   (método de Levenberg–Marquardt) para achar o centro exato.
2. **Posição no céu** pelo WCS.
3. **Refração atmosférica** (o ar curva a luz, cerca de 1-2' a 30° de altura, mais perto do
   horizonte) é corrigida só para uma posição terrestre que **não** tenha vindo de uma solução de
   plate-solve por catálogo: essa solução é refratada do mesmo jeito que suas estrelas de
   referência, então já absorve a correção, e aplicá-la de novo dobraria o efeito. Na prática isso
   significa que a correção não roda sobre a posição medida típica de hoje, já que estas vêm de
   uma solução de plate-solve; ela também é sempre ignorada para telescópios espaciais e abaixo de
   1° de altura.
4. **Aberração anual** só vai para o Registro: a solução de placa com o catálogo já a inclui.
   Precessão e nutação também não são aplicadas, porque as estrelas do catálogo já dão
   coordenadas ICRF (J2000), que é o que o relatório usa (`sys=ICRF`).
5. **Forma da PSF:** o FWHM vai para o Registro; se a estrela estiver alongada, o Registro dá os
   dois eixos e o ângulo, e acima de 1,5 avisa "check tracking, focus, or coma".
6. **Fotometria de abertura:** a luz dentro de um círculo de raio max(3, 2×FWHM) px, menos o céu
   medido num anel em volta. Raio fixo: **Configurações → Detecção → Raio da abertura**.
7. **Magnitude** a partir de estrelas do catálogo próximas (ponto zero diferencial). Se nenhuma
   casar, o Registro diz "using instrumental mag".
8. **Massa de ar** (quanto ar a luz atravessou) é sempre calculada; a correção de extinção só roda
   se "Coef. de extinção k" > 0.

Fórmulas: `docs/technical-reference.md` no repositório, e [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced).

---

## 22. Growth curve and light curve / Curva de crescimento e curva de luz

🟡 Intermediate / Intermediário

🇬🇧 **English**
**What it is:** the **growth curve** shows how much of a star's light falls inside apertures of
growing radius. The **light curve** plots the magnitude of your measurements against time.
**What it's for:** choosing the best aperture radius; seeing an asteroid's brightness change
(rotation) or spotting a bad measurement.
**How to do it:**
- **Tools → Growth Curve…** (`Ctrl+Shift+G`) / **Ferramentas → Curva de Crescimento…** (needs
  detected stars): choose a star and radii, **Compute**, then **Use optimal aperture (N px)**,
  which switches the aperture to manual mode in Settings.
- **Tools → Light Curve…** (`Ctrl+Shift+L`) / **Ferramentas → Curva de Luz…**: choose "All objects" or one object;
  **Export PNG…**. It updates when you add measurements; click a point to open its image.

> 🇬🇧 💡 **Shortcut:** Light Curve is `Ctrl+Shift+L`. In 1.1.0 and earlier the menu showed
> `Ctrl+L`, which clashed with Load Images and could do nothing; there, use the menu.
>
> 🇧🇷 💡 **Atalho:** a Curva de Luz é `Ctrl+Shift+L`. Na 1.1.0 e anteriores o menu mostrava
> `Ctrl+L`, que conflitava com Carregar Imagens e podia não fazer nada; lá, use o menu.

🇧🇷 **Português**
**O que é:** a **curva de crescimento** mostra quanto da luz de uma estrela cai dentro de
aberturas de raio crescente. A **curva de luz** mostra a magnitude das suas medições ao longo do
tempo.
**Para que serve:** escolher o melhor raio de abertura; ver a variação de brilho de um asteroide
(rotação) ou achar uma medição ruim.
**Como fazer:**
- **Ferramentas → Curva de Crescimento…** (`Ctrl+Shift+G`) (precisa de estrelas detectadas):
  escolha uma estrela e os raios, **Compute**, depois **Use optimal aperture (N px)**, que passa a
  abertura para o modo manual nas Configurações.
- **Ferramentas → Curva de Luz…** (`Ctrl+Shift+L`): escolha "All objects" ou um objeto; **Export PNG…**. Atualiza
  quando você adiciona medições; clique num ponto para abrir a imagem dele.

---

# F. Reporting / Relatório

## 23. ADES report and submission / Relatório ADES e envio

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** **ADES** (Astrometry Data Exchange Standard, 2022 version) is the report format of
the Minor Planet Center (**MPC**). AstroFind writes it as **XML** and as **PSV** (text columns
separated by `|`). It does **not** produce the old MPC 80-column format.
**What it's for:** sending your observations to the MPC, or to your teacher.
**How to do it:**
1. Fill in **Settings → Observer**: MPC Station Code (otherwise the report uses `XXX`), Observer,
   Measurer, Telescope, and optionally contact details ("Include contact info in ADES report").
2. **File → View ADES Report File** / **Arquivo → Ver Arquivo de Relatório ADES** (or the toolbar
   or Workflow step 6). The "ADES 2022 Report Preview" window has **XML** and **PSV** tabs.
3. Buttons: **Save…** (visible tab), **Save to Reports Folder** (both files), **Copy**, **Export
   PDF…** (summary with thumbnails and a table), **Enviar para Professor** (only if "E-mail do
   Professor" is set), **Submit to MPC (HTTP)**, **Submit to MPC (Email)**, **Close**.
4. The magnitude band comes from the image's FILTER, or "Default band" in Settings → Detection;
   change it per row in the Observations panel. "Include magnitude in ADES report" turns
   magnitudes off.

> 🇬🇧 ⚠️ **Watch out:**
> - AstroFind has not yet been validated end to end with real observatory data. **Check every
>   result before submitting to the MPC.**
> - There are two report folders: "Report output folder" (Settings → Connections) gets automatic
>   copies `ades_report.xml`/`.psv` each time you open the preview; **Save to Reports Folder**
>   writes to a different folder (your home by default) that cannot be changed in Settings.

🇧🇷 **Português**
**O que é:** **ADES** (Astrometry Data Exchange Standard, versão 2022) é o formato de relatório do
Minor Planet Center (**MPC**). O AstroFind gera em **XML** e em **PSV** (colunas de texto
separadas por `|`). Ele **não** gera o antigo formato MPC de 80 colunas.
**Para que serve:** enviar suas observações ao MPC ou ao professor.
**Como fazer:**
1. Preencha **Configurações → Observador**: Código de estação MPC (senão o relatório usa `XXX`),
   Observador, Medidor, Telescópio e, se quiser, contato ("Incluir informações de contato no
   relatório ADES").
2. **Arquivo → Ver Arquivo de Relatório ADES** (ou a barra de ferramentas, ou a etapa 6 do Fluxo
   de Trabalho). A janela "Pré-visualização do relatório ADES 2022" tem as abas **XML** e **PSV**.
3. Botões: **Save…** (aba visível), **Save to Reports Folder** (os dois arquivos), **Copy**,
   **Export PDF…** (resumo com miniaturas e tabela), **Enviar para Professor** (só se o "E-mail do
   Professor" estiver preenchido), **Submit to MPC (HTTP)**, **Submit to MPC (Email)**, **Close**.
4. A banda da magnitude vem do FILTER da imagem ou da "Banda padrão" em Configurações → Detecção;
   mude linha a linha no painel Observações. "Incluir magnitude no relatório ADES" desliga as
   magnitudes.

> 🇧🇷 ⚠️ **Atenção:**
> - O AstroFind ainda não foi validado de ponta a ponta com dados reais de observatório.
>   **Confira cada resultado antes de enviar ao MPC.**
> - Há duas pastas de relatório: a "Pasta de saída de relatórios" (Configurações → Conexões)
>   recebe cópias automáticas `ades_report.xml`/`.psv` toda vez que você abre a prévia; **Save to
>   Reports Folder** grava em outra pasta (sua pasta pessoal por padrão), que não dá para mudar
>   nas Configurações.

---

# G. Interface

## 24. Workflow panel, "Fluxo automático" and school mode / Painel de fluxo, "Fluxo automático" e modo escola

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** the **Workflow** panel (left) lists six steps: 1 Load Images, 2 Data Reduction,
3 Known Object Overlay, 4 Blink Images, 5 Measure Objects, 6 ADES Report. ○ = not yet, ▶ = next,
✓ = done. The next step has a **▶ Do it** button (during reduction it becomes **⏹ Parar**).
**What it's for:** guiding beginners and classes step by step.
**How to do it:**
- Show/hide: **Windows → View Workflow Panel** / **Janelas → Exibir painel de fluxo de trabalho**.
- Tick **Fluxo automático** to run steps 2, 3 and 4 by themselves after loading.
- For teachers: **File → Exportar Configuração da Escola...** saves all settings to an `.ini`
  file; students load it with **File → Importar Configuração da Escola...** (both shown in
  Portuguese in every language). Set "E-mail do Professor" (Settings → Observer) so students get
  the **Enviar para Professor** button.

🇧🇷 **Português**
**O que é:** o painel **Fluxo de Trabalho** (à esquerda) lista seis etapas: 1 Carregar Imagens,
2 Redução de Dados, 3 Obj. Conhecidos, 4 Piscar Imagens, 5 Medir Objetos, 6 Relatório ADES.
○ = ainda não, ▶ = próxima, ✓ = feita. A próxima etapa tem o botão **▶ Executar** (durante a
redução vira **⏹ Parar**).
**Para que serve:** guiar iniciantes e turmas passo a passo.
**Como fazer:**
- Mostrar/esconder: **Janelas → Exibir painel de fluxo de trabalho**.
- Marque **Fluxo automático** para rodar sozinhas as etapas 2, 3 e 4 depois de carregar.
- Para professores: **Arquivo → Exportar Configuração da Escola...** salva todas as configurações
  num arquivo `.ini`; os alunos carregam com **Arquivo → Importar Configuração da Escola...**.
  Preencha o "E-mail do Professor" (Configurações → Observador) para os alunos terem o botão
  **Enviar para Professor**.

---

## 25. Themes, language, panels and toolbars / Temas, idioma, painéis e barras

🟢 Beginner / Iniciante

🇬🇧 **English**
**What it is:** the look of the program and which parts are shown.
**What it's for:** a dark theme protects your night vision at the telescope; hidden panels can
be brought back.
**How to do it:**
- **Windows → Toggle Day/Night Mode** / **Janelas → Alternar modo Dia/Noite**, or the moon/sun
  icon on the toolbar: Night → Day → Auto (follows the system). Also **Settings → Display →
  Theme**. The shortcut `Ctrl+Shift+T` does the same as the icon. (In 1.1.0 and earlier it
  was assigned twice and might not respond; there, use the menu or icon.)
- **Settings → Display → Language**: English or Português (BR). A restart is needed.
- **Windows → View …**: Standard, Display and Blink toolbars; Workflow, Calibration, Images, Log,
  Observations and Image Catalog panels.
- The **Log** panel (**File → View Log File** only shows this panel, no file is opened) has
  **Copy all** and **Clear** buttons.

🇧🇷 **Português**
**O que é:** a aparência do programa e quais partes aparecem.
**Para que serve:** o tema escuro protege sua visão noturna no telescópio; painéis escondidos
podem voltar.
**Como fazer:**
- **Janelas → Alternar modo Dia/Noite**, ou o ícone de lua/sol na barra: Noite → Dia →
  Automático (segue o sistema). Também **Configurações → Exibição → Tema**. O atalho
  `Ctrl+Shift+T` faz o mesmo que o ícone. (Na 1.1.0 e anteriores ele estava ligado duas vezes e
  podia não responder; lá, use o menu ou o ícone.)
- **Configurações → Exibição → Idioma**: English ou Português (BR). É preciso reiniciar.
- **Janelas → Exibir …**: barras padrão, de exibição e de piscar; painéis de fluxo de trabalho,
  calibração, imagens, log, observações e catálogo de imagens.
- O painel **Registro** (**Arquivo → Ver Arquivo de Log** só mostra esse painel, não abre arquivo)
  tem os botões **Copiar tudo** e **Limpar**.

---

## 26. Help and About / Ajuda e Sobre

🟢 Beginner / Iniciante

🇬🇧 **English**
- **Help → Help Contents** (`F1`) / **Ajuda → Conteúdo da Ajuda**: built-in help, with an
  English / Português (BR) selector.
- **Help → About...** / **Ajuda → Sobre...**: version, credits and licence (AGPL-3.0).
- **Help → Registration...** / **Ajuda → Registro...** only shows a status-bar message:
  "Registration — not needed (free software, AGPL-3.0)". (Version 1.1.0 and earlier wrongly
  said "MIT open source".)
- **File → Exit** (`Ctrl+Q`) / **Arquivo → Sair** offers to save a modified project.

🇧🇷 **Português**
- **Ajuda → Conteúdo da Ajuda** (`F1`): ajuda embutida, com seletor English / Português (BR).
- **Ajuda → Sobre...**: versão, créditos e licença (AGPL-3.0).
- **Ajuda → Registro...** só mostra uma mensagem na barra de status: "Registro — não é
  necessário (software livre, AGPL-3.0)". (A versão 1.1.0 e anteriores diziam, errado, "código
  aberto MIT".)
- **Arquivo → Sair** (`Ctrl+Q`) oferece salvar um projeto modificado.

---

🇬🇧 **Next:** the complete observing workflow in [Manual](https://github.com/petrinhu/astrofind/wiki/Manual); every option in [Settings](https://github.com/petrinhu/astrofind/wiki/Settings); problems
in [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting); terms in [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Próximo:** o fluxo completo de observação no [Manual](https://github.com/petrinhu/astrofind/wiki/Manual); todas as opções em [Settings](https://github.com/petrinhu/astrofind/wiki/Settings);
problemas em [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting); termos no [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).
