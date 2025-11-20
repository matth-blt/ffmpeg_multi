#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include "../../include/core/app.hpp"
#include "../../include/core/string_utils.hpp"
#include "../../include/core/colors.hpp"
#include "../../include/core/input.hpp"
#include "../../include/jobs/encode.hpp"
#include "../../include/jobs/extract_frames.hpp"
#include "../../include/jobs/probe.hpp"
#include "../../include/jobs/reencode.hpp"
#include "../../include/jobs/reencode_builder.hpp"
#include "../../include/jobs/svt_av1_essential.hpp"
#include "../../include/jobs/thumbnails.hpp"
#include "../../include/jobs/extract_frames.hpp"
#include "../../include/jobs/probe.hpp"
#include "../../include/jobs/reencode.hpp"
#include "../../include/jobs/reencode_builder.hpp"
#include "../../include/jobs/svt_av1_essential.hpp"
#include "../../include/jobs/thumbnails.hpp"
#include "../../include/jobs/concat.hpp"

using namespace FFmpegMulti::Jobs;
using namespace FFmpegMulti::Encode;
using namespace FFmpegMulti;

namespace App {

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Calculate actual display length (UTF-8)
// Approximately handles wide characters (CJK, Emojis)
int getDisplayLength(const std::string& str) {
    int length = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if ((c & 0x80) == 0) { // ASCII (1 byte)
            length += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) { // 2 bytes
            length += 1;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3 bytes (often CJK -> wide)
            length += 2; 
            i += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4 bytes (Emojis -> wide)
            length += 2;
            i += 4;
        } else {
            i += 1; // Invalid character
        }
    }
    return length;
}

void printHeader(const std::string& title) {
    const int TOTAL_WIDTH = 60;
    const int INNER_WIDTH = TOTAL_WIDTH - 2; // -2 for borders
    
    int titleLen = getDisplayLength(title);
    int padding = std::max(0, INNER_WIDTH - titleLen);
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;
    
    std::cout << Colors::BLUE << "╭";
    for(int i = 0; i < INNER_WIDTH; i++)
        std::cout << "─";
    std::cout << "╮" << Colors::RESET << std::endl;
    
    std::cout << Colors::BLUE << "│" << Colors::RESET;
    std::cout << std::string(leftPad, ' ');
    std::cout << Colors::BOLD << Colors::LAVENDER << title << Colors::RESET;
    std::cout << std::string(rightPad, ' ');
    std::cout << Colors::BLUE << "│" << Colors::RESET << std::endl;
    
    std::cout << Colors::BLUE << "╰";
    for(int i = 0; i < INNER_WIDTH; i++)
        std::cout << "─";
    std::cout << "╯" << Colors::RESET << std::endl;
}

void printSeparator() {
    const int TOTAL_WIDTH = 60;
    std::cout << Colors::BLUE;
    for(int i = 0; i < TOTAL_WIDTH; i++)
        std::cout << "─";
    std::cout << Colors::RESET << std::endl;
}

void printOption(int id, const std::string& description) {
    const int TOTAL_WIDTH = 60;
    // Structure: "│ ID  │  Description      │"
    // Prefix: "│ " (2) + ID (2) + "  │  " (5) = 9 chars
    // Suffix: "│" (1)
    // Available for description + padding: TOTAL_WIDTH - 10 = 50
    
    std::cout << Colors::BLUE << "│ " << Colors::MAUVE << Colors::BOLD;
    if (id < 10) std::cout << " ";
    std::cout << id << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << description;
    
    int descLen = getDisplayLength(description);
    int availableSpace = TOTAL_WIDTH - 10; // 50
    int paddingNeeded = std::max(0, availableSpace - descLen);
    
    std::cout << std::string(paddingNeeded, ' ') << Colors::BLUE << "│" << Colors::RESET << std::endl;
}

// Function to prompt for input and output files
void promptFiles(std::string& inputFile, std::string& outputFile, const std::string& outputHint = "") {
    inputFile = Input::getString("Input file");
    outputFile = Input::getString("Output file", outputHint);
}

