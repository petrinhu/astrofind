# Quick Start / Início Rápido

🇬🇧 **Who this page is for:** absolute beginners. It takes you from zero to your first asteroid
measurement in 10 steps, using only the main menus. You do not need to know astronomy or Linux
well. Any word you don't know is explained in the [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary). When you want more detail on a
feature, go to [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial).

🇧🇷 **Para quem é esta página:** iniciantes absolutos. Ela leva você do zero até a sua primeira
medição de asteroide em 10 passos, usando só os menus principais. Não precisa saber muito de
astronomia nem de Linux. Qualquer palavra desconhecida está explicada no
[Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary). Quando quiser mais detalhes de uma função, vá para o [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial).

| Step / Passo | What / O quê |
|---|---|
| 0 | What you need / O que você precisa |
| 1 | Install / Instalar |
| 2 | First start: Setup Wizard / Primeira execução: Assistente de Configuração |
| 3 | Set your location / Configurar sua localização |
| 4 | Load the images / Carregar as imagens |
| 5 | Adjust the display / Ajustar a exibição |
| 6 | Data Reduction / Redução de Dados |
| 7 | Known objects / Objetos conhecidos |
| 8 | Blink / Piscar |
| 9 | Measure / Medir |
| 10 | ADES report / Relatório ADES |

> 🇬🇧 💡 **Tip:** the **Workflow** panel on the left of the window shows these same stages
> (1 Load Images → 6 ADES Report). The next stage always has a **▶ Do it** button. You can
> follow this page *or* just press that button each time.
>
> 🇧🇷 💡 **Dica:** o painel **Fluxo de Trabalho** à esquerda da janela mostra essas mesmas etapas
> (1 Carregar Imagens → 6 Relatório ADES). A próxima etapa sempre tem um botão **▶ Executar**.
> Você pode seguir esta página *ou* só apertar esse botão a cada vez.

---

## Step 0 — What you need / Passo 0 — O que você precisa

🟢 Beginner / Iniciante

🇬🇧 **English**

- A computer with **Linux** (AstroFind is Linux only). Tested on Ubuntu 24.04, Debian 12,
  Fedora, Arch, CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9, Linux Mint 22, Pop!_OS and
  Zorin.
- An internet connection. It is used to identify the star field ("plate solving", see Step 6)
  and to find known asteroids (Step 7).
- **At least 3 images of the same patch of sky, taken several minutes apart** (for example
  4 images, 10–20 minutes apart). An asteroid needs time to move between the pictures. AstroFind
  needs 3 frames for its automatic moving-object search, and the blink view shows up to 4.
