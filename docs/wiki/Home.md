# AstroFind Wiki

🇬🇧 **Who this page is for:** everyone. It is the front door of the wiki, written for absolute
beginners in computing **and** in astronomy, with separate paths for experienced observers and
developers. No prior knowledge assumed.

🇧🇷 **Para quem é esta página:** para todo mundo. É a porta de entrada da wiki, escrita para
iniciantes absolutos em computação **e** em astronomia, com caminhos separados para
observadores experientes e desenvolvedores. Nenhum conhecimento prévio é assumido.

**Contents / Conteúdo:**
[What is AstroFind?](#what-is-astrofind--o-que-é-o-astrofind) ·
[Who is this for?](#who-is-this-for--para-quem-é-isso) ·
[Where do I start?](#where-do-i-start--por-onde-começo) ·
[Coming from Astrometrica](#coming-from-astrometrica--vindo-do-astrometrica) ·
[Map of the wiki](#map-of-the-wiki--mapa-da-wiki) ·
[What's new in 1.2.0](#whats-new-in-120--novidades-da-120) ·
[What's new in 1.1.0](#whats-new-in-110--novidades-da-110) ·
[Validation status](#validation-status--estado-da-validação)

---

## What is AstroFind? / O que é o AstroFind?

🟢 Beginner / Iniciante

🇬🇧 **English**

**AstroFind** is a free, open-source program (an "app", short for application: a piece of
software you install and run on your computer) for **Linux** that helps you find and measure
**asteroids** (rocky objects that orbit the Sun, most of them between Mars and Jupiter) by
comparing photographs of the night sky taken with a telescope.

It reads astronomy picture files, mainly **FITS** files (the standard file format for
astronomical images; see the [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)). Then it:

1. figures out exactly which patch of sky each picture shows (this is called **plate solving**);
2. helps you spot the one "star" that isn't really a star, because it moved between pictures
   (that's the asteroid);
3. measures its position (**astrometry**) and brightness (**photometry**) precisely;
4. produces a report in the **ADES** format (the modern standard for sending observations) that
   you can send to the **Minor Planet Center (MPC)**, the international organisation that
   collects and checks asteroid observations.

Think of it as: **camera → AstroFind → report for the MPC**.

AstroFind is inspired by [Astrometrica](http://www.astrometrica.at/), the classic Windows tool
used by amateur astronomers worldwide for this exact task (its author, Herbert Raab, is
credited in **Help → About AstroFind...**). AstroFind is a separate program written from scratch in
modern C++ with the Qt 6 toolkit, and runs on Linux only. Current version: **1.2.0**
(released 2026-09-25). License: **AGPL-3.0** (free software: you can read, change and share
the source code).

🇧🇷 **Português**

O **AstroFind** é um programa gratuito e de código aberto (um "app", abreviação de aplicativo:
um programa que você instala e executa no seu computador) para **Linux** que ajuda você a
encontrar e medir **asteroides** (objetos rochosos que orbitam o Sol, a maioria entre Marte e
Júpiter) comparando fotografias do céu noturno tiradas com um telescópio.

Ele lê arquivos de imagem astronômica, principalmente arquivos **FITS** (o formato padrão de
imagens astronômicas; veja o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary)). Depois ele:

1. descobre exatamente qual pedaço do céu cada foto mostra (isso se chama **plate solving**,
   ou "solução de campo");
2. ajuda você a achar aquela "estrela" que na verdade não é estrela, porque se moveu entre as
   fotos (esse é o asteroide);
3. mede com precisão a posição (**astrometria**) e o brilho (**fotometria**) dela;
4. gera um relatório no formato **ADES** (o padrão moderno para enviar observações) que você
   pode enviar ao **Minor Planet Center (MPC)**, a organização internacional que reúne e
   confere observações de asteroides.

Pense assim: **câmera → AstroFind → relatório para o MPC**.

O AstroFind é inspirado no [Astrometrica](http://www.astrometrica.at/), a ferramenta clássica
para Windows usada por astrônomos amadores do mundo todo para essa mesma tarefa (o autor,
Herbert Raab, aparece nos créditos em **Ajuda → Sobre o AstroFind...**). O AstroFind é um programa
separado, escrito do zero em C++ moderno com o kit Qt 6, e roda só em Linux. Versão atual:
**1.2.0** (lançada em 2026-09-25). Licença: **AGPL-3.0** (software livre: você pode ler,
alterar e compartilhar o código-fonte).

---

## Who is this for? / Para quem é isso?

🟢 Beginner / Iniciante

🇬🇧 **English**

- **Students and classrooms** doing asteroid-search projects (for example IASC, the
  International Astronomical Search Collaboration). AstroFind has a guided **Workflow** panel
  with six numbered steps, an optional "Fluxo automático" (automatic flow) checkbox, and
  "school" helpers: a teacher can export all settings to an `.ini` file
  (**File → Exportar Configuração da Escola...**) and students can send their report to the
  teacher by e-mail.
- **Amateur astronomers** with their own telescope and camera (an astronomy CCD/CMOS camera, or
  a DSLR since 1.1.0) who want to send real observations to the MPC.
- **Citizen scientists** without a telescope: you can download free FITS images from public
  sky surveys and search them from your own computer.
- **Curious beginners** who want to understand how asteroid astrometry works, from the file
  format to the final report.

You do **not** need to know how to program, and you do **not** need your own telescope.

🇧🇷 **Português**

- **Estudantes e turmas escolares** em projetos de busca de asteroides (por exemplo o IASC,
  International Astronomical Search Collaboration). O AstroFind tem um painel guiado
  **Fluxo de Trabalho** com seis passos numerados, uma caixa opcional "Fluxo automático", e
  recursos de "escola": o professor pode exportar todas as configurações para um arquivo `.ini`
  (**Arquivo → Exportar Configuração da Escola...**) e os alunos podem mandar o relatório para
  o professor por e-mail.
- **Astrônomos amadores** com telescópio e câmera próprios (câmera astronômica CCD/CMOS, ou
  uma DSLR a partir da 1.1.0) que querem enviar observações reais ao MPC.
- **Cientistas cidadãos** sem telescópio: dá para baixar imagens FITS gratuitas de
  levantamentos públicos do céu e analisá-las no seu computador.
- **Curiosos iniciantes** que querem entender como funciona a astrometria de asteroides, do
  formato de arquivo até o relatório final.

Você **não** precisa saber programar e **não** precisa ter telescópio próprio.

---

## Where do I start? / Por onde começo?

🇬🇧 **English** — pick the line that describes you best.

| Your level | Your path |
|---|---|
| 🟢 **Beginner.** I have never used this kind of app. | 1. [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) (install it step by step, including how to open a terminal) → 2. [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) (your first measurement) → 3. [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) (every feature, one at a time) |
| 🟡 **Intermediate.** I have used Astrometrica before. | 1. The [comparison table below](#coming-from-astrometrica--vindo-do-astrometrica) → 2. [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) (the full observing workflow) → 3. [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) (every menu item and shortcut) |
| 🔴 **Advanced / developer.** I want the maths, the internals, or to contribute. | 1. [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced) → 2. [`docs/technical-reference.md`](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md) (formulas: WCS, refraction, ICRS→CIRS, PSF, photometry) → 3. [`CONTRIBUTING.md`](https://github.com/petrinhu/astrofind/blob/main/CONTRIBUTING.md) |

Stuck at any point? Go to [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) or the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ). Unknown word? [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Português** — escolha a linha que mais combina com você.

| Seu nível | Seu caminho |
|---|---|
| 🟢 **Iniciante.** Nunca usei esse tipo de programa. | 1. [Instalação](https://github.com/petrinhu/astrofind/wiki/Installation) (instalar passo a passo, inclusive como abrir um terminal) → 2. [Início Rápido](https://github.com/petrinhu/astrofind/wiki/Quick-Start) (sua primeira medição) → 3. [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) (cada função, uma de cada vez) |
| 🟡 **Intermediário.** Já usei o Astrometrica. | 1. A [tabela de comparação abaixo](#coming-from-astrometrica--vindo-do-astrometrica) → 2. [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) (o fluxo completo de observação) → 3. [Referência de Menus](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) (cada item de menu e atalho) |
| 🔴 **Avançado / desenvolvedor.** Quero a matemática, o funcionamento interno ou contribuir. | 1. [Avançado](https://github.com/petrinhu/astrofind/wiki/Advanced) → 2. [`docs/technical-reference.md`](https://github.com/petrinhu/astrofind/blob/main/docs/technical-reference.md) (fórmulas: WCS, refração, ICRS→CIRS, PSF, fotometria) → 3. [`CONTRIBUTING.md`](https://github.com/petrinhu/astrofind/blob/main/CONTRIBUTING.md) |

Travou em algum ponto? Vá para [Solução de Problemas](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) ou o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ).
Palavra desconhecida? [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

---

## Coming from Astrometrica / Vindo do Astrometrica

🟡 Intermediate / Intermediário

🇬🇧 **English**

The overall routine is the one you know: load a series, reduce, overlay known objects, blink,
measure, report. The table maps each step to AstroFind's menus (English UI label, then the
Portuguese UI label). The last column says what is **different**.

| Step you know | In AstroFind (EN / PT) | What is different |
|---|---|---|
| Observer data, MPC code, camera | **File → Settings...** (`Ctrl+,`) / **Arquivo → Configurações...** — tabs Observer, Camera, Connections, Detection, Display, Legacy | The site can come from the FITS header, a built-in list of 84 observatories, manual coordinates, or "Telescópio Espacial". See [Settings](https://github.com/petrinhu/astrofind/wiki/Settings). |
| Load the image series | **File → Import Images...** (`Ctrl+L`) / **Arquivo → Importar Imagens...** | Also opens SER, XISF, TIFF/PNG, DSLR RAW, NASA PDS and archives (ZIP, TAR, 7Z, RAR). In 1.1.0 and earlier `Ctrl+L` was also given to **Utilities → Light Curve…** and could do nothing (use the menu or the toolbar there); version 1.2.0 moves Light Curve to `Ctrl+Shift+L`. |
| Dark / flat calibration | **File → Use Dark Frame for Calibration… / Use Flat Field for Calibration… / Calibration Wizard…** (PT **Arquivo → Usar Dark de Calibração / Usar Flat de Calibração / Assistente de Calibração…**) | Masters can be built in the Calibration dock ("Build Master…"). Calibration is applied inside Run Data Reduction. |
| Data Reduction | **Astrometry Tools → Run Data Reduction...** (`Ctrl+A`) / **Ferramentas de Astrometria → Executar Redução de Dados...** | The plate solution comes from **astrometry.net** (online, free API key) or **ASTAP** (offline). Images that already have a WCS (sky-coordinate solution) in the header are skipped. |
| Reference stars and known objects | **Utilities → Show Known Objects** (`Ctrl+K`) / **Utilitários → Mostrar Objetos Conhecidos** | Reference stars: UCAC4 or Gaia DR3 via VizieR (online) or a local FITS table. Known objects: IMCCE SkyBoT online, with MPCORB.DAT as the offline fallback. |
| Blink | **Utilities → Begin Blink Mode** (`Ctrl+B`) / **Utilitários → Ativar Modo de Piscagem**; stop with `Ctrl+F9` | The blink view shows up to 4 thumbnail slots; Space = play/pause, Left/Right = step. |
| Moving Object Detection | **Astrometry Tools → Detect Moving Objects...** (`Ctrl+M`) / **Ferramentas de Astrometria → Detectar Objetos em Movimento...** | Candidates appear as "Cand #N" markers. |
| Track & Stack | **Astrometry Tools → Stack Images...** (`Ctrl+T`) / **Ferramentas de Astrometria → Empilhar Imagens...** | You type dX/dY per frame by hand. |
| Measure an object | **Edit → Aperture Tool** (`A`) / **Editar → Ferramenta Abertura**, or the toolbar **Measure Object** button (`M`) | Elliptical PSF centroid, refraction correction when the position is not already from a catalog plate solution, differential photometry. A Verification window opens: click "✓ Aceitar". |
| MPC report | **File → View ADES Report File** / **Arquivo → Ver Arquivo de Relatório ADES** | **Only ADES 2022 (XML and PSV).** There is **no MPC 80-column (MPC1992) output.** Submission by HTTP to the MPC, by e-mail, or to a teacher. |
| MPCOrb | **Internet → Download MPCOrb Database** / **Update MPCOrb Database** (PT **Internet → Baixar Banco MPCOrb** / **Atualizar Banco MPCOrb**) | Used only as an offline fallback when SkyBoT can't be reached. |
| Local catalog folders (USNO-A2, UCAC-2/3, CMC-14) | **Settings → Legacy** / **Configurações → Legado** | Kept for compatibility only. **The pipeline never reads them.** Use VizieR or a local FITS BINTABLE catalog (Connections tab). |

> ⚠️ **Watch out:** some labels stay in Portuguese even when the interface is in English (for
> example "Fluxo automático", "✓ Aceitar"). This is how the app is today, not
> a problem on your side. [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) lists them.

🇧🇷 **Português**

A rotina é a que você já conhece: carregar a série, reduzir, sobrepor objetos conhecidos,
piscar, medir, relatar. A tabela liga cada passo aos menus do AstroFind (rótulo em português,
depois o rótulo em inglês). A última coluna diz o que é **diferente**.

| Passo que você conhece | No AstroFind (PT / EN) | O que muda |
|---|---|---|
| Dados do observador, código MPC, câmera | **Arquivo → Configurações...** (`Ctrl+,`) / **File → Settings...** — abas Observador, Câmera, Conexões, Detecção, Exibição, Legado | O local pode vir do cabeçalho FITS, de uma lista embutida de 84 observatórios, de coordenadas manuais ou de "Telescópio Espacial". Veja [Configurações](https://github.com/petrinhu/astrofind/wiki/Settings). |
| Carregar a série de imagens | **Arquivo → Importar Imagens...** (`Ctrl+L`) / **File → Import Images...** | Também abre SER, XISF, TIFF/PNG, RAW de DSLR, NASA PDS e arquivos compactados (ZIP, TAR, 7Z, RAR). Na 1.1.0 e anteriores o `Ctrl+L` também estava em **Utilitários → Curva de Luz…** e podia não fazer nada (lá, use o menu ou a barra de ferramentas); a versão 1.2.0 passa a Curva de Luz para `Ctrl+Shift+L`. |
| Calibração com dark / flat | **Arquivo → Usar Dark de Calibração...** / **Usar Flat de Calibração...** / **Assistente de Calibração…** | Dá para montar masters no painel Calibração ("Construir Master…"). A calibração é aplicada dentro da Execução da Redução de Dados. |
| Redução de Dados | **Ferramentas de Astrometria → Executar Redução de Dados...** (`Ctrl+A`) / **Astrometry Tools → Run Data Reduction...** | A solução de placa vem do **astrometry.net** (online, chave de API gratuita) ou do **ASTAP** (offline). Imagens que já têm WCS (solução de coordenadas do céu) no cabeçalho são puladas. |
| Estrelas de referência e objetos conhecidos | **Utilitários → Mostrar Objetos Conhecidos** (`Ctrl+K`) / **Utilities → Show Known Objects** | Estrelas de referência: UCAC4 ou Gaia DR3 via VizieR (online) ou tabela FITS local. Objetos conhecidos: IMCCE SkyBoT online, com o MPCORB.DAT como reserva offline. |
| Blink (piscar) | **Utilitários → Ativar Modo de Piscagem** (`Ctrl+B`) / **Utilities → Begin Blink Mode**; parar com `Ctrl+F9` | A visão de piscar mostra até 4 miniaturas; Espaço = tocar/pausar, Esquerda/Direita = avançar/voltar. |
| Detecção de objetos em movimento | **Ferramentas de Astrometria → Detectar Objetos em Movimento...** (`Ctrl+M`) | Os candidatos aparecem como marcadores "Cand #N". |
| Track & Stack | **Ferramentas de Astrometria → Empilhar Imagens...** (`Ctrl+T`) / **Astrometry Tools → Stack Images...** | Você digita dX/dY por quadro à mão. |
| Medir um objeto | **Editar → Ferramenta Abertura** (`A`) / **Edit → Aperture Tool**, ou o botão de medir da barra de ferramentas (tecla `M`) | Centroide por PSF elíptica, correção de refração quando a posição ainda não vem de uma solução de plate-solve por catálogo, fotometria diferencial. Abre a janela Verificação: clique em "✓ Aceitar". |
| Relatório para o MPC | **Arquivo → Ver Arquivo de Relatório ADES** / **File → View ADES Report File** | **Só ADES 2022 (XML e PSV).** **Não existe saída no formato MPC de 80 colunas (MPC1992).** Envio por HTTP ao MPC, por e-mail ou para o professor. |
| MPCOrb | **Internet → Baixar Banco MPCOrb** / **Atualizar Banco MPCOrb** | Usado só como reserva offline quando o SkyBoT não responde. |
| Pastas de catálogos locais (USNO-A2, UCAC-2/3, CMC-14) | **Configurações → Legado** / **Settings → Legacy** | Mantidas só por compatibilidade. **O processamento nunca as lê.** Use o VizieR ou um catálogo local FITS BINTABLE (aba Conexões). |

> ⚠️ **Atenção:** alguns rótulos ficam em português mesmo com a interface em inglês (por
> exemplo "Fluxo automático", "✓ Aceitar"). O programa é assim hoje; não é
> problema do seu lado. A [Referência de Menus](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) lista esses casos.

---

## Map of the wiki / Mapa da wiki

🇬🇧 **English**

| Page | Level | What you find there |
|---|---|---|
| [Home](https://github.com/petrinhu/astrofind/wiki/Home) | 🟢 | This page: what AstroFind is and where to start. |
| [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) | 🟢 | Step-by-step install for every supported Linux, first launch, uninstall, update, optional extras. |
| [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start) | 🟢 | From zero to your first measurement, in 10 steps. |
| [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) | 🟢🟡 | Every feature, one at a time: what it is, why it matters, how to use it. |
| [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) | 🟡 | The full observing workflow: calibration → reduction → measurement → submission. |
| [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats) | 🟡 | Which image files AstroFind opens (FITS, SER, XISF, RAW, PDS, archives) and what it saves. |
| [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) | 🟡 | Every menu item, toolbar button, dock and keyboard shortcut, in EN and PT. |
| [Settings](https://github.com/petrinhu/astrofind/wiki/Settings) | 🟡 | Every option in the Settings dialog: what it does and its default. |
| [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) | 🟢🟡 | Something went wrong: symptoms, causes, fixes. |
| [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ) | 🟢 | Short answers to common questions. |
| [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary) | 🟢 | Dictionary of the astronomy and computing words used in this wiki. |
| [Advanced](https://github.com/petrinhu/astrofind/wiki/Advanced) | 🔴 | How the calculations work, known limitations, and links for developers. |

🇧🇷 **Português**

| Página | Nível | O que você encontra |
|---|---|---|
| [Home](https://github.com/petrinhu/astrofind/wiki/Home) | 🟢 | Esta página: o que é o AstroFind e por onde começar. |
| [Instalação](https://github.com/petrinhu/astrofind/wiki/Installation) | 🟢 | Instalação passo a passo em cada Linux suportado, primeira execução, desinstalar, atualizar, extras opcionais. |
| [Início Rápido](https://github.com/petrinhu/astrofind/wiki/Quick-Start) | 🟢 | Do zero à primeira medição, em 10 passos. |
| [Tutorial](https://github.com/petrinhu/astrofind/wiki/Tutorial) | 🟢🟡 | Cada função, uma de cada vez: o que é, por que importa, como usar. |
| [Manual](https://github.com/petrinhu/astrofind/wiki/Manual) | 🟡 | O fluxo completo de observação: calibração → redução → medição → envio. |
| [Formatos de Arquivo](https://github.com/petrinhu/astrofind/wiki/File-Formats) | 🟡 | Quais arquivos de imagem o AstroFind abre (FITS, SER, XISF, RAW, PDS, compactados) e o que ele salva. |
| [Referência de Menus](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) | 🟡 | Cada item de menu, botão, painel e atalho de teclado, em EN e PT. |
| [Configurações](https://github.com/petrinhu/astrofind/wiki/Settings) | 🟡 | Cada opção da janela Configurações: o que faz e o valor padrão. |
| [Solução de Problemas](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) | 🟢🟡 | Algo deu errado: sintomas, causas, soluções. |
| [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ) | 🟢 | Respostas curtas para dúvidas comuns. |
| [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary) | 🟢 | Dicionário das palavras de astronomia e computação usadas na wiki. |
| [Avançado](https://github.com/petrinhu/astrofind/wiki/Advanced) | 🔴 | Como os cálculos funcionam, limitações conhecidas e links para desenvolvedores. |

---

## What's new in 1.2.0 / Novidades da 1.2.0

🇬🇧 **English**

Version 1.2.0 was released on 2026-09-25. It fixes the problems found in a full code audit of
1.1.0; most of them affect the numbers you send to the MPC, so updating is recommended.

- **More accurate positions.** Refraction is no longer added a second time to positions from a
  plate solution (the error was up to ~1.7′ at 30° altitude). Images whose WCS header has pole
  cards (LONPOLE/LATPOLE) are read correctly, and the catalog overlay is no longer misplaced on
  southern fields with the CAR/MER/GLS/AIT projections.
- **Better offline ephemeris.** Positions computed from MPCORB agree with JPL Horizons to under
  1′ (they could be off by 6′ to 49′).
- **Correct observation time.** Fractions of a second in the image time are kept. ΔT is no
  longer added to the report time, so `obsTime` is real UTC again. "Time Offset:" is no longer
  filled in automatically (an old value is reset to 0 once, with a note in the log).
- **Clearer Settings.** Pixel scale is in ″/px, "Minimum FWHM:" in ″ and "Time Precision:" in
  decimals of a second (the old labels said °/px, px and h).
- **Keyboard shortcuts work.** `Ctrl+L` is only Import Images (Light Curve is now
  `Ctrl+Shift+L`), and `Ctrl+Shift+T` toggles the theme.
- **Safer file loading.** Damaged or hostile files (FITS, SER, XISF, PNG/TIFF, TAR archives)
  are refused with a message instead of freezing or crashing the program.
- **AstroFind's own menu wording.** Menu texts that had been copied from Astrometrica were
  rewritten; shortcuts and behaviour are unchanged.
- **Ready-made packages** for Fedora, Rocky/RHEL 9, openSUSE, Ubuntu 24.04 (and Mint 22,
  Pop!_OS 24.04, Zorin OS 18), Debian 12 and 13, and Arch/Manjaro, plus an **AppImage** for
  other distributions (see [Installation](https://github.com/petrinhu/astrofind/wiki/Installation)).
  A list of all components (SBOM) is now generated automatically, and the packages include
  the full license texts.

Full details: [`CHANGELOG.md`](https://github.com/petrinhu/astrofind/blob/main/CHANGELOG.md).

🇧🇷 **Português**

A versão 1.2.0 saiu em 2026-09-25. Ela corrige os problemas achados numa auditoria completa do
código da 1.1.0; a maioria afeta os números que você envia ao MPC, por isso vale atualizar.

- **Posições mais precisas.** A refração não é mais somada uma segunda vez às posições vindas
  de uma solução de placa (o erro chegava a ~1,7′ a 30° de altura). Imagens cujo cabeçalho WCS
  tem cartões de polo (LONPOLE/LATPOLE) são lidas corretamente, e a sobreposição do catálogo
  não sai mais do lugar em campos do hemisfério sul com as projeções CAR/MER/GLS/AIT.
- **Efeméride offline melhor.** As posições calculadas a partir do MPCORB concordam com o JPL
  Horizons em menos de 1′ (podiam errar de 6′ a 49′).
- **Horário da observação correto.** As frações de segundo do horário da imagem são mantidas.
  O ΔT não é mais somado ao horário do relatório, então o `obsTime` volta a ser UTC de verdade.
  O "Deslocamento de tempo:" não é mais preenchido sozinho (um valor antigo é zerado uma vez,
  com um aviso no registro).
- **Configurações mais claras.** A escala de pixel está em ″/px, o "FWHM mínimo:" em ″ e a
  "Precisão de tempo:" em casas decimais do segundo (os rótulos antigos diziam °/px, px e h).
- **Atalhos de teclado funcionam.** O `Ctrl+L` é só Importar Imagens (a Curva de Luz agora é
  `Ctrl+Shift+L`), e o `Ctrl+Shift+T` alterna o tema.
- **Abertura de arquivos mais segura.** Arquivos danificados ou maliciosos (FITS, SER, XISF,
  PNG/TIFF, arquivos TAR) são recusados com uma mensagem, em vez de travar ou fechar o programa.
- **Textos de menu próprios do AstroFind.** Os textos de menu que tinham sido copiados do
  Astrometrica foram reescritos; atalhos e comportamento não mudaram.
- **Pacotes prontos** para Fedora, Rocky/RHEL 9, openSUSE, Ubuntu 24.04 (e Mint 22,
  Pop!_OS 24.04, Zorin OS 18), Debian 12 e 13, e Arch/Manjaro, mais um **AppImage** para as
  outras distribuições (veja [Instalação](https://github.com/petrinhu/astrofind/wiki/Installation)).
  Uma lista de todos os componentes (SBOM) agora é gerada automaticamente, e os pacotes
  incluem os textos completos das licenças.

Detalhes completos: [`CHANGELOG.md`](https://github.com/petrinhu/astrofind/blob/main/CHANGELOG.md).

---

## What's new in 1.1.0 / Novidades da 1.1.0

🇬🇧 **English**

Version 1.1.0 was tagged on 2026-09-24 but never published as a release; its changes ship in
1.2.0. The previous tag was 0.9.0, which was already as mature as a "1.0"; 1.1.0 adds new
features on top of it, so it is numbered 1.1.0.

- **DSLR RAW images** (the raw files of digital cameras: CR2, CR3, CRW, NEF, NRW, ARW, SRF, SR2,
  ORF, RW2, RAF, PEF, DNG, SRW, 3FR, ERF, KDC, MRW, X3F, IIQ, MEF, MOS, RWL), read through the
  LibRaw library.
  - The data stay linear (no white balance or gamma), which is what measurements need.
  - Star detection uses a 2×2 "superpixel" brightness, (R+2G+B)/4; a simple colour version is
    used only for display.
  - Exposure time, camera model and time come from the EXIF data (the camera's own tags).
  - ⚠️ The time is the **camera clock**, which is often local time and not UTC, so AstroFind
    flags it as ambiguous in the log. Check your camera clock before trusting the time.
  - The official packages include LibRaw. A copy built without LibRaw refuses RAW files with a
    clear message.
- **NASA PDS images** (the archive format of NASA planetary missions): PDS3 (`.img` with the
  label inside, or `.img` + a separate `.lbl`) and PDS4 (`.xml` label; if the label points to a
  FITS file, AstroFind opens that FITS).
  - Mission images rarely come with a plate solution: expect AstroFind to plate-solve them in
    Run Data Reduction.
- Internal robustness fixes from the code audit.

Full details: [`CHANGELOG.md`](https://github.com/petrinhu/astrofind/blob/main/CHANGELOG.md).
File details: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

🇧🇷 **Português**

A versão 1.1.0 recebeu a tag em 2026-09-24, mas nunca foi publicada como release; as mudanças
dela saem na 1.2.0. A tag anterior era a 0.9.0, que já tinha maturidade de "1.0"; a 1.1.0
acrescenta funcionalidades sobre ela, por isso recebeu o número 1.1.0.

- **Imagens RAW de DSLR** (os arquivos brutos de câmeras digitais: CR2, CR3, CRW, NEF, NRW, ARW,
  SRF, SR2, ORF, RW2, RAF, PEF, DNG, SRW, 3FR, ERF, KDC, MRW, X3F, IIQ, MEF, MOS, RWL), lidas pela
  biblioteca LibRaw.
  - Os dados ficam lineares (sem balanço de branco nem gama), que é o que as medições precisam.
  - A detecção de estrelas usa um brilho por "superpixel" 2×2, (R+2G+B)/4; uma versão colorida
    simples serve só para exibição.
  - Tempo de exposição, modelo da câmera e horário vêm dos dados EXIF (as etiquetas da própria
    câmera).
  - ⚠️ O horário é o **relógio da câmera**, que muitas vezes está em hora local e não em UTC;
    por isso o AstroFind marca o horário como ambíguo no registro. Confira o relógio da câmera
    antes de confiar no horário.
  - Os pacotes oficiais já incluem a LibRaw. Uma cópia compilada sem LibRaw recusa arquivos RAW
    com uma mensagem clara.
- **Imagens NASA PDS** (o formato de arquivo das missões planetárias da NASA): PDS3 (`.img` com
  o rótulo dentro, ou `.img` + um `.lbl` separado) e PDS4 (rótulo `.xml`; se o rótulo aponta
  para um arquivo FITS, o AstroFind abre esse FITS).
  - Imagens de missões raramente vêm com solução de placa: o AstroFind vai resolvê-las na
    Redução de Dados.
- Correções internas de robustez vindas da auditoria de código.

Detalhes completos: [`CHANGELOG.md`](https://github.com/petrinhu/astrofind/blob/main/CHANGELOG.md).
Detalhes dos arquivos: [Formatos de Arquivo](https://github.com/petrinhu/astrofind/wiki/File-Formats).

---

## Validation status / Estado da validação

🇬🇧 **English**

AstroFind builds and passes its automated tests on **Ubuntu 24.04, Debian 12, Fedora 44, Arch
Linux, CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9 and Linux Mint 22**. Pop!_OS and
Zorin OS are covered through Ubuntu 24.04 containers (there is no dedicated Pop!_OS or Zorin
test image). It runs on **Linux only**.

> ⚠️ **Honest note.** AstroFind has **not yet been validated end to end with real observatory
> data** (a full night of real telescope images compared against known-good results). The
> automated tests check each part of the program, not a real observing campaign. **Always
> double-check your positions and magnitudes before you submit anything to the MPC**, for
> example against JPL Horizons (**Internet → Query Horizons…**) for a known asteroid.

🇧🇷 **Português**

O AstroFind compila e passa nos testes automáticos em **Ubuntu 24.04, Debian 12, Fedora 44,
Arch Linux, CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9 e Linux Mint 22**. Pop!_OS e
Zorin OS são cobertos por containers do Ubuntu 24.04 (não há imagem de teste própria do Pop!_OS
nem do Zorin). Roda **só em Linux**.

> ⚠️ **Nota honesta.** O AstroFind **ainda não foi validado de ponta a ponta com dados reais de
> observatório** (uma noite inteira de imagens reais de telescópio comparada com resultados
> confiáveis). Os testes automáticos conferem cada parte do programa, não uma campanha real de
> observação. **Sempre confira suas posições e magnitudes antes de enviar qualquer coisa ao
> MPC**, por exemplo comparando com o JPL Horizons (**Internet → Consultar Horizons…**) para
> um asteroide conhecido.

---

## A note on language / Uma nota sobre idioma

🇬🇧 **English**
AstroFind's interface is available in English and Brazilian Portuguese (**File → Settings... →
Display → Language**; a restart is needed). This wiki follows the same rule: every section has
an 🇬🇧 English block and a 🇧🇷 Portuguese block with the same content.

🇧🇷 **Português**
A interface do AstroFind existe em inglês e português do Brasil (**Arquivo → Configurações... →
Exibição → Idioma**; é preciso reiniciar). Esta wiki segue a mesma regra: cada seção tem um
bloco 🇬🇧 em inglês e um bloco 🇧🇷 em português com o mesmo conteúdo.
