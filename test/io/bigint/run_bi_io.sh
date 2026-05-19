#!/bin/bash
################################################################################
# run_bi_io.sh - Bigint I/O test executor (Hybrid Interactive CLI)
# Self-contained: Handles its own build + runner selection + execution
# Prompts user to:
#   1) Select individual runners via cursor keys OR alphabet hotkeys
#   2) Enter random case count
# Builds CMake project, executes selected runners sequentially
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_fail() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_section() {
    echo -e "\n${YELLOW}=== $1 ===${NC}\n"
}

# ============================================================================
# Self-contained build
# ============================================================================

log_section "Building Bigint I/O Tests"
CMAKELISTS_DIR=""

if [ -f "${SCRIPT_DIR}/../CMakeLists.txt" ]; then
    CMAKELISTS_DIR="${SCRIPT_DIR}/.."
elif [ -f "${SCRIPT_DIR}/../../CMakeLists.txt" ]; then
    CMAKELISTS_DIR="${SCRIPT_DIR}/../.."
elif [ -f "${SCRIPT_DIR}/../../../CMakeLists.txt" ]; then
    CMAKELISTS_DIR="${SCRIPT_DIR}/../../.."
elif [ -f "${SCRIPT_DIR}/../../../../CMakeLists.txt" ]; then
    CMAKELISTS_DIR="${SCRIPT_DIR}/../../../../"
else
    echo -e "${RED}Error: Could not find CMakeLists.txt${NC}"
    exit 1
fi

BUILD_DIR="${CMAKELISTS_DIR}/build"
BIN_DIR="${BUILD_DIR}/bin"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

log_info "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

log_info "Building..."
CPU_COUNT=$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)

if cmake --build "${BUILD_DIR}" --parallel ${CPU_COUNT}; then
    true
else
    (cd "${BUILD_DIR}" && make -j"${CPU_COUNT}")
fi

cd "$SCRIPT_DIR"

# Verify BIN_DIR exists
if [ ! -d "$BIN_DIR" ]; then
    echo -e "${RED}Error: Build failed - ${BIN_DIR} not created${NC}"
    exit 1
fi

# Clear out all verbose CMake compilation output clutter before transitioning to selection menu
clear

# ============================================================================
# Define available runners
# ============================================================================
ALL_RUNNERS=(
    "binary_io"
    "bitos_conv_runner"
    "bitos_conv_sa_runner"
    "bitos_convf_runner"
    "bitos_convf_sa_runner"
    "bitos_print_runner"
    "bitos_printf_runner"
    "bitos_tconv_runner"
    "bitos_tconv_sa_runner"
    "bitos_util_runner"
    "stobi_assign_runner"
    "stobi_conv_runner"
    "stobi_fscan_runner"
    "stobi_fscan_strict"
    "stobi_fscan_trunc"
    "stobi_init_runner"
    "stobi_sassign_runner"
    "stobi_sassign_sa_runner"
    "stobi_tassign_runner"
    "stobi_tassign_sa_runner"
)

# Master hotkey mapping (indices 0-51: a-z, A-Z)
HOTKEYS_TEMPLATE="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