- The images are usually **FITS** files (`.fits`, `.fit`, `.fts`: the standard file type of
  astronomy, which keeps the exact brightness of every pixel plus a text "header" with the date,
  time and telescope). AstroFind also opens other types directly, including **DSLR camera RAW**
  files (`.cr2`, `.nef`, `.arw`, …) and NASA **PDS** files (`.img`/`.lbl`, `.xml`). See
  [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

**No images?** AstroFind does not come with sample images.
- If you take part in an **IASC** (International Astronomical Search Collaboration) campaign,
  your team receives real image sets. You must register with IASC first.
- Otherwise, **ask your campaign coordinator, teacher or observatory** for a set of images of
  one field taken minutes apart.

🇧🇷 **Português**

- Um computador com **Linux** (o AstroFind só roda em Linux). Testado em Ubuntu 24.04,
  Debian 12, Fedora, Arch, CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9, Linux Mint 22,
  Pop!_OS e Zorin.
- Conexão com a internet. Ela é usada para identificar o campo de estrelas ("plate solving",
  veja o Passo 6) e para achar asteroides conhecidos (Passo 7).
- **Pelo menos 3 imagens do mesmo pedaço do céu, tiradas com alguns minutos de intervalo**
  (por exemplo 4 imagens, com 10–20 minutos entre elas). O asteroide precisa de tempo para se
  mover entre as fotos. A busca automática de objetos em movimento precisa de 3 quadros, e a
  tela de piscar mostra até 4.
- As imagens normalmente são arquivos **FITS** (`.fits`, `.fit`, `.fts`: o tipo de arquivo padrão
  da astronomia, que guarda o brilho exato de cada pixel e um "cabeçalho" de texto com data, hora
  e telescópio). O AstroFind também abre outros tipos direto, incluindo arquivos **RAW de câmera
  DSLR** (`.cr2`, `.nef`, `.arw`, …) e arquivos **PDS** da NASA (`.img`/`.lbl`, `.xml`). Veja
  [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

**Não tem imagens?** O AstroFind não vem com imagens de exemplo.
- Se você participa de uma campanha do **IASC** (International Astronomical Search
  Collaboration), sua equipe recebe conjuntos de imagens reais. É preciso se cadastrar no IASC
  antes.
- Fora isso, **peça ao coordenador da campanha, ao professor ou ao observatório** um conjunto de
  imagens de um campo tiradas com minutos de intervalo.

---

## Step 1 — Install AstroFind / Passo 1 — Instalar o AstroFind

🟢 Beginner / Iniciante

🇬🇧 **English**
"Installing" means putting the program on your computer. The easiest way is the universal
installer. Open a **terminal** (the text window where you type commands) and type these three
lines, pressing Enter after each one:

```bash
# Download the installer script for version 1.1.0
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.1.0/packaging/install.sh
# Allow the file to run as a program
chmod +x install.sh
# Run it: it detects your Linux and installs the right package
./install.sh
```

The installer may ask for your password (to install packages). For `.rpm`/`.deb`/Arch packages
or building from source, see
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md) and [Installation](https://github.com/petrinhu/astrofind/wiki/Installation).

🇧🇷 **Português**
"Instalar" significa colocar o programa no seu computador. O jeito mais fácil é o instalador
universal. Abra um **terminal** (a janela de texto onde você digita comandos) e digite estas três
linhas, apertando Enter depois de cada uma:

```bash
# Baixa o script instalador da versão 1.1.0
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.1.0/packaging/install.sh
# Permite que o arquivo rode como programa
chmod +x install.sh
# Executa: ele detecta seu Linux e instala o pacote certo
./install.sh
```

O instalador pode pedir sua senha (para instalar pacotes). Para pacotes `.rpm`/`.deb`/Arch ou para
compilar do código-fonte, veja o
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md) e [Installation](https://github.com/petrinhu/astrofind/wiki/Installation).

---

## Step 2 — First start: the Setup Wizard / Passo 2 — Primeira execução: o Assistente de Configuração

🟢 Beginner / Iniciante

🇬🇧 **English**
Open AstroFind from your applications menu (search "AstroFind"), or type `AstroFind` in a
terminal. The **AstroFind Setup Wizard** opens. It has 3 pages; each has a **Skip this step**
button.

1. **Step 1 of 3 — Observer Identity:** your **MPC Station Code** (a 3-character code of your
   observatory, e.g. `568`; leave it empty if you have none), **Your name** and **Telescope**.
2. **Step 2 of 3 — Plate-solving API Key:** a free key from nova.astrometry.net (click the
   link on the page to register). It lets AstroFind identify your star field online. Without
   it you can still work, but plate solving will not run.
3. **Step 3 of 3 — Asteroid Catalog (optional):** **Download now (~200 MB)** gets the
   `MPCORB.DAT` asteroid list for offline use. You can skip it; it is only a backup.

**What you should see:** the main window, dark (Night theme), with the Workflow panel on the
left, and the Images and Log panels at the bottom.

You can reopen the wizard at any time: **Help → Setup Wizard…**.

🇧🇷 **Português**
Abra o AstroFind pelo menu de aplicativos (procure "AstroFind") ou digite `AstroFind` num
terminal. O **Assistente de configuração do AstroFind** abre. Ele tem 3 páginas; cada uma tem o
botão **Pular esta etapa**.

1. **Etapa 1 de 3 — Identidade do observador:** seu **código de estação MPC** (código de 3
   caracteres do seu observatório, ex.: `568`; deixe vazio se não tiver), **Seu nome** e
   **Telescópio**.
2. **Etapa 2 de 3 — Chave de API para solução de campo:** uma chave gratuita do
   nova.astrometry.net (clique no link da página para se cadastrar). Ela permite ao AstroFind
   identificar seu campo de estrelas online. Sem ela dá para trabalhar, mas o plate solving não
   roda.
3. **Etapa 3 de 3 — Catálogo de asteroides (opcional):** **Baixar agora (~200 MB)** baixa a lista
   de asteroides `MPCORB.DAT` para uso offline. Pode pular; é só um reserva.

**O que você deve ver:** a janela principal, escura (tema Noite), com o painel Fluxo de Trabalho
à esquerda e os painéis Imagens e Registro embaixo.

Você pode reabrir o assistente quando quiser: **Ajuda → Assistente de Configuração…**.

---

## Step 3 — Set your location / Passo 3 — Configurar sua localização

🟢 Beginner / Iniciante

🇬🇧 **English**
AstroFind needs to know **where on Earth** the images were taken, to correct for the atmosphere.

1. Open **File → Settings...** (`Ctrl+,`) / **Arquivo → Configurações...**.
2. On the **Observer** tab, in the group **Location (topocentric correction)**, choose one:
   - **Automático (do FITS)** if your images already contain the site (it is greyed out
     otherwise);
   - **Observatório predefinido** to pick a known observatory from a list;
   - **Coordenadas manuais** to type latitude (+N), longitude (+E) and altitude.
3. Click **OK**. A small message "Settings saved." appears.

> 🇬🇧 ⚠️ **Watch out:** these four options are shown in Portuguese even in the English interface.
> If you skip this step and the images have no location, Data Reduction warns
> **"Localização não configurada"**: the positions would be computed as if seen from the centre
> of the Earth. For images from a space telescope (e.g. Hubble) AstroFind sets this up by itself.

🇧🇷 **Português**
O AstroFind precisa saber **de onde na Terra** as imagens foram tiradas, para corrigir o efeito
da atmosfera.

1. Abra **Arquivo → Configurações...** (`Ctrl+,`).
2. Na aba **Observador**, no grupo **Localização (correção topocêntrica)**, escolha uma opção:
   - **Automático (do FITS)** se suas imagens já trazem o local (fica cinza se não trouxerem);
   - **Observatório predefinido** para escolher um observatório conhecido numa lista;
   - **Coordenadas manuais** para digitar latitude (+N), longitude (+E) e altitude.
3. Clique em **OK**. Aparece a mensagem "Settings saved.".

> 🇧🇷 ⚠️ **Atenção:** se você pular este passo e as imagens não tiverem localização, a Redução de
> Dados avisa **"Localização não configurada"**: as posições seriam calculadas como se vistas do
> centro da Terra. Para imagens de telescópio espacial (ex.: Hubble) o AstroFind configura isso
> sozinho.

---

## Step 4 — Load your images / Passo 4 — Carregar suas imagens

🟢 Beginner / Iniciante

🇬🇧 **English**
1. Click **File → Load Images...** / **Arquivo → Carregar Imagens...** (or the Workflow step
   1 **▶ Do it** button).
2. In the window "Load Astronomical Images", select **all** your images at once (Ctrl+click or
   Shift+click), then **Open**. ZIP files and other archives are unpacked automatically.

> 🇬🇧 💡 **Shortcut:** `Ctrl+L` also opens this window. In version 1.1.0 and earlier the same
> key was also used by Tools → Light Curve… and could do nothing; there, use the menu.

**What you should see:**
- One window per image, arranged side by side. Thumbnails appear in the **Images** bar at the
  bottom.
- A notice **"Imagens carregadas"** saying how many images were loaded and whether they are
  black-and-white or colour. Click OK. (Untick "Mostrar sempre este aviso" to stop it.)
- A bar above the images with green ✓ or orange ⚠ chips (pixel scale, date/time, location…).
  An orange chip has a button that opens Settings.
- Status bar: "Step 1: Images loaded — Run Data Reduction".

If you load again while images are open, AstroFind asks **Acréscimo** (add to the current
images) or **Novo Projeto** (start over).

🇧🇷 **Português**
1. Clique em **Arquivo → Carregar Imagens...** (ou no botão **▶ Executar** da etapa 1 do Fluxo
   de Trabalho).
2. Na janela "Carregar Imagens Astronômicas", selecione **todas** as imagens de uma vez
   (Ctrl+clique ou Shift+clique) e clique em **Abrir**. Arquivos ZIP e outros compactados são
   extraídos automaticamente.

> 🇧🇷 💡 **Atalho:** `Ctrl+L` também abre esta janela. Na versão 1.1.0 e anteriores a mesma
> tecla também era usada por Ferramentas → Curva de Luz… e podia não fazer nada; lá, use o
> menu.

**O que você deve ver:**
- Uma janela por imagem, lado a lado. Miniaturas aparecem na barra **Imagens** embaixo.
- Um aviso **"Imagens carregadas"** dizendo quantas imagens foram carregadas e se são preto e
  branco ou coloridas. Clique em OK. (Desmarque "Mostrar sempre este aviso" para não ver mais.)
- Uma barra acima das imagens com etiquetas ✓ verdes ou ⚠ laranja (escala de pixel, data/hora,
  localização…). Uma etiqueta laranja tem um botão que abre as Configurações.
- Barra de status: "Step 1: Images loaded — Run Data Reduction".

Se você carregar de novo com imagens abertas, o AstroFind pergunta **Acréscimo** (somar às
imagens atuais) ou **Novo Projeto** (começar do zero).

---

## Step 5 — Adjust the display (if the image looks black) / Passo 5 — Ajustar a exibição (se a imagem parecer preta)

🟢 Beginner / Iniciante

🇬🇧 **English**
Astronomy images hold more brightness levels than a screen can show, so they may look almost
black or white. This step only changes **how the image looks**, never the measured data.

1. Click on one image, then **Images → Background and Range...** / **Imagens → Fundo e
   Intervalo...**.
2. Click **⟳ Auto**. If needed, move the **Background (min)** and **Range (max)** sliders, or
   try another "Função de transferência" such as **Asinh** or **Raiz Quadrada (Sqrt)**.
3. Tick **Apply to all images in session**, then **OK**.

**What you should see:** stars as clear dots on a grey-black sky.

🇧🇷 **Português**
Imagens astronômicas têm mais níveis de brilho do que a tela consegue mostrar, então podem
parecer quase pretas ou brancas. Este passo só muda **a aparência**, nunca os dados medidos.

1. Clique numa imagem e depois em **Imagens → Fundo e Intervalo...**.
2. Clique em **⟳ Auto**. Se precisar, mexa nos controles **Fundo (mín)** e **Intervalo (máx)** ou
   teste outra "Função de transferência", como **Asinh** ou **Raiz Quadrada (Sqrt)**.
3. Marque **Aplicar a todas as imagens da sessão** e clique em **OK**.

**O que você deve ver:** estrelas como pontos nítidos num céu cinza-escuro.

---

## Step 6 — Data Reduction (find stars, identify the field) / Passo 6 — Redução de Dados (achar estrelas, identificar o campo)

🟢 Beginner / Iniciante

🇬🇧 **English**
Click **Astrometry → Data Reduction...** (`Ctrl+A`) / **Astrometria → Redução de Dados...**, or
Workflow step 2. AstroFind finds the stars in each image, then **plate-solves** it: it compares
the star pattern with a sky map to learn exactly where the image points. The result is a
**WCS** (a formula that turns a pixel into sky coordinates, RA/Dec). See [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

- If no API key is stored, a box asks for it ("Enter your free API key from
  nova.astrometry.net:").
- Online solving can take a few minutes per image. To cancel: **Astrometry → Stop Data
  Reduction** (`Ctrl+.`).
- Images that already contain a WCS are not solved again.

**What you should see:** in the **Log** panel, lines like "Image 1: 230 stars found" and
"Solved! RA=… Dec=…", and at the end "Redução concluída: N resolvidas, …". Cyan circles mark the
detected stars. The **Image Catalog** panel opens on the right. Status bar: "Step 2 done — run
Known Object Overlay (Ctrl+K)".

🇧🇷 **Português**
Clique em **Astrometria → Redução de Dados...** (`Ctrl+A`), ou na etapa 2 do Fluxo de Trabalho. O
AstroFind acha as estrelas de cada imagem e depois faz o **plate solving**: compara o desenho
das estrelas com um mapa do céu para saber exatamente para onde a imagem aponta. O resultado é
um **WCS** (uma fórmula que converte um pixel em coordenadas do céu, AR/Dec). Veja o
[Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

- Se não houver chave de API salva, uma caixa pede a chave ("Enter your free API key from
  nova.astrometry.net:").
- A solução online pode levar alguns minutos por imagem. Para cancelar: **Astrometria → Parar
  Redução de Dados** (`Ctrl+.`).
- Imagens que já têm WCS não são resolvidas de novo.

**O que você deve ver:** no painel **Registro**, linhas como "Image 1: 230 stars found" e
"Solved! RA=… Dec=…", e no fim "Redução concluída: N resolvidas, …". Círculos ciano marcam as
estrelas detectadas. O painel **Catálogo de Imagem** abre à direita. Barra de status: "Step 2
done — run Known Object Overlay (Ctrl+K)".

---

## Step 7 — Mark known objects / Passo 7 — Marcar objetos conhecidos

🟢 Beginner / Iniciante

🇬🇧 **English**
Click **Tools → Known Object Overlay** (`Ctrl+K`) / **Ferramentas → Sobreposição de Objetos
Conhecidos**, or Workflow step 3. AstroFind downloads reference stars (UCAC4 catalog by default)
and asks the SkyBoT service which known asteroids and comets are in your field.

**What you should see:** yellow crosses (catalog reference stars) and green circles labelled
with a number or name (known asteroids). The Log shows "SkyBoT: N known object(s) in field".
Zero known objects is normal for many fields.

🇧🇷 **Português**
Clique em **Ferramentas → Sobreposição de Objetos Conhecidos** (`Ctrl+K`), ou na etapa 3 do Fluxo
de Trabalho. O AstroFind baixa estrelas de referência (catálogo UCAC4 por padrão) e pergunta ao
serviço SkyBoT quais asteroides e cometas conhecidos estão no seu campo.

**O que você deve ver:** cruzes amarelas (estrelas de referência do catálogo) e círculos verdes
com número ou nome (asteroides conhecidos). O Registro mostra "SkyBoT: N known object(s) in
field". Nenhum objeto conhecido é normal em muitos campos.

---

## Step 8 — Blink to find the moving dot / Passo 8 — Piscar para achar o ponto que se move

🟢 Beginner / Iniciante

🇬🇧 **English**
Click **Tools → Blink Images** (`Ctrl+B`) / **Ferramentas → Piscar Imagens**, or Workflow step 4.
A large "Blink View" window shows your images one after another in the same place. Stars stay
still; an **asteroid jumps** a little each frame.

- `Space` pauses/plays; `←`/`→` go to the previous/next image.
- Use the **Speed** slider if it is too fast.
- **Sharpen: Off** cycles USM → LoG → Off to make faint dots easier to see.

🇧🇷 **Português**
Clique em **Ferramentas → Piscar Imagens** (`Ctrl+B`), ou na etapa 4 do Fluxo de Trabalho. Uma
janela grande "Visão de piscar" mostra as imagens uma depois da outra no mesmo lugar. As
estrelas ficam paradas; um **asteroide pula** um pouco a cada quadro.

- `Espaço` pausa/continua; `←`/`→` vão para a imagem anterior/seguinte.
- Use o controle **Speed** se estiver rápido demais.
- **Nitidez: Desativado** alterna USM → LoG → desligado para realçar pontos fracos.

---

## Step 9 — Measure the object / Passo 9 — Medir o objeto

🟢 Beginner / Iniciante

🇬🇧 **English**
1. Press **`M`** (the **Measure Object** toolbar button turns orange), or Workflow step 5, or
   **Edit → Aperture Tool** (`A`) / **Editar → Ferramenta Abertura**.
2. Click exactly on the moving dot. If the blink is playing, choose **Parar e Medir** (stop and
   measure).
3. The **Verification** window opens with RA, Dec, Mag, FWHM (star size) and SNR (signal-to-noise
   ratio). The list "Objetos conhecidos próximos" shows known objects nearby; double-click one to
   use its name. Otherwise you may type a name in **Object**.
4. Click **✓ Aceitar** (keep the window) or **✓ Aceitar e Fechar**. **✗ Rejeitar** discards it.
5. Repeat on the same object in **every** image.

**What you should see:** the **Observations** panel at the bottom, one row per accepted
measurement. Made a mistake? **Edit → Undo** (`Ctrl+Z`).

🇧🇷 **Português**
1. Aperte **`M`** (o botão **Measure Object** da barra de ferramentas fica laranja), ou a etapa 5
   do Fluxo de Trabalho, ou **Editar → Ferramenta Abertura** (`A`).
2. Clique exatamente no ponto que se move. Se o piscar estiver rodando, escolha **Parar e Medir**.
3. A janela **Verificação** abre com AR, Dec, Mag, FWHM (tamanho da estrela) e SNR (razão
   sinal-ruído). A lista "Objetos conhecidos próximos" mostra objetos conhecidos por perto; dê
   duplo clique em um para usar o nome dele. Se não, você pode digitar um nome em **Object**.
4. Clique em **✓ Aceitar** (mantém a janela) ou **✓ Aceitar e Fechar**. **✗ Rejeitar** descarta.
5. Repita no mesmo objeto em **todas** as imagens.

**O que você deve ver:** o painel **Observações** embaixo, uma linha por medição aceita. Errou?
**Editar → Desfazer** (`Ctrl+Z`).

---

## Step 10 — The ADES report / Passo 10 — O relatório ADES

🟢 Beginner / Iniciante

🇬🇧 **English**
**ADES** is the report format the Minor Planet Center (MPC) accepts.

1. Click **File → View ADES Report File** / **Arquivo → Ver Arquivo de Relatório ADES**, or
   Workflow step 6.
2. The window "ADES 2022 Report Preview" shows the report in **XML** and **PSV** tabs.
3. **Save…** saves the visible tab; **Export PDF…** makes a printable summary.
4. To send: **Submit to MPC (HTTP)** or **Submit to MPC (Email)**. For a school project, if a
   teacher e-mail is set in Settings, **Enviar para Professor**.
5. Save your work: **File → Save Project** (`Ctrl+S`) creates a `.gus` project file.

> 🇬🇧 ⚠️ **Watch out:** AstroFind has not yet been validated end to end with real observatory
> data. **Check your results before sending anything to the MPC.** Beginners should send to
> their teacher or campaign coordinator first.

🇧🇷 **Português**
**ADES** é o formato de relatório que o Minor Planet Center (MPC) aceita.

1. Clique em **Arquivo → Ver Arquivo de Relatório ADES**, ou na etapa 6 do Fluxo de Trabalho.
2. A janela "Pré-visualização do relatório ADES 2022" mostra o relatório nas abas **XML** e
   **PSV**.
3. **Save…** salva a aba visível; **Export PDF…** gera um resumo para imprimir.
4. Para enviar: **Submit to MPC (HTTP)** ou **Submit to MPC (Email)**. Em projeto escolar, se um
   e-mail de professor estiver nas Configurações, **Enviar para Professor**.
5. Salve seu trabalho: **Arquivo → Salvar Projeto** (`Ctrl+S`) cria um arquivo de projeto `.gus`.

> 🇧🇷 ⚠️ **Atenção:** o AstroFind ainda não foi validado de ponta a ponta com dados reais de
> observatório. **Confira seus resultados antes de enviar qualquer coisa ao MPC.** Iniciantes
> devem enviar primeiro ao professor ou ao coordenador da campanha.

---

## Shortcut: "Fluxo automático" / Atalho: "Fluxo automático"

🟢 Beginner / Iniciante

🇬🇧 **English**
At the bottom of the Workflow panel there is a checkbox **Fluxo automático** (shown in
Portuguese in both languages). When ticked, loading images runs Steps 6, 7 and 8 by themselves:
Data Reduction, then Known Object Overlay, then Blink. You only measure and report.

🇧🇷 **Português**
No fim do painel Fluxo de Trabalho há a caixa **Fluxo automático**. Marcada, carregar as imagens
já roda sozinho os Passos 6, 7 e 8: Redução de Dados, depois Sobreposição de Objetos Conhecidos,
depois Piscar. Você só mede e gera o relatório.

---

## What next? / E agora?

🇬🇧 **English**
- Every feature explained (calibration, stacking, automatic detection, light curves…):
  [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial).
- The full observing workflow and all settings: [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) and [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).
- Something went wrong: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) and [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).
- Which files can be opened: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

🇧🇷 **Português**
- Todas as funções explicadas (calibração, empilhamento, detecção automática, curvas de luz…):
  [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial).
- O fluxo completo de observação e todas as configurações: [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) e [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).
- Deu algo errado: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) e [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).
- Quais arquivos podem ser abertos: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).
