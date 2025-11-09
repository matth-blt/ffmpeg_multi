# Test Auto-Boost-Filtering
# Quick compile and run test for the filtering script

param(
    [string]$InputVideo = ""
)

$ErrorActionPreference = "Stop"

Write-Host "`n🔨 Compiling test_filtering.cpp..." -ForegroundColor Cyan

# Paths
$projectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$toolsDir = Join-Path $projectRoot "tools"
$testSource = Join-Path $toolsDir "test_filtering.cpp"
$testExe = Join-Path $toolsDir "test_filtering.exe"

# Check if source exists
if (-not (Test-Path $testSource)) {
    Write-Host "✗ ERROR: Source not found: $testSource" -ForegroundColor Red
    exit 1
}

# Compile with g++
try {
    Push-Location $toolsDir
    
    $compileCmd = "g++ -std=c++17 -O2 -o test_filtering.exe test_filtering.cpp"
    Write-Host "  $compileCmd" -ForegroundColor Gray
    
    Invoke-Expression $compileCmd
    
    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed"
    }
    
    Write-Host "✓ Compilation successful!`n" -ForegroundColor Green
    
    Pop-Location
    
} catch {
    Write-Host "✗ ERROR: Compilation failed: $_" -ForegroundColor Red
    Pop-Location
    exit 1
}

# Run test
Write-Host "🚀 Running test...`n" -ForegroundColor Cyan

if ($InputVideo -ne "") {
    & $testExe $InputVideo
} else {
    & $testExe
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n✗ Test failed" -ForegroundColor Red
    exit 1
}

Write-Host "`n✓ Test completed!`n" -ForegroundColor Green
