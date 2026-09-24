# Settings / Configurações

🇬🇧 **Who this page is for.** Anyone who opens **File → Settings...** (`Ctrl+,`) and wants to
know what a field means, what the default is, and whether to touch it. Beginners: you only
need the fields marked 🟢; the rest already have good values. Advanced users will also find
the internal *settings key* of every field (the name AstroFind uses in its configuration file).
Several fields have a label that does not match what the program really does: they are marked
**⚠️ Watch out / Atenção**, with the value you should actually enter. Unknown words: see
[Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Para quem é esta página.** Para quem abre **Arquivo → Configurações...** (`Ctrl+,`) e
quer saber o que um campo significa, qual é o padrão e se deve mexer nele. Iniciantes: só
precisam dos campos marcados 🟢; o resto já vem com bons valores. Usuários avançados também
encontram a *chave* interna de cada campo (o nome que o AstroFind usa no arquivo de
configuração). Vários campos têm um rótulo que não bate com o que o programa faz de verdade:
estão marcados com **⚠️ Watch out / Atenção**, com o valor que você deve digitar. Palavras
desconhecidas: veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

**Contents / Conteúdo**

1. [Opening Settings / Abrindo as Configurações](#opening-settings--abrindo-as-configurações)
2. [Observer tab / Aba Observador](#observer-tab--aba-observador)
3. [Camera tab / Aba Câmera](#camera-tab--aba-câmera)
4. [Connections tab / Aba Conexões](#connections-tab--aba-conexões)
5. [Detection tab / Aba Detecção](#detection-tab--aba-detecção)
6. [Display tab / Aba Exibição](#display-tab--aba-exibição)
7. [Legacy tab / Aba Legado](#legacy-tab--aba-legado)
8. [Setup Wizard / Assistente de Configuração](#setup-wizard--assistente-de-configuração)
9. [Settings that change by themselves / Configurações que mudam sozinhas](#settings-that-change-by-themselves--configurações-que-mudam-sozinhas)
10. [Where settings are stored, and how to reset / Onde ficam as configurações e como restaurar](#where-settings-are-stored-and-how-to-reset--onde-ficam-as-configurações-e-como-restaurar)
11. [Summary of label/unit problems / Resumo dos problemas de rótulo e unidade](#summary-of-labelunit-problems--resumo-dos-problemas-de-rótulo-e-unidade)

---

## Opening Settings / Abrindo as Configurações

🟢

🇬🇧 **English**

1. Open **File → Settings...** (`Ctrl+,`) / **Arquivo → Configurações...**, or click the gear
   icon "Settings (Ctrl+,)" on the Standard toolbar. The chips of the information bar
   (**Câmera →**, **Observatório →**, **Configurar →**) also open it.
2. The window "Settings" has six tabs, in this order: **Observer, Camera, Connections,
   Detection, Display, Legacy**.
3. At the bottom: **Reset to Defaults** (see [Reset](#where-settings-are-stored-and-how-to-reset--onde-ficam-as-configurações-e-como-restaurar)), **OK** and **Cancel**.
4. Nothing is saved until you press **OK**. **Cancel** throws away your changes.

**What you should see:** after **OK**, a small message "Settings saved." appears. If you
changed the language, AstroFind asks to restart.

Some tabs and groups show a yellow banner: "Best values are already set — only change if you
are an advanced user." Believe it: leave those fields alone unless you know why.

🇧🇷 **Português**

1. Abra **Arquivo → Configurações...** (`Ctrl+,`), ou clique no ícone de engrenagem
   "Configurações (Ctrl+,)" da barra Padrão. As etiquetas da barra de informações
   (**Câmera →**, **Observatório →**, **Configurar →**) também abrem a janela.
2. A janela "Configurações" tem seis abas, nesta ordem: **Observador, Câmera, Conexões,
   Detecção, Exibição, Legado**.
3. Embaixo: **Restaurar padrões**, **OK** e **Cancelar**.
4. Nada é gravado até você apertar **OK**. **Cancelar** descarta as mudanças.

**O que você deve ver:** depois do **OK**, aparece a mensagem "Settings saved.". Se você
mudou o idioma, o AstroFind pede para reiniciar.

Algumas abas e grupos mostram uma faixa amarela: "Os melhores valores já estão definidos —
altere somente se for um usuário avançado." Leve a sério: não mexa nesses campos sem saber
por quê.

---

## Observer tab / Aba Observador

🇬🇧 **English** — Who you are and where you observe from. These go into every report you send
to the MPC (Minor Planet Center, the world office that collects asteroid measurements).

🇧🇷 **Português** — Quem você é e de onde observa. Isso vai em todo relatório enviado ao MPC
(Minor Planet Center, o escritório mundial que recebe medições de asteroides).

### Identity / Identidade

| EN label | PT label | Meaning | Significado | Default | Range | Key | Level |
|---|---|---|---|---|---|---|---|
| MPC Station Code: | Código de estação MPC: | The 3-character code the MPC gave your observatory (e.g. `568`). Saved in capitals. Without it the report uses `XXX`. | O código de 3 caracteres que o MPC deu ao seu observatório (ex.: `568`). Gravado em maiúsculas. Sem ele o relatório usa `XXX`. | empty | 3 characters | `observer/mpcCode` | 🟢 |
| Observer: | Observador: | Your full name, as it should appear in the report. | Seu nome completo, como deve aparecer no relatório. | empty | text | `observer/name` | 🟢 |
| Measurer: | Medidor: | Who measured the images, if not the observer. "Leave blank to use Observer name". | Quem mediu as imagens, se não for o observador. "Deixe em branco para usar o nome do observador". | empty | text | `observer/measurer` | 🟢 |
| Telescope / Instrument: | Telescópio / Instrumento: | A short description, e.g. `0.35-m f/7 SCT + CCD`. | Uma descrição curta, ex.: `0.35-m f/7 SCT + CCD`. | empty | text | `observer/telescope` | 🟢 |

> 💡 **Tip / Dica**
> 🇬🇧 No MPC code yet? You can still practise. Real submissions need a code: ask the MPC for
> one after you have good measurements. 🇧🇷 Ainda não tem código MPC? Dá para praticar mesmo
> assim. Envios reais precisam de código: peça ao MPC depois de ter boas medições.

### Location (topocentric correction) / Localização (correção topocêntrica)

🇬🇧 *Topocentric* means "as seen from your place on Earth". Your site matters because an
asteroid near Earth looks shifted depending on where you stand. Choose **one** of four options
(the option labels are Portuguese in both languages). Key: `observer/locationMode`.

🇧🇷 *Topocêntrico* quer dizer "visto do seu lugar na Terra". O local importa porque um
asteroide próximo parece deslocado conforme onde você está. Escolha **uma** das quatro opções.
Chave: `observer/locationMode`.

| Option (both languages) | Meaning (EN) | Significado (PT) | Stored as | Level |
|---|---|---|---|---|
| ○ Automático (do FITS) | Use the site coordinates written in the image files (SITELAT/SITELONG or LAT-OBS/LONG-OBS). Greyed out when the files have none; then a badge says "FITS não fornece localização". | Usa as coordenadas do local gravadas nas imagens. Fica cinza quando os arquivos não têm; aí aparece "FITS não fornece localização". | `fits` | 🟢 |
| ○ Observatório predefinido | Pick your country ("País…") and then an observatory "CODE — Name (City)" from the built-in list of 84 observatories. Shows its Lat/Lon/Alt. | Escolha o país ("País…") e depois um observatório "CÓDIGO — Nome (Cidade)" da lista embutida de 84 observatórios. Mostra Lat/Lon/Alt. | `preset` (+ `observer/presetMpcCode`) | 🟢 |
| ○ Coordenadas manuais | Type your latitude, longitude and altitude yourself. | Digite latitude, longitude e altitude você mesmo. | `manual` | 🟢 |
| ○ Telescópio Espacial | Only enabled when the images come from a space telescope (Hubble, JWST, …). Shows the name, "Código MPC: …" and "Lat/Lon: não aplicável (órbita terrestre)". No refraction is applied. | Só habilitado quando as imagens vêm de um telescópio espacial (Hubble, JWST, …). Mostra o nome, "Código MPC: …" e "Lat/Lon: não aplicável (órbita terrestre)". Não aplica refração. | `spacecraft` | 🔴 |

Manual coordinates / Coordenadas manuais:

| Field | Meaning (EN) | Significado (PT) | Default | Range | Key |
|---|---|---|---|---|---|
| Latitude (`°  (+N)`) | Degrees, north positive, 6 decimals. Example Recife: `-8.054553`. | Graus, norte positivo, 6 casas. Exemplo Recife: `-8.054553`. | 0 | −90 … 90 | `observer/latitude` |
| Longitude (`°  (+E)`) | Degrees, **east positive** (west is negative). Recife: `-34.882830`. | Graus, **leste positivo** (oeste é negativo). Recife: `-34.882830`. | 0 | −180 … 180 | `observer/longitude` |
| Altitude (`m`) | Height above sea level in metres. | Altura acima do nível do mar em metros. | 0 | −500 … 8000 | `observer/altitude` |

> ⚠️ **Watch out / Atenção**
> 🇬🇧 If the location stays at 0°, 0°, Data Reduction warns "Localização não configurada":
> your positions would be reported as if taken from the centre of the Earth (a point in the
> Atlantic Ocean). Always set a location before a real submission. The status bar shows the
> current choice ("Obs: …").
> 🇧🇷 Se a localização ficar em 0°, 0°, a Redução de Dados avisa "Localização não
> configurada": as posições seriam relatadas como se feitas do centro da Terra (um ponto no
> Oceano Atlântico). Sempre configure o local antes de um envio real. A barra de status mostra
> a escolha atual ("Obs: …").

> 💡 **Tip / Dica**
> 🇬🇧 When you open this tab, the Manual option is pre-selected unless you chose another mode
> before. If the images carry a location, a badge suggests switching to Automático.
> 🇧🇷 Ao abrir esta aba, a opção Manual vem marcada, a menos que você tenha escolhido outro
> modo antes. Se as imagens trazem localização, um aviso sugere usar Automático.

### Time / Hora

| EN label | PT label | Meaning | Significado | Default | Range | Key | Level |
|---|---|---|---|---|---|---|---|
| Time Zone: | Fuso horário: | Your UTC offset in hours. Filled from your computer clock the first time Settings opens. | Seu fuso em horas em relação ao UTC. Preenchido pelo relógio do computador na primeira vez que as Configurações abrem. | system offset | −14 … 14 h | `observer/timeZone` | 🟡 |
| Time Offset: | Deslocamento de tempo: | A fixed correction added to every image time, in seconds (e.g. your camera clock is known to be 2 s late). | Uma correção fixa somada ao horário de toda imagem, em segundos (ex.: o relógio da câmera atrasa 2 s). | 0 | −999 … 999 s | `observer/timeOffset` | 🔴 |
| Time Precision: | Precisão de tempo: | How many decimals of seconds are written in the report time (see warning). | Quantas casas decimais de segundos vão no horário do relatório (veja o aviso). | 1.0 | 0.001 … 24 "h" | `observer/timePrecision` | 🔴 |

> ⚠️ **Watch out: Time Zone / Atenção: Fuso horário**
> 🇬🇧 The tooltip says this is "Used only if FITS headers do not contain UTC timestamps". In
> version 1.1.0 no part of the processing actually reads this value. Image times without a
> time zone are simply **assumed to be UTC**, and the Log warns "DATE-OBS sem fuso horário e
> sem TIMESYS — assumido UTC; verifique se a câmera grava hora local." If your camera records
> local time, fix the times at the source (camera/capture program) or per image in
> **Images → Edit Image Parameters...** (Julian Date).
> 🇧🇷 A dica diz que o valor é "usado apenas se os cabeçalhos FITS não tiverem horário UTC". Na
> versão 1.1.0 nenhuma parte do processamento lê esse valor. Horários sem fuso são
> simplesmente **considerados UTC**, e o Registro avisa "DATE-OBS sem fuso horário e sem
> TIMESYS — assumido UTC; verifique se a câmera grava hora local." Se sua câmera grava hora
> local, corrija na origem (câmera/programa de captura) ou por imagem em **Imagens → Editar
> Parâmetros da Imagem...** (Data Juliana).

> ⚠️ **Watch out: Time Offset / Atenção: Deslocamento de tempo**
> 🇬🇧 The label says **seconds**, and Data Reduction really adds it as seconds. But whenever
> the field is exactly 0 and a longitude is known, AstroFind fills it **by itself** with
> *longitude ÷ 15* — a number of **hours** (local solar time), e.g. `-2.33` for Recife. That
> number is then used as −2.33 **seconds**, which shifts all your times by a couple of
> seconds. It happens on loading images and on opening Settings; the Log shows
> "Auto-fill: fuso horário = UTC…h (longitude …°)".
> **What to enter:** your real clock correction in seconds if you know it. If you have none,
> enter `0.01` (one hundredth of a second — harmless). A value different from 0 stops the
> automatic fill. Check this field again after **Novo Projeto**, which resets it to 0.
> 🇧🇷 O rótulo diz **segundos**, e a Redução de Dados de fato soma em segundos. Mas sempre que
> o campo está exatamente em 0 e a longitude é conhecida, o AstroFind preenche **sozinho** com
> *longitude ÷ 15* — um número de **horas** (hora solar local), ex.: `-2.33` para Recife. Esse
> número é usado como −2,33 **segundos**, deslocando todos os horários em alguns segundos.
> Acontece ao carregar imagens e ao abrir as Configurações; o Registro mostra "Auto-fill:
> fuso horário = UTC…h (longitude …°)".
> **O que digitar:** sua correção real de relógio em segundos, se souber. Se não tiver,
> digite `0.01` (um centésimo de segundo — inofensivo). Um valor diferente de 0 impede o
> preenchimento automático. Confira de novo depois de **Novo Projeto**, que volta o campo a 0.

> ⚠️ **Watch out: Time Precision / Atenção: Precisão de tempo**
> 🇬🇧 The field shows "h" (hours), but it is **not** hours. AstroFind takes the whole-number
> part and uses it as the **number of decimal places of the seconds** in the report time
> (limited to 0–3). `1` (the default, shown `1.000 h`) = tenths of a second, e.g.
> `…T03:12:45.3Z`. **What to enter:** `1` for normal amateur timing; `2` only if your times
> are good to about 0.01 s; `0` for whole seconds. Values like `0.5` become 0 decimals.
> 🇧🇷 O campo mostra "h" (horas), mas **não** são horas. O AstroFind pega a parte inteira e usa
> como **número de casas decimais dos segundos** no horário do relatório (limitado a 0–3).
> `1` (o padrão, mostrado `1,000 h`) = décimos de segundo, ex.: `…T03:12:45.3Z`. **O que
> digitar:** `1` para cronometragem amadora normal; `2` só se seus horários forem bons até
> ~0,01 s; `0` para segundos inteiros. Valores como `0,5` viram 0 casas.

### Contact and school / Contato e escola

| EN label | PT label | Meaning | Significado | Default | Key | Level |
|---|---|---|---|---|---|---|
| Contact 1: | Contato 1: | "Name / organization". | "Nome / organização". | empty | `observer/contact1` | 🟡 |
| Contact 2: | Contato 2: | "Address line 2 (optional)". Written as `<institution>` in the XML report. | "Linha de endereço 2 (opcional)". Vai como `<institution>` no XML. | empty | `observer/contact2` | 🟡 |
| e-Mail: | E-mail: | Your e-mail. | Seu e-mail. | empty | `observer/email` | 🟡 |
| ☐ Include contact info in ADES report | ☐ Incluir informações de contato no relatório ADES | Put the three fields above into the report. | Coloca os três campos acima no relatório. | off | `observer/includeContact` | 🟡 |
| E-mail do Professor: (PT in both languages, under "Escola:") | E-mail do Professor: | The teacher's e-mail. When filled, the report window shows an **Enviar para Professor** button. | E-mail do professor. Preenchido, a janela do relatório mostra o botão **Enviar para Professor**. | empty | `school/recipientEmail` | 🟢 |

---

## Camera tab / Aba Câmera

🇬🇧 **English** — Shows the yellow "advanced" banner. All fields here are only *fallbacks*:
AstroFind prefers the values written in your image files (FITS header) and the plate solution.

🇧🇷 **Português** — Mostra a faixa amarela de "avançado". Todos os campos aqui são apenas
*reserva*: o AstroFind prefere os valores gravados nas imagens (cabeçalho FITS) e a solução de
placa.

| EN label | PT label | Meaning | Significado | Default | Range | Key | Level |
|---|---|---|---|---|---|---|---|
| Pixel Scale X: | Escala de pixel X: | How much sky one pixel covers, horizontally. Used only when the image has no scale of its own. "Auto" = 0. | Quanto céu um pixel cobre, na horizontal. Usado só quando a imagem não tem escala própria. "Automático" = 0. | 0 (Auto) | 0 … 1 (shown "°/px") | `camera/pixelScaleX` | 🔴 |
| Pixel Scale Y: | Escala de pixel Y: | Same, vertically. | O mesmo, na vertical. | 0 (Auto) | 0 … 1 | `camera/pixelScaleY` | 🔴 |
| Focal Length: | Comprimento focal: | Telescope focal length. For display only. "Unknown" = 0. | Distância focal do telescópio. Só para exibição. "Desconhecido" = 0. | 0 | 0 … 20000 mm | `camera/focalLength` | 🟡 |
| Saturation Level: | Nível de saturação: | The brightest value your camera can record before a pixel "fills up". Saturated stars give bad positions. | O maior valor que a câmera registra antes de o pixel "encher". Estrelas saturadas dão posições ruins. | 60000 | 100 … 1 000 000 ADU | `camera/saturation` | 🟡 |
| ΔT (TT − UTC): | ΔT (TT − UTC): | Difference between Terrestrial Time and UTC, added to image times. "Current value (2024): ~68 s." | Diferença entre Tempo Terrestre e UTC, somada aos horários. "Valor atual (2024): ~68 s." | 68.0 s | 0 … 200 s | `camera/deltaT` | 🔴 |
| ☑ Corrigir pixels ruins automaticamente (ativado por padrão) (PT in both languages, group "Bad Pixel Correction" / "Correção de Pixels Ruins") | same | Replaces hot pixels and bad columns with the median of their neighbours before star detection. Turning it off shows a warning: expect false detections. | Troca pixels quentes e colunas ruins pela mediana dos vizinhos antes da detecção. Desligar mostra um aviso: espere falsas detecções. | on | — | `camera/badPixelCorrection` | 🟡 |
| Limiar (σ): (PT in both languages) | Limiar (σ): | How far (in σ, noise units) a pixel must stand out to count as bad. Lower = more pixels fixed. | Quanto (em σ, unidades de ruído) um pixel precisa destoar para ser considerado ruim. Menor = mais pixels corrigidos. | 5.0 | 2.0 … 15.0 | `camera/badPixelSigma` | 🔴 |

> ⚠️ **Watch out: ΔT makes report times ~68 s late / Atenção: o ΔT atrasa o horário do relatório em ~68 s**
> 🇬🇧 In 1.1.0, **Data Reduction** adds this ΔT to every image time, **each time you run it**,
> and the ADES report then labels that time as UTC (`…Z`). With the default 68 s, `obsTime`
> comes out about 68 s late (136 s after two runs). This is bug **AUD-CORR-15** (see
> [TODO.md](https://github.com/petrinhu/astrofind/blob/main/TODO.md)). **Until it is fixed:**
> set **ΔT (TT − UTC):** to **0** and **Time Offset:** to **0** *before* the first Data
> Reduction; if you already ran it, reload the images. See [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced) for details.
>
> 🇧🇷 Na 1.1.0, a **Redução de Dados** soma este ΔT ao horário de cada imagem, **a cada vez que
> é executada**, e o relatório ADES marca esse horário como UTC (`…Z`). Com o padrão de 68 s, o
> `obsTime` sai cerca de 68 s atrasado (136 s depois de duas execuções). É o bug **AUD-CORR-15**
> (veja o [TODO.md](https://github.com/petrinhu/astrofind/blob/main/TODO.md)). **Até ser
> corrigido:** coloque **ΔT (TT − UTC):** em **0** e **Deslocamento de tempo:** em **0** *antes*
> da primeira Redução de Dados; se já rodou, recarregue as imagens. Detalhes em
> [Avançado](https://github.com/petrinhu/astrofind/wiki/Advanced).

> ⚠️ **Watch out: Pixel Scale / Atenção: Escala de pixel**
> 🇬🇧 The box says **°/px** (degrees per pixel) and its tooltip says degrees, but AstroFind
> really uses the number as **arcseconds per pixel** (″/px). When it fills the field from a
> FITS file it writes arcseconds too (Log: "Auto-fill: escala X = 1.5200"/px (FITS)").
> **What to enter:** leave it at **Auto (0)** — the plate solver finds the true scale. If you
> must type it, type **arcseconds per pixel** (formula: 206.265 × pixel size in µm ÷ focal
> length in mm; e.g. 3.76 µm and 530 mm → 1.46″/px). **Do not** convert to degrees.
> The box cannot hold more than **1**: if your scale is above 1″/px (common for small
> telescopes), a value filled from FITS is cut to `1.000000` when you press **OK**. In that
> case set it back to Auto (0) before pressing OK, and give the real scale per image in
> **Images → Edit Image Parameters...** → "Plate scale X/Y" ("/px), which is labelled
> correctly.
> 🇧🇷 A caixa diz **°/px** (graus por pixel) e a dica diz graus, mas o AstroFind usa o número
> como **segundos de arco por pixel** (″/px). Quando preenche o campo a partir de um FITS, ele
> também grava segundos de arco (Registro: "Auto-fill: escala X = 1.5200"/px (FITS)").
> **O que digitar:** deixe em **Automático (0)** — o plate solver acha a escala real. Se
> precisar digitar, digite **segundos de arco por pixel** (fórmula: 206,265 × tamanho do pixel
> em µm ÷ distância focal em mm; ex.: 3,76 µm e 530 mm → 1,46″/px). **Não** converta para
> graus. A caixa não aceita mais que **1**: se sua escala for maior que 1″/px (comum em
> telescópios pequenos), um valor vindo do FITS é cortado para `1,000000` ao apertar **OK**.
> Nesse caso volte para Automático (0) antes do OK e informe a escala real por imagem em
> **Imagens → Editar Parâmetros da Imagem...** → "Plate scale X/Y" ("/px), que tem o rótulo
> certo.

---

## Connections tab / Aba Conexões

🇬🇧 **English** — Plate solving, folders, star catalog and MPC submission. *Plate solving*
means matching the stars of your image to a star map to find exactly where the image points.

🇧🇷 **Português** — Plate solving, pastas, catálogo de estrelas e envio ao MPC. *Plate
solving* é comparar as estrelas da imagem com um mapa do céu para saber exatamente para onde
ela aponta.

### Plate Solving

| EN label | PT label | Meaning | Significado | Default | Range | Key | Level |
|---|---|---|---|---|---|---|---|
| Backend: | Backend: | Which solver to use: "astrometry.net (online)" (needs internet and a free API key) or "ASTAP (local, offline)" (a program you install yourself). | Qual solver usar: "astrometry.net (online)" (precisa de internet e chave gratuita) ou "ASTAP (local, offline)" (programa que você instala). | astrometry.net | — | `astrometry/backend` (`astrometry_net` / `astap`) | 🟢 |
| Executável ASTAP: (PT in both languages) | Executável ASTAP: | Full path of the ASTAP program, e.g. `/usr/bin/astap`. The **…** button opens "Localizar ASTAP". | Caminho completo do programa ASTAP, ex.: `/usr/bin/astap`. O botão **…** abre "Localizar ASTAP". | empty | path | `astrometry/astapPath` | 🟡 |
| API Key: | Chave de API: | Your personal key from nova.astrometry.net (link "Get a free key at nova.astrometry.net"). **Show**/**Hide** reveals it. A badge says where it is kept: "🔒 Stored in system keychain (KWallet / SecretService)" or "⚠ Stored in plain text". | Sua chave pessoal do nova.astrometry.net (link "Obtenha uma chave gratuita em nova.astrometry.net"). **Mostrar**/**Ocultar** revela. Um selo diz onde fica guardada: no chaveiro do sistema ou em texto puro. | empty | text | system keychain (service "AstroFind"), or `astrometry/apiKey` | 🟢 |
| Server URL: | URL do servidor: | The astrometry.net server. Only `https://…` (or `http://localhost…`) is accepted. | O servidor astrometry.net. Só `https://…` (ou `http://localhost…`) é aceito. | `https://nova.astrometry.net` | URL | `astrometry/baseUrl` | 🔴 |
| Timeout: | Tempo limite: | How long to wait for an online solution before giving up. | Quanto esperar pela solução online antes de desistir. | 300 s | 60 … 600 s | `astrometry/timeoutSec` | 🟡 |

> 💡 **Tip / Dica**
> 🇬🇧 No key yet? When you run Data Reduction with the astrometry.net backend, AstroFind asks
> "Enter your free API key from nova.astrometry.net:". Images that already contain a plate
> solution (WCS) are not solved again, so they need no key.
> 🇧🇷 Ainda sem chave? Ao rodar a Redução de Dados com astrometry.net, o AstroFind pede "Enter
> your free API key from nova.astrometry.net:". Imagens que já têm solução de placa (WCS) não
> são resolvidas de novo, então não precisam de chave.

### Paths / Caminhos

| EN label | PT label | Meaning | Significado | Default | Key | Level |
|---|---|---|---|---|---|---|
| MPCORB.DAT: | MPCORB.DAT: | Where the offline asteroid orbit file is kept. | Onde fica o arquivo offline de órbitas de asteroides. | `~/.local/share/AstroFind/AstroFind/MPCORB.DAT` | `catalog/mpcOrbPath` | 🟡 |
| Default image folder: | Pasta padrão de imagens: | Folder that **Load Images** opens the first time (later it remembers the last folder used, `paths/lastImageDir`). | Pasta que **Carregar Imagens** abre da primeira vez (depois lembra a última usada). | your Pictures folder | `paths/ccdDir` | 🟢 |
| Report output folder: | Pasta de saída de relatórios: | When not empty, **View ADES Report File** auto-saves `ades_report.xml` and `ades_report.psv` here each time. | Quando preenchida, **Ver Arquivo de Relatório ADES** salva automaticamente `ades_report.xml` e `ades_report.psv` aqui a cada vez. | your Documents folder | `report/outputDir` | 🟢 |
| ☐ Save WCS back to FITS file after plate solve | ☐ Salvar WCS no arquivo FITS após solução de campo | Writes the plate solution into your **original** FITS file. | Grava a solução de placa no seu arquivo FITS **original**. | off | `astrometry/saveWcs` | 🔴 |

> ⚠️ **Watch out: two report folders / Atenção: duas pastas de relatório**
> 🇬🇧 "Report output folder" is **not** the folder used by the **Save to Reports Folder**
> button of the report window. That button uses a different, hidden setting
> (`paths/reportDir`, default: your home folder) that you cannot change in this window. So
> with the defaults you get `ades_report.*` in Documents (auto-save) and `ADES_<station>_<date>.*`
> in your home folder (button). If you want one place, use **Save…** in the report window
> and pick the folder yourself; it remembers the last folder.
> 🇧🇷 "Pasta de saída de relatórios" **não** é a pasta usada pelo botão **Save to Reports
> Folder** da janela do relatório. Esse botão usa outra configuração escondida
> (`paths/reportDir`, padrão: sua pasta pessoal) que não dá para mudar nesta janela. Com os
> padrões você recebe `ades_report.*` em Documentos (salvamento automático) e
> `ADES_<estação>_<data>.*` na pasta pessoal (botão). Se quiser um lugar só, use **Save…** na
> janela do relatório e escolha a pasta; ela é lembrada.

> ⚠️ **Watch out / Atenção**
> 🇬🇧 "Save WCS back to FITS file" **changes your original file**. Keep a backup, or leave it
> off and use **File → Save all FITS** (writes `<name>_wcs.fits` copies) instead.
> 🇧🇷 "Salvar WCS no arquivo FITS" **altera seu arquivo original**. Faça cópia, ou deixe
> desligado e use **Arquivo → Salvar todos como FITS** (grava cópias `<nome>_wcs.fits`).

### Star Catalog (VizieR) / Catálogo de estrelas (VizieR)

🔴 advanced banner / faixa de avançado

| EN label | PT label | Meaning | Significado | Default | Key |
|---|---|---|---|---|---|
| Catalog: | Catálogo: | Reference stars used to measure positions and magnitudes: "UCAC4 (recommended)" or "Gaia DR3" (labels not translated). | Estrelas de referência para medir posições e magnitudes: "UCAC4 (recommended)" ou "Gaia DR3". | UCAC4 | `catalog/type` (`UCAC4` / `GaiaDR3`) |
| VizieR mirror: | Espelho VizieR: | The server the star catalog is downloaded from. | O servidor de onde o catálogo é baixado. | shown `vizier.cfa.harvard.edu`; really used: `https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync` | `catalog/vizierServer` |
| Source: | Fonte: | "VizieR (online, recommended)" or "Local FITS BINTABLE" (a catalog file on your disk, for no-internet use). | "VizieR (online, recommended)" ou "Local FITS BINTABLE" (arquivo de catálogo no disco, para uso sem internet). | VizieR | `catalog/source` (`vizier` / `local`) |
| Local catalog: | Catálogo local: | Path of that file (`/path/to/catalog.fits`), with **Browse…**. Enabled only when Source = local. | Caminho do arquivo, com **Procurar…**. Só habilitado quando Fonte = local. | empty | `catalog/localPath` |

> ⚠️ **Watch out: VizieR mirror / Atenção: espelho VizieR**
> 🇬🇧 The value shown by default, `vizier.cfa.harvard.edu`, is **rejected** by AstroFind
> because it does not start with `https://`. The program then silently keeps its built-in
> server, `https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync`, which works. So: **leave the
> field as it is.** If you really want another server, type its **complete TAP address**
> starting with `https://` (for example the default above); a bare host name will not work.
> **Reset to Defaults** puts the rejected Harvard value back, which is harmless.
> 🇧🇷 O valor mostrado por padrão, `vizier.cfa.harvard.edu`, é **rejeitado** pelo AstroFind
> porque não começa com `https://`. O programa então mantém em silêncio o servidor embutido,
> `https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync`, que funciona. Então: **deixe o campo
> como está.** Se quiser mesmo outro servidor, digite o **endereço TAP completo** começando com
> `https://` (por exemplo o padrão acima); só o nome do host não funciona. **Restaurar
> padrões** volta o valor de Harvard, o que é inofensivo.

### MPC Submission / Envio ao MPC

| EN label | PT label | Meaning | Significado | Default | Key | Level |
|---|---|---|---|---|---|---|
| Submit URL: | URL de envio: | Where **Submit to MPC (HTTP)** sends the report. Only `https://` (or localhost). | Para onde **Submit to MPC (HTTP)** envia o relatório. Só `https://` (ou localhost). | `https://www.minorplanetcenter.net/report_ades` | `mpc/submitUrl` | 🔴 |

---

## Detection tab / Aba Detecção

🇬🇧 **English** — How stars are found and how brightness is measured.

🇧🇷 **Português** — Como as estrelas são encontradas e como o brilho é medido.

### Sky Background / Fundo do Céu

| Label (PT in both languages) | Meaning (EN) | Significado (PT) | Default | Range | Key | Level |
|---|---|---|---|---|---|---|
| ☐ Subtrair modelo de fundo antes da detecção | Removes smooth brightness gradients (moonlight, light pollution) before looking for stars. Turn on if one side of your image is much brighter. | Remove gradientes suaves de brilho (Lua, poluição luminosa) antes de procurar estrelas. Ligue se um lado da imagem for bem mais claro. | off | — | `detection/backgroundSubtraction` | 🟡 |
| Tamanho do tile: | Size of the squares used to model the background. Must be much bigger than a star. | Tamanho dos quadrados usados para modelar o fundo. Precisa ser bem maior que uma estrela. | 64 px | 16 … 512, step 16 | `detection/backgroundTileSize` | 🔴 |

### Star Detection / Detecção de estrelas

🔴 advanced banner / faixa de avançado

| EN label | PT label | Meaning | Significado | Default | Range | Key |
|---|---|---|---|---|---|---|
| Detection threshold: | Limiar de detecção: | How many times brighter than the noise (σ) a spot must be to count as a star. Lower = more (and fainter, and more false) detections. | Quantas vezes acima do ruído (σ) uma mancha precisa estar para contar como estrela. Menor = mais detecções (mais fracas e mais falsas). | 4.0 σ | 1 … 20 | `detection/sigmaLimit` |
| Minimum FWHM: | FWHM mínimo: | Smallest star size accepted; smaller spots (hot pixels, cosmic rays) are dropped. FWHM = star width at half its peak. | Menor tamanho de estrela aceito; manchas menores (pixels quentes, raios cósmicos) são descartadas. FWHM = largura da estrela na metade do pico. | 0.70 | 0.1 … 10 (shown "px") | `detection/minFwhm` |
| MOD min. SNR: | MOD SNR mín.: | Minimum signal-to-noise ratio for a moving-object candidate in **Moving Object Detection**. | Relação sinal/ruído mínima de um candidato em **Detecção de Objetos em Movimento**. | 5.0 | 1 … 50 | `detection/minSnr` |
| Streak threshold (a/b): | Limiar de traço (a/b): | How elongated (long axis ÷ short axis) a source must be to be flagged as a streak (orange). | Quão alongada (eixo maior ÷ menor) uma fonte precisa ser para virar traço (laranja). | 3.0 | 1.5 … 20, step 0.5 | `detection/streakElongation` |

> ⚠️ **Watch out: Minimum FWHM / Atenção: FWHM mínimo**
> 🇬🇧 The box says **px** (pixels), but AstroFind treats the number as **arcseconds** and
> divides it by the pixel scale. It is applied only when the image scale is known.
> **What to enter:** a size in **arcseconds**. The default `0.70` (0.7″) is fine for almost
> everyone. If real faint stars disappear, lower it; if hot pixels are detected as stars, raise
> it toward about half your typical seeing (e.g. `1.0` when stars are ~2″ wide).
> 🇧🇷 A caixa diz **px** (pixels), mas o AstroFind trata o número como **segundos de arco** e
> divide pela escala do pixel. Só é aplicado quando a escala da imagem é conhecida.
> **O que digitar:** um tamanho em **segundos de arco**. O padrão `0,70` (0,7″) serve para
> quase todos. Se estrelas fracas reais somem, diminua; se pixels quentes viram estrelas,
> aumente até mais ou menos metade do seu seeing típico (ex.: `1,0` quando as estrelas têm ~2″).

### Photometry / Fotometria

🇬🇧 *Photometry* = measuring brightness (magnitude). 🇧🇷 *Fotometria* = medir o brilho
(magnitude).

| EN label | PT label | Meaning | Significado | Default | Range | Key | Level |
|---|---|---|---|---|---|---|---|
| Default band: | Banda padrão: | Filter used when the image does not say (FITS FILTER). Options: C — Clear / unfiltered, V — Johnson V, R — Cousins R, B — Johnson B, g — Sloan g', r — Sloan r', i — Sloan i', w — Pan-STARRS w. | Filtro usado quando a imagem não informa. Sem filtro = C. | C | — | `photometry/defaultBand` | 🟢 |
| Catalog mag (faint): | Magnitude do catálogo (fraca): | Faintest catalog stars to download. | Estrelas de catálogo mais fracas a baixar. | 16.0 | 10 … 21 | `catalog/magLimit` | 🟡 |
| Catalog mag (bright): | Magnitude do catálogo (brilhante): | Catalog stars **brighter** than this are ignored (they are often saturated). | Estrelas de catálogo **mais brilhantes** que isso são ignoradas (costumam saturar). | 10.0 | 0 … 15 | `catalog/maxMag` | 🟡 |
| ☑ Include magnitude in ADES report | ☑ Incluir magnitude no relatório ADES | Put the measured magnitude in the report. | Coloca a magnitude medida no relatório. | on | — | `report/includeMag` | 🟢 |
| ◉ Automatic  (2 × FWHM — recommended) | ◉ Automático  (2 × FWHM — recomendado) | Measuring circle radius = max(3 px, 2 × FWHM). Group "Aperture Radius" / "Raio da abertura". | Raio do círculo de medição = máx(3 px, 2 × FWHM). | on | — | `photometry/apertureAuto` | 🔴 |
| ○ Fixed: | ○ Fixo: | A fixed radius in pixels. **Tools → Growth Curve…** can fill it for you ("Use optimal aperture"). | Um raio fixo em pixels. **Ferramentas → Curva de Crescimento…** pode preencher ("Use optimal aperture"). | 8.0 px | 1 … 50 | `photometry/apertureManPx` | 🔴 |
| Extinction coeff k: | Coef. de extinção k: | How much the atmosphere dims stars per airmass (thickness of air). `0` = no correction. | Quanto a atmosfera apaga as estrelas por massa de ar. `0` = sem correção. | 0.0 | 0 … 1 mag/airmass | `photometry/extinctionCoeff` | 🔴 |

> 💡 **Tip / Dica**
> 🇬🇧 Magnitudes from AstroFind are differential (compared with nearby catalog stars), so the
> extinction correction usually matters little. Leave k at 0 unless you know your site's value.
> 🇧🇷 As magnitudes do AstroFind são diferenciais (comparadas com estrelas de catálogo
> próximas), então a correção de extinção costuma pesar pouco. Deixe k em 0 se não conhece o
> valor do seu local.

### Settings with no field / Configurações sem campo

🔴 🇬🇧 These exist but cannot be changed in the window: `detection/modTolerance` (2.0 px),
`detection/modMinFrames` (3) and `session/maxImages` (**20** images per session). When you load
more than 20 images, the message says to "increase the limit in Settings", but **there is no
such field**. Load at most 20 images, or edit the configuration file (see below) while
AstroFind is closed.

🔴 🇧🇷 Existem mas não dá para mudar pela janela: `detection/modTolerance` (2,0 px),
`detection/modMinFrames` (3) e `session/maxImages` (**20** imagens por sessão). Ao carregar
mais de 20 imagens, a mensagem manda "aumentar o limite nas Configurações", mas **esse campo
não existe**. Carregue no máximo 20 imagens, ou edite o arquivo de configuração (veja abaixo)
com o AstroFind fechado.

---

## Display tab / Aba Exibição

🟢

| EN label | PT label | Meaning | Significado | Default | Range | Key |
|---|---|---|---|---|---|---|
| Blink interval: | Intervalo de piscar: | Time each image stays on screen while blinking. | Tempo que cada imagem fica na tela no piscar. | 500 ms | 100 … 2000 ms | `display/blinkIntervalMs` |
| Theme: | Tema: | "Night (dark)" / "Noite (escuro)", "Day (light)" / "Dia (claro)", "Auto (follow system)" / "Automático (seguir o sistema)". | Tema de cores. | Night | — | `ui/theme` (`night`/`day`/`auto`) |
| ☑ Show setup wizard when the application starts | ☑ Mostrar assistente de configuração ao iniciar o aplicativo | Open the Setup Wizard every time AstroFind starts. | Abre o Assistente toda vez que o AstroFind inicia. | on | — | `display/showWizardOnStartup` |
| ☑ Avisar ao carregar se imagens são PB ou coloridas (PT in both languages) | same | Show the "Imagens carregadas" notice (black & white or colour) after loading. | Mostra o aviso "Imagens carregadas" (PB ou coloridas) após carregar. | on | — | `display/showColorTypeWarning` |
| Language: | Idioma: | "English" or "Português (BR)". "⟳ Restart required to apply language change." | "English" ou "Português (BR)". "⟳ Reinicialização necessária para aplicar a mudança de idioma." | your system language | — | `ui/language` (`en` / `pt_BR`) |

> 💡 **Tip / Dica**
> 🇬🇧 Night theme keeps your eyes adapted to the dark at the telescope. Even in English, some
> labels stay in Portuguese: see [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference).
> 🇧🇷 O tema Noite preserva a adaptação dos olhos ao escuro no telescópio. Mesmo em português,
> alguns rótulos ficam em inglês (por exemplo as colunas da tabela Observações).

---

## Legacy tab / Aba Legado

🔴

🇬🇧 **English** — A red banner reads "OBSOLETE — These local catalog paths are kept for
compatibility only." Fields: **USNO-A2.0 directory:**, **UCAC-2 directory:**, **UCAC-3
directory:**, **CMC-14 directory:** (placeholder "(not configured)"). Keys
`legacy/usnoA2Dir`, `legacy/ucac2Dir`, `legacy/ucac3Dir`, `legacy/cmc14Dir`. **Nothing in
AstroFind reads these folders.** For an offline catalog use **Connections → Source: Local FITS
BINTABLE** instead.

🇧🇷 **Português** — Uma faixa vermelha diz "OBSOLETO — Esses caminhos de catálogo local são
mantidos apenas por compatibilidade." Campos: **Diretório USNO-A2.0:**, **Diretório UCAC-2:**,
**Diretório UCAC-3:**, **Diretório CMC-14:** (texto "(não configurado)"). **Nada no AstroFind
lê essas pastas.** Para catálogo offline use **Conexões → Fonte: Local FITS BINTABLE**.

---

## Setup Wizard / Assistente de Configuração

🟢

🇬🇧 **English** — The "AstroFind Setup Wizard" opens a moment after start-up while **Show
setup wizard when the application starts** is on, and any time from **Help → Setup Wizard…**.
Every page has **Skip this step**. A field left empty does **not** erase a value you already
had.

🇧🇷 **Português** — O "Assistente de configuração do AstroFind" abre logo depois de iniciar
enquanto **Mostrar assistente de configuração ao iniciar o aplicativo** estiver ligado, e a
qualquer momento por **Ajuda → Assistente de Configuração…**. Toda página tem **Pular esta
etapa**. Um campo deixado vazio **não** apaga um valor que você já tinha.

| Wizard page | Field EN / PT | Same as Settings field | Key |
|---|---|---|---|
| Step 1 of 3 — Observer Identity / Etapa 1 de 3 — Identidade do observador | MPC Station Code: / Código de estação MPC: | Observer → MPC Station Code | `observer/mpcCode` |
| | Your name: / Seu nome: | Observer → Observer | `observer/name` |
| | Telescope: / Telescópio: | Observer → Telescope / Instrument | `observer/telescope` |
| | ☐ Show this wizard every time AstroFind starts / Mostrar este assistente sempre que o AstroFind iniciar | Display → Show setup wizard… | `display/showWizardOnStartup` |
| Step 2 of 3 — Plate-solving API Key / Etapa 2 de 3 — Chave de API para solução de campo | API key ("Paste your key here" / "Cole sua chave aqui"), **Show**/**Hide**, link "Register for a free API key at nova.astrometry.net" | Connections → API Key | `astrometry/apiKey`, moved into the system keychain the first time it is read when a keychain is available |
| Step 3 of 3 — Asteroid Catalog (optional) / Etapa 3 de 3 — Catálogo de asteroides (opcional) | **Download now (~200 MB)** / **Baixar agora (~200 MB)**, **Cancel**; status "Not downloaded" or "✓ MPCORB.DAT already present" | Same as **Internet → Download MPCOrb**; file at Connections → MPCORB.DAT | `catalog/mpcOrbPath` (file location) |

🇬🇧 The wizard does **not** ask for your location. Set it afterwards in **Settings →
Observer → Location**. Without an API key you can still work: images that already have a plate
solution are measured normally, and you can switch to the ASTAP backend.

🇧🇷 O assistente **não** pede sua localização. Configure depois em **Configurações →
Observador → Localização**. Sem chave de API dá para trabalhar: imagens que já têm solução de
placa são medidas normalmente, e você pode trocar para o backend ASTAP.

---

## Settings that change by themselves / Configurações que mudam sozinhas

🟡

🇬🇧 **English** — AstroFind fills some settings from the **first** image of a session. This
happens after loading images and also just before the Settings window opens. The Log shows an
"Auto-fill:" line each time.

- **Pixel scale** and **saturation**: filled from the FITS header when they are 0.
- **Location**: space telescope → mode Telescópio Espacial; FITS site coordinates → mode
  Automático; an MPC code in the header that matches the built-in list → mode Observatório
  predefinido.
- **Time Offset**: filled with longitude ÷ 15 when it is 0 (see the warning in the Observer
  tab).
- **Time Zone**: filled from your computer clock the first time Settings opens.
- Choosing **Novo Projeto** in the "Sessão em andamento" dialog sets pixel scale, saturation,
  latitude, longitude, altitude and time offset back to 0 and the location mode to FITS.

🇧🇷 **Português** — O AstroFind preenche algumas configurações a partir da **primeira** imagem
da sessão. Isso acontece depois de carregar imagens e também logo antes de abrir a janela de
Configurações. O Registro mostra uma linha "Auto-fill:" a cada vez.

- **Escala de pixel** e **saturação**: vindas do cabeçalho FITS quando estão em 0.
- **Localização**: telescópio espacial → modo Telescópio Espacial; coordenadas no FITS → modo
  Automático; código MPC no cabeçalho que existe na lista embutida → modo Observatório
  predefinido.
- **Deslocamento de tempo**: preenchido com longitude ÷ 15 quando está em 0 (veja o aviso na
  aba Observador).
- **Fuso horário**: preenchido pelo relógio do computador na primeira vez que as
  Configurações abrem.
- Escolher **Novo Projeto** na janela "Sessão em andamento" volta escala, saturação, latitude,
  longitude, altitude e deslocamento de tempo para 0 e o modo de localização para FITS.

---

## Where settings are stored, and how to reset / Onde ficam as configurações e como restaurar

🟡

🇬🇧 **English**

- **Settings file:** `~/.config/AstroFind/AstroFind.conf` (a plain text file; `~` is your
  home folder). It also stores recent folders/projects and the window layout.
- **API key:** in the system keychain (KWallet or GNOME Keyring/Secret Service, service name
  "AstroFind") when AstroFind was built with keychain support; otherwise inside the settings
  file as `astrometry/apiKey`, in plain text. The badge in the Connections tab tells you which.
- **Data files:** `~/.local/share/AstroFind/AstroFind/` holds `MPCORB.DAT` (unless you chose
  another path) and the star-catalog cache `catalog_cache.db`.

**Reset to Defaults** (button in the Settings window):

1. Click **Reset to Defaults** / **Restaurar padrões**. The fields change on screen.
2. Press **OK** to keep the reset, or **Cancel** to undo it.

It resets the location (to Manual 0°, 0°, 0 m), time fields, contact checkbox, all Camera
fields, server URL, timeout, folders, Save-WCS, catalog type, VizieR mirror, submit URL, all
Detection fields, blink interval, theme, both Display checkboxes, the teacher e-mail and the
Legacy folders. It **keeps** your MPC code, name, telescope, contacts and e-mail, the solver
backend, ASTAP path, API key, catalog source and local catalog path, and the language. It
clears the Measurer field.

**Full reset** (everything, as on a new computer):

```bash
# 1. Close AstroFind first. Then rename the settings file (a backup, in case you want it back):
mv ~/.config/AstroFind/AstroFind.conf ~/.config/AstroFind/AstroFind.conf.bak
```

`mv` moves/renames a file. Next time AstroFind starts, it creates a fresh settings file and
shows the Setup Wizard. To undo, rename it back. The API key in the keychain is not touched.

**Copying settings to another computer (teachers):** **File → Exportar Configuração da
Escola...** writes every setting to an `.ini` file; **File → Importar Configuração da
Escola...** reads it on the other computer. See [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference).

🇧🇷 **Português**

- **Arquivo de configurações:** `~/.config/AstroFind/AstroFind.conf` (um arquivo de texto;
  `~` é sua pasta pessoal). Também guarda pastas/projetos recentes e o layout da janela.
- **Chave de API:** no chaveiro do sistema (KWallet ou GNOME Keyring/Secret Service, serviço
  "AstroFind") quando o AstroFind foi compilado com esse suporte; senão dentro do arquivo de
  configurações como `astrometry/apiKey`, em texto puro. O selo na aba Conexões diz qual.
- **Arquivos de dados:** `~/.local/share/AstroFind/AstroFind/` guarda o `MPCORB.DAT` (a menos
  que você tenha escolhido outro caminho) e o cache de catálogo `catalog_cache.db`.

**Restaurar padrões** (botão na janela de Configurações):

1. Clique em **Restaurar padrões**. Os campos mudam na tela.
2. Aperte **OK** para manter, ou **Cancelar** para desfazer.

Ele restaura a localização (Manual 0°, 0°, 0 m), campos de hora, a caixa de contato, todos os
campos da Câmera, URL do servidor, tempo limite, pastas, Salvar WCS, tipo de catálogo, espelho
VizieR, URL de envio, todos os campos de Detecção, intervalo de piscar, tema, as duas caixas de
Exibição, o e-mail do professor e as pastas do Legado. Ele **mantém** código MPC, nome,
telescópio, contatos e e-mail, o backend do solver, caminho do ASTAP, chave de API, fonte e
caminho do catálogo local e o idioma. Ele apaga o campo Medidor.

**Restauração completa** (tudo, como num computador novo):

```bash
# 1. Feche o AstroFind antes. Depois renomeie o arquivo (fica como cópia, caso queira voltar):
mv ~/.config/AstroFind/AstroFind.conf ~/.config/AstroFind/AstroFind.conf.bak
```

`mv` move/renomeia um arquivo. Na próxima vez, o AstroFind cria um arquivo novo e mostra o
Assistente de Configuração. Para desfazer, renomeie de volta. A chave de API no chaveiro não é
afetada.

**Levar as configurações para outro computador (professores):** **Arquivo → Exportar
Configuração da Escola...** grava tudo num arquivo `.ini`; **Arquivo → Importar Configuração
da Escola...** lê no outro computador.

---

## Summary of label/unit problems / Resumo dos problemas de rótulo e unidade

🟡

| Field | Shown as | Really means | Enter / Digite |
|---|---|---|---|
| Camera → Pixel Scale X/Y | °/px, max 1 | ″/px (arcsec per pixel) | Auto (0), or arcsec/px; if > 1″/px use Images → Edit Image Parameters |
| Detection → Minimum FWHM | px | arcseconds | Keep 0.70 (arcsec) |
| Observer → Time Offset | s | seconds, but auto-filled with hours (longitude ÷ 15) | Your real correction in s, or 0.01 |
| Observer → Time Precision | h | number of decimals of seconds (0–3) | 1 |
| Observer → Time Zone | h, "used if no UTC" | not used by the processing | anything; fix times at the source |
| Connections → VizieR mirror | vizier.cfa.harvard.edu | rejected; built-in https TAP server is used | leave as is, or a full `https://` TAP URL |
| Connections → Report output folder | one folder | only auto-save; "Save to Reports Folder" uses your home folder | use **Save…** to choose |
| (no field) session limit | "increase the limit in Settings" | no such field; 20 images | load ≤ 20 images |

See also / Veja também: [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference), [Manual](https://github.com/petrinhu/astrofind/wiki/Manual), [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting), [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ),
[Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).
