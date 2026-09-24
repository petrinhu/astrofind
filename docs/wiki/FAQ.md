# FAQ — Frequently Asked Questions / Perguntas Frequentes

🇬🇧 **English**
**Who this page is for:** everyone, from people opening AstroFind for the first time to
experienced Astrometrica/MPC observers. Each answer is short and gives the exact menu path.
If something is going wrong right now, go to [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting), which lists real error
messages and how to fix them. Words you don't know are explained in the [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Português**
**Para quem é esta página:** para todos, de quem está abrindo o AstroFind pela primeira vez
até observadores experientes do Astrometrica/MPC. Cada resposta é curta e traz o caminho exato
no menu. Se algo está dando errado agora, vá para [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting), que lista as mensagens
de erro reais e como resolvê-las. As palavras que você não conhece estão explicadas no
[Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

**Contents / Conteúdo**

- About AstroFind / Sobre o AstroFind: Is it free? · Windows/macOS? · Is it validated? ·
  Coming from Astrometrica
- Files and data / Arquivos e dados: Which files? · DSLR · NASA PDS · No telescope · How many
  images?
- Setup / Configuração: Internet? · Which catalog? · Which plate solver? · MPC code
- Using it / Usando: Black image · No plate solution · Solver fails · Ambiguous time · Blink ·
  Report to the MPC
- Help / Ajuda: Common problems · Report a bug

---

## Is AstroFind free? / O AstroFind é gratuito?

🟢 Beginner / Iniciante

🇬🇧 **English**
Yes. AstroFind is free and open-source software (anyone can read and change its source code).
It uses the **AGPL-3.0** license: you may use it for anything, including classes and paid
projects. If you distribute a modified version, or offer one as a network service, you must
publish its source code under the same license. See the `LICENSE` file in the repository for
the exact terms.

> ⚠️ **Watch out:** **Help → About…** correctly says "AGPL-3.0". **Help → Registration…**
> shows "Registration — N/A (MIT open source)". That MIT text is an old leftover. The license is
> AGPL-3.0.

For schools there are extra helpers: **File → Exportar Configuração da Escola…** (a teacher
saves all settings to an `.ini` file) and **File → Importar Configuração da Escola…** (students
load it). These menu items appear in Portuguese even in the English interface.

🇧🇷 **Português**
Sim. O AstroFind é software livre e de código aberto (qualquer pessoa pode ler e alterar o
código-fonte). A licença é a **AGPL-3.0**: você pode usá-lo para qualquer coisa, inclusive
aulas e projetos pagos. Se você distribuir uma versão modificada, ou oferecê-la como serviço
pela rede, precisa publicar o código-fonte dela sob a mesma licença. Veja o arquivo `LICENSE`
no repositório para os termos exatos.

> ⚠️ **Atenção:** **Ajuda → Sobre…** mostra corretamente "AGPL-3.0". **Ajuda → Registro…**
> mostra "Registro — N/D (código aberto MIT)". Esse texto de MIT é uma sobra antiga. A licença é
> AGPL-3.0.

Para escolas há recursos extras: **Arquivo → Exportar Configuração da Escola…** (o professor
salva todas as configurações num arquivo `.ini`) e **Arquivo → Importar Configuração da
Escola…** (os alunos carregam esse arquivo).

---

## Does it run on Windows or macOS? / Roda no Windows ou no macOS?

🟢 Beginner / Iniciante

🇬🇧 **English**
No. AstroFind runs only on **Linux**. Version 1.1.0 is tested on Ubuntu 24.04, Debian 12,
Fedora 44, Arch Linux, CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9 and Linux Mint 22.
Pop!_OS and Zorin OS are checked with Ubuntu 24.04 containers (test machines that imitate
those systems), not with the real systems. Installation steps: [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) and
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md).

If you use Windows or macOS, one option is to run Linux inside a virtual machine (a program
that runs a second operating system in a window). This is not tested by the project.

🇧🇷 **Português**
Não. O AstroFind roda apenas no **Linux**. A versão 1.1.0 é testada no Ubuntu 24.04, Debian 12,
Fedora 44, Arch Linux, CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9 e Linux Mint 22.
Pop!_OS e Zorin OS são verificados com containers do Ubuntu 24.04 (máquinas de teste que imitam
esses sistemas), não com os sistemas reais. Passos de instalação: [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) e
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md).

Se você usa Windows ou macOS, uma opção é rodar o Linux dentro de uma máquina virtual (um
programa que roda um segundo sistema operacional numa janela). Isso não é testado pelo projeto.

---

## Is AstroFind validated? Is it safe to submit its results? / O AstroFind é validado? É seguro enviar os resultados?

🟢 Beginner / Iniciante · 🔴 Advanced / Avançado

🇬🇧 **English**
AstroFind has a large automated test suite, but it has **not yet been validated end to end
with real observatory data**. Treat every result as something to check before you send it to
the MPC (Minor Planet Center, the body that collects asteroid observations). Good checks:

1. Measure a **known** asteroid in your field and compare with a prediction. Use
   **Tools → Known Object Overlay** (`Ctrl+K`), or **Internet → Query Horizons…**. The Horizons
   query is **geocentric** (seen from the centre of the Earth), so expect small differences for
   nearby objects.
