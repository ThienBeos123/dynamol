@echo off
setlocal enabledelayedexpansion
set "MY_CPU_ARCH=%PROCESSOR_ARCHITECTURE%"
if defined PROCESSOR_ARCHITEW6432 (
    set "MY_CPU_ARCH=%PROCESSOR_ARCHITEW6432%"
)

echo /p="=== lib-dnml dev setup: Windows / " < NUL
if /I "%MY_CPU_ARCH%" == "AMD64" (
    echo x86_64
) else if /I "%MY_CPU_ARCH%" == "ARM64" (
    echo AARCH64
) else if /I "%MY_CPU_ARCH%" == "x86" (
    echo i386
) else (
    echo ?
)

winget source update
winget install --id Kitware.CMake -e --silent
winget install --id Ninja-build.Ninja -e --silent
winget install --id Python.Python.3.11 -e --silent

refreshenv
pip install --quiet matplotlib pandas pytest

echo.
echo "=== Setup complete ==="