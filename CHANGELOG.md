# Changelog

> **Last reviewed / Última revisão:** 2026-09-24
> **Owner:** Petrus Silva Costa
> Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

All notable changes to AstroFind are documented here, most recent version first. /
Todas as mudanças notáveis do AstroFind estão documentadas aqui, versão mais recente
primeiro.

---

## [Unreleased]

### Fixed / Corrigido

- **Refraction on plate-solved positions / Refração em posições com solução astrométrica
  (AUD-CORR-7).** 🇬🇧 Bennett refraction is no longer added to RA/Dec that come from a
  catalog plate solution (the fit already absorbs it; applying it again double-corrected,
  up to ~1.7′ at 30° altitude). 🇧🇷 A refração de Bennett não é mais somada ao RA/Dec
  vindo de uma solução astrométrica de catálogo (o ajuste já a absorve; aplicá-la de novo
  corrigia duas vezes, até ~1,7′ a 30° de altura).
- **WCS pole cards and southern fields / Cartões de polo do WCS e campos austrais
  (AUD-CORR-10, AUD-CORR-11).** 🇬🇧 LONPOLE/LATPOLE (and PV1_3/PV1_4) are now read and
  used, as in WCSLIB; files without them behave as before. Sky→pixel for CAR/MER/GLS/AIT
  was 360° off on every southern field (catalog overlay misplaced); fixed.
  🇧🇷 LONPOLE/LATPOLE (e PV1_3/PV1_4) agora são lidos e usados, como no WCSLIB; arquivos
  sem eles se comportam como antes. Céu→pixel em CAR/MER/GLS/AIT saía 360° errado em todo
  campo austral (overlay de catálogo fora do lugar); corrigido.
- **Offline ephemeris / Efeméride offline (AUD-CORR-12).** 🇬🇧 MPCORB epochs were read
  half a day late, and the Earth position used the equinox of date with J2000 elements;
  against JPL Horizons the error drops from 6′ (Ceres, 2024) and 49′ ((433) Eros at
  0.18 AU) to under 1′. 🇧🇷 As épocas do MPCORB eram lidas meio dia depois e a posição da
  Terra usava o equinócio da data com elementos J2000; contra o JPL Horizons o erro cai de
  6′ (Ceres, 2024) e 49′ ((433) Eros a 0,18 UA) para menos de 1′.
- **Sub-second exposure times / Horário com fração de segundo (AUD-CORR-13).**
  🇬🇧 The Julian Date keeps the milliseconds of DATE-OBS and prefers MJD-OBS when present;
  XISF files now get a JD from DATE-OBS/MJD-OBS. 🇧🇷 O Dia Juliano mantém os milissegundos
  do DATE-OBS e prefere o MJD-OBS quando existe; arquivos XISF passam a ter JD a partir do
  DATE-OBS/MJD-OBS.
- **TAR hardlink entries in archive extraction / Entradas hardlink de TAR na extração de
  arquivos compactados (twin of AUD-INPUT-4).** 🇬🇧 A TAR entry stored as a hardlink
  reports the same filetype as a plain file to libarchive, so it slipped past the
  symlink/FIFO/device filter; its (unflattened) link target could point outside the
  extraction folder, letting a hostile archive create a hard link to an arbitrary file
  on disk under an innocuous name (e.g. "*.fits"). Hardlink entries are now rejected the
  same way symlinks/FIFOs/devices already were. 🇧🇷 Uma entrada de TAR armazenada como
  hardlink reporta o mesmo tipo de arquivo que um arquivo comum para o libarchive, e por
  isso passava pelo filtro de link simbólico/FIFO/dispositivo; o alvo do link (não
  tratado) podia apontar para fora da pasta de extração, permitindo que um arquivo
  compactado malicioso criasse um hard link para um arquivo arbitrário do disco sob um
  nome inofensivo (ex.: "*.fits"). Entradas hardlink agora são recusadas do mesmo jeito
  que link simbólico/FIFO/dispositivo já eram.

- **Observation time (AUD-CORR-15, critical).** 🇬🇧 Data Reduction no longer adds ΔT
  (68 s by default) to the image Julian Date, so the ADES `obsTime` is UTC again instead
  of ~68 s late (and 136 s after a second run). ΔT is now used only for the offline MPCORB
  ephemeris (TT). The Time Offset setting is a camera-clock correction in **seconds**,
  applied exactly once; it is no longer auto-filled with longitude/15 (hours). A stale
  value from older versions is reset to 0 once, with a warning in the log.
  🇧🇷 A Redução de Dados não soma mais o ΔT (68 s por padrão) ao Dia Juliano da imagem:
  o `obsTime` do ADES volta a ser UTC, em vez de ~68 s atrasado (136 s na segunda
  execução). O ΔT agora só entra na efeméride offline do MPCORB (TT). O Time Offset é uma
  correção do relógio da câmera em **segundos**, aplicada uma única vez, e não é mais
  preenchido com longitude/15 (horas). Um valor antigo é zerado uma vez, com aviso no log.
- **Settings units / Unidades no Settings.** 🇬🇧 Pixel scale is labelled ″/px (it was °/px
  and capped at 1); Minimum FWHM is labelled ″ (was px); Time Precision is 0–3 decimal
  places of the seconds (was labelled hours). 🇧🇷 Escala de pixel em ″/px (era °/px e
  limitada a 1); FWHM mínima em ″ (era px); Precisão de tempo = 0–3 casas decimais dos
  segundos (o rótulo dizia horas).
- **Shortcuts / Atalhos.** 🇬🇧 Light Curve moved to `Ctrl+Shift+L` (`Ctrl+L` is Load
  Images); `Ctrl+Shift+T` was declared twice and did nothing, now toggles the theme.
  🇧🇷 Curva de Luz passou para `Ctrl+Shift+L` (`Ctrl+L` é Carregar Imagens);
  `Ctrl+Shift+T` estava declarado duas vezes e não fazia nada, agora alterna o tema.
- **Help → Registration** 🇬🇧 no longer says "MIT" (the license is AGPL-3.0). 🇧🇷 não diz
  mais "MIT" (a licença é AGPL-3.0).
- **XISF size ceiling / Teto de tamanho no XISF (AUD-INPUT-9).** 🇬🇧 XISF geometry is checked
  against the same axis/pixel ceiling and file size as FITS before any allocation.
  🇧🇷 A geometria do XISF passa pelo mesmo teto de eixo/pixels e pela checagem do tamanho
  do arquivo que o FITS, antes de qualquer alocação.
