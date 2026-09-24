# User Manual / Manual do Usuário

🇬🇧 **Who this page is for.** This is the complete manual for AstroFind 1.1.0. It follows the
real path an observer takes: **plan → load → calibrate → reduce → find → measure → report →
submit**. It is written for first-time reporters as well as experienced Astrometrica/MPC
observers: each section has a level badge (🟢 Beginner, 🟡 Intermediate, 🔴 Advanced), so you
can skip what you already know. For the fastest path see [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start); for every button in
detail see [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) and [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference); for words you don't know see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Para quem é esta página.** Este é o manual completo do AstroFind 1.1.0. Ele segue o
caminho real de quem observa: **planejar → carregar → calibrar → reduzir → encontrar → medir →
relatar → enviar**. Serve para quem vai relatar pela primeira vez e também para quem já usa o
Astrometrica e envia ao MPC: cada seção tem um selo de nível (🟢 Iniciante, 🟡 Intermediário,
🔴 Avançado), então pule o que você já sabe. Para o caminho mais rápido veja o [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start);
para cada botão em detalhe, o [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) e a [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference); para palavras desconhecidas,
o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

> ⚠️ **Watch out / Atenção** — AstroFind has not yet been validated end to end with real
> observatory data. **Always double-check positions, magnitudes and times before you submit to
> the MPC.** / O AstroFind ainda não foi validado de ponta a ponta com dados reais de
> observatório. **Sempre confira posições, magnitudes e horários antes de enviar ao MPC.**

**Contents / Conteúdo**

