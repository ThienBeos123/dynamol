#!/bin/bash
################################################################################
# run_io.sh - I/O test suite dispatcher
# Delegates to specific I/O test suites
# Each suite handles its own build independently
# Self-contained CLI - no arguments
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

log_section "I/O Test Suite"

echo "Select I/O test suite:"
echo "  1) Bigint I/O Tests (bash run_bi_io.sh)"
echo ""

read -p "Enter choice (1): " choice

case "$choice" in
    1)
        log_info "Launching bigint I/O tests..."
        bash "$SCRIPT_DIR/run_bi_io.sh"
        ;;
    *)
        echo -e "${RED}Invalid choice${NC}"
        exit 1
        ;;
esac