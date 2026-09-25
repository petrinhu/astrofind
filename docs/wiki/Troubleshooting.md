# Troubleshooting / Solução de Problemas

🇬🇧 **English**
**Who this page is for:** anyone for whom something in AstroFind does not work. Find your
problem in the list below. Each entry says what you see, why it happens, and how to fix it.
The messages in quotes are copied from AstroFind itself; `…` stands for a file name or a number
that changes. Many messages appear in the **Log panel** (the list of time-stamped lines at the
bottom of the window, see [Where is the log?](#where-is-the-log--onde-fica-o-log)). For general
questions see the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ); for words you don't know, the [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary).

🇧🇷 **Português**
**Para quem é esta página:** para quem tem alguma coisa do AstroFind que não funciona. Procure o
seu problema na lista abaixo. Cada item diz o que você vê, por que acontece e como resolver. As
mensagens entre aspas foram copiadas do próprio AstroFind; `…` representa um nome de arquivo ou
um número que muda. Muitas mensagens aparecem no **painel de Log** (a lista de linhas com horário
na parte de baixo da janela, veja [Onde fica o log?](#where-is-the-log--onde-fica-o-log)).
Algumas mensagens não têm tradução e aparecem em inglês mesmo com a interface em português;
nesses casos a tabela mostra o texto em inglês. Para dúvidas gerais veja o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ); para
palavras que você não conhece, o [Glossário](https://github.com/petrinhu/astrofind/wiki/Glossary).

**Contents / Conteúdo**

1. [Installation and dependencies / Instalação e dependências](#installation-and-dependencies--instalação-e-dependências)
2. [AstroFind won't start / O AstroFind não abre](#astrofind-wont-start--o-astrofind-não-abre)
3. [Files won't open / Arquivos não abrem](#files-wont-open--arquivos-não-abrem)
4. [Plate solving fails / O plate solving falha](#plate-solving-fails--o-plate-solving-falha)
5. [Catalog, VizieR and known objects / Catálogo, VizieR e objetos conhecidos](#catalog-vizier-and-known-objects--catálogo-vizier-e-objetos-conhecidos)
6. [No stars detected / Nenhuma estrela detectada](#no-stars-detected--nenhuma-estrela-detectada)
7. [Blink, moving objects and measuring / Blink, objetos em movimento e medição](#blink-moving-objects-and-measuring--blink-objetos-em-movimento-e-medição)
8. [Report and submission / Relatório e envio](#report-and-submission--relatório-e-envio)
9. [The image looks black or white / A imagem parece preta ou branca](#the-image-looks-black-or-white--a-imagem-parece-preta-ou-branca)
10. [Keyboard shortcuts don't work / Atalhos de teclado não funcionam](#keyboard-shortcuts-dont-work--atalhos-de-teclado-não-funcionam)
11. [Language: Portuguese text in the English interface / Idioma: texto em português na interface em inglês](#language-portuguese-text-in-the-english-interface--idioma-texto-em-português-na-interface-em-inglês)
12. [Where is the log? / Onde fica o log?](#where-is-the-log--onde-fica-o-log)
13. [How to reset the settings / Como restaurar as configurações](#how-to-reset-the-settings--como-restaurar-as-configurações)
14. [Collecting information for a bug report / Juntando informações para relatar um bug](#collecting-information-for-a-bug-report--juntando-informações-para-relatar-um-bug)

---

## Installation and dependencies / Instalação e dependências

🟢 Beginner / Iniciante · 🟡 Intermediate / Intermediário

The installation commands for every distribution are in [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) and in
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md) (the single source of
truth). / Os comandos de instalação de cada distribuição estão em [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) e no
[INSTALL.md](https://github.com/petrinhu/astrofind/blob/main/INSTALL.md) (a referência oficial).

🇬🇧 **English**

| Problem | Cause | Fix |
|---|---|---|
| RAW camera files are refused with "DSLR RAW support is not available in this build of AstroFind (compiled without LibRaw): …" | AstroFind was compiled without the optional LibRaw library (the library that reads camera RAW files). | Install LibRaw and build again. Package names: `LibRaw-devel` (Fedora; Rocky Linux 9 from the CRB repository), `libraw-dev` (Debian, Ubuntu, Mint, Pop!_OS, Zorin), `libraw` (Arch, Manjaro, CachyOS), `libraw-devel` (openSUSE). When you run `cmake`, check that it prints "Found LibRaw" and not "LibRaw NOT found — DSLR RAW loading disabled". |
| A TAR.GZ, TAR.BZ2, TAR.XZ, 7Z or RAR archive does not open. The Log panel says "Cannot extract '…': libarchive not available. Install libarchive-devel and recompile." | AstroFind was compiled without the optional libarchive library. | Install `libarchive-devel` (Fedora, Rocky, openSUSE), `libarchive-dev` (Debian/Ubuntu family) or `libarchive` (Arch family), then build again. Or extract the archive yourself and load the images. |
| A ZIP file does not open. The Log panel says "Timed out extracting ZIP: …" | ZIP files are extracted with the system `unzip` program. If `unzip` is not installed, or the file is very large (limit 30 s), extraction stops. | Install `unzip` with your package manager (for example `sudo dnf install unzip` or `sudo apt install unzip`), or extract the ZIP yourself. |
| "No image files found in ZIP: …" | The ZIP has no files of a type AstroFind extracts. BMP, JPEG and PDS4 `.xml` are not extracted from archives. | Extract the ZIP yourself and load the files with **File → Import Images…**. |
| Settings → Connections shows "⚠ Stored in plain text — install qtkeychain-qt6-devel + libsecret-devel for secure storage" | AstroFind was built without qt6keychain, so the astrometry.net API key is saved in the normal settings file. | This works, but the key is readable by anyone with access to your account. To store it securely, install the keychain packages listed in INSTALL.md and build again. |
| `cmake` stops with an error about Qt6 or a missing package | A required build package is missing (Qt 6.4 or newer, Qt Charts, cfitsio, fftw). | Install the "Required" packages of your distribution from INSTALL.md, delete the `build` folder and run `cmake` again. |

🇧🇷 **Português**

| Problema | Causa | Solução |
|---|---|---|
| Arquivos RAW de câmera são recusados com "DSLR RAW support is not available in this build of AstroFind (compiled without LibRaw): …" (sem tradução) | O AstroFind foi compilado sem a biblioteca opcional LibRaw (a biblioteca que lê arquivos RAW de câmera). | Instale a LibRaw e compile de novo. Nomes dos pacotes: `LibRaw-devel` (Fedora; Rocky Linux 9 pelo repositório CRB), `libraw-dev` (Debian, Ubuntu, Mint, Pop!_OS, Zorin), `libraw` (Arch, Manjaro, CachyOS), `libraw-devel` (openSUSE). Ao rodar o `cmake`, confira se ele mostra "Found LibRaw" e não "LibRaw NOT found — DSLR RAW loading disabled". |
| Um arquivo TAR.GZ, TAR.BZ2, TAR.XZ, 7Z ou RAR não abre. O painel de Log diz "Não foi possível extrair '…': libarchive não disponível. Instale libarchive-devel e recompile." | O AstroFind foi compilado sem a biblioteca opcional libarchive. | Instale `libarchive-devel` (Fedora, Rocky, openSUSE), `libarchive-dev` (família Debian/Ubuntu) ou `libarchive` (família Arch) e compile de novo. Ou extraia o arquivo você mesmo e carregue as imagens. |
| Um ZIP não abre. O painel de Log diz "Tempo esgotado ao extrair ZIP: …" | Arquivos ZIP são extraídos pelo programa `unzip` do sistema. Se o `unzip` não estiver instalado, ou o arquivo for muito grande (limite de 30 s), a extração para. | Instale o `unzip` pelo gerenciador de pacotes (por exemplo `sudo dnf install unzip` ou `sudo apt install unzip`), ou extraia o ZIP você mesmo. |
| "Nenhum arquivo de imagem encontrado no ZIP: …" | O ZIP não tem arquivos de um tipo que o AstroFind extrai. BMP, JPEG e `.xml` PDS4 não são extraídos de arquivos compactados. | Extraia o ZIP você mesmo e carregue os arquivos com **Arquivo → Importar Imagens…**. |
| Configurações → Conexões mostra "⚠ Armazenado em texto simples — instale qtkeychain-qt6-devel + libsecret-devel para armazenamento seguro" | O AstroFind foi compilado sem qt6keychain, então a chave de API do astrometry.net fica no arquivo de configurações comum. | Funciona, mas a chave pode ser lida por quem tiver acesso à sua conta. Para guardá-la com segurança, instale os pacotes de keychain indicados no INSTALL.md e compile de novo. |
| O `cmake` para com um erro sobre Qt6 ou um pacote faltando | Falta um pacote obrigatório de compilação (Qt 6.4 ou mais novo, Qt Charts, cfitsio, fftw). | Instale os pacotes "Obrigatórios" da sua distribuição pelo INSTALL.md, apague a pasta `build` e rode o `cmake` de novo. |

---

## AstroFind won't start / O AstroFind não abre

🟢 Beginner / Iniciante

🇬🇧 **English**
First, start AstroFind from a terminal, so you can read what goes wrong:

```bash
AstroFind
```

(This starts the program by its name. Messages from AstroFind and from the system appear in the
terminal window. The name has a capital A and a capital F.)

| Problem | Cause | Fix |
|---|---|---|
| The terminal says that a shared library (a file ending in `.so`, for example one whose name starts with `libQt6`, `libcfitsio` or `libraw`) cannot be found | A library AstroFind needs is not installed. | Install the package with its package manager, which installs the dependencies too: `sudo dnf install ./astrofind-1.2.1-1.fc44.x86_64.rpm` (Rocky/RHEL 9: the `.el9` file, after enabling EPEL and CRB; openSUSE: `sudo zypper install ./…opensuse-tumbleweed….rpm`), `sudo apt-get install ./astrofind_1.2.1-1.ubuntu24.04_amd64.deb` (Debian 13: the `.debian13` file; Debian 12 uses the AppImage) or `sudo pacman -U ./astrofind-1.2.1-1-x86_64.pkg.tar.zst`. Use the file made for your system. See [Installation](https://github.com/petrinhu/astrofind/wiki/Installation). |
| The terminal mentions the Qt "platform plugin" (for example `xcb` or `wayland`) | Graphics libraries used by Qt are missing. | Install the OpenGL and keyboard libraries of your distribution (on Fedora: `sudo dnf install mesa-libGL libxkbcommon`; on Ubuntu/Debian: `sudo apt install libgl1 libxkbcommon0`). |
| The window opens in a strange place, off screen, or with panels missing | The saved window layout does not fit your current screen. | Use **Window → Auto-Arrange Windows** and the **Window → View …** items to show the panels again. If that does not help, reset the settings (see [below](#how-to-reset-the-settings--como-restaurar-as-configurações)). |
| AstroFind closes right after starting | Could be a bad settings file or a bug. | Reset the settings as explained below. If it still closes, report a bug with the terminal output. |

🇧🇷 **Português**
Primeiro, abra o AstroFind pelo terminal, para poder ler o que dá errado:

```bash
AstroFind
```

(Isso abre o programa pelo nome. As mensagens do AstroFind e do sistema aparecem na janela do
terminal. O nome tem A e F maiúsculos.)

| Problema | Causa | Solução |
|---|---|---|
| O terminal diz que uma biblioteca compartilhada (um arquivo terminado em `.so`, por exemplo um cujo nome começa com `libQt6`, `libcfitsio` ou `libraw`) não foi encontrada | Falta instalar uma biblioteca de que o AstroFind precisa. | Instale o pacote pelo gerenciador de pacotes, que instala também as dependências: `sudo dnf install ./astrofind-1.2.1-1.fc44.x86_64.rpm` (Rocky/RHEL 9: o arquivo `.el9`, depois de ligar EPEL e CRB; openSUSE: `sudo zypper install ./…opensuse-tumbleweed….rpm`), `sudo apt-get install ./astrofind_1.2.1-1.ubuntu24.04_amd64.deb` (Debian 13: o arquivo `.debian13`; o Debian 12 usa o AppImage) ou `sudo pacman -U ./astrofind-1.2.1-1-x86_64.pkg.tar.zst`. Use o arquivo feito para o seu sistema. Veja [Instalação](https://github.com/petrinhu/astrofind/wiki/Installation). |
| O terminal fala do "platform plugin" do Qt (por exemplo `xcb` ou `wayland`) | Faltam bibliotecas gráficas usadas pelo Qt. | Instale as bibliotecas de OpenGL e de teclado da sua distribuição (no Fedora: `sudo dnf install mesa-libGL libxkbcommon`; no Ubuntu/Debian: `sudo apt install libgl1 libxkbcommon0`). |
| A janela abre num lugar estranho, fora da tela, ou sem alguns painéis | O layout de janela salvo não serve para a tela atual. | Use **Janela → Organizar Automaticamente** e os itens **Janela → Exibir …** para mostrar os painéis de novo. Se não resolver, restaure as configurações (veja [abaixo](#how-to-reset-the-settings--como-restaurar-as-configurações)). |
| O AstroFind fecha logo depois de abrir | Pode ser um arquivo de configurações ruim ou um bug. | Restaure as configurações como explicado abaixo. Se continuar fechando, relate um bug com a saída do terminal. |

---

## Files won't open / Arquivos não abrem

🟢 Beginner / Iniciante · 🟡 Intermediate / Intermediário

🇬🇧 **English**
When a file fails, AstroFind shows a "Load Error" window with "Could not load:" followed by the
file name and the reason, and the Log panel shows "Failed to load …: …". The reason is one of
the messages below. More about formats: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

| Message (the reason) | Cause | Fix |
|---|---|---|
| "Unsupported image format '.…' — supported: fits, ser, xisf, tiff, tif, png, bmp, jpg, PDS3 (img/lbl), PDS4 (xml), DSLR RAW" | The file extension is not one AstroFind knows. | Check the file is really an image. Convert it to FITS with another program if needed. |
| "Cannot load image (unsupported or corrupt): …" | A TIFF/PNG/BMP/JPEG file could not be decoded. | Open it in an image viewer to check that it is not damaged. Save it again as 16-bit TIFF or PNG, or as FITS. |
| "Cannot open FITS file: … (…)" | cfitsio (the FITS library) could not open the file; the text in parentheses gives its reason. | Check the file is complete (download it again) and that you have permission to read it. |
| "FITS file has no 2D image: …" | The FITS file has no picture, only tables or a header. | Use a FITS file with an image. Star tables can be loaded with **Astrometry Tools → Import Detected Stars (DAOPHOT/SExtractor)…**. |
| "Declared image size (… px) in '…' exceeds the …-byte file on disk (lying/corrupt header)" | The header promises more pixels than the file contains, usually because the download was cut off. | Download or copy the file again. |
| "PDS label declares … bytes of image data but '…' has only … (lying/corrupt label)" | Same problem for a PDS file: the image file is shorter than its label says. | Download the `.img` again. Make sure the `.lbl` belongs to that `.img`. |
| "Not a PDS3 file (no attached or detached label): …" | The `.img` has no label inside and there is no `.lbl` with the same name next to it. | Put the matching `.lbl` file in the same folder, with the same base name. |
| "Not a PDS4 observational product: …" | The `.xml` file is not a PDS4 image label. | Select the PDS4 label of the image product. |
| "PDS4 label has no Array_2D_Image: …" | The PDS4 product holds no 2-D image (for example a table or a spectrum). | Choose a product that contains an image. |
| "VAX floating point PDS3 images are not supported" or "Unsupported PDS3 SAMPLE_TYPE '…'" | The PDS3 image uses a number format AstroFind does not read. | Look for another version of the product (many archives also offer FITS or PDS4). |
| "Cannot decode RAW file '…': …" or "Unsupported RAW layout in '…'" | LibRaw could not read this camera file. | Check that the file is complete. Update LibRaw, or convert the RAW to FITS with another program. |
| "File too small to be a valid SER file: …" or "SER file truncated (…)" | The SER video is incomplete. | Copy or record the file again. |
| "Not an XISF 1.0 file: …" or "XISF: unsupported sampleFormat '…' in: …" | The XISF file uses a variant AstroFind does not read. | Save it again from PixInsight as FITS or as a simple XISF. |
| "Session Limit Reached" | The session already has 20 images. The message says to "increase the limit in Settings", but that field does not exist. | See "How many images can I load?" in the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ). |
| You drop a TIFF, PNG, RAW or PDS file onto the window and nothing happens | Drag and drop accepts only `.fits`, `.fit`, `.fts`, ZIP and the other archive types. | Use **File → Import Images…** (`Ctrl+L`). |
| **File → Open Recent…** does not load all files of the folder | This list only looks for FITS, SER, XISF, TIFF, PNG, PDS3 and the common RAW types. It skips archives, BMP, JPEG and PDS4 `.xml`. | Use **File → Import Images…**. |
| A "Spectrum" window opens instead of an image | The FITS file is a 1-D spectrum (NAXIS=1), not an image. | This is expected; spectra cannot be used for astrometry. |
| **Image Tools → View FITS Header…** does nothing | This item reads FITS headers only. | For other formats the information is in the Log panel line "Loaded: …" and in **Image Tools → Edit Image Settings…**. |
| A dark or flat frame does not load ("Cannot load: …") | Dark and flat frames must be FITS files. | Convert the calibration frames to FITS. |
| **File → Open Project…** asks "Localizar…" / "Pular" / "Cancelar tudo" | An image of the project was moved or deleted (projects store the path, not the pixels). | Click "Localizar…" and point to the file. If nothing can be found, the log says "Projeto: nenhuma imagem pôde ser carregada." |

> ⚠️ **Watch out:** **File → Save FITS Copy…** copies the original file and adds the WCS. For a
> file that is not FITS (RAW, PDS, TIFF…), the copy is still in the original format, only with a
> `.fits` name, and the Log panel shows "WCS write warning: …".

🇧🇷 **Português**
Quando um arquivo falha, o AstroFind mostra uma janela "Erro ao carregar" com "Não foi possível
carregar:" seguido do nome do arquivo e do motivo, e o painel de Log mostra "Falha ao carregar …:
…". O motivo é uma das mensagens abaixo. Mais sobre formatos: [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats).

| Mensagem (o motivo) | Causa | Solução |
|---|---|---|
| "Unsupported image format '.…' — supported: fits, ser, xisf, tiff, tif, png, bmp, jpg, PDS3 (img/lbl), PDS4 (xml), DSLR RAW" (aparece em inglês) | A extensão do arquivo não é uma que o AstroFind conhece. | Confira se o arquivo é mesmo uma imagem. Converta para FITS com outro programa, se preciso. |
| "Não foi possível carregar a imagem (formato não suportado ou corrompida): …" | Um arquivo TIFF/PNG/BMP/JPEG não pôde ser decodificado. | Abra-o num visualizador de imagens para ver se não está danificado. Salve de novo como TIFF ou PNG de 16 bits, ou como FITS. |
| "Não é possível abrir arquivo FITS: … (…)" | O cfitsio (a biblioteca de FITS) não conseguiu abrir o arquivo; o texto entre parênteses dá o motivo. | Confira se o arquivo está completo (baixe de novo) e se você tem permissão para lê-lo. |
| "Arquivo FITS não contém imagem 2D: …" | O arquivo FITS não tem imagem, só tabelas ou cabeçalho. | Use um FITS com imagem. Tabelas de estrelas podem ser carregadas em **Ferramentas de Astrometria → Importar Estrelas Detectadas (DAOPHOT/SExtractor)…**. |
| "Declared image size (… px) in '…' exceeds the …-byte file on disk (lying/corrupt header)" (em inglês) | O cabeçalho promete mais pixels do que o arquivo tem, normalmente porque o download foi interrompido. | Baixe ou copie o arquivo de novo. |
| "PDS label declares … bytes of image data but '…' has only … (lying/corrupt label)" (em inglês) | O mesmo problema num arquivo PDS: o arquivo de imagem é menor do que o rótulo diz. | Baixe o `.img` de novo. Confira se o `.lbl` é mesmo daquele `.img`. |
| "Not a PDS3 file (no attached or detached label): …" (em inglês) | O `.img` não tem rótulo dentro e não há um `.lbl` com o mesmo nome ao lado. | Coloque o `.lbl` correspondente na mesma pasta, com o mesmo nome base. |
| "Not a PDS4 observational product: …" (em inglês) | O arquivo `.xml` não é um rótulo de imagem PDS4. | Selecione o rótulo PDS4 do produto de imagem. |
| "PDS4 label has no Array_2D_Image: …" (em inglês) | O produto PDS4 não tem imagem 2-D (por exemplo, é uma tabela ou um espectro). | Escolha um produto que contenha imagem. |
| "VAX floating point PDS3 images are not supported" ou "Unsupported PDS3 SAMPLE_TYPE '…'" (em inglês) | A imagem PDS3 usa um formato numérico que o AstroFind não lê. | Procure outra versão do produto (muitos arquivos também oferecem FITS ou PDS4). |
| "Cannot decode RAW file '…': …" ou "Unsupported RAW layout in '…'" (em inglês) | A LibRaw não conseguiu ler esse arquivo de câmera. | Confira se o arquivo está completo. Atualize a LibRaw, ou converta o RAW para FITS com outro programa. |
| "Arquivo muito pequeno para ser um arquivo SER válido: …" ou "SER file truncated (…)" | O vídeo SER está incompleto. | Copie ou grave o arquivo de novo. |
| "Não é um arquivo XISF 1.0: …" ou "XISF: sampleFormat '…' não suportado em: …" | O arquivo XISF usa uma variante que o AstroFind não lê. | Salve de novo no PixInsight como FITS ou como XISF simples. |
| "Limite de Sessão Atingido" | A sessão já tem 20 imagens. A mensagem manda "aumente o limite nas Configurações", mas esse campo não existe. | Veja "Quantas imagens posso carregar?" no [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ). |
| Você solta um TIFF, PNG, RAW ou PDS na janela e nada acontece | Arrastar e soltar só aceita `.fits`, `.fit`, `.fts`, ZIP e os outros tipos compactados. | Use **Arquivo → Importar Imagens…** (`Ctrl+L`). |
| **Arquivo → Abrir Recente…** não carrega todos os arquivos da pasta | Essa lista só procura FITS, SER, XISF, TIFF, PNG, PDS3 e os tipos RAW mais comuns. Ela ignora arquivos compactados, BMP, JPEG e `.xml` PDS4. | Use **Arquivo → Importar Imagens…**. |
| Abre uma janela "Espectro" em vez de uma imagem | O FITS é um espectro 1-D (NAXIS=1), não uma imagem. | É o esperado; espectros não servem para astrometria. |
| **Ferramentas de Imagem → Ver Cabeçalho FITS…** não faz nada | Esse item só lê cabeçalhos FITS. | Para outros formatos, a informação está na linha "Carregado: …" do painel de Log e em **Ferramentas de Imagem → Editar Configurações da Imagem…**. |
| Um dark ou flat não carrega ("Não é possível carregar: …") | Darks e flats precisam ser arquivos FITS. | Converta os quadros de calibração para FITS. |
| **Arquivo → Abrir Projeto…** pergunta "Localizar…" / "Pular" / "Cancelar tudo" | Uma imagem do projeto foi movida ou apagada (o projeto guarda o caminho, não os pixels). | Clique em "Localizar…" e aponte o arquivo. Se nada for encontrado, o log diz "Projeto: nenhuma imagem pôde ser carregada." |

> ⚠️ **Atenção:** **Arquivo → Salvar Cópia em FITS…** copia o arquivo original e acrescenta o WCS.
> Para um arquivo que não é FITS (RAW, PDS, TIFF…), a cópia continua no formato original, só com
> nome `.fits`, e o painel de Log mostra "Aviso de escrita WCS: …".

---

## Plate solving fails / O plate solving falha

🟡 Intermediate / Intermediário

🇬🇧 **English**
Plate solving (finding which part of the sky an image shows) runs inside
**Astrometry Tools → Run Data Reduction…** (`Ctrl+A`). Each failed image gets a Log line
"Plate solving falhou: …" with one of the reasons below. The end summary looks like
"Redução concluída: … resolvidas, … falhou".

| Message | Cause | Fix |
|---|---|---|
| Window "ASTAP não configurado" | The backend is ASTAP but "Executável ASTAP:" is empty. | Fill it in **File → Settings…** → **Connections**. |
| "Não foi possível iniciar o ASTAP. Verifique o caminho: …" | The path does not point to a working ASTAP program. | Click "…" next to "Executável ASTAP:" and select the ASTAP program file. Check it runs from a terminal. |
| "ASTAP: estrelas insuficientes para resolver a placa" | ASTAP found too few stars. | Use a longer exposure, check the focus, or see [No stars detected](#no-stars-detected--nenhuma-estrela-detectada). |
| "ASTAP: nenhuma solução de placa encontrada" | ASTAP could not match the stars. Common reasons: no ASTAP star database installed, a database that does not cover your field size, or a wrong position hint in the header. | Install an ASTAP star database suitable for your field of view (see [hnsky.org](https://www.hnsky.org/astap.htm)). Check the RA/Dec in **Image Tools → Edit Image Settings…**. |
| "ASTAP: arquivo de solução não encontrado: …" | ASTAP writes a `.wcs` file next to the image. It could not, or it did not solve. | Make sure the image folder is writable (copy the images to a folder in your home directory). |
| "ASTAP falhou (código …)" or "ASTAP encerrado de forma inesperada" | ASTAP stopped with an error. | Run ASTAP alone on the same file to see its message. |
| "…: chave API ausente — plate solving ignorado." or "Astrometry.net API key is not set" | No astrometry.net API key. | Get a free key at nova.astrometry.net and paste it in **Settings → Connections → API Key:**. |
| "Login failed: …" | The key is wrong or the server rejected it. | Copy the key again from your nova.astrometry.net account (no spaces). |
| "Upload rejected by astrometry.net" | The server did not accept the file. | The solver receives your original file. If it is a format the server does not read (for example a camera RAW or a PDS file), convert it to FITS with another program and load that FITS. |
| "Timeout: no job started after 5 min", "Timeout: plate solving took too long", "Timeout waiting for astrometry job" or "Timeout polling job status" | The server is busy or slow. | Try later, raise "Timeout:" in **Settings → Connections** (60–600 s), or switch to ASTAP. |
| "Astrometry.net: no plate solution found" | The server could not match the stars. | Check that the image has enough stars and is in focus. |
| "Todas as submissões falharam. Verifique a chave API e a conexão com a internet." | Every astrometry.net submission failed. If ASTAP is configured, a "Dica:" line suggests switching to it. | Check your internet connection and the key, or switch "Backend:" to "ASTAP (local, offline)". |
| Window "Localização não configurada" before solving | Your site is 0°, 0°; positions would be computed as if seen from the centre of the Earth. | Answer No and set your location in **Settings → Observer** (or use "Automático (do FITS)"). |

> 💡 **Tip:** images that already have a WCS in the header are skipped ("WCS pré-existente —
> plate solving ignorado."). This is normal.

🇧🇷 **Português**
O plate solving (descobrir qual parte do céu a imagem mostra) roda dentro de
**Ferramentas de Astrometria → Executar Redução de Dados…** (`Ctrl+A`). Cada imagem que falha ganha uma linha no Log
"Plate solving falhou: …" com um dos motivos abaixo. O resumo final se parece com
"Redução concluída: … resolvidas, … falhou".

| Mensagem | Causa | Solução |
|---|---|---|
| Janela "ASTAP não configurado" | O backend é ASTAP, mas "Executável ASTAP:" está vazio. | Preencha em **Arquivo → Configurações…** → **Conexões**. |
| "Não foi possível iniciar o ASTAP. Verifique o caminho: …" | O caminho não aponta para um programa ASTAP que funcione. | Clique em "…" ao lado de "Executável ASTAP:" e selecione o programa ASTAP. Confira se ele roda no terminal. |
| "ASTAP: estrelas insuficientes para resolver a placa" | O ASTAP encontrou poucas estrelas. | Use uma exposição mais longa, confira o foco, ou veja [Nenhuma estrela detectada](#no-stars-detected--nenhuma-estrela-detectada). |
| "ASTAP: nenhuma solução de placa encontrada" | O ASTAP não conseguiu casar as estrelas. Motivos comuns: nenhum banco de estrelas do ASTAP instalado, um banco que não serve para o tamanho do seu campo, ou uma posição errada no cabeçalho. | Instale um banco de estrelas do ASTAP adequado ao seu campo de visão (veja [hnsky.org](https://www.hnsky.org/astap.htm)). Confira AR/Dec em **Ferramentas de Imagem → Editar Configurações da Imagem…**. |
| "ASTAP: arquivo de solução não encontrado: …" | O ASTAP grava um arquivo `.wcs` ao lado da imagem. Ele não conseguiu, ou não resolveu. | Garanta que a pasta das imagens permite gravação (copie as imagens para uma pasta dentro da sua pasta pessoal). |
| "ASTAP falhou (código …)" ou "ASTAP encerrado de forma inesperada" | O ASTAP parou com erro. | Rode o ASTAP sozinho no mesmo arquivo para ver a mensagem dele. |
| "…: chave API ausente — plate solving ignorado." ou "Chave de API do Astrometry.net não configurada" | Falta a chave de API do astrometry.net. | Crie uma chave gratuita em nova.astrometry.net e cole em **Configurações → Conexões → Chave de API:**. |
| "Falha no login: …" | A chave está errada ou o servidor a recusou. | Copie a chave de novo da sua conta no nova.astrometry.net (sem espaços). |
| "Upload rejeitado pelo astrometry.net" | O servidor não aceitou o arquivo. | O solver recebe o seu arquivo original. Se for um formato que o servidor não lê (por exemplo RAW de câmera ou PDS), converta para FITS com outro programa e carregue esse FITS. |
| "Tempo esgotado: nenhum trabalho iniciado após 5 min", "Tempo esgotado: solução de campo demorou muito", "Tempo esgotado aguardando trabalho de astrometria" ou "Tempo esgotado ao verificar status do trabalho" | O servidor está ocupado ou lento. | Tente mais tarde, aumente "Tempo limite:" em **Configurações → Conexões** (60–600 s), ou mude para o ASTAP. |
| "Astrometry.net: nenhuma solução de campo encontrada" | O servidor não conseguiu casar as estrelas. | Confira se a imagem tem estrelas suficientes e está em foco. |
| "Todas as submissões falharam. Verifique a chave API e a conexão com a internet." | Todas as submissões ao astrometry.net falharam. Se o ASTAP estiver configurado, uma linha "Dica:" sugere trocar para ele. | Confira a internet e a chave, ou mude "Backend:" para "ASTAP (local, offline)". |
| Janela "Localização não configurada" antes da solução | O seu local está em 0°, 0°; as posições seriam calculadas como se vistas do centro da Terra. | Responda Não e configure a localização em **Configurações → Observador** (ou use "Automático (do FITS)"). |

> 💡 **Dica:** imagens que já têm WCS no cabeçalho são puladas ("WCS pré-existente — plate
> solving ignorado."). Isso é normal.

---

## Catalog, VizieR and known objects / Catálogo, VizieR e objetos conhecidos

🟡 Intermediate / Intermediário

🇬🇧 **English**
These messages come from **Utilities → Show Known Objects** (`Ctrl+K`), which downloads reference
stars (VizieR or a local file) and known asteroids (SkyBoT or MPCORB.DAT).

| Problem / message | Cause | Fix |
|---|---|---|
| "Catalog download failed: …" | VizieR could not be reached, or it returned an error. | Check your internet connection and try again later. Offline, use a local catalog (**Settings → Connections** → "Source:" → "Local FITS BINTABLE"). |
| "No catalog stars found in field" | No catalog star in the field fits the magnitude limits, or the field centre is wrong. | Check "Catalog mag (faint):" (default 16) and "Catalog mag (bright):" (default 10) in **Settings → Detection**. Make sure Run Data Reduction solved image 1: without a solution the search uses the header position and a 0.5° radius. |
| You changed "VizieR mirror:" and nothing changed | The field needs the full address of a VizieR TAP service (`https://…/TAPVizieR/tap/sync`). A bare host name, or `http://` for anything other than `localhost`, is rejected, and AstroFind keeps using the default `https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync`. (In 1.1.0 and earlier the field showed `vizier.cfa.harvard.edu`, which was always rejected; version 1.2.1 replaces that old value with the default.) | Leave the default. If you type another server, give its complete `https://` TAP address. |
| "No local catalog file configured. Set it in Settings → Connections." | "Source:" is "Local FITS BINTABLE" but "Local catalog:" is empty. | Choose the file with "Browse…". |
| "Local catalog error: …" or "No RA/Dec columns found in local catalog" | The local file is not a FITS table with RA/Dec columns. | Use a FITS BINTABLE export (for example from VizieR) that includes RA and Dec columns. |
| "SkyBoT query failed: …" | The IMCCE SkyBoT service could not be reached. | Download MPCORB.DAT once with **Internet → Download MPCOrb Database**; AstroFind then uses it when SkyBoT is unreachable. |
| "MPCORB.DAT not found. Use Internet → Download MPCOrb Database." | The offline asteroid file is missing. | Use **Internet → Download MPCOrb Database** (about 200 MB). The file path is shown in **Settings → Connections** → "MPCORB.DAT:". **Internet → Update MPCOrb Database** needs the file to exist first. |
| No known objects at all, and no SkyBoT line in the log | Known objects are only searched when the image has a valid date (Julian Date above 2400000). | Check the date in **Image Tools → Edit Image Settings…** ("Julian Date:"). |
| High "WCS RMS" value in the log (well over 1") | Poor match between image and catalog: wrong solution, distortion, or few matched stars. | Check focus and plate solution; try the other catalog. |

🇧🇷 **Português**
Estas mensagens vêm de **Utilitários → Mostrar Objetos Conhecidos** (`Ctrl+K`), que baixa
estrelas de referência (VizieR ou um arquivo local) e asteroides conhecidos (SkyBoT ou
MPCORB.DAT).

| Problema / mensagem | Causa | Solução |
|---|---|---|
| "Falha no download do catálogo: …" | O VizieR não pôde ser acessado, ou devolveu um erro. | Confira a internet e tente mais tarde. Sem internet, use um catálogo local (**Configurações → Conexões** → "Fonte:" → "FITS BINTABLE local"). |
| "Nenhuma estrela do catálogo encontrada no campo" | Nenhuma estrela do catálogo no campo cabe nos limites de magnitude, ou o centro do campo está errado. | Confira "Magnitude do catálogo (fraca):" (padrão 16) e "Magnitude do catálogo (brilhante):" (padrão 10) em **Configurações → Detecção**. Garanta que a Redução de Dados resolveu a imagem 1: sem solução, a busca usa a posição do cabeçalho e um raio de 0,5°. |
| Você mudou "Espelho VizieR:" e nada mudou | O campo precisa do endereço completo de um serviço TAP do VizieR (`https://…/TAPVizieR/tap/sync`). Só o nome do host, ou `http://` para algo que não seja `localhost`, é recusado, e o AstroFind continua usando o padrão `https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync`. (Na 1.1.0 e anteriores o campo mostrava `vizier.cfa.harvard.edu`, sempre recusado; a versão 1.2.0 troca esse valor antigo pelo padrão.) | Deixe o padrão. Se digitar outro servidor, informe o endereço TAP completo com `https://`. |
| "Nenhum arquivo de catálogo local configurado. Defina em Configurações → Conexões." | "Fonte:" está em "FITS BINTABLE local", mas "Catálogo local:" está vazio. | Escolha o arquivo com "Procurar…". |
| "Erro no catálogo local: …" ou "Nenhuma coluna AR/Dec encontrada no catálogo local" | O arquivo local não é uma tabela FITS com colunas AR/Dec. | Use uma exportação FITS BINTABLE (por exemplo do VizieR) que tenha colunas de AR e Dec. |
| "Consulta ao SkyBoT falhou: …" | O serviço SkyBoT do IMCCE não pôde ser acessado. | Baixe o MPCORB.DAT uma vez em **Internet → Baixar Banco MPCOrb**; o AstroFind passa a usá-lo quando o SkyBoT não responde. |
| "MPCORB.DAT não encontrado. Use Internet → Baixar Banco MPCOrb." | Falta o arquivo offline de asteroides. | Use **Internet → Baixar Banco MPCOrb** (cerca de 200 MB). O caminho do arquivo aparece em **Configurações → Conexões** → "MPCORB.DAT:". **Internet → Atualizar Banco MPCOrb** precisa que o arquivo já exista. |
| Nenhum objeto conhecido, e nenhuma linha do SkyBoT no log | Objetos conhecidos só são buscados quando a imagem tem data válida (Data Juliana acima de 2400000). | Confira a data em **Ferramentas de Imagem → Editar Configurações da Imagem…** ("Data Juliana:"). |
| Valor alto de "WCS RMS" no log (bem acima de 1") | Casamento ruim entre imagem e catálogo: solução errada, distorção, ou poucas estrelas casadas. | Confira o foco e a solução de placa; experimente o outro catálogo. |

---

## No stars detected / Nenhuma estrela detectada

🟡 Intermediate / Intermediário

🇬🇧 **English**
During Run Data Reduction the Log panel shows "Image …: … stars found" for each image. If the number
is 0 or very small:

| Cause | Fix |
|---|---|
| The detection threshold is too high. | Lower "Detection threshold:" in **Settings → Detection** (default 4.0 σ; σ is the noise level). Try 3.0. |
| A wrong pixel scale in **Settings → Camera**. The value is in **arcseconds per pixel** (″/px, unbinned). A value typed in degrees (as the old "°/px" label of 1.1.0 and earlier suggested) makes the "Minimum FWHM:" filter (in arcseconds) throw away every star. | Leave "Pixel Scale X:" and "Pixel Scale Y:" at 0 ("Auto") unless you know the scale in ″/px. |
| Strong gradients (light pollution, moonlight) hide faint stars. | Tick "Subtrair modelo de fundo antes da detecção" in **Settings → Detection**. |
| The dark or flat frame does not match the image size. The log shows "Image …: dark frame size mismatch — skipped" (or flat field). | Use calibration frames of the same size and binning as the images. |
| The image is out of focus or trailed. | Check the image; long trails are drawn in orange and may not count as stars. |
| Utilities → Growth Curve… says "No stars detected in this image.\nRun plate solving or star detection first." | Run **Astrometry Tools → Run Data Reduction…** first. |

> 💡 **Tip:** at most 500 stars (the brightest) are kept per image. That is enough for plate
> solving.

🇧🇷 **Português**
Durante a Redução de Dados, o painel de Log mostra "Imagem …: … estrelas encontradas" para cada
imagem. Se o número é 0 ou muito pequeno:

| Causa | Solução |
|---|---|
| O limiar de detecção está alto demais. | Diminua "Limiar de detecção:" em **Configurações → Detecção** (padrão 4,0 σ; σ é o nível de ruído). Tente 3,0. |
| Uma escala de pixel errada em **Configurações → Câmera**. O valor é em **segundos de arco por pixel** (″/px, sem binning). Um valor digitado em graus (como sugeria o antigo rótulo "°/px" da 1.1.0 e anteriores) faz o filtro "FWHM mínimo:" (em segundos de arco) jogar fora todas as estrelas. | Deixe "Escala de pixel X:" e "Escala de pixel Y:" em 0 ("Automático"), a menos que você saiba a escala em ″/px. |
| Gradientes fortes (poluição luminosa, luar) escondem estrelas fracas. | Marque "Subtrair modelo de fundo antes da detecção" em **Configurações → Detecção**. |
| O dark ou o flat não tem o tamanho da imagem. O log mostra "Imagem …: tamanho do dark frame incompatível — ignorado" (ou do flat field). | Use quadros de calibração com o mesmo tamanho e binning das imagens. |
| A imagem está fora de foco ou com rastro. | Confira a imagem; rastros longos são desenhados em laranja e podem não contar como estrelas. |
| Utilitários → Curva de Crescimento… diz "Nenhuma estrela detectada nesta imagem.\nExecute o plate solving ou a detecção de estrelas primeiro." | Rode **Ferramentas de Astrometria → Executar Redução de Dados…** antes. |

> 💡 **Dica:** no máximo 500 estrelas (as mais brilhantes) são mantidas por imagem. É suficiente
> para o plate solving.

---

## Blink, moving objects and measuring / Blink, objetos em movimento e medição

🟢 Beginner / Iniciante · 🟡 Intermediate / Intermediário

🇬🇧 **English**

| Problem / message | Cause | Fix |
|---|---|---|
| "Load at least 2 images to use blink mode." | Blink needs two or more images. | Load at least 2 images of the same field (3–5 is better). |
| The blink view shows only 4 thumbnails | The blink view has 4 slots. | Normal. |
| "Run Data Reduction first to detect stars in the images." (Detect Moving Objects) | No stars were detected yet. | Run **Astrometry Tools → Run Data Reduction…** (`Ctrl+A`) first. |
| "No moving objects detected across … images." | Nothing matched the search rules. By default an object must appear in at least 3 images, move steadily (within 2 px), and have SNR (signal-to-noise ratio) of at least 5. | Use at least 3 images. Lower "MOD min. SNR:" in **Settings → Detection**. Blink the images yourself: very fast objects may appear as trails. |
| Clicking on the image measures nothing | The **Select** tool is active. | Press `M` (Measure Object) or `A` (**Edit → Aperture Tool**) before clicking. |
| Log shows "Centroid failed — no source found at click position" | No star-like dot was found where you clicked. | Zoom in (mouse wheel) and click exactly on the dot. |
| A window "Blink em execução" asks "Parar e Medir" / "Continuar Blink" | You clicked while the blink was playing. | Choose "Parar e Medir" to measure. Pause with Space first to avoid the question. |
| Log shows "PSF elongation=… — check tracking, focus, or coma" | The star images are stretched (elongated). | Check the telescope tracking and focus. Measurements still work, but may be less precise. |
| Log shows "Photometry: no catalog stars matched — using instrumental mag" | No catalog star near the object could calibrate the brightness. | Run Show Known Objects (`Ctrl+K`) before measuring. The position is still valid. |

🇧🇷 **Português**

| Problema / mensagem | Causa | Solução |
|---|---|---|
| "Carregue pelo menos 2 imagens para usar o modo piscar." | O blink precisa de duas ou mais imagens. | Carregue pelo menos 2 imagens do mesmo campo (3–5 é melhor). |
| A visão de piscar mostra só 4 miniaturas | A visão de piscar tem 4 espaços. | Normal. |
| "Execute a Redução de Dados primeiro para detectar estrelas nas imagens." (Detectar Objetos em Movimento) | Ainda não há estrelas detectadas. | Rode **Ferramentas de Astrometria → Executar Redução de Dados…** (`Ctrl+A`) antes. |
| "Nenhum objeto em movimento detectado em … imagens." | Nada atendeu às regras da busca. Por padrão, um objeto precisa aparecer em pelo menos 3 imagens, andar de forma regular (dentro de 2 px) e ter SNR (razão sinal-ruído) de pelo menos 5. | Use pelo menos 3 imagens. Diminua "MOD SNR mín.:" em **Configurações → Detecção**. Pisque as imagens você mesmo: objetos muito rápidos podem aparecer como rastros. |
| Clicar na imagem não mede nada | A ferramenta **Seleção** está ativa. | Aperte `M` (Medir Objeto) ou `A` (**Editar → Ferramenta Abertura**) antes de clicar. |
| O Log mostra "Centroide falhou — nenhuma fonte encontrada na posição clicada" | Nenhum ponto parecido com estrela foi achado onde você clicou. | Aproxime o zoom (roda do mouse) e clique exatamente no ponto. |
| Uma janela "Blink em execução" pergunta "Parar e Medir" / "Continuar Blink" | Você clicou com o blink rodando. | Escolha "Parar e Medir" para medir. Pause antes com a barra de espaço para evitar a pergunta. |
| O Log mostra "Elongação PSF=… — verifique rastreamento, foco ou coma" | As imagens das estrelas estão esticadas (alongadas). | Confira o acompanhamento do telescópio e o foco. A medição funciona, mas pode ser menos precisa. |
| O Log mostra "Fotometria: nenhuma estrela do catálogo correspondida — usando magnitude instrumental" | Nenhuma estrela de catálogo perto do objeto pôde calibrar o brilho. | Rode Mostrar Objetos Conhecidos (`Ctrl+K`) antes de medir. A posição continua válida. |

---

## Report and submission / Relatório e envio

🟡 Intermediate / Intermediário

🇬🇧 **English**

| Problem / message | Cause | Fix |
|---|---|---|
| **File → View ADES Report File** is greyed out | It is enabled only after Run Data Reduction. | Run **Astrometry Tools → Run Data Reduction…** first. |
| "No observations yet.\n\nUse the Measure tool (M) to click on moving objects first." | No accepted measurement. | Measure the object and click "✓ Aceitar" in the Verification window. |
| The summary says "Station: (not set)" and the report uses `XXX` | "MPC Station Code:" is empty. | Fill it in **Settings → Observer**, or keep `XXX` if you have no code yet (see the [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ)). |
| Window "MPC Submission Failed" with "HTTP submission failed: …" | The MPC server returned an error or could not be reached. | Answer Yes to send by e-mail instead, or save the report and try later. |
| Window "MPC Submission — No Response" | The MPC did not answer within the time limit. The report **may or may not** have arrived. | Do not send it again straight away. Check your MPC account or e-mail first. |
| Window "Large Report" | The PSV text is over 2000 characters; some e-mail programs cut long messages. | Save the PSV with "Save…" and attach it to the e-mail by hand. |
| You can't find the saved report | There are two folders. **Settings → Connections → Report output folder:** is used for the automatic copy (`ades_report.xml` and `ades_report.psv`) when you open the report. The "Save to Reports Folder" button uses a different folder (your home folder by default), which cannot be changed in Settings. | Look in both places, or use "Save…" and choose the folder yourself. |
| "ADES auto-save failed: …" | The "Report output folder:" is not writable. | Choose another folder in **Settings → Connections**. |
| The report has no magnitudes | "Include magnitude in ADES report" is off in **Settings → Detection**. | Tick it. |
| You need the old MPC 80-column format | AstroFind writes only ADES (XML and PSV). | Send the ADES report; the MPC accepts it. |
| `obsTime` in the ADES report is about 1 minute (68 s) later than the real UTC mid-exposure, or grows each time you rerun Data Reduction (1.1.0 and earlier) | Bug AUD-CORR-15, fixed in version 1.2.0: Data Reduction added ΔT (default 68 s) and "Time Offset:" to every image time on each run, and the report labelled the result UTC. Now `obsTime` is the UTC mid-exposure plus Time Offset, applied once; ΔT is used only for the offline MPCORB search | Update. On 1.1.0: set **Settings → Camera → ΔT (TT − UTC):** and **Settings → Observer → Time Offset:** to 0, reload the images, run Data Reduction once. See [Settings](https://github.com/petrinhu/astrofind/wiki/Settings) |
| Log warning "Time Offset reset from … to 0 s: older versions filled it automatically with a wrong value…" | Shown once, on the first start of version 1.2.0, because 1.1.0 and earlier filled Time Offset with longitude ÷ 15 | Nothing to do. If your camera clock has a known error, type it again in **Settings → Observer → Time Offset:** (seconds) |
| The seconds in `obsTime` have an unexpected number of decimals | "Time Precision:" in **Settings → Observer** is the number of decimals of the seconds (0–3; 1 = tenths of a second). In 1.1.0 and earlier it was labelled in hours, with the same meaning. | Leave it at 1 unless you need another precision. |

🇧🇷 **Português**

| Problema / mensagem | Causa | Solução |
|---|---|---|
| **Arquivo → Ver Arquivo de Relatório ADES** está desativado | Ele só fica ativo depois da Redução de Dados. | Rode **Ferramentas de Astrometria → Executar Redução de Dados…** antes. |
| "Nenhuma observação ainda.\n\nUse a ferramenta Medir (M) para clicar nos objetos em movimento primeiro." | Nenhuma medição aceita. | Meça o objeto e clique em "✓ Aceitar" na janela de Verificação. |
| O resumo mostra a estação como "(não definido)" e o relatório usa `XXX` | "Código de estação MPC:" está vazio. | Preencha em **Configurações → Observador**, ou mantenha `XXX` se ainda não tem código (veja o [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ)). |
| Janela "Falha no envio ao MPC" com "Falha no envio via HTTP: …" | O servidor do MPC devolveu um erro ou não pôde ser acessado. | Responda Sim para enviar por e-mail, ou salve o relatório e tente mais tarde. |
| Janela "Submissão ao MPC — Sem Resposta" | O MPC não respondeu dentro do tempo limite. O relatório **pode ou não** ter chegado. | Não envie de novo na hora. Confira antes sua conta ou e-mail do MPC. |
| Janela "Relatório Grande" | O texto PSV passa de 2000 caracteres; alguns programas de e-mail cortam mensagens longas. | Salve o PSV com "Salvar…" e anexe no e-mail manualmente. |
| Você não encontra o relatório salvo | Existem duas pastas. **Configurações → Conexões → Pasta de saída de relatórios:** é usada para a cópia automática (`ades_report.xml` e `ades_report.psv`) quando você abre o relatório. O botão "Salvar na Pasta de Relatórios" usa outra pasta (a sua pasta pessoal por padrão), que não pode ser mudada nas Configurações. | Procure nos dois lugares, ou use "Salvar…" e escolha a pasta você mesmo. |
| "Falha ao salvar automaticamente o relatório ADES: …" | A "Pasta de saída de relatórios:" não permite gravação. | Escolha outra pasta em **Configurações → Conexões**. |
| O relatório não tem magnitudes | "Incluir magnitude no relatório ADES" está desmarcado em **Configurações → Detecção**. | Marque a opção. |
| Você precisa do antigo formato MPC de 80 colunas | O AstroFind só escreve ADES (XML e PSV). | Envie o relatório ADES; o MPC aceita. |
| O `obsTime` do relatório ADES sai cerca de 1 minuto (68 s) depois do meio da exposição em UTC, ou aumenta cada vez que você roda a Redução de Dados de novo (1.1.0 e anteriores) | Bug AUD-CORR-15, corrigido na versão 1.2.0: a Redução de Dados somava o ΔT (padrão 68 s) e o "Deslocamento de tempo:" ao horário de cada imagem a cada execução, e o relatório marcava o resultado como UTC. Agora o `obsTime` é o meio da exposição em UTC mais o Deslocamento de tempo, aplicado uma vez; o ΔT só é usado na busca offline pelo MPCORB | Atualize. Na 1.1.0: coloque **Configurações → Câmera → ΔT (TT − UTC):** e **Configurações → Observador → Deslocamento de tempo:** em 0, recarregue as imagens e rode a Redução de Dados uma vez. Veja [Configurações](https://github.com/petrinhu/astrofind/wiki/Settings) |
| Aviso no Registro "Time Offset redefinido de … para 0 s: versões anteriores o preenchiam automaticamente com um valor errado…" | Aparece uma vez, na primeira execução da versão 1.2.0, porque a 1.1.0 e anteriores preenchiam o Deslocamento de tempo com longitude ÷ 15 | Nada a fazer. Se o relógio da câmera tem um erro conhecido, digite de novo em **Configurações → Observador → Deslocamento de tempo:** (segundos) |
| Os segundos em `obsTime` têm um número inesperado de casas decimais | "Precisão de tempo:" em **Configurações → Observador** é o número de casas decimais dos segundos (0–3; 1 = décimos de segundo). Na 1.1.0 e anteriores ela aparecia em horas, com o mesmo significado. | Deixe em 1, a menos que precise de outra precisão. |

---

## The image looks black or white / A imagem parece preta ou branca

🟢 Beginner / Iniciante

🇬🇧 **English**

| Problem | Cause | Fix |
|---|---|---|
| Almost black image | Normal: the screen cannot show the full brightness range. | **Image Tools → Adjust Black Point and Contrast…** → **"⟳ Auto"**; try the "Asinh" or "Raiz Quadrada (Sqrt)" transfer function. |
| Almost white image | **Invert Colors** is on, or the range is set too low. | Turn off **Image Tools → Invert Colors** (`Ctrl+I`); click "⟳ Auto". |
| Only some images look right | The display settings were applied to one image. | Tick "Apply to all images in session" in Adjust Black Point and Contrast. |
| Magenta pixels | Those pixels have no value (NaN) in the file, for example outside the camera's area. | Nothing to fix; they are ignored. |
| Strange colours | A false-colour palette is active. | Set "Cor falsa (LUT):" to "Cinza (Grayscale)". |

These settings change only what you see, never the measurements.

🇧🇷 **Português**

| Problema | Causa | Solução |
|---|---|---|
| Imagem quase preta | Normal: a tela não consegue mostrar toda a faixa de brilho. | **Ferramentas de Imagem → Ajustar Ponto Preto e Contraste…** → **"⟳ Auto"**; experimente a função de transferência "Asinh" ou "Raiz Quadrada (Sqrt)". |
| Imagem quase branca | **Inverter Cores** está ligado, ou o intervalo está baixo demais. | Desligue **Ferramentas de Imagem → Inverter Cores** (`Ctrl+I`); clique em "⟳ Auto". |
| Só algumas imagens ficam boas | Os ajustes foram aplicados a uma imagem só. | Marque "Aplicar a todas as imagens da sessão" em Ajustar Ponto Preto e Contraste. |
| Pixels magenta | Esses pixels não têm valor (NaN) no arquivo, por exemplo fora da área da câmera. | Nada a corrigir; eles são ignorados. |
| Cores estranhas | Uma paleta de cor falsa está ativa. | Mude "Cor falsa (LUT):" para "Cinza (Grayscale)". |

Esses ajustes mudam só o que você vê, nunca as medições.

---

## Keyboard shortcuts don't work / Atalhos de teclado não funcionam

🟢 Beginner / Iniciante

🇬🇧 **English**

| Problem | Cause | Fix |
|---|---|---|
| `Ctrl+L` does nothing (1.1.0 and earlier) | In 1.1.0 `Ctrl+L` was assigned to both **File → Import Images…** and **Utilities → Light Curve…**. When one key has two actions, Qt may run neither. Version 1.2.0 moves Light Curve to `Ctrl+Shift+L`. | Update, or use the menu or the "Import Images (Ctrl+L)" toolbar button. |
| `Ctrl+Shift+T` does not change the theme (1.1.0 and earlier) | In 1.1.0 it was assigned twice (menu item and toolbar button). In version 1.2.0 it belongs only to the toolbar theme button and works. | Update, or click the moon/sun icon on the toolbar, or use **Window → Toggle Day/Night Mode**, or **Settings → Display → Theme:**. |
| `S`, `A`, `N` or `M` do nothing | A text field or another window has the keyboard focus. | Click on an image window first, then press the key. |
| `Ctrl+A` does not select all text | In AstroFind `Ctrl+A` starts **Run Data Reduction**. | Use the mouse to select text. |
| Space, Left/Right do not control the blink | These keys work only while the blink view has focus. | Click inside the blink view first. |
| Images disappeared after `Ctrl+W` or `Ctrl+R` | `Ctrl+W` (**File → Close Loaded Images**) and `Ctrl+R` (**File → Reset Session Files**) close all images **without asking** to save. | Reopen the images or your saved `.gus` project with **File → Open Project…**. Save often with `Ctrl+S`. |

Full shortcut list: [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference).

🇧🇷 **Português**

| Problema | Causa | Solução |
|---|---|---|
| `Ctrl+L` não faz nada (1.1.0 e anteriores) | Na 1.1.0 o `Ctrl+L` estava ligado a **Arquivo → Importar Imagens…** e a **Utilitários → Curva de Luz…**. Quando uma tecla tem duas ações, o Qt pode não rodar nenhuma. A versão 1.2.0 passa a Curva de Luz para `Ctrl+Shift+L`. | Atualize, ou use o menu ou o botão "Importar Imagens (Ctrl+L)" da barra de ferramentas. |
| `Ctrl+Shift+T` não muda o tema (1.1.0 e anteriores) | Na 1.1.0 o atalho estava em dois lugares (item de menu e botão da barra). Na versão 1.2.0 ele é só do botão de tema da barra e funciona. | Atualize, ou clique no ícone de lua/sol da barra, ou use **Janela → Alternar modo Dia/Noite**, ou **Configurações → Exibição → Tema:**. |
| `S`, `A`, `N` ou `M` não fazem nada | Um campo de texto ou outra janela está com o foco do teclado. | Clique numa janela de imagem antes e depois aperte a tecla. |
| `Ctrl+A` não seleciona todo o texto | No AstroFind, `Ctrl+A` inicia a **Redução de Dados**. | Selecione o texto com o mouse. |
| Espaço e Esquerda/Direita não controlam o blink | Essas teclas só funcionam quando a visão de piscar está com o foco. | Clique dentro da visão de piscar antes. |
| As imagens sumiram depois de `Ctrl+W` ou `Ctrl+R` | `Ctrl+W` (**Arquivo → Fechar Imagens**) e `Ctrl+R` (**Arquivo → Limpar Arquivos da Sessão**) fecham todas as imagens **sem perguntar** se quer salvar. | Abra as imagens de novo ou o seu projeto `.gus` com **Arquivo → Abrir Projeto…**. Salve sempre com `Ctrl+S`. |

Lista completa de atalhos: [Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference).

---

## Language: Portuguese text in the English interface / Idioma: texto em português na interface em inglês

🟢 Beginner / Iniciante

🇬🇧 **English**
Some labels and messages are written in Portuguese in the program itself, so they stay in
Portuguese with the English interface too. Examples: "Fluxo automático", "✓ Aceitar",
"Função de transferência:", and many Log
lines about plate solving and time. This is known and harmless. The other way round, a few new
messages have no Portuguese translation yet and appear in English in the Portuguese interface.

To change the language, use **File → Settings…** → **Display** → "Language:" and restart
AstroFind (the message "⟳ Restart required to apply language change." reminds you).

🇧🇷 **Português**
Alguns rótulos e mensagens foram escritos em português no próprio programa, então continuam em
português mesmo com a interface em inglês. Exemplos: "Fluxo automático", "✓ Aceitar",
"Função de transferência:", e muitas linhas
do Log sobre plate solving e horário. Isso é conhecido e não causa problema. Ao contrário,
algumas mensagens novas ainda não têm tradução e aparecem em inglês na interface em português.

Para mudar o idioma, use **Arquivo → Configurações…** → **Exibição** → "Idioma:" e reinicie o
AstroFind (a mensagem de reinício lembra você).

---

## Where is the log? / Onde fica o log?

🟢 Beginner / Iniciante

🇬🇧 **English**
AstroFind writes its messages to the **Log panel** ("Log" dock, at the bottom of the window).
Warnings are orange with "⚠", errors are red with "✗". The panel keeps the last 2000 lines.

- If the panel is hidden, use **Window → View Log Panel**. **File → View Log File** does the
  same thing: it only shows the panel. **There is no log file on disk.**
- **"Copy all"** copies every line to the clipboard (to paste into a bug report).
  **"Clear"** empties the panel.
- More technical messages go to the terminal when you start AstroFind from a terminal. To save
  them to a file:

  ```bash
  AstroFind 2>&1 | tee ~/astrofind-terminal.txt
  ```

  (This starts AstroFind and copies everything it prints into the file `astrofind-terminal.txt`
  in your home folder, while still showing it in the terminal.)

🇧🇷 **Português**
O AstroFind escreve suas mensagens no **painel de Log** (o painel "Registro", na parte de baixo da
janela). Avisos aparecem em laranja com "⚠", erros em vermelho com "✗". O painel guarda as
últimas 2000 linhas.

- Se o painel estiver escondido, use **Janela → Exibir painel de log**. **Arquivo → Ver Arquivo
  de Log** faz a mesma coisa: só mostra o painel. **Não existe arquivo de log no disco.**
- **"Copiar tudo"** copia todas as linhas para a área de transferência (para colar num relato de
  bug). **"Limpar"** esvazia o painel.
- Mensagens mais técnicas vão para o terminal quando você abre o AstroFind por um terminal. Para
  salvá-las num arquivo:

  ```bash
  AstroFind 2>&1 | tee ~/astrofind-terminal.txt
  ```

  (Isso abre o AstroFind e copia tudo o que ele imprime para o arquivo `astrofind-terminal.txt`
  na sua pasta pessoal, mostrando também no terminal.)

---

## How to reset the settings / Como restaurar as configurações

🟢 Beginner / Iniciante · 🔴 Advanced / Avançado

🇬🇧 **English**
**Normal way:**

1. Open **File → Settings…** (`Ctrl+,`).
2. Click **"Reset to Defaults"**.
3. Click **OK** to save.

This keeps your name, telescope and MPC code, but resets the location to manual 0°, 0° (set it
again, or Run Data Reduction will warn "Localização não configurada").

**Full reset** (for example when AstroFind does not start): close AstroFind and rename the
settings file, so AstroFind starts as if it were new:

```bash
mv ~/.config/AstroFind/AstroFind.conf ~/.config/AstroFind/AstroFind.conf.bak
```

(`mv` renames the file. Your old settings stay in `AstroFind.conf.bak`; to go back, rename it
again.) The Setup Wizard appears at the next start. An API key stored in the system keychain is
not removed by this.

> 💡 **Tip:** before experimenting, save all settings with **File → Exportar Configuração da
> Escola…**. **File → Importar Configuração da Escola…** restores them.

🇧🇷 **Português**
**Jeito normal:**

1. Abra **Arquivo → Configurações…** (`Ctrl+,`).
2. Clique em **"Restaurar padrões"**.
3. Clique em **OK** para salvar.

Isso mantém seu nome, telescópio e código MPC, mas volta a localização para manual 0°, 0°
(configure de novo, senão a Redução de Dados avisa "Localização não configurada").

**Restauração completa** (por exemplo, quando o AstroFind não abre): feche o AstroFind e renomeie
o arquivo de configurações, para ele abrir como se fosse novo:

```bash
mv ~/.config/AstroFind/AstroFind.conf ~/.config/AstroFind/AstroFind.conf.bak
```

(`mv` renomeia o arquivo. Suas configurações antigas ficam em `AstroFind.conf.bak`; para voltar,
renomeie de novo.) O Assistente de Configuração aparece na próxima abertura. Uma chave de API
guardada no chaveiro do sistema não é apagada por isso.

> 💡 **Dica:** antes de experimentar, salve todas as configurações com **Arquivo → Exportar
> Configuração da Escola…**. **Arquivo → Importar Configuração da Escola…** traz tudo de volta.

---

## Collecting information for a bug report / Juntando informações para relatar um bug

🟢 Beginner / Iniciante

🇬🇧 **English**
Collect these before you open an issue at <https://github.com/petrinhu/astrofind/issues>:

1. **Version:** **Help → About AstroFind…** shows "Version …" (for example 1.2.1). In a
   terminal, `AstroFind --version` prints the same number.
2. **Distribution:** run

   ```bash
   cat /etc/os-release
   ```

   (This prints the name and version of your Linux system. Copy the `PRETTY_NAME` line.)
3. **How you installed AstroFind:** package (RPM, DEB, Arch package or PKGBUILD), AppImage, or built from source. If
   built from source, say whether LibRaw and libarchive were found by `cmake`.
4. **The Log panel:** click **"Copy all"** and paste the text.
5. **Terminal output**, if the problem is a crash (see [Where is the log?](#where-is-the-log--onde-fica-o-log)).
6. **Steps:** what you did, what you expected, what happened.
7. **A sample file**, if you may share it. Remove private information first.

🇧🇷 **Português**
Junte isto antes de abrir uma issue em <https://github.com/petrinhu/astrofind/issues>:

1. **Versão:** **Ajuda → Sobre o AstroFind…** mostra "Versão …" (por exemplo 1.2.1). Num
   terminal, `AstroFind --version` mostra o mesmo número.
2. **Distribuição:** rode

   ```bash
   cat /etc/os-release
   ```

   (Isso mostra o nome e a versão do seu sistema Linux. Copie a linha `PRETTY_NAME`.)
3. **Como você instalou o AstroFind:** pacote (RPM, DEB, pacote ou PKGBUILD do Arch), AppImage ou compilado do
   código-fonte. Se compilou, diga se o `cmake` encontrou a LibRaw e a libarchive.
4. **O painel de Log:** clique em **"Copiar tudo"** e cole o texto.
5. **A saída do terminal**, se o problema for um travamento (veja [Onde fica o log?](#where-is-the-log--onde-fica-o-log)).
6. **Passos:** o que você fez, o que esperava, o que aconteceu.
7. **Um arquivo de exemplo**, se puder compartilhar. Tire antes as informações privadas.

---

**See also / Veja também:** [FAQ](https://github.com/petrinhu/astrofind/wiki/FAQ) · [Installation](https://github.com/petrinhu/astrofind/wiki/Installation) · [File-Formats](https://github.com/petrinhu/astrofind/wiki/File-Formats) · [Settings](https://github.com/petrinhu/astrofind/wiki/Settings) ·
[Menu-Reference](https://github.com/petrinhu/astrofind/wiki/Menu-Reference) · [Glossary](https://github.com/petrinhu/astrofind/wiki/Glossary)
