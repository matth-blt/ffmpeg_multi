#include "../../include/jobs/svt_av1_essential.hpp"
#include "../../include/core/path_utils.hpp"
#include "../../include/core/colors.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONSTRUCTOR
// ============================================================================

SvtAv1EssentialJob::SvtAv1EssentialJob(const std::string& input, const std::string& output) {
    config_.input_path = input;
    config_.output_path = output;
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void SvtAv1EssentialJob::setQuality(Quality q) {
    config_.quality = q;
}

void SvtAv1EssentialJob::setAggressive(bool enabled) {
    config_.aggressive = enabled;
}

void SvtAv1EssentialJob::setUnshackle(bool enabled) {
    config_.unshackle = enabled;
}

void SvtAv1EssentialJob::setVerbose(bool enabled) {
    config_.verbose = enabled;
}

void SvtAv1EssentialJob::setCleanup(bool enabled) {
    config_.cleanup = enabled;
}

SvtAv1EssentialJob::Config& SvtAv1EssentialJob::config() {
    return config_;
}

const SvtAv1EssentialJob::Config& SvtAv1EssentialJob::config() const {
    return config_;
}

// ============================================================================
// HELPERS
// ============================================================================

std::string SvtAv1EssentialJob::getQualityString() const {
    switch (config_.quality) {
        case Quality::LOW: return "low";
        case Quality::MEDIUM: return "medium";
        case Quality::HIGH: return "high";
        default: return "high";
    }
}

std::filesystem::path SvtAv1EssentialJob::getTempDir() const {
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::string stem = input.stem().string();
    return parent / stem; // Folder with the same name as the video
}

std::filesystem::path SvtAv1EssentialJob::getAviPath() const {
    // Auto-Boost generates the .ivf next to the source video, not in the subfolder
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::string stem = input.stem().string();
    return parent / (stem + ".ivf");
}

std::filesystem::path SvtAv1EssentialJob::getAudioPath() const {
    // Put the audio next to the source video, not in the folder that will be overwritten by Auto-Boost
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::string stem = input.stem().string();
    return parent / (stem + "_audio.mka");
}

std::string SvtAv1EssentialJob::buildABECommand() const {
    std::ostringstream cmd;
    
    std::filesystem::path extern_path = PathUtils::getExternPath();
    std::filesystem::path abe_script = extern_path / "scripts" / "ABE.ps1";
    
    // Convert paths with forward slashes
    std::string abe_str = abe_script.string();
    std::string input_str = config_.input_path;
    
    std::replace(abe_str.begin(), abe_str.end(), '\\', '/');
    std::replace(input_str.begin(), input_str.end(), '\\', '/');
    
    // PowerShell command
    cmd << "powershell -ExecutionPolicy Bypass -File \"" << abe_str << "\"";
    cmd << " -inputFile \"" << input_str << "\"";
    cmd << " -quality " << getQualityString();
    
    if (config_.aggressive)
        cmd << " -aggressive";
    if (config_.unshackle)
        cmd << " -unshackle";
    if (config_.verbose)
        cmd << " -verboseOutput";
    
    return cmd.str();
}

// ============================================================================
// VALIDATION
// ============================================================================

bool SvtAv1EssentialJob::validatePaths() {
    // Check that the input file exists
    if (!std::filesystem::exists(config_.input_path)) {
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Input file does not exist: "
        << Colors::RESET << Colors::RED << config_.input_path << Colors::RESET << std::endl;
        return false;
    }
    
    // Check that the output directory exists
    std::filesystem::path output(config_.output_path);
    std::filesystem::path output_dir = output.parent_path();
    
    if (!output_dir.empty() && !std::filesystem::exists(output_dir)) {
        std::cout << Colors::YELLOW << "[INFO] Creating output directory: "
        << Colors::TEXT << output_dir << Colors::RESET << std::endl;
        std::filesystem::create_directories(output_dir);
    }
    
    return true;
}

// ============================================================================
// STEP 1: AUDIO EXTRACTION
// ============================================================================

bool SvtAv1EssentialJob::extractAudio() {
    std::cout << std::endl;
    std::cout << Colors::SAPPHIRE << Colors::BOLD << "[STEP 1/4] Extracting audio..." << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "────────────────────────────────────────────" << Colors::RESET << std::endl;
    
    // Create temporary directory
    std::filesystem::path temp_dir = getTempDir();
    if (!std::filesystem::exists(temp_dir)) {
        std::filesystem::create_directories(temp_dir);
    }
    
    std::filesystem::path audio_path = getAudioPath();
    std::filesystem::path extern_path = PathUtils::getExternPath();
    std::filesystem::path ffmpeg_exe = extern_path / "ffmpeg.exe";
    
    // Convert paths to string with normal slashes for the command
    std::string ffmpeg_str = ffmpeg_exe.string();
    std::string input_str = config_.input_path;
    std::string audio_str = audio_path.string();
    
    // Replace backslashes with forward slashes
    std::replace(ffmpeg_str.begin(), ffmpeg_str.end(), '\\', '/');
    std::replace(input_str.begin(), input_str.end(), '\\', '/');
    std::replace(audio_str.begin(), audio_str.end(), '\\', '/');
    
    // FFmpeg command to extract audio
    std::ostringstream cmd;
    cmd << ffmpeg_str
        << " -i \"" << input_str << "\""
        << " -vn" // No video
        << " -c:a copy" // Copy audio without re-encoding
        << " \"" << audio_str << "\""
        << " -y"; // Overwrite if exists
    
    std::cout << Colors::SUBTEXT << "[CMD] " << cmd.str() << Colors::RESET << std::endl;
    
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Audio extraction failed" << Colors::RESET << std::endl;
        return false;
    }
    
    std::cout << Colors::GREEN << "[OK] Audio extracted: " << Colors::TEXT << audio_path << Colors::RESET << std::endl;
    return true;
}

