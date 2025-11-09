// Test program for Auto-Boost-Filtering.py
// Copyright (c) 2025 - MIT License

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

// ANSI color codes (Catppuccin Mocha)
const std::string RESET = "\033[0m";
const std::string BLUE = "\033[38;5;117m";      // Lavender
const std::string GREEN = "\033[38;5;108m";     // Green
const std::string RED = "\033[38;5;203m";       // Red
const std::string YELLOW = "\033[38;5;229m";    // Yellow
const std::string CYAN = "\033[38;5;139m";      // Mauve

void printHeader(const std::string& title) {
    std::cout << "\n" << BLUE << "╔═══════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BLUE << "║  " << CYAN << title << std::string(53 - title.length(), ' ') << BLUE << "║" << RESET << "\n";
    std::cout << BLUE << "╚═══════════════════════════════════════════════════════╝" << RESET << "\n\n";
}

void printInfo(const std::string& label, const std::string& value) {
    std::cout << GREEN << "  " << label << ": " << RESET << value << "\n";
}

void printError(const std::string& message) {
    std::cout << RED << "  ✗ ERROR: " << message << RESET << "\n";
}

void printSuccess(const std::string& message) {
    std::cout << GREEN << "  ✓ " << message << RESET << "\n";
}

void printWarning(const std::string& message) {
    std::cout << YELLOW << "  ⚠ " << message << RESET << "\n";
}

std::string buildFilteringCommand(
    const std::string& scriptPath,
    const std::string& inputFile,
    const std::string& outputFile,
    const std::string& profile,
    const std::string& codec,
    const std::string& quality,
    const std::string& preset,
    bool verbose = false,
    bool analyzeOnly = false
) {
    std::string cmd = "powershell -ExecutionPolicy Bypass -File \"" + scriptPath + "\"";
    cmd += " -inputFile \"" + inputFile + "\"";
    cmd += " -outputFile \"" + outputFile + "\"";
    cmd += " -profile " + profile;
    cmd += " -codec " + codec;
    
    if (codec != "prores" && codec != "ffv1") {
        cmd += " -quality " + quality;
    }
    
    if (codec == "x264" || codec == "x265" || codec == "av1") {
        cmd += " -preset " + preset;
    }
    
    if (verbose) {
        cmd += " -verboseOutput";
    }
    
    if (analyzeOnly) {
        cmd += " -analyzeOnly";
    }
    
    return cmd;
}

