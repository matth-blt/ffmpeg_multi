#include "../../include/jobs/thumbnails.hpp"
#include "../../include/core/ffmpeg_process.hpp"
#include "../../include/core/path_utils.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <iomanip>

namespace fs = std::filesystem;

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONSTRUCTORS
// ============================================================================

ThumbnailsJob::ThumbnailsJob(const ThumbnailsConfig& config) : config_(config) {}

// ============================================================================
// CONFIGURATION
// ============================================================================

void ThumbnailsJob::setConfig(const ThumbnailsConfig& config) {
    config_ = config;
}

ThumbnailsConfig& ThumbnailsJob::config() {
    return config_;
}

const ThumbnailsConfig& ThumbnailsJob::config() const {
    return config_;
}

// ============================================================================
// HELPERS
// ============================================================================

bool ThumbnailsJob::validatePaths() const {
    if (!fs::exists(config_.input_path)) {
        std::cerr << "Error: Input file does not exist: " << config_.input_path << std::endl;
        return false;
    }

    if (config_.output_dir.empty()) {
        std::cerr << "Error: Output directory is not defined." << std::endl;
        return false;
    }

    return true;
}

bool ThumbnailsJob::createOutputDirectory() const {
    try {
        std::string target_dir = config_.output_dir;
        
        if (config_.create_subfolder && !config_.subfolder_name.empty()) {
            target_dir = (fs::path(config_.output_dir) / config_.subfolder_name).string();
        }

        if (!fs::exists(target_dir)) {
            fs::create_directories(target_dir);
            std::cout << "Directory created: " << target_dir << std::endl;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

std::string ThumbnailsJob::getOutputPattern() const {
    std::string target_dir = config_.output_dir;
    
    if (config_.create_subfolder && !config_.subfolder_name.empty()) {
        target_dir = (fs::path(config_.output_dir) / config_.subfolder_name).string();
    }

    std::string extension = getFileExtension();
    return (fs::path(target_dir) / ("thumb_%08d" + extension)).string();
}

std::string ThumbnailsJob::getFileExtension() const {
    switch (config_.format) {
        case ThumbnailFormat::PNG:
            return ".png";
        case ThumbnailFormat::TIFF:
            return ".tiff";
        case ThumbnailFormat::JPEG:
            return ".jpg";
        default:
            return ".png";
    }
}

std::string ThumbnailsJob::getSceneFilter() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "select='gt(scene," << config_.scene_threshold << ")',showinfo";
    return oss.str();
}

// ============================================================================
// COMMAND CONSTRUCTION
// ============================================================================

std::vector<std::string> ThumbnailsJob::buildCommand() const {
    std::vector<std::string> args;

    // Global options
    args.push_back("-hide_banner");
    args.push_back("-i");
    args.push_back(config_.input_path);

    // Scaling and color conversion
    args.push_back("-sws_flags");
    args.push_back("spline+accurate_rnd+full_chroma_int");

    // Scene detection filter + showinfo
    args.push_back("-vf");
    args.push_back(getSceneFilter());

    // vsync vfr for variable frame rate (avoid duplications)
    args.push_back("-vsync");
    args.push_back("vfr");

    // Configuration according to format
    switch (config_.format) {
        case ThumbnailFormat::PNG:
            args.push_back("-color_trc");
            args.push_back("2");
            args.push_back("-colorspace");
            args.push_back("2");
            args.push_back("-color_primaries");
            args.push_back("2");
            args.push_back("-map");
            args.push_back("0:v");
            args.push_back("-c:v");
            args.push_back("png");
            args.push_back("-pix_fmt");
            args.push_back("rgb24");
            args.push_back("-start_number");
            args.push_back("0");
            break;

        case ThumbnailFormat::TIFF:
            args.push_back("-color_trc");
            args.push_back("1");
            args.push_back("-colorspace");
            args.push_back("1");
            args.push_back("-color_primaries");
            args.push_back("1");
            args.push_back("-map");
            args.push_back("0:v");
            args.push_back("-c:v");
            args.push_back("tiff");
            args.push_back("-pix_fmt");
            args.push_back("rgb24");
            args.push_back("-compression_algo");
            args.push_back("deflate");
            args.push_back("-start_number");
            args.push_back("0");
            args.push_back("-movflags");
            args.push_back("frag_keyframe+empty_moov+delay_moov+use_metadata_tags+write_colr");
            args.push_back("-bf");
            args.push_back("0");
            break;

        case ThumbnailFormat::JPEG:
            args.push_back("-color_trc");
            args.push_back("2");
            args.push_back("-colorspace");
            args.push_back("2");
            args.push_back("-color_primaries");
            args.push_back("2");
            args.push_back("-map");
            args.push_back("0:v");
            args.push_back("-c:v");
            args.push_back("mjpeg");
            args.push_back("-pix_fmt");
            args.push_back("yuvj420p");
            args.push_back("-q:v");
            args.push_back("1");
            args.push_back("-start_number");
            args.push_back("0");
            break;
    }

    // Output pattern
    args.push_back(getOutputPattern());

    return args;
}

std::string ThumbnailsJob::getCommandString() const {
    auto args = buildCommand();
    std::ostringstream oss;
    oss << "ffmpeg";
    for (const auto& arg : args) {
        oss << " ";
        if (arg.find(' ') != std::string::npos || arg.find('\'') != std::string::npos) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }
    return oss.str();
}

// ============================================================================
// EXECUTION
// ============================================================================

bool ThumbnailsJob::execute() {
    // Validation
    if (!validatePaths()) {
        return false;
    }

    // Directory creation
    if (!createOutputDirectory()) {
        return false;
    }

    // Command construction
    auto args = buildCommand();
    
    // Command log
    std::cout << "[INFO] Thumbnails extraction command: " << getCommandString() << std::endl;
    std::cout << "[INFO] Scene detection threshold: " << config_.scene_threshold << std::endl;
    
    // Execution via FFmpegProcess
    std::filesystem::path extern_path = FFmpegMulti::PathUtils::getExternPath();
    std::filesystem::path ffmpeg_path = extern_path / "ffmpeg.exe";
    
    ffmpegProcess ffmpeg(ffmpeg_path, args);
    bool success = ffmpeg.execute();

    if (success) {
        std::cout << "[SUCCESS] Thumbnails extraction completed successfully!" << std::endl;
        std::cout << "[INFO] Thumbnails extracted in: " << (config_.create_subfolder && !config_.subfolder_name.empty() 
            ? (fs::path(config_.output_dir) / config_.subfolder_name).string() : config_.output_dir) << std::endl;
        std::cout << "[INFO] Only images corresponding to scene changes were extracted." << std::endl;
    } else {
        std::cerr << "[ERROR] Thumbnails extraction failed!" << std::endl;
    }

    return success;
}

} // namespace Jobs
} // namespace FFmpegMulti
