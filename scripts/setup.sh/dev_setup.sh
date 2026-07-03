#!/bin/sh
# lib-dnml developer setup — POSIX compliant for maximum UNIX portability
set -e

OS="$(uname -s)"
ARCH="$(uname -m)"

echo "=== lib-dnml dev setup: $OS / $ARCH ==="

# POSIX compliant function (no 'function' keyword, no local vars)
install_python_deps() {
    echo "Installing Python dependencies (matplotlib, pandas, pytest)..."
    # Attempt modern PEP 668 bypass, fallback to standard if old pip version
    pip3 install --quiet --break-system-packages matplotlib pandas pytest 2>/dev/null || \
    pip3 install --quiet matplotlib pandas pytest 2>/dev/null || \
}

# 1. macOS (Darwin)
if [ "$OS" = "Darwin" ]; then
    if ! command -v brew > /dev/null 2>&1; then
        echo "Error: Homebrew is required on macOS but was not found."
        exit 1
    fi
    brew install cmake ninja python3 lua
    install_python_deps

# 2. Linux & GNU-free/BSD Ecosystems
elif [ "$OS" = "Linux" ] || [ "$OS" = "FreeBSD" ] || [ "$OS" = "OpenBSD" ]; then
    # Debian/Ubuntu (apt)
    if command -v apt-get > /dev/null 2>&1; then
        echo "Detected Debian/Ubuntu-based system."
        sudo apt-get update -q
        sudo apt-get install -y cmake ninja-build python3 python3-pip lua5.3
        install_python_deps

    # Arch Linux (pacman)
    elif command -v pacman > /dev/null 2>&1; then
        echo "Detected Arch-based system."
        sudo pacman -Sy --noconfirm cmake ninja python python-pip lua
        install_python_deps

    # Fedora/RHEL (dnf)
    elif command -v dnf > /dev/null 2>&1; then
        echo "Detected Fedora/RHEL-based system."
        sudo dnf install -y cmake ninja-build python3 python3-pip lua
        install_python_deps

    # Alpine Linux (apk) — Core of many GNU-free environments
    elif command -v apk > /dev/null 2>&1; then
        echo "Detected Alpine Linux (GNU-free BusyBox)."
        sudo apk add --no-cache cmake ninja python3 py3-pip lua5.3
        install_python_deps

    # FreeBSD (pkg) — Pure BSD / True UNIX-derived
    elif command -v pkg > /dev/null 2>&1; then
        echo "Detected FreeBSD system."
        sudo pkg install -y cmake ninja python3 py38-pip lua53
        install_python_deps

    else
        echo "Error: Package manager not recognized."
        echo "Please manually install: cmake, ninja, python3, and lua."
        exit 1
    fi
fi

# 3. Cross-Platform Rust Toolchain (Works on GNU-free Musl targets via rustup)
if ! command -v rustc > /dev/null 2>&1; then
    echo "Rust not found. Installing via rustup..."
    curl --proto '=https' --tlsv1.2 -sSf https://rustup.rs | sh -s -- -y
    
    # Safely source Cargo environment across different shells
    if [ -f "$HOME/.cargo/env" ]; then
        . "$HOME/.cargo/env"
    fi
else
    echo "Rust is already installed."
fi

echo ""
echo "=== Setup complete ==="