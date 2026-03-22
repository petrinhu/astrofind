#!/usr/bin/env bash
# =============================================================================
# AstroFind Installer — install.sh
# Version 0.9.0-beta
# =============================================================================
# Usage:
#   ./install.sh           # interactive (auto-detect locale)
#   ./install.sh --lang pt # force Portuguese
#   ./install.sh --lang en # force English
#   ./install.sh --help
# =============================================================================

set -euo pipefail

# ─── Constants ────────────────────────────────────────────────────────────────
readonly VERSION="0.9.0-beta"
readonly REPO="petrinhu/astrofind"
readonly BASE_URL="https://codeberg.org/${REPO}/releases/download/v${VERSION}"
readonly RPM_FILE="astrofind-0.9.0-0.beta1.x86_64.rpm"
readonly DEB_FILE="astrofind_0.9.0~beta-1_amd64.deb"
readonly ARCH_FILE="astrofind-0.9.0-beta-arch-PKGBUILD.tar.gz"

readonly BINARY="/usr/bin/AstroFind"
readonly DESKTOP_SRC="$(dirname "$(realpath "${BASH_SOURCE[0]}")")/../astrofind.desktop"
readonly ICON_NAME="astrofind"

TMPDIR_WORK=""
LANG_FORCED=""

# ─── ANSI Colors ──────────────────────────────────────────────────────────────
if [[ -t 1 ]]; then
    C_RESET="\033[0m"
    C_BOLD="\033[1m"
    C_DIM="\033[2m"
    C_CYAN="\033[1;36m"
    C_GREEN="\033[1;32m"
    C_YELLOW="\033[1;33m"
    C_RED="\033[1;31m"
    C_WHITE="\033[1;37m"
    C_BLUE="\033[1;34m"
else
    C_RESET="" C_BOLD="" C_DIM="" C_CYAN="" C_GREEN="" C_YELLOW=""
    C_RED="" C_WHITE="" C_BLUE=""
fi

# ─── Bilingual string table ───────────────────────────────────────────────────
# Usage: msg KEY
# All strings stored as arrays indexed by lang (0=en, 1=pt)
declare -A EN PT

