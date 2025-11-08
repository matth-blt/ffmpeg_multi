#include "../../include/jobs/extract_frames.hpp"

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONFIGURATION
// ============================================================================

ExtractFramesBuilder& ExtractFramesBuilder::input(const std::string& path) {
    config_.input_path = path;
    return *this;
}

ExtractFramesBuilder& ExtractFramesBuilder::outputDir(const std::string& path) {
    config_.output_dir = path;
    return *this;
}

ExtractFramesBuilder& ExtractFramesBuilder::createSubfolder(bool create) {
    config_.create_subfolder = create;
    return *this;
}

ExtractFramesBuilder& ExtractFramesBuilder::subfolderName(const std::string& name) {
    config_.subfolder_name = name;
    return *this;
}

ExtractFramesBuilder& ExtractFramesBuilder::format(ImageFormat fmt) {
    config_.format = fmt;
    return *this;
}

// ============================================================================
// FORMAT SHORTCUTS
// ============================================================================

ExtractFramesBuilder& ExtractFramesBuilder::png() {
    config_.format = ImageFormat::PNG;
    return *this;
}

ExtractFramesBuilder& ExtractFramesBuilder::tiff() {
    config_.format = ImageFormat::TIFF;
    return *this;
}

ExtractFramesBuilder& ExtractFramesBuilder::jpeg() {
    config_.format = ImageFormat::JPEG;
    return *this;
}

// ============================================================================
// BUILD
// ============================================================================

ExtractFramesJob ExtractFramesBuilder::build() const {
    return ExtractFramesJob(config_);
}

} // namespace Jobs
} // namespace FFmpegMulti
