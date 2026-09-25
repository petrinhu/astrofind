# Security Policy / Política de Segurança

> **Last reviewed / Última revisão:** 2026-09-25
> **Owner:** Petrus Silva Costa (petrinhu@yahoo.com.br)

---

## 🇬🇧 English

### How to report a vulnerability

If you found a security flaw ("vulnerability": a bug that lets an attacker do something
they shouldn't, e.g. crash the app with a malicious file or leak a secret) in this project,
please **do not open a public issue**. Report it privately by email to:

**petrinhu@yahoo.com.br**

Include, if possible:

- Description of the flaw and its impact.
- Steps to reproduce it (or a proof of concept, a minimal file/input that triggers it).
- The affected version or commit hash.

I will acknowledge receipt as soon as possible and work on a fix before any public
disclosure ("disclosure": telling the world about the flaw, normally done only after
a fix is released). Thank you for reporting responsibly.

### Scope

AstroFind is a desktop application (Qt6/C++). Relevant security surfaces include:
file parsing (FITS/SER/XISF/BINTABLE, PDS3/PDS4 labels, DSLR RAW via LibRaw, and
ZIP/TAR/7Z/RAR archives: an attacker-controlled file could try to trigger a crash or
memory corruption), network calls (astrometry.net, VizieR, JPL Horizons, SkyBoT, MPC
submission), and stored credentials (API keys via Qt Keychain or the `QSettings`
fallback). The audit dossier [`AUDIT_FIND.md`](AUDIT_FIND.md) lists every finding with
its status **at the time of the audit**. How each one was fixed is tracked in
[`TODO.md`](TODO.md), where every row cites the commit or pull request: since version
1.2.0 all audit items are marked ✅ (fixed). If you plan to rely on a specific item,
check its row first.

---

## 🇧🇷 Português

### Como reportar uma vulnerabilidade

Se você encontrou uma falha de segurança ("vulnerabilidade": um bug que permite a um
atacante fazer algo que não deveria, ex.: travar o app com um arquivo malicioso ou
vazar um segredo) neste projeto, por favor **não abra um issue público**. Reporte em
sigilo por email:

**petrinhu@yahoo.com.br**

Inclua, se possível:

- Descrição da falha e o impacto.
- Passos para reproduzir (ou uma prova de conceito, um arquivo/entrada mínima que
  dispara o problema).
- Versão ou commit afetado.

Vou confirmar o recebimento assim que possível e trabalhar numa correção antes de
qualquer divulgação pública ("disclosure": contar ao mundo sobre a falha, normalmente
feito só depois que a correção já foi lançada). Obrigado por reportar de forma
responsável.

### Escopo

AstroFind é uma aplicação desktop (Qt6/C++). Superfícies de segurança relevantes
incluem: parsing de arquivos (FITS/SER/XISF/BINTABLE, rótulos PDS3/PDS4, RAW de DSLR
via LibRaw e arquivos ZIP/TAR/7Z/RAR: um arquivo controlado por um atacante pode tentar
disparar um crash ou corrupção de memória), chamadas de rede (astrometry.net, VizieR,
JPL Horizons, SkyBoT, submissão ao MPC), e credenciais armazenadas (chaves de API via Qt
Keychain ou o fallback `QSettings`). O dossiê de auditoria [`AUDIT_FIND.md`](AUDIT_FIND.md)
lista todos os achados com o status **da época da auditoria**. Como cada um foi corrigido
fica no [`TODO.md`](TODO.md), em que cada linha cita o commit ou pull request: desde a
versão 1.2.0 todos os itens da auditoria estão marcados ✅ (corrigidos). Se você depende
de um item específico, confira a linha dele antes.