EN[title]="AstroFind Installer"
PT[title]="Instalador do AstroFind"
EN[version_label]="Version"
PT[version_label]="Versão"
EN[lang_screen_header]="Language / Idioma"
EN[lang_screen_body]="Select installation language:"
PT[lang_screen_header]="Language / Idioma"
PT[lang_screen_body]="Selecione o idioma de instalação:"
EN[lang_opt_en]="  1) English  ← (detected)"
EN[lang_opt_en_other]="  1) English"
EN[lang_opt_pt]="  2) Português (Brasil)"
EN[lang_opt_pt_other]="  2) Português (Brasil)  ← (detectado)"
PT[lang_opt_en]="  1) English"
PT[lang_opt_en_other]="  1) English"
PT[lang_opt_pt]="  2) Português (Brasil)"
PT[lang_opt_pt_other]="  2) Português (Brasil)  ← (detectado)"
EN[lang_prompt]="Choice [1/2]: "
PT[lang_prompt]="Escolha [1/2]: "
EN[running_as_root]="WARNING: Running as root. Installation will affect system-wide paths."
PT[running_as_root]="AVISO: Executando como root. A instalação afetará caminhos do sistema."
EN[no_sudo_warn]="Neither sudo nor su is available. Package install may fail."
PT[no_sudo_warn]="Nem sudo nem su estão disponíveis. A instalação do pacote pode falhar."
EN[distro_detected]="Detected distribution"
PT[distro_detected]="Distribuição detectada"
EN[pkg_type]="Package type"
PT[pkg_type]="Tipo de pacote"
EN[pkg_manager]="Package manager"
PT[pkg_manager]="Gerenciador de pacotes"
EN[unknown_distro]="Unknown distribution — will build from source tarball."
PT[unknown_distro]="Distribuição desconhecida — compilação a partir do tarball fonte."
EN[priv_test_start]="Testing privilege escalation..."
PT[priv_test_start]="Testando escalada de privilégios..."
EN[priv_test_ok]="Privilege escalation available."
PT[priv_test_ok]="Escalada de privilégios disponível."
EN[priv_test_fail]="Could not obtain root privileges. Please run as root or configure sudo/su."
PT[priv_test_fail]="Não foi possível obter privilégios root. Execute como root ou configure sudo/su."
EN[confirm_install]="Proceed with installation?"
PT[confirm_install]="Prosseguir com a instalação?"
EN[prompt_yn_y]="[Y/n]: "
PT[prompt_yn_y]="[S/n]: "
EN[prompt_yn_n]="[y/N]: "
PT[prompt_yn_n]="[s/N]: "
EN[yes_char]="yY"
PT[yes_char]="sSyY"
EN[no_char]="nN"
PT[no_char]="nN"
EN[step_download]="Downloading package"
PT[step_download]="Baixando pacote"
EN[step_download_ok]="Download complete."
PT[step_download_ok]="Download concluído."
EN[step_download_fail]="Download failed. Check your internet connection and try again."
PT[step_download_fail]="Falha no download. Verifique a conexão com a internet e tente novamente."
EN[step_download_manual]="Manual download:"
PT[step_download_manual]="Download manual:"
EN[step_install]="Installing package"
PT[step_install]="Instalando pacote"
EN[step_install_ok]="Package installed successfully."
PT[step_install_ok]="Pacote instalado com sucesso."
EN[step_install_fail]="Package installation failed. Try manually:"
PT[step_install_fail]="Falha na instalação do pacote. Tente manualmente:"
EN[step_verify]="Verifying installation"
PT[step_verify]="Verificando instalação"
EN[step_verify_ok]="AstroFind binary found at ${BINARY}."
PT[step_verify_ok]="Binário AstroFind encontrado em ${BINARY}."
EN[step_verify_fail]="Binary not found at ${BINARY} after installation."
PT[step_verify_fail]="Binário não encontrado em ${BINARY} após a instalação."
EN[opt_header]="Optional Dependencies"
PT[opt_header]="Dependências Opcionais"
EN[opt_installed]="Status: already installed"
PT[opt_installed]="Status: já instalado"
EN[opt_not_installed]="Status: not installed"
PT[opt_not_installed]="Status: não instalado"
EN[opt_recommended]="★ Highly recommended"
PT[opt_recommended]="★ Altamente recomendado"
EN[opt_install_prompt]="Install"
PT[opt_install_prompt]="Instalar"
EN[opt_install_ok]="Installed successfully."
PT[opt_install_ok]="Instalado com sucesso."
EN[opt_install_fail]="Installation failed. You can install it later manually."
PT[opt_install_fail]="Falha na instalação. Você pode instalá-lo manualmente mais tarde."
EN[opt_skipped]="Skipped."
PT[opt_skipped]="Ignorado."
EN[libarchive_name]="libarchive (TAR/7Z/RAR support)"
PT[libarchive_name]="libarchive (suporte a TAR/7Z/RAR)"
EN[libarchive_desc]="Enables opening compressed archives directly (TAR.GZ, TAR.BZ2, XZ, 7Z, RAR)."
PT[libarchive_desc]="Permite abrir arquivos comprimidos diretamente (TAR.GZ, TAR.BZ2, XZ, 7Z, RAR)."
EN[keychain_name]="qt6keychain (encrypted API key storage)"
PT[keychain_name]="qt6keychain (armazenamento criptografado de chaves)"
EN[keychain_desc]="Stores astrometry.net and catalog API keys securely in the system keychain."
PT[keychain_desc]="Armazena chaves de API do astrometry.net e catálogos com segurança no chaveiro do sistema."
EN[astap_name]="ASTAP (offline plate solving)"
PT[astap_name]="ASTAP (plate solving offline)"
EN[astap_desc]="Enables plate solving without internet. Not in distro repos — visit:"
PT[astap_desc]="Permite plate solving sem internet. Não está nos repositórios — acesse:"
EN[astap_url]="https://www.hnsky.org/astap.htm"
PT[astap_url]="https://www.hnsky.org/astap.htm"
EN[desktop_header]="Desktop Integration"
PT[desktop_header]="Integração com o Desktop"
EN[menu_name]="Application menu entry"
PT[menu_name]="Entrada no menu de aplicativos"
EN[menu_desc]="Adds AstroFind to the system application menu (launcher, Dash, etc.)."
PT[menu_desc]="Adiciona AstroFind ao menu de aplicativos do sistema (launcher, Dash, etc.)."
EN[menu_installed]="Status: already present"
PT[menu_installed]="Status: já presente"
EN[menu_not_installed]="Status: not present"
PT[menu_not_installed]="Status: não presente"
EN[menu_prompt]="Add to application menu?"
PT[menu_prompt]="Adicionar ao menu de aplicativos?"
EN[menu_ok]="Menu entry added."
PT[menu_ok]="Entrada no menu adicionada."
EN[menu_fail]="Could not add menu entry."
PT[menu_fail]="Não foi possível adicionar entrada no menu."
EN[desktop_name]="Desktop shortcut"
PT[desktop_name]="Atalho na área de trabalho"
EN[desktop_desc]="Places an AstroFind icon on your Desktop."
PT[desktop_desc]="Coloca um ícone do AstroFind na sua área de trabalho."
EN[desktop_installed]="Status: already present"
PT[desktop_installed]="Status: já presente"
EN[desktop_not_installed]="Status: not present"
PT[desktop_not_installed]="Status: não presente"
EN[desktop_prompt]="Add shortcut to Desktop?"
PT[desktop_prompt]="Adicionar atalho na área de trabalho?"
EN[desktop_ok]="Desktop shortcut added."
PT[desktop_ok]="Atalho na área de trabalho adicionado."
EN[desktop_fail]="Could not add Desktop shortcut."
PT[desktop_fail]="Não foi possível adicionar atalho na área de trabalho."
EN[summary_header]="Installation Summary"
PT[summary_header]="Resumo da Instalação"
EN[summary_main]="AstroFind ${VERSION}"
PT[summary_main]="AstroFind ${VERSION}"
EN[summary_installed]="installed"
PT[summary_installed]="instalado"
EN[summary_skipped]="skipped"
PT[summary_skipped]="ignorado"
EN[summary_already]="already present"
PT[summary_already]="já presente"
EN[summary_failed]="FAILED"
PT[summary_failed]="FALHOU"
EN[done_msg]="Installation complete. Run:  AstroFind"
PT[done_msg]="Instalação concluída. Execute:  AstroFind"
EN[abort_msg]="Installation aborted."
PT[abort_msg]="Instalação cancelada."
EN[cleanup_msg]="Cleaning up temporary files..."
PT[cleanup_msg]="Removendo arquivos temporários..."
EN[no_downloader]="Neither curl nor wget found. Install one and retry."
PT[no_downloader]="Nem curl nem wget encontrado. Instale um deles e tente novamente."
EN[arch_build_info]="Building from PKGBUILD (Arch/Manjaro)..."
PT[arch_build_info]="Compilando via PKGBUILD (Arch/Manjaro)..."
EN[source_build_info]="Building from source (unknown distro)..."
PT[source_build_info]="Compilando a partir do fonte (distro desconhecida)..."
EN[source_deps_warn]="Ensure Qt6, CMake, cfitsio, Eigen3, fftw3 are installed before building."
PT[source_deps_warn]="Certifique-se de que Qt6, CMake, cfitsio, Eigen3, fftw3 estão instalados antes de compilar."
EN[help_text]="Usage: install.sh [OPTIONS]
Options:
  --lang pt    Force Portuguese (Brazil)
  --lang en    Force English
  --help       Show this help message

Interactive installer for AstroFind ${VERSION}.
Detects your Linux distribution and installs the appropriate package."
PT[help_text]="Uso: install.sh [OPÇÕES]
Opções:
  --lang pt    Forçar Português (Brasil)
  --lang en    Forçar Inglês
  --help       Exibir esta mensagem de ajuda

Instalador interativo do AstroFind ${VERSION}.
Detecta sua distribuição Linux e instala o pacote apropriado."

