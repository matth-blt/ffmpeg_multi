#include "../../include/jobs/thumbnails.hpp"
#include <stdexcept>

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// BUILDER - CONFIGURATION
// ============================================================================

ThumbnailsBuilder& ThumbnailsBuilder::input(const std::string& path) {
    config_.input_path = path;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::outputDir(const std::string& path) {
    config_.output_dir = path;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::createSubfolder(bool create) {
    config_.create_subfolder = create;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::subfolderName(const std::string& name) {
    config_.subfolder_name = name;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::format(ThumbnailFormat fmt) {
    config_.format = fmt;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::sceneThreshold(float threshold) {
    if (threshold < 0.0f || threshold > 1.0f) {
        throw std::invalid_argument("Scene threshold must be between 0.0 and 1.0");
    }
    config_.scene_threshold = threshold;
    return *this;
}

// ============================================================================
// BUILDER - SHORTCUTS
// ============================================================================

ThumbnailsBuilder& ThumbnailsBuilder::png() {
    config_.format = ThumbnailFormat::PNG;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::tiff() {
    config_.format = ThumbnailFormat::TIFF;
    return *this;
}

ThumbnailsBuilder& ThumbnailsBuilder::jpeg() {
    config_.format = ThumbnailFormat::JPEG;
    return *this;
}

// ============================================================================
// BUILDER - BUILD
// ============================================================================

ThumbnailsJob ThumbnailsBuilder::build() const {
    // Validation de base
    if (config_.input_path.empty()) {
        throw std::invalid_argument("Input path is required");
    }
    
    if (config_.output_dir.empty()) {
        throw std::invalid_argument("Output directory is required");
    }
    
    return ThumbnailsJob(config_);
}

} // namespace Jobs
} // namespace FFmpegMulti
