#include "../../include/jobs/encode.hpp"

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONFIGURATION DE BASE
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::inputDir(const std::string& path) {
    config_.input_dir = path;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::outputDir(const std::string& path) {
    config_.output_dir = path;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::outputFilename(const std::string& name) {
    config_.output_filename = name;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::inputPattern(const std::string& pattern) {
    config_.input_pattern = pattern;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::framerate(int fps) {
    config_.framerate = fps;
    return *this;
}

// ============================================================================
// FORMAT DE CONTENEUR
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::format(ContainerFormat fmt) {
    config_.format = fmt;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::mkv() {
    config_.format = ContainerFormat::MKV;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::webm() {
    config_.format = ContainerFormat::WEBM;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::mp4() {
    config_.format = ContainerFormat::MP4;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::mov() {
    config_.format = ContainerFormat::MOV;
    return *this;
}

// ============================================================================
// CODEC SHORTCUTS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::codec(Encode::Codec c) {
    config_.codec = c;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::x264() {
    config_.codec = Encode::Codec::X264;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::x265() {
    config_.codec = Encode::Codec::X265;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::av1() {
    config_.codec = Encode::Codec::AV1;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::h264_nvenc() {
    config_.codec = Encode::Codec::H264_NVENC;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::h265_nvenc() {
    config_.codec = Encode::Codec::H265_NVENC;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::prores() {
    config_.codec = Encode::Codec::ProRes;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::ffv1() {
    config_.codec = Encode::Codec::FFV1;
    return *this;
}

// ============================================================================
// PARAMÈTRES D'ENCODAGE
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::quality(int crf) {
    config_.quality = crf;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::preset(const std::string& p) {
    config_.preset = p;
    return *this;
}

// ============================================================================
// BUILD
// ============================================================================

EncodeJob EncodeJobBuilder::build() const {
    return EncodeJob(config_);
}

} // namespace Jobs
} // namespace FFmpegMulti