# ─── Language selection ────────────────────────────────────────────────────────
LANG_CODE="en"

detect_locale() {
    local sys_locale="${LANG:-${LC_ALL:-${LC_MESSAGES:-}}}"
    if [[ "$sys_locale" == pt_BR* ]] || [[ "$sys_locale" == pt_* ]]; then
        echo "pt"
    else
        echo "en"
    fi
}

msg() {
    local key="$1"
    if [[ "$LANG_CODE" == "pt" ]]; then
        echo -e "${PT[$key]:-${EN[$key]}}"
    else
        echo -e "${EN[$key]}"
    fi
}

# ─── Terminal helpers ──────────────────────────────────────────────────────────
TERM_WIDTH=80

get_term_width() {
    if command -v tput &>/dev/null; then
        local w
        w=$(tput cols 2>/dev/null || echo 80)
        [[ "$w" -gt 30 ]] && TERM_WIDTH="$w" || TERM_WIDTH=80
    fi
}

center_text() {
    local text="$1"
    # Strip ANSI codes for length calculation
    local plain
    plain=$(echo -e "$text" | sed 's/\x1b\[[0-9;]*m//g')
    local len=${#plain}
    local pad=$(( (TERM_WIDTH - len) / 2 ))
    [[ $pad -lt 0 ]] && pad=0
    printf "%${pad}s%b\n" "" "$text"
}

hr() {
    local char="${1:─}"
    local color="${2:-$C_CYAN}"
    printf "${color}"
    printf '%*s' "$TERM_WIDTH" '' | tr ' ' "${char}"
    printf "${C_RESET}\n"
}

box_line() {
    # box_line "text" width color
    local text="$1"
    local width="${2:-$((TERM_WIDTH - 4))}"
    local color="${3:-$C_CYAN}"
    local plain
    plain=$(echo -e "$text" | sed 's/\x1b\[[0-9;]*m//g')
    local content_len=${#plain}
    local pad=$(( width - content_len - 2 ))
    [[ $pad -lt 0 ]] && pad=0
    printf "${color}│${C_RESET}  %b%*s${color}│${C_RESET}\n" "$text" "$pad" ""
}

box_top() {
    local width="${1:-$((TERM_WIDTH - 4))}"
    local color="${2:-$C_CYAN}"
    local indent="${3:-2}"
    printf "%${indent}s${color}┌" ""
    printf '─%.0s' $(seq 1 $((width - 2)))
    printf "┐${C_RESET}\n"
}

box_bottom() {
    local width="${1:-$((TERM_WIDTH - 4))}"
    local color="${2:-$C_CYAN}"
    local indent="${3:-2}"
    printf "%${indent}s${color}└" ""
    printf '─%.0s' $(seq 1 $((width - 2)))
    printf "┘${C_RESET}\n"
}

box_separator() {
    local width="${1:-$((TERM_WIDTH - 4))}"
    local color="${2:-$C_CYAN}"
    local indent="${3:-2}"
    printf "%${indent}s${color}├" ""
    printf '─%.0s' $(seq 1 $((width - 2)))
    printf "┤${C_RESET}\n"
}

print_banner() {
    clear
    echo
    center_text "${C_CYAN}${C_BOLD}  ██████╗ ███████╗████████╗██████╗  ██████╗ ███████╗██╗███╗  ██╗██████╗  ${C_RESET}"
    center_text "${C_CYAN}${C_BOLD} ██╔══██╗██╔════╝╚══██╔══╝██╔══██╗██╔═══██╗██╔════╝██║████╗ ██║██╔══██╗ ${C_RESET}"
    center_text "${C_CYAN}${C_BOLD} ███████║███████╗   ██║   ██████╔╝██║   ██║█████╗  ██║██╔██╗██║██║  ██║ ${C_RESET}"
    center_text "${C_CYAN}${C_BOLD} ██╔══██║╚════██║   ██║   ██╔══██╗██║   ██║██╔══╝  ██║██║╚████║██║  ██║ ${C_RESET}"
    center_text "${C_CYAN}${C_BOLD} ██║  ██║███████║   ██║   ██║  ██║╚██████╔╝██║     ██║██║ ╚███║██████╔╝ ${C_RESET}"
    center_text "${C_DIM}${C_CYAN} ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚══╝╚═════╝  ${C_RESET}"
    echo
    center_text "${C_WHITE}${C_BOLD}Asteroid Detection & Astrometry Software${C_RESET}"
    center_text "${C_DIM}$(msg version_label) ${VERSION}${C_RESET}"
    echo
    hr "─"
    echo
}

section_header() {
    local title="$1"
    local step="${2:-}"
    echo
    if [[ -n "$step" ]]; then
        center_text "${C_CYAN}${C_BOLD}[ ${step} ]  ${title}${C_RESET}"
    else
        center_text "${C_CYAN}${C_BOLD}◈  ${title}${C_RESET}"
    fi
    echo
}

print_ok()   { echo -e "  ${C_GREEN}✓${C_RESET}  $*"; }
print_warn() { echo -e "  ${C_YELLOW}⚠${C_RESET}  $*"; }
print_err()  { echo -e "  ${C_RED}✗${C_RESET}  $*" >&2; }
print_info() { echo -e "  ${C_BLUE}ℹ${C_RESET}  $*"; }
print_step() { echo -e "  ${C_WHITE}${C_BOLD}▶${C_RESET}  $*"; }

# ─── Cleanup trap ─────────────────────────────────────────────────────────────
cleanup() {
    if [[ -n "$TMPDIR_WORK" && -d "$TMPDIR_WORK" ]]; then
        echo -e "\n  ${C_DIM}$(msg cleanup_msg)${C_RESET}"
        rm -rf "$TMPDIR_WORK"
    fi
}
trap cleanup EXIT INT TERM

# ─── Argument parsing ──────────────────────────────────────────────────────────
parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --lang)
                shift
                case "${1:-}" in
                    pt|pt_BR|pt-BR) LANG_FORCED="pt" ;;
                    en|en_US|en-US) LANG_FORCED="en" ;;
                    *) echo "Unknown language: ${1:-}. Use 'en' or 'pt'."; exit 1 ;;
                esac
                ;;
            --help|-h)
                echo "${EN[help_text]}"
                echo
                echo "---"
                echo
                echo "${PT[help_text]}"
                exit 0
                ;;
            *)
                echo "Unknown option: $1. Use --help for usage."
                exit 1
                ;;
        esac
        shift
    done
}

