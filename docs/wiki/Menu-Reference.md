# Menu Reference / Referência de Menus

🇬🇧 **Who this page is for.** Anyone who wants to know what a menu item, button or key does in
AstroFind 1.2.0. You do not need to read it from top to bottom: use it like a dictionary. Each
entry gives the English label, the Portuguese label, the keyboard shortcut, what it does in
plain words, and a level badge: 🟢 Beginner, 🟡 Intermediate, 🔴 Advanced. New to the words?
See [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary). For a guided first session, start with [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start).

🇧🇷 **Para quem é esta página.** Para quem quer saber o que faz um item de menu, botão ou tecla
do AstroFind 1.2.0. Não precisa ler de cima a baixo: use como um dicionário. Cada item traz o
rótulo em inglês, o rótulo em português, o atalho de teclado, o que ele faz em palavras simples
e um selo de nível: 🟢 Iniciante, 🟡 Intermediário, 🔴 Avançado. Alguma palavra estranha? Veja o
[Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary). Para uma primeira sessão guiada, comece pelo [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start).

**Contents / Conteúdo**

1. [Before you start / Antes de começar](#before-you-start--antes-de-começar)
2. [File / Arquivo](#file--arquivo)
3. [Edit / Editar](#edit--editar)
4. [Astrometry / Astrometria](#astrometry--astrometria)
5. [Images / Imagens](#images--imagens)
6. [Tools / Ferramentas](#tools--ferramentas)
7. [Internet](#internet)
8. [Windows / Janelas](#windows--janelas)
9. [Help / Ajuda](#help--ajuda)
10. [Context menus and pop-ups / Menus de contexto e janelas rápidas](#context-menus-and-pop-ups--menus-de-contexto-e-janelas-rápidas)
11. [Toolbars / Barras de ferramentas](#toolbars--barras-de-ferramentas)
12. [Docks (side and bottom panels) / Painéis laterais e inferiores](#docks-side-and-bottom-panels--painéis-laterais-e-inferiores)
13. [Status bar / Barra de status](#status-bar--barra-de-status)
14. [Blink view / Visão de piscar](#blink-view--visão-de-piscar)
15. [Image view: mouse and keyboard / Visão da imagem: mouse e teclado](#image-view-mouse-and-keyboard--visão-da-imagem-mouse-e-teclado)
16. [All keyboard shortcuts / Todos os atalhos de teclado](#all-keyboard-shortcuts--todos-os-atalhos-de-teclado)

---

## Before you start / Antes de começar

🟢

🇬🇧 **English**

- The menu bar has eight menus, in this order: **File, Edit, Astrometry Tools, Image Tools,
  Utilities, Internet, Window, Help**. In Portuguese: **Arquivo, Editar, Ferramentas de
  Astrometria, Ferramentas de Imagem, Utilitários, Internet, Janela, Ajuda**. This page still
  groups them under their older section names (**Astrometry**, **Images**, **Tools**,
  **Windows**) so links from other pages keep working; the "EN label" column always shows the
  current on-screen text.
- The main window title is `AstroFind 1.2.0` when the program starts. After you save or open a
  project it shows `AstroFind — <name>.gus`, with a `*` when there are unsaved changes.
- A shortcut such as `Ctrl+L` means: hold the **Ctrl** key and press **L**.
- Some items are greyed out (disabled) until you do an earlier step. For example, most
  Astrometry Tools items need loaded images first.
- **Portuguese labels in the English UI.** A few labels were written in Portuguese directly in
  the program code, so they stay in Portuguese even when you choose English. On this page they
  are marked **(PT in both languages)**. Examples: **Exportar/Importar Configuração da
  Escola…**, the "Session in progress" dialog, and the column titles of the Image Catalog.

🇧🇷 **Português**

- A barra de menus tem oito menus, nesta ordem: **Arquivo, Editar, Ferramentas de Astrometria,
  Ferramentas de Imagem, Utilitários, Internet, Janela, Ajuda** (em inglês: File, Edit,
  Astrometry Tools, Image Tools, Utilities, Internet, Window, Help). Esta página ainda agrupa
  os itens sob os nomes de seção antigos (**Astrometria**, **Imagens**, **Ferramentas**,
  **Janelas**) para não quebrar os links de outras páginas; a coluna "PT label" sempre mostra o
  texto atual na tela.
- O título da janela principal é `AstroFind 1.2.0` quando o programa abre. Depois de salvar ou
  abrir um projeto ele mostra `AstroFind — <nome>.gus`, com `*` quando há alterações não salvas.
- Um atalho como `Ctrl+L` quer dizer: segure a tecla **Ctrl** e aperte **L**.
- Alguns itens ficam cinza (desabilitados) até você fazer um passo anterior. Por exemplo, a
  maior parte do menu Ferramentas de Astrometria precisa de imagens carregadas antes.
- **Rótulos em português na interface em inglês.** Alguns textos foram escritos em português
  direto no código do programa, então continuam em português mesmo com o idioma inglês. Nesta
  página eles aparecem marcados **(PT nos dois idiomas)**. Exemplos: **Exportar/Importar
  Configuração da Escola…**, a janela "Sessão em andamento" e os títulos das colunas do
  Catálogo de Imagem.

---

## File / Arquivo

🇬🇧 **English** — Opening and saving images and projects, calibration frames, reports, and
the program settings. A *project* (`.gus` file) remembers which images you used and your
measurements; it does not copy the pixel data (see [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats)).

🇧🇷 **Português** — Abrir e salvar imagens e projetos, quadros de calibração, relatórios e as
configurações do programa. Um *projeto* (arquivo `.gus`) guarda quais imagens você usou e as
suas medições; ele não copia os pixels (veja [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats)).

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Import Images... | Importar Imagens... | `Ctrl+L` | Opens the "Load Astronomical Images" file dialog. You can pick many files, or a ZIP/TAR/7Z/RAR archive. One window opens per image. If images are already loaded, asks whether to add them (**Acréscimo**) or start a **Novo Projeto**. | Abre a janela "Carregar Imagens Astronômicas". Você pode escolher vários arquivos ou um arquivo compactado ZIP/TAR/7Z/RAR. Abre uma janela por imagem. Se já houver imagens, pergunta se quer acrescentar (**Acréscimo**) ou começar um **Novo Projeto**. | 🟢 |
| Open Recent... ▸ | Abrir Recente... ▸ | — | Submenu with the last 8 folders you loaded images from. Clicking one loads every supported image in that folder. Ends with **Clear Recent** / **Limpar Recentes**. Empty list shows "(no recent files)". | Submenu com as 8 últimas pastas de onde você carregou imagens. Clicar numa carrega todas as imagens suportadas dela. Termina com **Limpar Recentes**. Vazio mostra "(nenhum arquivo recente)". | 🟢 |
| Save Project | Salvar Projeto | `Ctrl+S` | Saves the session to its `.gus` file. If it has no file yet, works like Save Project As. Needs loaded images. | Salva a sessão no arquivo `.gus`. Se ainda não tem arquivo, funciona como Salvar Projeto Como. Precisa de imagens carregadas. | 🟢 |
| Save Project As... | Salvar Projeto Como... | `Ctrl+Shift+S` | Asks for a new `.gus` file name. The default folder is `~/projects` (created if missing). | Pede um novo nome de arquivo `.gus`. A pasta padrão é `~/projects` (criada se não existir). | 🟢 |
| Open Project... | Abrir Projeto... | `Ctrl+O` | Offers to save the current project, then opens a `.gus`. Reloads the images from disk (or re-extracts them from the original archive). If a file is missing, offers **Localizar…**, **Pular** or **Cancelar tudo**. | Oferece salvar o projeto atual e abre um `.gus`. Recarrega as imagens do disco (ou extrai de novo do arquivo compactado original). Se faltar um arquivo, oferece **Localizar…**, **Pular** ou **Cancelar tudo**. | 🟢 |
| Recent Projects ▸ | Projetos Recentes ▸ | — | Submenu with the last 5 projects, then **Limpar Recentes**. Empty shows "(nenhum projeto recente)". | Submenu com os 5 últimos projetos e **Limpar Recentes**. Vazio mostra "(nenhum projeto recente)". | 🟢 |
| Close Project | Fechar Projeto | — | Offers to save, then clears the session, the observations and the undo history. | Oferece salvar e depois limpa a sessão, as observações e o histórico de desfazer. | 🟢 |
| Save all FITS | Salvar todos como FITS | — | Asks for a folder and writes a copy of every image as `<name>_wcs.fits`, with the plate solution (WCS) written in when the image is solved. | Pede uma pasta e grava uma cópia de cada imagem como `<nome>_wcs.fits`, com a solução de placa (WCS) gravada quando a imagem foi resolvida. | 🟡 |
| Save FITS Copy... | Salvar Cópia em FITS... | — | Same, for the active image only. | O mesmo, só para a imagem ativa. | 🟡 |
| Export Image As... ▸ | Exportar Imagem Como... ▸ | — | Submenu: **JPEG / PNG / BMP…**, **JPEG (quick)…** (PT **JPEG (rápido)…**), **PNG…**. Saves the active image as you see it on screen (a picture, not science data). | Submenu: **JPEG / PNG / BMP…**, **JPEG (rápido)…**, **PNG…**. Salva a imagem ativa como aparece na tela (uma figura, não dado científico). | 🟢 |
| Use Dark Frame for Calibration… | Usar Dark de Calibração | — | Loads a FITS dark frame (a picture taken with the telescope covered, to remove camera noise) and shows the Calibration panel. | Carrega um dark frame FITS (foto com o telescópio tampado, para tirar o ruído da câmera) e mostra o painel Calibração. | 🟡 |
| Use Flat Field for Calibration… | Usar Flat de Calibração | — | Loads a FITS flat field (a picture of an evenly lit surface, to correct vignetting and dust) and shows the Calibration panel. | Carrega um flat field FITS (foto de uma superfície iluminada por igual, para corrigir vinheta e poeira) e mostra o painel Calibração. | 🟡 |
| Calibration Wizard… | Assistente de Calibração… | — | Step-by-step window to choose a dark and a flat and apply them. | Janela passo a passo para escolher dark e flat e aplicá-los. | 🟡 |
| View ADES Report File | Ver Arquivo de Relatório ADES | — | Opens the ADES 2022 report preview (the format the Minor Planet Center accepts), where you can save, export PDF or submit. With no measurements it says "No observations yet". If a report folder is set in Settings, it first auto-saves `ades_report.xml` and `ades_report.psv` there. | Abre a pré-visualização do relatório ADES 2022 (o formato aceito pelo Minor Planet Center), onde você salva, exporta PDF ou envia. Sem medições mostra "No observations yet". Se houver pasta de relatórios em Configurações, antes salva `ades_report.xml` e `ades_report.psv` lá. | 🟢 |
| Show Photometry Results | Mostrar Resultados de Fotometria | — | **Does not open a file.** It only shows the Observations panel. | **Não abre arquivo.** Só mostra o painel Observações. | 🟢 |
| View Log File | Ver Arquivo de Log | — | **Does not open a file.** It only shows the Log panel. | **Não abre arquivo.** Só mostra o painel Registro (Log). | 🟢 |
| Reset Session Files | Limpar Arquivos da Sessão | `Ctrl+R` | Closes all images and clears the session. **Does not ask to save.** | Fecha todas as imagens e limpa a sessão. **Não pergunta se quer salvar.** | 🟢 |
| Close Loaded Images | Fechar Imagens | `Ctrl+W` | Exactly the same as Reset Session Files. **Does not ask to save.** | Exatamente o mesmo que Limpar Arquivos da Sessão. **Não pergunta se quer salvar.** | 🟢 |
| Reload MPCOrb | Recarregar MPCOrb | — | Re-reads the offline asteroid list `MPCORB.DAT`. If it is missing, the log says to use **Internet → Download MPCOrb Database**. | Relê a lista offline de asteroides `MPCORB.DAT`. Se não existir, o registro manda usar **Internet → Baixar Banco MPCOrb**. | 🟡 |
| Settings... | Configurações... | `Ctrl+,` | Opens the Settings window. See [Settings](https://github.com/petrinhu/astrofind/wiki/Settings). | Abre a janela de Configurações. Veja [Settings](https://github.com/petrinhu/astrofind/wiki/Settings). | 🟢 |
| Exportar Configuração da Escola... (PT in both languages) | Exportar Configuração da Escola... | — | Saves **all** settings to an `.ini` file (default `~/astrofind_escola.ini`) so a teacher can hand the same setup to students. | Grava **todas** as configurações num arquivo `.ini` (padrão `~/astrofind_escola.ini`) para o professor distribuir aos alunos. | 🔴 |
| Importar Configuração da Escola... (PT in both languages) | Importar Configuração da Escola... | — | Reads every setting from such an `.ini` file. | Lê todas as configurações de um arquivo `.ini` desses. | 🔴 |
| Exit | Sair | `Ctrl+Q` | Closes AstroFind. Offers to save a modified project. | Fecha o AstroFind. Oferece salvar um projeto modificado. | 🟢 |

> ⚠️ **Watch out / Atenção**
> 🇬🇧 `Ctrl+R` and `Ctrl+W` throw away the current session **without asking**. Save the
> project first (`Ctrl+S`) if you want to keep your work.
> 🇧🇷 `Ctrl+R` e `Ctrl+W` descartam a sessão atual **sem perguntar**. Salve o projeto antes
> (`Ctrl+S`) se quiser guardar o trabalho.

> 💡 **Version 1.1.0 / Versão 1.1.0**
> 🇬🇧 In 1.1.0 and earlier `Ctrl+L` was also given to **Utilities → Light Curve…**, so the key
> could do nothing; there, use the menu or the toolbar button. In version
> 1.2.0 `Ctrl+L` is only Import Images. See [the shortcut table](#all-keyboard-shortcuts--todos-os-atalhos-de-teclado).
> 🇧🇷 Na 1.1.0 e anteriores o `Ctrl+L` também estava em **Utilitários → Curva de Luz…**, então
> a tecla podia não fazer nada; lá, use o menu ou o botão da barra. Na versão
> 1.2.0 o `Ctrl+L` é só Importar Imagens.

---

## Edit / Editar

🇬🇧 **English** — Undo, clipboard and the three mouse *tools* (what a left-click on the image
does). Only one tool is active at a time.

🇧🇷 **Português** — Desfazer, área de transferência e as três *ferramentas* do mouse (o que um
clique esquerdo na imagem faz). Só uma ferramenta fica ativa por vez.

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Undo | Desfazer | `Ctrl+Z` | Undoes the last accepted measurement ("Add Observation"). | Desfaz a última medição aceita ("Add Observation"). | 🟢 |
| Redo | Refazer | `Ctrl+Shift+Z` (or `Ctrl+Y`) | Redoes what you just undid. | Refaz o que você acabou de desfazer. | 🟢 |
| Clear All Markings | Limpar Todas as Marcações | — | Empties the Observations table and removes all text notes. | Esvazia a tabela Observações e apaga todas as anotações de texto. | 🟢 |
| Copy Image | Copiar Imagem | `Ctrl+C` | Copies a picture of the active image window to the clipboard (paste it in a document or chat). | Copia uma figura da janela de imagem ativa para a área de transferência (cole num documento ou chat). | 🟢 |
| Select Tool | Ferramenta Seleção | `S` | The default tool (arrow cursor). Clicking a marker selects that object in the Image Catalog. | A ferramenta padrão (cursor seta). Clicar numa marca seleciona o objeto no Catálogo de Imagem. | 🟢 |
| Aperture Tool | Ferramenta Abertura | `A` | Measure mode (cross cursor). Clicking an object measures its position and brightness and opens the **Verification** window. Same as the **Measure Object** toolbar button (`M`). | Modo de medição (cursor em cruz). Clicar num objeto mede posição e brilho e abre a janela **Verificação**. Igual ao botão **Medir Objeto** (`M`). | 🟢 |
| Annotate Tool | Ferramenta Anotação | `N` | Clicking the image asks for "Label text:" and draws a magenta text note there. | Clicar na imagem pede "Label text:" e desenha ali uma anotação em magenta. | 🟢 |

> 💡 **Tip / Dica**
> 🇬🇧 `Ctrl+A` is **not** "select all" in AstroFind: it starts Data Reduction.
> 🇧🇷 `Ctrl+A` **não** é "selecionar tudo" no AstroFind: ele inicia a Redução de Dados.

> ⚠️ **Watch out / Atenção**
> 🇬🇧 The **Remove** button of the Observations panel cannot be undone with `Ctrl+Z`.
> 🇧🇷 O botão **Remover** do painel Observações não pode ser desfeito com `Ctrl+Z`.

---

## Astrometry / Astrometria

🇬🇧 **English** — On screen this menu is now called **Astrometry Tools**; this page keeps the
shorter section name so existing links keep working. The processing steps. *Astrometry* means
measuring precise sky positions. *Data Reduction* prepares the images and finds where each one
points in the sky (*plate solving*).

🇧🇷 **Português** — Na tela este menu agora se chama **Ferramentas de Astrometria**; esta
página mantém o nome de seção mais curto para não quebrar links existentes. As etapas de
processamento. *Astrometria* é medir posições precisas no céu. *Redução de Dados* prepara as
imagens e descobre para onde cada uma aponta no céu (*plate solving*, solução de placa).

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Run Data Reduction... | Executar Redução de Dados... | `Ctrl+A` | Runs, for every image: calibration (dark/flat), bad-pixel fix, optional background removal, star detection, then plate solving with astrometry.net or ASTAP. Images that already have a WCS are not solved again. | Executa, em cada imagem: calibração (dark/flat), correção de pixels ruins, remoção de fundo (opcional), detecção de estrelas e plate solving com astrometry.net ou ASTAP. Imagens que já têm WCS não são resolvidas de novo. | 🟢 |
| Stop Data Reduction | Parar Redução de Dados | `Ctrl+.` | Cancels a running reduction. Enabled only while it runs. | Cancela uma redução em andamento. Só fica ativo enquanto ela roda. | 🟢 |
| Detect Moving Objects... | Detectar Objetos em Movimento... | `Ctrl+M` | Automatically looks for sources that move in a straight line across the images and marks them "Cand #N". Needs Data Reduction first and at least 2 images with stars. | Procura automaticamente fontes que andam em linha reta entre as imagens e marca como "Cand #N". Precisa da Redução de Dados e de pelo menos 2 imagens com estrelas. | 🟡 |
| Stack Images... | Empilhar Imagens... | `Ctrl+T` | **Track & Stack**: you give the motion per frame (dX, dY in pixels, −500…500) and a method (Average / Median / Add). The images are shifted and added so a faint moving object adds up. Opens `track_stacked.fits`. | **Track & Stack**: você informa o movimento por quadro (dX, dY em pixels, −500…500) e o método (Média / Mediana / Soma). As imagens são deslocadas e somadas para um objeto fraco em movimento se somar. Abre `track_stacked.fits`. | 🔴 |
| Import Detected Stars (DAOPHOT/SExtractor)… | Importar Estrelas Detectadas (DAOPHOT/SExtractor)… | — | Loads a star list made by another program (FITS table) into the active image. | Carrega uma lista de estrelas feita por outro programa (tabela FITS) na imagem ativa. | 🔴 |
| Import Reduction Table (IRAF/Astropy)… | Importar Tabela de Redução (IRAF/Astropy)… | — | Same, for reduction tables with RA/Dec from IRAF or Astropy. | O mesmo, para tabelas de redução com AR/Dec do IRAF ou Astropy. | 🔴 |

---

## Images / Imagens

🇬🇧 **English** — On screen this menu is now called **Image Tools**; this page keeps the
shorter section name so existing links keep working. How the image looks on screen and
information about it. None of these change your original files.

🇧🇷 **Português** — Na tela este menu agora se chama **Ferramentas de Imagem**; esta página
mantém o nome de seção mais curto para não quebrar links existentes. Como a imagem aparece na
tela e informações sobre ela. Nada disso altera seus arquivos originais.

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Edit Image Settings... | Editar Configurações da Imagem... | — | Edit the active image's object name, filter, observer, telescope, RA/Dec, Julian Date, exposure, plate scale ("/px), gain and saturation. | Edita da imagem ativa: nome do objeto, filtro, observador, telescópio, AR/Dec, Data Juliana, exposição, escala ("/px), ganho e saturação. | 🟡 |
| View FITS Header... | Ver Cabeçalho FITS... | — | Shows the FITS header (the text block of information stored in the file). Does nothing for files that are not FITS. | Mostra o cabeçalho FITS (o bloco de texto com informações guardado no arquivo). Não faz nada em arquivos que não são FITS. | 🟡 |
| Adjust Black Point and Contrast… | Ajustar Ponto Preto e Contraste… | — | Adjusts brightness and contrast: black level, white level, transfer function (Linear, Logarítmica, Raiz Quadrada, Asinh, Equalização de Histograma) and false colour. Has an **⟳ Auto** button. | Ajusta brilho e contraste: nível de preto, nível de branco, função de transferência (Linear, Logarítmica, Raiz Quadrada, Asinh, Equalização de Histograma) e cor falsa. Tem botão **⟳ Auto**. | 🟢 |
| Rebuild Stack | Reconstruir Empilhamento | — | Aligns all images on the stars and combines them (Average / Median / Add). Opens `stacked.fits`. | Alinha todas as imagens pelas estrelas e combina (Média / Mediana / Soma). Abre `stacked.fits`. | 🟡 |
| Zoom In | Ampliar Mais | `Ctrl++`, `+`, `=` | Zooms in. | Aproxima. | 🟢 |
| Zoom Out | Ampliar Menos | `Ctrl+-`, `-` | Zooms out. | Afasta. | 🟢 |
| Fit to Window | Ajustar à Janela | `Ctrl+F`, `0` | Fits the whole image in its window. | Encaixa a imagem inteira na janela. | 🟢 |
| Choose Marker Display… | Escolher Exibição de Marcadores… | — | Choose which overlays to show: detected stars, catalog stars, known objects, labels. | Escolhe quais marcas mostrar: estrelas detectadas, estrelas de catálogo, objetos conhecidos, rótulos. | 🟢 |
| Invert Colors | Inverter Cores | `Ctrl+I` | On/off. Shows black stars on a white sky. | Liga/desliga. Mostra estrelas pretas em céu branco. | 🟢 |
| Flip Horizontal | Espelhar na Horizontal | — | Mirrors the view left–right. | Espelha a visão esquerda–direita. | 🟢 |
| Flip Vertical | Espelhar na Vertical | — | Mirrors the view top–bottom. | Espelha a visão cima–baixo. | 🟢 |
| Magnifier Tool | Ferramenta Lupa | `Ctrl+G` | On/off. A zoomed loupe follows the cursor on all images. | Liga/desliga. Uma lupa acompanha o cursor em todas as imagens. | 🟢 |
| Region Statistics | Estatísticas de Região | `Ctrl+Shift+R` | Drag a rectangle on the image to see its statistics. `Esc` cancels. | Arraste um retângulo na imagem para ver as estatísticas. `Esc` cancela. | 🟡 |

---

## Tools / Ferramentas

🇬🇧 **English** — On screen this menu is now called **Utilities**; this page keeps the
shorter section name so existing links keep working. Blinking, overlays and analysis windows.

🇧🇷 **Português** — Na tela este menu agora se chama **Utilitários**; esta página mantém o
nome de seção mais curto para não quebrar links existentes. Piscar, sobreposições e janelas de
análise.

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Begin Blink Mode | Ativar Modo de Piscagem | `Ctrl+B` | Opens the Blink View: shows the images one after the other so a moving object "jumps". Needs 2 or more images. See [Blink view](#blink-view--visão-de-piscar). | Abre a Visão de piscar: mostra as imagens uma após a outra para um objeto em movimento "pular". Precisa de 2 ou mais imagens. | 🟢 |
| End Blink Mode | Encerrar Modo de Piscagem | `Ctrl+F9` | Closes the Blink View. | Fecha a Visão de piscar. | 🟢 |
| Show Known Objects | Mostrar Objetos Conhecidos | `Ctrl+K` | Downloads reference stars (UCAC4 or Gaia DR3 from VizieR, or a local file) and the known asteroids/comets in the field (SkyBoT, or offline `MPCORB.DAT`), and marks them on the images. | Baixa estrelas de referência (UCAC4 ou Gaia DR3 do VizieR, ou arquivo local) e os asteroides/cometas conhecidos no campo (SkyBoT, ou `MPCORB.DAT` offline) e marca nas imagens. | 🟢 |
| Ecliptic / Galactic Overlay | Sobreposição Eclíptica / Galáctica | `Ctrl+E` | On/off. Draws the ecliptic (the Sun's path, where most asteroids are; label "Eclíptica") and the Milky Way plane (label "Via Láctea"). Warns when the field is near the galactic plane (many stars, more dust). | Liga/desliga. Desenha a eclíptica (caminho do Sol, onde ficam a maioria dos asteroides; rótulo "Eclíptica") e o plano da Via Láctea (rótulo "Via Láctea"). Avisa quando o campo está perto do plano galáctico (muitas estrelas, mais poeira). | 🟡 |
| Light Curve… | Curva de Luz… | `Ctrl+Shift+L` | Plots magnitude against time for your measurements. Can export PNG. | Faz o gráfico de magnitude contra tempo das suas medições. Exporta PNG. | 🟡 |
| Growth Curve… | Curva de Crescimento… | `Ctrl+Shift+G` | Shows how a star's measured light grows with the aperture (measuring circle) size and can set the best aperture in Settings. | Mostra como a luz medida de uma estrela cresce com o tamanho da abertura (círculo de medição) e pode gravar a melhor abertura nas Configurações. | 🔴 |
| Compare Sessions… | Comparar Sessões… | `Ctrl+Shift+C` | Subtracts a reference image from the session images and opens a "Residual" window with what changed. | Subtrai uma imagem de referência das imagens da sessão e abre uma janela "Residual" com o que mudou. | 🔴 |

> 💡 **Version 1.1.0 / Versão 1.1.0**
> 🇬🇧 In 1.1.0 and earlier Light Curve showed `Ctrl+L`, shared with Import Images, so the key
> might not work; there, open it from the menu (**Utilities → Light Curve…**).
> 🇧🇷 Na 1.1.0 e anteriores a Curva de Luz mostrava `Ctrl+L`, compartilhado com Carregar
> Imagens, então a tecla podia não funcionar; lá, abra pelo menu (**Utilitários → Curva de
> Luz…**).

---

## Internet

🇬🇧 **English** — Downloads and online services. You need an internet connection.

🇧🇷 **Português** — Downloads e serviços online. Precisa de conexão com a internet.

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Download MPCOrb Database | Baixar Banco MPCOrb | — | Downloads `MPCORB.DAT` (the Minor Planet Center list of all known asteroid orbits) so known objects can be found offline. The confirmation says about 200 MB; the status-bar tip says ~500 MB (the text is inconsistent). | Baixa o `MPCORB.DAT` (lista do Minor Planet Center com as órbitas de todos os asteroides conhecidos) para achar objetos conhecidos offline. A confirmação diz cerca de 200 MB; a dica na barra de status diz ~500 MB (o texto é inconsistente). | 🟡 |
| Update MPCOrb Database | Atualizar Banco MPCOrb | — | Downloads the daily update and adds the new objects. Needs `MPCORB.DAT` already downloaded. | Baixa a atualização diária e acrescenta os objetos novos. Precisa do `MPCORB.DAT` já baixado. | 🟡 |
| Query Horizons… | Consultar Horizons… | — | Asks NASA JPL Horizons where an object (e.g. `433`, `Eros`) is at the time of the first image, and can add it to the overlay. The position is **geocentric** (from Earth's centre), not from your site. | Pergunta ao JPL Horizons da NASA onde um objeto (ex.: `433`, `Eros`) está no horário da primeira imagem e pode acrescentá-lo à sobreposição. A posição é **geocêntrica** (do centro da Terra), não do seu local. | 🔴 |
| Minor Planet Center… | Minor Planet Center… | — | Opens minorplanetcenter.net in your web browser. | Abre minorplanetcenter.net no navegador. | 🟢 |

---

## Windows / Janelas

🇬🇧 **English** — On screen this menu is now called **Window** (singular); this page keeps
the older section name so existing links keep working. Arranging windows, the colour theme,
and showing/hiding toolbars and panels.

🇧🇷 **Português** — Na tela este menu agora se chama **Janela** (singular); esta página mantém
o nome de seção mais antigo para não quebrar links existentes. Organizar janelas, o tema de
cores e mostrar/esconder barras e painéis.

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Arrange Windows Side by Side | Organizar Janelas Lado a Lado | — | Arranges the image windows side by side. | Organiza as janelas de imagem lado a lado. | 🟢 |
| Stack Windows Diagonally | Empilhar Janelas na Diagonal | — | Stacks them diagonally. | Empilha na diagonal. | 🟢 |
| Auto-Arrange Windows | Organizar Automaticamente | — | Restores minimized/maximized windows and fits each image. | Restaura janelas minimizadas/maximizadas e encaixa cada imagem. | 🟢 |
| Close Every Open Image | Fechar Todas as Imagens Abertas | — | Same as **File → Reset Session Files**: clears the session **without asking**. | Igual a **Arquivo → Limpar Arquivos da Sessão**: limpa a sessão **sem perguntar**. | 🟢 |
| Close Every Window | Fechar Todas as Janelas | — | Closes the image windows but keeps the session. | Fecha as janelas de imagem mas mantém a sessão. | 🟢 |
| Toggle Day/Night Mode | Alternar modo Dia/Noite | — (`Ctrl+Shift+T` via the toolbar theme button) | Cycles the colour theme: Night → Day → Auto → Night. The menu item shows no shortcut; `Ctrl+Shift+T` belongs to the theme button of the Standard toolbar and does the same. | Alterna o tema de cores: Noite → Dia → Automático → Noite. O item de menu não mostra atalho; o `Ctrl+Shift+T` é do botão de tema da barra Padrão e faz o mesmo. | 🟢 |
| Show Main Toolbar | Mostrar barra de ferramentas principal | — | Shows/hides that toolbar. | Mostra/esconde essa barra. | 🟢 |
| Show Display Toolbar | Mostrar barra de exibição | — | Shows/hides that toolbar. | Mostra/esconde essa barra. | 🟢 |
| Show Blink Toolbar | Mostrar barra de piscagem | — | Shows/hides that toolbar. | Mostra/esconde essa barra. | 🟢 |
| View Workflow Panel | Exibir painel de fluxo de trabalho | — | Shows/hides the Workflow panel. | Mostra/esconde o painel Fluxo de Trabalho. | 🟢 |
| View Calibration Panel | Exibir painel de calibração | — | Shows/hides the Calibration panel. | Mostra/esconde o painel Calibração. | 🟢 |
| View Images Bar | Exibir barra de imagens | — | Shows/hides the thumbnail bar. | Mostra/esconde a barra de miniaturas. | 🟢 |
| View Log Panel | Exibir painel de log | — | Shows/hides the Log panel. | Mostra/esconde o painel Registro. | 🟢 |
| View Observations | Exibir observações | — | Shows/hides the Observations panel. | Mostra/esconde o painel Observações. | 🟢 |
| View Image Catalog | Exibir catálogo de imagens | — | Shows/hides the Image Catalog panel. | Mostra/esconde o painel Catálogo de Imagem. | 🟢 |

> 💡 **Tip / Dica**
> 🇬🇧 Closed a panel by accident? Bring it back from this **Window** menu.
> 🇧🇷 Fechou um painel sem querer? Traga de volta por este menu **Janela**.

---

## Help / Ajuda

🇬🇧 **English** — Built-in help, the setup wizard and program information.

🇧🇷 **Português** — Ajuda embutida, o assistente de configuração e informações do programa.

| EN label | PT label | Shortcut | What it does | O que faz | Level |
|---|---|---|---|---|---|
| Help Topics | Tópicos de Ajuda | `F1` | Opens the built-in help. A "Language:" box switches between English and Português (BR). | Abre a ajuda embutida. A caixa "Language:" troca entre English e Português (BR). | 🟢 |
| Setup Wizard… | Assistente de Configuração… | — | Opens the 3-step first-run wizard (name, MPC code, API key, MPCORB download). See [Settings](https://github.com/petrinhu/astrofind/wiki/Settings) (section Setup Wizard). | Abre o assistente de 3 etapas (nome, código MPC, chave de API, download do MPCORB). Veja [Settings](https://github.com/petrinhu/astrofind/wiki/Settings) (seção Assistente). | 🟢 |
| Product Registration... | Registro do Produto... | — | Only shows a status-bar message "Registration — not needed (free software, AGPL-3.0)". (1.1.0 and earlier wrongly said "MIT open source".) | Só mostra na barra de status "Registro — não é necessário (software livre, AGPL-3.0)". (A 1.1.0 e anteriores diziam, errado, "código aberto MIT".) | 🟢 |
| About AstroFind... | Sobre o AstroFind... | — | Version, credits, license and reference software. | Versão, créditos, licença e programas de referência. | 🟢 |

---

## Context menus and pop-ups / Menus de contexto e janelas rápidas

### Image window right-click / Clique direito na janela de imagem

🇬🇧 **English** — Right-click inside an image window. 🇧🇷 **Português** — Clique com o botão
direito dentro de uma janela de imagem.

| EN label | PT label | What it does | O que faz | Level |
|---|---|---|---|---|
| Export as JPEG/PNG… | Exportar como JPEG/PNG… | Save this image as seen (JPEG/PNG/BMP). | Salva esta imagem como aparece (JPEG/PNG/BMP). | 🟢 |
| Apply Dark Frame… | Aplicar Dark Frame… | Applies the loaded dark to this image only (asks for one if none is loaded). | Aplica o dark carregado só nesta imagem (pede um se não houver). | 🟡 |
| Apply Flat Field… | Aplicar Flat Field… | Same for the flat. | O mesmo para o flat. | 🟡 |
| Show Histogram | Exibir Histograma | Graph of how many pixels have each brightness, with sky level and noise (μ, σ). | Gráfico de quantos pixels têm cada brilho, com nível do céu e ruído (μ, σ). | 🟡 |
| Show Power Spectrum | Mostrar Espectro de Potência | 2-D frequency picture (FFT) to spot periodic noise or bad focus. | Figura de frequências 2-D (FFT) para achar ruído periódico ou foco ruim. | 🔴 |
| Animate Cube (N frames)… | Animar Cubo (N quadros)… | Only for FITS cubes with more than 3 planes: plays them like a blink. | Só para cubos FITS com mais de 3 planos: toca como um piscar. | 🔴 |
| Show Image Catalog | Exibir Catálogo de Imagem | Shows the Image Catalog panel. | Mostra o painel Catálogo de Imagem. | 🟢 |
| FITS Header… | Cabeçalho FITS… | Same as **Image Tools → View FITS Header...** | Igual a **Ferramentas de Imagem → Ver Cabeçalho FITS...** | 🟡 |

### Drag and drop / Arrastar e soltar

🇬🇧 **English** — Dropping **one** file on the window while images are already loaded shows a
small menu: **Open as Science Image**, **Use as Dark Frame**, **Use as Flat Field**, **Cancel**.
Drag and drop only accepts `.fits .fit .fts .zip` and TAR/7Z/RAR archives; other formats
(TIFF, RAW, PDS, …) are ignored when dropped: use **File → Import Images...** for those.

🇧🇷 **Português** — Soltar **um** arquivo na janela com imagens já carregadas mostra um
pequeno menu: **Abrir como imagem científica**, **Usar como dark frame**, **Usar como flat
field**, **Cancelar**. Arrastar e soltar só aceita `.fits .fit .fts .zip` e arquivos
TAR/7Z/RAR; outros formatos (TIFF, RAW, PDS, …) são ignorados: use **Arquivo → Importar
Imagens...** para eles.

### Pop-up dialogs you will meet / Janelas que você vai encontrar

🇬🇧 **English**

- **"Sessão em andamento"** (PT in both languages): appears when you load images while others
  are open. **Acréscimo** = add the new images to the session. **Novo Projeto** = unload
  everything and start clean; it also resets pixel scale, saturation and location to 0 and
  sets location to "from FITS" (Time Offset is kept; 1.1.0 and earlier also reset it). **Cancel** = do nothing.
- **"Salvar projeto?"** (PT in both languages): **Salvar** (save) / **Não salvar** (don't
  save) / **Cancelar** (go back).
- **"Imagens carregadas"** (PT in both languages): tells you whether the images are black &
  white or colour. Untick **Mostrar sempre este aviso** to stop seeing it.
- **Verification / Verificação**: opens after every measurement. Buttons **✓ Aceitar**
  (accept, keep window open), **✓ Aceitar e Fechar** (accept and close), **✗ Rejeitar**
  (discard) (PT in both languages). Double-click a name in "Objetos conhecidos próximos" to use
  it as the object's designation.
- **"Blink em execução"**: appears if you click to measure while blinking. **Parar e Medir**
  (stop and measure) or **Continuar Blink** (keep blinking).

🇧🇷 **Português**

- **"Sessão em andamento"**: aparece quando você carrega imagens com outras já abertas.
  **Acréscimo** = junta as novas à sessão. **Novo Projeto** = descarrega tudo e começa do zero;
  também zera escala de pixel, saturação e localização e coloca a localização em "do FITS"
  (o Deslocamento de tempo é mantido; a 1.1.0 e anteriores também o zeravam). **Cancelar** = não faz nada.
- **"Salvar projeto?"**: **Salvar** / **Não salvar** / **Cancelar**.
- **"Imagens carregadas"**: diz se as imagens são preto e branco ou coloridas. Desmarque
  **Mostrar sempre este aviso** para não ver mais.
- **Verificação**: abre depois de cada medição. Botões **✓ Aceitar** (aceita e deixa a janela
  aberta), **✓ Aceitar e Fechar**, **✗ Rejeitar** (descarta). Clique duplo num nome em
  "Objetos conhecidos próximos" para usá-lo como designação do objeto.
- **"Blink em execução"**: aparece se você clicar para medir durante o piscar. **Parar e
  Medir** ou **Continuar Blink**.

🇬🇧 The Observations table, the Log and the Image Catalog have **no** right-click menu. 🇧🇷 A
tabela Observações, o Registro e o Catálogo de Imagem **não** têm menu de clique direito.

---

## Toolbars / Barras de ferramentas

🇬🇧 **English** — Three toolbars with icons only. Hover over an icon to see its tooltip
(quoted below). Show or hide them in the **Window** menu.

🇧🇷 **Português** — Três barras só com ícones. Pare o mouse sobre um ícone para ver a dica
(citada abaixo). Mostre ou esconda pelo menu **Janela**.

### Standard / Padrão

| # | Tooltip EN | Tooltip PT | Does |
|---|---|---|---|
| 1 | Settings (Ctrl+,) | Configurações (Ctrl+,) | Opens [Settings](https://github.com/petrinhu/astrofind/wiki/Settings). |
| 2 | Night Mode — click to switch to Day (Ctrl+Shift+T) · Day Mode — click to switch to Auto · Auto Mode — click to switch to Night | Modo Noturno — clique para mudar para Diurno · Modo Diurno — clique para mudar para Auto · Modo Auto — clique para mudar para Noturno | Theme button. The icon (moon / sun / auto) shows the **current** theme. |
| 3 | Import Images (Ctrl+L) | Importar Imagens (Ctrl+L) | File → Import Images... |
| 4 | Use this dark frame for calibration | Usar este dark para calibração | File → Use Dark Frame for Calibration… |
| 5 | Use this flat field for calibration | Usar este flat para calibração | File → Use Flat Field for Calibration… |
| 6 | Run Data Reduction (Ctrl+A) | Executar Redução de Dados (Ctrl+A) | Astrometry Tools → Run Data Reduction... |
| 7 | Detect Moving Objects (Ctrl+M) | Detectar Objetos em Movimento (Ctrl+M) | Astrometry Tools → Detect Moving Objects... |
| 8 | Stack Images (Ctrl+T) | Empilhar Imagens (Ctrl+T) | Astrometry Tools → Stack Images... |
| 9 | **Measure Object** — Click on a moving object to measure its position and magnitude (M) | **Medir Objeto** — Clique em um objeto em movimento para medir sua posição e magnitude (M) | On/off (`M`). On = Aperture Tool, the button turns orange. Off = Select Tool. |
| 10 | View ADES Report | Ver Relatório ADES | File → View ADES Report File |
| 11 | Close every open image window | Fechar todas as janelas de imagem abertas | Clears the session (like Reset Session Files). |
| 12 | Close every window | Fechar todas as janelas | Window → Close Every Window |

### Display / Exibição

| # | Tooltip EN | Tooltip PT | Does |
|---|---|---|---|
| 1 | Adjust the black point and contrast stretch | Ajustar o ponto preto e o contraste da exibição | Image Tools → Adjust Black Point and Contrast… |
| 2 | Choose which markers are shown | Escolher quais marcadores são exibidos | Image Tools → Choose Marker Display… |
| 3 | Invert display (Ctrl+I) | Inverter exibição (Ctrl+I) | Image Tools → Invert Colors |
| 4 | Show Known Objects (Ctrl+K) | Mostrar objetos conhecidos (Ctrl+K) | Utilities → Show Known Objects |
| 5 | Zoom In | Ampliar | Zoom in |
| 6 | Zoom Out | Reduzir | Zoom out |
| 7 | Fit image to window (Ctrl+F) | Ajustar imagem à janela (Ctrl+F) | Image Tools → Fit to Window |

### Blink / Piscar

| # | Tooltip EN | Tooltip PT | Does |
|---|---|---|---|
| 1 | Begin Blink Mode (Ctrl+B) | Ativar Modo de Piscagem (Ctrl+B) | Utilities → Begin Blink Mode |
| 2 | Blink interval (tenths of a second) | Intervalo de piscar (décimos de segundo) | Number box 1–9, shown as "×0.1s" ("×0,1s"): 3 = 0.3 s per image. |
| 3 | End Blink Mode (Ctrl+F9) | Encerrar Modo de Piscagem (Ctrl+F9) | Utilities → End Blink Mode |
| 4 | Step to previous image (Shift+Ctrl+F10) | Ir para imagem anterior (Shift+Ctrl+F10) | Previous image |
| 5 | Step to next image (Ctrl+F10) | Ir para próxima imagem (Ctrl+F10) | Next image |
| 6 | Start blink playback (Ctrl+F11) | Iniciar reprodução de piscar (Ctrl+F11) | Play |

> 💡 **Tip / Dica**
> 🇬🇧 The toolbar interval box only goes up to 0.9 s and is not saved when you quit. The
> permanent value is **Settings → Display → Blink interval** (100–2000 ms).
> 🇧🇷 A caixa de intervalo da barra só vai até 0,9 s e não é salva ao sair. O valor permanente
> fica em **Configurações → Exibição → Intervalo de piscar** (100–2000 ms).

---

## Docks (side and bottom panels) / Painéis laterais e inferiores

🇬🇧 **English** — *Docks* are panels around the image area. You can drag them to another side,
or close them and reopen them from the **Windows** menu.

🇧🇷 **Português** — *Docks* são painéis em volta da área das imagens. Você pode arrastar para
outro lado, ou fechar e reabrir pelo menu **Janelas**.

| EN title | PT title | Where / when | What is inside | O que tem dentro | Level |
|---|---|---|---|---|---|
| Workflow | Fluxo de Trabalho | Left, visible at start | Six numbered steps (Import Images, Run Data Reduction, Show Known Objects, Begin Blink Mode, Measure Objects, ADES Report). ○ = not yet, ▶ = next step, ✓ = done. The next step has a **▶ Do it** button. The **Fluxo automático** box (PT in both languages) runs steps 2, 3 and 4 by itself after loading. | Seis passos numerados (Importar Imagens, Executar Redução de Dados, Mostrar Conhecidos, Ativar Modo de Piscagem, Medir Objetos, Relatório ADES). ○ = ainda não, ▶ = próximo passo, ✓ = feito. O próximo passo tem o botão **▶ Executar**. A caixa **Fluxo automático** executa sozinha as etapas 2, 3 e 4 depois de carregar. | 🟢 |
| Calibration | Calibração | Left, appears when a dark or flat is loaded | Two groups, **Dark Frame** and **Flat Field**, each with **Load…** / **Carregar…**, **Build Master…** / **Construir Master…** (combine several frames into one) and **Clear** / **Limpar**. Then **Apply to All Images** / **Aplicar a Todas as Imagens**. A frame is used only on images of the same size. | Dois grupos, **Dark Frame** e **Flat Field**, cada um com **Carregar…**, **Construir Master…** (combinar vários quadros em um) e **Limpar**. Depois **Aplicar a Todas as Imagens**. O quadro só é usado em imagens do mesmo tamanho. | 🟡 |
| Images | Imagens | Bottom, visible | One thumbnail per image. Click one to bring its window to the front. | Uma miniatura por imagem. Clique para trazer a janela à frente. | 🟢 |
| Log | Registro | Bottom, visible | Time-stamped messages. Grey-blue = info, orange ⚠ = warning, red ✗ = error. Buttons **Copy all** / **Copiar tudo** and **Clear** / **Limpar**. Read it when something fails. | Mensagens com horário. Cinza-azulado = informação, laranja ⚠ = aviso, vermelho ✗ = erro. Botões **Copiar tudo** e **Limpar**. Leia quando algo der errado. | 🟢 |
| Observations | Observações | Bottom right, after your first measurement | Your accepted measurements. Columns `# Object RA (°) Dec (°) Mag Band FWHM" SNR Airmass JD Image` (English in both languages). Double-click **Object** or **Band** to edit. **Remove** / **Remover** deletes selected rows (no undo). | Suas medições aceitas. Colunas em inglês nos dois idiomas. Clique duplo em **Object** ou **Band** para editar. **Remover** apaga as linhas selecionadas (sem desfazer). | 🟢 |
| Image Catalog | Catálogo de Imagem | Right, appears the first time stars are detected | Every object found in the active image. Columns `Tipo AR (°) Dec (°) Mag FWHM" SNR X px Y px` (Portuguese in both languages). Types: **Estrela** (detected star), **Estrela Guia** (catalog reference star), **Asteroide Conhecido** (known object). Click a row to highlight it in red and centre the view; `Esc` clears. | Todos os objetos achados na imagem ativa. Tipos: **Estrela** (detectada), **Estrela Guia** (estrela de referência do catálogo), **Asteroide Conhecido**. Clique numa linha para destacar em vermelho e centralizar; `Esc` limpa. | 🟡 |

🇬🇧 **Also on screen.** Above the images, an **information bar** (Portuguese in both
languages) shows the number of images, telescope, date, filter, exposure and size, and chips
such as "✓ Escala …"/px", "⚠ Escala de pixel ausente" with a **Câmera →** button, "✓ WCS
pré-resolvido", "✓ Data/hora UTC", "✓ Saturação …", and the location chip with **Observatório
→** or **Configurar →**. These buttons open Settings. Each image window has a thin line at the
bottom (`name | W×H | JD | exp`) and, for files with several images inside (HDUs), a bar with
`‹`, a list and `›` to switch between them. The Image Catalog tooltip for **Estrela Guia**
always says "UCAC4", even when Gaia DR3 is selected.

🇧🇷 **Também na tela.** Acima das imagens, uma **barra de informações** mostra o número de
imagens, telescópio, data, filtro, exposição e tamanho, e etiquetas como "✓ Escala …"/px",
"⚠ Escala de pixel ausente" com o botão **Câmera →**, "✓ WCS pré-resolvido", "✓ Data/hora
UTC", "✓ Saturação …" e a etiqueta de localização com **Observatório →** ou **Configurar →**.
Esses botões abrem as Configurações. Cada janela de imagem tem uma linha fina embaixo (`nome |
L×A | JD | exp`) e, para arquivos com várias imagens dentro (HDUs), uma barra com `‹`, uma
lista e `›` para trocar entre elas. A dica de **Estrela Guia** sempre diz "UCAC4", mesmo com
Gaia DR3 selecionado.

---

## Status bar / Barra de status

🟢

🇬🇧 **English** — The strip at the very bottom of the window.

- **Left: the step message.** It tells you what to do next, for example "Ready — load images
  to start", "Step 1: Images loaded — Run Data Reduction", "Step 2 done - show Known
  Objects (Ctrl+K)", "Step 3 done — start Blinking (Ctrl+B)", "Blinking — Space = pause,
  Left/Right = step, click object to measure", "Step 6 done — view and send ADES Report".
  Progress messages also appear here ("Detecting stars... 2/4", MPCORB download speed).
- **Right, from left to right:**
  1. **Observatory**: "Obs: auto (…)", "Obs: <code> (<name>)", "Obs: <lat>°, <lon>°" or
     "Obs: (não configurado)" (not configured).
  2. **Pixel value** under the cursor: "Value: --", then "Val …".
  3. **Sky coordinates** under the cursor: "RA -- Dec --", then "RA … Dec …" in decimal
     degrees (needs a solved image).
  4. **Image count**: "No images" or "N image(s)".
- Small pop-up **toasts** confirm events such as "Settings saved." or "Exported: …".

🇧🇷 **Português** — A faixa bem embaixo da janela.

- **À esquerda: a mensagem de etapa.** Diz o que fazer em seguida, por exemplo "Pronto",
  "Step 1: Images loaded — Run Data Reduction", "Step 2 done - show Known Objects
  (Ctrl+K)". Mensagens de progresso também aparecem aqui ("Detecting stars... 2/4", velocidade
  do download do MPCORB).
- **À direita, da esquerda para a direita:**
  1. **Observatório**: "Obs: auto (…)", "Obs: <código> (<nome>)", "Obs: <lat>°, <lon>°" ou
     "Obs: (não configurado)".
  2. **Valor do pixel** sob o cursor: "Valor: --", depois "Val …".
  3. **Coordenadas no céu** sob o cursor: "RA -- Dec --", depois "RA … Dec …" em graus
     decimais (precisa de imagem resolvida).
  4. **Número de imagens**: "Sem imagens" ou "N imagem(ns)".
- Pequenos avisos **toast** confirmam eventos como "Settings saved." ou "Exported: …".

---

## Blink view / Visão de piscar

🟢

🇬🇧 **English** — Opened with **Utilities → Begin Blink Mode** (`Ctrl+B`). It fills the window and
shows the images one after the other. Stars stay still; an asteroid jumps.

- **Top bar**: image counter, file name, JD (Julian Date, a continuous day count used in
  astronomy) and the RA/Dec under the cursor.
- **Bottom bar**: `◀` previous, `⏸` play/pause, `▶` next; **■ Stop**; **Sharpen: Off**
  (cycles Off → USM (unsharp mask) → LoG (Laplacian) → Off, to make faint objects stand out);
  **Speed: 500ms** slider (50–2000 ms); and **four** numbered thumbnails. The blink view shows
  at most 4 images.
- **Keys**: `Space` play/pause · `Left`/`Right` previous/next image (or pan when zoomed) ·
  `Esc` stop blink · `+`, `=`, `-`, `0`, `Up`, `Down` zoom and pan.
- **Measuring**: turn on **Measure Object** (`M`) and click the object. If the blink is
  playing, AstroFind asks **Parar e Medir** or **Continuar Blink** first.

🇧🇷 **Português** — Aberta com **Utilitários → Ativar Modo de Piscagem** (`Ctrl+B`). Ocupa a janela e
mostra as imagens uma após a outra. As estrelas ficam paradas; o asteroide pula.

- **Barra de cima**: contador, nome do arquivo, JD (Data Juliana, contagem contínua de dias
  usada em astronomia) e AR/Dec sob o cursor.
- **Barra de baixo**: `◀` anterior, `⏸` tocar/pausar, `▶` próxima; **■ Parar**; **Nitidez:
  Desativado** (alterna Desativado → USM (máscara de nitidez) → LoG (Laplaciano) →
  Desativado, para realçar objetos fracos); controle **Velocidade: 500ms** (50–2000 ms); e
  **quatro** miniaturas numeradas. A visão de piscar mostra no máximo 4 imagens.
- **Teclas**: `Espaço` tocar/pausar · `Esquerda`/`Direita` imagem anterior/próxima (ou mover
  quando ampliado) · `Esc` parar · `+`, `=`, `-`, `0`, `Cima`, `Baixo` zoom e mover.
- **Medir**: ligue **Medir Objeto** (`M`) e clique no objeto. Se o piscar estiver tocando, o
  AstroFind pergunta antes **Parar e Medir** ou **Continuar Blink**.

---

## Image view: mouse and keyboard / Visão da imagem: mouse e teclado

🟢

🇬🇧 **English**

| Action | Result |
|---|---|
| Mouse wheel | Zoom around the cursor (×1.15 per step, 0.05× to 32×) |
| Middle-button drag | Pan (move the image) |
| Left-button drag (more than 4 px) | Pan |
| Left click (no drag) | Depends on the tool: select (`S`), measure (`A`/`M`), annotate (`N`) |
| Arrow keys | Pan 10 px |
| `+` / `=` / `-` | Zoom in / in / out |
| `0` | Fit image to window |
| `Esc` | Cancel Region Statistics and clear the catalog selection |

**Marker colours:** cyan circle = detected star · orange rotated ellipse = streak/trail
(fast object or satellite) · magenta double circle = blended source (two stars touching) ·
yellow cross = catalog star · green circle = known asteroid (light blue = planet, orange =
comet) with its number or name · "Cand #N" = moving-object candidate · red = selected item ·
magenta text = your annotations. **Magenta pixels** are pixels with no valid value (NaN). An
empty window says "No image loaded / Use File → Import Images…".

🇧🇷 **Português**

| Ação | Resultado |
|---|---|
| Roda do mouse | Zoom em volta do cursor (×1,15 por passo, de 0,05× a 32×) |
| Arrastar com o botão do meio | Mover a imagem |
| Arrastar com o botão esquerdo (mais de 4 px) | Mover a imagem |
| Clique esquerdo (sem arrastar) | Depende da ferramenta: selecionar (`S`), medir (`A`/`M`), anotar (`N`) |
| Setas | Mover 10 px |
| `+` / `=` / `-` | Aproximar / aproximar / afastar |
| `0` | Encaixar a imagem na janela |
| `Esc` | Cancela Estatísticas de Região e limpa a seleção do catálogo |

**Cores das marcas:** círculo ciano = estrela detectada · elipse laranja girada = traço
(objeto rápido ou satélite) · círculo duplo magenta = fonte misturada (duas estrelas
encostadas) · cruz amarela = estrela de catálogo · círculo verde = asteroide conhecido (azul
claro = planeta, laranja = cometa) com número ou nome · "Cand #N" = candidato a objeto em
movimento · vermelho = item selecionado · texto magenta = suas anotações. **Pixels magenta**
são pixels sem valor válido (NaN).

---

## All keyboard shortcuts / Todos os atalhos de teclado

🟢

🇬🇧 **English** — Sorted by key: single keys first, then `Ctrl+…`, then `Ctrl+Shift+…`.
"Image view" and "Blink" keys work only when that view has the keyboard focus (click on it
first).

🇧🇷 **Português** — Ordenado pela tecla: teclas sozinhas primeiro, depois `Ctrl+…`, depois
`Ctrl+Shift+…`. As teclas de "Visão da imagem" e "Piscar" só funcionam quando essa visão tem o
foco do teclado (clique nela antes).

| Key / Tecla | Action (EN) | Ação (PT) | Where / Onde |
|---|---|---|---|
| `+` / `=` | Zoom in | Aproximar | Image view, Blink |
| `-` | Zoom out | Afastar | Image view, Blink |
| `0` | Fit image to window | Encaixar na janela | Image view, Blink |
| `A` | Aperture Tool (measure) | Ferramenta Abertura (medir) | Edit menu |
| `Arrow keys` / `Setas` | Pan 10 px | Mover 10 px | Image view |
| `Left` / `Right` | Previous / next image (pan when zoomed) | Imagem anterior / próxima (mover com zoom) | Blink |
| `Up` / `Down` | Pan | Mover | Blink |
| `Esc` | Cancel region mode, clear selection / stop blink | Cancela região, limpa seleção / para o piscar | Image view / Blink |
| `F1` | Help Topics | Tópicos de Ajuda | Help menu |
| `M` | Measure Object on/off | Medir Objeto liga/desliga | Toolbar |
| `N` | Annotate Tool | Ferramenta Anotação | Edit menu |
| `S` | Select Tool | Ferramenta Seleção | Edit menu |
| `Space` / `Espaço` | Play / pause | Tocar / pausar | Blink |
| `Ctrl+,` | Settings | Configurações | File menu |
| `Ctrl+.` | Stop Data Reduction | Parar Redução de Dados | Astrometry Tools menu |
| `Ctrl++` | Zoom In | Ampliar Mais | Image Tools menu |
| `Ctrl+-` | Zoom Out | Ampliar Menos | Image Tools menu |
| `Ctrl+A` | Run Data Reduction | Executar Redução de Dados | Astrometry Tools menu |
| `Ctrl+B` | Begin Blink Mode | Ativar Modo de Piscagem | Utilities menu |
| `Ctrl+C` | Copy Image | Copiar Imagem | Edit menu |
| `Ctrl+E` | Ecliptic / Galactic Overlay | Sobreposição Eclíptica / Galáctica | Utilities menu |
| `Ctrl+F` | Fit to Window | Ajustar à Janela | Image Tools menu |
| `Ctrl+F9` | End Blink Mode | Encerrar Modo de Piscagem | Utilities menu |
| `Ctrl+F10` | Step to next image | Próxima imagem | Blink toolbar |
| `Ctrl+F11` | Start blink playback | Iniciar piscar | Blink toolbar |
| `Ctrl+G` | Magnifier Tool | Ferramenta Lupa | Image Tools menu |
| `Ctrl+I` | Invert Colors | Inverter Cores | Image Tools menu |
| `Ctrl+K` | Show Known Objects | Mostrar Objetos Conhecidos | Utilities menu |
| `Ctrl+L` | Import Images | Importar Imagens | File menu |
| `Ctrl+M` | Detect Moving Objects | Detectar Objetos em Movimento | Astrometry Tools menu |
| `Ctrl+O` | Open Project | Abrir Projeto | File menu |
| `Ctrl+Q` | Exit | Sair | File menu |
| `Ctrl+R` | Reset Session Files (no save prompt) | Limpar Arquivos da Sessão (sem perguntar) | File menu |
| `Ctrl+S` | Save Project | Salvar Projeto | File menu |
| `Ctrl+T` | Stack Images (Track & Stack) | Empilhar Imagens | Astrometry Tools menu |
| `Ctrl+W` | Close Loaded Images (= Reset Session Files, no save prompt) | Fechar Imagens (sem perguntar) | File menu |
| `Ctrl+Y` | Redo (alternative) | Refazer (alternativo) | Edit menu |
| `Ctrl+Z` | Undo | Desfazer | Edit menu |
| `Ctrl+Shift+C` | Compare Sessions | Comparar Sessões | Utilities menu |
| `Ctrl+Shift+G` | Growth Curve | Curva de Crescimento | Utilities menu |
| `Ctrl+Shift+L` | Light Curve | Curva de Luz | Utilities menu |
| `Ctrl+Shift+R` | Region Statistics | Estatísticas de Região | Image Tools menu |
| `Ctrl+Shift+S` | Save Project As | Salvar Projeto Como | File menu |
| `Ctrl+Shift+T` | Toggle Night/Day/Auto theme | Alternar tema Noite/Dia/Automático | Standard toolbar (theme button) |
| `Ctrl+Shift+Z` | Redo | Refazer | Edit menu |
| `Shift+Ctrl+F10` | Step to previous image | Imagem anterior | Blink toolbar |

> 💡 **Shortcut clashes in 1.1.0 / Atalhos em conflito na 1.1.0**
>
> 🇬🇧 In 1.1.0 and earlier two keys were given to two actions at once, so pressing them could
> do **nothing**: `Ctrl+L` (Import Images and Light Curve) and `Ctrl+Shift+T` (the Window menu
> item and the toolbar theme button). Version 1.2.0 fixes both: Light Curve
> moved to `Ctrl+Shift+L`, and `Ctrl+Shift+T` belongs only to the toolbar theme button. If you
> are still on 1.1.0, use the menu entries or the toolbar buttons instead.
>
> 🇧🇷 Na 1.1.0 e anteriores duas teclas estavam em duas ações ao mesmo tempo, e apertá-las
> podia não fazer **nada**: `Ctrl+L` (Carregar Imagens e Curva de Luz) e `Ctrl+Shift+T` (o item
> do menu Janelas e o botão de tema da barra). A versão 1.2.0 corrige as
> duas: a Curva de Luz passou para `Ctrl+Shift+L`, e o `Ctrl+Shift+T` é só do botão de tema da
> barra. Se ainda usa a 1.1.0, use os itens de menu ou os botões da barra.

See also / Veja também: [Settings](https://github.com/petrinhu/astrofind/wiki/Settings), [Manual](https://github.com/petrinhu/astrofind/wiki/Manual), [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial), [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting), [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).
