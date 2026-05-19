#!/bin/bash
################################################################################
# run_test.sh - Top-level test dispatcher
# Delegates to lower-level test suites
# Each suite handles its own build independently
# No parameters - just dispatches
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_section() {
    echo -e "\n${YELLOW}=== $1 ===${NC}\n"
}

# ============================================================================
# Show dispatcher menu
# ============================================================================

log_section "Test Suite Dispatcher"

echo "Select a test suite to run:"
echo "  1) I/O Tests (bash run_io.sh)"
echo "  2) Low-Level Tests (bash run_low.sh)"
echo ""

read -p "Enter choice (1 or 2): " choice

case "$choice" in
    1)
        log_info "Launching I/O tests..."
        bash "$SCRIPT_DIR/run_io.sh"
        ;;
    2)
        log_info "Launching low-level tests..."
        bash "$SCRIPT_DIR/run_low.sh"
        ;;
    *)
        echo -e "${RED}Invalid choice${NC}"
        exit 1
        ;;
esac