# ─── Language selection screen ────────────────────────────────────────────────
select_language() {
    local detected
    detected=$(detect_locale)

    if [[ -n "$LANG_FORCED" ]]; then
        LANG_CODE="$LANG_FORCED"
        return
    fi

    # Show lang screen using raw English/Portuguese hybrid (always shown)
    clear
    echo
    center_text "${C_CYAN}${C_BOLD}◈  Language / Idioma${C_RESET}"
    echo
    echo -e "  Select installation language / Selecione o idioma de instalação:"
    echo

    if [[ "$detected" == "en" ]]; then
        echo -e "  ${C_WHITE}${C_BOLD}1) English${C_RESET}  ${C_DIM}← detected / detectado${C_RESET}"
        echo -e "  ${C_DIM}2) Português (Brasil)${C_RESET}"
    else
        echo -e "  ${C_DIM}1) English${C_RESET}"
        echo -e "  ${C_WHITE}${C_BOLD}2) Português (Brasil)${C_RESET}  ${C_DIM}← detectado / detected${C_RESET}"
    fi

    echo
    local default_num
    [[ "$detected" == "pt" ]] && default_num=2 || default_num=1

    while true; do
        printf "  Choice / Escolha [1/2] (default %d): " "$default_num"
        local choice
        IFS= read -r choice
        choice="${choice:-$default_num}"
        case "$choice" in
            1) LANG_CODE="en"; break ;;
            2) LANG_CODE="pt"; break ;;
            *) echo -e "  ${C_YELLOW}Please enter 1 or 2 / Digite 1 ou 2.${C_RESET}" ;;
        esac
    done
}

# ─── Distro detection ─────────────────────────────────────────────────────────
DISTRO_ID=""
DISTRO_LIKE=""
PKG_TYPE=""       # rpm | deb | arch | source
PKG_MANAGER=""    # dnf | zypper | apt | makepkg | cmake

detect_distro() {
    if [[ -f /etc/os-release ]]; then
        # shellcheck disable=SC1091
        source /etc/os-release
        DISTRO_ID="${ID:-unknown}"
        DISTRO_LIKE="${ID_LIKE:-}"
    else
        DISTRO_ID="unknown"
        DISTRO_LIKE=""
    fi

    local combined="${DISTRO_ID} ${DISTRO_LIKE}"

    if echo "$combined" | grep -qiE '\bfedora\b|\brhel\b|\brocky\b|\balma\b|\bcentos\b'; then
        PKG_TYPE="rpm"
        PKG_MANAGER="dnf"
    elif echo "$combined" | grep -qiE '\bopensuse\b|\bsuse\b'; then
        PKG_TYPE="rpm"
        PKG_MANAGER="zypper"
    elif echo "$combined" | grep -qiE '\bdebian\b|\bubuntu\b|\bmint\b|\bzorin\b|\bpop\b|\belementary\b|\bkali\b|\bparrot\b'; then
        PKG_TYPE="deb"
        PKG_MANAGER="apt"
    elif echo "$combined" | grep -qiE '\barch\b|\bmanjaro\b|\bendeavour\b|\bgaruda\b'; then
        PKG_TYPE="arch"
        PKG_MANAGER="makepkg"
    else
        PKG_TYPE="source"
        PKG_MANAGER="cmake"
    fi
}

# ─── Privilege helpers ────────────────────────────────────────────────────────
IS_ROOT=false
PRIV_CMD=""

check_privileges() {
    if [[ "$EUID" -eq 0 ]]; then
        IS_ROOT=true
        PRIV_CMD=""
        return
    fi

    if command -v sudo &>/dev/null; then
        PRIV_CMD="sudo"
    elif command -v su &>/dev/null; then
        PRIV_CMD="su_wrap"
    else
        PRIV_CMD=""
    fi
}

run_priv() {
    if [[ "$IS_ROOT" == true ]]; then
        "$@"
    elif [[ "$PRIV_CMD" == "sudo" ]]; then
        sudo "$@"
    elif [[ "$PRIV_CMD" == "su_wrap" ]]; then
        su -c "$(printf '%q ' "$@")"
    else
        print_err "$(msg no_sudo_warn)"
        "$@"
    fi
}

test_privilege() {
    print_step "$(msg priv_test_start)"
    if [[ "$IS_ROOT" == true ]]; then
        print_ok "$(msg priv_test_ok)"
        return 0
    fi
    if [[ "$PRIV_CMD" == "sudo" ]]; then
        if sudo -n true 2>/dev/null || sudo -v 2>/dev/null; then
            print_ok "$(msg priv_test_ok)"
            return 0
        fi
    elif [[ "$PRIV_CMD" == "su_wrap" ]]; then
        if su -c "true" 2>/dev/null; then
            print_ok "$(msg priv_test_ok)"
            return 0
        fi
    fi
    print_err "$(msg priv_test_fail)"
    return 1
}

# ─── Download helper ──────────────────────────────────────────────────────────
download_file() {
    local url="$1"
    local dest="$2"

    if command -v curl &>/dev/null; then
        curl -# -fL -o "$dest" "$url"
    elif command -v wget &>/dev/null; then
        wget --show-progress -q -O "$dest" "$url"
    else
        print_err "$(msg no_downloader)"
        return 1
    fi
}