// Function to prompt for CRF/QP
int promptQuality(const std::string& type, const std::string& examples) {
    std::cout << Colors::TEXT << "  Quality " << type << " " << Colors::SUBTEXT << examples << Colors::RESET << std::endl;
    return Input::getInt("Value");
}

// Function to prompt for a preset
std::string promptPreset(const std::string& options) {
    std::cout << Colors::TEXT << "  Preset " << Colors::SUBTEXT << options << Colors::RESET << std::endl;
    return Input::getString("Choice");
}

// Function to confirm and execute a job
template<typename JobType> bool confirmAndExecute(JobType& job, const std::string& outputFile = "") {
    std::cout << std::endl;
    
    bool confirm = Input::getConfirm("Start operation?");
    
    if (confirm) {
        std::cout << std::endl;
        std::cout << Colors::BLUE << Colors::BOLD << ">>> Starting operation..." << Colors::RESET << std::endl;
        printSeparator();
        
        bool success = job.execute();
        
        printSeparator();
        std::cout << std::endl;
        if (success) {
            std::cout << Colors::GREEN << Colors::BOLD << "[OK] Operation completed successfully!" << Colors::RESET << std::endl;
            if (!outputFile.empty()) {
                std::cout << Colors::TEAL << "> Output file: " << Colors::TEXT << outputFile << Colors::RESET << std::endl;
            }
        } else {
            std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Operation failed. Check errors above." << Colors::RESET << std::endl;
        }
        return success;
    } else {
        std::cout << std::endl;
        std::cout << Colors::YELLOW << "[INFO] Operation cancelled." << Colors::RESET << std::endl;
        return false;
    }
}

// Function to handle errors
void handleError(const std::exception& e) {
    std::cerr << std::endl;
    std::cerr << Colors::RED << Colors::BOLD << "[ERROR]: " << Colors::RESET << Colors::RED << e.what() << Colors::RESET << std::endl;
}

void affiche() {
    const int TOTAL_WIDTH = 60;
    const int INNER_WIDTH = TOTAL_WIDTH - 2;

    std::cout << std::endl;
    printHeader("MULTI-FFMPEG TOOL");
    std::cout << std::endl;
    
    // Top border of the menu table
    std::cout << Colors::BLUE << "╭─────┬";
    for(int i = 0; i < INNER_WIDTH - 6; i++)
        std::cout << "─";
    std::cout << "╮" << Colors::RESET << std::endl;

    // Header row
    std::cout << Colors::BLUE << "│" << Colors::MAUVE << Colors::BOLD << " ID  " << Colors::RESET << Colors::BLUE << "│  " << Colors::TEXT << "Action" << std::string(INNER_WIDTH - 6 - 8, ' ') << Colors::BLUE << "│" << Colors::RESET << std::endl;
    
    // Separator
    std::cout << Colors::BLUE << "├─────┼";
    for(int i = 0; i < INNER_WIDTH - 6; i++)
        std::cout << "─";
    std::cout << "┤" << Colors::RESET << std::endl;
    
    printOption(1, "Extract frames");
    printOption(2, "Encode a video");
    printOption(3, "Re-encode a file");
    printOption(4, "Concatenate multiple videos");
    printOption(5, "Generate thumbnails");
    printOption(6, "Encode with SVT-AV1-Essential");
    printOption(7, "Analyze media (ffprobe)");
    
    // Separator
    std::cout << Colors::BLUE << "├─────┼";
    for(int i = 0; i < INNER_WIDTH - 6; i++)
        std::cout << "─";
    std::cout << "┤" << Colors::RESET << std::endl;

    printOption(0, "Quit");
    
    // Bottom border
    std::cout << Colors::BLUE << "╰─────┴";
    for(int i = 0; i < INNER_WIDTH - 6; i++)
        std::cout << "─";
    std::cout << "╯" << Colors::RESET << std::endl;
}