0. [Key ideas before your first report / Ideias-chave antes do primeiro relatório](#part-0--key-ideas-before-your-first-report--parte-0--ideias-chave-antes-do-primeiro-relatório)
1. [Planning / Planejamento](#part-1--before-you-open-astrofind-planning-your-observation--parte-1--antes-de-abrir-o-astrofind-planejando-sua-observação)
2. [First-time setup / Configuração inicial](#part-2--first-time-setup--parte-2--configuração-inicial)
3. [Loading images / Carregando imagens](#part-3--loading-images--parte-3--carregando-imagens)
4. [Calibration / Calibração](#part-4--calibration-bias-dark-and-flat--parte-4--calibração-bias-dark-e-flat)
5. [Data Reduction / Redução de Dados](#part-5--data-reduction-background-star-detection-and-plate-solving--parte-5--redução-de-dados-fundo-detecção-de-estrelas-e-plate-solving)
6. [Reference stars and known objects / Estrelas de referência e objetos conhecidos](#part-6--reference-stars-and-known-objects--parte-6--estrelas-de-referência-e-objetos-conhecidos)
7. [Finding the moving object / Encontrando o objeto em movimento](#part-7--finding-the-moving-object--parte-7--encontrando-o-objeto-em-movimento)
8. [Stacking / Empilhamento](#part-8--optional-stacking-for-faint-objects--parte-8--opcional-empilhamento-para-objetos-fracos)
9. [Measuring / Medindo](#part-9--measuring--parte-9--medindo)
10. [Photometry / Fotometria](#part-10--photometry--parte-10--fotometria)
11. [Getting the time right / Acertando o horário](#part-11--getting-the-time-right--parte-11--acertando-o-horário)
12. [Report and submission / Relatório e envio](#part-12--the-ades-report-and-mpc-submission--parte-12--o-relatório-ades-e-o-envio-ao-mpc)
13. [Sessions and projects / Sessões e projetos](#part-13--sessions-and-projects-gus--parte-13--sessões-e-projetos-gus)
14. [School / guided mode / Modo escola / guiado](#part-14--school-and-guided-mode--parte-14--modo-escola-e-guiado)

---

## Part 0 — Key ideas before your first report / Parte 0 — Ideias-chave antes do primeiro relatório

🟢 **Beginner / Iniciante**

🇬🇧 **English**

- **The MPC** (Minor Planet Center, see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)) is the world clearing house for asteroid
  and comet positions. Observers send it *measurements*; the MPC uses them to compute and
  improve orbits.
- **What you send is astrometry** (measuring *positions*: right ascension and declination on
  the sky, at a precise time). **Photometry** (measuring *brightness*, a magnitude) is optional
  extra information in the same report.
- **ADES** (Astrometry Data Exchange Standard) is the file format the MPC wants. AstroFind
  writes ADES 2022 as **XML** and as **PSV** (pipe-separated text). It does **not** write the
  old MPC 80-column format.
- **Observatory code** (3 characters, e.g. `568`) identifies *where* on Earth you observed. The
  MPC needs it to correct for your position on the planet. You enter it in
  **File → Settings... → Observer → MPC Station Code:** / **Arquivo → Configurações... →
  Observador → Código de estação MPC:**. With no code, the report says `XXX`. If you do not have
  a code yet, read the "new observers" instructions on the MPC website
  (**Internet → Minor Planet Center…** opens it).
- **Residuals** are "measured minus expected" differences. Two kinds appear in this workflow:
  - *Astrometric residuals of the plate solution*: after AstroFind matches catalog stars, it
    reports how far, on average, the stars fall from where the solution puts them (the
    **WCS RMS**, in arcseconds, see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)). This number goes into the report as
    `rmsRA`/`rmsDec` — it tells the MPC how much to trust your positions. Good values are
    usually well below 1″.
  - *Photometric residuals*: how well the catalog-star magnitudes agree with the zero point
    AstroFind fitted. Large scatter means an unreliable magnitude.
  - After you submit, the MPC compares your positions with the object's orbit: those are the
    *O−C* (observed minus computed) residuals it may send back. Big O−C values usually mean a
    wrong time, a wrong observatory code or a bad measurement.
- **Why the time matters so much.** An asteroid moves while you measure it. A main-belt
  asteroid can move about 0.5″ per minute; a near-Earth object can move several arcseconds per
  minute. A clock that is wrong by **one minute** can shift your position by more than your
  whole measurement error. Get the time right to a second or better (see
  [Part 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário)).

🇧🇷 **Português**

- **O MPC** (Minor Planet Center, veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)) é a central mundial de posições
  de asteroides e cometas. Observadores enviam *medições*; o MPC as usa para calcular e melhorar
  órbitas.
- **O que você envia é astrometria** (medir *posições*: ascensão reta e declinação no céu, num
  horário preciso). **Fotometria** (medir *brilho*, uma magnitude) é uma informação extra
  opcional no mesmo relatório.
- **ADES** (Astrometry Data Exchange Standard) é o formato de arquivo que o MPC pede. O AstroFind
  gera ADES 2022 em **XML** e em **PSV** (texto separado por barras verticais). Ele **não** gera
  o antigo formato MPC de 80 colunas.
- **Código de observatório** (3 caracteres, ex.: `568`) identifica *de onde* na Terra você
  observou. O MPC precisa dele para corrigir sua posição no planeta. Você o informa em
  **Arquivo → Configurações... → Observador → Código de estação MPC:** / **File → Settings... →
  Observer → MPC Station Code:**. Sem código, o relatório sai com `XXX`. Se você ainda não tem
  código, leia as instruções para novos observadores no site do MPC
  (**Internet → Minor Planet Center…** abre o site).
- **Resíduos** são diferenças "medido menos esperado". Dois tipos aparecem neste fluxo:
  - *Resíduos astrométricos da solução de placa*: depois de casar as estrelas do catálogo, o
    AstroFind informa o quanto, em média, as estrelas ficam longe de onde a solução as coloca
    (o **RMS do WCS**, em segundos de arco, veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)). Esse número entra no
    relatório como `rmsRA`/`rmsDec` — ele diz ao MPC quanto confiar nas suas posições. Valores
    bons costumam ficar bem abaixo de 1″.
  - *Resíduos fotométricos*: o quanto as magnitudes das estrelas do catálogo concordam com o
    ponto zero que o AstroFind ajustou. Muita dispersão significa magnitude pouco confiável.
  - Depois do envio, o MPC compara suas posições com a órbita do objeto: esses são os resíduos
    *O−C* (observado menos calculado) que ele pode devolver. O−C grande costuma indicar horário
    errado, código de observatório errado ou medição ruim.
- **Por que o horário importa tanto.** O asteroide se move enquanto você o mede. Um asteroide do
  cinturão principal pode andar cerca de 0,5″ por minuto; um objeto próximo da Terra pode andar
  vários segundos de arco por minuto. Um relógio errado em **um minuto** pode deslocar sua
  posição mais do que todo o seu erro de medição. Acerte o horário com precisão de um segundo
  ou melhor (veja a [Parte 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário)).

---

## Part 1 — Before you open AstroFind: planning your observation / Parte 1 — Antes de abrir o AstroFind: planejando sua observação

🟢 **Beginner / Iniciante**

🇬🇧 **English**

Good asteroid astrometry starts before you take a picture:

1. **Pick a field near the ecliptic** (the plane where planets and most asteroids orbit — see
   [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)). This raises your odds of catching something. Avoid the Milky Way if you can:
   dense star fields make detection harder.
2. **Take at least 3 images, ideally 4**, of the same field, spaced about **20–60 minutes
   apart**. Automatic moving-object detection needs the object in at least 3 frames, and the
   blink view shows up to 4 images. Keep the exposure time the same in every frame.
3. **Set your camera or computer clock to UTC** (Universal Time) and synchronise it (internet
   time or GPS) before the night starts. See [Part 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário).
4. **Save FITS if your software can.** FITS keeps the time, exposure and often the site
   coordinates. See [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats) for other formats (DSLR RAW, SER, XISF, TIFF…).
5. **Take calibration frames** with the same camera: **darks** (same exposure and temperature,
   lens cap on) and **flats** (evenly lit field). See
   [Part 4](#part-4--calibration-bias-dark-and-flat--parte-4--calibração-bias-dark-e-flat).
6. No telescope? Use public survey data or your school's IASC data set — see [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

🇧🇷 **Português**

Uma boa astrometria de asteroides começa antes da primeira foto:

1. **Escolha um campo perto da eclíptica** (o plano onde orbitam os planetas e a maioria dos
   asteroides — veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)). Isso aumenta suas chances. Se puder, evite a Via
   Láctea: campos muito cheios de estrelas dificultam a detecção.
2. **Faça pelo menos 3 imagens, de preferência 4**, do mesmo campo, com cerca de **20 a 60
   minutos** entre elas. A detecção automática de objetos em movimento precisa do objeto em pelo
   menos 3 quadros, e o blink mostra até 4 imagens. Use o mesmo tempo de exposição em todas.
3. **Acerte o relógio da câmera ou do computador em UTC** (Tempo Universal) e sincronize-o
   (hora da internet ou GPS) antes de começar a noite. Veja a
   [Parte 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário).
4. **Salve em FITS se o seu programa permitir.** O FITS guarda o horário, a exposição e muitas
   vezes as coordenadas do local. Veja [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats) para outros formatos (RAW de DSLR, SER,
   XISF, TIFF…).
5. **Faça frames de calibração** com a mesma câmera: **darks** (mesma exposição e temperatura,
   com a tampa) e **flats** (campo iluminado por igual). Veja a
   [Parte 4](#part-4--calibration-bias-dark-and-flat--parte-4--calibração-bias-dark-e-flat).
6. Não tem telescópio? Use dados públicos de levantamentos ou o conjunto IASC da sua escola —
   veja o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).

---

## Part 2 — First-time setup / Parte 2 — Configuração inicial

🟢 **Beginner / Iniciante**

🇬🇧 **English**

1. On first start the **Setup Wizard** opens (also at **Help → Setup Wizard…** / **Ajuda →
   Assistente de Configuração…**). It has 3 steps:
   - *Observer Identity*: MPC station code, your name, telescope.
   - *Plate-solving API Key*: a free key from nova.astrometry.net (needed only for online plate
     solving).
   - *Asteroid Catalog (optional)*: downloads MPCORB.DAT (~200 MB) for offline known-object
     identification.
   Every page has **Skip this step** / **Pular esta etapa**.
2. Open **File → Settings...** (`Ctrl+,`) / **Arquivo → Configurações...** and check the
   **Observer / Observador** tab:
   - **Observer:**, **Measurer:** (blank = same as observer), **Telescope / Instrument:**.
   - **Location (topocentric correction)** / **Localização (correção topocêntrica)**: choose
     *Automático (do FITS)* when your FITS files carry site coordinates, *Observatório
     predefinido* to pick from the built-in list of 84 observatories, or *Coordenadas manuais*
     (latitude +N, longitude +E, altitude in m). *Telescópio Espacial* is enabled only for
     space-telescope images.
3. In **Connections / Conexões → Plate Solving → Backend:** choose
   *astrometry.net (online)* or *ASTAP (local, offline)* (see
   [Part 5](#part-5--data-reduction-background-star-detection-and-plate-solving--parte-5--redução-de-dados-fundo-detecção-de-estrelas-e-plate-solving)).
4. Click **OK**. The status bar toasts "Settings saved.".

> ⚠️ **Watch out / Atenção** — If the site stays at 0°, 0°, Run Data Reduction warns
> "Localização não configurada": your positions would be reported as if seen from the centre of
> the Earth. Fix the location before reporting.

> 💡 **Tip / Dica** — The whole Settings dialog is described in [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

🇧🇷 **Português**

1. Na primeira vez, abre o **Assistente de Configuração** (também em **Ajuda → Assistente de
   Configuração…** / **Help → Setup Wizard…**). Ele tem 3 etapas:
   - *Identidade do observador*: código de estação MPC, seu nome, telescópio.
   - *Chave de API para solução de campo*: uma chave gratuita do nova.astrometry.net (só
     necessária para o plate solving online).
   - *Catálogo de asteroides (opcional)*: baixa o MPCORB.DAT (~200 MB) para identificar objetos
     conhecidos sem internet.
   Toda página tem **Pular esta etapa** / **Skip this step**.
2. Abra **Arquivo → Configurações...** (`Ctrl+,`) / **File → Settings...** e confira a aba
   **Observador / Observer**:
   - **Observador:**, **Medidor:** (vazio = igual ao observador), **Telescópio / Instrumento:**.
   - **Localização (correção topocêntrica)** / **Location (topocentric correction)**: escolha
     *Automático (do FITS)* quando seus FITS trazem as coordenadas do local, *Observatório
     predefinido* para escolher na lista interna de 84 observatórios, ou *Coordenadas manuais*
     (latitude +N, longitude +L, altitude em m). *Telescópio Espacial* só fica disponível para
     imagens de telescópio espacial.
3. Em **Conexões / Connections → Plate Solving → Backend:** escolha *astrometry.net (online)* ou
   *ASTAP (local, offline)* (veja a
   [Parte 5](#part-5--data-reduction-background-star-detection-and-plate-solving--parte-5--redução-de-dados-fundo-detecção-de-estrelas-e-plate-solving)).
4. Clique em **OK**. Aparece o aviso "Settings saved." / configurações salvas.

> ⚠️ **Atenção** — Se o local ficar em 0°, 0°, a Redução de Dados avisa "Localização não
> configurada": suas posições seriam relatadas como se vistas do centro da Terra. Corrija o
> local antes de relatar.

> 💡 **Dica** — A janela de Configurações inteira está descrita em [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

---

## Part 3 — Loading images / Parte 3 — Carregando imagens

🟢 **Beginner / Iniciante**

🇬🇧 **English**

1. Choose **File → Import Images...** / **Arquivo → Importar Imagens...** (toolbar button, or the
   **▶ Do it** button of step 1 in the Workflow panel).
2. Select all the frames of one field (FITS, SER, XISF, TIFF/PNG/BMP/JPEG, DSLR RAW, NASA PDS,
   or an archive — details in [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats)). You can also drag and drop FITS files or
   archives onto the window.
3. If images are already loaded, AstroFind asks "Sessão em andamento": **Acréscimo** adds the new
   files, **Novo Projeto** starts clean (and resets pixel scale, saturation and location; your
   Time Offset is kept).

**What you should see:** one window per image, tiled; thumbnails in the **Images** bar; the
Log shows `Loaded: <file> [W×H] JD=…` for each file; an information bar above the images shows
chips such as "✓ Escala …\"/px", "✓ WCS pré-resolvido", "✓ Data/hora UTC" and
"✓ Localização …", or "⚠" chips with a button that opens Settings. A notice "Imagens
carregadas" says whether the images are black-and-white or colour.

- If the image looks black, that is only the display: use **Image Tools → Background and Range...**
  / **Imagens → Fundo e Intervalo...** (⟳ Auto, transfer function, false colour). This does not
  change the data.
- The session holds up to **20 images**. The limit message says to raise it in Settings, but
  there is no such field.
- AstroFind reads pixel scale, saturation, location and MPC code from the first image when
  your settings are empty.

> 💡 **Version 1.1.0 / Versão 1.1.0** — in 1.1.0 and earlier `Ctrl+L` was assigned to both
> **Import Images...** and **Utilities → Light Curve…**, so the key could do nothing; use the menu or
> the toolbar there. The next version (after 1.1.0) moves Light Curve to `Ctrl+Shift+L`.

> ⚠️ **DSLR RAW and files without a time zone** — the Log shows
> "DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora
> local." This means AstroFind *assumed* the time is UTC. Read
> [Part 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário) before measuring.
> **NASA PDS** images rarely have a plate solution: run Run Data Reduction before measuring.

🇧🇷 **Português**

1. Escolha **Arquivo → Importar Imagens...** / **File → Import Images...** (botão da barra de
   ferramentas, ou o botão **▶ Executar** da etapa 1 no painel de Fluxo).
2. Selecione todos os quadros de um campo (FITS, SER, XISF, TIFF/PNG/BMP/JPEG, RAW de DSLR,
   NASA PDS, ou um arquivo compactado — detalhes em [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats)). Também dá para arrastar
   e soltar arquivos FITS ou compactados na janela.
3. Se já houver imagens carregadas, o AstroFind pergunta "Sessão em andamento": **Acréscimo**
   adiciona os novos arquivos, **Novo Projeto** começa do zero (e zera escala de pixel,
   saturação e localização; seu Deslocamento de tempo é mantido).

**O que você deve ver:** uma janela por imagem, lado a lado; miniaturas na barra **Imagens**; o
Registro mostra `Loaded: <arquivo> [L×A] JD=…` para cada arquivo; uma barra de informações
acima das imagens mostra etiquetas como "✓ Escala …\"/px", "✓ WCS pré-resolvido",
"✓ Data/hora UTC" e "✓ Localização …", ou etiquetas "⚠" com um botão que abre as
Configurações. Um aviso "Imagens carregadas" informa se as imagens são preto e branco ou
coloridas.

- Se a imagem parecer preta, é só a exibição: use **Imagens → Fundo e Intervalo...** /
  **Ferramentas de Imagem → Background and Range...** (⟳ Auto, função de transferência, cor falsa). Isso não
  altera os dados.
- A sessão aceita até **20 imagens**. A mensagem de limite manda aumentar nas Configurações, mas
  esse campo não existe.
- O AstroFind lê escala de pixel, saturação, localização e código MPC da primeira imagem quando
  suas configurações estão vazias.

> 💡 **Versão 1.1.0** — na 1.1.0 e anteriores o `Ctrl+L` estava ligado a **Carregar
> Imagens...** e também a **Ferramentas → Curva de Luz…**, então a tecla podia não fazer nada;
> lá, use o menu ou a barra de ferramentas. A próxima versão (depois da 1.1.0) passa a Curva de
> Luz para `Ctrl+Shift+L`.

> ⚠️ **RAW de DSLR e arquivos sem fuso horário** — o Registro mostra
> "DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora
> local." Isso quer dizer que o AstroFind *supôs* que o horário está em UTC. Leia a
> [Parte 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário) antes de medir.
> Imagens **NASA PDS** raramente têm solução de placa: rode a Redução de Dados antes de medir.

---

## Part 4 — Calibration: bias, dark and flat / Parte 4 — Calibração: bias, dark e flat

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

Calibration removes the camera's own signature from your images (see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)):

| Frame | What it is | What it fixes |
|---|---|---|
| **Bias** | A zero-second exposure: the electronic offset every pixel has. | The constant "floor" of the sensor. |
| **Dark** | An exposure with the shutter or cap closed, same exposure time and temperature as your images. | Thermal signal (dark current) and hot pixels. A dark also contains the bias. |
| **Flat** | An image of an evenly lit surface or twilight sky. | Vignetting (dark corners) and dust shadows. |
| **Master frame** | Several darks (or flats) combined into one clean frame. | Reduces the noise of a single calibration frame. |

**How AstroFind does it:**

- There is **no separate bias input**. Take darks with the *same exposure and temperature* as
  your images: the master dark then removes the bias too. AstroFind subtracts the dark as is
  (no scaling). It normalises the flat by its median and divides by it; it does **not**
  bias- or dark-correct the flat for you. If you need a fully calibrated flat, prepare it in
  another program and load the result as FITS.
- Dark and flat must be **FITS** files with the **same size** as your images; otherwise the Log
  says "Image %1: dark frame size mismatch — skipped".

**Steps:**

1. Build masters (optional): in the **Calibration** dock, click **Build Master…** /
   **Construir Master…** in the *Dark Frame* group, **Add Files…** (at least 2), choose
   *Average* or *Median*, then **Build Master**. Repeat in the *Flat Field* group.
2. Or load ready masters: **File → Load Dark Frame...** / **Arquivo → Carregar Dark Frame...**
   and **File → Load Flat Field...** / **Arquivo → Carregar Flat Field...**. Or use the guided
   **File → Calibration Wizard…** / **Arquivo → Assistente de Calibração…**.
3. Apply: **Apply to All Images** / **Aplicar a Todas as Imagens** in the dock, or right-click one
   image → **Apply Dark Frame…** / **Apply Flat Field…**. If you skip this, the frames are
   applied automatically during Run Data Reduction.

**What you should see:** the dock shows "✓ <name>" with the frame size; a toast
"Calibration applied to N image(s)".

Two more corrections run inside Run Data Reduction:

- **Bad pixels** (hot pixels, bad columns) are replaced by the median of their 3×3 neighbours.
  On by default (**Camera → Bad Pixel Correction**, threshold 5σ). Keep it on.
- **Sky background** removal (a sliding median that flattens gradients) is **off** by default:
  **Detection → Sky Background → Subtrair modelo de fundo antes da detecção**. Turn it on for
  light pollution gradients or moonlight.

🇧🇷 **Português**

A calibração tira da imagem a "assinatura" da própria câmera (veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)):

| Frame | O que é | O que corrige |
|---|---|---|
| **Bias** | Exposição de zero segundo: o deslocamento eletrônico que todo pixel tem. | O "piso" constante do sensor. |
| **Dark** | Exposição com obturador ou tampa fechados, mesmo tempo de exposição e temperatura das suas imagens. | Sinal térmico (corrente de escuro) e pixels quentes. Um dark também contém o bias. |
| **Flat** | Imagem de uma superfície iluminada por igual ou do céu do crepúsculo. | Vinhetas (cantos escuros) e sombras de poeira. |
| **Master (frame mestre)** | Vários darks (ou flats) combinados em um frame limpo. | Reduz o ruído de um único frame de calibração. |

**Como o AstroFind faz:**

- **Não existe entrada separada para bias.** Faça darks com a *mesma exposição e temperatura*
  das suas imagens: o master dark então remove o bias também. O AstroFind subtrai o dark como
  está (sem escalonar). Ele normaliza o flat pela mediana e divide por ele; ele **não** corrige
  o flat de bias ou dark para você. Se precisar de um flat totalmente calibrado, prepare-o em
  outro programa e carregue o resultado em FITS.
- Dark e flat precisam ser **FITS** com o **mesmo tamanho** das suas imagens; senão o Registro
  diz "Image %1: dark frame size mismatch — skipped".

**Passos:**

1. Monte masters (opcional): no painel **Calibração**, clique em **Construir Master…** /
   **Build Master…** no grupo *Dark Frame*, **Add Files…** (pelo menos 2), escolha *Average*
   (média) ou *Median* (mediana) e depois **Build Master**. Repita no grupo *Flat Field*.
2. Ou carregue masters prontos: **Arquivo → Carregar Dark Frame...** / **File → Load Dark
   Frame...** e **Arquivo → Carregar Flat Field...** / **File → Load Flat Field...**. Ou use o
   guia **Arquivo → Assistente de Calibração…** / **File → Calibration Wizard…**.
3. Aplique: **Aplicar a Todas as Imagens** / **Apply to All Images** no painel, ou clique com o
   botão direito numa imagem → **Aplicar Dark Frame…** / **Aplicar Flat Field…**. Se você pular
   isso, os frames são aplicados automaticamente na Redução de Dados.

**O que você deve ver:** o painel mostra "✓ <nome>" com o tamanho do frame; um aviso
"Calibration applied to N image(s)".

Mais duas correções rodam dentro da Redução de Dados:

- **Pixels ruins** (pixels quentes, colunas defeituosas) são trocados pela mediana dos 3×3
  vizinhos. Ligado por padrão (**Câmera → Correção de Pixels Ruins**, limiar 5σ). Deixe ligado.
- A remoção do **fundo do céu** (uma mediana deslizante que aplaina gradientes) vem
  **desligada**: **Detecção → Fundo do Céu → Subtrair modelo de fundo antes da detecção**.
  Ligue para gradientes de poluição luminosa ou de Lua.

---

## Part 5 — Data Reduction: background, star detection and plate solving / Parte 5 — Redução de Dados: fundo, detecção de estrelas e plate solving

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

Run **Astrometry Tools → Run Data Reduction...** (`Ctrl+A`) / **Ferramentas de Astrometria → Executar Redução de Dados...**, or
**▶ Do it** on workflow step 2. **Astrometry Tools → Stop Data Reduction** (`Ctrl+.`) cancels it. For
each image AstroFind:

1. Applies calibration, bad-pixel correction and (if enabled) background subtraction
   ([Part 4](#part-4--calibration-bias-dark-and-flat--parte-4--calibração-bias-dark-e-flat)).
2. **Detects stars** with SEP (the Source Extractor library): sources above the threshold
   (**Detection → Detection threshold:**, default 4σ), up to the 500 brightest. Elongated
   sources are flagged as **streaks** (orange ellipses — possibly a fast asteroid or a
   satellite); blended pairs are split (magenta double circles). Log:
   "Image %1: %2 stars found".
3. **Plate solves** (finds where the image points on the sky, its scale and rotation — the
   **WCS**, see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)). Images that already have a WCS in the header are skipped
   ("WCS pré-existente — plate solving ignorado").

**Which plate solver? / Qual plate solver?**

| | astrometry.net (default) | ASTAP |
|---|---|---|
| Where it runs | Online (nova.astrometry.net) | On your computer |
| Needs | Internet + free API key | ASTAP installed + its star database; path in **Connections → Executável ASTAP:** |
| Speed | Depends on the queue (timeout 300 s) | Usually seconds |
| Privacy | Uploads your image (not public) | Nothing leaves your PC |
| Good for | Beginners, unknown fields | Many images, no internet, field roughly known |

**What you should see:** "  Solved! RA=… Dec=… scale=…\"/px" per image, then
"Redução concluída: N resolvidas, N com WCS pré-existente, N falhou". The status bar says
"Step 2 done - show Known Objects (Ctrl+K)".

> 💡 **Tip / Dica** — **Connections → Save WCS back to FITS file after plate solve** writes the
> solution into your *original* file. Leave it off unless you want that; use
> **File → Save FITS Copy...** to write a `<name>_wcs.fits` copy instead.

> 💡 **Time / Horário** — Run Data Reduction adds your **Time Offset** (camera-clock correction, in
> seconds) to each image's time **once**; running it again is safe. ΔT is not added to the
> report time. (Version 1.1.0 and earlier added Time Offset and ΔT again on every run.) See
> [Part 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário).

🇧🇷 **Português**

Rode **Ferramentas de Astrometria → Executar Redução de Dados...** (`Ctrl+A`) / **Astrometry Tools → Run Data Reduction...**, ou
**▶ Executar** na etapa 2 do fluxo. **Astrometria → Parar Redução de Dados** (`Ctrl+.`)
cancela. Para cada imagem o AstroFind:

1. Aplica calibração, correção de pixels ruins e (se ligada) subtração de fundo
   ([Parte 4](#part-4--calibration-bias-dark-and-flat--parte-4--calibração-bias-dark-e-flat)).
2. **Detecta estrelas** com o SEP (a biblioteca do Source Extractor): fontes acima do limiar
   (**Detecção → Limiar de detecção:**, padrão 4σ), até as 500 mais brilhantes. Fontes
   alongadas são marcadas como **traços** (elipses laranja — talvez um asteroide rápido ou um
   satélite); pares grudados são separados (círculos duplos magenta). Registro:
   "Image %1: %2 stars found".
3. **Faz o plate solving** (descobre para onde a imagem aponta no céu, sua escala e rotação — o
   **WCS**, veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)). Imagens que já têm WCS no cabeçalho são puladas
   ("WCS pré-existente — plate solving ignorado").

| | astrometry.net (padrão) | ASTAP |
|---|---|---|
| Onde roda | Online (nova.astrometry.net) | No seu computador |
| Precisa de | Internet + chave de API gratuita | ASTAP instalado + seu banco de estrelas; caminho em **Conexões → Executável ASTAP:** |
| Velocidade | Depende da fila (tempo limite 300 s) | Em geral segundos |
| Privacidade | Envia sua imagem (não pública) | Nada sai do seu PC |
| Bom para | Iniciantes, campos desconhecidos | Muitas imagens, sem internet, campo mais ou menos conhecido |

**O que você deve ver:** "  Solved! RA=… Dec=… scale=…\"/px" para cada imagem e depois
"Redução concluída: N resolvidas, N com WCS pré-existente, N falhou". A barra de status diz
"Step 2 done - show Known Objects (Ctrl+K)".

> 💡 **Dica** — **Conexões → Salvar WCS no arquivo FITS após solução de campo** grava a solução
> no seu arquivo *original*. Deixe desligado, a menos que queira isso; use
> **Arquivo → Salvar Cópia em FITS...** para gravar uma cópia `<nome>_wcs.fits`.

> 💡 **Horário** — A Execução da Redução de Dados soma o seu **Deslocamento de tempo** (correção do relógio
> da câmera, em segundos) ao horário de cada imagem **uma vez**; rodar de novo não causa
> problema. O ΔT não é somado ao horário do relatório. (A versão 1.1.0 e anteriores somavam o
> Deslocamento de tempo e o ΔT de novo a cada execução.) Veja a
> [Parte 11](#part-11--getting-the-time-right--parte-11--acertando-o-horário).

---

## Part 6 — Reference stars and known objects / Parte 6 — Estrelas de referência e objetos conhecidos

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

Run **Utilities → Show Known Objects** (`Ctrl+K`) / **Utilitários → Mostrar Objetos
Conhecidos**, or workflow step 3. It does two things:

**1. Reference stars (catalog).** Stars of a precise catalog are matched to your detected
stars. This gives the **WCS RMS** per image ("Image %1: WCS RMS = %2\" (%3 matched stars)"),
used as the position uncertainty in the report, and the reference for photometry. Choose the
catalog in **Settings → Connections → Star Catalog (VizieR)**:

| Catalog | Notes |
|---|---|
| **UCAC4 (recommended)** | Default. Has a V magnitude. |
| **Gaia DR3** | Most precise positions; G magnitude. |
| **Local FITS BINTABLE** (**Source: → Local FITS BINTABLE**, then **Local catalog:**) | Offline: a catalog table you exported (USNO-B, UCAC, Gaia…). |

Proper motion is applied to the image date. Results are cached locally. Only stars between
**Catalog mag (bright)** (10) and **Catalog mag (faint)** (16) are used (tab Detection).
Catalog stars are yellow crosses.

**2. Known solar-system objects (KOO).** AstroFind asks IMCCE **SkyBoT** (online) which
asteroids and comets are in the field at the image time. If SkyBoT cannot be reached and
**MPCORB.DAT** is present, it computes positions from that file instead ("Scanning MPCORB…").
Known objects are green (asteroids), light blue (planets) or orange (comets), labelled with
their number or name. Log: "SkyBoT: N known object(s) in field".

- Get MPCORB.DAT with **Internet → Download MPCOrb Database** / **Internet → Baixar Banco MPCOrb** (the dialog
  says ~200 MB; the status-bar tip says ~500 MB). Refresh it with **Internet → Update MPCOrb Database**.
  **File → Reload MPCOrb** re-reads the file from disk.
- **Internet → Query Horizons…** asks JPL Horizons for one object and can **Add to Overlay**.
  The ephemeris is geocentric (from the Earth's centre), so it can differ from your view by a
  few arcseconds for nearby objects.
- **Utilities → Ecliptic / Galactic Overlay** (`Ctrl+E`) draws the ecliptic and the Milky Way plane.

**What you should see:** yellow crosses on catalog stars, green labels on known asteroids, the
**Image Catalog** dock listing "Estrela", "Estrela Guia" and "Asteroide Conhecido" rows.

🇧🇷 **Português**

Rode **Utilitários → Mostrar Objetos Conhecidos** (`Ctrl+K`) / **Utilities → Show Known
Overlay**, ou a etapa 3 do fluxo. Ela faz duas coisas:

**1. Estrelas de referência (catálogo).** Estrelas de um catálogo preciso são casadas com as
estrelas detectadas. Isso dá o **RMS do WCS** por imagem ("Image %1: WCS RMS = %2\" (%3
matched stars)"), usado como incerteza de posição no relatório, e a referência para a
fotometria. Escolha o catálogo em **Configurações → Conexões → Catálogo de estrelas
(VizieR)**:

| Catálogo | Observações |
|---|---|
| **UCAC4 (recommended)** | Padrão. Tem magnitude V. |
| **Gaia DR3** | Posições mais precisas; magnitude G. |
| **Local FITS BINTABLE** (**Fonte: → Local FITS BINTABLE**, depois **Catálogo local:**) | Sem internet: uma tabela de catálogo que você exportou (USNO-B, UCAC, Gaia…). |

O movimento próprio é aplicado à data da imagem. Os resultados ficam em cache local. Só são
usadas estrelas entre **Magnitude do catálogo (brilhante)** (10) e **Magnitude do catálogo
(fraca)** (16) (aba Detecção). Estrelas de catálogo são cruzes amarelas.

**2. Objetos conhecidos do Sistema Solar (KOO).** O AstroFind pergunta ao **SkyBoT** do IMCCE
(online) quais asteroides e cometas estão no campo no horário da imagem. Se o SkyBoT não
responder e o **MPCORB.DAT** existir, ele calcula as posições a partir desse arquivo
("Scanning MPCORB…"). Objetos conhecidos aparecem em verde (asteroides), azul-claro (planetas)
ou laranja (cometas), com número ou nome. Registro: "SkyBoT: N known object(s) in field".

- Baixe o MPCORB.DAT em **Internet → Baixar Banco MPCOrb** / **Internet → Download MPCOrb Database** (a janela
  diz ~200 MB; a dica da barra de status diz ~500 MB). Atualize com **Internet → Atualizar
  MPCOrb**. **Arquivo → Recarregar MPCOrb** relê o arquivo do disco.
- **Internet → Consultar Horizons…** pede ao JPL Horizons a posição de um objeto e permite
  **Adicionar à Sobreposição**. A efeméride é geocêntrica (do centro da Terra), então pode
  diferir alguns segundos de arco do que você vê, para objetos próximos.
- **Ferramentas → Sobreposição Eclíptica / Galáctica** (`Ctrl+E`) desenha a eclíptica e o plano
  da Via Láctea.

**O que você deve ver:** cruzes amarelas nas estrelas de catálogo, rótulos verdes nos
asteroides conhecidos, o painel **Catálogo de Imagem** listando linhas "Estrela", "Estrela
Guia" e "Asteroide Conhecido".

---

## Part 7 — Finding the moving object / Parte 7 — Encontrando o objeto em movimento

🟢 **Beginner / Iniciante** (blink) · 🟡 **Intermediate / Intermediário** (automatic detection)

🇬🇧 **English**

**Blink** (showing the frames one after another; stars stay still, the asteroid jumps):

1. **Utilities → Begin Blink Mode** (`Ctrl+B`) / **Utilitários → Ativar Modo de Piscagem**. Needs at least 2
   images.
2. Keys in the blink view: `Space` play/pause, `←`/`→` previous/next, `Esc` stop. The
   **Speed** slider sets 50–2000 ms per frame.
3. For faint objects click **Sharpen: Off** to cycle *USM* (unsharp mask) → *LoG* (Laplacian).
4. Stop with **Utilities → Stop Blinking** (`Ctrl+F9`).

**Automatic Moving Object Detection (MOD):** **Astrometry Tools → Detect Moving Objects...**
(`Ctrl+M`) / **Astrometria → Detecção de Objetos em Movimento...**. It links detections that
move in a straight line at a steady rate across at least 3 frames, with SNR ≥ 5 (**Detection →
MOD min. SNR:**). A box lists the candidates ("#1 dx=… dy=… px/frame …"), and each is marked
"Cand #N" on the images. **Always confirm a candidate by eye in the blink.**

**Compare Sessions** (**Utilities → Compare Sessions…**, `Ctrl+Shift+C`) subtracts a reference
image of the same field from your images and shows what is left in a "Residual" window — useful
to spot anything new.

If your candidate sits on a green known-object label, it is already known. You can still
measure and report it: known objects need observations too.

🇧🇷 **Português**

**Blink** (mostrar os quadros um após o outro; as estrelas ficam paradas, o asteroide pula):

1. **Utilitários → Ativar Modo de Piscagem** (`Ctrl+B`) / **Utilities → Begin Blink Mode**. Precisa de pelo menos
   2 imagens.
2. Teclas no blink: `Espaço` toca/pausa, `←`/`→` anterior/próxima, `Esc` para. O controle
   **Speed** ajusta de 50 a 2000 ms por quadro.
3. Para objetos fracos clique em **Nitidez: Desativado** para alternar *USM* (máscara de
   nitidez) → *LoG* (Laplaciano).
4. Pare com **Ferramentas → Parar Piscar** (`Ctrl+F9`).

**Detecção automática de objetos em movimento (MOD):** **Astrometria → Detecção de Objetos em
Movimento...** (`Ctrl+M`) / **Astrometry Tools → Detect Moving Objects...**. Ela liga detecções que
andam em linha reta, com velocidade constante, em pelo menos 3 quadros, com SNR ≥ 5 (**Detecção
→ MOD SNR mín.:**). Uma janela lista os candidatos ("#1 dx=… dy=… px/frame …"), e cada um é
marcado "Cand #N" nas imagens. **Sempre confirme um candidato a olho no blink.**

**Comparar Sessões** (**Ferramentas → Comparar Sessões…**, `Ctrl+Shift+C`) subtrai uma imagem de
referência do mesmo campo das suas imagens e mostra o que sobra numa janela "Residual" — útil
para achar algo novo.

Se o seu candidato estiver sobre um rótulo verde de objeto conhecido, ele já é conhecido. Você
ainda pode medir e relatar: objetos conhecidos também precisam de observações.

---

## Part 8 — Optional: stacking for faint objects / Parte 8 — Opcional: empilhamento para objetos fracos

🔴 **Advanced / Avançado**

🇬🇧 **English**

- **Image Tools → Rebuild Stack** / **Ferramentas de Imagem → Reconstruir Empilhamento** combines the frames aligned
  on the *stars* (Average, Median or Add). Stars get deeper; a moving asteroid smears. Result:
  `stacked.fits` in a new window.
- **Astrometry Tools → Stack Images...** (`Ctrl+T`) / **Ferramentas de Astrometria → Empilhar Imagens...** is
  **Track & Stack**: you type the object's motion in pixels per frame (dX, dY — read them from
  the MOD candidate list) and choose the method. The frames are shifted to follow the
  *asteroid*, so it becomes a sharp dot while stars trail. Result: `track_stacked.fits`.

Stacking N frames improves signal-to-noise by roughly √N. Measure on single frames whenever you
can; a stack has one combined time, and each single-frame position is worth more to the MPC.

🇧🇷 **Português**

- **Ferramentas de Imagem → Reconstruir Empilhamento** / **Image Tools → Rebuild Stack** combina os quadros alinhados
  nas *estrelas* (Average, Median ou Add). As estrelas ficam mais profundas; um asteroide em
  movimento borra. Resultado: `stacked.fits` numa nova janela.
- **Ferramentas de Astrometria → Empilhar Imagens...** (`Ctrl+T`) / **Astrometry Tools → Stack Images...** é o
  **Track & Stack**: você digita o movimento do objeto em pixels por quadro (dX, dY — leia na
  lista de candidatos do MOD) e escolhe o método. Os quadros são deslocados para seguir o
  *asteroide*, que vira um ponto nítido enquanto as estrelas viram traços. Resultado:
  `track_stacked.fits`.

Empilhar N quadros melhora o sinal-ruído em cerca de √N. Meça em quadros individuais sempre que
puder; uma pilha tem um único horário combinado, e cada posição de quadro individual vale mais
para o MPC.

---

## Part 9 — Measuring / Parte 9 — Medindo

🟢 **Beginner / Iniciante**

🇬🇧 **English**

1. Turn on the **Aperture Tool**: **Edit → Aperture Tool** (`A`) / **Editar → Ferramenta
   Abertura**, or the **Measure Object** toolbar button (`M`, it turns orange), or workflow
   step 5.
2. Click the asteroid (in an image window or in the blink view — a playing blink asks
   "Parar e Medir" / "Continuar Blink").
3. AstroFind then:
   - fits an elliptical **PSF** (the star's light profile, see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)) to find the exact
     centre (**centroid**) to a fraction of a pixel;
   - converts it to RA/Dec with the WCS;
   - corrects **atmospheric refraction**, but only when the position did **not** already come
     from a catalog plate solution (which absorbs refraction itself), and never for space
     telescopes;
   - measures brightness (see [Part 10](#part-10--photometry--parte-10--fotometria));
   - names the object if a known object lies within 10 pixels.
   The Log shows the refraction correction (or, when skipped because of the plate solution,
   "Refraction: not applied") and the PSF shape. "PSF elongation=… — check
   tracking, focus, or coma" means the star images are not round.
4. The **Verification** / **Verificação** window opens: a 4× zoom of the spot, RA/Dec, Mag,
   FWHM, SNR, an **Object:** field and the list "Objetos conhecidos próximos:" (double-click an
   entry to use its designation). Buttons (Portuguese in both languages): **✓ Aceitar** (keep
   window open), **✓ Aceitar e Fechar**, **✗ Rejeitar**.
5. Repeat in **every image** where the object is visible.

**What you should see:** each accepted measurement appears in the **Observations** dock
(`#, Object, RA (°), Dec (°), Mag, Band, FWHM", SNR, Airmass, JD, Image`). Double-click
**Object** or **Band** to edit them. **Edit → Undo** (`Ctrl+Z`) removes the last added
observation; the dock's **Remove** button deletes selected rows (this cannot be undone).

> ⚠️ **Watch out / Atenção** — "Centroid failed — no source found at click position" means you
> clicked too far from the object. Zoom in (mouse wheel) and click on its centre. Give the
> object its designation in **Object:**; an empty name is reported as `UNKN`.

🇧🇷 **Português**

1. Ligue a **Ferramenta Abertura**: **Editar → Ferramenta Abertura** (`A`) / **Edit → Aperture
   Tool**, ou o botão **Measure Object** da barra (`M`, fica laranja), ou a etapa 5 do fluxo.
2. Clique no asteroide (numa janela de imagem ou no blink — um blink tocando pergunta
   "Parar e Medir" / "Continuar Blink").
3. O AstroFind então:
   - ajusta uma **PSF** elíptica (o perfil de luz da estrela, veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary))
     para achar o centro exato (**centroide**) com fração de pixel;
   - converte para AR/Dec com o WCS;
   - corrige a **refração atmosférica**, mas só quando a posição **não** veio de uma solução de
     plate-solve por catálogo (que já absorve a refração), e nunca para telescópios espaciais;
   - mede o brilho (veja a [Parte 10](#part-10--photometry--parte-10--fotometria));
   - dá nome ao objeto se houver um objeto conhecido a até 10 pixels.
   O Registro mostra a correção de refração (ou, quando pulada por causa da solução de placa,
   "Refraction: not applied") e a forma da PSF. "PSF elongation=… — check
   tracking, focus, or coma" quer dizer que as estrelas não estão redondas.
4. Abre a janela **Verificação** / **Verification**: zoom 4× do ponto, AR/Dec, Mag, FWHM, SNR,
   um campo **Object:** e a lista "Objetos conhecidos próximos:" (clique duplo numa entrada para
   usar a designação). Botões: **✓ Aceitar** (mantém a janela aberta), **✓ Aceitar e Fechar**,
   **✗ Rejeitar**.
5. Repita em **todas as imagens** em que o objeto aparece.

**O que você deve ver:** cada medição aceita aparece no painel **Observações**
(`#, Object, RA (°), Dec (°), Mag, Band, FWHM", SNR, Airmass, JD, Image` — cabeçalhos em inglês).
Clique duas vezes em **Object** ou **Band** para editar. **Editar → Desfazer** (`Ctrl+Z`) remove
a última observação adicionada; o botão **Remover** do painel apaga as linhas selecionadas (não
dá para desfazer).

> ⚠️ **Atenção** — "Centroid failed — no source found at click position" quer dizer que você
> clicou longe demais do objeto. Aproxime (roda do mouse) e clique no centro dele. Dê a
> designação em **Object:**; nome vazio vai para o relatório como `UNKN`.

---

## Part 10 — Photometry / Parte 10 — Fotometria

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

For each measurement AstroFind sums the light inside a circle (the **aperture**, radius
2 × FWHM, at least 3 px — or a fixed radius in **Detection → Aperture Radius**) and turns it
into a magnitude using nearby **catalog stars** of known brightness (differential photometry;
the offset is the **zero point**, see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)). If no catalog star matches, the log says
"Photometry: no catalog stars matched — using instrumental mag": that number is not a real
magnitude, so do not report it (untick **Detection → Include magnitude in ADES report**).

- **Band**: taken from the FITS `FILTER` keyword, else **Detection → Default band:** (`C` =
  clear/unfiltered). Remember the catalog band: UCAC4 is V-like, Gaia is G.
- **Airmass** (how much air the light crossed) is always computed; the extinction correction
  is applied only if **Extinction coeff k:** is above 0.
- **Utilities → Growth Curve…** (`Ctrl+Shift+G`) shows how much light each aperture radius captures
  and can set the optimal radius for you.
- **Utilities → Light Curve…** (`Ctrl+Shift+L`) plots magnitude against time. **Export PNG…**
  saves the plot. (In 1.1.0 and earlier its shortcut was `Ctrl+L`, which clashed with Load
  Images; open it from the menu there.)

🇧🇷 **Português**

Em cada medição o AstroFind soma a luz dentro de um círculo (a **abertura**, raio 2 × FWHM, no
mínimo 3 px — ou um raio fixo em **Detecção → Raio da abertura**) e a transforma em magnitude
usando **estrelas de catálogo** próximas de brilho conhecido (fotometria diferencial; o
deslocamento é o **ponto zero**, veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)). Se nenhuma estrela de catálogo
casar, o registro diz "Photometry: no catalog stars matched — using instrumental mag": esse
número não é uma magnitude real, então não o relate (desmarque **Detecção → Incluir magnitude
no relatório ADES**).

- **Banda**: vem da palavra-chave FITS `FILTER`, senão de **Detecção → Banda padrão:** (`C` =
  sem filtro). Lembre a banda do catálogo: UCAC4 é parecida com V, Gaia é G.
- A **massa de ar** (quanto ar a luz atravessou) é sempre calculada; a correção de extinção só é
  aplicada se **Coef. de extinção k:** for maior que 0.
- **Ferramentas → Curva de Crescimento…** (`Ctrl+Shift+G`) mostra quanta luz cada raio de
  abertura captura e pode ajustar o raio ideal para você.
- **Ferramentas → Curva de Luz…** (`Ctrl+Shift+L`) traça magnitude contra tempo. **Export
  PNG…** salva o gráfico. (Na 1.1.0 e anteriores o atalho era `Ctrl+L`, que conflitava com
  Carregar Imagens; lá, abra pelo menu.)

---

## Part 11 — Getting the time right / Parte 11 — Acertando o horário

🟡 **Intermediate / Intermediário**

🇬🇧 **English**

The time in your report is the **middle of the exposure**. AstroFind uses the `JD` keyword if
present, otherwise `DATE-OBS` + half of `EXPTIME`.

**The "ambiguous time" warning.** A FITS `DATE-OBS` ending in `Z`, with a `±HH:MM` offset, or
with `TIMESYS = 'UTC'` is clear. Without any of those, AstroFind *assumes* UTC and logs
"DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora
local." **DSLR RAW files always get this warning**, because the EXIF time is simply what the
camera clock showed, with no time zone. If your camera was on local time (for example UTC−3),
every position will be computed for the wrong moment.

What to do:

1. Best: **set the camera clock to UTC** before observing, and sync it.
2. If the clock was on local time or drifted, fix each image's time in **Image Tools → Edit Image
   Settings...** / **Ferramentas de Imagem → Editar Configurações da Imagem...** → **Julian Date:** (mid-exposure)
   **before** running Run Data Reduction.
3. **Settings → Observer → Time Zone:** is *not* used to correct the images. Do not rely on it.

**What Run Data Reduction does to the time:**

- It adds **Time Offset** (**Observer → Time Offset:**, in seconds) to each image. This is a
  camera-clock correction: leave it at `0` unless you know your clock error. It is applied
  **once** — running Run Data Reduction again, or re-opening a project, does not add it again, and
  changing the value applies only the difference. AstroFind never fills this field by itself.
- It does **not** add **ΔT** (**Camera → ΔT (TT − UTC):**, default 68 s). The report time
  `obsTime` stays in UTC (`Z`). ΔT is used only to compute known asteroids from the offline
  MPCORB file; **leave it at the default**.
- **Time Precision:** (0–3) is the number of decimals of the seconds in `obsTime`
  (1 = tenths).

> ⚠️ **Still on version 1.1.0?** There, Run Data Reduction added Time Offset and ΔT to each image
> **again on every run**, so `obsTime` came out about 68 s late, and loading images filled Time
> Offset with *longitude ÷ 15* (hours, applied as seconds). Workaround for 1.1.0: set **ΔT**
> and **Time Offset** to `0` after loading and before Run Data Reduction, and reload the images
> before running it a second time. The next version (after 1.1.0) needs none of this; on its
> first start it resets an old non-zero Time Offset to 0 once, with a warning in the Log.

✅ **Check before sending:** open the PSV tab of the report and compare `obsTime` with the
mid-exposure UTC time you expect for one image.

🇧🇷 **Português**

O horário do relatório é o **meio da exposição**. O AstroFind usa a palavra-chave `JD` se ela
existir, senão `DATE-OBS` + metade do `EXPTIME`.

**O aviso de "horário ambíguo".** Um `DATE-OBS` terminando em `Z`, com deslocamento `±HH:MM`, ou
com `TIMESYS = 'UTC'` é claro. Sem nada disso, o AstroFind *supõe* UTC e registra
"DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora
local." **Arquivos RAW de DSLR sempre recebem esse aviso**, porque o horário EXIF é só o que o
relógio da câmera mostrava, sem fuso. Se a câmera estava em hora local (por exemplo UTC−3), toda
posição será calculada para o momento errado.

O que fazer:

1. Melhor: **acerte o relógio da câmera em UTC** antes de observar e sincronize-o.
2. Se o relógio estava em hora local ou atrasado/adiantado, corrija o horário de cada imagem em
   **Ferramentas de Imagem → Editar Configurações da Imagem...** / **Image Tools → Edit Image Settings...** →
   **Julian Date:** (meio da exposição) **antes** de rodar a Redução de Dados.
3. **Configurações → Observador → Fuso horário:** *não* é usado para corrigir as imagens. Não
   conte com ele.

**O que a Redução de Dados faz com o horário:**

- Soma o **Deslocamento de tempo** (**Observador → Deslocamento de tempo:**, em segundos) a cada
  imagem. É uma correção do relógio da câmera: deixe em `0`, a menos que saiba o erro do seu
  relógio. Ele é aplicado **uma vez** — rodar a Redução de Dados de novo, ou reabrir um
  projeto, não soma outra vez, e mudar o valor aplica só a diferença. O AstroFind nunca
  preenche esse campo sozinho.
- **Não** soma o **ΔT** (**Câmera → ΔT (TT − UTC):**, padrão 68 s). O horário do relatório
  `obsTime` fica em UTC (`Z`). O ΔT só é usado para calcular os asteroides conhecidos pelo
  arquivo offline MPCORB; **deixe no padrão**.
- **Precisão de tempo:** (0–3) é o número de casas decimais dos segundos no `obsTime`
  (1 = décimos).

> ⚠️ **Ainda na versão 1.1.0?** Nela, a Redução de Dados somava o Deslocamento de tempo e o ΔT
> a cada imagem **de novo a cada execução**, então o `obsTime` saía cerca de 68 s atrasado, e
> carregar imagens preenchia o Deslocamento de tempo com *longitude ÷ 15* (horas, aplicadas
> como segundos). Contorno na 1.1.0: ponha **ΔT** e **Deslocamento de tempo** em `0` depois de
> carregar e antes da Redução de Dados, e recarregue as imagens antes de rodar uma segunda vez.
> A próxima versão (depois da 1.1.0) não precisa de nada disso; na primeira execução ela zera
> uma vez um Deslocamento de tempo antigo diferente de 0, com um aviso no Registro.

✅ **Confira antes de enviar:** abra a aba PSV do relatório e compare o `obsTime` com o horário
UTC de meio de exposição que você espera para uma imagem.

---

## Part 12 — The ADES report and MPC submission / Parte 12 — O relatório ADES e o envio ao MPC

🟢 **Beginner / Iniciante**

🇬🇧 **English**

1. Check **Settings → Observer**: MPC Station Code, Observer, Measurer, Telescope. To include
   contact data tick **Include contact info in ADES report** and fill Contact 1/2 and e-Mail.
2. Open **File → View ADES Report File** / **Arquivo → Ver Arquivo de Relatório ADES** (toolbar
   "View ADES Report", or workflow step 6). If **Connections → Report output folder:** is set,
   `ades_report.xml` and `ades_report.psv` are saved there automatically first.
3. The **ADES 2022 Report Preview** window shows "N observation(s) — Station: …" and the
   **XML** and **PSV** tabs. Each line has the object ID, `mode=CCD`, station, `obsTime`, RA/Dec
   (`sys=ICRF`), `rmsRA`/`rmsDec` (from the WCS RMS, default 0.5″), the catalog (`astCat`) and,
   if present, magnitude and band.
4. Review it, then choose:
   - **Save…** (visible tab), **Save to Reports Folder** (both files, to your home folder by
     default — not the same folder as step 2), **Copy**, **Export PDF…** (a printable summary
     with thumbnails).
   - **Submit to MPC (HTTP)**: after a confirmation, sends the PSV to the MPC address in
     **Connections → MPC Submission → Submit URL:**. If it fails, AstroFind offers e-mail.
   - **Submit to MPC (Email)**: opens your e-mail program addressed to
     `obs@minorplanetcenter.net` with the PSV in the body (warns "Large Report" above 2000
     characters).
   - **Enviar para Professor**: only when a teacher e-mail is set (Part 14).

> ⚠️ **Watch out / Atenção** — Before a real submission: correct station code (not `XXX`),
> correct site, correct times (Part 11), a sensible WCS RMS, and a designation for each
> object. Send only measurements you checked by eye.

🇧🇷 **Português**

1. Confira **Configurações → Observador**: Código de estação MPC, Observador, Medidor,
   Telescópio. Para incluir contato marque **Incluir informações de contato no relatório ADES**
   e preencha Contato 1/2 e E-mail.
2. Abra **Arquivo → Ver Arquivo de Relatório ADES** / **File → View ADES Report File** (botão
   "View ADES Report" da barra, ou a etapa 6 do fluxo). Se **Conexões → Pasta de saída de
   relatórios:** estiver preenchida, `ades_report.xml` e `ades_report.psv` são salvos lá
   automaticamente antes.
3. A janela **Pré-visualização do relatório ADES 2022** mostra "N observation(s) — Station: …"
   e as abas **XML** e **PSV**. Cada linha tem o ID do objeto, `mode=CCD`, estação, `obsTime`,
   AR/Dec (`sys=ICRF`), `rmsRA`/`rmsDec` (do RMS do WCS, padrão 0,5″), o catálogo (`astCat`) e,
   se houver, magnitude e banda.
4. Revise e escolha:
   - **Save…** (aba visível), **Save to Reports Folder** (os dois arquivos, por padrão na sua
     pasta pessoal — não é a mesma pasta do passo 2), **Copy**, **Export PDF…** (um resumo para
     imprimir, com miniaturas).
   - **Submit to MPC (HTTP)**: após confirmação, envia o PSV para o endereço do MPC em
     **Conexões → Envio ao MPC → URL de envio:**. Se falhar, o AstroFind oferece o e-mail.
   - **Submit to MPC (Email)**: abre seu programa de e-mail endereçado a
     `obs@minorplanetcenter.net` com o PSV no corpo (avisa "Large Report" acima de 2000
     caracteres).
   - **Enviar para Professor**: só quando há e-mail de professor configurado (Parte 14).

> ⚠️ **Atenção** — Antes de um envio real: código de estação correto (não `XXX`), local correto,
> horários corretos (Parte 11), RMS do WCS razoável e designação para cada objeto. Envie só
> medições que você conferiu a olho.

---

## Part 13 — Sessions and projects (.gus) / Parte 13 — Sessões e projetos (.gus)

🟢 **Beginner / Iniciante**

🇬🇧 **English**

- **File → Save Project** (`Ctrl+S`) / **Save Project As...** (`Ctrl+Shift+S`) writes a `.gus`
  file (default folder `~/projects`). It stores image paths and metadata, WCS, detected and
  catalog stars, known objects, display range and observations — **not the pixels**. Keep your
  image files where they are.
- **File → Open Project...** (`Ctrl+O`) reloads the images from disk (and re-extracts them from
  their archive). If a file moved, choose **Localizar…**, **Pular** or **Cancelar tudo**.
  **Projetos Recentes** lists the last 5 projects.
- **File → Close Project** offers to save first. **File → Reset Session Files** (`Ctrl+R`) and
  **Fechar Imagens** (`Ctrl+W`) clear everything **without asking to save**.
- **File → Open Recent...** reopens every image in one of the last 8 folders.

🇧🇷 **Português**

- **Arquivo → Salvar Projeto** (`Ctrl+S`) / **Salvar Projeto Como...** (`Ctrl+Shift+S`) grava um
  arquivo `.gus` (pasta padrão `~/projects`). Ele guarda caminhos e metadados das imagens, WCS,
  estrelas detectadas e de catálogo, objetos conhecidos, faixa de exibição e observações —
  **não os pixels**. Mantenha os arquivos de imagem onde estão.
- **Arquivo → Abrir Projeto...** (`Ctrl+O`) recarrega as imagens do disco (e as extrai de novo do
  arquivo compactado). Se um arquivo mudou de lugar, escolha **Localizar…**, **Pular** ou
  **Cancelar tudo**. **Projetos Recentes** lista os 5 últimos projetos.
- **Arquivo → Fechar Projeto** oferece salvar antes. **Arquivo → Resetar Arquivos** (`Ctrl+R`) e
  **Fechar Imagens** (`Ctrl+W`) limpam tudo **sem perguntar se quer salvar**.
- **Arquivo → Abrir Recente...** reabre todas as imagens de uma das 8 últimas pastas.

---

## Part 14 — School and guided mode / Parte 14 — Modo escola e guiado

🟢 **Beginner / Iniciante**

🇬🇧 **English**

- The **Workflow** panel (left) lists the 6 steps: Import Images, Run Data Reduction, Show Known
  Objects, Begin Blink Mode, Measure Objects, ADES Report. ○ = not yet, ▶ = next, ✓ = done. Click
  **▶ Do it** / **▶ Executar** on the next step.
- **Fluxo automático** (checkbox, off by default) runs steps 2, 3 and 4 by itself after loading.
- Teachers: set everything up once, then **File → Exportar Configuração da Escola...** writes
  **all** settings to an `.ini` file (default `~/astrofind_escola.ini`); students use
  **File → Importar Configuração da Escola...**. These menu labels are Portuguese in both
  languages.
- Set **Observer → E-mail do Professor:** and the report window shows **Enviar para Professor**
  (opens an e-mail with the PSV).

> ⚠️ **Watch out / Atenção** — The `.ini` contains *all* settings. If your build stores the
> astrometry.net API key in plain text (Settings shows "⚠ Stored in plain text"), the key goes
> into the file too.

🇧🇷 **Português**

- O painel **Fluxo de Trabalho** (à esquerda) lista as 6 etapas: Carregar Imagens, Redução de
  Dados, Obj. Conhecidos, Piscar Imagens, Medir Objetos, Relatório ADES. ○ = ainda não,
  ▶ = próxima, ✓ = feita. Clique em **▶ Executar** na próxima etapa.
- **Fluxo automático** (caixa, desligada por padrão) roda as etapas 2, 3 e 4 sozinho depois de
  carregar.
- Professores: configurem tudo uma vez e usem **Arquivo → Exportar Configuração da Escola...**,
  que grava **todas** as configurações num `.ini` (padrão `~/astrofind_escola.ini`); os alunos
  usam **Arquivo → Importar Configuração da Escola...**.
- Preencha **Observador → E-mail do Professor:** e a janela do relatório mostra **Enviar para
  Professor** (abre um e-mail com o PSV).

> ⚠️ **Atenção** — O `.ini` contém *todas* as configurações. Se a sua versão guarda a chave de
> API do astrometry.net em texto puro (as Configurações mostram "⚠ Stored in plain text"), a
> chave vai junto no arquivo.

---

## Advanced / Avançado

🔴 **Advanced / Avançado**

🇬🇧 Positions are reported as astrometric ICRF positions: the plate solution already absorbs
annual aberration and precession/nutation (AstroFind only logs their size for information). The
one correction that can still be applied on top is atmospheric refraction (Bennett formula), and
only when the position was **not** already derived from that catalog plate solution, since such a
solution already absorbs refraction too, in practice this means Bennett does not run on a typical
measured position today. The mathematics of WCS projections, PSF fitting, photometry and the ADES
format is in [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced) and in
`docs/technical-reference.md` in the repository.

🇧🇷 As posições são relatadas como posições astrométricas ICRF: a solução de placa já absorve a
aberração anual e a precessão/nutação (o AstroFind só registra o tamanho delas em log, para
informação). A única correção que ainda pode ser aplicada por cima é a refração atmosférica
(fórmula de Bennett), e só quando a posição **não** veio dessa solução de plate-solve por
catálogo, já que tal solução também já absorve a refração; na prática isso significa que Bennett
não roda sobre uma posição medida típica hoje. A matemática das projeções WCS, do ajuste de PSF,
da fotometria e do formato ADES está em [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced) e em
`docs/technical-reference.md` no repositório.

---

**See also / Veja também:** [Home](https://github.com/petrinhu/astrofind/wiki/Home) · [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) · [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) · [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats) ·
[Settings](https://github.com/petrinhu/astrofind/wiki/Settings) · [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) · [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary) · [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ)
