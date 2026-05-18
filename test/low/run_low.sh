#!/bin/bash
################################################################################
# run_low.sh - Low-level architecture test runner (Hybrid Interactive CLI)
# Self-contained: Handles its own build + runner selection + execution
# Prompts user to:
#   1) Select runners via cursor keys OR alphabet hotkeys
#   2) Enter random case count
# Builds CMake project, executes runners sequentially
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

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
}

log_section() {
    echo -e "\n${YELLOW}=== $1 ===${NC}\n"
}

# ============================================================================
# Self-contained build
# ============================================================================

log_section "Building Low-Level Tests"

# Find CMakeLists.txt
CMAKELISTS_DIR="$SCRIPT_DIR"
if [ ! -f "${CMAKELISTS_DIR}/CMakeLists.txt" ]; then
    if [ -f "${SCRIPT_DIR}/../CMakeLists.txt" ]; then
        CMAKELISTS_DIR="${SCRIPT_DIR}/.."
    elif [ -f "${SCRIPT_DIR}/../../CMakeLists.txt" ]; then
        CMAKELISTS_DIR="${SCRIPT_DIR}/../.."
    else
        echo -e "${RED}Error: Could not find CMakeLists.txt${NC}"
        echo "Searched:"
        echo "  - ${SCRIPT_DIR}/CMakeLists.txt"
        echo "  - ${SCRIPT_DIR}/../CMakeLists.txt"
        echo "  - ${SCRIPT_DIR}/../../CMakeLists.txt"
        exit 1
    fi
fi

BUILD_DIR="${CMAKELISTS_DIR}/build"
BIN_DIR="${BUILD_DIR}/bin"

# Create and configure build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

log_info "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

log_info "Building..."
make -j$(nproc)

cd "$SCRIPT_DIR"

# Verify BIN_DIR exists
if [ ! -d "$BIN_DIR" ]; then
    echo -e "${RED}Error: Build failed - ${BIN_DIR} not created${NC}"
    exit 1
fi

# ============================================================================
# Define available runners
# ============================================================================
ALL_RUNNERS=(
    "arm64_runner"
    "rv64_runner"
    "x86_runner"
)

# Master hotkey mapping (indices 0-51: a-z, A-Z)
HOTKEYS_TEMPLATE="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

# Initialize selection flags
SELECTED_FLAGS=()
for ((i=0; i<${#ALL_RUNNERS[@]}; i++)); do
    SELECTED_FLAGS[i]=0
done

# Cleanup function
cleanup() {
    tput cnorm 2>/dev/null || true
    echo -e "\n${RED}Testing cancelled by user.${NC}"
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
log_section "Low-Level Architecture Test Runner"
echo -e "${CYAN}Controls: [Arrows] Navigate | [Space] Toggle | [Enter] Confirm${NC}"
echo -e "${CYAN}Hotkeys:  Press a single letter key (case-sensitive) to toggle immediately${NC}\n"

cursor=0
runner_count=${#ALL_RUNNERS[@]}

# Hide cursor
tput civis 2>/dev/null || true

# Initial menu draw
redraw_menu

while true; do
    # Read single character with longer timeout (30 seconds for thinking)
    IFS= read -rsn1 -t 30 key
    read_status=$?
    
    # Check for escape sequence (arrow keys)
    if [[ "$key" == $'\x1b' ]]; then
        # Read the rest of the escape sequence
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
        
        # Clear and redraw
        for ((i=0; i<runner_count; i++)); do
            tput cuu1 2>/dev/null || true
        done
        tput ed 2>/dev/null || true
        redraw_menu
        
    elif [[ "$key" == "" ]]; then
        # Enter key - confirm selection
        break
        
    elif [[ "$key" == " " ]]; then
        # Spacebar - toggle current item
        if [ ${SELECTED_FLAGS[$cursor]} -eq 1 ]; then
            SELECTED_FLAGS[$cursor]=0
        else
            SELECTED_FLAGS[$cursor]=1
        fi
        
        # Clear and redraw
        for ((i=0; i<runner_count; i++)); do
            tput cuu1 2>/dev/null || true
        done
        tput ed 2>/dev/null || true
        redraw_menu
        
    elif [[ "$key" =~ ^[a-zA-Z]$ ]]; then
        # Alphabet hotkey - find index in template
        hotkey_idx=-1
        for ((idx=0; idx<${#HOTKEYS_TEMPLATE}; idx++)); do
            if [[ "${HOTKEYS_TEMPLATE:idx:1}" == "$key" ]]; then
                hotkey_idx=$idx
                break
            fi
        done
        
        if [ $hotkey_idx -ge 0 ] && [ $hotkey_idx -lt $runner_count ]; then
            # Toggle the runner at this index
            if [ ${SELECTED_FLAGS[$hotkey_idx]} -eq 1 ]; then
                SELECTED_FLAGS[$hotkey_idx]=0
            else
                SELECTED_FLAGS[$hotkey_idx]=1
            fi
            
            # Clear and redraw
            for ((i=0; i<runner_count; i++)); do
                tput cuu1 2>/dev/null || true
            done
            tput ed 2>/dev/null || true
            redraw_menu
        fi
    fi
done

# Restore cursor
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

# Guard clause
if [ -z "${SELECTED_RUNNERS}" ]; then
    log_fail "No architecture suites were selected. Exiting."
    exit 1
fi

# ============================================================================
# CLI - Random case count
# ============================================================================
read -p "Number of random cases per runner [default 100]: " rcount_input
RCOUNT=${rcount_input:-100}

if ! [[ "$RCOUNT" =~ ^[0-9]+$ ]]; then
    echo -e "${RED}Error: Invalid number.${NC}"
    exit 1
fi

# ============================================================================
# Execute runners sequentially
# ============================================================================
SESSION_COUNT=$(echo "$SELECTED_RUNNERS" | wc -w)
log_section "Running Architecture Suites (rcount=$RCOUNT, sessions=$SESSION_COUNT)"

PASSED=0
FAILED=0
RUNNER_NUM=1

for runner in $SELECTED_RUNNERS; do
    exe="$BIN_DIR/$runner"
    
    if [ ! -f "$exe" ]; then
        log_fail "$runner (executable not found at $exe)"
        ((FAILED++))
        ((RUNNER_NUM++))
        continue
    fi
    
    log_info "[$RUNNER_NUM/$SESSION_COUNT] Running: $runner"
    
    if "$exe" "$RCOUNT" "$SESSION_COUNT" 2>&1; then
        log_pass "$runner"
        ((PASSED++))
    else
        log_fail "$runner"
        ((FAILED++))
    fi
    
    ((RUNNER_NUM++))
done

# ============================================================================
# Summary
# ============================================================================
log_section "Summary"
echo -e "Passed: ${GREEN}$PASSED${NC} / Failed: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}All selected architecture tests passed!${NC}\n"
    exit 0
else
    echo -e "\n${RED}Some framework architectures failed.${NC}\n"
    exit 1
fi