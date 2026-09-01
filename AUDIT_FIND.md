# AUDIT_FIND - Dossiê de Auditoria Eventual AstroFind

> Livro de auditoria interna consolidado (2026-09-01), pronto para entrega a auditor externo.
> Cada achado tem ID estável (`AUD-<LENTE>-<n>`), severidade adjudicada, âncora por símbolo/arquivo, evidência e remediação sugerida.
> Referência viva é por símbolo/função. Número de arquivo:linha aparece só no repro (snapshot datado).
> Este arquivo sucede o dossiê de 2026-07-10 no mesmo path; o git guarda o retrato de julho. Não é reimpressão.

---

## 1. Cabeçalho

| Campo | Valor |
|---|---|
| **Projeto** | AstroFind (repo `astrometrica`) |
| **HEAD SHA** | `0940509243f1bbf8d2958bb373ac2e298fdd9c6d` |
| **HEAD data** | 2026-08-20 11:18:28 -0300 (`chore(ci): migra do Codeberg para o GitHub como host único`) |
| **Data desta auditoria** | 2026-09-01 (`01/09/26 - 08:16:19`, America/Recife) |
| **Tipo** | EVENTUAL NOVA do estado atual (pós Ondas 1-4 e release v0.9.0). READ-ONLY: nenhuma correção de `src/` / `tests/` / CMake. Sem git add/commit/push nesta fase. |
| **Versão de produto** | v0.9.0 |
| **Licença** | AGPL-3.0-or-later |
| **Stack** | Qt6 / C++23, CMake, cfitsio, fftw3 + bundled SEP, CCfits, quazip, spdlog, nlohmann_json, Catch2 |
| **Porte** | desktop single-binary; `src/core` 53 cpp/h, `src/ui` 73 cpp/h, `src/main.cpp`; `tests/` 20 cpp + `synthetic_fits.h`; 11 workflows |
| **Stake** | alto: output astrométrico ao Minor Planet Center (MPC); arquivo de terceiro (FITS/SER/XISF/TIFF/PNG/ZIP); reimplementação com `originals/Astrometrica.exe` |

### Método (três papéis independentes)

1. **Finder** (grok-4.5) - uma passada por lente (INPUT, MEM, SEC, PROV, CORR, CI, DOC, TEST), gerando candidatos em `/var/tmp/astrofind-audit-eventual-20260901/reports/F-*.md`. Relatório de finder **não** é prova.
2. **Verificador adversarial** (agente DIFERENTE, grok-4.5) - EXECUTA repro, oráculo, `gitleaks`, `gh run`, mutação em cópia `/var/tmp`. Saídas `V-*.md`.
3. **Orquestrador** - re-executou os CRÍTICOS (ver §1.1). **Internal-auditor** consolida este livro a partir dos V-*.md e da re-execução; não reabre finder vs verifier. Empate: prevalece o verificador que executou + a re-execução do orquestrador.

### Disciplina read-only

Nenhum arquivo de produção (`src/`, `tests/`) foi editado. Nenhum `git add/commit/push` nesta fase. Working tree permanece `M astrofind.desktop` + untracked `audit_completa_eventual.md` (intocados). `TODO.md` não foi editado (fase seguinte).

### Calibração de porte e taxonomia

AstroFind é aplicação técnica de nicho (astrometria), single-binary desktop, sem multi-tenant, sem API REST, sem QML, sem MySQL de servidor. As 8 lentes da ordem do líder foram mantidas. Capítulos REST/LGPD-titular/QML/firmware não viraram lente própria (anti over-engineering; nenhum item da ordem foi cortado).

Severidade (empate: verbatim do líder em `audit_completa_eventual.md`):

| Classe | Significado |
|---|---|
| CRÍTICO | crash / corrupção / RCE / UB / vazamento explorável. Output errado ao MPC = corrupção científica = CRÍTICO. |
| IMPORTANTE | bug real de corretude ou robustez, sem exploit direto. |
| COSMÉTICO | drift de doc, mina defensiva não-alcançável, estilo. |

Manual plugin AUDITORIAS (Crítico bloqueia release; Importante = antes do próximo marco; Cosmético = incremental) só desempataria se o verbatim fosse silente. Não inflar "violação de arquitetura" a CRÍTICO sem crash/corrupção/UB/RCE/vazamento explorável.

CONFIRMADO = repro executado **ou** fato de código inequívoco re-lido no blob HEAD + verifier. Sem repro = PLAUSÍVEL, seção separada. IDs de julho não foram reusados para bug novo.

Frase-guarda L-21 (aplicada; não cortou WCS, loaders nem formatos): uso/contagem de uso no repositório do consumidor NUNCA corta escopo, desenho ou qualidade de um produto feito para distribuição - dor do consumidor é evidência de lacuna, jamais prova de que o que ele não usa pode sair; "ninguém usa X" é afirmação sobre UM repositório, nunca sobre o mundo; regra de paridade: se o motor/biblioteca substituído aceita, o produto novo aceita também - paridade completa, não o mínimo medido no único consumidor conhecido.

### FATO vs INFERÊNCIA (L-18)

**FATO** neste livro: comando/log/md5/`git show HEAD:<arquivo>` / tabela de V-*.md / re-execução do orquestrador. **INFERÊNCIA** está marcada. Achado que não está num V-*.md **não entra**.

### 1.1 Re-verificação do orquestrador (CRÍTICOS)

| ID | Comando / medição | Resultado | Veredito |
|---|---|---|---|
| AUD-INPUT-1 | `input_asan_harness loadFits naxis_4.fits` | Unsupported NAXIS=4; ASan silencioso | AINDA-FECHADO |
| AUD-INPUT-2 | FITS 2880 B, 100k×100k | ceiling 20000; RSS baixo | AINDA-FECHADO |
| AUD-INPUT-8 | `loadSpectrum1D spec_naxis1_1e8.fits` | aloca a partir de header 2880 B; controle 2-D barrado. md5 harness `d96c17485bf68a23a926f12320d214c2` | CONFIRMADO |
| AUD-MEM-2 | `mem_asan_harness mem2` | nullopt, vivo, EXIT 0 | AINDA-FECHADO |
| AUD-MEM-6 | `mem_asan_harness mem6` | ASan heap-buffer-overflow `Calibration.cpp:291` `subtractBackground`, EXIT 134; controlo 128×128 OK. md5 `a4c5b9807a9c17368a73f86f91b4d185` | CONFIRMADO |
| AUD-MEM-5 | `mem5_asan_harness under` (50×100) | EXIT 134, ASan heap-buffer-overflow READ `crossPowerSpectrum` `ImageStacker.cpp:52`, alocação `forwardFFT` L34, 40800 B = 2550×16. Controlo equal/over EXIT 0. md5 harness `9191cb6c1c0d9a8770ed63996eb5a3e6`. fftw-devel-3.3.10-17.fc44 instalado. | CONFIRMADO (overflow ASan + aritmética) |
| AUD-CORR-1 | `wcs_harness` × astropy 8.0.1 | CAR/MER/GLS/AIT ref = 180.0 (não 270) | AINDA-FECHADO |
| AUD-TEST-5 | Catch2 filtro com vírgula no nome | "No tests ran" exit 0 (falso-verde) | CONFIRMADO |

Limitações da re-execução: cmake ASan do produto não configurou; harnesses parciais. `fftw-devel` passou a estar presente em 2026-09-01 (runtime MEM-5 executado; ver §1.1). Freeze `astrofind_tests` em `build/` é de **2026-03-22**, STALE vs HEAD; **não** foi usado como prova da suíte atual.

---

## 2. Sumário executivo

**Contagem desta eventual (IDs novos CONFIRMADOS):** **3 CRÍTICOS**, **25 IMPORTANTES**, **10 COSMÉTICOS**, **4 PLAUSÍVEIS** (IDs próprios). DOC-9 é gêmeo L-17 de PROV-9 (um primário + ponte; não infla a conta).

**Re-teste de julho (não reconta como novo):** 5 CRÍTICOS de produto **AINDA-FECHADOS** (INPUT-1, INPUT-2, CORR-1, MEM-2, MEM-3). **AINDA-ABERTOS:** MEM-1 (leak SEP no binário linkado), SEC-1 (residual `.runner`), MEM-4 residual (= INPUT-8), INPUT-gaps, MEM-gaps. **REGREDIU:** CI-3 (mesmo fato que CI-5: `audit.yml` `runs-on: docker`, 9 cancelled). INPUT-4/5/6/7: guards no tree, **não re-executados** nesta onda.

Os 3 CRÍTICOS novos, em uma frase cada:

- **AUD-INPUT-8** - `loadSpectrum1D` aloca `vector<float>(NAXIS1)` a partir de um FITS de 2880 B com header mentiroso; o path 2-D irmão já é barrado pelo teto de INPUT-2.
- **AUD-MEM-6** - `subtractBackground` lê fora de `tileVal` quando um eixo tem uma só tile (128×64, tile 64); ASan heap-buffer-overflow, EXIT 134.
- **AUD-MEM-5** - `forwardFFT` aloca `w*(h/2+1)` e planeja FFTW `h*(w/2+1)`; CCD portrait 50×100 no path `stackImages` FFT aborta sob ASan (primeiro trap: READ em `crossPowerSpectrum`).

**Veredito geral (honesto):** a remediação de julho segurou os quatro CRÍTICOS de produto que iam ao crash/MPC (repro hostil + oráculo astropy + mutação). O livro de 2026-07-10 **não** pode ser lido como "curado": (1) três CRÍTICOS novos na superfície que a Onda 1 não fechou (espectro 1-D, fundo em crop, FFT portrait); (2) o patch SEP está na fonte e **não** no `libsep_lib.a` linkado; (3) o portão `audit.yml` nunca correu com sucesso no GitHub após o cutover (9 cancelled, 0 success); (4) a suíte Catch2 HEAD **não** pega INPUT-1/2 nem MEM-2/3 (mutação do guard NAXIS deixa 13/13 verdes). Não há CRÍTICO de RCE nesta passagem. **Não** taggear release até fechar INPUT-8, MEM-6 e MEM-5 (overflow ASan já CONFIRMADO; código ainda aberto), e religar o portão ASan no GitHub.

---

## 3. Índice mestre