// ============================================================================
// STEP 2: AUTO-BOOST ENCODING
// ============================================================================

bool SvtAv1EssentialJob::runAutoBoost() {
    std::cout << std::endl;
    std::cout << Colors::SAPPHIRE << Colors::BOLD << "[STEP 2/4] SVT-AV1 Encoding via Auto-Boost..." << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "────────────────────────────────────────────" << Colors::RESET << std::endl;
    
    std::string cmd = buildABECommand();
    std::cout << Colors::SUBTEXT << "[CMD] " << cmd << Colors::RESET << std::endl;
    std::cout << std::endl;
    std::cout << Colors::PEACH << "⏳ Encoding in progress (this may take a while)..." << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    int result = std::system(cmd.c_str());
    
    if (result != 0) {
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Auto-Boost encoding failed" << Colors::RESET << std::endl;
        return false;
    }
    
    // Check that the IVF file was created
    std::filesystem::path ivf_path = getAviPath();
    if (!std::filesystem::exists(ivf_path)) {
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] IVF file was not generated: " 
                  << Colors::RESET << Colors::RED << ivf_path << Colors::RESET << std::endl;
        return false;
    }
    
    std::cout << Colors::GREEN << "[OK] Encoding finished: " << Colors::TEXT << ivf_path << Colors::RESET << std::endl;
    return true;
}

// ============================================================================
// STEP 3: FINAL MUXING
// ============================================================================

bool SvtAv1EssentialJob::muxFinal() {
    std::cout << std::endl;
    std::cout << Colors::SAPPHIRE << Colors::BOLD << "[STEP 3/4] Final Muxing (AV1 video + audio)..." << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "────────────────────────────────────────────" << Colors::RESET << std::endl;
    
    std::filesystem::path ivf_path = getAviPath();
    std::filesystem::path audio_path = getAudioPath();
    
    // Create output_temp.mkv next to the source video, not in the subfolder
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::filesystem::path temp_mkv = parent / "output_temp.mkv";
    
    std::filesystem::path extern_path = PathUtils::getExternPath();
    std::filesystem::path mkvmerge_exe = extern_path / "env" / "mkvtoolnix" / "mkvmerge.exe";
    
    // Convert paths to string with normal slashes
    std::string mkvmerge_str = mkvmerge_exe.string();
    std::string temp_mkv_str = temp_mkv.string();
    std::string ivf_str = ivf_path.string();
    std::string audio_str = audio_path.string();
    
    std::replace(mkvmerge_str.begin(), mkvmerge_str.end(), '\\', '/');
    std::replace(temp_mkv_str.begin(), temp_mkv_str.end(), '\\', '/');
    std::replace(ivf_str.begin(), ivf_str.end(), '\\', '/');
    std::replace(audio_str.begin(), audio_str.end(), '\\', '/');
    
    // mkvmerge command to merge video and audio
    std::ostringstream cmd;
    cmd << mkvmerge_str
        << " -o \"" << temp_mkv_str << "\""
        << " \"" << ivf_str << "\""
        << " \"" << audio_str << "\"";
    
    std::cout << Colors::SUBTEXT << "[CMD] " << cmd.str() << Colors::RESET << std::endl;
    
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Muxing failed" << Colors::RESET << std::endl;
        return false;
    }
    
    // Move the final file to the destination
    try {
        std::filesystem::copy_file(
            temp_mkv, 
            config_.output_path,
            std::filesystem::copy_options::overwrite_existing
        );
        std::cout << Colors::GREEN << "[OK] Final file created: " << Colors::TEXT << config_.output_path << Colors::RESET << std::endl;
    } catch (const std::exception& e) {
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Failed to copy final file: " 
                  << Colors::RESET << Colors::RED << e.what() << Colors::RESET << std::endl;
        return false;
    }
    
    return true;
}