- **API key from the Setup Wizard / Chave de API do assistente (AUD-SEC-6).** 🇬🇧 Stored
  through the keychain (or a 0600 settings file), like Settings already did. 🇧🇷 Gravada
  pelo chaveiro do sistema (ou num arquivo de configurações 0600), como o Settings já fazia.
- **Extinction / Extinção (AUD-CORR-8).** 🇬🇧 The MPC magnitude uses the unit-tested
  extinction function instead of a duplicated inline formula (same result). 🇧🇷 A magnitude
  do MPC usa a função de extinção testada em vez de uma fórmula duplicada (mesmo resultado).
- **PNG/TIFF size ceiling / Teto de tamanho em PNG/TIFF (AUD-INPUT-gaps).** 🇬🇧 Images read
  through Qt (PNG, TIFF, BMP, JPEG) get the same 20000 px/axis and total-pixel ceiling as
  every other loader, checked on the size the header declares before Qt decodes anything,
  so a header claiming 100000×100000 px is refused cleanly. No file-size cross-check for
  these formats (they are compressed). 🇧🇷 Imagens lidas pelo Qt (PNG, TIFF, BMP, JPEG)
  passam pelo mesmo teto de 20000 px/eixo e de pixels totais dos outros carregadores,
  checado no tamanho declarado pelo cabeçalho antes de o Qt decodificar qualquer coisa;
  um cabeçalho de 100000×100000 px é recusado sem travar. Sem checagem de tamanho do
  arquivo nesses formatos (são comprimidos).
- **SER observer/telescope / Observador/telescópio do SER.** 🇬🇧 The 40-byte text fields
  are cut at the first NUL; the padding used to end up inside the strings. 🇧🇷 Os campos
  de texto de 40 bytes param no primeiro NUL; o preenchimento ia parar dentro das strings.
- **clang-tidy never ran in CI / clang-tidy nunca rodou no CI (AUD-CI-7).** 🇬🇧 Every
  audit report so far was "No compilation database found" or a run-clang-tidy crash
  (missing PyYAML for `-export-fixes`). `compile_commands.json` is now enabled before any
  target is created, `-export-fixes` was dropped and the run is limited to `src/`.
  🇧🇷 Todos os relatórios até aqui eram "No compilation database found" ou uma queda do
  run-clang-tidy (sem PyYAML para `-export-fixes`). O `compile_commands.json` agora é
  ligado antes de qualquer alvo, o `-export-fixes` saiu e a análise fica restrita a `src/`.
- **Pop!_OS / Zorin QA jobs (AUD-CI-6).** 🇬🇧 Job names, comments, log banners, README
  badges and `docs/qa-distros.md` now say what runs: an Ubuntu 24.04 base standing in for
  those distros (their 22.04 base ships Qt 6.2.4, below the Qt 6.4 AstroFind needs), not a
  22.04 base. 🇧🇷 Nomes dos jobs, comentários, banners do log, badges do README e
  `docs/qa-distros.md` agora dizem o que roda: uma base Ubuntu 24.04 no lugar dessas
  distros (a base 22.04 delas traz Qt 6.2.4, abaixo do Qt 6.4 exigido), não uma base 22.04.
- **VizieR mirror / Espelho VizieR.** 🇬🇧 The default shown in Settings was a bare hostname
  that the client rejected; it is now the real TAP endpoint, and the old value is
  replaced on load. 🇧🇷 O padrão mostrado era um hostname sem esquema, rejeitado pelo
  cliente; agora é o endpoint TAP real, e o valor antigo é substituído ao abrir.
- **Network timeouts and cancel / Timeouts de rede e cancelamento (AUD-SEC-10).** 🇬🇧 The
  SkyBoT known-object query and the MPCORB.DAT / DAILY.DAT downloads now have the same 30 s
  transfer timeout as the other clients (it restarts on every received chunk, so a slow but
  alive download keeps going). The MPC downloads get a Cancel button in the status bar;
  re-running the known-object overlay replaces a SkyBoT query still in flight; a timeout in
  the Setup Wizard is reported as such instead of "Download cancelled".
  🇧🇷 A consulta de objetos conhecidos ao SkyBoT e os downloads do MPCORB.DAT / DAILY.DAT
  ganham o mesmo timeout de transferência de 30 s dos outros clientes (reinicia a cada bloco
  recebido, então um download lento mas vivo continua). Os downloads do MPC ganham um botão
  Cancelar na barra de status; refazer o overlay de objetos conhecidos substitui uma consulta
  SkyBoT ainda pendente; no assistente, um timeout aparece como tal, e não como "Download
  cancelado".
- **Plate-solving cancel / Cancelar a redução astrométrica (AUD-SEC-11).** 🇬🇧 Cancel now
  aborts the astrometry.net status poll in flight too, so no progress or error shows up after
  cancelling; overlapping polls on a slow server are no longer stacked. 🇧🇷 Cancelar agora
  aborta também a consulta de status ao astrometry.net em andamento: nenhum progresso ou erro
  aparece depois do cancelamento, e consultas sobrepostas num servidor lento não se acumulam.
- **Horizons target / Alvo do Horizons (AUD-SEC-12).** 🇬🇧 The target typed in Query JPL
  Horizons is checked against an allowlist (letters, digits, space and `/ - ( ) . _`, up to
  64 characters) before it goes into the `COMMAND` parameter; quotes, `;`, control characters
  and similar input get a clear error and no request is sent. 🇧🇷 O alvo digitado em
  Consultar JPL Horizons é validado por uma lista de permitidos (letras, dígitos, espaço e
  `/ - ( ) . _`, até 64 caracteres) antes de entrar no parâmetro `COMMAND`; aspas, `;`,
  caracteres de controle e afins recebem um erro claro e nenhuma requisição é enviada.
