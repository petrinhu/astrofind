AUDITORIA COMPLETA DO PROJETO — dossiê AUDIT_FIND.md, read-only, orquestração main grok latest + trabalhadores grok latestest-1

Faça uma auditoria técnica COMPLETA deste repositório e entregue um DOSSIÊ consolidado em
AUDIT_FIND.md NA RAIZ (o "livro" que se entregaria a um auditor externo), classificando cada
achado por severidade. É READ-ONLY: a auditoria só INVESTIGA e REPORTA — nenhuma correção de
código nesta fase (a remediação é uma fase posterior, sob minha decisão).

## PLanejamento (modelos e papéis — NÃO misturar)
- PLANEJADOR = um C-level no modelo **GROK latest**: use o `internal-auditor`
  (dono do dossiê) e/ou `caetano-cto` para ESCOPAR a auditoria, decompor em lentes, e
  SINTETIZAR os retornos num livro coeso. O agente grok latest PLANEJA e CONSOLIDA — ele NÃO
  dispara subagentes em background (o harness perde os retornos: confirmado).
- EXECUTORES = agentes especialistas no modelo **Grok latest-1**, um por lente:
  `security-engineer`, `qa-engineer`, `backend-engineer`/`embedded-firmware-engineer`
  (conforme a stack), `software-architect`, `compliance-legal`/`claudio-clo`,
  `devops-sre`, `technical-writer`. O ORQUESTRADOR (thread principal) faz o FAN-OUT desses
  subagentes e CAPTURA os outputs — nunca um agente disparando outro em background.
- Regra de ouro: quem ACHA um problema (finder) NUNCA é quem o VERIFICA (verificador
  adversarial). Implementer ≠ reviewer ≠ orquestrador.

## Lentes da auditoria (pode podar/expandir conforme a stack e o porte)
1. Robustez de input / superfície pública: fuzzing e input HOSTIL (truncado, OOB, valores
   mentirosos, tamanhos negativos), guards de entrada, fail-clean vs crash do host.
2. Memory-safety: rode sob ASan/UBSan quando aplicável — UAF, double-free, leaks, buffer
   overflow, integer overflow, casts estreitantes (int→int e float→int), reentrância.
3. Segurança (AppSec): threat model (STRIDE), secrets vazados, authn/authz, injeção,
   criptografia, supply-chain (deps, SBOM, pinning), licenças das dependências.
4. Proveniência / clean-room (se houver reimplementação de terceiros): confirmar que a
   derivação é de specs públicas, NÃO cópia de código com licença incompatível.
5. Corretude: fórmulas de conversão (coordenada/unidade/sinal) SEM teste que as exercite =
   classe de bug silencioso; oráculos analíticos, não golden flaky.
6. CI/build/reprodutibilidade: gates de fato rodando, cache que não mascara quebra,
   compila do zero, encapsulamento (nenhum tipo de terceiro cruzando a fronteira pública).
7. Docs / licença / atribuição: SPDX, NOTICE, README, drift doc-vs-código.
8. Testes: cobertura SIGNIFICATIVA (não vaidade), suíte estável, testes que asseguram algo.

## Método por lente (adversarial que EXECUTA)
Para cada lente: finder (grok latest-1) mapeia os candidatos → verificador adversarial DIFERENTE
(grok latest-1) que EXECUTA/ataca cada achado (roda o código, mutation testing, fuzz sob
sanitizer — "com dente"), com repro mínimo. Achado sem repro executável cai para PLAUSÍVEL,
não CONFIRMADO. Aplique AUDITORIA-DOMINÓ: todo achado pontual vira varredura da superfície
inteira atrás do gêmeo ("esse guard/bug existe num irmão e faltou aqui?") — nunca assuma
isolado.

## Identificação por ID ESTÁVEL (não por linha — linhas apodrecem)
Dê a CADA achado um ID estável no formato AUD-<LENTE>-<n> (ex.: AUD-SEC-1, AUD-MEM-3,
AUD-INPUT-2). TODA referência cruzada — entre o dossiê, a tabela de pendências e os commits
de remediação — é feita por esse ID, NUNCA por número de linha (que apodrece a cada edição).
Ao ancorar no código, prefira nome de FUNÇÃO/SÍMBOLO + o ID do achado; o número de arquivo:linha
só aparece dentro do repro (é um snapshot datado, não uma referência viva).

## Taxonomia de severidade
CRÍTICO (crash/corrupção/RCE/UB/vazamento explorável) · IMPORTANTE (bug real, corretude ou
robustez, sem exploit direto) · COSMÉTICO (drift de doc, mina defensiva não-alcançável,
estilo). Cada achado: ID estável + severidade + descrição do failure (inputs concretos →
efeito) + repro executável + remediação sugerida.

## Disciplinas invioláveis
- "Relatório de agente não é prova": o orquestrador RE-VERIFICA independentemente os
  achados (build limpo, spot-check das claims, roda o repro) antes de aceitar.
- Read-only: NÃO corrigir nesta fase; NÃO fazer `git push`; NÃO alterar código de produção.
  Se um agente precisar commitar o dossiê, pathspec EXPLÍCITO (nunca `git add -A`).
- Escopo do porte (anti over-engineering): calibrar a profundidade ao tamanho/criticidade do
  projeto — não montar 8 lentes num utilitário de 200 linhas.

## Entregável (dossiê + integração com a tabela de pendências)
1. AUDIT_FIND.md na RAIZ: sumário executivo + índice mestre, todos os achados CONFIRMADOS
   ordenados por severidade (cada um com seu ID estável, repro e remediação), os PLAUSÍVEIS
   separados, um veredito por lente, e uma tabela de rastreamento de remediação (status por ID).
2. TABELA DE PENDÊNCIAS (TODO.md na raiz): acrescente CADA achado que exige remediação como
   um item na tabela, com seu ID AUD-* e um LINK para a seção correspondente do AUDIT_FIND.md
   (link por âncora/ID, NUNCA por linha). O item da tabela referencia o achado por ID.
3. PASSO FINAL — atualize a tabela de pendências classificando as feats de remediação
   oriundas da auditoria como URGENTES (prioridade Alta, onda no topo da ordem de execução,
   respeitando os pré-requisitos). Rode a reordenação (topological + WSJF) para que essas
   feats urgentes subam ao topo, sem quebrar dependências.