# ─── Prompt helpers ───────────────────────────────────────────────────────────
prompt_yn() {
    # prompt_yn "question" default_yes -> returns 0 for yes, 1 for no
    local question="$1"
    local default_yes="${2:-true}"
    local prompt_str

    if [[ "$default_yes" == "true" ]]; then
        prompt_str="$(msg prompt_yn_y)"
    else
        prompt_str="$(msg prompt_yn_n)"
    fi

    while true; do
        printf "  %b %b" "${C_WHITE}${C_BOLD}${question}${C_RESET}" "${prompt_str}"
        local answer
        IFS= read -r answer
        answer="${answer:-}"

        if [[ -z "$answer" ]]; then
            [[ "$default_yes" == "true" ]] && return 0 || return 1
        fi

        local yes_chars
        yes_chars=$(msg yes_char)
        local no_chars
        no_chars=$(msg no_char)

        if [[ "$answer" =~ ^[${yes_chars}]$ ]]; then
            return 0
        elif [[ "$answer" =~ ^[${no_chars}]$ ]]; then
            return 1
        else
            echo -e "  ${C_YELLOW}Invalid input. Try again.${C_RESET}"
        fi
    done
}

# ─── Optional dependency helper ───────────────────────────────────────────────
check_lib() {
    local pattern="$1"
    ldconfig -p 2>/dev/null | grep -q "$pattern"
}

install_optional() {
    # install_optional "pkg_name_display" "pkg_desc" "pkg_recommended" \
    #                  "check_cmd" "pkg_fedora" "pkg_deb" "pkg_arch"
    local name="$1"
    local desc="$2"
    local recommended="$3"
    local check_cmd="$4"
    local pkg_fedora="$5"
    local pkg_deb="$6"
    local pkg_arch="$7"

    local pkg_to_install
    case "$PKG_MANAGER" in
        dnf)     pkg_to_install="$pkg_fedora" ;;
        zypper)  pkg_to_install="$pkg_fedora" ;;
        apt)     pkg_to_install="$pkg_deb" ;;
        makepkg) pkg_to_install="$pkg_arch" ;;
        *)       pkg_to_install="" ;;
    esac

    local is_installed=false
    local status_msg
    if eval "$check_cmd" &>/dev/null; then
        is_installed=true
        status_msg="${C_GREEN}$(msg opt_installed)${C_RESET}"
    else
        status_msg="${C_DIM}$(msg opt_not_installed)${C_RESET}"
    fi

    local box_w=$(( TERM_WIDTH - 8 ))
    echo
    box_top $box_w "$C_CYAN" 4
    # Print box content lines
    printf "    ${C_CYAN}│${C_RESET}  ${C_WHITE}${C_BOLD}%-$((box_w - 4))s${C_CYAN}│${C_RESET}\n" "◆ ${name}"
    printf "    ${C_CYAN}│${C_RESET}  ${C_DIM}%-$((box_w - 4))s${C_CYAN}│${C_RESET}\n" "  ${desc}"
    if [[ "$recommended" == "true" ]]; then
        printf "    ${C_CYAN}│${C_RESET}  ${C_YELLOW}%-$((box_w - 4))s${C_CYAN}│${C_RESET}\n" "  $(msg opt_recommended)"
    fi
    printf "    ${C_CYAN}│${C_RESET}  "
    echo -e "${status_msg}"
    box_bottom $box_w "$C_CYAN" 4

    if [[ "$is_installed" == true ]]; then
        print_ok "${name}: ${C_DIM}$(msg opt_installed)${C_RESET}"
        return 0
    fi

    if [[ -z "$pkg_to_install" ]]; then
        print_warn "No package available for your distro/package manager. Skipping."
        return 1
    fi

    if prompt_yn "$(msg opt_install_prompt) ${pkg_to_install}?" true; then
        local install_cmd
        case "$PKG_MANAGER" in
            dnf)    install_cmd="dnf install -y ${pkg_to_install}" ;;
            zypper) install_cmd="zypper install -y ${pkg_to_install}" ;;
            apt)    install_cmd="apt install -y ${pkg_to_install}" ;;
            makepkg)
                if [[ "$pkg_arch" == *"AUR"* ]]; then
                    echo -e "  ${C_DIM}(AUR package — install manually with: yay -S ${pkg_to_install})${C_RESET}"
                    return 1
                fi
                install_cmd="pacman -S --noconfirm ${pkg_to_install}"
                ;;
        esac

        if run_priv bash -c "$install_cmd"; then
            print_ok "$(msg opt_install_ok)"
            return 0
        else
            print_err "$(msg opt_install_fail)"
            return 1
        fi
    else
        print_info "$(msg opt_skipped)"
        return 1
    fi
}

# ─── Desktop integration helpers ─────────────────────────────────────────────
get_desktop_file_src() {
    # Try to find the .desktop file relative to the installer, or use embedded
    local script_dir
    script_dir="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
    local candidates=(
        "${script_dir}/../astrofind.desktop"
        "/usr/share/applications/astrofind.desktop"
        "/tmp/astrofind.desktop"
    )
    for f in "${candidates[@]}"; do
        [[ -f "$f" ]] && echo "$f" && return
    done
    # Generate a minimal one on the fly
    cat > "${TMPDIR_WORK}/astrofind.desktop" <<'DESK'
[Desktop Entry]
Name=AstroFind
GenericName=Asteroid Detection Software
Comment=Detect and measure moving objects in astronomical FITS images
Exec=AstroFind %F
Icon=astrofind
Type=Application
Categories=Science;Astronomy;Education;
MimeType=image/fits;application/fits;
Keywords=astronomy;asteroid;FITS;astrometry;photometry;moving object;
StartupNotify=true
StartupWMClass=AstroFind
DESK
    echo "${TMPDIR_WORK}/astrofind.desktop"
}

install_menu_entry() {
    local desktop_src
    desktop_src=$(get_desktop_file_src)

    if [[ "$IS_ROOT" == true ]]; then
        local dest_dir="/usr/share/applications"
        run_priv install -Dm644 "$desktop_src" "${dest_dir}/astrofind.desktop" && \
            print_ok "$(msg menu_ok)" && return 0
    else
        local dest_dir="${HOME}/.local/share/applications"
        mkdir -p "$dest_dir"
        install -Dm644 "$desktop_src" "${dest_dir}/astrofind.desktop" && \
            print_ok "$(msg menu_ok)" && return 0
    fi
    print_err "$(msg menu_fail)"
    return 1
}

