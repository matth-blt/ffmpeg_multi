@echo off
REM Test Auto-Boost-Filtering script
REM Quick compile and run

setlocal enabledelayedexpansion

echo.
echo [36m🔨 Compiling test_filtering.cpp...[0m
echo.

cd /d "%~dp0..\tools"

REM Compile
g++ -std=c++17 -O2 -o test_filtering.exe test_filtering.cpp

if %errorlevel% neq 0 (
    echo [31m✗ Compilation failed[0m
    exit /b 1
)

echo [32m✓ Compilation successful![0m
echo.
echo [36m🚀 Running test...[0m
echo.

REM Run
if "%~1"=="" (
    test_filtering.exe
) else (
    test_filtering.exe "%~1"
)

if %errorlevel% neq 0 (
    echo.
    echo [31m✗ Test failed[0m
    exit /b 1
)

echo.
echo [32m✓ Test completed![0m
echo.