2. Look at the WCS RMS line in the Log panel after the Known Object Overlay, for example
   `Image 1: WCS RMS = 0.4" (35 matched stars)`. A small value (well under 1") is a good sign.
3. Check the observation time and your MPC station code in the report preview.
4. Read the report itself (XML and PSV tabs) before pressing any Submit button.

🇧🇷 **Português**
O AstroFind tem uma grande suíte de testes automáticos, mas **ainda não foi validado de ponta
a ponta com dados reais de observatório**. Trate todo resultado como algo a conferir antes de
enviar ao MPC (Minor Planet Center, o órgão que reúne observações de asteroides). Boas
conferências:

1. Meça um asteroide **conhecido** no seu campo e compare com uma previsão. Use
   **Ferramentas → Sobreposição de Objetos Conhecidos** (`Ctrl+K`) ou **Internet → Consultar
   Horizons…**. A consulta ao Horizons é **geocêntrica** (vista do centro da Terra), então
   espere pequenas diferenças para objetos próximos.
2. Veja a linha de WCS RMS no painel de Log depois da Sobreposição de Objetos Conhecidos, por
   exemplo `Image 1: WCS RMS = 0.4" (35 matched stars)`. Um valor pequeno (bem abaixo de 1") é
   bom sinal.
3. Confira o horário da observação e o seu código de estação MPC na pré-visualização do
   relatório.
4. Leia o próprio relatório (abas XML e PSV) antes de apertar qualquer botão de envio.

---

## I use Astrometrica — what is different? / Eu uso o Astrometrica — o que muda?

🔴 Advanced / Avançado

🇬🇧 **English**
AstroFind is inspired by Astrometrica (Herbert Raab) and follows the same idea: load, reduce,
overlay known objects, blink, measure, report. Main differences:

| Topic | AstroFind |
|---|---|
| Report format | Only **ADES 2022** (XML and PSV). There is **no MPC 80-column** (MPC1992) output. |
| Plate solving | Blind solving with astrometry.net (online) or ASTAP (offline). Images that already have a WCS in the header are not solved again. |
| Reference catalog | UCAC4 (default) or Gaia DR3 from VizieR, or a local FITS BINTABLE file. The **Legacy** tab paths (USNO-A2.0, UCAC-2/3, CMC-14) are stored but never used. |
| Known objects | IMCCE SkyBoT online; offline fallback with MPCORB.DAT. |
| Main keys | `Ctrl+A` Data Reduction, `Ctrl+K` Known Object Overlay, `Ctrl+B` Blink, `Ctrl+M` Moving Object Detection, `M` Measure. Full list: [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference). |
| Positions | Refraction correction is applied to ground-based measurements; the report says `sys=ICRF`. |
| Session size | 20 images per session by default (see below). |

🇧🇷 **Português**
O AstroFind é inspirado no Astrometrica (Herbert Raab) e segue a mesma ideia: carregar,
reduzir, sobrepor objetos conhecidos, piscar, medir, reportar. Principais diferenças:

| Tema | AstroFind |
|---|---|
| Formato de relatório | Só **ADES 2022** (XML e PSV). **Não existe** saída no formato **MPC de 80 colunas** (MPC1992). |
| Plate solving | Solução "às cegas" com astrometry.net (online) ou ASTAP (offline). Imagens que já têm WCS no cabeçalho não são resolvidas de novo. |
| Catálogo de referência | UCAC4 (padrão) ou Gaia DR3 via VizieR, ou um arquivo FITS BINTABLE local. Os caminhos da aba **Legado** (USNO-A2.0, UCAC-2/3, CMC-14) são guardados mas nunca usados. |
| Objetos conhecidos | IMCCE SkyBoT online; alternativa offline com o MPCORB.DAT. |
| Teclas principais | `Ctrl+A` Redução de Dados, `Ctrl+K` Sobreposição de Objetos Conhecidos, `Ctrl+B` Piscar, `Ctrl+M` Detecção de Objetos em Movimento, `M` Medir. Lista completa: [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference). |
| Posições | A correção de refração é aplicada em medições feitas do solo; o relatório usa `sys=ICRF`. |
| Tamanho da sessão | 20 imagens por sessão por padrão (veja abaixo). |

---

## What kind of file do I open? / Que tipo de arquivo eu abro?

🟢 Beginner / Iniciante

🇬🇧 **English**
Usually a **FITS** file (`.fits`, `.fit` or `.fts`), the standard format for astronomical
images (see [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)). Open it with **File → Load Images…** (`Ctrl+L`, but see the shortcut clash under "What are the most common problems?" below).
AstroFind also reads:

- SER video (`.ser`) and PixInsight XISF (`.xisf`);
- TIFF, PNG, BMP and JPEG (16-bit grey TIFF/PNG keeps full precision; 8-bit is less precise);
- DSLR/mirrorless **RAW** files (`.cr2`, `.cr3`, `.nef`, `.arw`, `.dng`, `.raf`, `.orf`,
  `.rw2`, `.pef` and more), when AstroFind was built with LibRaw;
- NASA **PDS3** (`.img`, with the label inside or in a `.lbl` file beside it) and **PDS4**
  (`.xml` label);
- ZIP, TAR.GZ, TAR.BZ2, TAR.XZ, 7Z and RAR archives, extracted automatically. Not every format
  inside an archive is extracted: BMP, JPEG and PDS4 `.xml` are skipped.

Full details: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

> ⚠️ **Watch out:** dragging files onto the window only accepts `.fits`, `.fit`, `.fts`, ZIP
> and the other archive types. Use **File → Load Images…** for everything else.

🇧🇷 **Português**
Normalmente um arquivo **FITS** (`.fits`, `.fit` ou `.fts`), o formato padrão de imagens
astronômicas (veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)). Abra com **Arquivo → Carregar Imagens…**
(`Ctrl+L`, mas veja o conflito de atalho em "Quais são os problemas mais comuns?" abaixo).
O AstroFind também lê:

- vídeo SER (`.ser`) e XISF do PixInsight (`.xisf`);
- TIFF, PNG, BMP e JPEG (TIFF/PNG cinza de 16 bits mantém a precisão total; 8 bits é menos
  preciso);
- arquivos **RAW** de DSLR/mirrorless (`.cr2`, `.cr3`, `.nef`, `.arw`, `.dng`, `.raf`,
  `.orf`, `.rw2`, `.pef` e outros), quando o AstroFind foi compilado com LibRaw;
- **PDS3** da NASA (`.img`, com o rótulo dentro ou num arquivo `.lbl` ao lado) e **PDS4**
  (rótulo `.xml`);
- arquivos compactados ZIP, TAR.GZ, TAR.BZ2, TAR.XZ, 7Z e RAR, extraídos automaticamente. Nem
  todo formato dentro deles é extraído: BMP, JPEG e `.xml` PDS4 são ignorados.

Detalhes completos: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

> ⚠️ **Atenção:** arrastar arquivos para a janela só aceita `.fits`, `.fit`, `.fts`, ZIP e os
> outros tipos compactados. Para o resto, use **Arquivo → Carregar Imagens…**.

---

## Can I use my DSLR camera? / Posso usar minha câmera DSLR?

🟢 Beginner / Iniciante · 🟡 Intermediate / Intermediário

🇬🇧 **English**
Yes. Open the camera's RAW file directly with **File → Load Images…**; no conversion to FITS
is needed. AstroFind keeps the data linear (no gamma curve, no white balance), which is what
measurements need. It detects stars on a 2×2 "superpixel" brightness image, made from each
group of four coloured sensor pixels. For the screen it builds a simple colour picture. Three
things to check:

- **Time.** The time comes from the camera clock (EXIF data inside the file). It is usually
  local time with no time zone, so AstroFind flags it as **ambiguous** (see "Why is the time
  flagged ambiguous?" below). Set the camera clock to UTC before observing, or correct the time
  of each image in **Images → Edit Image Parameters…** (field "Julian Date:", the mid-exposure
  time).
- **Build.** RAW support needs the LibRaw library when AstroFind is compiled: `LibRaw-devel` on
  Fedora and Rocky Linux 9 (CRB repository), `libraw-dev` on Debian/Ubuntu/Mint, `libraw` on
  Arch/Manjaro/CachyOS, `libraw-devel` on openSUSE. Without it, RAW files are refused with the
  message "DSLR RAW support is not available in this build of AstroFind (compiled without
  LibRaw)".
- **Plate solving.** The solver receives your original file. If it cannot read that RAW
  format, solving fails; see [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

🇧🇷 **Português**
Sim. Abra o arquivo RAW da câmera direto com **Arquivo → Carregar Imagens…**; não é preciso
converter para FITS. O AstroFind mantém os dados lineares (sem curva de gama, sem balanço de
branco), que é o que as medições precisam. Ele detecta estrelas numa imagem de brilho por
"superpixel" 2×2, feita de cada grupo de quatro pixels coloridos do sensor. Para a tela, monta
uma imagem colorida simples. Três coisas para conferir:

- **Horário.** O horário vem do relógio da câmera (dados EXIF dentro do arquivo). Ele costuma
  estar no horário local, sem fuso, então o AstroFind marca o horário como **ambíguo** (veja
  "Por que o horário aparece como ambíguo?" abaixo). Acerte o relógio da câmera em UTC antes de
  observar, ou corrija o horário de cada imagem em **Imagens → Editar Parâmetros da Imagem…**
  (campo "Data Juliana:", o meio da exposição).
- **Compilação.** O suporte a RAW precisa da biblioteca LibRaw quando o AstroFind é compilado:
  `LibRaw-devel` no Fedora e no Rocky Linux 9 (repositório CRB), `libraw-dev` no
  Debian/Ubuntu/Mint, `libraw` no Arch/Manjaro/CachyOS, `libraw-devel` no openSUSE. Sem ela,
  arquivos RAW são recusados com a mensagem "DSLR RAW support is not available in this build of
  AstroFind (compiled without LibRaw)".
- **Plate solving.** O solver recebe o seu arquivo original. Se ele não souber ler aquele
  formato RAW, a solução falha; veja [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

---

## Can I open NASA PDS images? / Posso abrir imagens PDS da NASA?

🟡 Intermediate / Intermediário

🇬🇧 **English**
Yes. PDS (Planetary Data System) is NASA's archive format for mission data.

- **PDS3:** select the `.img` file. The label (the text that describes the image) can be
  inside the `.img` or in a `.lbl` file with the same name in the same folder. Selecting both
  is fine; AstroFind drops the extra `.lbl`.
- **PDS4:** select the `.xml` label. If that label points to a FITS file, AstroFind opens the
  FITS.

Mission images rarely carry a plate solution (the link between pixels and sky coordinates), so
run **Astrometry → Data Reduction…** (`Ctrl+A`) first. Some old PDS3 files use VAX number
formats; those are refused ("VAX floating point PDS3 images are not supported").

> ⚠️ **Watch out:** observations made from a spacecraft need extra information for the MPC
> (the spacecraft position), which AstroFind does not write. Use PDS images for learning and
> practice unless you know what the MPC requires for your case.

🇧🇷 **Português**
Sim. PDS (Planetary Data System) é o formato de arquivo da NASA para dados de missões.

- **PDS3:** selecione o arquivo `.img`. O rótulo (o texto que descreve a imagem) pode estar
  dentro do `.img` ou num arquivo `.lbl` com o mesmo nome, na mesma pasta. Selecionar os dois
  não tem problema; o AstroFind descarta o `.lbl` a mais.
- **PDS4:** selecione o rótulo `.xml`. Se esse rótulo aponta para um arquivo FITS, o AstroFind
  abre o FITS.

Imagens de missões raramente trazem solução de placa (a ligação entre pixels e coordenadas do
céu), então rode **Astrometria → Redução de Dados…** (`Ctrl+A`) primeiro. Alguns arquivos PDS3
antigos usam formatos numéricos VAX; eles são recusados ("VAX floating point PDS3 images are
not supported").

> ⚠️ **Atenção:** observações feitas a partir de uma nave precisam de informação extra para o
> MPC (a posição da nave), que o AstroFind não escreve. Use imagens PDS para aprender e praticar,
> a menos que você saiba o que o MPC exige no seu caso.

---

## I don't have a telescope — where do I get data? / Não tenho telescópio — onde consigo dados?

🟢 Beginner / Iniciante

🇬🇧 **English**
You don't need one to practise. Many observatories and public sky surveys publish real FITS
images for free, for example the SDSS, Pan-STARRS and CFHT archives, and remote-telescope rental
services such as iTelescope.Net. For school projects, look at the **IASC** (International
Astronomical Search Collaboration), which gives classrooms real asteroid-search images.

🇧🇷 **Português**
Você não precisa de um para praticar. Muitos observatórios e levantamentos públicos do céu
publicam imagens FITS reais de graça, por exemplo os arquivos do SDSS, Pan-STARRS e CFHT, e
serviços de aluguel de telescópio remoto como o iTelescope.Net. Para projetos escolares, veja o
**IASC** (International Astronomical Search Collaboration), que fornece a turmas imagens reais
de busca de asteroides.

---

## How many images can I load? / Quantas imagens posso carregar?

🟢 Beginner / Iniciante · 🔴 Advanced / Avançado

🇬🇧 **English**
**20 images per session** by default. Extra files are not loaded and a "Session Limit Reached"
message appears. That message says "increase the limit in Settings", but **there is no such
field in Settings**. What you can do:

- Close the current session (**File → Close Project**) and load the next group. Asteroid work
  usually needs only 3 to 5 images of a field.
- Advanced: change the hidden setting `session/maxImages`. Close AstroFind, then edit the
  settings file:

  ```bash
  nano ~/.config/AstroFind/AstroFind.conf
  ```

  (`nano` is a simple text editor that runs in the terminal; this file holds all AstroFind
  settings.) Under the `[session]` line, write `maxImages=40` (add the `[session]` line if it is
  missing). Save with `Ctrl+O`, exit with `Ctrl+X`, then start AstroFind again.

> 💡 **Tip:** the blink view shows at most 4 thumbnail slots, even when more images are loaded.

🇧🇷 **Português**
**20 imagens por sessão** por padrão. Os arquivos a mais não são carregados e aparece a mensagem
"Limite de Sessão Atingido". Essa mensagem manda "aumente o limite nas Configurações", mas
**esse campo não existe nas Configurações**. O que você pode fazer:

- Feche a sessão atual (**Arquivo → Fechar Projeto**) e carregue o próximo grupo. O trabalho
  com asteroides normalmente precisa de só 3 a 5 imagens de um campo.
- Avançado: mude a configuração escondida `session/maxImages`. Feche o AstroFind e edite o
  arquivo de configurações:

  ```bash
  nano ~/.config/AstroFind/AstroFind.conf
  ```

  (`nano` é um editor de texto simples que roda no terminal; esse arquivo guarda todas as
  configurações do AstroFind.) Abaixo da linha `[session]`, escreva `maxImages=40` (crie a linha
  `[session]` se ela não existir). Salve com `Ctrl+O`, saia com `Ctrl+X` e abra o AstroFind de
  novo.

> 💡 **Dica:** a visão de piscar mostra no máximo 4 miniaturas, mesmo com mais imagens
> carregadas.

---

## Do I need internet? / Preciso de internet?

🟢 Beginner / Iniciante

🇬🇧 **English**
Not always. These parts use the internet:

| Task | Online service | Offline alternative |
|---|---|---|
| Plate solving | astrometry.net | ASTAP (install the program and a star database) |
| Reference stars | VizieR (UCAC4 or Gaia DR3) | Local FITS BINTABLE catalog (**Settings → Connections** → "Source:" → "Local FITS BINTABLE") |
| Known asteroids | IMCCE SkyBoT | MPCORB.DAT, downloaded once with **Internet → Download MPCOrb** (about 200 MB) |
| Ephemeris of one object | JPL Horizons | none |
| Sending the report | MPC (HTTP or e-mail) | Save the report file and send it later |

Images that already have a plate solution in their header skip the solving step completely.

> ⚠️ **Watch out:** the Download MPCOrb status tip says "~500 MB", but the confirmation dialog
> and the Setup Wizard say "~200 MB".

🇧🇷 **Português**
Nem sempre. Estas partes usam a internet:

| Tarefa | Serviço online | Alternativa offline |
|---|---|---|
| Plate solving | astrometry.net | ASTAP (instale o programa e um banco de estrelas) |
| Estrelas de referência | VizieR (UCAC4 ou Gaia DR3) | Catálogo local FITS BINTABLE (**Configurações → Conexões** → "Fonte:" → "FITS BINTABLE local") |
| Asteroides conhecidos | IMCCE SkyBoT | MPCORB.DAT, baixado uma vez em **Internet → Baixar MPCOrb** (cerca de 200 MB) |
| Efeméride de um objeto | JPL Horizons | nenhuma |
| Envio do relatório | MPC (HTTP ou e-mail) | Salve o arquivo do relatório e envie depois |

Imagens que já têm solução de placa no cabeçalho pulam totalmente a etapa de solução.

> ⚠️ **Atenção:** a dica da barra de status do Baixar MPCOrb diz "~500 MB", mas o diálogo de
> confirmação e o Assistente de Configuração dizem "~200 MB".

---

## Which star catalog should I use? / Qual catálogo de estrelas devo usar?

🟡 Intermediate / Intermediário

🇬🇧 **English**
Keep the default, **UCAC4**, unless you have a reason to change. The setting is in
**File → Settings…** (`Ctrl+,`) → **Connections** → "Catalog:", with "UCAC4 (recommended)" or
"Gaia DR3". Gaia DR3 has more precise positions and fainter stars; its magnitudes are in the
Gaia G band. Also check "Catalog mag (faint):" (default 16) and "Catalog mag (bright):" (default
10) on the **Detection** tab: stars outside that range are not used.

> 💡 **Tip:** the Image Catalog tooltip for "Estrela Guia" always says "UCAC4", even when Gaia
> DR3 is selected. The catalog really used is written in the report (`astCat`).

🇧🇷 **Português**
Fique com o padrão, **UCAC4**, a menos que tenha um motivo para mudar. A opção fica em
**Arquivo → Configurações…** (`Ctrl+,`) → **Conexões** → "Catálogo:", com "UCAC4
(recommended)" ou "Gaia DR3" (esses nomes não são traduzidos). O Gaia DR3 tem posições mais
precisas e estrelas mais fracas; as magnitudes dele estão na banda G do Gaia. Confira também
"Magnitude do catálogo (fraca):" (padrão 16) e "Magnitude do catálogo (brilhante):" (padrão 10)
na aba **Detecção**: estrelas fora dessa faixa não são usadas.

> 💡 **Dica:** a dica de "Estrela Guia" no Catálogo de Imagem sempre diz "UCAC4", mesmo com o
> Gaia DR3 escolhido. O catálogo realmente usado vai escrito no relatório (`astCat`).

---

## Which plate solver should I use? / Qual plate solver devo usar?

🟢 Beginner / Iniciante · 🟡 Intermediate / Intermediário

🇬🇧 **English**
A plate solver works out which part of the sky an image shows. AstroFind can use two:

| | astrometry.net (default) | ASTAP |
|---|---|---|
| Where it runs | Online, on nova.astrometry.net | On your computer, offline |
| What you need | A free API key (a password-like code for the service) | The ASTAP program **and** one of its star databases, both from [hnsky.org](https://www.hnsky.org/astap.htm) |
| Speed | Minutes per image, depends on the server | Usually faster |

Choose it in **File → Settings…** → **Connections** → "Backend:": "astrometry.net (online)" or
"ASTAP (local, offline)". For ASTAP, also fill "Executável ASTAP:" with the path to the program
(the "…" button opens a file browser).

🇧🇷 **Português**
Um plate solver descobre qual parte do céu uma imagem mostra. O AstroFind pode usar dois:

| | astrometry.net (padrão) | ASTAP |
|---|---|---|
| Onde roda | Online, no nova.astrometry.net | No seu computador, offline |
| O que precisa | Uma chave de API gratuita (um código tipo senha para o serviço) | O programa ASTAP **e** um dos bancos de estrelas dele, ambos em [hnsky.org](https://www.hnsky.org/astap.htm) |
| Velocidade | Minutos por imagem, depende do servidor | Normalmente mais rápido |

Escolha em **Arquivo → Configurações…** → **Conexões** → "Backend:": "astrometry.net (online)"
ou "ASTAP (local, offline)". Para o ASTAP, preencha também "Executável ASTAP:" com o caminho do
programa (o botão "…" abre um navegador de arquivos).

---

## How do I get an MPC observatory code? / Como consigo um código de observatório do MPC?

🟢 Beginner / Iniciante · 🔴 Advanced / Avançado

🇬🇧 **English**
An observatory (station) code is a 3-character name, such as `568` or `W49`, that the MPC gives
to one observing site. You need one for formal submissions. Getting one is free. The MPC asks
new observers to first send good measurements of already-known asteroids, taken on more than
one night, together with the site coordinates. The exact rules change over time, so follow the
current instructions on [minorplanetcenter.net](https://www.minorplanetcenter.net).

In AstroFind, type your code in **File → Settings…** → **Observer** → "MPC Station Code:" (or
in the Setup Wizard). While you have no code, leave the field empty: the report then uses
`XXX`, the MPC's placeholder for "no code yet".

> ⚠️ **Watch out:** AstroFind does not put your latitude/longitude into the report. If the MPC
> asks for your site coordinates with an `XXX` report, add them the way their instructions say.
> Do not type `500` as a placeholder: `500` is a real code that means "the centre of the Earth".

> 💡 **Tip:** if your observatory is in AstroFind's built-in list (84 observatories), choose
> "Observatório predefinido" in the Observer tab to fill in the coordinates.

🇧🇷 **Português**
Um código de observatório (de estação) é um nome de 3 caracteres, como `568` ou `W49`, que o
MPC dá a um local de observação. Você precisa dele para envios formais. Conseguir um é
gratuito. O MPC pede que novos observadores enviem antes boas medições de asteroides já
conhecidos, feitas em mais de uma noite, junto com as coordenadas do local. As regras exatas
mudam com o tempo, então siga as instruções atuais em
[minorplanetcenter.net](https://www.minorplanetcenter.net).

No AstroFind, digite o seu código em **Arquivo → Configurações…** → **Observador** → "Código de
estação MPC:" (ou no Assistente de Configuração). Enquanto não tiver código, deixe o campo
vazio: o relatório usa `XXX`, o marcador do MPC para "ainda sem código".

> ⚠️ **Atenção:** o AstroFind não coloca sua latitude/longitude no relatório. Se o MPC pedir as
> coordenadas do local junto com um relatório `XXX`, adicione-as do jeito que as instruções
> deles mandam. Não use `500` como marcador: `500` é um código real que significa "o centro da
> Terra".

> 💡 **Dica:** se o seu observatório está na lista embutida do AstroFind (84 observatórios),
> escolha "Observatório predefinido" na aba Observador para preencher as coordenadas.

---

## Why is my image black? / Por que minha imagem está preta?

🟢 Beginner / Iniciante

🇬🇧 **English**
This is normal and your data is fine. Astronomical images hold a much larger range of brightness
than a screen can show, so most of the picture looks dark. To see more:

1. Open **Images → Background and Range…**.
2. Click **"⟳ Auto"**.
3. If it is still dark, change "Função de transferência:" to "Raiz Quadrada (Sqrt)", "Asinh"
   or "Equalização de Histograma" (these labels are in Portuguese in both languages).
4. Tick "Apply to all images in session" to use the same view on every image, then click
   **OK**.

**What you should see:** a grey sky background with many white dots (stars).

This only changes the view, never the measured data. If the image is almost **white**, check
whether **Images → Invert Display** (`Ctrl+I`) is switched on. Pixels shown in **magenta** have
no value (NaN) in the file. More cases: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

🇧🇷 **Português**
Isso é normal e os seus dados estão bem. Imagens astronômicas guardam uma faixa de brilho muito
maior do que a tela consegue mostrar, então a maior parte parece escura. Para ver mais:

1. Abra **Imagens → Fundo e Intervalo…**.
2. Clique em **"⟳ Auto"**.
3. Se continuar escuro, mude "Função de transferência:" para "Raiz Quadrada (Sqrt)", "Asinh" ou
   "Equalização de Histograma".
4. Marque "Aplicar a todas as imagens da sessão" para usar a mesma visualização em todas, e
   clique em **OK**.

**O que você deve ver:** um fundo de céu cinza com muitos pontos brancos (estrelas).

Isso só muda a visualização, nunca os dados medidos. Se a imagem está quase **branca**, veja se
**Imagens → Inverter Exibição** (`Ctrl+I`) está ligada. Pixels em **magenta** não têm valor
(NaN) no arquivo. Mais casos: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

---

## My images don't have a plate solution — what do I do? / Minhas imagens não têm solução de placa — o que eu faço?

🟢 Beginner / Iniciante

🇬🇧 **English**
1. Choose a solver (see "Which plate solver should I use?" above). For astrometry.net, paste
   your API key in **File → Settings…** → **Connections** → "API Key:". If you skip this,
   AstroFind asks for the key when the reduction starts.
2. Set your location in **File → Settings…** → **Observer** (or use "Automático (do FITS)" when
   the files contain it). If the location is 0°, 0°, AstroFind warns "Localização não
   configurada".
3. Run **Astrometry → Data Reduction…** (`Ctrl+A`).

**What you should see:** in the Log panel, `Solved! RA=… Dec=… scale=…"/px` for each image, then
a summary line starting with "Redução concluída:".

🇧🇷 **Português**
1. Escolha um solver (veja "Qual plate solver devo usar?" acima). Para o astrometry.net, cole
   sua chave em **Arquivo → Configurações…** → **Conexões** → "Chave de API:". Se pular isso, o
   AstroFind pede a chave quando a redução começar.
2. Configure sua localização em **Arquivo → Configurações…** → **Observador** (ou use
   "Automático (do FITS)" quando os arquivos trazem essa informação). Se a localização for
   0°, 0°, o AstroFind avisa "Localização não configurada".
3. Rode **Astrometria → Redução de Dados…** (`Ctrl+A`).

**O que você deve ver:** no painel de Log, `Solved! RA=… Dec=… scale=…"/px` para cada imagem, e
depois uma linha de resumo que começa com "Redução concluída:".

---

## The plate solver keeps timing out or failing / O plate solver fica dando timeout ou falhando

🟡 Intermediate / Intermediário

🇬🇧 **English**
Common causes:

1. **API key** wrong or missing. Check it on nova.astrometry.net and paste it again in
   **Settings → Connections → API Key:**.
2. **Slow server.** Raise "Timeout:" in **Settings → Connections** (default 300 s, maximum
   600 s), or switch to ASTAP.
3. **Too few stars.** Look for the line `Image 1: … stars found` in the Log panel. If there are
   very few, see "No stars detected" in [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).
4. **ASTAP without a star database.** ASTAP needs one of its databases installed as well as the
   program.

Each failed image shows a line starting with `Plate solving falhou:` in the Log panel, followed
by the reason. [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) explains every reason.

🇧🇷 **Português**
Causas comuns:

1. **Chave de API** errada ou ausente. Confira no nova.astrometry.net e cole de novo em
   **Configurações → Conexões → Chave de API:**.
2. **Servidor lento.** Aumente "Tempo limite:" em **Configurações → Conexões** (padrão 300 s,
   máximo 600 s), ou mude para o ASTAP.
3. **Poucas estrelas.** Procure a linha `Image 1: … stars found` no painel de Log. Se forem
   muito poucas, veja "Nenhuma estrela detectada" em [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).
4. **ASTAP sem banco de estrelas.** O ASTAP precisa de um dos seus bancos instalado, além do
   programa.

Cada imagem que falha mostra no painel de Log uma linha que começa com `Plate solving falhou:`,
seguida do motivo. O [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) explica cada motivo.

---

## Why is the time flagged ambiguous? / Por que o horário aparece como ambíguo?

🟡 Intermediate / Intermediário

🇬🇧 **English**
Asteroids move, so the exact time of each image matters. The Log panel shows this warning
(in Portuguese in both languages):

`<file>: DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.`

It means: the date/time in the file (`DATE-OBS`) has no time zone and no `TIMESYS` keyword
saying it is UTC (Universal Time), so AstroFind **assumes UTC**. The same warning appears when
`TIMESYS` names another time scale, and **always** for DSLR RAW files (camera clock).

What to do:

1. If your camera or capture software already records UTC, nothing is wrong. You can ignore the
   warning.
2. If it records local time, fix the time before reporting. For each image, open
   **Images → Edit Image Parameters…** and correct "Julian Date:" (the Julian Date, a day count
   used by astronomers, of the middle of the exposure).
3. For the future, set the camera or software clock to UTC.

> ⚠️ **Watch out:** "Time Zone:" in **Settings → Observer** does **not** correct image times; it
> is stored but not used in the calculations. "Time Offset:" only accepts −999…999 seconds, so
> it cannot fix an error of whole hours. AstroFind may also fill "Time Offset:" automatically
> with longitude/15 (an hours value, although the field says seconds). Check that field before a
> reduction; see [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

🇧🇷 **Português**
Asteroides se movem, então o horário exato de cada imagem importa. O painel de Log mostra este
aviso:

`<arquivo>: DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.`

Quer dizer: a data/hora do arquivo (`DATE-OBS`) não tem fuso nem a palavra-chave `TIMESYS`
dizendo que é UTC (Tempo Universal), então o AstroFind **assume UTC**. O mesmo aviso aparece
quando o `TIMESYS` indica outra escala de tempo, e **sempre** para arquivos RAW de DSLR (relógio
da câmera).

O que fazer:

1. Se a sua câmera ou programa de captura já grava em UTC, não há erro. Pode ignorar o aviso.
2. Se grava horário local, corrija o horário antes de reportar. Para cada imagem, abra
   **Imagens → Editar Parâmetros da Imagem…** e corrija "Data Juliana:" (a Data Juliana, uma
   contagem de dias usada por astrônomos, do meio da exposição).
3. Daqui para a frente, acerte o relógio da câmera ou do programa em UTC.

> ⚠️ **Atenção:** "Fuso horário:" em **Configurações → Observador** **não** corrige o horário
> das imagens; ele é guardado mas não entra nos cálculos. "Deslocamento de tempo:" só aceita
> −999…999 segundos, então não resolve um erro de horas inteiras. O AstroFind também pode
> preencher "Deslocamento de tempo:" sozinho com longitude/15 (um valor em horas, embora o campo
> diga segundos). Confira esse campo antes de uma redução; veja [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

---

## Blink doesn't show anything moving — is that normal? / O blink não mostra nada se movendo — isso é normal?

🟢 Beginner / Iniciante

🇬🇧 **English**
Often, yes. Many fields have no asteroid bright enough to see. A real asteroid is a small dot
that **changes position** from one image to the next while the stars stay still. It does not
streak across the screen like a meteor. Tips:

- Use images of the same field taken over at least 30–60 minutes, so the motion is visible.
- Run **Tools → Known Object Overlay** (`Ctrl+K`) first: green circles mark known asteroids
  that should be in the field. Watch one of them in the blink as a check.
- Try **Astrometry → Moving Object Detection…** (`Ctrl+M`), which searches automatically and
  marks candidates as "Cand #N".
- In the blink view, the "Sharpen:" button can make faint dots easier to see.

🇧🇷 **Português**
Muitas vezes, sim. Vários campos não têm nenhum asteroide brilhante o bastante para ver. Um
asteroide real é um pontinho que **muda de posição** de uma imagem para a outra, enquanto as
estrelas ficam paradas. Ele não risca a tela como um meteoro. Dicas:

- Use imagens do mesmo campo tiradas ao longo de pelo menos 30–60 minutos, para o movimento
  aparecer.
- Rode antes **Ferramentas → Sobreposição de Objetos Conhecidos** (`Ctrl+K`): círculos verdes
  marcam asteroides conhecidos que deveriam estar no campo. Observe um deles no blink como
  teste.
- Experimente **Astrometria → Detecção de Objetos em Movimento…** (`Ctrl+M`), que procura
  sozinha e marca candidatos como "Cand #N".
- Na visão de piscar, o botão "Nitidez:" pode deixar pontos fracos mais fáceis de ver.

---

## How do I report an observation to the MPC? / Como eu reporto uma observação ao MPC?

🟡 Intermediate / Intermediário

🇬🇧 **English**
1. Fill in your details in **File → Settings…** (`Ctrl+,`) → **Observer**: "MPC Station Code:",
   "Observer:", "Telescope / Instrument:".
2. Measure the object in each image with the **Measure Object** toolbar button (`M`) or
   **Edit → Aperture Tool** (`A`), and click **"✓ Aceitar"** in the Verification window.
3. Open **File → View ADES Report File** (or step 6 of the Workflow panel). This item is enabled
   only after Data Reduction.
4. Check the XML and PSV tabs. Then use **"Submit to MPC (HTTP)"** or **"Submit to MPC
   (Email)"**, or save the file with **"Save…"**.

AstroFind writes only the **ADES** format (XML and PSV). Details: [Manual](https://github.com/petrinhu/astrofind/wiki/Manual).

🇧🇷 **Português**
1. Preencha seus dados em **Arquivo → Configurações…** (`Ctrl+,`) → **Observador**: "Código de
   estação MPC:", "Observador:", "Telescópio / Instrumento:".
2. Meça o objeto em cada imagem com o botão **Medir Objeto** da barra de ferramentas (`M`) ou
   **Editar → Ferramenta Abertura** (`A`), e clique em **"✓ Aceitar"** na janela de
   Verificação.
3. Abra **Arquivo → Ver Arquivo de Relatório ADES** (ou o passo 6 do painel de Fluxo). Esse item
   só fica ativo depois da Redução de Dados.
4. Confira as abas XML e PSV. Depois use **"Enviar ao MPC (HTTP)"** ou **"Enviar ao MPC
   (E-mail)"**, ou salve o arquivo com **"Salvar…"**.

O AstroFind só escreve o formato **ADES** (XML e PSV). Detalhes: [Manual](https://github.com/petrinhu/astrofind/wiki/Manual).

---

## What are the most common problems? / Quais são os problemas mais comuns?

🟢 Beginner / Iniciante

🇬🇧 **English**

| Symptom | Likely cause / fix |
|---|---|
| Image is all black or all white | Adjust the view in **Images → Background and Range…**, or turn off **Invert Display** (`Ctrl+I`) |
| Clicking the image measures nothing | The **Select** tool (`S`) is active. Press `M` or `A` first |
| Log says "Centroid failed — no source found at click position" | Click closer to the centre of the dot, or zoom in first |
| **View ADES Report File** is greyed out | Run **Astrometry → Data Reduction…** (`Ctrl+A`) first |
| "No observations yet." | Measure and accept at least one object first |
| Report shows station `XXX` | "MPC Station Code:" is empty in **Settings → Observer** |
| `Ctrl+L` does nothing | It is assigned twice (Load Images and Light Curve); use the menu or the toolbar |
| Verification window shows "Nenhum objeto conhecido próximo" | Normal when no known object is nearby |

Full list with the exact messages: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

🇧🇷 **Português**

| Sintoma | Causa provável / solução |
|---|---|
| Imagem toda preta ou toda branca | Ajuste a visualização em **Imagens → Fundo e Intervalo…**, ou desligue **Inverter Exibição** (`Ctrl+I`) |
| Clicar na imagem não mede nada | A ferramenta **Seleção** (`S`) está ativa. Aperte `M` ou `A` antes |
| O Log diz "Centroide falhou — nenhuma fonte encontrada na posição clicada" | Clique mais perto do centro do ponto, ou aproxime o zoom antes |
| **Ver Arquivo de Relatório ADES** está desativado | Rode **Astrometria → Redução de Dados…** (`Ctrl+A`) antes |
| "Nenhuma observação ainda." | Meça e aceite pelo menos um objeto antes |
| O relatório mostra a estação `XXX` | "Código de estação MPC:" está vazio em **Configurações → Observador** |
| `Ctrl+L` não faz nada | O atalho está em dois lugares (Carregar Imagens e Curva de Luz); use o menu ou a barra de ferramentas |
| A janela de Verificação mostra "Nenhum objeto conhecido próximo" | Normal quando não há objeto conhecido por perto |

Lista completa com as mensagens exatas: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

---

## How do I report a bug? / Como eu reporto um bug?

🟢 Beginner / Iniciante

🇬🇧 **English**
Open an issue on GitHub: <https://github.com/petrinhu/astrofind/issues> (you need a free GitHub
account). Please include:

1. The AstroFind version from **Help → About…** ("Version 1.1.0", for example).
2. Your Linux distribution and its version.
3. What you did, step by step, what you expected, and what happened.
4. The Log panel text: in the Log panel click **"Copy all"** and paste it.
5. If possible, a sample file that shows the problem (only if you are allowed to share it).

[Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) explains how to collect all of this.

🇧🇷 **Português**
Abra uma issue no GitHub: <https://github.com/petrinhu/astrofind/issues> (é preciso uma conta
gratuita no GitHub). Inclua, por favor:

1. A versão do AstroFind em **Ajuda → Sobre…** (por exemplo "Version 1.1.0").
2. Sua distribuição Linux e a versão dela.
3. O que você fez, passo a passo, o que esperava e o que aconteceu.
4. O texto do painel de Log: no painel de Log clique em **"Copiar tudo"** e cole.
5. Se possível, um arquivo de exemplo que mostre o problema (só se você puder compartilhá-lo).

O [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) explica como juntar tudo isso.

---

**See also / Veja também:** [Home](https://github.com/petrinhu/astrofind/wiki/Home) · [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) · [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) · [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) ·
[Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) · [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)