install_desktop_shortcut() {
    local desktop_src
    desktop_src=$(get_desktop_file_src)
    local desk_dir="${HOME}/Desktop"
    [[ -d "${HOME}/Área de Trabalho" ]] && desk_dir="${HOME}/Área de Trabalho"
    [[ -d "${HOME}/Escritorio" ]]       && desk_dir="${HOME}/Escritorio"

    if [[ ! -d "$desk_dir" ]]; then
        desk_dir="${HOME}"
    fi

    if install -Dm755 "$desktop_src" "${desk_dir}/astrofind.desktop"; then
        print_ok "$(msg desktop_ok)"
        return 0
    fi
    print_err "$(msg desktop_fail)"
    return 1
}

# ─── Main package install: RPM ────────────────────────────────────────────────
install_rpm() {
    local pkg_file="${TMPDIR_WORK}/${RPM_FILE}"
    local url="${BASE_URL}/${RPM_FILE}"

    print_step "$(msg step_download): ${RPM_FILE}"
    if ! download_file "$url" "$pkg_file"; then
        print_err "$(msg step_download_fail)"
        print_info "$(msg step_download_manual) ${url}"
        return 1
    fi
    print_ok "$(msg step_download_ok)"

    print_step "$(msg step_install)"
    local install_cmd
    [[ "$PKG_MANAGER" == "zypper" ]] && \
        install_cmd="zypper install -y ${pkg_file}" || \
        install_cmd="dnf install -y ${pkg_file}"

    if run_priv bash -c "$install_cmd"; then
        print_ok "$(msg step_install_ok)"
        return 0
    else
        print_err "$(msg step_install_fail)"
        print_info "  ${install_cmd}"
        return 1
    fi
}

# ─── Main package install: DEB ────────────────────────────────────────────────
install_deb() {
    local pkg_file="${TMPDIR_WORK}/${DEB_FILE}"
    local url="${BASE_URL}/${DEB_FILE}"

    print_step "$(msg step_download): ${DEB_FILE}"
    if ! download_file "$url" "$pkg_file"; then
        print_err "$(msg step_download_fail)"
        print_info "$(msg step_download_manual) ${url}"
        return 1
    fi
    print_ok "$(msg step_download_ok)"

    print_step "$(msg step_install)"
    if run_priv bash -c "dpkg -i '${pkg_file}' && apt install -f -y"; then
        print_ok "$(msg step_install_ok)"
        return 0
    else
        print_err "$(msg step_install_fail)"
        print_info "  dpkg -i '${pkg_file}'"
        return 1
    fi
}

# ─── Main package install: Arch PKGBUILD ──────────────────────────────────────
install_arch() {
    local pkg_file="${TMPDIR_WORK}/${ARCH_FILE}"
    local url="${BASE_URL}/${ARCH_FILE}"

    print_step "$(msg step_download): ${ARCH_FILE}"
    if ! download_file "$url" "$pkg_file"; then
        print_err "$(msg step_download_fail)"
        print_info "$(msg step_download_manual) ${url}"
        return 1
    fi
    print_ok "$(msg step_download_ok)"

    print_step "$(msg arch_build_info)"
    local build_dir="${TMPDIR_WORK}/arch_build"
    mkdir -p "$build_dir"

    if ! tar -xzf "$pkg_file" -C "$build_dir"; then
        print_err "Failed to extract PKGBUILD tarball."
        return 1
    fi

    # Find the PKGBUILD
    local pkgbuild_dir
    pkgbuild_dir=$(find "$build_dir" -name "PKGBUILD" -exec dirname {} \; | head -1)

    if [[ -z "$pkgbuild_dir" ]]; then
        print_err "PKGBUILD not found in archive."
        return 1
    fi

    # makepkg must NOT run as root
    if [[ "$IS_ROOT" == true ]]; then
        print_warn "makepkg cannot run as root. Creating build user or running via fakeroot..."
        if command -v fakeroot &>/dev/null; then
            (cd "$pkgbuild_dir" && fakeroot makepkg -si --noconfirm)
        else
            print_err "Please run the installer as a non-root user for Arch builds."
            return 1
        fi
    else
        (cd "$pkgbuild_dir" && makepkg -si --noconfirm)
    fi

    local exit_code=$?
    if [[ $exit_code -eq 0 ]]; then
        print_ok "$(msg step_install_ok)"
        return 0
    else
        print_err "$(msg step_install_fail)"
        print_info "  cd '${pkgbuild_dir}' && makepkg -si"
        return 1
    fi
}

# ─── Main package install: Source ─────────────────────────────────────────────
install_source() {
    local src_file="${TMPDIR_WORK}/astrofind-${VERSION}-source.tar.gz"
    local url="${BASE_URL}/astrofind-${VERSION}-source.tar.gz"

    print_warn "$(msg source_build_info)"
    print_warn "$(msg source_deps_warn)"
    echo

    print_step "$(msg step_download): source tarball"
    if ! download_file "$url" "$src_file"; then
        print_err "$(msg step_download_fail)"
        print_info "$(msg step_download_manual) ${url}"
        return 1
    fi
    print_ok "$(msg step_download_ok)"

    print_step "Extracting and building..."
    local build_dir="${TMPDIR_WORK}/source_build"
    mkdir -p "$build_dir"
    tar -xzf "$src_file" -C "$build_dir" || { print_err "Extraction failed."; return 1; }

    local src_dir
    src_dir=$(find "$build_dir" -maxdepth 2 -name "CMakeLists.txt" -exec dirname {} \; | head -1)

    if [[ -z "$src_dir" ]]; then
        print_err "CMakeLists.txt not found in source archive."
        return 1
    fi

    local cmake_build="${build_dir}/cmake_build"
    mkdir -p "$cmake_build"

    if ! cmake -S "$src_dir" -B "$cmake_build" -DCMAKE_BUILD_TYPE=Release \
               -DCMAKE_INSTALL_PREFIX=/usr; then
        print_err "CMake configuration failed."
        return 1
    fi

    local nproc
    nproc=$(nproc 2>/dev/null || echo 2)

    if ! cmake --build "$cmake_build" --parallel "$nproc"; then
        print_err "Build failed."
        return 1
    fi

    if run_priv cmake --install "$cmake_build"; then
        print_ok "$(msg step_install_ok)"
        return 0
    else
        print_err "$(msg step_install_fail)"
        print_info "  cmake --install '${cmake_build}'"
        return 1
    fi
}

