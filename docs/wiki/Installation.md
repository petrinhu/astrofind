# Installation / Instalação

🇬🇧 **Who this page is for:** anyone who wants to put AstroFind 1.2.0 on a Linux computer,
including people who have never opened a terminal. Every command is explained line by line.
If you already know your way around Linux, jump to [your distribution](#4-install-for-your-distribution--instalar-na-sua-distribuição).
The complete per-distribution dependency lists live in
[`INSTALL.md` on GitHub](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md), the single
source of truth for the commands on this page.

🇧🇷 **Para quem é esta página:** qualquer pessoa que queira colocar o AstroFind 1.2.0 num
computador com Linux, inclusive quem nunca abriu um terminal. Cada comando é explicado linha a
linha. Se você já conhece Linux, pule para [a sua distribuição](#4-install-for-your-distribution--instalar-na-sua-distribuição).
As listas completas de dependências por distribuição estão no
[`INSTALL.md` no GitHub](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md), a fonte
oficial dos comandos desta página.

**Contents / Conteúdo**

1. [Before you start / Antes de começar](#1-before-you-start--antes-de-começar)
2. [The terminal / O terminal](#2-the-terminal--o-terminal)
3. [Which Linux do I have? / Qual Linux eu tenho?](#3-which-linux-do-i-have--qual-linux-eu-tenho)
4. [Install for your distribution / Instalar na sua distribuição](#4-install-for-your-distribution--instalar-na-sua-distribuição)
   - [4.1 Universal installer / Instalador universal](#41-universal-installer-easiest--instalador-universal-mais-fácil)
   - [4.2 Fedora / Rocky Linux / AlmaLinux / RHEL 9 (RPM)](#42-fedora--rocky-linux--almalinux--rhel-9-rpm)
   - [4.3 Ubuntu / Debian / Linux Mint / Pop!_OS / Zorin OS (DEB)](#43-ubuntu--debian--linux-mint--pop_os--zorin-os-deb)
   - [4.4 Arch Linux / Manjaro / CachyOS / EndeavourOS (pacman)](#44-arch-linux--manjaro--cachyos--endeavouros-pacman)
   - [4.5 openSUSE Tumbleweed (RPM)](#45-opensuse-tumbleweed-rpm)
   - [4.6 Any other distro: AppImage / Qualquer outra distro: AppImage](#46-any-other-distro-appimage--qualquer-outra-distro-appimage)
   - [4.7 Build from source / Compilar do código-fonte](#47-build-from-source--compilar-do-código-fonte)
   - [4.8 Checking the download / Conferir o download](#48-checking-the-download--conferir-o-download)
5. [First launch and the Setup Wizard / Primeira execução e o Assistente](#5-first-launch-and-the-setup-wizard--primeira-execução-e-o-assistente-de-configuração)
6. [Where AstroFind keeps its files / Onde o AstroFind guarda seus arquivos](#6-where-astrofind-keeps-its-files--onde-o-astrofind-guarda-seus-arquivos)
7. [Optional features / Recursos opcionais](#7-optional-features--recursos-opcionais)
8. [Updating / Atualizar](#8-updating--atualizar)
9. [Uninstalling / Desinstalar](#9-uninstalling--desinstalar)
10. [It didn't work / Não funcionou](#10-it-didnt-work--não-funcionou)

---

## 1. Before you start / Antes de começar

🟢 Beginner / Iniciante

🇬🇧 **English**

You need:

- A computer running **Linux** (AstroFind does not run on Windows or macOS) with a 64-bit Intel
  or AMD processor (the packages are built for `x86_64`, also called `amd64`).
- An **internet connection** (to download AstroFind, and later for plate solving and star
  catalogs).
- Your **user password**. Installing software changes system files, so Linux asks for your
  password to confirm it is really you. Your user must be allowed to use `sudo` ("super-user
  do": run one command with administrator rights). On a personal computer the first user
  created is normally allowed.

AstroFind 1.2.0 has been built and tested on Ubuntu 24.04, Debian 12, Fedora 44, Arch Linux,
CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9 and Linux Mint 22. Pop!_OS and Zorin OS are
covered through Ubuntu 24.04 test containers. Distros on an Ubuntu 22.04 base (Pop!_OS 22.04,
Zorin OS 17, Linux Mint 21) are **not supported**: their Qt 6.2 is too old (AstroFind needs
Qt 6.4) and so is their glibc 2.35 (the AppImage needs 2.36).

🇧🇷 **Português**

Você precisa de:

- Um computador com **Linux** (o AstroFind não roda em Windows nem macOS) com processador Intel
  ou AMD de 64 bits (os pacotes são feitos para `x86_64`, também chamado `amd64`).
- **Conexão com a internet** (para baixar o AstroFind e, depois, para a solução de campo e os
  catálogos de estrelas).
- A **sua senha de usuário**. Instalar programas mexe em arquivos do sistema, então o Linux pede
  a sua senha para confirmar que é você mesmo. Seu usuário precisa poder usar o `sudo` ("super-user
  do": executar um comando com direitos de administrador). Num computador pessoal, o primeiro
  usuário criado normalmente pode.

O AstroFind 1.2.0 foi compilado e testado em Ubuntu 24.04, Debian 12, Fedora 44, Arch Linux,
CachyOS, Manjaro, openSUSE Tumbleweed, Rocky Linux 9 e Linux Mint 22. Pop!_OS e Zorin OS são
cobertos por containers de teste do Ubuntu 24.04. Distros com base Ubuntu 22.04 (Pop!_OS 22.04,
Zorin OS 17, Linux Mint 21) **não são suportadas**: o Qt 6.2 delas é antigo demais (o AstroFind
precisa do Qt 6.4), e a glibc 2.35 também (o AppImage precisa da 2.36).

---

## 2. The terminal / O terminal

🟢 Beginner / Iniciante

🇬🇧 **English**

A **terminal** is a window where you type commands instead of clicking. It looks old-fashioned,
but it is the most reliable way to install software on Linux, because the exact same text works
for everybody. You will only copy and paste a few lines.

**How to open it:**

| Your desktop (what your screen looks like) | How to open a terminal |
|---|---|
| Ubuntu, Pop!_OS, Zorin OS, Fedora Workstation (GNOME desktop) | Press the **Super** key (the one with the Windows logo), type `terminal`, press Enter. On Ubuntu, Pop!_OS and Zorin, `Ctrl+Alt+T` also works. |
| Linux Mint (Cinnamon desktop) | Press `Ctrl+Alt+T`, or open the **Menu** (bottom-left) and click **Terminal**. |
| KDE Plasma (default on Manjaro KDE, openSUSE, CachyOS, Fedora KDE) | Open the application launcher (bottom-left), type `konsole`, press Enter. |
| Xfce (Xubuntu, Manjaro Xfce, Mint Xfce) | Open the **Applications** menu and click **Terminal Emulator**. |

**How to use it:**

- **Paste** into a terminal with `Ctrl+Shift+V` (plain `Ctrl+V` usually does not work there),
  or right-click → **Paste**.
- Press **Enter** to run the line you pasted. Run the lines **one at a time**, in order, and
  wait until each one finishes (you see the prompt, the line ending in `$`, again).
- When a command starts with `sudo`, it asks for **your password**. ⚠️ **While you type the
  password nothing appears on screen, not even dots.** That is normal. Type it and press Enter.
- Messages in English scrolling past are normal. Read the **last lines**: they tell you whether
  it worked.

> 💡 **Tip:** in this wiki, grey boxes like the one below are commands. Do **not** type the `$`
> you may see on your own screen; it is just the prompt.

🇧🇷 **Português**

O **terminal** é uma janela onde você digita comandos em vez de clicar. Parece antiquado, mas é
o jeito mais confiável de instalar programas no Linux, porque exatamente o mesmo texto funciona
para todo mundo. Você só vai copiar e colar algumas linhas.

**Como abrir:**

| Sua área de trabalho (a cara da sua tela) | Como abrir um terminal |
|---|---|
| Ubuntu, Pop!_OS, Zorin OS, Fedora Workstation (área de trabalho GNOME) | Aperte a tecla **Super** (a do logotipo do Windows), digite `terminal` e aperte Enter. No Ubuntu, Pop!_OS e Zorin, `Ctrl+Alt+T` também funciona. |
| Linux Mint (área de trabalho Cinnamon) | Aperte `Ctrl+Alt+T`, ou abra o **Menu** (canto inferior esquerdo) e clique em **Terminal**. |
| KDE Plasma (padrão no Manjaro KDE, openSUSE, CachyOS, Fedora KDE) | Abra o lançador de aplicativos (canto inferior esquerdo), digite `konsole` e aperte Enter. |
| Xfce (Xubuntu, Manjaro Xfce, Mint Xfce) | Abra o menu **Aplicativos** e clique em **Emulador de Terminal**. |

**Como usar:**

- Para **colar** no terminal use `Ctrl+Shift+V` (o `Ctrl+V` comum normalmente não funciona
  ali), ou clique com o botão direito → **Colar**.
- Aperte **Enter** para executar a linha colada. Execute as linhas **uma de cada vez**, na
  ordem, e espere cada uma terminar (o prompt, a linha que termina em `$`, aparece de novo).
- Quando um comando começa com `sudo`, ele pede **a sua senha**. ⚠️ **Enquanto você digita a
  senha, nada aparece na tela, nem pontinhos.** Isso é normal. Digite e aperte Enter.
- Mensagens em inglês rolando na tela são normais. Leia as **últimas linhas**: elas dizem se deu
  certo.

> 💡 **Dica:** nesta wiki, as caixas cinzas como a de baixo são comandos. **Não** digite o `$`
> que talvez apareça na sua tela; ele é só o prompt.

---

## 3. Which Linux do I have? / Qual Linux eu tenho?

🟢 Beginner / Iniciante

🇬🇧 **English**

"Linux" is a family. A **distribution** ("distro") is one complete flavour of it: Ubuntu,
Fedora, Arch and so on. Each family installs software in its own way (its own **package
manager**, the program that installs and removes software, and its own **package** format, the
file that contains a ready-to-install program). So first find out which one you have.

Open a terminal and run:

```bash
cat /etc/os-release
```

(`cat` prints a text file on screen; `/etc/os-release` is the small file where every Linux
writes its own name.)

**What you should see** (example on Linux Mint):

```text
NAME="Linux Mint"
VERSION="22 (Wilma)"
ID=linuxmint
ID_LIKE="ubuntu debian"
...
```

Look at `NAME`, `VERSION`, `ID` and `ID_LIKE` ("which family this distro belongs to"; the head of
a family, such as Fedora, Debian or Arch, has only `ID`). Then use this table:

| If you see… | Your family | Go to |
|---|---|---|
| Fedora 44, Rocky Linux / AlmaLinux / RHEL 9 (`ID` is `fedora`, or `ID_LIKE` contains `rhel`) | RPM with `dnf` | [4.2](#42-fedora--rocky-linux--almalinux--rhel-9-rpm) |
| Ubuntu 24.04, Debian 13, Linux Mint 22, Pop!_OS 24.04, Zorin OS 18 (`ID` or `ID_LIKE` contains `ubuntu` or `debian`) | DEB with `apt` | [4.3](#43-ubuntu--debian--linux-mint--pop_os--zorin-os-deb) |
| Arch Linux, Manjaro, CachyOS, EndeavourOS (`ID` or `ID_LIKE` contains `arch`) | package for `pacman` | [4.4](#44-arch-linux--manjaro--cachyos--endeavouros-pacman) |
| openSUSE Tumbleweed (`ID_LIKE` contains `suse`) | RPM with `zypper` | [4.5](#45-opensuse-tumbleweed-rpm) |
| Debian 12, or any other 64-bit (x86-64) distro | AppImage (no installation) | [4.6](#46-any-other-distro-appimage--qualquer-outra-distro-appimage) |
| Not sure | let the installer decide | [4.1](#41-universal-installer-easiest--instalador-universal-mais-fácil) |

🇧🇷 **Português**

"Linux" é uma família. Uma **distribuição** ("distro") é uma versão completa dela: Ubuntu,
Fedora, Arch e assim por diante. Cada família instala programas do seu jeito (com o seu
**gerenciador de pacotes**, o programa que instala e remove programas, e o seu formato de
**pacote**, o arquivo que contém um programa pronto para instalar). Então primeiro descubra qual
é a sua.

Abra um terminal e execute:

```bash
cat /etc/os-release
```

(`cat` mostra um arquivo de texto na tela; `/etc/os-release` é o arquivinho onde todo Linux
escreve o próprio nome.)

**O que você deve ver** (exemplo no Linux Mint):

```text
NAME="Linux Mint"
VERSION="22 (Wilma)"
ID=linuxmint
ID_LIKE="ubuntu debian"
...
```

Olhe `NAME`, `VERSION`, `ID` e `ID_LIKE` ("a que família esta distro pertence"; a distro que
encabeça uma família, como Fedora, Debian ou Arch, só tem `ID`). Depois use a tabela:

| Se aparecer… | Sua família | Vá para |
|---|---|---|
| Fedora 44, Rocky Linux / AlmaLinux / RHEL 9 (`ID` é `fedora`, ou `ID_LIKE` contém `rhel`) | RPM com `dnf` | [4.2](#42-fedora--rocky-linux--almalinux--rhel-9-rpm) |
| Ubuntu 24.04, Debian 13, Linux Mint 22, Pop!_OS 24.04, Zorin OS 18 (`ID` ou `ID_LIKE` contém `ubuntu` ou `debian`) | DEB com `apt` | [4.3](#43-ubuntu--debian--linux-mint--pop_os--zorin-os-deb) |
| Arch Linux, Manjaro, CachyOS, EndeavourOS (`ID` ou `ID_LIKE` contém `arch`) | pacote para o `pacman` | [4.4](#44-arch-linux--manjaro--cachyos--endeavouros-pacman) |
| openSUSE Tumbleweed (`ID_LIKE` contém `suse`) | RPM com `zypper` | [4.5](#45-opensuse-tumbleweed-rpm) |
| Debian 12, ou qualquer outra distro de 64 bits (x86-64) | AppImage (sem instalação) | [4.6](#46-any-other-distro-appimage--qualquer-outra-distro-appimage) |
| Não sei | deixe o instalador decidir | [4.1](#41-universal-installer-easiest--instalador-universal-mais-fácil) |

---

## 4. Install for your distribution / Instalar na sua distribuição

### 4.1 Universal installer (easiest) / Instalador universal (mais fácil)

🟢 Beginner / Iniciante

🇬🇧 **English**

The universal installer is a small script (a text file with commands) that detects your distro,
downloads the right package, installs it, and offers a menu entry and a desktop icon. It talks
to you in English or Portuguese.

```bash
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.2.0/packaging/install.sh
chmod +x install.sh
./install.sh
```

Line by line:

1. `curl -LO …` downloads the installer (`curl` is a download tool; `-L` follows redirects, `-O`
   saves the file under its own name, `install.sh`, in the current folder).
2. `chmod +x install.sh` marks the file as "executable" (allowed to run as a program).
3. `./install.sh` runs it (`./` means "the file in this folder").

**What happens next** (the installer shows 7 numbered stages):

1. It asks for the language (1 = English, 2 = Português (Brasil)).
2. **1/7** shows the detected distribution, the package type it will use and the package
   manager. On Fedora, Rocky/AlmaLinux/RHEL 9, openSUSE Tumbleweed, Ubuntu 24.04 and its
   derivatives, Debian 13 and the Arch family it picks the matching package. On any other x86-64
   distro (Debian 12 included) it uses the AppImage if the system glibc is 2.36 or newer, and
   stops with an "unsupported" message if it is older. On other CPUs (not x86-64) it builds
   AstroFind from source.
3. **2/7** checks that you can use `sudo` (it may ask your password; remember, nothing appears
   while you type).
4. **3/7** asks you to confirm. Answer `y` (English) or `s` (Portuguese) and press Enter.
5. **4/7** downloads and installs AstroFind. **5/7** checks that it worked.
6. **6/7** offers two optional libraries: **libarchive** (open TAR/7Z/RAR archives) and
   **qtkeychain** (store your astrometry.net key securely). It also shows where to get **ASTAP**
   (offline plate solver), but does not install it. See [section 7](#7-optional-features--recursos-opcionais).
7. **7/7** offers an **application menu entry** (recommended) and a **desktop shortcut**.
8. A summary table lists what was installed, skipped or failed.

> ⚠️ **Watch out:** run it as your normal user, **not** as `root` and not with `sudo ./install.sh`.
> The script calls `sudo` itself when needed.

Other releases: replace `v1.2.0` in the address with the tag you want; the
[releases page](https://github.com/petrinhu/astrofind/releases) lists them all.

🇧🇷 **Português**

O instalador universal é um pequeno script (um arquivo de texto com comandos) que detecta sua
distro, baixa o pacote certo, instala e oferece uma entrada no menu e um ícone na área de
trabalho. Ele conversa com você em inglês ou português.

```bash
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.2.0/packaging/install.sh
chmod +x install.sh
./install.sh
```

Linha por linha:

1. `curl -LO …` baixa o instalador (`curl` é uma ferramenta de download; `-L` segue
   redirecionamentos, `-O` salva o arquivo com o próprio nome, `install.sh`, na pasta atual).
2. `chmod +x install.sh` marca o arquivo como "executável" (pode rodar como programa).
3. `./install.sh` executa o script (`./` quer dizer "o arquivo desta pasta").

**O que acontece em seguida** (o instalador mostra 7 etapas numeradas):

1. Ele pergunta o idioma (1 = English, 2 = Português (Brasil)).
2. **1/7** mostra a distribuição detectada, o tipo de pacote que vai usar e o gerenciador de
   pacotes. No Fedora, Rocky/AlmaLinux/RHEL 9, openSUSE Tumbleweed, Ubuntu 24.04 e derivadas,
   Debian 13 e família Arch ele escolhe o pacote certo. Em qualquer outra distro x86-64 (inclusive
   o Debian 12) usa o AppImage se a glibc do sistema for 2.36 ou mais nova, e para com uma
   mensagem de "não suportada" se for mais antiga. Em outros processadores (não x86-64) ele
   compila o AstroFind a partir do código-fonte.
3. **2/7** confere se você pode usar o `sudo` (pode pedir sua senha; lembre: nada aparece
   enquanto você digita).
4. **3/7** pede confirmação. Responda `s` (português) ou `y` (inglês) e aperte Enter.
5. **4/7** baixa e instala o AstroFind. **5/7** confere se deu certo.
6. **6/7** oferece duas bibliotecas opcionais: **libarchive** (abrir arquivos TAR/7Z/RAR) e
   **qtkeychain** (guardar a chave do astrometry.net de forma segura). Também mostra onde obter
   o **ASTAP** (solucionador de campo offline), mas não o instala. Veja a [seção 7](#7-optional-features--recursos-opcionais).
7. **7/7** oferece uma **entrada no menu de aplicativos** (recomendado) e um **atalho na área de
   trabalho**.
8. Uma tabela de resumo lista o que foi instalado, pulado ou falhou.

> ⚠️ **Atenção:** rode como seu usuário normal, **não** como `root` e nem com
> `sudo ./install.sh`. O script chama o `sudo` sozinho quando precisa.

Outras versões: troque `v1.2.0` no endereço pela tag desejada; a
[página de releases](https://github.com/petrinhu/astrofind/releases) lista todas.

---

### 4.2 Fedora / Rocky Linux / AlmaLinux / RHEL 9 (RPM)

🟢 Beginner / Iniciante

🇬🇧 **English**

An **RPM** is the package format of the Fedora/Red Hat family; `dnf` is their package manager.
There is one file per system: `.fc44` for Fedora 44 and `.el9` for Rocky Linux, AlmaLinux and
RHEL 9.

**Fedora 44:**

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1.fc44.x86_64.rpm
sudo dnf install ./astrofind-1.2.0-1.fc44.x86_64.rpm
```

1. Downloads the AstroFind 1.2.0 package into the current folder.
2. Installs it. `./` tells `dnf` to use the file you just downloaded; `dnf` also fetches the
   libraries it needs. Answer `y` when asked "Is this ok".

**Rocky Linux / AlmaLinux / RHEL 9.** These systems ship Qt 5 by default. Qt 6 comes from
**EPEL** (Extra Packages for Enterprise Linux, a community repository) and needs **CRB**
(CodeReady Builder) enabled. Run the first line **before** installing the package:

```bash
sudo dnf install epel-release && sudo dnf config-manager --set-enabled crb
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1.el9.x86_64.rpm
sudo dnf install ./astrofind-1.2.0-1.el9.x86_64.rpm
```

1. Adds the EPEL repository, then turns on the CRB repository (`&&` runs the second command
   only if the first one worked).
2. Downloads the AstroFind 1.2.0 package for version 9 of these systems.
3. Installs it, with the libraries it needs.

**What you should see:** `Complete!` at the end. Then look for **AstroFind** in your
applications menu.

🇧🇷 **Português**

**RPM** é o formato de pacote da família Fedora/Red Hat; o `dnf` é o gerenciador de pacotes
dela. Há um arquivo para cada sistema: `.fc44` para o Fedora 44 e `.el9` para Rocky Linux,
AlmaLinux e RHEL 9.

**Fedora 44:**

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1.fc44.x86_64.rpm
sudo dnf install ./astrofind-1.2.0-1.fc44.x86_64.rpm
```

1. Baixa o pacote do AstroFind 1.2.0 para a pasta atual.
2. Instala. O `./` diz ao `dnf` para usar o arquivo que você acabou de baixar; o `dnf` também
   busca as bibliotecas necessárias. Responda `s` (ou `y`) quando ele perguntar se está ok.

**Rocky Linux / AlmaLinux / RHEL 9.** Esses sistemas vêm com Qt 5. O Qt 6 vem do **EPEL**
(Extra Packages for Enterprise Linux, um repositório comunitário) e precisa do **CRB**
(CodeReady Builder) ligado. Rode a primeira linha **antes** de instalar o pacote:

```bash
sudo dnf install epel-release && sudo dnf config-manager --set-enabled crb
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1.el9.x86_64.rpm
sudo dnf install ./astrofind-1.2.0-1.el9.x86_64.rpm
```

1. Adiciona o repositório EPEL e depois liga o repositório CRB (o `&&` só roda o segundo
   comando se o primeiro deu certo).
2. Baixa o pacote do AstroFind 1.2.0 para a versão 9 desses sistemas.
3. Instala, junto com as bibliotecas necessárias.

**O que você deve ver:** `Complete!` (ou `Concluído!`) no final. Depois procure **AstroFind**
no menu de aplicativos.

---

### 4.3 Ubuntu / Debian / Linux Mint / Pop!_OS / Zorin OS (DEB)

🟢 Beginner / Iniciante

🇬🇧 **English**

A **DEB** is the package format of the Debian/Ubuntu family; `apt` is their package manager.
Pick the file for your system:

| Your system | File |
|---|---|
| Ubuntu 24.04, Linux Mint 22, Pop!_OS 24.04, Zorin OS 18 | `astrofind_1.2.0-1~ubuntu24.04_amd64.deb` |
| Debian 13 Trixie | `astrofind_1.2.0-1~debian13_amd64.deb` |
| Debian 12 Bookworm | no `.deb`: use the [AppImage](#46-any-other-distro-appimage--qualquer-outra-distro-appimage) |

Example for Ubuntu 24.04 and its derivatives (for Debian 13, replace `ubuntu24.04` with
`debian13` in both lines):

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind_1.2.0-1~ubuntu24.04_amd64.deb
sudo apt-get install ./astrofind_1.2.0-1~ubuntu24.04_amd64.deb
```

1. Downloads the AstroFind 1.2.0 package.
2. Installs it and, because of the `./`, also downloads every library it depends on. Answer
   `Y` when asked "Do you want to continue?".

> 💡 **Tip:** if `curl` is not installed, first run `sudo apt-get install curl`.

> ⚠️ **Watch out (older bases):** AstroFind needs **Qt 6.4 or newer**. Pop!_OS 22.04, Zorin OS 17
> and Linux Mint 21 sit on Ubuntu 22.04, whose Qt is 6.2, so they are **not supported** (and
> their glibc is too old for the [AppImage](#46-any-other-distro-appimage--qualquer-outra-distro-appimage)
> too). Upgrade to Pop!_OS 24.04, Zorin OS 18 or Linux Mint 22. See
> [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

**What you should see:** the last lines mention `Setting up astrofind (1.2.0-1~ubuntu24.04)`
(or `~debian13`).

🇧🇷 **Português**

**DEB** é o formato de pacote da família Debian/Ubuntu; o `apt` é o gerenciador de pacotes dela.
Escolha o arquivo do seu sistema:

| Seu sistema | Arquivo |
|---|---|
| Ubuntu 24.04, Linux Mint 22, Pop!_OS 24.04, Zorin OS 18 | `astrofind_1.2.0-1~ubuntu24.04_amd64.deb` |
| Debian 13 Trixie | `astrofind_1.2.0-1~debian13_amd64.deb` |
| Debian 12 Bookworm | sem `.deb`: use o [AppImage](#46-any-other-distro-appimage--qualquer-outra-distro-appimage) |

Exemplo para o Ubuntu 24.04 e derivadas (no Debian 13, troque `ubuntu24.04` por `debian13` nas
duas linhas):

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind_1.2.0-1~ubuntu24.04_amd64.deb
sudo apt-get install ./astrofind_1.2.0-1~ubuntu24.04_amd64.deb
```

1. Baixa o pacote do AstroFind 1.2.0.
2. Instala e, por causa do `./`, também baixa todas as bibliotecas de que ele depende.
   Responda `S` (ou `Y`) quando perguntar se deseja continuar.

> 💡 **Dica:** se o `curl` não estiver instalado, rode antes `sudo apt-get install curl`.

> ⚠️ **Atenção (bases antigas):** o AstroFind precisa do **Qt 6.4 ou mais novo**. Pop!_OS 22.04,
> Zorin OS 17 e Linux Mint 21 ficam sobre o Ubuntu 22.04, cujo Qt é o 6.2, então **não são
> suportados** (e a glibc deles também é velha demais para o
> [AppImage](#46-any-other-distro-appimage--qualquer-outra-distro-appimage)). Atualize para
> Pop!_OS 24.04, Zorin OS 18 ou Linux Mint 22. Veja
> [Solução de Problemas](https://github.com/petrinhu/astrofind/wiki/Troubleshooting).

**O que você deve ver:** as últimas linhas falam em `Setting up astrofind (1.2.0-1~ubuntu24.04)`
(ou `Configurando astrofind (1.2.0-1~ubuntu24.04)`; no Debian 13, `~debian13`).

---

### 4.4 Arch Linux / Manjaro / CachyOS / EndeavourOS (pacman)

🟢 Beginner / Iniciante

🇬🇧 **English**

`pacman` is the package manager of the Arch family. The ready-made package is a
`.pkg.tar.zst` file:

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1-x86_64.pkg.tar.zst
sudo pacman -U ./astrofind-1.2.0-1-x86_64.pkg.tar.zst
```

1. Downloads the AstroFind 1.2.0 package.
2. Installs it (`-U` = install from a file) with the libraries it needs. Answer `Y` when asked
   "Proceed with installation?".

**What you should see:** the last lines show `pacman` installing `astrofind`. Manjaro, CachyOS
and EndeavourOS use exactly the same commands.

**Building it yourself (optional, 🟡 Intermediate).** Arch users can also build the package on
their own computer from a **PKGBUILD** (a recipe script that says where to download the source
code and how to compile it). `makepkg` follows the recipe:

```bash
mkdir astrofind-arch && cd astrofind-arch
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.2.0/packaging/arch/PKGBUILD
makepkg -si
```

1. Creates a folder `astrofind-arch` and enters it (`mkdir` = make directory, `cd` = change
   directory).
2. Downloads the PKGBUILD recipe of version 1.2.0.
3. `makepkg -si` installs the build tools and libraries the recipe declares (`-s`, through
   `pacman`), downloads the AstroFind 1.2.0 source code, compiles it, and installs the result
   (`-i`). It asks for your password and a few `Y/n` confirmations.

This takes several minutes: it is compiling the whole program. Run it as your **normal user**
(`makepkg` refuses to run as root).

> 💡 **Tip:** if `makepkg` says a tool like `fakeroot` or `strip` is missing, install the basic
> build tools with `sudo pacman -S --needed base-devel` and run `makepkg -si` again.

🇧🇷 **Português**

O `pacman` é o gerenciador de pacotes da família Arch. O pacote pronto é um arquivo
`.pkg.tar.zst`:

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1-x86_64.pkg.tar.zst
sudo pacman -U ./astrofind-1.2.0-1-x86_64.pkg.tar.zst
```

1. Baixa o pacote do AstroFind 1.2.0.
2. Instala (`-U` = instalar a partir de um arquivo) junto com as bibliotecas necessárias.
   Responda `S` (ou `Y`) quando perguntar se deseja continuar a instalação.

**O que você deve ver:** as últimas linhas mostram o `pacman` instalando o `astrofind`. Manjaro,
CachyOS e EndeavourOS usam exatamente os mesmos comandos.

**Compilar você mesmo (opcional, 🟡 Intermediário).** No Arch também dá para montar o pacote no
seu próprio computador a partir de um **PKGBUILD** (um script-receita que diz de onde baixar o
código-fonte e como compilá-lo). O `makepkg` segue a receita:

```bash
mkdir astrofind-arch && cd astrofind-arch
curl -LO https://raw.githubusercontent.com/petrinhu/astrofind/v1.2.0/packaging/arch/PKGBUILD
makepkg -si
```

1. Cria a pasta `astrofind-arch` e entra nela (`mkdir` = criar diretório, `cd` = mudar de
   diretório).
2. Baixa a receita PKGBUILD da versão 1.2.0.
3. `makepkg -si` instala as ferramentas e bibliotecas declaradas na receita (`-s`, via
   `pacman`), baixa o código-fonte do AstroFind 1.2.0, compila e instala o resultado (`-i`).
   Ele pede sua senha e algumas confirmações `S/n`.

Isso leva vários minutos: ele está compilando o programa inteiro. Rode como seu **usuário
normal** (o `makepkg` se recusa a rodar como root).

> 💡 **Dica:** se o `makepkg` disser que falta uma ferramenta como `fakeroot` ou `strip`,
> instale as ferramentas básicas de compilação com `sudo pacman -S --needed base-devel` e rode
> `makepkg -si` de novo.

---

### 4.5 openSUSE Tumbleweed (RPM)

🟢 Beginner / Iniciante

🇬🇧 **English**

openSUSE also uses **RPM** packages, but its package manager is `zypper`, and it has its own
AstroFind file:

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1.opensuse-tumbleweed.x86_64.rpm
sudo zypper install ./astrofind-1.2.0-1.opensuse-tumbleweed.x86_64.rpm
```

1. Downloads the AstroFind 1.2.0 package for openSUSE Tumbleweed.
2. Installs it with the libraries it needs. Answer `y` when asked "Continue?".

**What you should see:** the installation ends without an error. Then look for **AstroFind** in
your applications menu.

🇧🇷 **Português**

O openSUSE também usa pacotes **RPM**, mas o gerenciador de pacotes dele é o `zypper`, e ele tem
um arquivo próprio do AstroFind:

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/astrofind-1.2.0-1.opensuse-tumbleweed.x86_64.rpm
sudo zypper install ./astrofind-1.2.0-1.opensuse-tumbleweed.x86_64.rpm
```

1. Baixa o pacote do AstroFind 1.2.0 para o openSUSE Tumbleweed.
2. Instala junto com as bibliotecas necessárias. Responda `s` (ou `y`) quando perguntar se
   deseja continuar.

**O que você deve ver:** a instalação termina sem erro. Depois procure **AstroFind** no menu de
aplicativos.

---

### 4.6 Any other distro: AppImage / Qualquer outra distro: AppImage

🟢 Beginner / Iniciante

🇬🇧 **English**

An **AppImage** is a single file that contains the program and the libraries it needs. You do
not install it: you just make it executable and run it. This is also the file for **Debian 12
Bookworm** (there is no Debian 12 `.deb`). It works on 64-bit (x86-64) distros with
**glibc 2.36 or newer** (glibc is the basic system library; for example Ubuntu 23.04+,
Debian 12+, Fedora 37+).

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/AstroFind-1.2.0-x86_64.AppImage
chmod +x AstroFind-1.2.0-x86_64.AppImage
./AstroFind-1.2.0-x86_64.AppImage
```

1. Downloads the AppImage.
2. Marks it as executable.
3. Starts AstroFind.

No `sudo` and no password are needed. The AppImage does not add a menu entry by itself; keep the
file somewhere safe (for example `~/Applications`) and start it from there. **ASTAP** (offline
plate solver, [7.3](#73-astap--offline-plate-solver--solucionador-de-campo-offline)) and
`unzip` (for ZIP files, [7.2](#72-libarchive--tar-7z-rar-archives--arquivos-tar-7z-rar)) are
still separate, optional programs.

🇧🇷 **Português**

Um **AppImage** é um único arquivo que contém o programa e as bibliotecas de que ele precisa.
Você não instala: só marca como executável e roda. É também o arquivo para o **Debian 12
Bookworm** (não há `.deb` para o Debian 12). Funciona em distros de 64 bits (x86-64) com **glibc
2.36 ou mais nova** (a glibc é a biblioteca básica do sistema; por exemplo Ubuntu 23.04+,
Debian 12+, Fedora 37+).

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/AstroFind-1.2.0-x86_64.AppImage
chmod +x AstroFind-1.2.0-x86_64.AppImage
./AstroFind-1.2.0-x86_64.AppImage
```

1. Baixa o AppImage.
2. Marca como executável.
3. Abre o AstroFind.

Não precisa de `sudo` nem de senha. O AppImage não cria sozinho uma entrada no menu; guarde o
arquivo num lugar seguro (por exemplo `~/Applications`) e abra-o de lá. O **ASTAP** (solucionador
de campo offline, [7.3](#73-astap--offline-plate-solver--solucionador-de-campo-offline)) e o
`unzip` (para arquivos ZIP, [7.2](#72-libarchive--tar-7z-rar-archives--arquivos-tar-7z-rar))
continuam sendo programas separados e opcionais.

---

### 4.7 Build from source / Compilar do código-fonte

🟡 Intermediate / Intermediário

🇬🇧 **English**

If no package fits your system, you can **compile** AstroFind yourself: you download the
**source code** (the human-readable program text) and turn it into a program with the compiler.
The example below uses openSUSE's `zypper`; the commands for every other distro are in
[`INSTALL.md`](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md). The release page also
has the code as `astrofind-1.2.0-source.tar.gz`.

**Step 1 — install the tools and libraries** (from `INSTALL.md`):

```bash
sudo zypper install -y \
    cmake gcc-c++ \
    qt6-base-devel qt6-charts-devel \
    qt6-opengl-devel \
    cfitsio-devel fftw3-devel
```

(Installs CMake, the build organiser; `gcc-c++`, the C++ compiler; and the Qt 6, FITS and FFT
libraries in their `-devel` form, the version used for compiling. The `\` at the end of a line
just means "the command continues on the next line": paste the whole block at once.)

**Step 2 — optional extras** (recommended: archives, DSLR RAW, secure key storage,
translation tools):

```bash
sudo zypper install -y \
    libarchive-devel \
    libraw-devel \
    qtkeychain-qt6-devel libsecret-devel \
    qt6-linguist-devel \
    valgrind cppcheck clang-tools
```

(The last line, `valgrind cppcheck clang-tools`, is only for developers who want to run the code
audits; you may delete it from the block.)

**Step 3 — get the source code of version 1.2.0:**

```bash
sudo zypper install -y git
git clone --branch v1.2.0 https://github.com/petrinhu/astrofind.git
cd astrofind
```

1. Installs `git`, the tool that downloads source code. The build also uses it to fetch a few
   helper libraries, so keep your internet on.
2. Downloads the AstroFind code at the `v1.2.0` tag into a folder `astrofind`.
3. Enters that folder.

**Step 4 — compile and run:**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

1. Prepares the build in a `build/` folder, in "Release" (optimised) mode.
2. Compiles the program (`-j$(nproc)` uses all your processor cores; this takes a few minutes).
3. Starts AstroFind from the build folder.

**Optional — add it to the menu:** `sudo cmake --install build` copies the program, its menu
entry and its icons into the system (under `/usr/local`), so you can start it like any other app.

> ⚠️ **Watch out:** install the optional `libarchive-devel` and `libraw-devel` **before** step 4.
> If you add them later, run step 4 again: the features are switched on only when the libraries
> are present at compile time.

🇧🇷 **Português**

Se nenhum pacote serve para o seu sistema, você pode **compilar** o AstroFind: baixa o
**código-fonte** (o texto do programa, legível por pessoas) e o transforma em programa com o
compilador. O exemplo abaixo usa o `zypper` do openSUSE; os comandos das outras distros estão no
[`INSTALL.md`](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md). A página da release
também traz o código como `astrofind-1.2.0-source.tar.gz`.

**Passo 1 — instalar as ferramentas e bibliotecas** (do `INSTALL.md`):

```bash
sudo zypper install -y \
    cmake gcc-c++ \
    qt6-base-devel qt6-charts-devel \
    qt6-opengl-devel \
    cfitsio-devel fftw3-devel
```

(Instala o CMake, o organizador da compilação; o `gcc-c++`, o compilador C++; e as bibliotecas
Qt 6, FITS e FFT na forma `-devel`, a versão usada para compilar. A `\` no fim da linha só quer
dizer "o comando continua na próxima linha": cole o bloco inteiro de uma vez.)

**Passo 2 — extras opcionais** (recomendados: arquivos compactados, RAW de DSLR, guarda segura da
chave, ferramentas de tradução):

```bash
sudo zypper install -y \
    libarchive-devel \
    libraw-devel \
    qtkeychain-qt6-devel libsecret-devel \
    qt6-linguist-devel \
    valgrind cppcheck clang-tools
```

(A última linha, `valgrind cppcheck clang-tools`, é só para desenvolvedores que querem rodar as
auditorias de código; pode apagá-la do bloco.)

**Passo 3 — obter o código-fonte da versão 1.2.0:**

```bash
sudo zypper install -y git
git clone --branch v1.2.0 https://github.com/petrinhu/astrofind.git
cd astrofind
```

1. Instala o `git`, a ferramenta que baixa código-fonte. A compilação também o usa para buscar
   algumas bibliotecas auxiliares, então mantenha a internet ligada.
2. Baixa o código do AstroFind na tag `v1.2.0` para uma pasta `astrofind`.
3. Entra nessa pasta.

**Passo 4 — compilar e executar:**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AstroFind -j$(nproc)
./build/bin/AstroFind
```

1. Prepara a compilação numa pasta `build/`, no modo "Release" (otimizado).
2. Compila o programa (`-j$(nproc)` usa todos os núcleos do processador; leva alguns minutos).
3. Abre o AstroFind a partir da pasta de compilação.

**Opcional — colocar no menu:** `sudo cmake --install build` copia o programa, a entrada de menu
e os ícones para o sistema (em `/usr/local`), para você abri-lo como qualquer outro aplicativo.

> ⚠️ **Atenção:** instale os opcionais `libarchive-devel` e `libraw-devel` **antes** do passo 4.
> Se instalar depois, rode o passo 4 de novo: os recursos só são ligados quando as bibliotecas
> estão presentes na hora da compilação.

### 4.8 Checking the download / Conferir o download

🟡 Intermediate / Intermediário

🇬🇧 **English**

Optional. The release also has a file `SHA256SUMS` with a "fingerprint" (checksum) of every
file. Download it into the folder where your AstroFind file is and run:

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/SHA256SUMS
sha256sum -c SHA256SUMS --ignore-missing
```

1. Downloads the list of checksums.
2. Recomputes the checksum of each file you have and compares it with the list
   (`--ignore-missing` skips the files you did not download).

**What you should see:** your file name followed by `OK`. `FAILED` means the download is damaged:
delete the file and download it again.

🇧🇷 **Português**

Opcional. A release também traz um arquivo `SHA256SUMS` com uma "impressão digital" (checksum) de
cada arquivo. Baixe-o para a pasta onde está o seu arquivo do AstroFind e rode:

```bash
curl -LO https://github.com/petrinhu/astrofind/releases/download/v1.2.0/SHA256SUMS
sha256sum -c SHA256SUMS --ignore-missing
```

1. Baixa a lista de checksums.
2. Recalcula o checksum de cada arquivo que você tem e compara com a lista (`--ignore-missing`
   pula os arquivos que você não baixou).

**O que você deve ver:** o nome do seu arquivo seguido de `OK`. `FAILED` quer dizer que o
download está corrompido: apague o arquivo e baixe de novo.

---

## 5. First launch and the Setup Wizard / Primeira execução e o Assistente de Configuração

🟢 Beginner / Iniciante

🇬🇧 **English**

**Start AstroFind:** open your applications menu and search for **AstroFind**, or type
`AstroFind` in a terminal and press Enter (capital A and F).

About a fraction of a second after the main window opens, the **AstroFind Setup Wizard**
appears. It has three pages. Every page has a **"Skip this step"** button, and nothing here is
final: you can change everything later.

1. **"Step 1 of 3 — Observer Identity"** ("These details identify you in every ADES report
   submitted to the MPC.")
   - **MPC Station Code:** the 3-character code the MPC gave your observatory (for example
     `568`). No code yet? Leave it empty; reports will then use `XXX`.
   - **Your name:** your full name, as it should appear in reports.
   - **Telescope:** a short description, for example `0.35-m f/7 SCT + CCD`.
   - Checkbox **"Show this wizard every time AstroFind starts"**. Uncheck it if you don't want
     to see the wizard again.
2. **"Step 2 of 3 — Plate-solving API Key"**: paste your free **astrometry.net API key**
   (see [7.4](#74-astrometrynet-api-key--chave-de-api-do-astrometrynet)). The **Show** / **Hide**
   button reveals what you typed. Without a key AstroFind still works, but online plate solving
   is unavailable (images that already have a WCS in their header don't need it).
3. **"Step 3 of 3 — Asteroid Catalog (optional)"**: **"Download now (~200 MB)"** fetches
   **MPCORB.DAT**, the MPC's list of all known asteroid orbits. AstroFind uses it only when the
   online SkyBoT service can't be reached. You can do it later with
   **Internet → Download MPCOrb Database**.

The wizard does **not** ask for your location. Set it afterwards in
**File → Settings...** (`Ctrl+,`) → **Observer** → group "Location (topocentric correction)".
Without it, Run Data Reduction warns "Localização não configurada" and your positions would be
reported as if seen from the centre of the Earth. See [Settings](https://github.com/petrinhu/astrofind/wiki/Settings).

To open the wizard again: **Help → Setup Wizard…**. To turn its automatic start on or off:
**File → Settings... → Display →** "Show setup wizard when the application starts".

**What you should see:** a dark window (the "Night" theme is the default) titled
`AstroFind 1.2.0`, with the **Workflow** panel on the left and the text "No image loaded / Use
File → Import Images…" in the middle. Next step: [Quick-Start](https://github.com/petrinhu/astrofind/wiki/Quick-Start).

🇧🇷 **Português**

**Abra o AstroFind:** procure **AstroFind** no menu de aplicativos, ou digite `AstroFind` num
terminal e aperte Enter (A e F maiúsculos).

Uma fração de segundo depois que a janela principal abre, aparece o **Assistente de configuração
do AstroFind**. Ele tem três páginas. Toda página tem o botão **"Pular esta etapa"**, e nada aqui
é definitivo: dá para mudar tudo depois.

1. **"Etapa 1 de 3 — Identidade do observador"** (os dados que identificam você em cada
   relatório ADES enviado ao MPC).
   - **Código de estação MPC:** o código de 3 caracteres que o MPC deu ao seu observatório (por
     exemplo `568`). Ainda não tem? Deixe vazio; os relatórios usarão `XXX`.
   - **Seu nome:** seu nome completo, como deve aparecer nos relatórios.
   - **Telescópio:** uma descrição curta, por exemplo `0.35-m f/7 SCT + CCD`.
   - Caixa **"Mostrar este assistente sempre que o AstroFind iniciar"**. Desmarque se não quiser
     ver o assistente de novo.
2. **"Etapa 2 de 3 — Chave de API para solução de campo"**: cole a sua **chave de API gratuita
   do astrometry.net** (veja [7.4](#74-astrometrynet-api-key--chave-de-api-do-astrometrynet)).
   O botão **Mostrar** / **Ocultar** revela o que você digitou. Sem chave o AstroFind funciona,
   mas a solução de campo online fica indisponível (imagens que já têm WCS no cabeçalho não
   precisam dela).
3. **"Etapa 3 de 3 — Catálogo de asteroides (opcional)"**: **"Baixar agora (~200 MB)"** baixa o
   **MPCORB.DAT**, a lista do MPC com as órbitas de todos os asteroides conhecidos. O AstroFind
   só o usa quando o serviço online SkyBoT não responde. Dá para fazer depois em
   **Internet → Baixar Banco MPCOrb**.

O assistente **não** pergunta a sua localização. Configure depois em
**Arquivo → Configurações...** (`Ctrl+,`) → **Observador** → grupo "Localização (correção
topocêntrica)". Sem ela, a Redução de Dados avisa "Localização não configurada" e suas posições
seriam relatadas como se vistas do centro da Terra. Veja [Configurações](https://github.com/petrinhu/astrofind/wiki/Settings).

Para abrir o assistente de novo: **Ajuda → Assistente de Configuração…**. Para ligar ou
desligar a abertura automática: **Arquivo → Configurações... → Exibição →** "Mostrar assistente
de configuração ao iniciar o aplicativo".

**O que você deve ver:** uma janela escura (o tema "Noite" é o padrão) com o título
`AstroFind 1.2.0`, o painel **Fluxo de Trabalho** à esquerda e o texto "Nenhuma imagem
carregada / Use Arquivo → Importar Imagens…" no meio. Próximo passo: [Início Rápido](https://github.com/petrinhu/astrofind/wiki/Quick-Start).

---

## 6. Where AstroFind keeps its files / Onde o AstroFind guarda seus arquivos

🟡 Intermediate / Intermediário

🇬🇧 **English**

Folders starting with `.` are hidden; in your file manager press `Ctrl+H` to show them. `~`
means your home folder.

| What | Where |
|---|---|
| All settings (a plain text file) | `~/.config/AstroFind/AstroFind.conf` |
| MPCORB.DAT, star-catalog cache (`catalog_cache.db`) | `~/.local/share/AstroFind/AstroFind/` |
| astrometry.net API key | the system keychain (KWallet or GNOME Keyring / Secret Service), service "AstroFind", when AstroFind was built with qtkeychain; otherwise in plain text in the settings file |
| Your projects (`.gus`) | `~/projects` by default |
| Reports | the "Report output folder" in **Settings → Connections** (default: Documents) |

> 💡 **Tip:** **File → Settings... → Reset to Defaults** restores the recommended values
> without touching files by hand. Teachers can copy all settings to students with
> **File → Exportar Configuração da Escola...** and **Importar Configuração da Escola...**.

🇧🇷 **Português**

Pastas que começam com `.` são ocultas; no gerenciador de arquivos aperte `Ctrl+H` para vê-las.
`~` quer dizer a sua pasta pessoal.

| O quê | Onde |
|---|---|
| Todas as configurações (um arquivo de texto simples) | `~/.config/AstroFind/AstroFind.conf` |
| MPCORB.DAT, cache do catálogo de estrelas (`catalog_cache.db`) | `~/.local/share/AstroFind/AstroFind/` |
| Chave de API do astrometry.net | o chaveiro do sistema (KWallet ou GNOME Keyring / Secret Service), serviço "AstroFind", quando o AstroFind foi compilado com qtkeychain; senão, em texto simples no arquivo de configurações |
| Seus projetos (`.gus`) | `~/projects` por padrão |
| Relatórios | a "Pasta de saída de relatórios" em **Configurações → Conexões** (padrão: Documentos) |

> 💡 **Dica:** **Arquivo → Configurações... → Restaurar padrões** volta aos valores
> recomendados sem mexer em arquivos à mão. Professores podem copiar todas as configurações para
> os alunos com **Arquivo → Exportar Configuração da Escola...** e **Importar Configuração da
> Escola...**.

---

## 7. Optional features / Recursos opcionais

🟡 Intermediate / Intermediário

### 7.1 LibRaw — DSLR RAW images / imagens RAW de DSLR

🇬🇧 **English**

Needed to open digital-camera RAW files (CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2, PEF and others).

- **Official packages** (RPM, DEB, Arch package, PKGBUILD): already included, nothing to do.
- **Built from source:** install `LibRaw-devel` (Fedora/Rocky), `libraw-dev`
  (Ubuntu/Debian/Mint), `libraw` (Arch family) or `libraw-devel` (openSUSE) **before** compiling.
- Without it, opening a RAW file shows: "DSLR RAW support is not available in this build of
  AstroFind (compiled without LibRaw): …".

🇧🇷 **Português**

Necessária para abrir arquivos RAW de câmeras digitais (CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2,
PEF e outros).

- **Pacotes oficiais** (RPM, DEB, pacote do Arch, PKGBUILD): já incluída, nada a fazer.
- **Compilando do fonte:** instale `LibRaw-devel` (Fedora/Rocky), `libraw-dev`
  (Ubuntu/Debian/Mint), `libraw` (família Arch) ou `libraw-devel` (openSUSE) **antes** de
  compilar.
- Sem ela, abrir um RAW mostra: "DSLR RAW support is not available in this build of AstroFind
  (compiled without LibRaw): …".

### 7.2 libarchive — TAR, 7Z, RAR archives / arquivos TAR, 7Z, RAR

🇬🇧 **English**

Lets AstroFind open image sets packed as `.tar.gz`, `.tgz`, `.tar.bz2`, `.tar.xz`, `.7z` or
`.rar`. Official packages include it; the universal installer also offers it at stage 6/7. For
source builds install `libarchive-devel` / `libarchive-dev` / `libarchive` before compiling.
Without it the log shows "Cannot extract '…': libarchive not available. Install
libarchive-devel and recompile."

> 💡 **Tip:** **ZIP** files are opened with the system `unzip` command, not libarchive. If ZIPs
> don't open, install `unzip` with your package manager (for example `sudo apt-get install unzip`).

🇧🇷 **Português**

Permite ao AstroFind abrir conjuntos de imagens compactados em `.tar.gz`, `.tgz`, `.tar.bz2`,
`.tar.xz`, `.7z` ou `.rar`. Os pacotes oficiais já incluem; o instalador universal também a
oferece na etapa 6/7. Para compilação do fonte instale `libarchive-devel` / `libarchive-dev` /
`libarchive` antes de compilar. Sem ela o registro mostra "Cannot extract '…': libarchive not
available. Install libarchive-devel and recompile."

> 💡 **Dica:** arquivos **ZIP** são abertos pelo comando `unzip` do sistema, não pela libarchive.
> Se os ZIPs não abrirem, instale o `unzip` pelo gerenciador de pacotes (por exemplo
> `sudo apt-get install unzip`).

### 7.3 ASTAP — offline plate solver / solucionador de campo offline

🇬🇧 **English**

**ASTAP** is a free (but not open-source) program that plate-solves images **on your own
computer**, without internet and without an API key. It is not in the Linux repositories, and
AstroFind's installer does not install it.

1. Download ASTAP for Linux from [hnsky.org/astap.htm](https://www.hnsky.org/astap.htm).
2. From the same page, also download and install one of its **star databases** (ASTAP cannot
   solve without one; the page explains which database fits your field of view).
3. In AstroFind open **File → Settings... → Connections** (PT **Arquivo → Configurações... →
   Conexões**), group "Plate Solving":
   - **Backend:** choose **"ASTAP (local, offline)"**.
   - **Executável ASTAP:** click **"…"** and select the ASTAP program (for example
     `/usr/bin/astap`).
4. Click **OK**.

Without a path, Run Data Reduction shows "ASTAP não configurado".

🇧🇷 **Português**

O **ASTAP** é um programa gratuito (mas não de código aberto) que faz a solução de campo **no
seu próprio computador**, sem internet e sem chave de API. Ele não está nos repositórios do
Linux, e o instalador do AstroFind não o instala.

1. Baixe o ASTAP para Linux em [hnsky.org/astap.htm](https://www.hnsky.org/astap.htm).
2. Na mesma página, baixe e instale também um dos **bancos de estrelas** dele (o ASTAP não
   resolve sem um; a página explica qual banco serve para o seu campo de visão).
3. No AstroFind abra **Arquivo → Configurações... → Conexões** (EN **File → Settings... →
   Connections**), grupo "Plate Solving":
   - **Backend:** escolha **"ASTAP (local, offline)"**.
   - **Executável ASTAP:** clique em **"…"** e selecione o programa ASTAP (por exemplo
     `/usr/bin/astap`).
4. Clique em **OK**.

Sem o caminho, a Redução de Dados mostra "ASTAP não configurado".

### 7.4 astrometry.net API key / Chave de API do astrometry.net

🇬🇧 **English**

**astrometry.net** is the default, online plate solver. It is free but needs an **API key** (a
personal password-like code that identifies your account to the service).

1. Create a free account at [nova.astrometry.net](https://nova.astrometry.net) and sign in.
2. Open [nova.astrometry.net/api_help](https://nova.astrometry.net/api_help): your key is shown
   there. Copy it.
3. Paste it in either place:
   - the Setup Wizard, page **"Step 2 of 3 — Plate-solving API Key"**, or
   - **File → Settings... → Connections → API Key:** (PT **Arquivo → Configurações... →
     Conexões → Chave de API:**). The link "Get a free key at nova.astrometry.net" next to it
     opens the same page.
4. If you forget, AstroFind asks for it the first time it needs to solve ("Enter your free API
   key from nova.astrometry.net:").

Next to the field a badge tells you how the key is stored: "🔒 Stored in system keychain
(KWallet / SecretService)" or "⚠ Stored in plain text — install qtkeychain-qt6-devel +
libsecret-devel for secure storage".

🇧🇷 **Português**

O **astrometry.net** é o solucionador de campo padrão, online. É gratuito, mas precisa de uma
**chave de API** (um código pessoal, parecido com uma senha, que identifica sua conta no
serviço).

1. Crie uma conta gratuita em [nova.astrometry.net](https://nova.astrometry.net) e entre nela.
2. Abra [nova.astrometry.net/api_help](https://nova.astrometry.net/api_help): sua chave aparece
   ali. Copie.
3. Cole em um destes lugares:
   - no Assistente, página **"Etapa 2 de 3 — Chave de API para solução de campo"**, ou
   - em **Arquivo → Configurações... → Conexões → Chave de API:** (EN **File → Settings... →
     Connections → API Key:**). O link "Obtenha uma chave gratuita em nova.astrometry.net" ao lado abre a
     mesma página.
4. Se esquecer, o AstroFind pede a chave na primeira vez que precisar resolver ("Enter your free
   API key from nova.astrometry.net:").

Ao lado do campo, um selo diz como a chave está guardada: "🔒 Armazenado no chaveiro do sistema (KWallet
/ SecretService)" ou "⚠ Armazenado em texto simples — instale qtkeychain-qt6-devel +
libsecret-devel para armazenamento seguro".

---

## 8. Updating / Atualizar

🟢 Beginner / Iniciante

🇬🇧 **English**

Repeat the installation with the new version number. For example, for a future release, replace
`v1.2.0` and `1.2.0` in the commands above with the new tag listed on the
[releases page](https://github.com/petrinhu/astrofind/releases).

- **RPM / DEB:** `sudo dnf install ./<new>.rpm` (openSUSE: `sudo zypper install ./<new>.rpm`) or
  `sudo apt-get install ./<new>.deb` replaces the old version.
- **Arch family:** `sudo pacman -U ./<new>.pkg.tar.zst`; or, if you built it yourself, download
  the new PKGBUILD into a fresh folder and run `makepkg -si` again.
- **AppImage:** download the new AppImage, make it executable, and delete the old file.
- **Source build:** `cd astrofind`, then `git fetch --tags`, `git checkout <new-tag>`, and repeat
  step 4 of [4.7](#47-build-from-source--compilar-do-código-fonte).

Your settings, API key, MPCORB.DAT and projects are kept. To refresh the asteroid list, use
**Internet → Update MPCOrb Database**.

🇧🇷 **Português**

Repita a instalação com o novo número de versão. Por exemplo, para uma versão futura, troque
`v1.2.0` e `1.2.0` nos comandos acima pela nova tag listada na
[página de releases](https://github.com/petrinhu/astrofind/releases).

- **RPM / DEB:** `sudo dnf install ./<novo>.rpm` (openSUSE: `sudo zypper install ./<novo>.rpm`)
  ou `sudo apt-get install ./<novo>.deb` substitui a versão antiga.
- **Família Arch:** `sudo pacman -U ./<novo>.pkg.tar.zst`; ou, se você compilou, baixe o novo
  PKGBUILD numa pasta nova e rode `makepkg -si` de novo.
- **AppImage:** baixe o novo AppImage, marque como executável e apague o arquivo antigo.
- **Compilado do fonte:** `cd astrofind`, depois `git fetch --tags`, `git checkout <nova-tag>`,
  e repita o passo 4 de [4.7](#47-build-from-source--compilar-do-código-fonte).

Suas configurações, chave de API, MPCORB.DAT e projetos são mantidos. Para atualizar a lista de
asteroides, use **Internet → Atualizar Banco MPCOrb**.

---

## 9. Uninstalling / Desinstalar

🟢 Beginner / Iniciante

🇬🇧 **English**

| You installed with… | Remove with |
|---|---|
| RPM (Fedora, Rocky, AlmaLinux, RHEL) | `sudo dnf remove astrofind` |
| RPM (openSUSE) | `sudo zypper remove astrofind` |
| DEB (Ubuntu, Debian, Mint, Pop!_OS, Zorin) | `sudo apt-get remove astrofind` |
| Arch package or PKGBUILD (Arch, Manjaro, CachyOS, EndeavourOS) | `sudo pacman -R astrofind` |
| AppImage | just delete the `AstroFind-…-x86_64.AppImage` file |
| Source build without `cmake --install` | just delete the `astrofind` folder |
| Source build with `sudo cmake --install build` | from inside the `astrofind` folder: `sudo xargs rm < build/install_manifest.txt` (deletes every file the install copied), then delete the folder |

Removing the program **keeps your personal data**. To erase it too:

```bash
rm -rf ~/.config/AstroFind ~/.local/share/AstroFind
```

(`rm -rf` deletes folders and everything inside, **without asking and without a trash can**:
check the line before pressing Enter.) If the installer created a desktop shortcut, delete the
`astrofind.desktop` icon from your Desktop. A key stored in the system keychain can be removed
with your desktop's password manager (look for the entry "AstroFind").

🇧🇷 **Português**

| Você instalou com… | Remova com |
|---|---|
| RPM (Fedora, Rocky, AlmaLinux, RHEL) | `sudo dnf remove astrofind` |
| RPM (openSUSE) | `sudo zypper remove astrofind` |
| DEB (Ubuntu, Debian, Mint, Pop!_OS, Zorin) | `sudo apt-get remove astrofind` |
| Pacote do Arch ou PKGBUILD (Arch, Manjaro, CachyOS, EndeavourOS) | `sudo pacman -R astrofind` |
| AppImage | basta apagar o arquivo `AstroFind-…-x86_64.AppImage` |
| Compilação do fonte sem `cmake --install` | basta apagar a pasta `astrofind` |
| Compilação do fonte com `sudo cmake --install build` | de dentro da pasta `astrofind`: `sudo xargs rm < build/install_manifest.txt` (apaga cada arquivo que a instalação copiou), depois apague a pasta |

Remover o programa **mantém seus dados pessoais**. Para apagá-los também:

```bash
rm -rf ~/.config/AstroFind ~/.local/share/AstroFind
```

(`rm -rf` apaga pastas e tudo dentro, **sem perguntar e sem lixeira**: confira a linha antes de
apertar Enter.) Se o instalador criou um atalho na área de trabalho, apague o ícone
`astrofind.desktop` dela. Uma chave guardada no chaveiro do sistema pode ser removida pelo
gerenciador de senhas da sua área de trabalho (procure a entrada "AstroFind").

---

## 10. It didn't work / Não funcionou

🟢 Beginner / Iniciante

🇬🇧 **English**

| What you see | What to do |
|---|---|
| `curl: command not found` | Install curl: `sudo apt-get install curl`, `sudo dnf install curl`, `sudo pacman -S curl` or `sudo zypper install curl`. |
| `Permission denied` when running `./install.sh` | You skipped `chmod +x install.sh`. Run it, then try again. |
| `… is not in the sudoers file` | Your user can't use `sudo`. Ask whoever administers the computer. |
| `404` / `Not Found` while downloading | Check the address letter by letter (copy and paste it). The version must exist on the [releases page](https://github.com/petrinhu/astrofind/releases). |
| `dnf`: "nothing provides qt6-…" on Rocky Linux | Enable EPEL and CRB first ([4.2](#42-fedora--rocky-linux--almalinux--rhel-9-rpm)). |
| `apt`: a Qt 6 dependency "is not installable" | Your system's Qt is older than 6.4 ([4.3](#43-ubuntu--debian--linux-mint--pop_os--zorin-os-deb)), or you picked the `.deb` of another system (for example `~debian13` on Ubuntu). |
| The AppImage does not start and mentions `GLIBC_2.36` | Your system is too old for the AppImage (glibc older than 2.36). Use a package for your distro, or a newer release of your distro ([4.6](#46-any-other-distro-appimage--qualquer-outra-distro-appimage)). |
| AstroFind is installed but not in the menu | Log out and back in, or start it by typing `AstroFind` in a terminal. |
| RAW, TAR/7Z/RAR or ZIP files don't open | See [7.1](#71-libraw--dslr-raw-images--imagens-raw-de-dslr) and [7.2](#72-libarchive--tar-7z-rar-archives--arquivos-tar-7z-rar). |

More problems and their fixes: [Troubleshooting](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) and [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ). When asking for help, copy the
**last 20 lines** of the terminal output into your message.

🇧🇷 **Português**

| O que aparece | O que fazer |
|---|---|
| `curl: command not found` | Instale o curl: `sudo apt-get install curl`, `sudo dnf install curl`, `sudo pacman -S curl` ou `sudo zypper install curl`. |
| `Permission denied` (permissão negada) ao rodar `./install.sh` | Você pulou o `chmod +x install.sh`. Rode-o e tente de novo. |
| `… is not in the sudoers file` | Seu usuário não pode usar `sudo`. Peça a quem administra o computador. |
| `404` / `Not Found` durante o download | Confira o endereço letra por letra (copie e cole). A versão precisa existir na [página de releases](https://github.com/petrinhu/astrofind/releases). |
| `dnf`: "nothing provides qt6-…" no Rocky Linux | Ligue antes o EPEL e o CRB ([4.2](#42-fedora--rocky-linux--almalinux--rhel-9-rpm)). |
| `apt`: uma dependência do Qt 6 "não é instalável" | O Qt do seu sistema é anterior ao 6.4 ([4.3](#43-ubuntu--debian--linux-mint--pop_os--zorin-os-deb)), ou você escolheu o `.deb` de outro sistema (por exemplo `~debian13` no Ubuntu). |
| O AppImage não abre e fala em `GLIBC_2.36` | Seu sistema é antigo demais para o AppImage (glibc anterior à 2.36). Use um pacote da sua distro, ou uma versão mais nova da sua distro ([4.6](#46-any-other-distro-appimage--qualquer-outra-distro-appimage)). |
| O AstroFind está instalado mas não aparece no menu | Saia da sessão e entre de novo, ou abra digitando `AstroFind` num terminal. |
| Arquivos RAW, TAR/7Z/RAR ou ZIP não abrem | Veja [7.1](#71-libraw--dslr-raw-images--imagens-raw-de-dslr) e [7.2](#72-libarchive--tar-7z-rar-archives--arquivos-tar-7z-rar). |

Mais problemas e soluções: [Solução de Problemas](https://github.com/petrinhu/astrofind/wiki/Troubleshooting) e [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ). Ao pedir ajuda,
copie as **últimas 20 linhas** da saída do terminal na sua mensagem.