// ============================================================================
// STEP 4: CLEANUP
// ============================================================================

bool SvtAv1EssentialJob::cleanup() {
    if (!config_.cleanup) {
        std::cout << std::endl;
        std::cout << Colors::YELLOW << "[STEP 4/4] Cleanup disabled, temporary files kept." << Colors::RESET << std::endl;
        return true;
    }
    
    std::cout << std::endl;
    std::cout << Colors::SAPPHIRE << Colors::BOLD << "[STEP 4/4] Cleaning up temporary files..." << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "────────────────────────────────────────────" << Colors::RESET << std::endl;
    
    std::filesystem::path temp_dir = getTempDir();
    
    try {
        if (std::filesystem::exists(temp_dir)) {
            std::filesystem::remove_all(temp_dir);
            std::cout << Colors::GREEN << "[OK] Temporary directory deleted: " << Colors::TEXT << temp_dir << Colors::RESET << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << Colors::YELLOW << "[WARNING] Cannot delete temporary files: " 
                  << Colors::RESET << Colors::YELLOW << e.what() << Colors::RESET << std::endl;
        return false;
    }
    
    return true;
}

// ============================================================================
// MAIN EXECUTION
// ============================================================================

bool SvtAv1EssentialJob::execute() {
    const int TOTAL_WIDTH = 60;
    const int INNER_WIDTH = TOTAL_WIDTH - 2;
    std::string title = "SVT-AV1-ESSENTIAL via Auto-Boost";
    
    // Calculate title length (approx UTF-8)
    int titleLen = 0;
    for(char c : title) if((c & 0xC0) != 0x80) titleLen++;
    
    int padding = (std::max)(0, INNER_WIDTH - titleLen);
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;

    std::cout << std::endl;
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
    
    std::cout << std::endl;
    std::cout << Colors::SAPPHIRE << ":: Configuration :" << Colors::RESET << std::endl;
    std::cout << Colors::TEAL << "  • Input     : " << Colors::TEXT << config_.input_path << Colors::RESET << std::endl;
    std::cout << Colors::TEAL << "  • Output    : " << Colors::TEXT << config_.output_path << Colors::RESET << std::endl;
    std::cout << Colors::TEAL << "  • Quality   : " << Colors::TEXT << getQualityString() << Colors::RESET << std::endl;
    std::cout << Colors::TEAL << "  • Aggressive: " << Colors::TEXT << (config_.aggressive ? "Yes" : "No") << Colors::RESET << std::endl;
    std::cout << Colors::TEAL << "  • Unshackle : " << Colors::TEXT << (config_.unshackle ? "Yes" : "No") << Colors::RESET << std::endl;
    std::cout << Colors::TEAL << "  • Cleanup   : " << Colors::TEXT << (config_.cleanup ? "Yes" : "No") << Colors::RESET << std::endl;
    
    try {
        // Validation
        if (!validatePaths())
            return false;
        // Step 1: Audio extraction
        if (!extractAudio())
            return false;
        // Step 2: Auto-Boost encoding
        if (!runAutoBoost())
            return false;
        // Step 3: Final muxing
        if (!muxFinal())
            return false;
        // Step 4: Cleanup
        cleanup();
        
        std::cout << std::endl;
        std::cout << Colors::BLUE;
        for(int i = 0; i < TOTAL_WIDTH; i++)
            std::cout << "─";
        std::cout << Colors::RESET << std::endl;
        
        std::cout << Colors::GREEN << Colors::BOLD << "[OK] ENCODING COMPLETED SUCCESSFULLY" << Colors::RESET << std::endl;
        std::cout << Colors::TEAL << "> Output file: " << Colors::TEXT << config_.output_path << Colors::RESET << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << Colors::RED << Colors::BOLD << "[ERROR] Exception: " << Colors::RESET << Colors::RED << e.what() << Colors::RESET << std::endl;
        return false;
    }
}

// ============================================================================
// BUILDER IMPLEMENTATION
// ============================================================================

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::input(const std::string& path) {
    config_.input_path = path;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::output(const std::string& path) {
    config_.output_path = path;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::quality(SvtAv1EssentialJob::Quality q) {
    config_.quality = q;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::aggressive() {
    config_.aggressive = true;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::unshackle() {
    config_.unshackle = true;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::verbose(bool enabled) {
    config_.verbose = enabled;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::cleanup(bool enabled) {
    config_.cleanup = enabled;
    return *this;
}

SvtAv1EssentialJob SvtAv1EssentialBuilder::build() {
    if (config_.input_path.empty())
        throw std::runtime_error("Input path is required");
    if (config_.output_path.empty())
        throw std::runtime_error("Output path is required");
    
    SvtAv1EssentialJob job(config_.input_path, config_.output_path);
    job.config() = config_;
    return job;
}

} // namespace Jobs
} // namespace FFmpegMulti