| ID | Título | Severidade | Status | Lente | Âncora (símbolo/arquivo) |
|---|---|:---:|:---:|:---:|---|
| [AUD-INPUT-1](#aud-input-1) | Stack-buffer-overflow via NAXIS>3 | CRÍTICO | AINDA-FECHADO | INPUT | `loadFits*` / `kMaxImageAxes` |
| [AUD-INPUT-2](#aud-input-2) | Alocação sem teto por header mentiroso | CRÍTICO | AINDA-FECHADO | INPUT | `validateImageDims` |
| [AUD-CORR-1](#aud-corr-1) | Projeções não-zenitais 90° erradas → MPC | CRÍTICO | AINDA-FECHADO | CORR | `celestialPole` / `pixToSky` |
| [AUD-MEM-2](#aud-mem-2) | NaN/Inf → cast→INT_MIN→overflow abort | CRÍTICO | AINDA-FECHADO | MEM | `findCentroid` / `aperturePhotometry` |
| [AUD-MEM-3](#aud-mem-3) | Guard de sanidade contornável por NaN | CRÍTICO | AINDA-FECHADO | MEM | `findCentroidElliptical` / `findCentroidPsf` |
| [AUD-INPUT-8](#aud-input-8) | Spectrum1D aloca NAXIS1 mentiroso | CRÍTICO | CONFIRMADO | INPUT | `core::loadSpectrum1D` |
| [AUD-MEM-6](#aud-mem-6) | OOB `tileVal` em `subtractBackground` | CRÍTICO | CONFIRMADO | MEM | `core::subtractBackground` |
| [AUD-MEM-5](#aud-mem-5) | FFT under-alloc quando `h>w` | CRÍTICO | CONFIRMADO (overflow ASan + aritmética) | MEM | `forwardFFT` / `crossPowerSpectrum` |
| [AUD-MEM-1](#aud-mem-1) | Leak SEP `convert_to_catalog` no binário linkado | IMPORTANTE | AINDA-ABERTO | MEM | `sep convert_to_catalog` / `libsep_lib.a` |
| [AUD-CI-3](#aud-ci-3) | Auditoria numérica nunca roda em CI | IMPORTANTE | REGREDIU | CI | `audit.yml` (mesmo fato [AUD-CI-5](#aud-ci-5)) |
| [AUD-CI-5](#aud-ci-5) | `runs-on: docker` / residual Forgejo | IMPORTANTE | CONFIRMADO | CI | `.github/workflows/audit.yml` |
| [AUD-INPUT-9](#aud-input-9) | XISF sem teto de eixo FITS | IMPORTANTE | CONFIRMADO | INPUT | `loadXisf` |
| [AUD-SEC-6](#aud-sec-6) | SetupWizard grava API key sem ApiKeyStore | IMPORTANTE | CONFIRMADO (código; 0644 PLAUSÍVEL) | SEC | `SetupWizard::ApiKeyPage::save` |
| [AUD-SEC-7](#aud-sec-7) | `.runner` residual em disco (Codeberg) | IMPORTANTE | CONFIRMADO (presença; validade PLAUSÍVEL) | SEC | `.runner` (gitignore + mode 600) |
| [AUD-SEC-8](#aud-sec-8) | Sem scanner de secret em hook/CI | IMPORTANTE | CONFIRMADO | SEC | `scripts/pre-commit` / workflows |
| [AUD-SEC-10](#aud-sec-10) | KooEngine/MPCORB sem timeout | IMPORTANTE | CONFIRMADO (gap; hang PLAUSÍVEL) | SEC | `KooEngine::queryField` |
| [AUD-SEC-11](#aud-sec-11) | `cancel()` não aborta poll `get` | IMPORTANTE | CONFIRMADO (código; fantasma PLAUSÍVEL) | SEC | `AstrometryClient` poll |
| [AUD-SEC-12](#aud-sec-12) | Horizons `COMMAND` com input cru | IMPORTANTE | CONFIRMADO (construção; parse PLAUSÍVEL) | SEC | `HorizonsClient::query` |
| [AUD-SEC-13](#aud-sec-13) | Save de URL sem `isSafe*` | IMPORTANTE | CONFIRMADO | SEC | `SettingsDialog` URLs |
| [AUD-PROV-10](#aud-prov-10) | 23 rótulos de menu idênticos ao exe | IMPORTANTE | CONFIRMADO | PROV | `MainWindow.cpp` `tr("…")` |
| [AUD-CORR-7](#aud-corr-7) | Refração sempre após WCS plate-solved | IMPORTANTE | CONFIRMADO (política; erro ° PLAUSÍVEL) | CORR | `MainWindow_measurement.cpp` |
| [AUD-CORR-8](#aud-corr-8) | Extinção MPC inline; função morta | IMPORTANTE | CONFIRMADO | CORR | `applyExtinctionCorrection` vs inline |
| [AUD-CORR-10](#aud-corr-10) | LONPOLE/LATPOLE/PV1_* não lidos | IMPORTANTE | CONFIRMADO (código; delta PLAUSÍVEL) | CORR | parse WCS `FitsImage.cpp` |
| [AUD-CORR-11](#aud-corr-11) | Catch2 WCS sem polo sul / Dec− | IMPORTANTE | CONFIRMADO (suíte; código OK no oráculo) | CORR | `tests/` `[wcs]` |
| [AUD-CORR-12](#aud-corr-12) | Zero testes Ephemeris/KooEngine | IMPORTANTE | CONFIRMADO | CORR | `tests/` (ausência) |
| [AUD-CORR-13](#aud-corr-13) | JD trunca sub-segundo de DATE-OBS | IMPORTANTE | CONFIRMADO (truncamento; mag PLAUSÍVEL) | CORR | `QDateTime::secsTo` |
| [AUD-CORR-14](#aud-corr-14) | ADES `sys=ICRF` sem assert; "apparent" | IMPORTANTE | CONFIRMADO | CORR | `AdesReport.cpp` |
| [AUD-CI-6](#aud-ci-6) | Pop 22.04 / Zorin 17 mentem a base | IMPORTANTE | CONFIRMADO | CI | `qa-pop-os-22.yml` / `qa-zorin-17.yml` |
| [AUD-CI-7](#aud-ci-7) | Gate omite clang-tidy | IMPORTANTE | CONFIRMADO | CI | `audit.yml` Gate on critical findings |
| [AUD-DOC-5](#aud-doc-5) | SECURITY.md "tudo corrigido" | IMPORTANTE | CONFIRMADO | DOC | `SECURITY.md` |
| [AUD-DOC-6](#aud-doc-6) | Help anuncia RAW/PDS4 | IMPORTANTE | CONFIRMADO | DOC | `help_en.html` / `help_pt_br.html` |
| [AUD-DOC-8](#aud-doc-8) | Help: cadeia ICRS→CIRS com aberração/nutação | IMPORTANTE | CONFIRMADO | DOC | Help glossário CIRS |
| [AUD-TEST-4](#aud-test-4) | Suíte não pega INPUT-1/2 nem MEM-2/3 | IMPORTANTE | CONFIRMADO | TEST | `tests/` (ausência + mutação M1) |
| [AUD-TEST-5](#aud-test-5) | Catch2 vírgula no nome = falso-verde | IMPORTANTE | CONFIRMADO | TEST | nomes `TEST_CASE` (L-45) |
| [AUD-TEST-6](#aud-test-6) | SER/XISF/QImage/Spectrum1D/archives sem teste | IMPORTANTE | CONFIRMADO | TEST | `tests/*.cpp` |
| [AUD-SEC-1](#aud-sec-1) | Token `.runner` (julho) | IMPORTANTE | AINDA-ABERTO (residual; ver SEC-7) | SEC | `.runner` |
| [AUD-SEC-2](#aud-sec-2) | Pins FetchContent | IMPORTANTE | AINDA-FECHADO (6 pins) + residual CCfits | SEC | `cmake/dependencies.cmake` |
| [AUD-INPUT-3](#aud-input-3) | SER sinal/teto | IMPORTANTE | AINDA-FECHADO | INPUT | `loadSer` |
| [AUD-CORR-2](#aud-corr-2) | Teste absoluto WCS | IMPORTANTE | AINDA-FECHADO (residual → CORR-11) | CORR | `tests/` `[wcs]` |
| [AUD-CORR-3](#aud-corr-3) | Testes de refração | IMPORTANTE | AINDA-FECHADO (residual → CORR-7) | CORR | `[refraction]` |
| [AUD-CORR-4](#aud-corr-4) | Aberração/prec/nut | IMPORTANTE | AINDA-FECHADO | CORR | `Astronomy.cpp` |
| [AUD-CORR-5](#aud-corr-5) | Fotometria oráculo | IMPORTANTE | AINDA-FECHADO (residual → CORR-8) | CORR | `[AUD-CORR-5]` |
| [AUD-PROV-1](#aud-prov-1) | NOTICE WCSLIB | IMPORTANTE | AINDA-FECHADO | PROV | `NOTICE` §1 |
| [AUD-PROV-2](#aud-prov-2) | NOTICE SEP | IMPORTANTE | AINDA-FECHADO (residual → PROV-13) | PROV | `NOTICE` §2 |
| [AUD-PROV-3-6](#aud-prov-3-6) | NOTICE deps | IMPORTANTE | AINDA-FECHADO | PROV | `NOTICE` §§3-8 |
| [AUD-DOC-1](#aud-doc-1) | Bennett doc↔código | IMPORTANTE | AINDA-FECHADO | DOC | `docs/technical-reference.md` §5 |
| [AUD-DOC-3](#aud-doc-3) | SPDX 127/127 | IMPORTANTE | AINDA-FECHADO | DOC | `src/**/*.{cpp,h}` |
| [AUD-DOC-4](#aud-doc-4) | RAW/PDS4 honestos (CLAUDE+src) | IMPORTANTE | AINDA-FECHADO (gêmeos → DOC-6/7) | DOC | `CLAUDE.md` 21.1/21.2 |
| [AUD-TEST-2](#aud-test-2) | SKIP fantasma | IMPORTANTE | AINDA-FECHADO | TEST | `synthetic_fits.h` |
| [AUD-TEST-3](#aud-test-3) | Loaders cube/HDU/bintable | IMPORTANTE | AINDA-FECHADO | TEST | `test_fits_loaders.cpp` |
| [AUD-CI-4](#aud-ci-4) | `qa-wsl2.yml` órfão | IMPORTANTE | AINDA-FECHADO | CI | workflows |
| [AUD-SEC-3](#aud-sec-3) | HTTP timeout nos 3 clients | IMPORTANTE | AINDA-FECHADO (gêmeo → SEC-10) | SEC | `setTransferTimeout` |
| [AUD-SEC-4](#aud-sec-4) | Enforce https | IMPORTANTE | AINDA-FECHADO (gêmeo → SEC-13) | SEC | `isSafe*UrlScheme` |
| [AUD-MEM-4](#aud-mem-4) | RGB/Spectrum1D teto | IMPORTANTE | AINDA-ABERTO (residual = INPUT-8) | MEM | `loadSpectrum1D` |
| [AUD-SEC-9](#aud-sec-9) | Sem SBOM/syft no tree | COSMÉTICO | CONFIRMADO | SEC | CMake/workflows (ausência) |
| [AUD-PROV-9](#aud-prov-9) | Eigen citado, zero uso (primário; gêmeo DOC-9) | COSMÉTICO | CONFIRMADO | PROV | About/packaging/CONTRIBUTING |
| [AUD-DOC-9](#aud-doc-9) | Eigen fantasma (gêmeo L-17 de PROV-9) | COSMÉTICO | CONFIRMADO | DOC | ver [AUD-PROV-9](#aud-prov-9) |
| [AUD-PROV-11](#aud-prov-11) | README "every item above" falso | COSMÉTICO | CONFIRMADO | PROV | `README.md` Acknowledgments |
| [AUD-PROV-12](#aud-prov-12) | NOTICE MiniZip/fmt fino | COSMÉTICO | CONFIRMADO (impacto empacote PLAUSÍVEL) | PROV | `NOTICE` |
| [AUD-PROV-13](#aud-prov-13) | Sem LGPL full in-tree | COSMÉTICO | CONFIRMADO | PROV | `git ls-files` licenças |
| [AUD-CORR-9](#aud-corr-9) | FWHM 2.355 em Overlay/Centroid.h | COSMÉTICO | CONFIRMADO | CORR | `Overlay.h` / `Centroid.h` |
| [AUD-CI-8](#aud-ci-8) | Endurecimento em 1 de N workflows | COSMÉTICO | CONFIRMADO | CI | `permissions` / STEP_SUMMARY |
| [AUD-CI-9](#aud-ci-9) | pre-commit local não armado | COSMÉTICO | CONFIRMADO | CI | `.git/hooks/pre-commit` |
| [AUD-DOC-7](#aud-doc-7) | CHANGELOG 0.5.0 RAW/PDS sem errata | COSMÉTICO | CONFIRMADO | DOC | `CHANGELOG.md` `[0.5.0]` |
| [AUD-TEST-7](#aud-test-7) | Nome Catch2 frouxo vs assert estrito | COSMÉTICO | CONFIRMADO | TEST | `test_star_detector.cpp` |
| [AUD-SEC-5](#aud-sec-5) | ApiKeyStore 0600 | COSMÉTICO | AINDA-FECHADO (bypass = SEC-6) | SEC | `ApiKeyStore::write` |
| [AUD-CI-2](#aud-ci-2) | CRLF nos yml | COSMÉTICO | AINDA-FECHADO | CI | 11 workflows CR=0 |
| [AUD-CORR-6](#aud-corr-6) | Constante FWHM Centroid.cpp | COSMÉTICO | AINDA-FECHADO (gêmeo → CORR-9) | CORR | `kFwhmPerSigma` |
| [AUD-INPUT-6](#aud-input-6) | UB alinhamento SER | COSMÉTICO | NÃO RE-TESTADO | INPUT | `ImageLoader.cpp` locals packed |
| [AUD-CCFITS-ASAN](#aud-ccfits-asan) | `[bintable]` sob ASan | COSMÉTICO | AINDA-FECHADO (artefato) / PLAUSÍVEL (runtime) | TEST | `ccfits_lib` `-fno-sanitize=undefined` |
| [AUD-PROV-8](#aud-prov-8) | GDL/NEMO/Siril risco aceito | - | AINDA-FECHADO | PROV | `NOTICE` §9 |
| [AUD-INPUT-4](#aud-input-4) | Symlink na extração | IMPORTANTE | NÃO RE-TESTADO | INPUT | `expandArchive` / `expandZip` |
| [AUD-INPUT-5](#aud-input-5) | BINTABLE NAXIS2 | IMPORTANTE | NÃO RE-TESTADO | INPUT | `FitsTableReader` |
| [AUD-INPUT-7](#aud-input-7) | OOB coluna opcional | - | NÃO RE-TESTADO | INPUT | `FitsTableReader` |
| [AUD-INPUT-gaps](#aud-input-gaps) | Gaps RAW/PDS4/QImage/XISF | - | AINDA-ABERTO | INPUT | vários |
| [AUD-MEM-gaps](#aud-mem-gaps) | Gaps tooling memória | - | AINDA-ABERTO | MEM | valgrind/cppcheck/tidy |
| [AUD-INPUT-10](#aud-input-10) | `loadQImage` sem teto | IMPORTANTE | PLAUSÍVEL | INPUT | `loadQImage` |
| [AUD-INPUT-11](#aud-input-11) | SER produto 400e6 px | IMPORTANTE | PLAUSÍVEL | INPUT | `kMaxSerDim` |
| [AUD-INPUT-12](#aud-input-12) | BINTABLE `nRows` sem teto | IMPORTANTE | PLAUSÍVEL | INPUT | `FitsTableReader::readColumn` |
| [AUD-MEM-7](#aud-mem-7) | ClumpFind sem `isfinite` | IMPORTANTE | PLAUSÍVEL | MEM | `detectStars` blended |

---

## 4. Achados CONFIRMADOS por severidade

### 4.1 CRÍTICOS novos

<a id="aud-input-8"></a>
### AUD-INPUT-8 - `loadSpectrum1D` aloca a partir de header 1-D mentiroso

- **Severidade:** CRÍTICO - CONFIRMADO (V-INPUT + re-execução orquestrador)
- **Âncora:** `core::loadSpectrum1D` em `src/core/Spectrum1D.cpp` (blob HEAD). Rejeita só `N <= 0`; em seguida `std::vector<float> raw(static_cast<size_t>(N))` **antes** de `fits_read_pix`, sem `kMaxImagePixels` / `validateImageDims` / `QFileInfo::size()`. Caller UI: `MainWindow_io.cpp` (quando `loadFits` falha em `.fits`/`.fit`/`.fts`); `grep catch(` no caller = vazio (finder; não reaberto).
- **Failure concreto:** FITS 2880 B, NAXIS=1, NAXIS1=100000000 → ASan soft RSS limit (256 Mb vs 417 Mb) no frame `loadSpectrum1D` / `Spectrum1D.cpp:65`; MAXRSS ~427-461 MiB. N=250000000 → MAXRSS ~990 MiB, depois erro de leitura. Controle 2-D com o mesmo NAXIS1 é rejeitado por teto 20000 (`loadFits`, RSS ~39 MiB). Sanidade NAXIS1=8: OK. Classe irmã de AUD-INPUT-2 (OOM / `bad_alloc` → terminate em slot Qt) para N ainda maior.
- **Repro:** `ASAN_OPTIONS=detect_leaks=0:halt_on_error=0:soft_rss_limit_mb=256:hard_rss_limit_mb=800 input_asan_harness loadSpectrum1D hostile/spec_naxis1_1e8.fits`. Harness md5 `d96c17485bf68a23a926f12320d214c2`. Fonte `Spectrum1D.cpp` md5 blob HEAD `cf9164b927b7320b8c79f11c34974609`.
- **Remediação sugerida:** teto em `loadSpectrum1D` espelhando `validateImageDims` / produto máximo + cross-check `QFileInfo::size()`; nunca alocar antes de validar; try/catch no caller UI. Cruzamento: [AUD-MEM-4](#aud-mem-4) residual = este ID.
- **Confirmado por:** V-INPUT + orquestrador.

<a id="aud-mem-6"></a>
### AUD-MEM-6 - heap-buffer-overflow em `subtractBackground` (uma tile no eixo)

- **Severidade:** CRÍTICO - CONFIRMADO (V-MEM + re-execução orquestrador)
- **Âncora:** `core::subtractBackground` em `src/core/Calibration.cpp`. Com `nTX==1` ou `nTY==1`, `tx0`/`ty0` clampam via `nTX-2`/`nTY-2` e **sempre** leem `tx1=tx0+1`, `ty1=ty0+1` em `tileVal` de tamanho `nTX*nTY`. Default `tileSize=64` (`Calibration.h`).
- **Failure concreto:** `FitsImage` 128×64, `subtractBackground(img, 64)` sob ASan: `heap-buffer-overflow` READ size 4 em `Calibration.cpp:291`; região de 8 bytes (`nTX*nTY = 2` floats). EXIT 134. Aritmética: W=128 H=64 tile=64 → nTX=2 nTY=1; `ty1=1` lê `tileVal[ty1*nTX+…]` fora. Controlo 128×128 tile 64 (`nTX=nTY=2`): EXIT 0, ASan silencioso. cppcheck **não** reportou o OOB (falso negativo estático).
- **Repro:** `mem_asan_harness mem6`. md5 `a4c5b9807a9c17368a73f86f91b4d185`.
- **Remediação sugerida:** se `nTX<2` ou `nTY<2`, usar vizinho constante / não interpolar além de `nTX*nTY-1`; clamp `tx1 < nTX` e `ty1 < nTY`; teste 128×64 e 64×128. Este OOB é distinto do gap julho (float→int no denominador, refutado por análise).
- **Confirmado por:** V-MEM + orquestrador.

<a id="aud-mem-5"></a>
### AUD-MEM-5 - `forwardFFT` aloca `w*(h/2+1)` e planeja `h*(w/2+1)`

- **Severidade:** CRÍTICO - CONFIRMADO (overflow ASan + aritmética). Código **não** remediado.
- **Âncora:** `forwardFFT` em `src/core/ImageStacker.cpp`: aloca `nc = w*(h/2+1)` e chama `fftw_plan_dft_r2c_2d(h, w, ...)`. `phaseCorrelation` usa o tamanho **correto** `h*(w/2+1)` ao chamar `crossPowerSpectrum`. Blob verificado: git object `c09ecf69c2cf693691fb31da9603a6fed40e59f8`.
- **Failure concreto (medido):** aritmética 50×100: nc_fwd=2550 vs nc_plan=2600 UNDER-ALLOC. Runtime ASan (`01/09/26 - 09:01:42`, orquestrador; V-MEM5): `mem5_asan_harness under` EXIT 134; `heap-buffer-overflow` **READ** em `crossPowerSpectrum` `ImageStacker.cpp:52`; região 40800 B = 2550×16 alocada em `forwardFFT` L34 via `allocComplex`; stack `stackImages` AlignMode::FFT L333 → `phaseCorrelation` L88. Controlo `equal` 32×32 e `over` 100×50: EXIT 0, ASan silencioso. 1080×1920 não executado (50×100 já conclusivo). L-21: CCD portrait é entrada real.
- **Primeiro trap:** READ em `crossPowerSpectrum`. **FATO:** `libfftw3.so` do sistema **não** está instrumentada; WRITE em `fftw_execute` pode existir e não ter sido o primeiro hit. Isso não enfraquece o overflow: o buffer de `forwardFFT` é curto demais para o plano e para o `nc` que `phaseCorrelation` consome.
- **Repro:** `mem5_asan_harness under`. Harness md5 `9191cb6c1c0d9a8770ed63996eb5a3e6`. Pacote `fftw-devel-3.3.10-17.fc44.x86_64`. Fonte: `/var/tmp/astrofind-audit-eventual-20260901/reports/V-MEM5.md`.
- **Remediação sugerida:** alocar `h*(w/2+1)` em `forwardFFT` (mesmo contrato de `phaseCorrelation`); teste 50×100 (e 1080×1920) sob ASan; não confiar em 32×32.
- **Confirmado por:** V-MEM (aritmética) + V-MEM5 (runtime ASan) + re-execução do orquestrador.

### 4.2 CRÍTICOS de julho ainda abertos / residual

Nenhum dos 5 CRÍTICOS de produto de julho **REGREDIU**. Ver [§4.6](#46-re-teste-julho). Residual de memória de julho que **não** fechou:

<a id="aud-mem-1"></a>
### AUD-MEM-1 - leak SEP ainda no binário linkado (AINDA-ABERTO)

- **Severidade:** IMPORTANTE (não sobe a CRÍTICO: leak por chamada, não crash/RCE) - CONFIRMADO residual
- **Âncora:** `sep convert_to_catalog` (`extract.c:1094-1095`); `core::detectStars` chama `sep_catalog_free` (StarDetector.cpp). Patch versionado `cmake/patches/sep-leak-fix.patch`. `PATCH_COMMAND … || true` em `cmake/dependencies.cmake:105`.
- **Failure concreto:** LSan no harness `mem1` (gaussiana 64×64, 1 estrela): Direct leak 4+4 B em `convert_to_catalog`. Valgrind no freeze PLAIN `[stardetector]` (5 estrelas): 40 B definitely lost (20 B cflux + 20 B flux). Fonte em `build/_deps/sep-src` já sem o par duplicado (mtime 2026-07-10). `libsep_lib.a` (mtime 2026-03-21) compilado a partir de `projetos_dev/.../sep-src` (DWARF); **pré-patch**. O archive linkado **não** foi rebuildado após o patch de julho.
- **Repro:** `ASAN_OPTIONS=detect_leaks=1 mem_asan_harness mem1`; valgrind `astrofind_tests` filtro sem vírgula `StarDetector: detects stars in synthetic image`.
- **Remediação sugerida:** rebuild de `libsep_lib.a` a partir do `_deps` patcheado; remover `|| true` do `PATCH_COMMAND` (falha de patch tem de quebrar o configure); LSan `detect_leaks=1` no portão CI.
- **Confirmado por:** V-MEM. Não é AINDA-FECHADO.

<a id="aud-mem-4"></a>
### AUD-MEM-4 - residual Spectrum1D (AINDA-ABERTO = INPUT-8)

RGB/`validateImageDims` barram o equivalente 2-D ([AUD-INPUT-2](#aud-input-2) AINDA-FECHADO). Metade Spectrum1D do achado de julho permanece e foi re-medida como [AUD-INPUT-8](#aud-input-8). Não duplicar o CRÍTICO.

### 4.3 IMPORTANTES novos

<a id="aud-input-9"></a>
### AUD-INPUT-9 - XISF aceita eixo acima do teto FITS

- **Severidade:** IMPORTANTE - CONFIRMADO
- **Âncora:** `loadXisf` / `ImageLoader.cpp` (geometry `toInt()`; guard só `<=0` / canais; sem `validateImageDims` / `kMaxImagePixels`).
- **Failure concreto:** XISF truncado 50000:50000:1 fail-clean por truncagem (mitigação parcial). Geometry **20001:20001:1** (acima de `kMaxImageDim=20000`) + payload UInt8 ~400 MiB: ASan hard RSS limit 1536 Mb vs 1729 Mb, MAXRSS ~1,7 GiB. L-21: produto para distribuição não pode deixar XISF mais frouxo que FITS.
- **Repro:** `input_asan_harness loadImage hostile/xisf_geom_20001.xisf` com `hard_rss_limit_mb=1536`.
- **Remediação sugerida:** aplicar `validateImageDims` / `kMaxImagePixels` em `loadXisf` **antes** de alocar; teste 20001 vs 20000.

<a id="aud-ci-5"></a>
<a id="aud-ci-3"></a>
### AUD-CI-5 / AUD-CI-3 - `audit.yml` `runs-on: docker` (REGREDIU o gap de julho)

- **Severidade:** IMPORTANTE - CI-5 CONFIRMADO; CI-3 **REGREDIU**. **Mesmo fato:** não duplicar o crash. CI-3 é o ID de julho; CI-5 é a medição 2026-09-01 no host GitHub.
- **Âncora:** `.github/workflows/audit.yml` L55 `runs-on: docker`; comentário L11-12 `forgejo-runner.service` / label `docker`. Commit `0940509` removeu `.forgejo/` e **não** tocou `audit.yml`.
- **Failure concreto:** `gh run list` no SHA HEAD = 10 runs success (build + 9 qa). Numerical Audit **não disparou** neste push (trigger: PR→main, tags `v*`, `workflow_dispatch`). Histórico `audit.yml`: `{cancelled: 9}`, 0 success, 0 failure. Run `29138252749`: `The job has exceeded the maximum execution time while awaiting a runner for 24h0m0s`; `labels:["docker"]`, `runner_name:""`, `steps:[]`. YAML + `cmake/audit.cmake` existem; o portão nunca foi visto vermelho por sanitizer (L-36). Residual Forgejo no comentário (L-29: host único = GitHub).
- **Remediação sugerida:** `runs-on` hosted GitHub (`ubuntu-24.04` ou label que exista); apagar menção `forgejo-runner`; disparar o job e **provar vermelho** com sabotagem de estreia (L-36). Cruzamento: [AUD-CI-7](#aud-ci-7) (mesmo após o runner, clang-tidy continua fora do gate).

<a id="aud-sec-6"></a>
### AUD-SEC-6 - SetupWizard grava `astrometry/apiKey` sem `ApiKeyStore`

- **Severidade:** IMPORTANTE - CONFIRMADO (fato de código). Efeito 0644 no conf = PLAUSÍVEL (wizard GUI não rodado).
- **Âncora:** `git show HEAD:src/ui/SetupWizard.cpp`: `ApiKeyPage::save` faz `s.setValue("astrometry/apiKey", …)`; zero menção a `ApiKeyStore`. `SettingsDialog` / `MainWindow_reduction` usam `ApiKeyStore::write` (que chama `restrictSettingsFilePermissions`).
- **Remediação sugerida:** wizard chamar `ApiKeyStore::write`; não gravar a chave via `QSettings` cru.

<a id="aud-sec-7"></a>
### AUD-SEC-7 - arquivo `.runner` ainda em disco (residual de SEC-1)

- **Severidade:** IMPORTANTE - CONFIRMADO (presença). Validade do token no Codeberg = PLAUSÍVEL (não verificável só por disco). **L-28:** valor do token omitido; só path/mode/rule.
- **Âncora:** `.runner` na raiz. `git check-ignore` casa `.gitignore:75:.runner*`. `stat` mode **600**. Não rastreado. gitleaks `--all` (109 commits) **no leaks found**. gitleaks `--no-git`: 1 finding, RuleID `generic-api-key`, File `.runner`, StartLine 6, Fingerprint `.runner:generic-api-key:6`, Secret_len 40. `address` = `https://codeberg.org`. `systemctl is-active forgejo-runner.service` = inactive. Remote só GitHub.
- **Cruzamento:** [AUD-SEC-1](#aud-sec-1) permanece AINDA-ABERTO (controles git presentes; credencial residual + [AUD-SEC-8](#aud-sec-8) sem scanner). Não republicar o segredo.
- **Remediação sugerida:** apagar `.runner` local após confirmar que o runner Codeberg está morto; rotacionar/revogar o token no host antigo (decisão do líder); não versionar o valor.

<a id="aud-sec-8"></a>
### AUD-SEC-8 - zero scanner de secret em hook/CI

- **Severidade:** IMPORTANTE - CONFIRMADO (ausência re-medida)
- **Âncora:** `rg gitleaks|trufflehog|detect-secrets|secret.scan` em `scripts/` e `.github/` = vazio. `scripts/pre-commit` = build+ctest; bypass `SKIP_TESTS=1`.
- **Remediação sugerida:** gitleaks (ou equivalente) no pre-commit e num job GitHub; L-28 no histórico (`git log --all -p`), não só `git grep`.

<a id="aud-sec-10"></a>
### AUD-SEC-10 - `KooEngine` e downloads MPCORB/DAILY sem `setTransferTimeout`

- **Severidade:** IMPORTANTE - CONFIRMADO (gap de código). Hang contra host real = **não executado** (PLAUSÍVEL).
- **Âncora:** `KooEngine::queryField`: `nam_->get` sem timeout, sem cancel de reply; `busy_=true` antes do get. Gêmeos: MPCORB/DAILY em `MainWindow_measurement.cpp` e `SetupWizard.cpp`. Os três clients de julho (Astrometry/Catalog/Horizons) **têm** timeout ([AUD-SEC-3](#aud-sec-3) AINDA-FECHADO). L-17: remediação não propagou ao 4º client.
- **Remediação sugerida:** `setTransferTimeout` + guardar `QNetworkReply*` + `cancel()` em `KooEngine` e nos downloads UI, espelhando `MpcSubmit`.

<a id="aud-sec-11"></a>
### AUD-SEC-11 - poll de astrometry.net: `get` não vai a `currentReply_`

- **Severidade:** IMPORTANTE - CONFIRMADO (código). Progresso fantasma em runtime = PLAUSÍVEL.
- **Âncora:** timer WaitingForJob/PollingJob: `auto* reply = nam_->get(req)` **não** atribuído a `currentReply_`; `cancel()` só `pollTimer_->stop()` + `abort` de `currentReply_`.
- **Remediação sugerida:** toda `get` de poll em `currentReply_`; `cancel()` aborta o reply vivo.

<a id="aud-sec-12"></a>
### AUD-SEC-12 - Horizons `COMMAND` interpola `target_` do usuário

- **Severidade:** IMPORTANTE - CONFIRMADO (construção). Quebra de gramática Horizons = PLAUSÍVEL (payload não enviado).
- **Âncora:** `HorizonsClient::query`: `COMMAND = "'%1'".arg(target_)`; timeout 30 s presente; URL base https fixa.
- **Remediação sugerida:** allowlist / escape de `target`; rejeitar aspas e `;`.

<a id="aud-sec-13"></a>
### AUD-SEC-13 - save de URL sem `isSafe*`

- **Severidade:** IMPORTANTE - CONFIRMADO (fato de código)
- **Âncora:** save grava `astrometry/baseUrl`, `catalog/vizierServer`, `mpc/submitUrl` sem `isSafe*` (`isSafe` count em SettingsDialog = 0). Reset VizieR = hostname nu `vizier.cfa.harvard.edu`. Apply em `MainWindow_settings.cpp` usa setters (que rejeitam não-https). [AUD-SEC-4](#aud-sec-4) AINDA-FECHADO nos clients; este é o gêmeo save-time.
- **Remediação sugerida:** validar no save com as mesmas `isSafe*`; não gravar URL que o setter recusaria.

<a id="aud-prov-10"></a>
### AUD-PROV-10 - paridade textual forte de menus com Astrometrica.exe

- **Severidade:** IMPORTANTE - CONFIRMADO (23 identidades exatas; finder inflou 27 por substring)
- **Âncora:** `tr("…")` em `src/ui/MainWindow.cpp` vs `originals/Astrometrica.exe.strings.txt` (local, gitignored). scancode **não executado**.
- **Failure concreto:** 23 rótulos de menu com identidade **exata** (ex. `&Moving Object Detection...`, `&Data Reduction...`, `&Blink Images`). 4 near-matches só substring (`View &Standard Toolbar` vs `&Standard Toolbar`, etc.). Nenhum símbolo de algoritmo AstroFind no strings do exe; erros distintivos do exe (`LinLsqFit`, `Unhandeled`, `Peihelion`) **ausentes** de `src/` ([AUD-PROV-8](#aud-prov-8) não reabre). Risco clean-room/UX-string, não cópia de motor.
- **Remediação sugerida:** decisão do líder/CLO: reescrever `tr(...)` de menu para redação própria, ou documentar paridade UX como risco aceito (não misturar com PROV-8).

<a id="aud-corr-7"></a>
### AUD-CORR-7 - política: refração sempre após `pixToSky`, inclusive plate-solved

- **Severidade:** IMPORTANTE - CONFIRMADO (gap de política / cobertura). Erro em graus / ~1.75′ = **PLAUSÍVEL** (harness de estrelas refratadas × catálogo ICRS **não executado**). **Não** CRÍTICO sem ângulo medido.
- **Âncora:** blob `MainWindow_measurement.cpp` SHA `0f64ef93…`: se `img.wcs.solved` → `pixToSky` → `raFinal`/`decFinal`; depois, se `!img.isSpaceTelescope && img.jd > 2400000` → **sempre** `applyRefractionCorrection`. Sem ramo header-WCS vs plate-solve. Doc `technical-reference.md` manda não reaplicar aberração/precessão/nutação (plate-solve absorve) e **aplicar** refração antes do ADES/MPC. `[refraction]` cobre Bennett, não a política.
- **Remediação sugerida:** decidir (líder) se plate-solved ICRS deve ou não receber Bennett; testar o ramo; não inventar o Δ agora.

<a id="aud-corr-8"></a>
### AUD-CORR-8 - extinção do MPC é inline na UI; `applyExtinctionCorrection` morta

- **Severidade:** IMPORTANTE - CONFIRMADO
- **Âncora:** `applyExtinctionCorrection` só em definição + testes. Pipeline MPC: `extCorr = -kExt * airmass` em `MainWindow_measurement.cpp:154` e soma em `obs.mag` (:178-185). Comentário em `test_photometry.cpp:401-402` admite que o inline UI não tem unit.
- **INFERÊNCIA:** mutação do sinal no inline deixaria `[AUD-CORR-5]` verde (rede na função morta). Mutação do inline não executada nesta onda; fato de código basta.
- **Remediação sugerida:** religar a função no pipeline **ou** testar o inline; uma fonte de verdade.

<a id="aud-corr-10"></a>
### AUD-CORR-10 - parse WCS não lê LONPOLE/LATPOLE/PV1_*

- **Severidade:** IMPORTANTE - CONFIRMADO (código). Delta vs astropy com LONPOLE não-default = PLAUSÍVEL (oráculo não rodado).
- **Âncora:** leitura WCS em `FitsImage.cpp` popula CRVAL/CRPIX/CD/CTYPE; grep HEAD de LONPOLE = só comentários de default. Zero teste LONPOLE em `tests/`.
- **Remediação sugerida:** ler os cards e alimentar `celestialPole`; oráculo astropy com LONPOLE=90/180. L-21: não cortar FITS com polo não-default.

<a id="aud-corr-11"></a>
### AUD-CORR-11 - Catch2 `[wcs]` sem polo sul e sem Dec negativa nas 5 projeções

- **Severidade:** IMPORTANTE - CONFIRMADO (buraco de suíte). Código **não** regrediu: oráculo H3 `poleS_*` e `south_*` ×8 projeções OK vs astropy 8.0.1.
- **Âncora:** Catch2 HEAD: near-pole só `dec=89.9`; Dec negativa absoluta só TAN/CAR/AIT.
- **Remediação sugerida:** acrescentar `dec=-89.9` e sul nas 8 projeções na suíte (não só no harness eventual).

<a id="aud-corr-12"></a>
### AUD-CORR-12 - zero testes de Ephemeris/KooEngine

- **Severidade:** IMPORTANTE - CONFIRMADO (ausência)
- **Âncora:** `rg Ephemeris|KooEngine|solveKepler|computeEphemeris` em `tests/` = vazio. Correção numérica **não verificada**.
- **Remediação sugerida:** oráculo Horizons/Meeus para `solveKepler` / efeméride; não cortar KOO por "suíte atual não usa" (L-21).

<a id="aud-corr-13"></a>
### AUD-CORR-13 - JD a partir de DATE-OBS trunca sub-segundo

- **Severidade:** IMPORTANTE - CONFIRMADO (truncamento). Magnitude no ADES/refração = PLAUSÍVEL (fixture `.5` s × astropy Time **não executada**).
- **Âncora:** `FitsImage.cpp` ~701-704: `jd = 2451545.0 + j2000.secsTo(dateObs)/86400.0 + expTime/172800.0`. `QDateTime::secsTo` devolve segundos inteiros. Sem `MJD-OBS`/`DATE-AVG` no parse.
- **Remediação sugerida:** usar fração (msecs / `MJD-OBS`); teste DATE-OBS com `.5` s vs astropy.

<a id="aud-corr-14"></a>
### AUD-CORR-14 - ADES escreve `ICRF` sem assert; comentário "apparent"

- **Severidade:** IMPORTANTE - CONFIRMADO
- **Âncora:** XML/PSV escrevem `ICRF` (`AdesReport.cpp:224-226`). Comentário: "topocentric apparent place in the ICRF". `rg ICRF|sys` em `test_ades_report.cpp` = vazio. Doc: ICRF porque o plate-solve é ICRS, **não** porque aplica aberração ([AUD-CORR-4](#aud-corr-4) / [AUD-DOC-8](#aud-doc-8)).
- **Remediação sugerida:** assert de `sys`; alinhar comentário à prática (aberração só log).

<a id="aud-ci-6"></a>
### AUD-CI-6 - Pop!_OS 22.04 e Zorin 17 rodam Ubuntu 24.04

- **Severidade:** IMPORTANTE - CONFIRMADO
- **Âncora:** `qa-pop-os-22.yml` / `qa-zorin-17.yml`: comentário `# IMAGE USED: ubuntu:22.04`, claims Qt 6.4 / GCC 12; `image: ubuntu:24.04`; echo ainda imprime `(via ubuntu:22.04)`. Commit `67bcc92` trocou a imagem sem alinhar comentários.
- **Failure concreto (`gh run view --log` no SHA HEAD):** Pop `32379357823` e Zorin `32379357723`: `PRETTY_NAME="Ubuntu 24.04.4 LTS"`, GCC 13.3.0. L-21: não apagar Pop/Zorin por duplicata; corrigir honestidade ou repor base 22.04 real.
- **Remediação sugerida:** alinhar nome/comentário/echo à imagem real, **ou** restaurar aproximação 22.04 se o produto ainda declara.

<a id="aud-ci-7"></a>
### AUD-CI-7 - gate de `audit.yml` não lê clang-tidy

- **Severidade:** IMPORTANTE - CONFIRMADO
- **Âncora:** step "Gate on critical findings" parseia só `asan_tests.txt`, `cppcheck.xml`, `valgrind.xml`. `mentions clang_tidy.txt? False`. Ausência de cppcheck/valgrind = `::warning` (não FAIL); só ASan ausente falha duro. `cmake/audit.cmake`: cppcheck `--error-exitcode=0`; clang-tidy `|| true`; valgrind `--error-exitcode=0`. Item 38.4 no nome do workflow.
- **Remediação sugerida:** gate ler `clang_tidy.txt`; error-exitcode honesto; L-36: provar o gate vermelho.

<a id="aud-doc-5"></a>
### AUD-DOC-5 - SECURITY.md afirma que o dossiê está 100% corrigido

- **Severidade:** IMPORTANTE - CONFIRMADO (efeito de trust; não só drift)
- **Âncora:** `SECURITY.md` L34-36 EN "every issue listed there has already been fixed and released"; L69-72 PT equivalente. Last-reviewed 2026-07-10. `TODO.md` ainda marca `AUD-INPUT-gaps` e `AUD-MEM-gaps` como `🟡 Parcial` (leitura nesta sessão; este livro **não** editou TODO).
- **Remediação sugerida:** retratar a frase; apontar para IDs abertos por âncora.

<a id="aud-doc-6"></a>
### AUD-DOC-6 - Help in-app anuncia RAW DSLR e PDS4

- **Severidade:** IMPORTANTE - CONFIRMADO (feature-fantasma user-facing; gêmeo L-17 de [AUD-DOC-4](#aud-doc-4))
- **Âncora:** `help_en.html` L417-418; `help_pt_br.html` L421-422. `rg libraw|PDS4|.cr2` em `src/` = 0. Key Features do mesmo Help **omite** RAW/PDS (inconsistência interna).
- **Remediação sugerida:** remover os itens do Help ou marcar "não implementado", como `CLAUDE.md` 21.1/21.2.

<a id="aud-doc-8"></a>
### AUD-DOC-8 - Help descreve cadeia ICRS→CIRS com aberração e nutação

- **Severidade:** IMPORTANTE - CONFIRMADO (claim científico no Help)
- **Âncora:** Help EN L281 / PT L283 "cadeia completa"; glossário CIRS EN L1300-1301 "including annual aberration and nutation … ADES". `technical-reference.md` §6 (AUD-CORR-4): precessão/nutação removidas; aberração só logada; correção de frame real = refração.
- **Remediação sugerida:** alinhar Help à prática; não vender correção que não entra no RA/Dec exportado.

<a id="aud-test-4"></a>
### AUD-TEST-4 - suíte Catch2 não regressa os CRÍTICOS de julho

- **Severidade:** IMPORTANTE - CONFIRMADO (gap de suíte; produto INPUT-1/2 e MEM-2/3 AINDA-FECHADOS no código)
- **Âncora:** `tests/`: sem `AUD-INPUT-1/2`, `AUD-MEM-2/3`; sem NAXIS=4 / teto 100000; sem `isfinite` em centroid. Tags `[regression]`: só INPUT-5 e INPUT-7.
- **Failure concreto (mutação L-27, cópia `/var/tmp`):** 4 sites `naxis > kMaxImageAxes` → `naxis > 999`; `[loaders]` 13/13 verde, EXIT 0. Probe NAXIS=4: HEAD "Unsupported NAXIS=4"; M1 "Error reading pixel data … (bad first element number)". O guard deixou de rejeitar cedo; a suíte não notou.
- **Remediação sugerida:** casos hostis NAXIS=4, 100k×100k, pixel +Inf no centroide, **vistos falhar** (L-35) antes de fechar.

<a id="aud-test-5"></a>
### AUD-TEST-5 - nome Catch2 com vírgula parte o filtro (falso-verde)

- **Severidade:** IMPORTANTE - CONFIRMADO (V-TEST + orquestrador H4). 19/186 `TEST_CASE` com vírgula.
- **Âncora:** L-45. Exemplo: `WCS: reference pixel maps to CRVAL sub-arcsec, all 8 projections`.
- **Failure concreto:** `corr_catch_tests "WCS: reference pixel maps to CRVAL sub-arcsec, all 8 projections"` → Filters parte em dois; `No tests ran`; EXIT 0. Controle por tag `[wcs]` lista o caso. Risco se CI/script filtrar por nome; tags evitam hoje.
- **Remediação sugerida:** remover vírgulas dos nomes; filtro por tag.

<a id="aud-test-6"></a>
### AUD-TEST-6 - loaders SER/XISF/QImage/Spectrum1D/archives sem `TEST_CASE`

- **Severidade:** IMPORTANTE - CONFIRMADO (gap; L-21)
- **Âncora:** `loadSer` / `loadXisf` / `loadQImage` / `loadSpectrum1D` / `expandZip` / `expandArchive` / `importReductionTable`: 0 casos em `tests/*.cpp` (só comentário para a última). [AUD-TEST-3](#aud-test-3) cobriu cube/HDU/bintable.
- **Remediação sugerida:** T1 por loader; hostis de [AUD-INPUT-8](#aud-input-8)/[AUD-INPUT-9](#aud-input-9) na suíte.

### 4.4 COSMÉTICOS novos

<a id="aud-sec-9"></a>
### AUD-SEC-9 - sem SBOM no tree / sem job syft

CONFIRMADO ausência (`ls` SBOM* vazio; `rg syft|sbom|spdx|cyclonedx` em CMake/workflows vazio nesta passagem). Residual da remediação [AUD-SEC-2](#aud-sec-2) (pins OK; SBOM não nasceu). Remediação: target/job syft; não é crash.

<a id="aud-prov-9"></a>
<a id="aud-doc-9"></a>
### AUD-PROV-9 (primário) / AUD-DOC-9 (gêmeo L-17) - Eigen citado, zero uso

- **Severidade:** COSMÉTICO - CONFIRMADO
- **FATO:** `#include` / `Eigen::` / `find_package(Eigen*)` em `src/`, `cmake/`, `CMakeLists.txt` = vazio. NOTICE/README/INSTALL **não** listam Eigen. Citações órfãs: `AboutDialog.cpp:131`, `CONTRIBUTING.md:34,49`, `CLAUDE.md` stack, `packaging/debian/control` `libeigen3-dev`, `packaging/rpm/astrofind.spec` `eigen3-devel`, `packaging/arch/PKGBUILD` `'eigen'`, `packaging/install.sh:201-202`, Help créditos EN/PT, i18n About. V-DOC ampliou a superfície (L-17); **um** primário (PROV-9) + ponte DOC-9. Não é dep fantasma no binário; é atribuição falsa / BuildRequires inútil.
- **Remediação sugerida:** uma varredura: About, Help, CONTRIBUTING, CLAUDE stack, packaging, i18n. Não deixar gêmeo.

<a id="aud-prov-11"></a>
### AUD-PROV-11 - README "every item above" / "cada item acima" não cobre a tabela

CONFIRMADO: README L506/L537. NOTICE tem Siril/GDL/NEMO/WCSLIB; **ausentes** Astrometrica/Herbert Raab, find_orb, umbrella2, astrometry.net, IRAF, Boost.Astronomy. Overclaim documental.

<a id="aud-prov-12"></a>
### AUD-PROV-12 - NOTICE MiniZip/fmt incompleto

CONFIRMADO gap fino (MiniZip sem texto zlib; fmt só URL). Impacto em pacote instalado PLAUSÍVEL.

<a id="aud-prov-13"></a>
### AUD-PROV-13 - nenhum texto LGPL full rastreado

CONFIRMADO: `LICENSE` = AGPL; hyperlinks no NOTICE. Residual de PROV-2. Cópia em `build/_deps` não é fonte de distribuição versionada.

<a id="aud-corr-9"></a>
### AUD-CORR-9 - FWHM 2.355 residual (gêmeo L-17 de CORR-6)

CONFIRMADO: `Overlay.h:36` `2.355 * (a+b)*0.5`; `Centroid.h:49` doc 2.355. `Centroid.cpp` já `2.354820045` ([AUD-CORR-6](#aud-corr-6) AINDA-FECHADO).

<a id="aud-ci-8"></a>
### AUD-CI-8 - endurecimento em 1 de N (L-17)

CONFIRMADO inventário: `permissions:` só `audit.yml` (1/11); `GITHUB_STEP_SUMMARY` 0/11; `FORCE_JAVASCRIPT_ACTIONS_TO_NODE24` nos 9 qa-*, ausente em `build.yml` e `audit.yml`.

<a id="aud-ci-9"></a>
### AUD-CI-9 - hook pre-commit não instalado neste clone

CONFIRMADO: `scripts/pre-commit` existe; `test -e .git/hooks/pre-commit` exit 1. Higiene local; bypass `SKIP_TESTS=1`.

<a id="aud-doc-7"></a>
### AUD-DOC-7 - CHANGELOG `[0.5.0]` RAW/PDS sem errata em `[0.9.0]`

CONFIRMADO. Cosmético (histórico); o claim vivo no Help é [AUD-DOC-6](#aud-doc-6).

<a id="aud-test-7"></a>
### AUD-TEST-7 - nome `returns stars or succeeds` vs `CHECK(size()==0)`

CONFIRMADO higiene. Assert **não** circular.

### 4.5 Encapsulamento core (negativo)

V-CI: `#include` de fitsio/CCfits/sep/nlohmann/spdlog/Eigen/fftw/quazip/archive em `src/core/*.h` = vazio. Sem achado. CONFIRMADO negativo.

### 4.6 Re-teste julho

Só veredito. Detalhe original vive no blob git de 2026-07-10.

| ID | Veredito 2026-09-01 | Evidência curta |
|---|---|---|
| AUD-INPUT-1 | AINDA-FECHADO | NAXIS 4/5/10 → Unsupported; ASan silencioso; RSS~39M. Orquestrador. |
| AUD-INPUT-2 | AINDA-FECHADO | 100k×100k → ceiling 20000; RSS~39M. Orquestrador. |
| AUD-INPUT-3 | AINDA-FECHADO | SER sign/0xFFFFFFFF → out of range |
| AUD-INPUT-4 | NÃO RE-TESTADO | Guard no tree; harness sem expandZip/Archive |
| AUD-INPUT-5 | NÃO RE-TESTADO | Freeze Catch2/spdlog shared ausentes; teste `[AUD-INPUT-5]` existe |
| AUD-INPUT-6 | NÃO RE-TESTADO | Locals packed no HEAD; SER válido UBSan dedicado não rodado |
| AUD-INPUT-7 | NÃO RE-TESTADO | Idem INPUT-5; guard + teste existem |
| AUD-INPUT-gaps | AINDA-ABERTO | RAW/PDS4 rg vazio; XISF via INPUT-9; QImage ainda aberto |
| AUD-MEM-1 | AINDA-ABERTO | ver [§4.2](#aud-mem-1) |
| AUD-MEM-2 | AINDA-FECHADO | nullopt vivo; mutação dual-guard → UBSan signed overflow. Orquestrador. |
| AUD-MEM-3 | AINDA-FECHADO | PSF/elíptico + clicks NaN/Inf rejeitam |
| AUD-MEM-4 | AINDA-ABERTO | residual = INPUT-8 |
| AUD-MEM-gaps | AINDA-ABERTO | tools rodados; cppcheck não viu MEM-6; tidy sem fftw |
| AUD-CORR-1 | AINDA-FECHADO | 80/80 astropy 8.0.1; mutação CAR→270 prova a rede. Orquestrador. |
| AUD-CORR-2 | AINDA-FECHADO | `[wcs]` 7 cases / 226 asserts; residual → CORR-11 |
| AUD-CORR-3 | AINDA-FECHADO | `[refraction]` 4/29; residual política → CORR-7 |
| AUD-CORR-4 | AINDA-FECHADO | `[aberration]` 2/33; dead code continua ausente |
| AUD-CORR-5 | AINDA-FECHADO | `[AUD-CORR-5]` 4/17; residual inline → CORR-8 |
| AUD-CORR-6 | AINDA-FECHADO | `kFwhmPerSigma = 2.354820045`; gêmeo → CORR-9 |
| AUD-SEC-1 | AINDA-ABERTO | gitignore+600+histórico limpo; residual disco = SEC-7; sem scanner = SEC-8 |
| AUD-SEC-2 | AINDA-FECHADO (6 pins MATCH=YES) | residual CCfits URL 404 + `GIT_TAG v2.7`; bundle `originals/CCfits.tar.gz` presente |
| AUD-SEC-3 | AINDA-FECHADO | timeout nos 3; gêmeo → SEC-10 |
| AUD-SEC-4 | AINDA-FECHADO | `isSafe*`; gêmeo save → SEC-13 |
| AUD-SEC-5 | AINDA-FECHADO (ApiKeyStore) | bypass wizard = SEC-6 |
| AUD-PROV-1 | AINDA-FECHADO | NOTICE §1 + FitsImage.cpp:27 |
| AUD-PROV-2 | AINDA-FECHADO | NOTICE §2 + pin/patch; residual → PROV-13 |
| AUD-PROV-3-6 | AINDA-FECHADO | pins cmake ↔ NOTICE §§3-8 |
| AUD-PROV-8 | AINDA-FECHADO | sem cópia literal algoritmo/erros; risco aceito; menus = PROV-10 |
| AUD-CI-2 | AINDA-FECHADO | CR=0 / CRLF=0 nos 11 yml |
| AUD-CI-3 | REGREDIU | ver [AUD-CI-5](#aud-ci-5) |
| AUD-CI-4 | AINDA-FECHADO | `qa-wsl2.yml` ausente; `gh workflow list` sem WSL |
| AUD-DOC-1 | AINDA-FECHADO | Bennett alinhado doc↔código |
| AUD-DOC-3 | AINDA-FECHADO | SPDX 127 encontrados / 127 analisados / 127 com / 0 sem |
| AUD-DOC-4 | AINDA-FECHADO (CLAUDE+src) | gêmeos Help/CHANGELOG → DOC-6/7 |
| AUD-TEST-2 | AINDA-FECHADO | 0 `SKIP(` código; `[functional]` completo não rodado (fftw) |
| AUD-TEST-3 | AINDA-FECHADO | 13 `[loaders]` verdes; mutação `loadFitsCube` → 1 failed |
| AUD-CCFITS-ASAN | AINDA-FECHADO (CMake/tag) | 7 `[bintable]` plain OK; runtime ASan **não** re-provado |

---

## 5. PLAUSÍVEIS (separados; sem repro observável nesta onda)

Não promover a CONFIRMADO. Não misturar com §4.

<a id="aud-input-10"></a>
### AUD-INPUT-10 - `loadQImage` sem teto pós-Qt

V-INPUT: TIFF/PNG 16-bit enorme **não fabricado**. Código sem `validateImageDims` após `QImage` válido (finder). IMPORTANTE preliminar.

<a id="aud-input-11"></a>
### AUD-INPUT-11 - SER aceita 20000² = 400e6 px (> `kMaxImagePixels` 200e6)

V-INPUT: payload multi-GB **não fabricado**. INPUT-3 (sinal) fechado; paridade de produto aberta. L-21.

<a id="aud-input-12"></a>
### AUD-INPUT-12 - BINTABLE `nRows` sem teto

V-INPUT: harness não linkou `FitsTableReader`/CCfits. Guards INPUT-5/7 no tree.

<a id="aud-mem-7"></a>
### AUD-MEM-7 - ClumpFind `round(star.x/y)` sem `isfinite`

V-MEM: `detectStars` com NaN/Inf, `detectBlended=true`: processo vivo, n=0 estrelas; path blended **não exercitado**. `StarDetector.cpp:156-158` sem `isfinite` (count 0). Dominó MEM-2 aberto até forçar `DetectedStar` não-finito.

### Efeitos PLAUSÍVEIS de IDs já CONFIRMADOS (não são IDs novos)

- Hang HTTP de [AUD-SEC-10](#aud-sec-10) contra SkyBoT/MPC reais: **não executado**.
- Token Codeberg de [AUD-SEC-7](#aud-sec-7) ainda válido: desconhecido.
- Conf 0644 pós-wizard ([AUD-SEC-6](#aud-sec-6)); progresso fantasma ([AUD-SEC-11](#aud-sec-11)); parse Horizons ([AUD-SEC-12](#aud-sec-12)).
- Ângulo de double-refraction ([AUD-CORR-7](#aud-corr-7)); delta LONPOLE ([AUD-CORR-10](#aud-corr-10)); magnitude sub-segundo ([AUD-CORR-13](#aud-corr-13)).
- WRITE em `fftw_execute` no mesmo under-alloc de [AUD-MEM-5](#aud-mem-5): **INFERÊNCIA** (lib do sistema sem ASan; primeiro hit foi READ em `crossPowerSpectrum`).
- Runtime ASan de [AUD-CCFITS-ASAN](#aud-ccfits-asan): flag `-fno-sanitize=undefined` presente; suíte sob sanitizer **não** re-rodada.

Nenhum candidato dos V-*.md foi REFUTADO no mérito (só a contagem 27 de PROV-10 corrigida para 23).

---

## 6. Veredito por lente

- **INPUT - COM PROBLEMA GRAVE (novo).** INPUT-1/2/3 AINDA-FECHADOS no path 2-D/SER. **AUD-INPUT-8** reabre a classe DoS no espectro 1-D. **AUD-INPUT-9** quebra paridade de teto no XISF. Gaps QImage/SER-produto/BINTABLE-nRows PLAUSÍVEIS. RAW/PDS4 continuam ausentes (honesto).

- **MEM - COM PROBLEMA GRAVE (novo).** MEM-2/3 AINDA-FECHADOS. **AUD-MEM-6** é heap-OOB alcançável em crop/guia. **AUD-MEM-5** é under-alloc FFT em portrait, overflow ASan CONFIRMADO no path `stackImages` FFT (primeiro trap: `crossPowerSpectrum`). **AUD-MEM-1** leak ainda no `.a` linkado. MEM-7 PLAUSÍVEL. cppcheck não substitui ASan.

- **CORR - SEM REGRESSÃO DO +90°; COM DÉBITO DE POLÍTICA.** CORR-1 AINDA-FECHADO (80/80 astropy + mutação vermelha). Nenhum CORR de julho REGREDIU. Novos são política/suíte/parse (CORR-7 **não** é CRÍTICO sem ângulo). Efeméride sem teste (CORR-12).

- **SEC - RAZOÁVEL, COM RESIDUAL.** Pins 6/6 MATCH. HTTPS enforcement no client OK. Sem leak no histórico git. `.runner` gitignored mode 600 mas **ainda no disco** (gitleaks `--no-git` 1 finding). Wizard contorna ApiKeyStore. 4º client HTTP (Koo) sem timeout. Sem scanner, sem SBOM. Hang real **não executado**.

- **PROV - NOTICE DE JULHO SEGUE; UX-STRING NOVO.** PROV-1/2/3-6/8 AINDA-FECHADOS. PROV-10 (23 menus) é o achado novo de clean-room. Eigen órfão (PROV-9). scancode ausente.

- **CI - PORTÃO DE AUDITORIA MORTO NO GITHUB.** CI-3 REGREDIU = CI-5. 10 jobs do push de migração verdes **não** provam ASan/cppcheck/tidy/valgrind. Pop/Zorin mentem 22.04. Gate omite clang-tidy. CRLF e WSL2 fechados. Matrix qa-distro **não disparada** nesta eventual (leitura + `gh`).

- **DOC - CLAUDE/SPDX/Bennett OK; Help e SECURITY NÃO.** DOC-1/3/4 AINDA-FECHADOS no eixo que julho fechou. Help ainda vende RAW/PDS4 e CIRS com aberração. SECURITY.md mente o estado do dossiê.

- **TEST - LOADERS FITS OK; CRÍTICOS SEM REDE.** TEST-2/3 AINDA-FECHADOS (mutação cube vermelha). TEST-4: mutação do guard NAXIS deixa a suíte verde. TEST-5: falso-verde L-45 CONFIRMADO. SER/XISF/Spectrum1D sem caso. Freeze mar/2026 **não** julga HEAD.

---

## 7. Tabela de rastreamento de remediação

Status desta eventual. A fase de correção e o WSJF no `TODO.md` são posteriores (este livro **não** editou `TODO.md`).

| ID | Severidade | Status remediação | Nota |
|---|:---:|:---:|---|
| AUD-INPUT-1 | CRÍTICO | AINDA-FECHADO | repro 2026-09-01 fail-clean |
| AUD-INPUT-2 | CRÍTICO | AINDA-FECHADO | ceiling 20000 |
| AUD-CORR-1 | CRÍTICO | AINDA-FECHADO | astropy 80/80; mutação 90° |
| AUD-MEM-2 | CRÍTICO | AINDA-FECHADO | nullopt vivo |
| AUD-MEM-3 | CRÍTICO | AINDA-FECHADO | clicks NaN rejeitados |
| AUD-INPUT-8 | CRÍTICO | ❌ pendente | novo |
| AUD-MEM-6 | CRÍTICO | ❌ pendente | novo |
| AUD-MEM-5 | CRÍTICO | ❌ pendente | overflow ASan CONFIRMADO (READ `crossPowerSpectrum`); código aberto |
| AUD-MEM-1 | IMPORTANTE | AINDA-ABERTO | leak no `.a` pré-patch |
| AUD-CI-3 | IMPORTANTE | REGREDIU | mesmo fato CI-5 |
| AUD-CI-5 | IMPORTANTE | ❌ pendente | `runs-on: docker` |
| AUD-INPUT-9 | IMPORTANTE | ❌ pendente | |
| AUD-SEC-6 | IMPORTANTE | ❌ pendente | |
| AUD-SEC-7 | IMPORTANTE | ❌ pendente | residual SEC-1; não publicar token |
| AUD-SEC-8 | IMPORTANTE | ❌ pendente | |
| AUD-SEC-10 | IMPORTANTE | ❌ pendente | hang não executado |
| AUD-SEC-11 | IMPORTANTE | ❌ pendente | |
| AUD-SEC-12 | IMPORTANTE | ❌ pendente | |
| AUD-SEC-13 | IMPORTANTE | ❌ pendente | |
| AUD-PROV-10 | IMPORTANTE | ❌ pendente | 23 identidades |
| AUD-CORR-7 | IMPORTANTE | ❌ pendente | não CRÍTICO |
| AUD-CORR-8 | IMPORTANTE | ❌ pendente | |
| AUD-CORR-10 | IMPORTANTE | ❌ pendente | |
| AUD-CORR-11 | IMPORTANTE | ❌ pendente | suíte |
| AUD-CORR-12 | IMPORTANTE | ❌ pendente | |
| AUD-CORR-13 | IMPORTANTE | ❌ pendente | |
| AUD-CORR-14 | IMPORTANTE | ❌ pendente | |
| AUD-CI-6 | IMPORTANTE | ❌ pendente | |
| AUD-CI-7 | IMPORTANTE | ❌ pendente | |
| AUD-DOC-5 | IMPORTANTE | ❌ pendente | |
| AUD-DOC-6 | IMPORTANTE | ❌ pendente | |
| AUD-DOC-8 | IMPORTANTE | ❌ pendente | |
| AUD-TEST-4 | IMPORTANTE | ❌ pendente | |
| AUD-TEST-5 | IMPORTANTE | ❌ pendente | |
| AUD-TEST-6 | IMPORTANTE | ❌ pendente | |
| AUD-SEC-1 | IMPORTANTE | AINDA-ABERTO | ver SEC-7/8 |
| AUD-SEC-2 | IMPORTANTE | AINDA-FECHADO + residual CCfits | 6 pins MATCH; URL 404 |
| AUD-INPUT-3 | IMPORTANTE | AINDA-FECHADO | |
| AUD-CORR-2 | IMPORTANTE | AINDA-FECHADO | residual CORR-11 |
| AUD-CORR-3 | IMPORTANTE | AINDA-FECHADO | residual CORR-7 |
| AUD-CORR-4 | IMPORTANTE | AINDA-FECHADO | |
| AUD-CORR-5 | IMPORTANTE | AINDA-FECHADO | residual CORR-8 |
| AUD-PROV-1 | IMPORTANTE | AINDA-FECHADO | |
| AUD-PROV-2 | IMPORTANTE | AINDA-FECHADO | residual PROV-13 |
| AUD-PROV-3-6 | IMPORTANTE | AINDA-FECHADO | |
| AUD-DOC-1 | IMPORTANTE | AINDA-FECHADO | |
| AUD-DOC-3 | IMPORTANTE | AINDA-FECHADO | |
| AUD-DOC-4 | IMPORTANTE | AINDA-FECHADO | gêmeos DOC-6/7 |
| AUD-TEST-2 | IMPORTANTE | AINDA-FECHADO | |
| AUD-TEST-3 | IMPORTANTE | AINDA-FECHADO | |
| AUD-CI-4 | IMPORTANTE | AINDA-FECHADO | |
| AUD-SEC-3 | IMPORTANTE | AINDA-FECHADO | gêmeo SEC-10 |
| AUD-SEC-4 | IMPORTANTE | AINDA-FECHADO | gêmeo SEC-13 |
| AUD-MEM-4 | IMPORTANTE | AINDA-ABERTO | = INPUT-8 |
| AUD-SEC-9 | COSMÉTICO | ❌ pendente | |
| AUD-PROV-9 | COSMÉTICO | ❌ pendente | primário Eigen; gêmeo DOC-9 |
| AUD-DOC-9 | COSMÉTICO | ❌ pendente | ponte L-17 → PROV-9 |
| AUD-PROV-11 | COSMÉTICO | ❌ pendente | |
| AUD-PROV-12 | COSMÉTICO | ❌ pendente | |
| AUD-PROV-13 | COSMÉTICO | ❌ pendente | |
| AUD-CORR-9 | COSMÉTICO | ❌ pendente | |
| AUD-CI-8 | COSMÉTICO | ❌ pendente | |
| AUD-CI-9 | COSMÉTICO | ❌ pendente | |
| AUD-DOC-7 | COSMÉTICO | ❌ pendente | |
| AUD-TEST-7 | COSMÉTICO | ❌ pendente | |
| AUD-SEC-5 | COSMÉTICO | AINDA-FECHADO | bypass = SEC-6 |
| AUD-CI-2 | COSMÉTICO | AINDA-FECHADO | |
| AUD-CORR-6 | COSMÉTICO | AINDA-FECHADO | gêmeo CORR-9 |
| AUD-INPUT-6 | COSMÉTICO | NÃO RE-TESTADO | |
| AUD-CCFITS-ASAN | COSMÉTICO | AINDA-FECHADO (artefato) | runtime ASan PLAUSÍVEL |
| AUD-PROV-8 | - | AINDA-FECHADO | risco aceito |
| AUD-INPUT-4 | IMPORTANTE | NÃO RE-TESTADO | |
| AUD-INPUT-5 | IMPORTANTE | NÃO RE-TESTADO | |
| AUD-INPUT-7 | - | NÃO RE-TESTADO | |
| AUD-INPUT-gaps | - | AINDA-ABERTO | |
| AUD-MEM-gaps | - | AINDA-ABERTO | MEM-6 saiu daqui como ID próprio |
| AUD-INPUT-10 | IMPORTANTE | PLAUSÍVEL | sem remediação até repro |
| AUD-INPUT-11 | IMPORTANTE | PLAUSÍVEL | |
| AUD-INPUT-12 | IMPORTANTE | PLAUSÍVEL | |
| AUD-MEM-7 | IMPORTANTE | PLAUSÍVEL | |

Nenhum CRÍTICO novo ficou sem plano de remediação sugerido (§4.1). A implementação é fase posterior, sob o líder.

---

## 8. Limitações (honesto)

1. **`fftw-devel` instalado** em 2026-09-01 (`fftw-devel-3.3.10-17.fc44.x86_64`). Runtime ASan de [AUD-MEM-5](#aud-mem-5) **executado** (harness; não o cmake ASan do produto). Suíte ASan completa do produto = **não executado**. `[functional]` Catch2 completo = **não executado**. clang-tidy de ImageStacker na onda H2 falhou por `fftw3.h` ausente **naquela** passagem.
2. **Freeze `astrofind_tests`** em `build/` (md5 `3978071e96747031fdae7170372ab774`, mtime **2026-03-22**) é STALE vs HEAD. Não julga a suíte atual. Catch2/spdlog shared às vezes ausentes sem `LD_LIBRARY_PATH`.
3. **Matrix 9 qa-distro não disparada** nesta eventual (cada um é container = trabalho pesado). CI usou YAML + `gh run` do SHA `0940509`.
4. **Hang HTTP** contra SkyBoT/MPC/Horizons/astrometry.net **não executado**. Gap de código CONFIRMADO; hang observável PLAUSÍVEL.
5. **scancode** ausente; **syft** não gerou SBOM nesta passagem; **reuse lint** não rodado (SPDX contado item-a-item: 127/127/127/0).
6. Mutação só em `/var/tmp` (L-27). Repo IDrive intocado. Sem `isolation: worktree` (L-55). UI offscreen / `XDG_RUNTIME_DIR` isolado (L-50). Sem screenshot na sessão viva.
7. INPUT-4/5/6/7 e CCFITS sob ASan: sem repro 2026-09-01. Não declarar AINDA-FECHADO nem REGREDIU.

---

## 9. Parecer de prontidão para auditor externo

Os quatro CRÍTICOS de produto de 2026-07-10 (NAXIS, alocação 2-D, WCS +90°, NaN centroid) **continuam fechados** no HEAD `0940509`, com repro hostil, oráculo astropy 8.0.1 e mutação que prova a rede de CORR-1 e MEM-2. Isso é FATO desta eventual, não confiança no `TODO.md`.

O produto **não** está pronto para um auditor que pergunte "a Onda 1 segurou tudo?": três CRÍTICOS novos (espectro 1-D, OOB de fundo, FFT portrait), leak SEP ainda no binário que o processo carrega, portão `audit.yml` morto no GitHub, Help/SECURITY desalinhados, suíte que não pega os guards que julho vendeu como fechados.

Divergências registradas (finder vs verifier; prevalece o verifier + orquestrador): CORR-7 não é CRÍTICO (sem ângulo); PROV-10 são 23 identidades, não 27; MEM-5 overflow ASan **foi** promovido a runtime CONFIRMADO em 2026-09-01 (primeiro trap = READ `crossPowerSpectrum`, não `fftw_execute`); CI-3 e CI-5 são o mesmo fato; DOC-9 e PROV-9 são o mesmo Eigen (L-17).

**Recomendação:** não tratar v0.9.0 como fechado frente a este livro. Remediação e `TODO.md` são a fase seguinte, sob decisão do líder.
