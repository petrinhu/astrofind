# AstroFind: instruções para o Claude Code

AstroFind é um aplicativo Qt6/C++ de astrometria para detecção e medição de asteroides.
Stack: Qt6, CMake, cfitsio, fftw3.
Linguagem: C++ (`src/`) com traduções Qt (`i18n/astrofind_pt_BR.ts` → `.qm`).

## Caminhos

- Pasta temporária do usuário: `/home/petrus/IDrive/Documentos/temp/`, com logs de CI, screenshots e artefatos de debug compartilhados nas sessões.

## Leis do projeto

Leia [`GODS_LAWS.md`](GODS_LAWS.md) antes de agir. `L-NN` sem prefixo é lei global.

| Lei | Gatilho: dispara quando você vai... |
|---|---|
| [AF-01](GODS_LAWS.md#af-01) | citar ou usar API de Qt6, CMake ou outra biblioteca externa |
| [AF-02](GODS_LAWS.md#af-02) | começar implementação não trivial, depurar, ou criar feature |
| [AF-03](GODS_LAWS.md#af-03) | criar tela, ícone, lógica central ou chave de settings |
| [AF-04](GODS_LAWS.md#af-04) | acrescentar string `tr()` |
| [AF-05](GODS_LAWS.md#af-05) | acrescentar função à interface |
| [AF-06](GODS_LAWS.md#af-06) | terminar uma tarefa, ou acrescentar item à tabela de tarefas abaixo |

## Tabela de tarefas

Regra de exibição: AF-06.

| # | Feature | Categoria | Prioridade | Dificuldade | Status |
|---|---------|-----------|:----------:|:-----------:|:------:|
| 1 | Aviso B&W vs Color ao carregar (checkbox "Mostrar sempre" + Settings) | UX | Alta | D1 | ✅ |
| 2 | Transfer function: Linear (seletor de modo) | Renderização | Alta | D1 | ✅ |
| 3 | Transfer function: Raiz Quadrada | Renderização | Alta | D1 | ✅ |
| 4 | Transfer function: Asinh | Renderização | Alta | D1 | ✅ |
| 5 | Transfer function: Logarítmica | Renderização | Alta | D2 | ✅ |
| 6 | Seletor de transfer function no BackgroundRangeDialog | UX | Alta | D2 | ✅ |
| 7 | Suporte a imagens coloridas NAXIS3=3 | Core | Alta | D3 | ✅ |
| 8 | Correção de refração atmosférica nas coordenadas reportadas (desvio de até ~1' a 30° de altitude; ignorar se isSpaceTelescope) | Astrometria | Alta | D3 | ✅ |
| 9 | PSF elíptica com ângulo de rotação (Levenberg-Marquardt / MPFIT): mede FWHM real e detecta coma/tracking | Fotometria | Alta | D3 | ✅ |
| 10 | Máscara de pixels ruins (hot pixels, bad columns) com interpolação por vizinhos; evita falsas detecções | Core | Alta | D3 | ✅ |
| 11 | Detecção de traço (streak) por análise de blob: eixos, ângulo, elongação — identifica asteroides rápidos e satélites | Detecção | Alta | D3 | ✅ |
| 12 | Conversão ICRS → CIRS → topocêntrico para relatórios ADES com residuais sub-arcseg corretos | Astrometria | Alta | D4 | ✅ |
| 13 | Renderização de pixels NaN/indefinidos em cor distinta | Renderização | Média | D2 | ✅ |
| 14 | False color / LUT (Hot, Cool, Viridis, Grayscale) | Renderização | Média | D2 | ✅ |
| 15 | Transfer function: Equalização de histograma | Renderização | Média | D2 | ✅ |
| 35 | VerificationDialog: QListWidget "Objetos Conhecidos" exibe área em branco quando vazio — adicionar placeholder "Nenhum objeto conhecido próximo" | Bug/UX | Alta | D1 | ✅ |
| 16 | Overlay de coordenadas eclípticas + aviso quando campo está próximo ao plano galáctico (alta extinção) | UI/Astrometria | Média | D2 | ✅ |
| 17 | Estimativa de background por filtro de mediana deslizante (remove gradientes antes da detecção) | Core | Média | D2 | ✅ |
| 18 | Painel de histograma de pixels com ajuste gaussiano ao fundo (diagnóstico de ruído, faixas de brilho) | UI | Média | D2 | ✅ |
| 19 | Suporte a cor via multi-extensão (3 HDUs) | Core | Média | D3 | ✅ |
| 20 | Navegação de extensões HDU (painel lateral) | Core/UI | Média | D3 | ✅ |
| 21 | Suporte a outros formatos astronômicos: SER (.ser), XISF (.xisf), TIFF 16-bit (.tif), PNG 16-bit (.png) | Core/IO | Média | D3 | ✅ |
| 21.1 | RAW DSLR (.cr2/.cr3/.nef/.arw/.dng/.raf/.orf/.rw2/.pef… via LibRaw, opcional) — implementado 2026-09-24 (PR #7): CFA linear, luminância superpixel 2×2, EXIF (horário ambíguo); CI verde em 15 jobs com DNG sintético | Core/IO | Média | D3 | ✅ |
| 21.2 | PDS3/PDS4 (.img+.lbl, rótulo embutido, .xml) — implementado 2026-09-24 (PR #7): parser próprio, validação de tamanho antes de alocar; CI verde em 15 jobs | Core/IO | Média | D3 | ✅ |
| 22 | ClumpFind: detecção de fontes por contorno de intensidade — melhor para fontes blendadas e traços | Detecção | Média | D4 | ✅ |
| 23 | Filtro Laplaciano / unsharp-mask para realce de imagem no blink (visualização de candidatos fracos) | Renderização | Baixa | D2 | ✅ |
| 24 | Integração CCfits (bundled em ./originals) | Infraestrutura | Baixa | D3 | ✅ |
| 25 | Suporte a projeções WCS além de TAN (SIN, ARC, GLS, MER, AIT, STG) para FITS de campo largo | Core | Baixa | D3 | ✅ |
| 26 | Painel de espectro de potência 2-D (FFT) para diagnóstico de ruído periódico e qualidade de foco | Diagnóstico | Baixa | D3 | ✅ |
| 27 | Cubo FITS NAXIS=3 — animação temporal | Avançado | Baixa | D4 | ✅ |
| 28 | Plot de espectro NAXIS=1 (Qt6::Charts) | Avançado | Baixa | D4 | ✅ |
| 29 | Importar resultados DAOPHOT em FITS BINTABLE (via FitsTableReader/CCfits) — posições, magnitudes, erros de estrelas detectadas | Importação | Baixa | D3 | ✅ |
| 30 | Catálogos locais em FITS BINTABLE (USNO-B, UCAC, Gaia local) — leitura offline via FitsTableReader para astrometria sem internet | Catálogo | Baixa | D3 | ✅ |
| 31 | Importar tabelas de redução externas (IRAF/Astropy FITS tables) — RA/Dec/mag de arquivos gerados por outros softwares | Importação | Baixa | D3 | ✅ |
| 32 | Suporte a arquivos compactados além de ZIP: TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR via libarchive — requer: sudo dnf install libarchive-devel | IO | Baixa | D2 | ✅ |
| 33 | Atualizar traduções (.ts → .qm, PT-BR e EN) | Documentação | — fazer após base estável | D1 | ✅ |
| 34 | Atualizar Help (PT-BR e EN) | Documentação | — fazer após base estável | D2 | ✅ |
| 35 | Manual resumido (com links navegáveis) | Documentação | — fazer após base estável | D2 | ✅ |
| 36 | Manual extenso (com links navegáveis) | Documentação | — fazer após base estável | D3 | ✅ |
| 37 | HowTo passo a passo de todas as funções (com links navegáveis) | Documentação | — fazer após base estável | D3 | ✅ |
| 39 | Manual técnico de cálculos astronômicos (Markdown + LaTeX math `$...$`): estrutura FITS/SER/XISF, WCS pipeline (IWC→nativo→celeste), projeções TAN/SIN/ARC/STG/CAR/MER/GLS/AIT, refração atmosférica, ICRS→CIRS→topocêntrico, aberração anual, precessão, nutação, PSF elíptica, fotometria diferencial, zero-point, airmass, Julian Date, coordenadas eclípticas/galácticas, ADES/MPC — `docs/technical-reference.md` | Documentação | — fazer após base estável | D3 | ✅ |
| 38 | Auditoria numérica: verificar todos os tipos numéricos (float/int overflow, div/0, UB, cast inseguro, stack overflow em recursão) | Qualidade | P0 — fazer por último, após base estável | D3 | ✅ |
| 38.1 | Auditoria — ASan + UBSan: recompilar com -fsanitize=address,undefined e rodar toda a suite de testes (zero instalação necessária) | Qualidade | P0 | D1 | ✅ |
| 38.2 | Auditoria — Valgrind: rodar astrofind_tests sob memcheck, gerar relatório de leaks e acessos inválidos — requer: sudo dnf install valgrind | Qualidade | P0 | D2 | ✅ |
| 38.3 | Auditoria — cppcheck: análise estática de todo src/ com --enable=all, exportar relatório XML — requer: sudo dnf install cppcheck | Qualidade | P0 | D1 | ✅ |
| 38.4 | Auditoria — clang-tidy: análise estática com checks de bugprone, cert, cppcoreguidelines — requer: sudo dnf install clang-tools-extra | Qualidade | P0 | D2 | ✅ |
| 38.5 | Auditoria — target CMake `audit`: automatizar 38.1–38.4 em um único `cmake --build build --target audit` com relatório consolidado | Qualidade | P0 | D2 | ✅ |
| 39 | Testes — FITS loader: abertura, metadados, imagens inválidas (2 casos) | Testes | — | D1 | ✅ |
| 40 | Testes — ImageSession: criação, adição de imagens, persistência (3 casos) | Testes | — | D1 | ✅ |
| 41 | Testes — StarDetector: detecção básica, threshold, limite de estrelas (3 casos) | Testes | — | D2 | ✅ |
| 42 | Testes — Calibration: dark/flat/bias, subtração de background, máscaras (8 casos) | Testes | — | D2 | ✅ |
| 43 | Testes — Centroid: momentos, PSF gaussiana, PSF elíptica LM (6 casos) | Testes | — | D2 | ✅ |
| 44 | Testes — ImageStacker: Add/Average/Median, alinhamento FFT, Track&Stack (8 casos) | Testes | — | D2 | ✅ |
| 45 | Testes — MovingObjectDetector: trilhas, tolerância, SNR mínimo (5 casos) | Testes | — | D2 | ✅ |
| 46 | Testes — MpcOrb: parsing, lookup, atualização (4 casos) | Testes | — | D2 | ✅ |
| 47 | Testes — AdesReport: geração XML, PSV, campos obrigatórios, ICRS (10 casos) | Testes | — | D2 | ✅ |
| 48 | Testes — FitsFunctional: leitura multi-HDU, multi-extensão RGB, SER, XISF (10 casos) | Testes | — | D2 | ✅ |
| 49 | Testes — Astronomy: refração, aberração, precessão, nutação, CIRS (17 casos) | Testes | — | D3 | ✅ |
| 50 | Testes — Photometry: fotometria diferencial, zero-point, curva de crescimento (18 casos) | Testes | — | D2 | ✅ |
| 51 | Testes — Robustness: imagens degeneradas, overflow, valores extremos, NaN (22 casos) | Testes | — | D3 | ✅ |
| 52 | Testes UI — SettingsDialog: load/save/reset de configurações (2 casos) | Testes UI | — | D1 | ✅ |
| 53 | Testes UI — CalibrationPanel: dark/flat/bias widgets, estado habilitado/desabilitado (9 casos) | Testes UI | — | D2 | ✅ |
| 54 | Testes UI — SessionSignals: sinais entre MainWindow, WorkflowPanel e BlinkWidget (12 casos) | Testes UI | — | D2 | ✅ |
| 55.1 | Teste de build e execução — Ubuntu 24.04 LTS (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.2 | Teste de build e execução — Debian 12 Bookworm (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.3 | Teste de build e execução — Arch Linux (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.4 | Teste de build e execução — Manjaro (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.5 | Teste de build e execução — Linux Mint 22 (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.6 | Teste de build e execução — openSUSE Tumbleweed (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.7 | Teste de build e execução — Pop!_OS 22.04 (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.8 | Teste de build e execução — Rocky Linux 9 (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.9 | Teste de build e execução — Zorin OS 17 (container Docker) | QA Distros | — após base estável | D2 | ✅ |
| 55.10 | Teste de build e execução — Fedora (distro de desenvolvimento — já compila) | QA Distros | — | D1 | ✅ |