- **Server URLs in Settings / URLs de servidor no Settings (AUD-SEC-13).** 🇬🇧 OK refuses to
  save an astrometry.net server, VizieR mirror or MPC submission URL that the client would
  reject (anything but https://, or http:// on localhost/127.0.0.1/::1): a warning names
  the field and the dialog stays open. The clients and the dialog share one check.
  🇧🇷 O OK se recusa a gravar URL de servidor astrometry.net, espelho VizieR ou envio ao MPC
  que o cliente rejeitaria (qualquer coisa além de https://, ou http:// em
  localhost/127.0.0.1/::1): um aviso indica o campo e o diálogo continua aberto. Os clientes
  e o diálogo usam a mesma verificação.

### Added / Adicionado

- 🇬🇧 Secret scanning: a gitleaks CI job over the full git history and a gitleaks step in
  `scripts/pre-commit` (AUD-SEC-8). Regression tests with hostile headers (NAXIS=4,
  100000², lying sizes, XISF) and non-finite pixels in the centroids (AUD-TEST-4).
  🇧🇷 Varredura de segredos: job gitleaks no CI sobre todo o histórico e passo gitleaks no
  `scripts/pre-commit` (AUD-SEC-8). Testes de regressão com cabeçalhos hostis (NAXIS=4,
  100000², tamanhos mentirosos, XISF) e pixels não finitos nos centroides (AUD-TEST-4).
- 🇬🇧 Tests with a normal and a hostile case for the SER, XISF, PNG/TIFF (QImage), 1-D
  spectrum, reduction-table, TAR (libarchive) and ZIP loaders/extractors (AUD-TEST-6,
  `tests/test_loaders_extra.cpp`, `tests/test_archive_zip_ui.cpp`). The archive
  extraction moved from `MainWindow` to `core/ArchiveExtractor` so it is covered by the
  ASan/valgrind runs; behaviour and messages are unchanged. Catch2 test names no longer
  contain commas, which split a name filter into two and ran nothing (AUD-TEST-5).
  🇧🇷 Testes com um caso normal e um hostil para os carregadores/extratores SER, XISF,
  PNG/TIFF (QImage), espectro 1-D, tabela de redução, TAR (libarchive) e ZIP
  (AUD-TEST-6, `tests/test_loaders_extra.cpp`, `tests/test_archive_zip_ui.cpp`). A extração
  de arquivos saiu da `MainWindow` para `core/ArchiveExtractor`, para ser coberta pelas
  execuções ASan/valgrind; comportamento e mensagens iguais. Os nomes dos testes Catch2
  não têm mais vírgula, que partia o filtro por nome em dois e não rodava nada
  (AUD-TEST-5).
- 🇬🇧 Audit gate (AUD-CI-7): reads the clang-tidy report and fails on error-level lines
  (warnings stay non-blocking; on Debian 12 and Ubuntu 24.04, whose clang cannot parse
  this C++23 code with their own libstdc++, errors are shown as a warning). A missing
  report of any required tool, a clang-tidy run that analysed nothing, or a valgrind run
  that aborted now fails the job instead of passing with a warning. `unzip` is installed
  in the audit matrix so the ZIP tests run there. 🇧🇷 Portão da auditoria (AUD-CI-7): lê o
  relatório do clang-tidy e falha em linhas de nível error (warnings continuam sem
  bloquear; no Debian 12 e no Ubuntu 24.04, cujo clang não consegue analisar este código
  C++23 com a libstdc++ deles, os errors viram aviso). Relatório ausente de uma ferramenta
  obrigatória, clang-tidy que não analisou nada ou valgrind abortado agora reprovam o job
  em vez de passar com aviso. O `unzip` é instalado na matriz da auditoria para os testes
  de ZIP rodarem lá.

---

## [1.1.0] - 2026-09-24

> **Why 1.1.0 and not 1.0.0 / Por que 1.1.0 e não 1.0.0:** 0.9.0 already had 1.0.0
> maturity but was never tagged as such; this release adds new features on top of it, so it
> is the next minor version. / A 0.9.0 já tinha maturidade de 1.0.0, mas nunca recebeu essa
> tag; esta versão acrescenta funcionalidades sobre ela, então é a próxima versão minor.

### 🇬🇧 English

- **DSLR RAW loading (item 21.1)** via LibRaw (optional build dependency): CR2, CR3,
  NEF, ARW, DNG, RAF, ORF, RW2, PEF and other camera RAW files. Data stay linear;
  star detection uses the 2×2 superpixel luminance of the Bayer mosaic; exposure
  and time come from EXIF (camera clock, flagged ambiguous). Builds without LibRaw
  refuse RAW files with an explicit message. Erratum: `[0.5.0]` listed "RAW DSLR
  (libraw)" but it was never implemented until now (AUD-DOC-4/DOC-7).
- **NASA PDS images (item 21.2)**: PDS3 `.img` with attached label or detached
  `.lbl`, and PDS4 `.xml` labels (2-D arrays, every numeric data type, scaling,
  missing constants; a PDS4 label pointing to FITS opens the FITS file). Every
  declared size is checked against the file before allocation. Erratum: `[0.5.0]`
  listed "PDS format support" but it was never implemented until now.
- Audit wave E1 fixes (PR #6): INPUT-8, MEM-6, MEM-5, MEM-1, CI-5.

### 🇧🇷 Português

- **Leitura de RAW de DSLR (item 21.1)** via LibRaw (dependência opcional de
  compilação): CR2, CR3, NEF, ARW, DNG, RAF, ORF, RW2, PEF e outros RAW de câmera.
  Dados lineares; a detecção de estrelas usa a luminância por superpixel 2×2 do
  mosaico Bayer; exposição e horário vêm do EXIF (relógio da câmera, marcado como
  ambíguo). Builds sem LibRaw recusam RAW com mensagem explícita. Errata: `[0.5.0]`
  listava "RAW DSLR (libraw)", que só agora foi implementado (AUD-DOC-4/DOC-7).
- **Imagens NASA PDS (item 21.2)**: PDS3 `.img` com rótulo embutido ou `.lbl`
  separado, e rótulos PDS4 `.xml` (arrays 2-D, todos os tipos numéricos, escala,
  constantes de ausência; um rótulo PDS4 que aponta para FITS abre o FITS). Todo
  tamanho declarado é conferido contra o arquivo antes de alocar. Errata: `[0.5.0]`
  listava "suporte ao formato PDS", que só agora foi implementado.
- Correções da onda E1 da auditoria (PR #6): INPUT-8, MEM-6, MEM-5, MEM-1, CI-5.

---

## [0.9.0] - 2026-07-10

### 🇬🇧 English

**Stable release, first release after a full internal security/robustness/correctness
audit. Recommended over 0.9.0-beta for all users.**

A dedicated audit pass ("audit": a systematic review looking for bugs, security
issues and mismatches between docs and code) found and fixed **29 issues** across
input-handling robustness, memory safety, astrometric correctness, and dependency
security. The full write-up, with reproduction steps and remediation commits for
every item, is in [`AUDIT_FIND.md`](AUDIT_FIND.md).

#### Security

- `.runner` (a live self-hosted CI runner token accidentally left on disk) removed from
  tracking and the token rotated.
- All `FetchContent` dependencies (6 libraries fetched at build time) pinned to an
  exact commit SHA instead of a floating branch/tag, closing a supply-chain
  ("supply-chain attack": tampering with a dependency you pull in, instead of
  attacking your own code directly) exposure window.
- HTTP clients (astrometry.net, VizieR, JPL Horizons, SkyBoT) now enforce
  `https://` (or localhost-only `http://`) and have a request timeout + cancel path,
  closing a hang/DoS risk on a slow or malicious server.
- The plain-text API key fallback file (used only when Qt Keychain is unavailable)
  is now created with `chmod 600` permissions (owner read/write only).

#### Fixed - Crash and robustness (attacker-controlled files)

- FITS files with `NAXIS > 3` are now rejected before any cfitsio pixel read
  (previously: stack-buffer-overflow inside cfitsio).
- FITS image dimensions are validated against a ceiling and the real file size
  before any pixel-count multiplication (previously: a 2.9 KB file with a lying
  header could trigger a ~40 GB allocation attempt and an uncaught exception).
  Also fixed: a `NAXIS3` overflow bypass discovered in review of the first fix.
- SER video files reject width/height values with the sign bit set, before the
  narrowing cast to `int`.
- Archive extraction (ZIP/TAR/7Z/RAR) now rejects symlink, FIFO, and device
  entries, preventing a maliciously crafted archive from writing outside the
  extraction directory.
- BINTABLE (FITS binary table) column reads now propagate real errors instead of
  silently fabricating `NaN` rows, and out-of-bounds reads on optional columns are
  guarded.
- Centroid measurement (the pixel-position refinement step) now guards against
  non-finite (`NaN`/`Inf`) input before any `static_cast<int>` (previously: a bad
  pixel could reach `INT_MIN` and abort via signed-overflow), and photometry
  rejects a `NaN` net flux instead of reporting it as a successful measurement.
- Fixed the upstream SEP (Source Extractor library) double-allocation leak via a
  local patch applied at `FetchContent` time.

#### Fixed - Astrometric correctness

- **Non-zenithal WCS projections (CAR/MER/GLS/AIT) reported a celestial coordinate
  off by roughly 90 degrees with RA/Dec swapped.** This was the most severe finding:
  the wrong native-pole derivation fed straight into the ADES/MPC report for any
  field using one of those four projections. Fixed and validated against `astropy`
  (168/168 test cases) as an external, independent reference.
- Added a value-absolute (not just round-trip) test oracle for all 8 supported WCS
  projections, so a future regression like the one above is caught immediately.
- Added a refraction-correction test suite with known geometry, and confirmed the
  `isSpaceTelescope` gate correctly skips the correction for space telescopes.
- Removed `applyPrecessionJ2000ToDate()` and `applyNutation()`: dead code with zero
  callers anywhere in the pipeline (never actually wired in, see
  [`docs/technical-reference.md`](docs/technical-reference.md) §6/§8 for why the
  WCS plate-solve approach makes them unnecessary). Added oracle tests against
  aberration/precession/nutation/ecliptic reference values.
- Replaced circular photometry test assertions (which compared the code's output
  to itself) with an independent oracle using a known flux and zero-point.
- Centroid FWHM conversion now uses the precise constant
  `2.354820045` (`2√(2 ln 2)`) instead of a truncated literal.

#### Changed

- **License migrated from PolyForm Noncommercial 1.0.0 to the GNU Affero General
  Public License v3.0 (AGPL-3.0):** strong copyleft; commercial use is allowed but
  distributing a modified version (including running it as a network service)
  requires releasing the source code.
- Added SPDX license headers to all 127 source files under `src/` (REUSE
  compliance, a standard way to machine-check every file's license).
- Added a `NOTICE` file consolidating third-party attribution and copyright for
  every bundled/fetched dependency (CCfits, SEP, QuaZip, spdlog, nlohmann/json,
  FFTW3, cfitsio); README now shows a license column per dependency.
- Corrected the `docs/technical-reference.md` atmospheric-refraction section,
  which previously described a two-branch model that did not match the actual
  implementation (a single, unconditional Bennett-1982 formula).
- `README.md`'s GDL/NEMO/Siril reference-implementation credits reworded to make
  clear they are learning references, not code taken from those projects.

#### Infrastructure

- Added `SECURITY.md` with a responsible-disclosure contact.
- Added a `pull_request -> main` / release-tag-only CI workflow
  (`.github/workflows/audit.yml`) running the full numerical audit (ASan/UBSan,
  cppcheck, clang-tidy, Valgrind) plus a fast local `pre-commit` git hook that
  builds incrementally and runs `ctest` before every commit.
- Removed the orphaned `qa-wsl2.yml` CI workflow (WSL2 is no longer a supported
  target; all remaining distro QA items are complete, see
  [`docs/qa-distros.md`](docs/qa-distros.md)).
- Renormalized CRLF line endings to LF across all CI workflow files.
- Replaced 10 phantom-`SKIP` core tests with real, versioned synthetic FITS
  fixtures on disk.

#### Test suite

- ASan/UBSan sanitizer test run grew from **4426 to 5480 assertions** across
  **159 cases**, all green, including the new `[bintable]` and `[wcs]` oracle
  suites, with `detect_leaks=1` enabled.

### 🇧🇷 Português

**Versão estável, primeira versão após uma auditoria interna completa de
segurança/robustez/corretude. Recomendada sobre a 0.9.0-beta para todos os usuários.**

Uma passada de auditoria dedicada ("audit": uma revisão sistemática buscando bugs,
problemas de segurança e divergências entre a documentação e o código) encontrou e
corrigiu **29 problemas** em robustez de tratamento de entrada, segurança de memória,
corretude astrométrica e segurança de dependências. O relatório completo, com passos
de reprodução e commits de correção de cada item, está em
[`AUDIT_FIND.md`](AUDIT_FIND.md).

#### Segurança

- `.runner` (um token vivo do runner de CI self-hosted deixado por acidente no disco)
  removido do rastreamento e o token foi rotacionado.
- Todas as dependências via `FetchContent` (6 bibliotecas baixadas em tempo de
  build) foram fixadas num commit SHA exato em vez de uma branch/tag flutuante,
  fechando uma janela de exposição de supply-chain ("ataque de supply-chain":
  adulterar uma dependência que você importa, em vez de atacar seu próprio código
  diretamente).
- Os clientes HTTP (astrometry.net, VizieR, JPL Horizons, SkyBoT) agora exigem
  `https://` (ou `http://` restrito a localhost) e têm timeout de requisição +
  cancelamento, fechando um risco de travamento/DoS num servidor lento ou malicioso.
- O arquivo de fallback de chave de API em texto plano (usado só quando o Qt
  Keychain não está disponível) agora é criado com permissões `chmod 600`
  (leitura/escrita só para o dono).

#### Corrigido - Crash e robustez (arquivos controlados por atacante)

- Arquivos FITS com `NAXIS > 3` agora são rejeitados antes de qualquer leitura de
  pixel via cfitsio (antes: stack-buffer-overflow dentro do cfitsio).
- As dimensões de imagem FITS são validadas contra um teto e o tamanho real do
  arquivo antes de qualquer multiplicação de contagem de pixels (antes: um arquivo
  de 2,9 KB com um cabeçalho mentiroso podia disparar uma tentativa de alocação de
  ~40 GB e uma exceção não capturada). Também corrigido: um bypass de overflow em
  `NAXIS3` descoberto durante a revisão da primeira correção.
- Arquivos de vídeo SER rejeitam valores de largura/altura com o bit de sinal
  ligado, antes do cast de estreitamento para `int`.
- A extração de arquivos compactados (ZIP/TAR/7Z/RAR) agora rejeita entradas
  symlink, FIFO e de dispositivo, prevenindo que um arquivo compactado malicioso
  escreva fora do diretório de extração.
- Leituras de coluna de BINTABLE (tabela binária FITS) agora propagam erros reais
  em vez de fabricar silenciosamente linhas `NaN`, e leituras fora dos limites em
  colunas opcionais estão protegidas.
- A medição de centroide (etapa de refinamento da posição do pixel) agora protege
  contra entrada não finita (`NaN`/`Inf`) antes de qualquer `static_cast<int>`
  (antes: um pixel ruim podia chegar a `INT_MIN` e abortar por overflow com sinal),
  e a fotometria rejeita um fluxo líquido `NaN` em vez de reportá-lo como medição
  bem-sucedida.
- Corrigido o vazamento de dupla alocação do SEP (biblioteca Source Extractor) via
  um patch local aplicado no momento do `FetchContent`.

#### Corrigido - Corretude astrométrica

- **Projeções WCS não zenitais (CAR/MER/GLS/AIT) reportavam uma coordenada
  celeste com erro de aproximadamente 90 graus, com RA/Dec trocados.** Este foi o
  achado mais severo: a derivação errada do polo nativo ia direto para o relatório
  ADES/MPC em qualquer campo usando uma dessas quatro projeções. Corrigido e
  validado contra o `astropy` (168/168 casos de teste) como referência externa e
  independente.
- Adicionado um oráculo de teste por valor absoluto (não só ida-e-volta) para as 8
  projeções WCS suportadas, para que uma regressão futura como a acima seja pega
  imediatamente.
- Adicionada uma suíte de testes de correção de refração com geometria conhecida,
  e confirmado que o gate `isSpaceTelescope` pula corretamente a correção para
  telescópios espaciais.
- Removidas `applyPrecessionJ2000ToDate()` e `applyNutation()`: código morto sem
  nenhum chamador em todo o pipeline (nunca foi de fato conectado, ver
  [`docs/technical-reference.md`](docs/technical-reference.md) §6/§8 para o motivo
  pelo qual a abordagem de plate-solve WCS as torna desnecessárias). Adicionados
  testes de oráculo contra valores de referência de aberração/precessão/
  nutação/eclíptica.
- Substituídas asserções de teste de fotometria circulares (que comparavam a saída
  do código com ela mesma) por um oráculo independente usando fluxo e zero-point
  conhecidos.
- A conversão de FWHM do centroide agora usa a constante precisa `2.354820045`
  (`2√(2 ln 2)`) em vez de um literal truncado.

#### Alterado

- **Licença migrada de PolyForm Noncommercial 1.0.0 para a GNU Affero General
  Public License v3.0 (AGPL-3.0):** copyleft forte; uso comercial é permitido, mas
  distribuir uma versão modificada (inclusive rodando-a como serviço de rede) exige
  disponibilizar o código-fonte.
- Adicionados cabeçalhos de licença SPDX em todos os 127 arquivos-fonte de `src/`
  (conformidade REUSE, uma forma padronizada de checar a licença de cada arquivo
  por máquina).
- Adicionado um arquivo `NOTICE` consolidando atribuição de terceiros e copyright
  para cada dependência empacotada/baixada (CCfits, SEP, QuaZip, spdlog,
  nlohmann/json, FFTW3, cfitsio); o README agora mostra uma coluna de licença por
  dependência.
- Corrigida a seção de refração atmosférica de `docs/technical-reference.md`, que
  antes descrevia um modelo de dois ramos que não correspondia à implementação
  real (uma única fórmula Bennett-1982 incondicional).
- Os créditos de implementação de referência GDL/NEMO/Siril do `README.md` foram
  reformulados para deixar claro que são referências de aprendizado, não código
  retirado desses projetos.

#### Infraestrutura

- Adicionado `SECURITY.md` com contato para divulgação responsável.
- Adicionado um workflow de CI só em `pull_request -> main` / tag de release
  (`.github/workflows/audit.yml`) rodando a auditoria numérica completa (ASan/UBSan,
  cppcheck, clang-tidy, Valgrind), além de um git hook local `pre-commit` rápido que
  compila incrementalmente e roda `ctest` antes de cada commit.
- Removido o workflow de CI órfão `qa-wsl2.yml` (WSL2 não é mais um alvo suportado;
  todos os itens restantes de QA de distro estão completos, ver
  [`docs/qa-distros.md`](docs/qa-distros.md)).
- Renormalizados os fins de linha CRLF para LF em todos os arquivos de workflow de
  CI.
- Substituídos 10 testes core com `SKIP` fantasma por fixtures FITS sintéticas
  reais e versionadas em disco.

#### Suíte de testes

- A execução dos sanitizers ASan/UBSan cresceu de **4426 para 5480 assertions** em
  **159 casos**, todos verdes, incluindo as novas suítes de oráculo `[bintable]` e
  `[wcs]`, com `detect_leaks=1` habilitado.

---

## [0.9.0-beta] - 2026-03-22

### 🇬🇧 English

**Beta release, all features complete, pending real-world FITS validation.**

#### Added

- Release pipeline (CI workflow, since replaced by GitHub Actions): automated
  RPM, DEB, Arch PKGBUILD tarball, and `install.sh` artifacts on every version tag
  push
- `packaging/debian/control-binary`: binary-only DEB control template used by CI
- `packaging/install.sh`: universal bilingual (EN/PT-BR) installer; auto-detects
  distro, downloads correct package, resolves dependencies, sets up desktop
  integration
- `INSTALL.md`: "Quick install, binary packages" section with per-distro `curl` +
  install commands
- `README.md`: "Direct package install" section with one-liner commands for all
  supported distros

#### Changed

- Version bumped from 0.5.0 to 0.9.0-beta to reflect feature-complete beta status
- README: added beta warning (not yet validated with real observatory FITS files)

#### Fixed

- Invert / flip broken in blink mode (precomputed image path now uses
  `invertPixels()` / `mirrored()` directly)
- `QSpinBox` / `QDoubleSpinBox` / `QComboBox` button widths inconsistent between
  dark and light themes
- Overlay marking colors did not match catalog table colors (unified via
  `Theme::mark*()` constants)
- 10 truncated PT-BR translations caused by AI bulk-generation hitting output
  token limit

#### Infrastructure

- `Theme::dp(int)`: DPI-proportional sizing; all hard-coded pixel values scaled to
  logical DPI (no visual change at 96 DPI; proportional on HiDPI screens without
  OS-level scaling)
- `Theme::mark*()` inline color constants as single source of truth for overlay +
  table colors

### 🇧🇷 Português

**Versão beta, todas as funcionalidades completas, aguardando validação com FITS
reais.**

#### Adicionado

- Pipeline de release (workflow de CI, depois substituído pelo GitHub Actions):
  geração automática de RPM, DEB, tarball PKGBUILD para Arch e `install.sh` a cada
  push de tag de versão
- `packaging/debian/control-binary`: template de controle DEB binário usado pelo CI
- `packaging/install.sh`: instalador universal bilíngue (EN/PT-BR); detecta distro
  automaticamente, baixa o pacote correto, resolve dependências, configura
  integração com desktop
- `INSTALL.md`: seção "Instalação rápida, pacotes binários" com comandos `curl` +
  instalação por distro
- `README.md`: seção "Instalação direta do pacote" com comandos de uma linha para
  todas as distros suportadas

#### Alterado

- Versão incrementada de 0.5.0 para 0.9.0-beta para refletir o estado beta com
  funcionalidades completas
- README: adicionado aviso de beta (ainda não validado com FITS reais de
  observatório)

#### Corrigido

- Inversor / espelhamento quebrado em modo blink (caminho de imagem
  pré-computada agora usa `invertPixels()` / `mirrored()` diretamente)
- Largura dos botões `QSpinBox` / `QDoubleSpinBox` / `QComboBox` inconsistente
  entre temas escuro e claro
- Cores das marcações no overlay não correspondiam às cores na tabela do
  catálogo (unificadas via constantes `Theme::mark*()`)
- 10 traduções PT-BR truncadas causadas por geração em lote de IA que atingiu o
  limite de tokens de saída

#### Infraestrutura

- `Theme::dp(int)`: dimensionamento proporcional ao DPI; todos os valores de
  pixels em código escalados ao DPI lógico (sem mudança visual a 96 DPI;
  proporcional em telas HiDPI sem escala configurada pelo SO)
- Constantes de cor `Theme::mark*()` como fonte única de verdade para cores do
  overlay e da tabela

---

## [0.5.0] - 2026-03-21

### 🇬🇧 English

**Phase 4: Advanced rendering, formats, WCS pipeline, detection algorithms,
import/export, quality audit.**

#### Added - Display & Rendering

- Transfer functions: Linear, Square Root, Asinh, Logarithmic, Histogram
  Equalization
- Transfer function selector integrated in BackgroundRangeDialog
- False color / LUT presets: Hot, Cool, Viridis, Grayscale
- NaN pixel highlight in a distinct color overlay
- Laplacian / Unsharp-mask sharpening toggle in BlinkWidget (Off / USM / LoG)
- 2D power spectrum dialog (FFT with Hann window, log₁₊ scale, frequency tooltip)
- Pixel histogram dialog with Gaussian fit overlay

#### Added - Image Formats

- Color FITS support via NAXIS3 = 3 (RGB planes)
- Color FITS via multi-extension HDUs (3 separate image extensions)
- HDU navigator for multi-extension FITS files
- SER, XISF, TIFF 16-bit, PNG 16-bit, RAW DSLR (libraw), PDS format support
- FITS data cube animation: NAXIS3 > 3 planes played as blink animation (MDI
  sub-window)
- 1D spectrum plot dialog for FITS NAXIS=1 spectra (Qt6::Charts, dark theme, WCS
  axis)
- libarchive support: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR (optional dependency)

#### Added - Astrometry & WCS

- Full WCS projection pipeline beyond TAN: SIN, ARC, GLS/SFL, MER, AIT, STG
  (Calabretta & Greisen 2002)
- Atmospheric refraction correction applied before plate-solve and ADES export
- ICRS → CIRS → topocentric coordinate chain; `<sys>ICRF</sys>` in XML+PSV output
- Annual aberration, precession J2000→date and nutation applied
- Ecliptic plane overlay + Galactic plane proximity warning

#### Added - Detection & Measurement

- Elliptical PSF fitting with Levenberg-Marquardt solver (FWHM, coma, tracking
  quality)
- Bad pixel mask with nearest-neighbor interpolation
- Streak/trail detection by blob elongation analysis
- ClumpFind algorithm for blended sources
- Background median sliding filter (tile grid, sigma-clipping)

#### Added - Import / Export

- Import DAOPHOT/SExtractor FITS BINTABLE detected-star catalogs
- Import local offline catalogs from FITS BINTABLE (USNO-B/UCAC/Gaia-style; cone
  filter)
- Import reduction tables from IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/
  ycentroid)
- Settings → Connections: catalog source selector (VizieR / Local FITS)

#### Added - Infrastructure & Quality

- CCfits 2.7 bundled source; `FitsTableReader` built on CCfits BINTABLE API
- Numerical audit targets: `cmake --build build --target audit` (cppcheck,
  clang-tidy, Valgrind, ASan+UBSan)
- 4 bugs fixed by audit: FFTW r2c buffer size for non-square images, strict
  aliasing in ImageLoader, `Qt::UTC` deprecation, missing automatic-move
  optimization
- `INSTALL.md` with per-distro dependency installation instructions (10 distros)

#### Fixed

- B&W vs Color auto-detection warning for unexpected image type
- `QImage::flipped()` replaced with `mirrored()` for Qt 6.4 compatibility

### 🇧🇷 Português

**Fase 4: Renderização avançada, formatos, pipeline WCS, algoritmos de detecção,
importação/exportação, auditoria de qualidade.**

#### Adicionado - Exibição e Renderização

- Funções de transferência: Linear, Raiz Quadrada, Asinh, Logarítmica,
  Equalização de Histograma
- Seletor de função de transferência integrado ao BackgroundRangeDialog
- Presets de cor falsa / LUT: Hot, Cool, Viridis, Escala de Cinza
- Destaque de pixels NaN em cor distinta
- Alternância de nitidez Laplaciano / Unsharp-mask no BlinkWidget (Off / USM /
  LoG)
- Diálogo de espectro de potência 2D (FFT com janela Hann, escala log₁₊, tooltip
  de frequência)
- Diálogo de histograma de pixels com ajuste gaussiano

#### Adicionado - Formatos de Imagem

- Suporte a FITS colorido via NAXIS3 = 3 (planos RGB)
- FITS colorido via multi-extensão HDU (3 extensões de imagem separadas)
- Navegador de HDUs para arquivos FITS multi-extensão
- Suporte a SER, XISF, TIFF 16-bit, PNG 16-bit, RAW DSLR (libraw), PDS
- Animação de cubo de dados FITS: NAXIS3 > 3 planos reproduzidos como animação
  blink
- Diálogo de espectro 1D para FITS NAXIS=1 (Qt6::Charts, tema escuro, eixo WCS)
- Suporte a arquivos compactados via libarchive: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR
  (opcional)

#### Adicionado - Astrometria e WCS

- Pipeline completo de projeção WCS além de TAN: SIN, ARC, GLS/SFL, MER, AIT, STG
  (Calabretta & Greisen 2002)
- Correção de refração atmosférica aplicada antes do plate-solve e exportação
  ADES
- Cadeia ICRS → CIRS → topocêntrico; `<sys>ICRF</sys>` em XML+PSV
- Aberração anual, precessão J2000→data e nutação aplicadas
- Overlay do plano eclíptico + aviso de proximidade do plano galáctico

#### Adicionado - Detecção e Medição

- Ajuste de PSF elíptica com solver Levenberg-Marquardt (FWHM, coma, qualidade de
  tracking)
- Máscara de pixels ruins com interpolação por vizinhos mais próximos
- Detecção de traços/rastros por análise de elongação de blob
- Algoritmo ClumpFind para fontes blendadas
- Filtro mediano deslizante de fundo (grade de tiles, sigma-clipping)

#### Adicionado - Importação / Exportação

- Importar catálogos FITS BINTABLE de estrelas detectadas DAOPHOT/SExtractor
- Importar catálogos offline locais de FITS BINTABLE (estilo USNO-B/UCAC/Gaia;
  filtro por cone)
- Importar tabelas de redução IRAF/Astropy FITS (XCENTER/YCENTER/xcentroid/
  ycentroid)
- Configurações → Conexões: seletor de fonte de catálogo (VizieR / FITS Local)

#### Adicionado - Infraestrutura e Qualidade

- CCfits 2.7 empacotado; `FitsTableReader` construído sobre a API BINTABLE do
  CCfits
- Targets de auditoria numérica: `cmake --build build --target audit` (cppcheck,
  clang-tidy, Valgrind, ASan+UBSan)
- 4 bugs corrigidos pela auditoria: buffer FFTW r2c para imagens não quadradas,
  aliasing estrito no ImageLoader, deprecação de `Qt::UTC`, otimização de move
  automático ausente
- `INSTALL.md` com instruções de instalação de dependências por distribuição (10
  distros)

#### Corrigido

- Aviso de detecção automática B&W vs Cor para tipo de imagem inesperado
- `QImage::flipped()` substituído por `mirrored()` para compatibilidade com Qt
  6.4

---

## [0.4.0] - 2026-03-01

### 🇬🇧 English

**Phase 3: Complete science workflow, school UX, i18n, photometry, calibration,
MOD, ADES reporting.**

#### Added - Interface & UX

- Full bilingual interface: English and Brazilian Portuguese (1007 strings)
- School workflow: step-by-step guided mode for students
- `Toast` notification widget (non-blocking: success/info/warning/error)
- Day/Night/Auto UI theme toggle (Ctrl+Shift+T)
- Export/Import School Configuration (File menu)
- "Send to Teacher" button in Report Preview (mailto:)
- School email field in Settings → Observer tab
- Dynamic WCS status in workflow panel

#### Added - Help & Documentation

- Built-in bilingual help (HTML) with FAQ and "For Students" sections
- AboutDialog: license link; reference implementations with GitHub links

#### Added - Reporting & Submission

- ADES 2022 XML and PSV report generation with `<sys>ICRF</sys>` annotation
- MPC report preview dialog with HTML rendering
- MPC direct submission endpoint
- Solar System body identification via IMCCE SkyBoT
- JPL Horizons ephemeris query dialog

#### Added - Measurement

- Centroid engine: intensity-weighted moments + Gaussian PSF fitting
- Aperture photometry: differential, zero-point calibration, airmass correction
- Light curve dialog and growth curve dialog
- Region statistics panel (mean, median, σ, min/max)
- VerificationDialog: star match residuals and known-object check

#### Added - Calibration & Stacking

- Bias/dark/flat calibration pipeline
- Calibration wizard (step-by-step)
- Master dark / master flat builder dialog
- Image stacker: Add, Average, Median, Sigma-clipped; sub-pixel FFT registration
- Track & Stack with asteroid-tracking alignment

#### Added - Moving Object Detection

- Moving Object Detector across ≥3 images with tracklet linking

#### Added - Plate Solving

- ASTAP offline plate solver support

#### Added - Catalogs

- MPCORB offline catalog (KOO engine)
- Observatory preset database (MPC codes)

### 🇧🇷 Português

**Fase 3: Fluxo científico completo, UX escolar, i18n, fotometria, calibração,
MOD, relatórios ADES.**

#### Adicionado - Interface e UX

- Interface bilíngue completa: inglês e português brasileiro (1007 strings)
- Fluxo escolar: modo guiado passo a passo para alunos
- Widget de notificação `Toast` (não bloqueante: sucesso/info/aviso/erro)
- Alternância de tema Dia/Noite/Automático (Ctrl+Shift+T)
- Exportar/Importar Configuração da Escola (menu Arquivo)
- Botão "Enviar para Professor" na pré-visualização de relatório (mailto:)
- Campo de e-mail da escola em Configurações → aba Observador
- Status WCS dinâmico no painel de workflow

#### Adicionado - Ajuda e Documentação

- Ajuda integrada bilíngue (HTML) com seções FAQ e "Para Alunos"
- AboutDialog: link de licença; implementações de referência com links do GitHub

#### Adicionado - Relatórios e Submissão

- Geração de relatório ADES 2022 XML e PSV com anotação `<sys>ICRF</sys>`
- Diálogo de pré-visualização de relatório MPC com renderização HTML
- Endpoint de submissão direta ao MPC
- Identificação de corpos do Sistema Solar via IMCCE SkyBoT
- Diálogo de consulta de efemérides JPL Horizons

#### Adicionado - Medição

- Motor de centroide: momentos ponderados por intensidade + ajuste de PSF
  gaussiana
- Fotometria de abertura: diferencial, calibração de zero-point, correção de
  massa de ar
- Diálogo de curva de luz e diálogo de curva de crescimento
- Painel de estatísticas de região (média, mediana, σ, mín/máx)
- VerificationDialog: resíduos de correspondência de estrelas e verificação de
  objetos conhecidos

#### Adicionado - Calibração e Empilhamento

- Pipeline de calibração bias/dark/flat
- Assistente de calibração (passo a passo)
- Diálogo construtor de master dark / master flat
- Empilhador de imagens: Adição, Média, Mediana, Sigma-clipped; registro
  sub-pixel por FFT
- Track & Stack com alinhamento por rastreamento de asteroide

#### Adicionado - Detecção de Objetos em Movimento

- Detector de Objetos em Movimento em ≥3 imagens com ligação de tracklets

#### Adicionado - Plate Solving

- Suporte ao plate solver offline ASTAP

#### Adicionado - Catálogos

- Catálogo offline MPCORB (motor KOO)
- Banco de dados de observatórios predefinidos (códigos MPC)

---

## [0.3.0] - 2026-02-01

### 🇬🇧 English

**Phase 2: Online astrometry, star detection, plate solving, catalog overlay,
known-object engine.**

#### Added

- Automated star detection via SEP (Source Extractor C library)
- Online plate solving via astrometry.net REST API
- WCS TAN projection pipeline; WCS solution written back to FITS header
- VizieR star catalog overlay (UCAC4, Gaia DR3) with SQLite local cache
- Known Object Overlay (KOO) engine via IMCCE SkyBoT
- Session comparison dialog

### 🇧🇷 Português

**Fase 2: Astrometria online, detecção de estrelas, plate solving, sobreposição
de catálogo, motor de objetos conhecidos.**

#### Adicionado

- Detecção automática de estrelas via SEP (biblioteca C do Source Extractor)
- Plate solving online via API REST do astrometry.net
- Pipeline de projeção WCS TAN; solução WCS gravada de volta no cabeçalho FITS
- Sobreposição do catálogo de estrelas VizieR (UCAC4, Gaia DR3) com cache local
  SQLite
- Motor de Objetos Conhecidos (KOO) via IMCCE SkyBoT
- Diálogo de comparação de sessão

---

## [0.2.0] - 2025-12-01

### 🇬🇧 English

**Phase 1: Blink comparison workflow and image navigation.**

#### Added

- Multi-image blink engine (BlinkWidget) with configurable interval
- Thumbnail navigator bar for rapid frame selection
- Background/range dialog for interactive display adjustment
- Log panel (scrollable info/warning/error messages)
- B&W vs Color image type warning with "Do not show again" checkbox

### 🇧🇷 Português

**Fase 1: Fluxo de comparação blink e navegação entre imagens.**

#### Adicionado

- Motor de blink multi-imagem (BlinkWidget) com intervalo configurável
- Barra de miniaturas para seleção rápida de frames
- Diálogo de fundo/alcance para ajuste interativo de exibição
- Painel de log (mensagens info/aviso/erro com rolagem)
- Aviso de tipo de imagem B&W vs Cor com checkbox "Não mostrar novamente"

---

## [0.1.0] - 2025-11-01

### 🇬🇧 English

**Phase 0: Qt6/C++ skeleton with FITS image viewer.**

#### Added

- Qt6 application skeleton with MDI sub-window layout
- FITS file loading (cfitsio): single HDU, primary image
- FITS image viewer: zoom, pan, invert, flip horizontal/vertical
- Bilingual menu structure (English / Brazilian Portuguese stubs)
- CMake build system (Qt6, cfitsio, spdlog, nlohmann/json via FetchContent)
- PolyForm Noncommercial 1.0.0 license (historical: superseded by AGPL-3.0 in
  v0.9.0, see above)

### 🇧🇷 Português

**Fase 0: Esqueleto Qt6/C++ com visualizador de imagens FITS.**

#### Adicionado

- Esqueleto de aplicativo Qt6 com layout MDI de sub-janelas
- Carregamento de arquivos FITS (cfitsio): HDU único, imagem primária
- Visualizador de imagens FITS: zoom, pan, inverter, espelhar
  horizontal/vertical
- Estrutura de menus bilíngue (inglês / português brasileiro, stubs)
- Sistema de build CMake (Qt6, cfitsio, spdlog, nlohmann/json via FetchContent)
- Licença PolyForm Noncommercial 1.0.0 (histórico: substituída pela AGPL-3.0 na
  v0.9.0, ver acima)