int main(int argc, char* argv[]) {
    printHeader("🎬 Auto-Boost-Filtering Test");
    
    // Paths
    fs::path currentDir = fs::current_path();
    fs::path projectRoot = currentDir;
    
    // Find project root (contains extern/scripts)
    while (!fs::exists(projectRoot / "extern" / "scripts") && projectRoot.has_parent_path()) {
        projectRoot = projectRoot.parent_path();
    }
    
    fs::path scriptPath = projectRoot / "extern" / "scripts" / "ABF.ps1";
    
    // Check if PowerShell script exists
    if (!fs::exists(scriptPath)) {
        printError("PowerShell script not found: " + scriptPath.string());
        return 1;
    }
    printSuccess("Found script: " + scriptPath.filename().string());
    
    // Get input file
    std::string inputFile;
    if (argc > 1) {
        inputFile = argv[1];
    } else {
        std::cout << CYAN << "\n  Enter input video file path: " << RESET;
        std::getline(std::cin, inputFile);
    }
    
    // Remove surrounding quotes if present
    if (inputFile.size() >= 2 && inputFile.front() == '"' && inputFile.back() == '"') {
        inputFile = inputFile.substr(1, inputFile.size() - 2);
    }
    
    if (!fs::exists(inputFile)) {
        printError("Input file not found: " + inputFile);
        return 1;
    }
    
    fs::path inputPath(inputFile);
    fs::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + "_filtered" + inputPath.extension().string());
    
    printInfo("Input", inputPath.string());
    printInfo("Output", outputPath.string());
    
    // Get profile
    std::cout << "\n" << CYAN << "  Select profile:" << RESET << "\n";
    std::cout << "    1. Light (clean scenes only)\n";
    std::cout << "    2. Medium (automatic - default)\n";
    std::cout << "    3. Heavy (aggressive)\n";
    std::cout << "    4. Extreme (maximum quality)\n";
    std::cout << "    5. Debug (analysis only)\n";
    std::cout << CYAN << "\n  Choice [2]: " << RESET;
    
    std::string profileChoice;
    std::getline(std::cin, profileChoice);
    if (profileChoice.empty()) profileChoice = "2";
    
    std::string profile = "medium";
    bool analyzeOnly = false;
    
    switch (profileChoice[0]) {
        case '1': profile = "light"; break;
        case '2': profile = "medium"; break;
        case '3': profile = "heavy"; break;
        case '4': profile = "extreme"; break;
        case '5': analyzeOnly = true; break;
        default: profile = "medium"; break;
    }
    
    // Get codec
    std::cout << "\n" << CYAN << "  Select codec:" << RESET << "\n";
    std::cout << "    1. x264 (H.264 - default)\n";
    std::cout << "    2. x265 (H.265/HEVC)\n";
    std::cout << "    3. AV1 (SVT-AV1)\n";
    std::cout << "    4. ProRes (high quality)\n";
    std::cout << "    5. FFV1 (lossless)\n";
    std::cout << CYAN << "\n  Choice [1]: " << RESET;
    
    std::string codecChoice;
    std::getline(std::cin, codecChoice);
    if (codecChoice.empty()) codecChoice = "1";
    
    std::string codec = "x264";
    switch (codecChoice[0]) {
        case '1': codec = "x264"; break;
        case '2': codec = "x265"; break;
        case '3': codec = "av1"; break;
        case '4': codec = "prores"; break;
        case '5': codec = "ffv1"; break;
        default: codec = "x264"; break;
    }
    
    // Get quality
    std::string quality = "18";
    if (codec != "prores" && codec != "ffv1") {
        std::cout << "\n" << CYAN << "  Quality (CRF 0-51) [18]: " << RESET;
        std::getline(std::cin, quality);
        if (quality.empty()) quality = "18";
    }
    
    // Get preset
    std::string preset = "slow";
    if (codec == "x264" || codec == "x265" || codec == "av1") {
        std::cout << CYAN << "  Preset (ultrafast/superfast/veryfast/faster/fast/medium/slow/slower/veryslow) [slow]: " << RESET;
        std::getline(std::cin, preset);
        if (preset.empty()) preset = "slow";
    }
    
    // Build command
    std::string cmd = buildFilteringCommand(
        scriptPath.string(),
        inputPath.string(),
        outputPath.string(),
        profile,
        codec,
        quality,
        preset,
        true,  // verbose
        analyzeOnly
    );
    
    printHeader("🚀 Executing Filtering");
    printInfo("Profile", profile);
    printInfo("Codec", codec);
    if (codec != "prores" && codec != "ffv1") {
        printInfo("Quality", "CRF " + quality);
    }
    if (codec == "x264" || codec == "x265" || codec == "av1") {
        printInfo("Preset", preset);
    }
    
    std::cout << "\n" << YELLOW << "  Command: " << RESET << "\n";
    std::cout << "  " << cmd << "\n\n";
    
    std::cout << CYAN << "  Press Enter to execute or Ctrl+C to cancel... " << RESET;
    std::cin.get();
    
    // Execute
    std::cout << "\n" << BLUE << "═══════════════════════════════════════════════════════" << RESET << "\n\n";
    
    int result = system(cmd.c_str());
    
    std::cout << "\n" << BLUE << "═══════════════════════════════════════════════════════" << RESET << "\n";
    
    if (result == 0) {
        printSuccess("Filtering completed successfully!");
        printInfo("Output file", outputPath.string());
        
        if (fs::exists(outputPath)) {
            auto fileSize = fs::file_size(outputPath);
            double sizeMB = fileSize / (1024.0 * 1024.0);
            printInfo("File size", std::to_string(static_cast<int>(sizeMB)) + " MB");
        }
    } else {
        printError("Filtering failed with code: " + std::to_string(result));
        return 1;
    }
    
    std::cout << "\n";
    return 0;
}