# ─── Summary tracking ─────────────────────────────────────────────────────────
declare -A SUMMARY_STATUS   # key -> installed|skipped|failed|already

add_summary() {
    local key="$1"
    local status="$2"
    SUMMARY_STATUS["$key"]="$status"
}

print_summary() {
    local box_w=$(( TERM_WIDTH - 8 ))
    echo
    section_header "$(msg summary_header)"

    box_top $box_w "$C_CYAN" 4
    printf "    ${C_CYAN}│${C_RESET}  ${C_WHITE}${C_BOLD}%-$((box_w - 4))s${C_CYAN}│${C_RESET}\n" "$(msg summary_header)"
    box_separator $box_w "$C_CYAN" 4

    for key in "${!SUMMARY_STATUS[@]}"; do
        local status="${SUMMARY_STATUS[$key]}"
        local icon color label

        case "$status" in
            installed) icon="✓"; color="$C_GREEN";  label="$(msg summary_installed)" ;;
            skipped)   icon="✗"; color="$C_DIM";    label="$(msg summary_skipped)" ;;
            already)   icon="✓"; color="$C_GREEN";  label="$(msg summary_already)" ;;
            failed)    icon="✗"; color="$C_RED";    label="$(msg summary_failed)" ;;
            *)         icon="?"; color="$C_YELLOW"; label="$status" ;;
        esac

        printf "    ${C_CYAN}│${C_RESET}  ${color}%s${C_RESET}  %-30s  ${C_DIM}%s${C_RESET}%*s${C_CYAN}│${C_RESET}\n" \
            "$icon" "$key" "$label" \
            $(( box_w - 4 - 1 - 2 - 30 - 2 - ${#label} - 1 )) ""
    done

    box_bottom $box_w "$C_CYAN" 4
}

# ─── Main flow ────────────────────────────────────────────────────────────────
main() {
    parse_args "$@"
    get_term_width

    # Language selection (always shown unless --lang forced)
    select_language

    # Now we have LANG_CODE, show the banner
    print_banner

    # Root warning
    check_privileges
    if [[ "$IS_ROOT" == true ]]; then
        print_warn "$(msg running_as_root)"
        echo
    fi

    # Distro detection
    detect_distro

    section_header "$(msg distro_detected)" "1/7"

    local distro_display="${DISTRO_ID}"
    [[ -n "$DISTRO_LIKE" ]] && distro_display="${DISTRO_ID} (like: ${DISTRO_LIKE})"

    echo -e "  ${C_DIM}$(msg distro_detected):${C_RESET}  ${C_WHITE}${C_BOLD}${distro_display}${C_RESET}"
    echo -e "  ${C_DIM}$(msg pkg_type):${C_RESET}          ${C_WHITE}${C_BOLD}${PKG_TYPE}${C_RESET}"
    echo -e "  ${C_DIM}$(msg pkg_manager):${C_RESET}       ${C_WHITE}${C_BOLD}${PKG_MANAGER}${C_RESET}"

    if [[ "$PKG_TYPE" == "source" ]]; then
        echo
        print_warn "$(msg unknown_distro)"
    fi

    # Privilege test
    echo
    section_header "Privileges" "2/7"
    if ! test_privilege; then
        echo
        exit 1
    fi

    # Confirm
    echo
    section_header "$(msg confirm_install)" "3/7"
    echo
    echo -e "  ${C_DIM}Package: ${PKG_TYPE} via ${PKG_MANAGER}${C_RESET}"
    echo -e "  ${C_DIM}URL: ${BASE_URL}${C_RESET}"
    echo

    if ! prompt_yn "$(msg confirm_install)" true; then
        echo
        print_warn "$(msg abort_msg)"
        exit 0
    fi

    # Create temp dir
    TMPDIR_WORK=$(mktemp -d /tmp/astrofind_install.XXXXXX)

    # Download & Install
    echo
    section_header "$(msg step_download) & $(msg step_install)" "4/7"
    echo

    local install_ok=false
    case "$PKG_TYPE" in
        rpm)    install_rpm    && install_ok=true ;;
        deb)    install_deb    && install_ok=true ;;
        arch)   install_arch   && install_ok=true ;;
        source) install_source && install_ok=true ;;
    esac

    # Verify
    echo
    section_header "$(msg step_verify)" "5/7"
    echo

    if [[ "$install_ok" == true ]] && command -v AstroFind &>/dev/null; then
        print_ok "$(msg step_verify_ok)"
        add_summary "AstroFind ${VERSION}" "installed"
    elif [[ -x "$BINARY" ]]; then
        print_ok "$(msg step_verify_ok)"
        add_summary "AstroFind ${VERSION}" "installed"
    else
        print_err "$(msg step_verify_fail)"
        add_summary "AstroFind ${VERSION}" "failed"
    fi

    # Optional dependencies
    echo
    section_header "$(msg opt_header)" "6/7"

    # 1. libarchive
    local libarchive_check='ldconfig -p | grep -q libarchive'
    local la_pkg_fedora="libarchive"
    local la_pkg_deb="libarchive13"
    local la_pkg_arch="libarchive"

    if install_optional \
        "$(msg libarchive_name)" \
        "$(msg libarchive_desc)" \
        "true" \
        "$libarchive_check" \
        "$la_pkg_fedora" \
        "$la_pkg_deb" \
        "$la_pkg_arch"; then
        if eval "$libarchive_check" &>/dev/null; then
            add_summary "libarchive" "already"
        else
            add_summary "libarchive" "installed"
        fi
    else
        if eval "$libarchive_check" &>/dev/null; then
            add_summary "libarchive" "already"
        else
            add_summary "libarchive" "skipped"
        fi
    fi

    # 2. qtkeychain
    local keychain_check='ldconfig -p | grep -q qt6keychain'
    local kc_pkg_fedora="qtkeychain-qt6"
    local kc_pkg_deb="libqt6keychain1"
    local kc_pkg_arch="qtkeychain-qt6 (AUR)"

    if install_optional \
        "$(msg keychain_name)" \
        "$(msg keychain_desc)" \
        "true" \
        "$keychain_check" \
        "$kc_pkg_fedora" \
        "$kc_pkg_deb" \
        "$kc_pkg_arch"; then
        if eval "$keychain_check" &>/dev/null; then
            add_summary "qtkeychain" "already"
        else
            add_summary "qtkeychain" "installed"
        fi
    else
        if eval "$keychain_check" &>/dev/null; then
            add_summary "qtkeychain" "already"
        else
            add_summary "qtkeychain" "skipped"
        fi
    fi

    # 3. ASTAP (inform only)
    echo
    local box_w=$(( TERM_WIDTH - 8 ))
    box_top $box_w "$C_YELLOW" 4
    printf "    ${C_YELLOW}│${C_RESET}  ${C_WHITE}${C_BOLD}%-$((box_w - 4))s${C_YELLOW}│${C_RESET}\n" "◆ $(msg astap_name)"
    printf "    ${C_YELLOW}│${C_RESET}  ${C_DIM}  %-$((box_w - 6))s${C_YELLOW}│${C_RESET}\n" "$(msg astap_desc)"
    printf "    ${C_YELLOW}│${C_RESET}  ${C_BLUE}  %-$((box_w - 6))s${C_YELLOW}│${C_RESET}\n" "$(msg astap_url)"
    printf "    ${C_YELLOW}│${C_RESET}  ${C_YELLOW}  %-$((box_w - 6))s${C_YELLOW}│${C_RESET}\n" "(not installed by this script)"
    box_bottom $box_w "$C_YELLOW" 4

    if command -v astap &>/dev/null; then
        add_summary "ASTAP" "already"
    else
        add_summary "ASTAP" "skipped"
    fi

    # Desktop integration
    echo
    section_header "$(msg desktop_header)" "7/7"

    # Menu entry
    local menu_present=false
    local menu_dest_system="/usr/share/applications/astrofind.desktop"
    local menu_dest_user="${HOME}/.local/share/applications/astrofind.desktop"
    if [[ -f "$menu_dest_system" || -f "$menu_dest_user" ]]; then
        menu_present=true
    fi

    echo
    local box_w2=$(( TERM_WIDTH - 8 ))
    box_top $box_w2 "$C_CYAN" 4
    printf "    ${C_CYAN}│${C_RESET}  ${C_WHITE}${C_BOLD}%-$((box_w2 - 4))s${C_CYAN}│${C_RESET}\n" "◆ $(msg menu_name)"
    printf "    ${C_CYAN}│${C_RESET}  ${C_DIM}  %-$((box_w2 - 6))s${C_CYAN}│${C_RESET}\n" "$(msg menu_desc)"
    if [[ "$menu_present" == true ]]; then
        printf "    ${C_CYAN}│${C_RESET}  ${C_GREEN}  %-$((box_w2 - 6))s${C_CYAN}│${C_RESET}\n" "$(msg menu_installed)"
    else
        printf "    ${C_CYAN}│${C_RESET}  ${C_DIM}  %-$((box_w2 - 6))s${C_CYAN}│${C_RESET}\n" "$(msg menu_not_installed)"
    fi
    box_bottom $box_w2 "$C_CYAN" 4
    echo

    if [[ "$menu_present" == true ]]; then
        print_ok "$(msg menu_name): $(msg menu_installed)"
        add_summary "$(msg menu_name)" "already"
    elif prompt_yn "$(msg menu_prompt)" true; then
        if install_menu_entry; then
            add_summary "$(msg menu_name)" "installed"
        else
            add_summary "$(msg menu_name)" "failed"
        fi
    else
        print_info "$(msg opt_skipped)"
        add_summary "$(msg menu_name)" "skipped"
    fi

    # Desktop shortcut
    local desk_dir="${HOME}/Desktop"
    [[ -d "${HOME}/Área de Trabalho" ]] && desk_dir="${HOME}/Área de Trabalho"
    local desktop_present=false
    [[ -f "${desk_dir}/astrofind.desktop" ]] && desktop_present=true

    echo
    box_top $box_w2 "$C_CYAN" 4
    printf "    ${C_CYAN}│${C_RESET}  ${C_WHITE}${C_BOLD}%-$((box_w2 - 4))s${C_CYAN}│${C_RESET}\n" "◆ $(msg desktop_name)"
    printf "    ${C_CYAN}│${C_RESET}  ${C_DIM}  %-$((box_w2 - 6))s${C_CYAN}│${C_RESET}\n" "$(msg desktop_desc)"
    if [[ "$desktop_present" == true ]]; then
        printf "    ${C_CYAN}│${C_RESET}  ${C_GREEN}  %-$((box_w2 - 6))s${C_CYAN}│${C_RESET}\n" "$(msg desktop_installed)"
    else
        printf "    ${C_CYAN}│${C_RESET}  ${C_DIM}  %-$((box_w2 - 6))s${C_CYAN}│${C_RESET}\n" "$(msg desktop_not_installed)"
    fi
    box_bottom $box_w2 "$C_CYAN" 4
    echo

    if [[ "$desktop_present" == true ]]; then
        print_ok "$(msg desktop_name): $(msg desktop_installed)"
        add_summary "$(msg desktop_name)" "already"
    elif prompt_yn "$(msg desktop_prompt)" false; then
        if install_desktop_shortcut; then
            add_summary "$(msg desktop_name)" "installed"
        else
            add_summary "$(msg desktop_name)" "failed"
        fi
    else
        print_info "$(msg opt_skipped)"
        add_summary "$(msg desktop_name)" "skipped"
    fi

    # Summary
    print_summary

    # Done
    echo
    hr "─"
    echo
    center_text "${C_GREEN}${C_BOLD}$(msg done_msg)${C_RESET}"
    echo
}

main "$@"
