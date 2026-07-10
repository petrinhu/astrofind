# TODO — Remediação da Auditoria AstroFind

> **Nota:** Feats de remediação oriundas da auditoria read-only de 2026-07-10; ver [`AUDIT_FIND.md`](AUDIT_FIND.md). Fase de remediação sob decisão do líder.
>
> Ordenação: topological + WSJF, urgentes no topo. Links por âncora de ID (nunca por linha). Legenda: ✅ Resolvido · ❌ Pendente.
>
> **✅ Onda 1 CONCLUÍDA (2026-07-10)** — branch `audit-remediation-onda1` (pushada; CI só no fim de tudo). Implementer ≠ reviewer adversarial ≠ orquestrador; cada review adversarial pegou 1 bug real (bypass de overflow no NAXIS3; fail-open NaN no netFlux), corrigidos. Suíte ASan/UBSan 100% verde, WCS validado vs astropy nas 8 projeções.

---

## Onda 1 — CRÍTICOS + token vivo (🔴 Urgente)

| # | ID (AUD-*) | Feature (remediação) | Severidade | Status | Commit(s) | Link |
|---|---|---|:---:|:---:|---|---|
| 1 | AUD-INPUT-1 | Rejeitar `naxis > 3` antes de qualquer chamada cfitsio (fim do stack-buffer-overflow) | 🔴 CRÍTICO | ✅ Resolvido | `0463796` | [AUD-INPUT-1](AUDIT_FIND.md#aud-input-1) |
| 2 | AUD-INPUT-2 | Validar dims (w/h/depth) contra teto + `fileSize` antes do produto; `safeResizeFloat` | 🔴 CRÍTICO | ✅ Resolvido | `f3a7534` + `1d0fe30` (fix bypass NAXIS3) | [AUD-INPUT-2](AUDIT_FIND.md#aud-input-2) |
| 3 | AUD-CORR-1 | Polo nativo correto p/ não-zenitais (WCS Paper II, in-house) — fim do +90° no MPC | 🔴 CRÍTICO | ✅ Resolvido | `9d3e43b` (validado vs astropy 168/168) | [AUD-CORR-1](AUDIT_FIND.md#aud-corr-1) |
| 4 | AUD-MEM-2 | Guard `isfinite` antes de `static_cast<int>` + rejeitar `netFlux` NaN (fim do abort e do fail-open) | 🔴 CRÍTICO | ✅ Resolvido | `2dfc872` + `b3295c8` (fix fail-open netFlux) | [AUD-MEM-2](AUDIT_FIND.md#aud-mem-2) |
| 5 | AUD-MEM-3 | Guard de sanidade de centroide à prova de NaN (`!isfinite \|\| fora do box`) | 🔴 CRÍTICO | ✅ Resolvido | `44d6f22` | [AUD-MEM-3](AUDIT_FIND.md#aud-mem-3) |
| 5b | AUD-MEM-4 | Re-validar NAXIS antes do cast no path multi-ext RGB (coberto junto do INPUT-2) | 🟠 IMPORTANTE | ✅ Resolvido | `f3a7534` + `1d0fe30` | [AUD-MEM-4](AUDIT_FIND.md#aud-mem-4) |
| 6 | AUD-SEC-1 | `.runner` → `.gitignore` + `chmod 600` + token rotacionado no Codeberg pelo líder | 🟠 IMPORTANTE | ✅ Resolvido | `1a9ff5b` | [AUD-SEC-1](AUDIT_FIND.md#aud-sec-1) |

**Follow-ups descobertos nos reviews adversariais da Onda 1** (novos itens, ver Onda 2/3):
- Os dois bugs achados pelos reviewers (bypass overflow NAXIS3; fail-open NaN netFlux) já foram corrigidos nos commits `1d0fe30`/`b3295c8` acima.
- **AUD-TEST-3 (novo)**: `loadFitsCube`/`loadFitsHdu`/`scanImageHdus` não têm nenhum teste unitário/funcional — exatamente os loaders mais expostos. Adicionar cobertura. → Onda 3.
- **AUD-CORR-2 enriquecido**: ao portar o teste round-trip das 8 projeções, incluir Dec negativa, polo (±89.9) e wrap de RA (0/360) — casos que o teste atual não cobre e que o review validou vs astropy. → Onda 2.
- **Defense-in-depth (opcional, UI)**: `MainWindow_measurement.cpp:175` faz `obs.mag = phot->magInst + ...` sem `isfinite` — hoje blindado na origem; um guard lá seria defesa extra (decisão do dono da UI, fora do escopo "não tocar UI").

---

## Onda 2 — Robustez / corretude / segurança (🟠 Alta)

> Pré-req: **AUD-CORR-2 depende de AUD-CORR-1 corrigido** (o teste das 8 projeções só fica verde após o fix do polo nativo).

| # | ID (AUD-*) | Feature (remediação) | Severidade | Onda | Prioridade | Link |
|---|---|---|:---:|:---:|:---:|---|
| 7 | AUD-INPUT-3 | SER loader: validar `>0 && <TETO` antes do cast (igual ao FITS) | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-INPUT-3](AUDIT_FIND.md#aud-input-3) |
| 8 | AUD-INPUT-4 | Extração: rejeitar entradas não-`AE_IFREG` (symlink/FIFO); ou migrar p/ QuaZip com checagem de tipo | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-INPUT-4](AUDIT_FIND.md#aud-input-4) |
| 9 | AUD-INPUT-5 | BINTABLE: propagar erro em vez de fabricar NaN; cross-check `rows()`; filtrar NaN na importação | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-INPUT-5](AUDIT_FIND.md#aud-input-5) |
| 10 | AUD-MEM-1 | Patch local no SEP (`convert_to_catalog` duplo QMALLOC) via FetchContent REPLACE ou PR upstream | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-MEM-1](AUDIT_FIND.md#aud-mem-1) |
| 11 | AUD-MEM-4 | RGB multi-ext + Spectrum1D: validar teto antes do cast; padronizar guard pós-cast do single-image path | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-MEM-4](AUDIT_FIND.md#aud-mem-4) |
| 12 | AUD-SEC-2 | Pinar 6 deps FetchContent por commit SHA/URL_HASH; SBOM (syft); política de deps | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-SEC-2](AUDIT_FIND.md#aud-sec-2) |
| 13 | AUD-SEC-3 | `setTransferTimeout` + cancel/watchdog em AstrometryClient/CatalogClient/HorizonsClient | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-SEC-3](AUDIT_FIND.md#aud-sec-3) |
| 14 | AUD-CORR-2 | Teste `[wcs]` parametrizado nas 8 projeções (valor absoluto vs astropy/WCSLIB) — **pós AUD-CORR-1** | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-CORR-2](AUDIT_FIND.md#aud-corr-2) |
| 15 | AUD-CORR-3 | Testes `[refraction]` com geometria conhecida + gate `isSpaceTelescope` no pipeline | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-CORR-3](AUDIT_FIND.md#aud-corr-3) |
| 16 | AUD-CORR-4 | Testes com oráculo externo p/ aberração/prec/nut/eclíptica; decidir conectar ou remover dead code | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-CORR-4](AUDIT_FIND.md#aud-corr-4) |
| 17 | AUD-CORR-5 | Substituir asserts circulares de fotometria por caso com fluxo+ZP conhecidos; testar sinal de ZP/extinção | 🟠 IMPORTANTE | 2 | 🟠 Alta | [AUD-CORR-5](AUDIT_FIND.md#aud-corr-5) |

---

## Onda 3 — Doc / licença / CI / testes (🟡 Média)

| # | ID (AUD-*) | Feature (remediação) | Severidade | Onda | Prioridade | Link |
|---|---|---|:---:|:---:|:---:|---|
| 18 | AUD-PROV-1 | Apurar proveniência WCSLIB (paper vs prj.c); NOTICE+copyright+LGPL se código; alinhar README — **requer jurídico/CLO** | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-PROV-1](AUDIT_FIND.md#aud-prov-1) |
| 19 | AUD-PROV-2 | NOTICE para SEP (LGPL-3.0 + MIT), copyright Bertin/SEP, cópia da LGPL-3.0 | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-PROV-2](AUDIT_FIND.md#aud-prov-2) |
| 20 | AUD-PROV-3-6 | NOTICE consolidado (QuaZip/spdlog/json/FFTW3/cfitsio/CCfits) + coluna de licença no README | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-PROV-3-6](AUDIT_FIND.md#aud-prov-3-6) |
| 21 | AUD-DOC-1 | Alinhar doc de refração ao código (Bennett universal) ou implementar P/T | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-DOC-1](AUDIT_FIND.md#aud-doc-1) |
| 22 | AUD-DOC-3 | Adicionar header SPDX aos 127 arquivos de `src/`; conformidade REUSE | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-DOC-3](AUDIT_FIND.md#aud-doc-3) |
| 23 | AUD-DOC-4 | Corrigir status ✅ de RAW DSLR/PDS4 (feature inexistente) ou implementar | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-DOC-4](AUDIT_FIND.md#aud-doc-4) |
| 24 | AUD-CI-3 | Job nightly `target audit` + build ASan/UBSan rodando a suíte, falhando em severidade crítica | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-CI-3](AUDIT_FIND.md#aud-ci-3) |
| 25 | AUD-CI-4 | Deletar `qa-wsl2.yml` órfão ou mover para `workflow_dispatch`-only | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-CI-4](AUDIT_FIND.md#aud-ci-4) |
| 26 | AUD-TEST-2 | Versionar fixtures FITS sintéticas no repo (fim dos 10 SKIP) ou marcar "manual-only" na tabela | 🟠 IMPORTANTE | 3 | 🟡 Média | [AUD-TEST-2](AUDIT_FIND.md#aud-test-2) |
| 27 | AUD-SEC-4 | Validar `url.scheme()=="https"` (allowlist localhost) nas 3 URLs de QSettings | 🟠 IMPORTANTE (plausível) | 3 | 🟡 Média | [AUD-SEC-4](AUDIT_FIND.md#aud-sec-4) |

---

## Onda 4 — Cosméticos + plausíveis a fechar (🟢 Baixa)

| # | ID (AUD-*) | Feature (remediação) | Severidade | Onda | Prioridade | Link |
|---|---|---|:---:|:---:|:---:|---|
| 28 | AUD-SEC-5 | `chmod 600` no config do fallback ou cifrar o valor da API key | 🟢 COSMÉTICO | 4 | 🟢 Baixa | [AUD-SEC-5](AUDIT_FIND.md#aud-sec-5) |
| 29 | AUD-CI-2 | `git add --renormalize .` para limpar ruído CRLF dos 12 workflows | 🟢 COSMÉTICO | 4 | 🟢 Baixa | [AUD-CI-2](AUDIT_FIND.md#aud-ci-2) |
| 30 | AUD-CORR-6 | Usar constante FWHM 2.354820045 em Centroid.cpp | 🟢 COSMÉTICO | 4 | 🟢 Baixa | [AUD-CORR-6](AUDIT_FIND.md#aud-corr-6) |
| 31 | AUD-INPUT-6 | Copiar `frameCount` p/ local antes de logar (fim do UB de alinhamento) | 🟢 COSMÉTICO | 4 | 🟢 Baixa | [AUD-INPUT-6](AUDIT_FIND.md#aud-input-6) |
| 32 | AUD-INPUT-7 | Fechar/fabricar OOB read de colunas opcionais BINTABLE (TFORM P/Q) sob ASan; fix defensivo | Plausível | 4 | 🟢 Baixa | [AUD-INPUT-7](AUDIT_FIND.md#aud-input-7) |
| 33 | AUD-PROV-8 | Auditar GDL/NEMO/Siril linha-a-linha (scancode/diff) numa próxima rodada | Plausível | 4 | 🟢 Baixa | [AUD-PROV-8](AUDIT_FIND.md#aud-prov-8) |
| 34 | AUD-INPUT-gaps | Fabricar inputs hostis p/ RAW/PDS4/QImage/XISF/catálogos locais e rodar sob ASan | Plausível | 4 | 🟢 Baixa | [AUD-INPUT-gaps](AUDIT_FIND.md#aud-input-gaps) |
| 35 | AUD-MEM-gaps | Rodar Valgrind + cppcheck + clang-tidy num build limpo; triar `Calibration.cpp:276` float→int | Plausível | 4 | 🟢 Baixa | [AUD-MEM-gaps](AUDIT_FIND.md#aud-mem-gaps) |