bool choice() {
    int userChoice = 0;
    try {
        userChoice = Input::getInt("Enter your choice");
    } catch (const BackException&) {
        return true; // Exit if back at main menu
    }
    
    std::cout << std::endl;
    
    switch (userChoice) {
        case 1: {
            try {
                std::string inputFile, outputDir;
                bool createSubfolder;
                std::string subfolderName;
                int formatChoice;

                printHeader("EXTRACT FRAMES");
                std::cout << std::endl;

                // Ask for input file
                inputFile = Input::getString("Input file");
                std::cout << std::endl;

                // Ask for output directory
                outputDir = Input::getString("Output directory");
                std::cout << std::endl;

                // Ask if create subfolder
                createSubfolder = Input::getConfirm("Create a subfolder");
                
                if (createSubfolder) {
                    subfolderName = Input::getString("Subfolder name");
                }
                
                std::cout << std::endl;

                // Format choice menu
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choose an image format ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " PNG " << Colors::SUBTEXT << "- Lossless, RGB24" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " TIFF " << Colors::SUBTEXT << "- Deflate compression, RGB24" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " JPEG " << Colors::SUBTEXT << "- Max quality, YUV420p" << Colors::RESET << std::endl;
                
                printSeparator();
                formatChoice = Input::getIntRange("Your choice", 1, 3);
                std::cout << std::endl;
                
                try {
                    ExtractFramesBuilder builder;
                    builder.input(inputFile).outputDir(outputDir).createSubfolder(createSubfolder);
                    
                    if (createSubfolder && !subfolderName.empty()) {
                        builder.subfolderName(subfolderName);
                    }
                    
                    // Configuration according to chosen format
                    switch (formatChoice) {
                        case 1:
                            builder.png();
                            std::cout << Colors::GREEN << "[OK] PNG format selected" << Colors::RESET << std::endl;
                            break;
                        case 2:
                            builder.tiff();
                            std::cout << Colors::GREEN << "[OK] TIFF format selected" << Colors::RESET << std::endl;
                            break;
                        case 3:
                            builder.jpeg();
                            std::cout << Colors::GREEN << "[OK] JPEG format selected" << Colors::RESET << std::endl;
                            break;
                    }
                    
                    std::cout << std::endl;
                    
                    // Confirmation and execution
                    auto job = builder.build();
                    confirmAndExecute<ExtractFramesJob>(job);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 2: {
            try {
                std::string inputDir, outputDir, outputFilename, inputPattern;
                int codecChoice, qualityChoice, formatChoice, framerate;
                std::string presetChoice;

                printHeader("ENCODE A VIDEO");
                std::cout << std::endl;

                // Ask for input directory (images)
                inputDir = Input::getString("Input directory (images)");
                std::cout << std::endl;

                // Ask for image pattern
                inputPattern = Input::getString("Image pattern", "%08d.png", true);
                if (inputPattern.empty()) {
                    inputPattern = "%08d.png";
                }
                std::cout << std::endl;

                // Ask for framerate
                framerate = Input::getInt("Framerate (FPS)", "24");
                std::cout << std::endl;

                // Ask for output directory
                outputDir = Input::getString("Output directory");
                std::cout << std::endl;

                // Ask for output filename
                outputFilename = Input::getString("Output filename");
                std::cout << std::endl;

                // Format choice menu
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choose a container format ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " MKV " << Colors::SUBTEXT << "- Matroska (recommended)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " WEBM " << Colors::SUBTEXT << "- WebM (web)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " MP4 " << Colors::SUBTEXT << "- MPEG-4 (universal)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " MOV " << Colors::SUBTEXT << "- QuickTime" << Colors::RESET << std::endl;
                
                printSeparator();
                formatChoice = Input::getIntRange("Your choice", 1, 4);
                std::cout << std::endl;

                // Codec choice menu (reused from reencode)
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choose a codec ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " X264 " << Colors::SUBTEXT << "- Universal, good compatibility" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " X265 " << Colors::SUBTEXT << "- Better compression, superior quality" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " AV1 " << Colors::SUBTEXT << "- Max compression for web" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " H264_NVENC " << Colors::SUBTEXT << "- Hardware H.264 (NVIDIA)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " H265_NVENC " << Colors::SUBTEXT << "- Hardware H.265 (NVIDIA)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  6." << Colors::TEXT << " ProRes " << Colors::SUBTEXT << "- Apple ProRes 4444 (Production)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  7." << Colors::TEXT << " FFV1 " << Colors::SUBTEXT << "- Lossless codec for archiving" << Colors::RESET << std::endl;
                
                printSeparator();
                codecChoice = Input::getIntRange("Your choice", 1, 7);
                std::cout << std::endl;
                
                try {
                    EncodeJobBuilder builder;
                    builder.inputDir(inputDir).outputDir(outputDir).outputFilename(outputFilename).inputPattern(inputPattern).framerate(framerate);
                    
                    // Format configuration
                    switch (formatChoice) {
                        case 1:
                            builder.mkv();
                            std::cout << Colors::GREEN << "[OK] MKV format selected" << Colors::RESET << std::endl;
                            break;
                        case 2:
                            builder.webm();
                            std::cout << Colors::GREEN << "[OK] WEBM format selected" << Colors::RESET << std::endl;
                            break;
                        case 3:
                            builder.mp4();
                            std::cout << Colors::GREEN << "[OK] MP4 format selected" << Colors::RESET << std::endl;
                            break;
                        case 4:
                            builder.mov();
                            std::cout << Colors::GREEN << "[OK] MOV format selected" << Colors::RESET << std::endl;
                            break;
                    }
                    
                    // Configuration according to codec
                    switch (codecChoice) {
                        case 1: { // X264
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: X264 Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(18=excellent, 23=good, 28=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x264().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] X264 configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 2: { // X265
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: X265 Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(20=excellent, 28=good, 35=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x265().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] X265 configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 3: { // AV1
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: AV1 Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(25=excellent, 35=good, 45=acceptable)");
                            
                            builder.av1().quality(qualityChoice);
                            std::cout << Colors::GREEN << "[OK] AV1 configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 4: { // H264_NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: H264_NVENC Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CQ", "(18=excellent, 23=good, 28=acceptable)");
                            presetChoice = promptPreset("(fast/medium/slow/hq)");
                            
                            builder.h264_nvenc().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] H264_NVENC configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 5: { // H265_NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: H265_NVENC Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CQ", "(20=excellent, 28=good, 35=acceptable)");
                            presetChoice = promptPreset("(fast/medium/slow/hq)");
                            
                            builder.h265_nvenc().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] H265_NVENC configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 6: { // ProRes
                            builder.prores();
                            std::cout << Colors::GREEN << "[OK] ProRes 4444 configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 7: { // FFV1
                            builder.ffv1();
                            std::cout << Colors::GREEN << "[OK] FFV1 lossless configured" << Colors::RESET << std::endl;
                            break;
                        }
                    }
                    
                    std::cout << std::endl;
                    
                    // Confirmation and execution
                    auto job = builder.build();
                    confirmAndExecute<EncodeJob>(job);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 3: {
            try {
                std::string inputFile, outputFile;
                int codecChoice, qualityChoice;
                std::string presetChoice;

                printHeader("RE-ENCODE A FILE");
                std::cout << std::endl;

                // Ask for input and output files
                promptFiles(inputFile, outputFile);
                
                std::cout << std::endl;
                
                // Codec choice menu
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choose a codec ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " X264 " << Colors::SUBTEXT << "- Universal, good compatibility" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " X265 " << Colors::SUBTEXT << "- Better compression, superior quality" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " AV1 " << Colors::SUBTEXT << "- Max compression for web" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " H264_NVENC " << Colors::SUBTEXT << "- Hardware H.264 (NVIDIA)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " H265_NVENC " << Colors::SUBTEXT << "- Hardware H.265 (NVIDIA)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  6." << Colors::TEXT << " ProRes " << Colors::SUBTEXT << "- Apple ProRes 4444 (Production)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  7." << Colors::TEXT << " FFV1 " << Colors::SUBTEXT << "- Lossless codec for archiving" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  8." << Colors::TEXT << " YouTube Preset " << Colors::SUBTEXT << "- H.264, optimal quality" << Colors::RESET << std::endl;
                
                printSeparator();
                codecChoice = Input::getIntRange("Your choice", 1, 8);
                std::cout << std::endl;
                
                try {
                    ReencodeJobBuilder builder;
                    builder.input(inputFile).output(outputFile);
                    
                    // Configuration according to choice
                    switch (codecChoice) {
                        case 1: { // Custom X264
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: X264 Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(18=excellent, 23=good, 28=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x264().crf(qualityChoice).preset(presetChoice).copyAudio();
                            std::cout << Colors::GREEN << "[OK] X264 configured" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 2: { // Custom X265
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: X265 Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(18=excellent, 23=good, 28=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x265().crf(qualityChoice).preset(presetChoice).tenBit().copyAudio();
                            std::cout << Colors::GREEN << "[OK] X265 configured (10-bit)" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 3: { // AV1
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: AV1 Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(20=excellent, 30=good, 35=acceptable)");
                            
                            builder.svtav1().crf(qualityChoice).preset("5").copyAudio();
                            std::cout << Colors::GREEN << "[OK] AV1 configured (SVT-AV1)" << Colors::RESET << std::endl;
                            break;
                        }

                        case 4: { // H264 NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: H264_NVENC Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("QP", "(17=excellent, 22=good, 27=acceptable)");
                            presetChoice = promptPreset("(p1/p2/p3/p4/p5/p6/p7)");

                            builder.h264_nvenc().qp(qualityChoice).preset(presetChoice).pixelFormat(FFmpegMulti::Encode::PixelFormat::P010).copyAudio();
                            std::cout << Colors::GREEN << "[OK] H264_NVENC configured (Hardware)" << Colors::RESET << std::endl;
                            break;
                        }

                        case 5: { // H265 NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: H265_NVENC Configuration" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("QP", "(17=excellent, 22=good, 27=acceptable)");
                            presetChoice = promptPreset("(p1/p2/p3/p4/p5/p6/p7)");

                            builder.h265_nvenc().qp(qualityChoice).preset(presetChoice).pixelFormat(FFmpegMulti::Encode::PixelFormat::P010).tune("hq").copyAudio();
                            std::cout << Colors::GREEN << "[OK] H265_NVENC configured (Hardware, HQ)" << Colors::RESET << std::endl;
                            break;
                        }

                        case 6: { // ProRes
                            int profileChoice;
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: ProRes Profile" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  0." << Colors::TEXT << " Proxy " << Colors::SUBTEXT << "- Low quality, reduced size" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " LT " << Colors::SUBTEXT << "- Light, medium quality" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " Standard " << Colors::SUBTEXT << "- Good quality" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " HQ " << Colors::SUBTEXT << "- High Quality, excellent quality" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " 4444 " << Colors::SUBTEXT << "- Max quality with alpha" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " 4444 XQ " << Colors::SUBTEXT << "- Extreme quality" << Colors::RESET << std::endl;
                            
                            profileChoice = Input::getIntRange("Your choice", 0, 5);
                            
                            builder.prores().proresProfile(profileChoice).copyAudio().container("mov");
                            
                            std::cout << Colors::GREEN << "[OK] ProRes configured (Profile " << profileChoice << ")" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 7: { // FFV1
                            builder.ffv1Preset();
                            std::cout << Colors::GREEN << "[OK] FFV1 configured (Lossless, Level 3)" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 8: { // YouTube Preset
                            builder.youtubePreset();
                            std::cout << Colors::GREEN << "[OK] YouTube Preset applied (H.264, CRF 23, medium)" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        default:
                            std::cout << Colors::YELLOW << "[WARN] Invalid choice, using default YouTube preset" << Colors::RESET << std::endl;
                            builder.youtubePreset();
                            break;
                    }
                    
                    // Build the job
                    std::cout << std::endl;
                    std::cout << Colors::BLUE << ">>> Building re-encoding job..." << Colors::RESET << std::endl;
                    ReencodeJob job = builder.build();
                    
                    // Display the command that will be executed
                    std::cout << std::endl;
                    std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: FFmpeg Command" << Colors::RESET << std::endl;
                    printSeparator();
                    std::cout << Colors::SUBTEXT << job.getCommandString() << Colors::RESET << std::endl;
                    printSeparator();
                    
                    // Ask for confirmation and execute
                    confirmAndExecute(job, outputFile);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 4: {
            try {
                std::vector<std::string> inputs;
                std::string outputFile;
                
                printHeader("CONCATENATE VIDEOS");
                std::cout << std::endl;
                
                // Loop to add files
                while (true) {
                    std::string prompt = "Input file #" + std::to_string(inputs.size() + 1);
                    std::string input = Input::getString(prompt);
                    inputs.push_back(input);
                    
                    std::cout << std::endl;
                    if (!Input::getConfirm("Add another file")) {
                        break;
                    }
                    std::cout << std::endl;
                }
                
                std::cout << std::endl;
                
                // Ask for output file
                outputFile = Input::getString("Output file (.mkv)");
                
                // Force .mkv extension if missing
                if (outputFile.length() < 4 || outputFile.substr(outputFile.length() - 4) != ".mkv") {
                    outputFile += ".mkv";
                    std::cout << Colors::SUBTEXT << "[INFO] .mkv extension added automatically." << Colors::RESET << std::endl;
                }
                
                std::cout << std::endl;
                
                try {
                    ConcatBuilder builder;
                    for (const auto& input : inputs) {
                        builder.addInput(input);
                    }
                    builder.output(outputFile);
                    
                    ConcatJob job = builder.build();
                    
                    // Display summary
                    std::cout << Colors::SAPPHIRE << ":: Operation Summary :" << Colors::RESET << std::endl;
                    for (size_t i = 0; i < inputs.size(); ++i) {
                        std::cout << Colors::TEAL << "  • Input " << (i+1) << " : " << Colors::TEXT << inputs[i] << Colors::RESET << std::endl;
                    }
                    std::cout << Colors::TEAL << "  • Output   : " << Colors::TEXT << outputFile << Colors::RESET << std::endl;
                    std::cout << std::endl;
                    
                    confirmAndExecute(job, outputFile);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
                
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 5: {
            try {
                std::string inputFile, outputDir;
                bool createSubfolder;
                std::string subfolderName;
                int formatChoice;
                float sceneThreshold;

                printHeader("GENERATE THUMBNAILS");
                std::cout << std::endl;

                // Ask for input file
                inputFile = Input::getString("Input file");
                std::cout << std::endl;

                // Ask for output directory
                outputDir = Input::getString("Output directory");
                std::cout << std::endl;

                // Ask if create subfolder
                createSubfolder = Input::getConfirm("Create a subfolder");
                std::cout << std::endl;

                if (createSubfolder) {
                    subfolderName = Input::getString("Subfolder name");
                    std::cout << std::endl;
                }

                // Format choice
                std::cout << Colors::LAVENDER << "Available formats :" << Colors::RESET << std::endl;
                std::cout << Colors::BLUE << "  1. PNG  (rgb24, lossless)" << Colors::RESET << std::endl;
                std::cout << Colors::BLUE << "  2. TIFF (rgb24, deflate compression)" << Colors::RESET << std::endl;
                std::cout << Colors::BLUE << "  3. JPEG (yuvj420p, max quality)" << Colors::RESET << std::endl;
                std::cout << std::endl;
                formatChoice = Input::getIntRange("Output format", 1, 3);
                std::cout << std::endl;

                ThumbnailFormat format;
                switch (formatChoice) {
                    case 1: format = ThumbnailFormat::PNG; break;
                    case 2: format = ThumbnailFormat::TIFF; break;
                    case 3: format = ThumbnailFormat::JPEG; break;
                    default: format = ThumbnailFormat::PNG;
                }

                // Scene detection threshold
                sceneThreshold = Input::getFloat("Scene detection threshold (0.0-1.0)", "0.15");
                std::cout << std::endl;

                // Build the job
                try {
                    ThumbnailsBuilder builder;
                    builder.input(inputFile).outputDir(outputDir).format(format).sceneThreshold(sceneThreshold);

                    if (createSubfolder && !subfolderName.empty()) {
                        builder.createSubfolder(true).subfolderName(subfolderName);
                    } else {
                        builder.createSubfolder(false);
                    }

                    ThumbnailsJob job = builder.build();

                    // Display summary
                    std::cout << Colors::SAPPHIRE << ":: Operation Summary :" << Colors::RESET << std::endl;
                    std::cout << Colors::TEAL << "  • Input      : " << Colors::TEXT << inputFile << Colors::RESET << std::endl;
                    std::cout << Colors::TEAL << "  • Output      : " << Colors::TEXT << outputDir;
                    if (createSubfolder && !subfolderName.empty()) {
                        std::cout << "/" << subfolderName;
                    }
                    std::cout << Colors::RESET << std::endl;
                    
                    std::string formatName;
                    switch (format) {
                        case ThumbnailFormat::PNG: formatName = "PNG"; break;
                        case ThumbnailFormat::TIFF: formatName = "TIFF"; break;
                        case ThumbnailFormat::JPEG: formatName = "JPEG"; break;
                    }
                    std::cout << Colors::TEAL << "  • Format      : " << Colors::TEXT << formatName << Colors::RESET << std::endl;
                    std::cout << Colors::TEAL << "  • Scene threshold : " << Colors::TEXT << sceneThreshold << Colors::RESET << std::endl;
                    std::cout << std::endl;

                    // Ask for confirmation
                    bool confirm = Input::getConfirm("Start extraction");
                    
                    if (confirm) {
                        std::cout << std::endl;
                        std::cout << Colors::BLUE << Colors::BOLD << ">>> Starting extraction..." << Colors::RESET << std::endl;
                        printSeparator();
                        
                        bool success = job.execute();
                        
                        printSeparator();
                        std::cout << std::endl;
                        if (success) {
                            std::cout << Colors::GREEN << Colors::BOLD << "[OK] Thumbnails generated successfully!" << Colors::RESET << std::endl;
                        } else {
                            std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Extraction failed." << Colors::RESET << std::endl;
                        }
                    } else {
                        std::cout << std::endl;
                        std::cout << Colors::YELLOW << "[INFO] Extraction cancelled." << Colors::RESET << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            }
            break;
        }        
        
        case 6: { // SVT-AV1-Essential
            try {
                std::string inputFile, outputFile;
                int qualityChoice;
                bool aggressive, unshackle, cleanup;

                printHeader("ENCODE SVT-AV1-ESSENTIAL");
                std::cout << std::endl;

                // Files
                promptFiles(inputFile, outputFile, "(.mkv)");
                
                std::cout << std::endl;
                
                // Quality
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Quality :" << Colors::RESET << std::endl;
                printSeparator();
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " Low " << Colors::SUBTEXT << "- Fast, acceptable quality" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " Medium " << Colors::SUBTEXT << "- Balanced" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " High " << Colors::SUBTEXT << "- Slow, excellent quality" << Colors::RESET << std::endl;
                printSeparator();
                qualityChoice = Input::getIntRange("Your choice", 1, 3);
                
                std::cout << std::endl;
                
                // Advanced options
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Advanced options" << Colors::RESET << std::endl;
                printSeparator();
                aggressive = Input::getConfirm("Aggressive mode (more compression)");
                unshackle = Input::getConfirm("Unshackle (removes limits)");
                cleanup = Input::getConfirm("Cleanup temporary files");
                
                std::cout << std::endl;
                
                try {
                    SvtAv1EssentialBuilder builder;
                    builder.input(inputFile).output(outputFile);
                    
                    // Quality
                    switch (qualityChoice) {
                        case 1: builder.quality(SvtAv1EssentialJob::Quality::LOW); break;
                        case 2: builder.quality(SvtAv1EssentialJob::Quality::MEDIUM); break;
                        case 3: builder.quality(SvtAv1EssentialJob::Quality::HIGH); break;
                        default: builder.quality(SvtAv1EssentialJob::Quality::HIGH);
                    }
                    
                    // Options
                    if (aggressive) builder.aggressive();
                    if (unshackle) builder.unshackle();
                    builder.cleanup(cleanup);
                    
                    // Build and execute
                    SvtAv1EssentialJob job = builder.build();
                    
                    // Confirmation and execution (version without outputFile for SVT-AV1)
                    std::cout << std::endl;
                    bool confirm = Input::getConfirm("Start encoding");
                    
                    if (confirm) {
                        std::cout << std::endl;
                        bool success = job.execute();
                        
                        if (!success) {
                            std::cerr << std::endl;
                            std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Encoding failed." << Colors::RESET << std::endl;
                        }
                    } else {
                        std::cout << std::endl;
                        std::cout << Colors::YELLOW << "[INFO] Encoding cancelled." << Colors::RESET << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 7: {
            try {
                std::string inputFile;
                
                printHeader("ANALYZE MEDIA");
                std::cout << std::endl;
                
                // Ask for input file
                inputFile = Input::getString("Input file");
                std::cout << std::endl;
                
                // Create FFProbe job
                ::Jobs::ProbeJob job(inputFile);
                
                // Execute analysis
                printHeader("FFPROBE ANALYSIS");
                std::cout << std::endl;
                
                job.execute();
                
                std::cout << std::endl;
                
                // Ask if export
                bool exportChoice = Input::getConfirm("Export result");
                
                if (exportChoice) {
                    std::cout << std::endl;
                    
                    // Ask for format
                    std::cout << Colors::BLUE << "┌────────────────────────────────────────────┐" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "│" << Colors::TEXT << "  Export format :                          " << Colors::BLUE << "│" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "├────────────────────────────────────────────┤" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "│  " << Colors::MAUVE << "1" << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << "JSON (raw FFProbe output)        " << Colors::BLUE << "│" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "│  " << Colors::MAUVE << "2" << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << "TXT  (readable formatted output) " << Colors::BLUE << "│" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "└────────────────────────────────────────────┘" << Colors::RESET << std::endl;
                    std::cout << std::endl;
                    
                    int formatChoice = Input::getIntRange("Choice", 1, 2);
                    std::cout << std::endl;
                    
                    bool isJson = (formatChoice == 1);
                    std::string exportPath = job.generateExportPath(isJson);
                    
                    if (isJson) {
                        // Export raw JSON
                        job.writeToFile(exportPath, job.getOutput());
                        std::cout << Colors::GREEN << "[OK] Analysis exported (JSON) : " << Colors::TEAL << exportPath << Colors::RESET << std::endl;
                    } else {
                        // Export formatted text
                        job.writeToFile(exportPath, job.getFormattedOutput());
                        std::cout << Colors::GREEN << "[OK] Analysis exported (TXT)  : " << Colors::TEAL << exportPath << Colors::RESET << std::endl;
                    }
                } else {
                    std::cout << std::endl;
                    std::cout << Colors::SUBTEXT << "[INFO] Result not exported." << Colors::RESET << std::endl;
                }
                
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Back to main menu." << Colors::RESET << std::endl;
            } catch (const std::exception& e) {
                handleError(e);
            }
            
            break;
        }
        
        case 0: {
            std::cout << std::endl;
            std::cout << Colors::LAVENDER << "Goodbye !" << Colors::RESET << std::endl;
            return false;
        }
        
        default: {
            std::cout << std::endl;
            std::cout << Colors::YELLOW << "[WARN] Feature not yet implemented." << Colors::RESET << std::endl;
            break;
        }
    }
    return true;
}
} // namespace App
