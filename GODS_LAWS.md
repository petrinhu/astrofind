# GODS_LAWS.md: AstroFind (astrometrica)

> Ordens do líder para este projeto. Cada lei tem um gatilho, conferido no momento da ação.

## Protocolo de uso

1. Antes de agir, confira o índice de gatilhos do `CLAUDE.md` deste projeto; se um gatilho casa, leia a lei inteira antes do primeiro comando.
2. Siga o protocolo de uso do `~/.claude/GODS_LAWS.md`. No que este arquivo silencia, vale a lei global; `L-NN` sem prefixo é lei global e `AF-NN` é lei daqui.

---

## AF-01

**Data:** regra permanente.

Use sempre o context7 ao referenciar Qt6, CMake ou qualquer API de biblioteca externa. Antes de responder sobre classes, métodos ou signals/slots do Qt, busque a documentação atual pelo context7.

## AF-02

**Data:** regra permanente.

Em toda tarefa de implementação não trivial, use `recommend_skills` (superpowers-mcp) para ver se existe skill relevante. Para depurar, use a skill `debugging`; para feature nova, a `tdd` ou a `brainstorm`, conforme o caso.

## AF-03

**Data:** regra permanente.

- Construa a UI em `src/ui/` só com widgets Qt em código, sem arquivos `.ui`.
- Mantenha a lógica central em `src/core/` sem dependência de UI Qt.
- Desenhe os ícones por programa, via QPainter, em `AppIcons.cpp`.
- Use as chaves de settings dentro dos grupos `observer/*`, `camera/*`, `astrometry/*`, `catalog/*`, `photometry/*`, `report/*`, `ui/*` e `display/*`.

## AF-04

**Data:** regra permanente.

Depois de acrescentar strings `tr()`, rode `lupdate-qt6` e em seguida `lrelease-qt6`.

## AF-05

**Data:** regra permanente.

Não modifique a toolbar. Acrescente funções por menus, menus de contexto, docks e atalhos de teclado.

## AF-06

**Data:** regra permanente.

Sempre que houver tarefa pendente na tabela de tarefas do `CLAUDE.md`, mostre a tabela inteira atualizada ao fim de toda resposta de tarefa concluída, com ✅ para os itens concluídos e ❌ para os pendentes. Mostre-a automaticamente, sem esperar o usuário pedir.
