# AUDIT_FIND — Dossiê de Auditoria AstroFind

> Livro de auditoria interna consolidado, pronto para entrega a auditor externo.
> Cada achado tem ID estável (`AUD-<LENTE>-<n>`), severidade adjudicada, âncora por símbolo/arquivo, repro executável e remediação. Referência por símbolo/função — nunca por linha viva (linha aparece só no repro).

---

## 1. Cabeçalho

| Campo | Valor |
|---|---|
| **Projeto** | AstroFind |
| **Versão** | v0.9.0 |
| **Licença** | AGPL-3.0 |
| **Stack** | Qt6 / C++23, CMake |
| **Porte de código** | ~26k LOC (`src/core` + `src/ui`) |
| **Dependências** | cfitsio, Eigen3, fftw3 + bundled: SEP, CCfits, quazip, spdlog, nlohmann_json, Catch2 |
| **Data da auditoria** | 2026-07-10 |
| **Tipo** | Auditoria read-only (nenhuma alteração em `src/` ou `tests/`) |

### Escopo

Auditoria de robustez a input hostil de arquivo, corretude numérica/astronômica (WCS, refração, fotometria, cadeia de frames ICRS→CIRS→topocêntrica), memory safety, segurança/segredos/supply-chain, proveniência/licenças, e higiene de CI/documentação/testes. O maior stake do produto é científico: coordenadas medidas são submetidas ao **Minor Planet Center (MPC)**.

### Método (três papéis independentes)

1. **Finder** (Sonnet 5) — uma passada por lente (INPUT, CORR, MEM, SEC, PROV, CI/DOC/TEST), gerando candidatos.
2. **Verificador adversarial** — agente DIFERENTE do finder, que **EXECUTA** o código e usa **astropy 8.0.1 como oráculo externo** para corretude WCS, imagens sintéticas reais para memory safety, e extratores reais para input hostil. Princípio operante: **"relatório de agente não é prova"**.
3. **Orquestrador** — re-verifica independentemente: build ASan/UBSan próprio, spot-check das claims (símbolo/arquivo), e execução dos repros.

Build de sanitizers usado: fresh em `scratchpad/build-asan2` (o `build/` do repo é órfão — RUNPATH aponta p/ `/home/petrus/.../projetos_dev/astrometrica`, resíduo de mudança de pasta). Suíte: 116 casos core (106 pass, 10 SKIP) + 23 UI, 4426 asserts verde.

### Disciplina read-only

Nenhum arquivo de produção (`src/`, `tests/`) foi editado. Nenhum `git add/commit/push`. Os únicos artefatos escritos por esta fase são este livro (`AUDIT_FIND.md`) e a tabela de remediação (`TODO.md`).

### Calibração de porte

AstroFind é aplicação técnica de nicho (astrometria), single-binary desktop, sem multi-tenancy nem PII de terceiros no núcleo. A severidade seguiu [[AUDITORIAS]] sem inflar nem minimizar: CRÍTICO exige crash/corrupção/UB/vazamento explorável; IMPORTANTE é bug real de corretude/robustez sem exploit direto; COSMÉTICO é drift/estilo/defensivo inalcançável. O stake regulatório-científico (output ao MPC) puxa a corretude WCS e a robustez de input a bloqueadores de release.

---

## 2. Sumário executivo

**Contagem:** **4 CRÍTICOS**, **~18 IMPORTANTES**, **4 COSMÉTICOS**, **5 PLAUSÍVEIS**.

Os 4 CRÍTICOS, em uma frase cada:

