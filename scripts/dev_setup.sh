#!/bin/bash
# lib-dnml developer setup — run once after cloning
set -e

OS="$(uname -s)"
ARCH="$(uname -m)"

echo "=== lib-dnml dev setup: $OS / $ARCH ==="

if [ "$OS" = "Linux" ]; then
    sudo apt-get update -q
    sudo apt-get install -y \
        cmake ninja-build \
        python3 python3-pip lua \
        gcc-riscv64-linux-gnu \
        riscv64-elf-gcc riscv64-elf-binutils \
    pip3 install --quiet matplotlib pandas pytest

elif [ "$OS" = "Darwin" ]; then
    # macOS — Rosetta handles x86_64 automatically
    brew install \
        cmake ninja \
        python3 lua \
        riscv64-elf-gcc riscv64-elf-binutils
    pip3 install matplotlib pandas pytest
fi

echo ""
echo "=== Setup complete ==="
echo "Run tests:       ./scripts/run_tests.sh [low|io|calc|main_math]"
echo "Run benchmarks:  ./scripts/run_bench.sh [mul|div|all]"
echo "CI simulation:   act  (install via brew/apt if you want local CI)"