# Initialize selection flags
SELECTED_FLAGS=()
for ((i=0; i<${#ALL_RUNNERS[@]}; i++)); do
    SELECTED_FLAGS[i]=0
done

cleanup() {
    tput cnorm 2>/dev/null || true
    echo -e "\n${RED}Execution profile terminated by user.${NC}"
    exit 1
}
trap cleanup SIGINT SIGTERM

# ============================================================================
# Helper: Redraw menu
# ============================================================================
redraw_menu() {
    local runner_count=${#ALL_RUNNERS[@]}
    for ((i=0; i<runner_count; i++)); do
        local hotkey="${HOTKEYS_TEMPLATE:i:1}"
        local checkbox="[ ]"
        
        if [ ${SELECTED_FLAGS[i]} -eq 1 ]; then
            checkbox="[${GREEN}*${NC}]"
        fi
        
        if [ $i -eq $cursor ]; then
            echo -e " > ${checkbox} (${YELLOW}${hotkey}${NC}) ${BLUE}${ALL_RUNNERS[i]}${NC}"
        else
            echo -e "   ${checkbox} (${YELLOW}${hotkey}${NC}) ${ALL_RUNNERS[i]}"
        fi
    done
}

# ============================================================================
# Interactive Checklist Engine (Hybrid Controls)
# ============================================================================
log_section "Bigint I/O Execution Configuration Target Selector"
echo -e "${CYAN}Controls: [Arrows] Navigate | [Space] Toggle | [Enter] Confirm${NC}"
echo -e "${CYAN}Hotkeys:  Press a single letter key (case-sensitive) to toggle immediately${NC}\n"

cursor=0
runner_count=${#ALL_RUNNERS[@]}

tput civis 2>/dev/null || true
redraw_menu

while true; do
    IFS= read -rsn1 -t 30 key
    
    if [[ "$key" == $'\x1b' ]]; then
        IFS= read -rsn2 -t 1 arrow_seq
        
        case "$arrow_seq" in
            "[A")  # Up Arrow
                ((cursor--))
                [ $cursor -lt 0 ] && cursor=$((runner_count - 1))
                ;;
            "[B")  # Down Arrow
                ((cursor++))
                [ $cursor -ge $runner_count ] && cursor=0
                ;;
        esac
        
        for ((i=0; i<runner_count; i++)); do
            tput cuu1 2>/dev/null || true
        done
        tput ed 2>/dev/null || true
        redraw_menu
        
    elif [[ "$key" == "" ]]; then
        break
        
    elif [[ "$key" == " " ]]; then
        if [ ${SELECTED_FLAGS[$cursor]} -eq 1 ]; then
            SELECTED_FLAGS[$cursor]=0
        else
            SELECTED_FLAGS[$cursor]=1
        fi
        
        for ((i=0; i<runner_count; i++)); do
            tput cuu1 2>/dev/null || true
        done
        tput ed 2>/dev/null || true
        redraw_menu
        
    elif [[ "$key" =~ ^[a-zA-Z]$ ]]; then
        hotkey_idx=-1
        for ((idx=0; idx<${#HOTKEYS_TEMPLATE}; idx++)); do
            if [[ "${HOTKEYS_TEMPLATE:idx:1}" == "$key" ]]; then
                hotkey_idx=$idx
                break
            fi
        done
        
        if [ $hotkey_idx -ge 0 ] && [ $hotkey_idx -lt $runner_count ]; then
            if [ ${SELECTED_FLAGS[$hotkey_idx]} -eq 1 ]; then
                SELECTED_FLAGS[$hotkey_idx]=0
            else
                SELECTED_FLAGS[$hotkey_idx]=1
            fi
            
            for ((i=0; i<runner_count; i++)); do
                tput cuu1 2>/dev/null || true
            done
            tput ed 2>/dev/null || true
            redraw_menu
        fi
    fi
done

tput cnorm 2>/dev/null || true
echo ""

# ============================================================================
# Collate selections
# ============================================================================
SELECTED_RUNNERS=""
for ((i=0; i<runner_count; i++)); do
    if [ ${SELECTED_FLAGS[i]} -eq 1 ]; then
        SELECTED_RUNNERS="${SELECTED_RUNNERS} ${ALL_RUNNERS[i]}"
    fi
done

if [ -z "${SELECTED_RUNNERS}" ]; then
    log_fail "No execution streams were checked. Exiting."
    exit 1
fi

# ============================================================================
# CLI - Random case count
# ============================================================================
read -p "Number of random cases per runner [default 100]: " rcount_input
RCOUNT=${rcount_input:-100}

if ! [[ "$RCOUNT" =~ ^[0-9]+$ ]]; then
    echo -e "${RED}Error: Invalid allocation boundaries.${NC}"
    exit 1
fi

# Clear screen when transitioning away from menu setups directly into raw code diagnostics
clear

# ============================================================================
# Execute runners sequentially
# ============================================================================
SESSION_COUNT=$(echo "$SELECTED_RUNNERS" | wc -w)
log_section "Launching Architecture Suites (rcount=$RCOUNT, sessions=$SESSION_COUNT)"

RUNNER_NUM=1

for runner in $SELECTED_RUNNERS; do
    exe="$BIN_DIR/$runner"
    
    if [ ! -f "$exe" ]; then
        log_fail "$runner profile bypass: target file not found at $exe"
        ((RUNNER_NUM++))
        continue
    fi
    
    log_info "[$RUNNER_NUM/$SESSION_COUNT] Initializing Context: $runner"
    echo -e "${CYAN}------------------------------------------------------------${NC}"
    
    # Execution metrics and diagnostic stream tables completely print natively here
    "$exe" "$RCOUNT" "$SESSION_COUNT"
    
    echo -e "${CYAN}------------------------------------------------------------${NC}\n"
    ((RUNNER_NUM++))
done

log_info "Selected Bigint I/O framework execution loop completed."
exit 0