- **[AUD-INPUT-1](#aud-input-1)** — um `.fits` de ~3 KB com `NAXIS≥4` provoca stack-buffer-overflow dentro do cfitsio (arrays de 3 elementos sem teto superior).
- **[AUD-INPUT-2](#aud-input-2)** — um `.fits` de 2880 bytes com dimensões mentirosas (`NAXIS1=NAXIS2=100000`) dispara alocação de ~40 GB e exceção não capturada → `std::terminate()`.
- **[AUD-CORR-1](#aud-corr-1)** — projeções WCS não-zenitais (CAR/MER/GLS/AIT) reportam coordenada celeste ~90° errada com eixos RA/Dec trocados, e esse output vai ao MPC.
- **[AUD-MEM-2](#aud-mem-2)** (com **[AUD-MEM-3](#aud-mem-3)**) — um pixel `NaN`/`Inf` numa estrela mede-se via `static_cast<int>(round(NaN))` → INT_MIN → signed-overflow abort, alcançável pelo pipeline real de medição.

**Veredito geral (honesto):** a base é madura e disciplinada (RAII limpo no código próprio, suíte de 4426 asserts verde, fórmulas astronômicas numericamente corretas onde medidas), **mas contém duas classes de bug sério que a suíte de testes não pega**:

1. **Input hostil de arquivo** — qualquer `.fits`/`.ser`/BINTABLE de terceiro pode causar crash ou UB ao abrir (AUD-INPUT-1/2/3/5, AUD-MEM-2/4). A superfície de entrada não é validada antes de chamar cfitsio / redimensionar buffers.
2. **Corretude WCS não-TAN** — 7 das 8 projeções nunca foram testadas por valor absoluto (só round-trip, que é invariante necessário-mas-não-suficiente); 4 delas produzem coordenada errada que segue ao MPC (AUD-CORR-1/2).

Ambas as classes têm consequência de alto stake: **output cientificamente errado indo ao MPC**, e **crash ao abrir arquivo de terceiro**. Não há CRÍTICO de segurança com exploit direto, mas há riscos estruturais (token vivo `.runner`, supply-chain sem pin de SHA) e um débito sistêmico de proveniência/atribuição de licenças (WCSLIB, SEP) que requer decisão jurídica. **Recomendação:** não taggear release até fechar a Onda 1 (os 4 CRÍTICOS + `.runner`).

---

## 3. Índice mestre

| ID | Título | Severidade | Status | Lente | Âncora (símbolo/arquivo) |
|---|---|:---:|:---:|:---:|---|
| [AUD-INPUT-1](#aud-input-1) | Stack-buffer-overflow via NAXIS>3 | 🔴 CRÍTICO | CONFIRMADO | INPUT | `loadFits*` / `FitsImage.cpp` |
| [AUD-INPUT-2](#aud-input-2) | Alocação sem teto por header mentiroso | 🔴 CRÍTICO | CONFIRMADO | INPUT | `loadFits*` / `FitsImage.cpp` |
| [AUD-CORR-1](#aud-corr-1) | Projeções não-zenitais 90° erradas → MPC | 🔴 CRÍTICO | CONFIRMADO | CORR | `nativePoleAngle`/`nativeToCelestial` |
| [AUD-MEM-2](#aud-mem-2) | NaN/Inf → cast→INT_MIN→overflow abort | 🔴 CRÍTICO | CONFIRMADO | MEM | `findCentroid`/`aperturePhotometry` |
| [AUD-MEM-3](#aud-mem-3) | Guard de sanidade contornável por NaN | 🔴 CRÍTICO | CONFIRMADO | MEM | `findCentroidElliptical`/`findCentroidPsf` |
| [AUD-SEC-1](#aud-sec-1) | Token vivo `.runner` mundo-legível | 🟠 IMPORTANTE | CONFIRMADO | SEC | `.runner` (raiz) |
| [AUD-SEC-2](#aud-sec-2) | 6 deps FetchContent sem pin de SHA | 🟠 IMPORTANTE | CONFIRMADO | SEC | `cmake/dependencies.cmake` |
| [AUD-SEC-3](#aud-sec-3) | Clients HTTP sem timeout/cancel | 🟠 IMPORTANTE | CONFIRMADO | SEC | `AstrometryClient`/`CatalogClient`/`HorizonsClient` |
| [AUD-PROV-1](#aud-prov-1) | Proveniência WCSLIB não atribuída (LGPL) | 🟠 IMPORTANTE | CONFIRMADO | PROV | comentário `FitsImage.cpp` |
| [AUD-PROV-2](#aud-prov-2) | SEP (LGPL) sem NOTICE | 🟠 IMPORTANTE | CONFIRMADO | PROV | `sep` bundled |
| [AUD-PROV-3-6](#aud-prov-3-6) | Ausência sistêmica de NOTICE/THIRD_PARTY | 🟠 IMPORTANTE | CONFIRMADO | PROV | QuaZip/spdlog/json/FFTW3/cfitsio |
| [AUD-MEM-1](#aud-mem-1) | Leak 44B/chamada em SEP (upstream) | 🟠 IMPORTANTE | CONFIRMADO | MEM | `sep convert_to_catalog` |
| [AUD-MEM-4](#aud-mem-4) | RGB multi-ext sem re-validação pós-cast | 🟠 IMPORTANTE | PLAUSÍVEL | MEM | `FitsImage.cpp` RGB path / `Spectrum1D.cpp` |
| [AUD-INPUT-3](#aud-input-3) | SER loader sem checagem de sinal | 🟠 IMPORTANTE | CONFIRMADO | INPUT | `ImageLoader::loadSer` |
| [AUD-INPUT-4](#aud-input-4) | Symlink não filtrado na extração | 🟠 IMPORTANTE | CONFIRMADO | INPUT | `expandArchive`/`expandZip` |
| [AUD-INPUT-5](#aud-input-5) | BINTABLE NAXIS2 mentiroso → linhas NaN | 🟠 IMPORTANTE | CONFIRMADO | INPUT | `FitsTableReader::readColumn` |
| [AUD-CORR-2](#aud-corr-2) | 7/8 projeções sem teste de valor absoluto | 🟠 IMPORTANTE | CONFIRMADO | CORR | `tests/` WCS |
| [AUD-CORR-3](#aud-corr-3) | `applyRefractionCorrection` sem testes | 🟠 IMPORTANTE | CONFIRMADO | CORR | `Astronomy.cpp` |
| [AUD-CORR-4](#aud-corr-4) | Aberração/prec/nut sem teste + dead code | 🟠 IMPORTANTE | CONFIRMADO | CORR | `Astronomy.cpp` |
| [AUD-CORR-5](#aud-corr-5) | Testes de fotometria circulares | 🟠 IMPORTANTE | CONFIRMADO | CORR | `test_photometry.cpp` |
| [AUD-CI-3](#aud-ci-3) | Auditoria numérica nunca roda em CI | 🟠 IMPORTANTE | CONFIRMADO | CI | workflows |
| [AUD-CI-4](#aud-ci-4) | `qa-wsl2.yml` órfão (alvo removido) | 🟠 IMPORTANTE | CONFIRMADO | CI | `.github/workflows/qa-wsl2.yml` |
| [AUD-DOC-1](#aud-doc-1) | Drift doc-vs-código na refração | 🟠 IMPORTANTE | CONFIRMADO | DOC | `docs/technical-reference.md` §5 |
| [AUD-DOC-3](#aud-doc-3) | 0/127 arquivos com SPDX | 🟠 IMPORTANTE | CONFIRMADO | DOC | `src/` |
| [AUD-DOC-4](#aud-doc-4) | Feature-fantasma RAW/PDS4 (#21) | 🟠 IMPORTANTE | CONFIRMADO | DOC | `src/` (ausente) |
| [AUD-TEST-2](#aud-test-2) | Cobertura fantasma (10 SKIP) | 🟠 IMPORTANTE | CONFIRMADO | TEST | `test_fits_functional.cpp` |
| [AUD-SEC-5](#aud-sec-5) | ApiKeyStore fallback plain-text (avisado) | 🟢 COSMÉTICO | CONFIRMADO | SEC | `ApiKeyStore` |
| [AUD-CI-2](#aud-ci-2) | 12 arquivos CI = ruído CRLF | 🟢 COSMÉTICO | CONFIRMADO | CI | workflows |
| [AUD-CORR-6](#aud-corr-6) | Constante FWHM imprecisa (0.0077%) | 🟢 COSMÉTICO | CONFIRMADO | CORR | `Centroid.cpp` |
| [AUD-INPUT-6](#aud-input-6) | UB de alinhamento em log de struct packed | 🟢 COSMÉTICO | CONFIRMADO | INPUT | `ImageLoader.cpp` |
| [AUD-SEC-4](#aud-sec-4) | Downgrade TLS por config sem enforcement | 🟠 IMPORTANTE | PLAUSÍVEL | SEC | QSettings URLs |
| [AUD-INPUT-7](#aud-input-7) | OOB read em colunas opcionais BINTABLE | — | PLAUSÍVEL | INPUT | `FitsTableReader` |
| [AUD-PROV-8](#aud-prov-8) | GDL/NEMO/Siril não auditados linha-a-linha | — | PLAUSÍVEL | PROV | README credits |
| [AUD-INPUT-gaps](#aud-input-gaps) | Gaps de input (RAW/PDS4/QImage/XISF) | — | PLAUSÍVEL | INPUT | vários |
| [AUD-MEM-gaps](#aud-mem-gaps) | Gaps de memória não re-rodados | — | PLAUSÍVEL | MEM | `Calibration.cpp` etc |

---

## 4. Achados CONFIRMADOS por severidade

### 4.1 🔴 CRÍTICOS

<a id="aud-input-1"></a>
### AUD-INPUT-1 — Stack-buffer-overflow em cfitsio via NAXIS>3 não validado

- **Severidade:** 🔴 CRÍTICO — CONFIRMADO (finder + orquestrador)
- **Âncora:** `core::loadFits` / `loadFitsHdu` / `loadFitsCube` em `src/core/FitsImage.cpp` (arrays `long naxes[3]` / `long fpixel[3]`; guard só `if (naxis < 2)`, sem teto superior; chamada `fits_read_pix`).
- **Failure concreto:** um FITS de ~2.9–5.7 KB só-header com `NAXIS=4` (ou 5/6/10/50) passa a validação; cfitsio (`ffgpxv`) opera no NAXIS real e escreve além dos 3 elementos reservados na pilha. ASan reporta `stack-buffer-overflow ... in ffgpxv.part.0`, frame `FitsImage.cpp:787`. Limiar: NAXIS=3 OK, ≥4 overflow 100%. Em Release sem ASan = UB explorável / crash confiável.
- **Repro:** `QT_QPA_PLATFORM=offscreen $SCRATCH/harness/fits_harness $SCRATCH/fits_hostile/naxisN_4.fits` (+ variantes `--hdu`, `--cube`). Reproduzido pelo orquestrador independentemente.
- **Remediação:** rejeitar `naxis > 3` antes de qualquer chamada cfitsio nos 3 call sites.
- **Confirmado por:** finder + orquestrador (execução independente).

<a id="aud-input-2"></a>
### AUD-INPUT-2 — Alocação não controlada por header mentiroso (DoS / crash por exceção não capturada)

- **Severidade:** 🔴 CRÍTICO — CONFIRMADO
- **Âncora:** `core::loadFits` / `loadFitsHdu` / `loadFitsCube` (`FitsImage.cpp`) — `img.data.resize(planeSize)` / `frames.reserve(D)`; width/height/D só checados `>0`, sem teto nem cross-check com tamanho do arquivo.
- **Failure concreto:** um FITS de 2880 bytes com `NAXIS1=NAXIS2=100000` tenta alocar ~40 GB antes de ler qualquer pixel. ASan: `hard rss limit exhausted` (>1.2 GB RSS real). Sem ASan → `std::bad_alloc`/`std::length_error` que NINGUÉM captura (`grep catch(` em todos os call sites de UI + `main` = zero) → exceção escapa de slot Qt → `std::terminate()`. Alcançável por qualquer ponto de entrada (abrir, drag&drop, dark/flat, blink, comparar sessão).
- **Repro:** `fits_harness $SCRATCH/fits_hostile/01_huge_naxis_100000.fits`.
- **Remediação:** validar `width*height*canais*sizeof(float)` (e `D`) contra teto configurável e/ou `QFileInfo::size()`; try/catch nos resize; nunca deixar exceção escapar de slot Qt.
- **Confirmado por:** finder + orquestrador.

<a id="aud-corr-1"></a>
### AUD-CORR-1 — Projeções cilíndricas/pseudo reportam coordenada ~90° errada (output vai ao MPC)

- **Severidade:** 🔴 CRÍTICO — CONFIRMADO (finder + verificador adversarial via astropy 8.0.1 + orquestrador analítico)
- **Âncora:** `nativePoleAngle()` (`src/core/FitsImage.cpp`) retorna 90°/270° no `default:` (não-zenital); `nativeToCelestial()` usa CRVAL como polo nativo — válido só para zenitais (θ₀=90°: TAN/SIN/ARC/STG). Para cilíndricas (θ₀=0°: CAR, MER, GLS/SFL, AIT) o ponto de referência está no equador nativo; φ_p correto = 0°/180°, não 90°/270° (Calabretta & Greisen 2002, WCS Paper II eq. 9).
- **Failure concreto (medido):** com CRVAL=(180,0), CRPIX=(512,512), CD diagonal — pixel de referência → código dá **(270.000000, 0.0)** vs oráculo astropy **(180.000000, 0.0)**. Erro exato **+90°** em RA. +100px em X move Dec (deveria mover RA) → **eixos RA/Dec trocados**. TAN (controle) bate com astropy até 6 casas. O round-trip fecha (invertível-mas-errado) — por isso passou despercebido pela suíte.
- **Stake:** máximo — output vai ao **Minor Planet Center**.
- **Repro:** `python3 $SCRATCH/oracle_wcs.py` (astropy) vs `$SCRATCH/delta_wcs` (linkado a `libastrofind_core.a`). 4 confirmações independentes.
- **Remediação:** derivar (α_p, δ_p, φ_p) do polo celeste conforme WCS Paper II eqs 8–10 para θ₀≠90°; ou delegar a rotação ao WCSLIB (que o próprio comentário do código cita). Adicionar teste de VALOR ABSOLUTO (ref pixel → CRVAL) para as 8 projeções (ver [AUD-CORR-2](#aud-corr-2)).
- **Confirmado por:** finder + verificador adversarial (astropy) + orquestrador (análise).

<a id="aud-mem-2"></a>
### AUD-MEM-2 — Pixel NaN/Inf → `static_cast<int>` → INT_MIN → signed overflow abort

- **Severidade:** 🔴 CRÍTICO — CONFIRMADO (finder + verificador adversarial com imagem real)
- **Âncora:** `core::findCentroid` (`Centroid.cpp:63`, `static_cast<int>(std::round(cx))`) e `core::aperturePhotometry` (`Photometry.cpp:29`); cadeia real `MainWindow::onPixelClicked` → `runMeasurePipeline` (`MainWindow_measurement.cpp:60-131`) SEM guard `isfinite`.
- **Failure concreto:** uma "estrela" com um único pixel `+Inf` (valor FITS float legítimo — pixel saturado/mascarado; a feature #13 admite NaN em FITS reais) → soma ponderada `Inf/Inf = NaN` → `static_cast<int>(round(NaN))` aborta em `Centroid.cpp:63` ANTES da fotometria. Sob build oficial de auditoria (`-fno-sanitize-recover`) = abort do processo. Nenhum teste cobre NaN/Inf.
- **Repro:** `$SCRATCH/delta_nan_centroid` (imagem sintética com 1 pixel Inf) e `$SCRATCH/delta_direct_nan` (chamada direta).
- **Remediação:** guard `if (!std::isfinite(cx)||!std::isfinite(cy)) return nullopt/unexpected;` na entrada de `aperturePhotometry`/`computeDifferentialZeroPoint`/`findCentroid`/`findCentroidPsf`/`findCentroidElliptical`.
- **Confirmado por:** finder + verificador adversarial (imagem real).

<a id="aud-mem-3"></a>
### AUD-MEM-3 — Guard de sanidade de centroide contornável por NaN (parte do AUD-MEM-2)

- **Severidade:** 🔴 CRÍTICO — CONFIRMADO
- **Âncora:** `findCentroidElliptical` (`Centroid.cpp:361`, `if (p[0]<x0||p[0]>x1||...)`) e `findCentroidPsf` (`Centroid.cpp:179`).
- **Failure concreto:** o guard de sanidade é contornável por NaN — comparação IEEE-754 com NaN é sempre `false`, deixando NaN/Inf passar. Vetor real: o seed de moments já nasce NaN (ver [AUD-MEM-2](#aud-mem-2)). Gap sistêmico: `Astronomy`/`Calibration`/`FitsImage` USAM `std::isfinite`; `Centroid`/`Photometry` (as duas unidades com ajuste iterativo LM) NÃO — inconsistência de padrão.
- **Remediação:** trocar por `if (!std::isfinite(p[0])||!std::isfinite(p[1])||p[0]<x0||...) return seed;`.
- **Confirmado por:** finder + verificador adversarial.

### 4.2 🟠 IMPORTANTES

<a id="aud-sec-1"></a>
### AUD-SEC-1 — Token vivo do forgejo-runner em `.runner` mundo-legível

- **Severidade adjudicada:** 🟠 IMPORTANTE (com remediação URGENTE) — CONFIRMADO
- **⚠ Divergência de severidade:** o **CI-finder classificou CRÍTICO** (exposição do segredo); o **orquestrador adjudicou IMPORTANTE** porque nunca vazou ao git (risco estrutural, sem gate técnico, mas sem exposição consumada).
- **Âncora:** `.runner` (raiz do repo).
- **Failure concreto:** token vivo do forgejo-runner em `.runner`, untracked E fora do `.gitignore`, perm 644 (mundo-legível). Nunca commitado (gitleaks `--all` limpo em 59 commits — só não vazou por disciplina, sem gate técnico). Um `git add -A` de rotina o exporia. `address=codeberg.org`, token de 40 chars. Nota: também está num diretório sincronizado (IDrive) — já fora do trust boundary do git.
- **Remediação:** adicionar `.runner`/`.runner*` ao `.gitignore`; `chmod 600`; rotacionar o token no Codeberg por precaução; pre-commit gitleaks + secret-scan em CI.
- **Confirmado por:** CI-finder + orquestrador.

<a id="aud-sec-2"></a>
### AUD-SEC-2 — 6 deps FetchContent com GIT_TAG móvel sem pin de commit SHA

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `cmake/dependencies.cmake`.
- **Failure concreto:** 6 deps FetchContent com `GIT_TAG` móvel SEM pin de commit SHA nem `URL_HASH` (spdlog v1.14.1, nlohmann_json v3.11.3, cfitsio 4.6.3, quazip v1.4, sep v1.2.1, Catch2 v3.6.0), todas `GIT_SHALLOW`. Tag re-pointing / mantenedor comprometido = supply-chain (CWE-1357, SLSA).
- **Remediação:** pinar por commit SHA completo ou `URL`+`URL_HASH` SHA256; gerar SBOM (syft); documentar política de deps.
- **Confirmado por:** finder (leitura de `cmake/dependencies.cmake`).

<a id="aud-sec-3"></a>
### AUD-SEC-3 — Clients HTTP sem timeout / cancel (DoS de disponibilidade)

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `AstrometryClient` / `CatalogClient` / `HorizonsClient`.
- **Failure concreto:** só `MpcSubmit` tem `setTransferTimeout`; os outros três não. `CatalogClient`/`HorizonsClient` também não têm `cancel()`. Servidor que aceita conexão e nunca responde (proxy/captive/MITM) pendura `busy_=true` para sempre → funcionalidade morta até reiniciar.
- **Remediação:** `setTransferTimeout` nos 3; watchdog/cancel espelhando `MpcSubmit`.
- **Confirmado por:** finder + orquestrador (grep).

<a id="aud-prov-1"></a>
### AUD-PROV-1 — Proveniência WCSLIB não atribuída (dever LGPL)

- **Severidade adjudicada:** 🟠 IMPORTANTE (compliance) — CONFIRMADO (comentário verbatim pelo orquestrador)
- **⚠ Divergência de severidade:** o **PROV-finder classificou CRÍTICO-compliance**; o **orquestrador adjudicou IMPORTANTE** pela taxonomia (sem crash/UB). **Requer decisão jurídica — encaminhar ao CLO/jurídico.**
- **Âncora:** comentário em `FitsImage.cpp` (função do pipeline WCS): *"Projection formulas adapted from WCSLIB 8.2 (prj.c, Calabretta, CSIRO ATNF)."*
- **Failure concreto:** WCSLIB é LGPL-3.0-or-later. Sem NOTICE, sem copyright de Calabretta, e o README credita NEMO para a mesma funcionalidade (contradição interna). Não é incompatibilidade bloqueante (AGPL fornece fonte), mas é dever de atribuição LGPL não cumprido + proveniência não apurada. Cross-ref [AUD-CORR-1](#aud-corr-1) — esse mesmo código WCS está bugado.
- **Remediação:** apurar se derivou de paper (público) ou de `prj.c` (código LGPL); se código, criar NOTICE + copyright + texto LGPL; alinhar README.
- **Confirmado por:** PROV-finder + orquestrador (verbatim).

<a id="aud-prov-2"></a>
### AUD-PROV-2 — SEP (LGPL-3.0) linkado sem NOTICE

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `sep` (kbarbary/sep v1.2.1) bundled.
- **Failure concreto:** núcleo LGPL-3.0 (Emmanuel Bertin, herdado do SExtractor), linkado estático via glob `*.c`, sem NOTICE nem texto LGPL. Compat com AGPL OK (LGPLv3 §4 satisfeito por fonte disponível), mas dever de atribuição pendente.
- **Remediação:** NOTICE listando SEP (LGPL-3.0 + partes MIT), copyright Bertin/SEP developers, cópia da LGPL-3.0.
- **Confirmado por:** finder + orquestrador (header LGPL).

<a id="aud-prov-3-6"></a>
### AUD-PROV-3/4/5/6 — Ausência sistêmica de NOTICE/THIRD_PARTY

- **Severidade:** 🟠 IMPORTANTE (agregado "sem NOTICE") — CONFIRMADO
- **Âncora:** QuaZip / spdlog / nlohmann_json / FFTW3 / cfitsio / CCfits; README.
- **Failure concreto:** QuaZip (LGPL-2.1 + exceção de linkagem estática — compat explícita, mas texto ausente); spdlog/nlohmann_json (MIT exige reproduzir aviso — não cumprido, só link no README); FFTW3 (GPL-2.0+, compat, sem licença no README); cfitsio/CCfits (permissiva NASA, boa prática incluir). O README não tem coluna de licença de nenhuma dep.
- **Remediação:** gerar NOTICE consolidado (todas as deps distribuídas no binário) + coluna de licença no README.
- **Confirmado por:** finder + orquestrador.

<a id="aud-mem-1"></a>
### AUD-MEM-1 — Leak de 44 bytes por chamada de detectStars (bug upstream SEP)

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `sep convert_to_catalog` (`build/_deps/sep-src/src/extract.c:1094-1103`).
- **Failure concreto:** leak de 44 bytes / 6 alocações em toda chamada bem-sucedida de `detectStars`. A causa NÃO é o AstroFind (`StarDetector.cpp:134` chama `sep_catalog_free` corretamente) — é bug UPSTREAM em SEP: duplo `QMALLOC` de `cflux`/`flux` sobrescreve a 1ª alocação. Presente também no `main` atual do upstream. Cresce com nº fontes × frames em sessão longa/blink.
- **Repro:** `astrofind_tests` sob LeakSanitizer → `44 byte(s) leaked ... convert_to_catalog`.
- **Remediação:** patch local no `FetchContent_Populate` (string REPLACE removendo as linhas duplicadas) ou PR upstream; repactar `libsep_lib.a`.
- **Confirmado por:** finder + orquestrador (LSan).

<a id="aud-mem-4"></a>
### AUD-MEM-4 — Caminho RGB multi-extensão sem re-validação após cast

- **Severidade:** 🟠 IMPORTANTE — PLAUSÍVEL (classe confirmada por [AUD-INPUT-2](#aud-input-2))
- **Âncora:** `FitsImage.cpp:673-679` (RGB path) e `Spectrum1D.cpp`.
- **Failure concreto:** o caminho multi-extensão RGB valida `axes>0` como `long` mas NÃO re-valida após `static_cast<int>` (diferente do path single-image que re-checa). NAXIS perto de 2³¹ → width/height negativo → planeSize negativo → `resize(size_t enorme)` → `length_error` não capturado → terminate. Idem `Spectrum1D.cpp` (NAXIS1 sem teto).
- **Remediação:** validar teto antes do cast nos 5 locais; padronizar o guard pós-cast do single-image path.
- **Confirmado por:** finder (classe da mesma família de AUD-INPUT-2; caso RGB específico não fabricado).

<a id="aud-input-3"></a>
### AUD-INPUT-3 — SER loader sem checagem de sinal após cast

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `ImageLoader::loadSer` (`ImageLoader.cpp`).
- **Failure concreto:** checa `imageWidth`/`imageHeight` só `==0`, sem checar sinal após cast (diferente do path FITS). Um `.ser` com `width=0xFFFFFFFF`/`0x80000010` → `cannot create std::vector larger than max_size()` não capturado → crash-DoS ao abrir.
- **Repro:** `fits_harness $SCRATCH/fits_hostile/ser_width_int32_wrap.ser --imageloader`.
- **Remediação:** validar `>0 && <TETO` antes do cast, igual ao FITS.
- **Confirmado por:** finder + orquestrador.

<a id="aud-input-4"></a>
### AUD-INPUT-4 — Symlink não filtrado na extração de arquivo (leitura arbitrária / DoS)

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `expandArchive` (libarchive) e `expandZip` (unzip CLI).
- **Failure concreto:** os dois extratores achatam o NOME (path traversal `../` está OK — testado negativo), mas não filtram o TIPO da entrada. Uma entrada symlink com nome `.fits` apontando p/ `/etc/passwd` (ou `~/.ssh/id_rsa`, ou FIFO p/ travar) é recriada e devolvida a `loadImage()`, que segue o link.
- **Repro:** `tar`/`zip --symlinks` com `evil.fits -> /etc/passwd`; confirmado empiricamente nos dois extratores.
- **Remediação:** rejeitar entradas com `filetype != AE_IFREG` (libarchive); no unzip checar `file --mime-type`/`O_NOFOLLOW`, ou trocar por QuaZip (já linkado) com checagem de tipo.
- **Confirmado por:** finder + orquestrador (empírico).

<a id="aud-input-5"></a>
### AUD-INPUT-5 — BINTABLE com NAXIS2 mentiroso → linhas fantasma NaN silenciosas

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `FitsTableReader::readColumn` (`FitsTableReader.cpp`).
- **Failure concreto:** o `catch (CCfits::FitsException)` substitui a coluna inteira por um vetor de NaN do tamanho DECLARADO (controlado pelo atacante) e retorna SUCESSO. Header dizendo 1000 linhas (3 reais) → 1000 "estrelas" NaN entram em `importDaophotTable`/`readLocalCatalogTable`/`importIrafTable` sem `isfinite` check nem aviso. Perde inclusive as 3 linhas válidas.
- **Repro:** `table_harness_nosan $SCRATCH/fits_hostile/table_declared_1000_actual_3_lied.fits`.
- **Remediação:** propagar erro em vez de fabricar NaN; cross-check `tbl->rows()` vs dados reais; filtrar linhas NaN na importação.
- **Confirmado por:** finder + orquestrador.

<a id="aud-corr-2"></a>
### AUD-CORR-2 — 7/8 projeções WCS sem teste de valor absoluto

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `tests/` (WCS).
- **Failure concreto:** o teste de round-trip WCS cobre só TAN (`PlateSolution` nunca seta `.projection` → default TAN); 7/8 projeções sem teste (`grep projection`/`WcsProjection` em `tests/` = zero). Round-trip é invariante necessário-mas-não-suficiente: CAR fecha em 8.6e-11px enquanto o valor absoluto está 90° errado ([AUD-CORR-1](#aud-corr-1)). Faltou oráculo de valor absoluto.
- **Pré-req:** depende de [AUD-CORR-1](#aud-corr-1) corrigido para ficar verde.
- **Remediação:** teste `[wcs]` parametrizado nas 8 projeções: (a) `pixToSky(crpix)==crval` sub-arcseg, (b) 1–2 pixels vs WCSLIB/astropy.
- **Confirmado por:** finder + orquestrador.

<a id="aud-corr-3"></a>
### AUD-CORR-3 — `applyRefractionCorrection` sem testes (única correção de frame no output MPC)

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `Astronomy.cpp::applyRefractionCorrection` (aplicada via `MainWindow_measurement.cpp:79-88`).
- **Failure concreto:** é a única correção de frame realmente aplicada à coord do MPC e tem ZERO testes. Numericamente correta onde medida (1.746′ a 30°; corta <1°; guard NaN OK), MAS: inverter o sinal (linha 86) compila e passa 100% da suíte (mutation confirmado). O gate `isSpaceTelescope` mora só no caller, sem teste, burlável por outro caller.
- **Remediação:** testes `[refraction]` com geometria conhecida + gate no nível do pipeline; considerar mover `isSpaceTelescope` p/ dentro da função.
- **Confirmado por:** finder + verificador adversarial (mutation).

<a id="aud-corr-4"></a>
### AUD-CORR-4 — Aberração/precessão/nutação sem teste + dead code

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `Astronomy.cpp` (aberração, precessão, nutação, Sol, eclíptica, galáctica).
- **Failure concreto:** fórmulas corretas (medidas vs Meeus/oráculos: aberração max=20.4954″=κ; precessão 46.13″/ano; nutação 17.2″), mas ZERO testes. Pior: `applyPrecessionJ2000ToDate` e `applyNutation` NÃO são chamadas em `src/` (dead code); aberração só p/ logging. A cadeia ICRS→CIRS→topocêntrica (item 12) está implementada mas NÃO conectada ao pipeline. Regressão de sinal/unidade passaria verde.
- **Remediação:** testes com oráculo externo (Meeus/SOFA/Horizons) p/ cada conversão; decidir se conecta ou remove o dead code.
- **Confirmado por:** finder + verificador adversarial.

<a id="aud-corr-5"></a>
### AUD-CORR-5 — Testes de fotometria circulares (vaidade)

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `test_photometry.cpp:70,135` vs `Photometry.cpp:74`.
- **Failure concreto:** os testes de `magInst` são circulares/vaidade (asserta `magInst == -2.5·log10(flux)` = exatamente a fórmula do código). Zero-point (`zp=mag-magInst`) e extinção (`-k·X`) só têm testes de guarda (nullopt), nenhum com valores conhecidos → flip de sinal passa verde. Classificação oráculo-vs-vaidade: GMST/airmass/proper-motion/angular-dist são bons oráculos; WCS round-trip e magInst/fluxFrac são self-consistency/vaidade. Problema é o CONJUNTO AUSENTE, não os 17+18 existentes.
- **Remediação:** substituir asserts circulares por caso com fluxo+ZP conhecidos → mag esperada externa; testar sinal de ZP e extinção.
- **Confirmado por:** finder + verificador adversarial.

<a id="aud-ci-3"></a>
### AUD-CI-3 — Auditoria numérica (itens 38.x) nunca roda em CI

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** os 12 workflows GitHub Actions.
- **Failure concreto:** os itens 38.1–38.5 (marcados ✅) NUNCA rodam em CI. Nenhum dos 12 workflows referencia cppcheck/clang-tidy/fsanitize/valgrind/target audit (grep vazio). Foi rodada uma vez manualmente; sem gate contínuo → regressão de UB/leak passaria despercebida (exatamente o que AUD-INPUT-1/2 e AUD-MEM-2 demonstram que ainda existe).
- **Remediação:** job nightly `cmake --build build --target audit` + build ASan+UBSan rodando a suíte, falhando em severidade crítica.
- **Confirmado por:** finder + orquestrador.

<a id="aud-ci-4"></a>
### AUD-CI-4 / AUD-DOC-2 — `qa-wsl2.yml` órfão (alvo removido, workflow vivo)

- **Severidade:** 🟠 IMPORTANTE (CI) / 🟢 COSMÉTICO (doc) — CONFIRMADO
- **Âncora:** `.github/workflows/qa-wsl2.yml`.
- **Failure concreto:** a doc removeu o alvo WSL2 (commit 1ebfb7a) mas o workflow continua em push/PR, buildando+testando 116+23 casos a cada commit para um alvo descontinuado. Custo de CI real.
- **Remediação:** deletar `qa-wsl2.yml` ou mover para `workflow_dispatch`-only.
- **Confirmado por:** finder + orquestrador.

<a id="aud-doc-1"></a>
### AUD-DOC-1 — Drift doc-vs-código na refração atmosférica

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `docs/technical-reference.md` §5 vs `Astronomy.cpp::applyRefractionCorrection`.
- **Failure concreto:** a doc §5 descreve fórmula primária com correção de pressão/temperatura (R0=60.4″, P0=1013.25, T0=283K) acima de 15° + Bennett abaixo. O código implementa APENAS Bennett incondicional p/ alt≥1°, sem branch de 15°, sem parâmetros P/T (a função nem os recebe). A correção meteorológica documentada não existe.
- **Remediação:** alinhar doc ao código (Bennett universal) ou implementar P/T se desejado.
- **Confirmado por:** finder + orquestrador.

<a id="aud-doc-3"></a>
### AUD-DOC-3 — 0/127 arquivos com SPDX-License-Identifier

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO (overlap [AUD-PROV-3-6](#aud-prov-3-6))
- **Âncora:** `src/` (127 arquivos).
- **Failure concreto:** 0/127 arquivos em `src/` com `SPDX-License-Identifier` apesar de AGPL-3.0. Sem NOTICE/THIRD_PARTY. Conformidade REUSE zero.
- **Remediação:** adicionar header SPDX aos 127 arquivos; gerar NOTICE.
- **Confirmado por:** finder + orquestrador.

<a id="aud-doc-4"></a>
### AUD-DOC-4 — Feature-fantasma: RAW DSLR / PDS4 marcados ✅ sem implementação

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO (grep pelo orquestrador)
- **Âncora:** `src/` (implementação ausente); item #21 da tabela.
- **Failure concreto:** o item #21 marca ✅ suporte a RAW DSLR (.cr2/.nef/.arw via libraw) e PDS/PDS4, mas `grep libraw|LibRaw|PDS4|.cr2|.nef|.arw` em `src/` = VAZIO. Não há implementação. Status ✅ incorreto.
- **Remediação:** confirmar com dev; corrigir status na tabela ou implementar.
- **Confirmado por:** orquestrador (grep).

<a id="aud-test-2"></a>
### AUD-TEST-2 — Cobertura fantasma: 10 casos fazem SKIP condicionados a caminho de máquina do dev

- **Severidade:** 🟠 IMPORTANTE — CONFIRMADO
- **Âncora:** `test_fits_functional.cpp` (item 48).
- **Failure concreto:** 10 casos fazem SKIP condicionados a arquivos FITS Pan-STARRS hardcoded em `/tmp/fits_test/` (caminho de máquina do dev), NUNCA provisionados em CI. Catch2 conta SKIP como pass → a tabela diz "10 ✅" mas 0/10 executam fora da máquina do autor. Confirmado no baseline do orquestrador (10 SKIPPED).
- **Remediação:** versionar fixtures pequenas/sintéticas no repo apontando p/ dentro do repo; ou marcar honestamente como "manual-only" na tabela.
- **Confirmado por:** finder + orquestrador (baseline).

### 4.3 🟢 COSMÉTICOS

<a id="aud-sec-5"></a>
### AUD-SEC-5 — ApiKeyStore fallback plain-text (avisado ao usuário)

- **Severidade:** 🟢 COSMÉTICO — CONFIRMADO
- **Âncora:** `ApiKeyStore`.
- **Failure concreto:** fallback plain-text em QSettings (perm 644) quando sem qt6keychain. MAS avisado ao usuário (badge amarelo em `SettingsDialog` + migração automática p/ keychain). Não é fallback silencioso. Chave não presente na conf atual.
- **Remediação:** `chmod 600` no arquivo de config, ou cifrar o valor no fallback.
- **Confirmado por:** finder.

<a id="aud-ci-2"></a>
### AUD-CI-2 — 12 arquivos de CI "modificados" = 100% ruído CRLF→LF

- **Severidade:** 🟢 COSMÉTICO — CONFIRMADO (orquestrador)
- **Âncora:** `build.yml` + 10 `qa-*.yml` + `runner-config.yml`.
- **Failure concreto:** os 12 arquivos "modificados" são 100% ruído CRLF→LF (`git diff --ignore-space-at-eol` = 0 linhas; 11 com CR). `.gitattributes` já força `eol=lf`. Nenhum gate alterado.
- **Remediação:** `git add --renormalize .` / `git checkout --` para limpar o working tree.
- **Confirmado por:** orquestrador.

<a id="aud-corr-6"></a>
### AUD-CORR-6 — Constante FWHM imprecisa (0.0077%)

- **Severidade:** 🟢 COSMÉTICO — CONFIRMADO
- **Âncora:** `Centroid.cpp:88,89,181,364,365`.
- **Failure concreto:** constante FWHM = 2.355 em vez de 2.3548200 (2√(2ln2)). Erro 0.0077%, irrelevante na prática. O ângulo θ da PSF elíptica está correto (rad→deg, normalizado).
- **Remediação:** usar 2.354820045.
- **Confirmado por:** finder.

<a id="aud-input-6"></a>
### AUD-INPUT-6 — UB de alinhamento em log de struct packed

- **Severidade:** 🟢 COSMÉTICO — CONFIRMADO
- **Âncora:** `ImageLoader.cpp:251`.
- **Failure concreto:** `hdr.frameCount` (struct `#pragma pack(1)`) passado por referência a `spdlog::info`. Dispara em todo `.ser` sob UBSan; x86 tolera, quebraria em ARM strict-align.
- **Remediação:** copiar campo p/ local antes de logar.
- **Confirmado por:** finder + orquestrador (UBSan).

---

## 5. PLAUSÍVEIS (não fechados)

<a id="aud-sec-4"></a>
### AUD-SEC-4 — Downgrade TLS por config sem enforcement https

- **Severidade:** 🟠 IMPORTANTE — PLAUSÍVEL
- 3 URLs configuráveis (`astrometry/baseUrl`, `catalog/vizierServer`, `mpc/submitUrl`) lidas de QSettings sem validação de esquema → podem ser rebaixadas p/ `http://` (envia API key/ADES/catálogo sem TLS). Sem bypass de `sslErrors` (bom). Exige acesso local à conf ou engenharia social. Tooltip avisa mas não faz enforcement.
- **Falta para fechar:** demonstrar cenário de exploração realista (acesso à conf) e confirmar que a chave/ADES realmente trafega em claro no downgrade.
- **Remediação:** validar `url.scheme()=="https"` (allowlist http para localhost).

<a id="aud-input-7"></a>
### AUD-INPUT-7 — OOB read em colunas opcionais de BINTABLE

- OOB read em colunas opcionais (`FitsTableReader` guarda com `.isEmpty()` em vez de `i<col.size()`); heap de vetor-variável TFORM P/Q corrompido → OOB.
- **Falta para fechar:** não fabricado por time-box — construir o `.fits` com TFORM P/Q e coluna opcional curta, rodar sob ASan. Fix defensivo recomendado.

<a id="aud-prov-8"></a>
### AUD-PROV-8 — GDL/NEMO/Siril não auditados linha-a-linha

- README credita GDL/NEMO/Siril (GPL/LGPL) como "aprendemos de" para StarDetector/ImageStacker/Centroid; não auditado linha-a-linha vs esses repos (só WCSLIB e SOFA no escopo desta rodada).
- **Falta para fechar:** rodar scancode-toolkit / diff estrutural contra esses repos numa próxima rodada.

<a id="aud-input-gaps"></a>
### AUD-INPUT-gaps — Gaps de input não cobertos

- RAW/PDS4 (não existe — ver [AUD-DOC-4](#aud-doc-4)); QImage TIFF/PNG (plugins Qt); XISF overflow teórico; catálogos locais Gaia/UCAC (herda [AUD-INPUT-5](#aud-input-5)).
- **Falta para fechar:** fabricar inputs hostis específicos para cada formato e rodar sob ASan.

<a id="aud-mem-gaps"></a>
### AUD-MEM-gaps — Gaps de memória não re-rodados nesta sessão

- `Calibration.cpp:276` float→int (`fx`/`fy` Inf se `tileCx[1]==tileCx[0]`); Valgrind/cppcheck/clang-tidy não re-rodados nesta sessão.
- **Falta para fechar:** rodar Valgrind + cppcheck + clang-tidy num build limpo e triar os achados.

---

## 6. Veredito por lente

- **AUD-INPUT — COM PROBLEMA GRAVE.** 2 CRÍTICOS (stack overflow, alloc sem teto) tornam qualquer `.fits` de poucos KB um vetor de crash. Bloqueadores de release. A lente desmentiu honestamente um falso-positivo do escopo (`naxes[2]`).

- **AUD-CORR — COM PROBLEMA GRAVE.** 1 CRÍTICO (90° errado ao MPC) + camada inteira de correção de frame sem oráculo de valor absoluto. Maior stake científico do produto.

- **AUD-MEM — COM PROBLEMA.** 1 CRÍTICO alcançável (NaN/Inf → abort) + leak upstream SEP; o código próprio é disciplinado em RAII (dominós fftw/fits/archive limpos).

- **AUD-SEC — RAZOÁVEL.** Sem CRÍTICO com exploit direto; `.runner` e supply-chain são riscos estruturais; sem bypass SSL; keychain avisado.

- **AUD-PROV — SEM BLOQUEIO, COM DÉBITO.** Sem incompatibilidade de licença bloqueante, MAS proveniência WCSLIB não-atribuída (requer jurídico) + ausência sistêmica de NOTICE.

- **AUD-CI/DOC/TEST — HIGIENE PENDENTE.** Auditoria fora do CI, cobertura fantasma (10 SKIP + feature RAW inexistente), drift de refração, 0 SPDX.

---

## 7. Tabela de rastreamento de remediação

> Status inicial: todos ❌ Pendente. A fase de remediação está sob decisão do líder. Os itens TODO correspondentes estão em [`TODO.md`](TODO.md).

| ID | Severidade | Status remediação | Item TODO |
|---|:---:|:---:|:---:|
| [AUD-INPUT-1](#aud-input-1) | 🔴 CRÍTICO | ✅ Resolvido `0463796` | Onda 1 |
| [AUD-INPUT-2](#aud-input-2) | 🔴 CRÍTICO | ✅ Resolvido `f3a7534`+`1d0fe30` | Onda 1 |
| [AUD-CORR-1](#aud-corr-1) | 🔴 CRÍTICO | ✅ Resolvido `9d3e43b` (astropy 168/168) | Onda 1 |
| [AUD-MEM-2](#aud-mem-2) | 🔴 CRÍTICO | ✅ Resolvido `2dfc872`+`b3295c8` | Onda 1 |
| [AUD-MEM-3](#aud-mem-3) | 🔴 CRÍTICO | ✅ Resolvido `44d6f22` | Onda 1 |
| [AUD-SEC-1](#aud-sec-1) | 🟠 IMPORTANTE | ✅ Resolvido `1a9ff5b` + token rotacionado | Onda 1 |
| [AUD-INPUT-3](#aud-input-3) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-INPUT-4](#aud-input-4) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-INPUT-5](#aud-input-5) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-MEM-1](#aud-mem-1) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-MEM-4](#aud-mem-4) | 🟠 IMPORTANTE | ✅ Resolvido `f3a7534`+`1d0fe30` (junto do INPUT-2) | Onda 1 |
| [AUD-SEC-2](#aud-sec-2) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-SEC-3](#aud-sec-3) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-CORR-2](#aud-corr-2) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-CORR-3](#aud-corr-3) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-CORR-4](#aud-corr-4) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-CORR-5](#aud-corr-5) | 🟠 IMPORTANTE | ❌ Pendente | Onda 2 |
| [AUD-PROV-1](#aud-prov-1) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-PROV-2](#aud-prov-2) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-PROV-3-6](#aud-prov-3-6) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-DOC-1](#aud-doc-1) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-DOC-3](#aud-doc-3) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-DOC-4](#aud-doc-4) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-CI-3](#aud-ci-3) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-CI-4](#aud-ci-4) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-TEST-2](#aud-test-2) | 🟠 IMPORTANTE | ❌ Pendente | Onda 3 |
| [AUD-SEC-4](#aud-sec-4) | 🟠 IMPORTANTE (plausível) | ❌ Pendente | Onda 3 |
| [AUD-SEC-5](#aud-sec-5) | 🟢 COSMÉTICO | ❌ Pendente | Onda 4 |
| [AUD-CI-2](#aud-ci-2) | 🟢 COSMÉTICO | ❌ Pendente | Onda 4 |
| [AUD-CORR-6](#aud-corr-6) | 🟢 COSMÉTICO | ❌ Pendente | Onda 4 |
| [AUD-INPUT-6](#aud-input-6) | 🟢 COSMÉTICO | ❌ Pendente | Onda 4 |
| [AUD-INPUT-7](#aud-input-7) | Plausível | ❌ Pendente | Onda 4 |
| [AUD-PROV-8](#aud-prov-8) | Plausível | ❌ Pendente | Onda 4 |
| [AUD-INPUT-gaps](#aud-input-gaps) | Plausível | ❌ Pendente | Onda 4 |
| [AUD-MEM-gaps](#aud-mem-gaps) | Plausível | ❌ Pendente | Onda 4 |

---

## 8. Parecer de prontidão

**Onda 1 RESOLVIDA em 2026-07-10** (branch `audit-remediation-onda1`, pushada; implementer ≠ reviewer adversarial ≠ orquestrador; suíte ASan/UBSan 100% verde; WCS validado vs astropy nas 8 projeções). Os 4 CRÍTICOS + `.runner` + AUD-MEM-4 estão fechados. **Bloqueadores de release removidos** — falta o merge à `main` (com CI) e as Ondas 2-4. Registro histórico do risco original: os CRÍTICOS de input hostil ([AUD-INPUT-1/2](#aud-input-1)) e de memória ([AUD-MEM-2](#aud-mem-2)) causavam crash/UB ao abrir arquivo de terceiro; o CRÍTICO de corretude ([AUD-CORR-1](#aud-corr-1)) punha coordenada errada no MPC. Duas divergências de severidade ficam registradas para o auditor externo: [AUD-SEC-1](#aud-sec-1) (CI-finder=CRÍTICO vs adjudicado IMPORTANTE) e [AUD-PROV-1](#aud-prov-1) (PROV-finder=CRÍTICO-compliance vs adjudicado IMPORTANTE, requer jurídico). O restante é débito de robustez, corretude testável, higiene de CI/doc e proveniência de licenças — endereçável nas Ondas 2–4.
