#include "../../include/jobs/encode_builder.hpp"
#include <stdexcept>

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// I/O PATHS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::input(const std::string& path) {
    input_path_ = path;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::output(const std::string& path) {
    output_path_ = path;
    return *this;
}

// ============================================================================
// CODEC SHORTCUTS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::h264() {
    config_.codec = Encode::Codec::X264;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::h265() {
    config_.codec = Encode::Codec::X265;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::hevc() {
    return h265(); // Alias
}

EncodeJobBuilder& EncodeJobBuilder::av1() {
    config_.codec = Encode::Codec::AV1;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::svtav1() {
    config_.codec = Encode::Codec::SVT_AV1;
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

EncodeJobBuilder& EncodeJobBuilder::h264_nvenc() {
    config_.codec = Encode::Codec::H264_NVENC;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::h265_nvenc() {
    config_.codec = Encode::Codec::H265_NVENC;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::codec(Encode::Codec c) {
    config_.codec = c;
    return *this;
}

// ============================================================================
// RATE CONTROL
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::crf(int value) {
    config_.rate_control = Encode::RateControl::CRF;
    config_.quality = value;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::qp(int value) {
    config_.rate_control = Encode::RateControl::CQP;
    config_.quality = value;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::bitrate(int kbps) {
    config_.rate_control = Encode::RateControl::VBR;
    config_.bitrate_kbps = kbps;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::cbr(int kbps) {
    config_.rate_control = Encode::RateControl::CBR;
    config_.bitrate_kbps = kbps;
    config_.buffer_size_kbps = kbps * 2; // Buffer = 2x bitrate par défaut
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::vbr(int kbps) {
    return bitrate(kbps);
}

// ============================================================================
// ENCODING PARAMETERS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::preset(const std::string& p) {
    config_.preset = p;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::tune(const std::string& t) {
    config_.tune = t;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::gopSize(int frames) {
    config_.gop_size = frames;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::bframes(int count) {
    config_.bframes = count;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::threads(int count) {
    config_.threads = count;
    return *this;
}

// ============================================================================
// PIXEL FORMAT SHORTCUTS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::eightBit() {
    config_.pixel_format = Encode::PixelFormat::YUV420P8;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::tenBit() {
    config_.pixel_format = Encode::PixelFormat::YUV420P10;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::pixelFormat(Encode::PixelFormat fmt) {
    config_.pixel_format = fmt;
    return *this;
}

// ============================================================================
// COLOR SPACE PRESETS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::sdr() {
    config_.passthrough_color = false;
    config_.color_profile.primaries = Encode::ColorPrimaries::BT709;
    config_.color_profile.transfer = Encode::TransferCharacteristic::BT1886;
    config_.color_profile.matrix = Encode::MatrixCoefficients::BT709;
    config_.color_profile.range = Encode::Range::Limited;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::hdr10() {
    config_.passthrough_color = false;
    config_.color_profile.primaries = Encode::ColorPrimaries::BT2020;
    config_.color_profile.transfer = Encode::TransferCharacteristic::PQ;
    config_.color_profile.matrix = Encode::MatrixCoefficients::BT2020NCL;
    config_.color_profile.range = Encode::Range::Limited;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::hlg() {
    config_.passthrough_color = false;
    config_.color_profile.primaries = Encode::ColorPrimaries::BT2020;
    config_.color_profile.transfer = Encode::TransferCharacteristic::HLG;
    config_.color_profile.matrix = Encode::MatrixCoefficients::BT2020NCL;
    config_.color_profile.range = Encode::Range::Limited;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::colorSpace(
    Encode::ColorPrimaries primaries,
    Encode::TransferCharacteristic transfer,
    Encode::MatrixCoefficients matrix,
    Encode::Range range
) {
    config_.passthrough_color = false;
    config_.color_profile.primaries = primaries;
    config_.color_profile.transfer = transfer;
    config_.color_profile.matrix = matrix;
    config_.color_profile.range = range;
    return *this;
}

// ============================================================================
// HDR METADATA
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::maxCLL(uint16_t max_cll, uint16_t max_fall) {
    Encode::ContentLightLevel cll;
    cll.max_cll = max_cll;
    cll.max_fall = max_fall;
    config_.content_light_level = cll;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::masteringDisplay(
    float rx, float ry, float gx, float gy,
    float bx, float by, float wx, float wy,
    float min_lum, float max_lum
) {
    Encode::MasteringDisplay md;
    md.red_x = rx; md.red_y = ry;
    md.green_x = gx; md.green_y = gy;
    md.blue_x = bx; md.blue_y = by;
    md.white_x = wx; md.white_y = wy;
    md.min_luminance = min_lum;
    md.max_luminance = max_lum;
    config_.mastering_display = md;
    return *this;
}

// ============================================================================
// AUDIO
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::copyAudio() {
    config_.audio.copy_audio = true;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::audioCodec(const std::string& codec) {
    config_.audio.copy_audio = false;
    config_.audio.codec = codec;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::audioBitrate(int kbps) {
    config_.audio.bitrate_kbps = kbps;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::audioSampleRate(int hz) {
    config_.audio.sample_rate = hz;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::audioChannels(int channels) {
    config_.audio.channels = channels;
    return *this;
}

// ============================================================================
// COMPLETE PRESETS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::youtubePreset() {
    h264();
    crf(23);
    preset("medium");
    eightBit();
    copyAudio();
    config_.container = "mp4";
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::archivePreset() {
    h265();
    crf(18);
    preset("slow");
    tenBit();
    gopSize(250);
    bframes(4);
    audioCodec("flac");
    config_.container = "mkv";
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::webPreset() {
    svtav1();
    crf(30);
    preset("5");
    eightBit();
    audioCodec("libopus");
    audioBitrate(128);
    config_.container = "webm";
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::streamingPreset(int bitrate_kbps) {
    h264();
    cbr(bitrate_kbps);
    preset("veryfast");
    tune("zerolatency");
    gopSize(60);
    bframes(0);
    eightBit();
    audioCodec("aac");
    audioBitrate(128);
    config_.container = "mp4";
    return *this;
}

// ============================================================================
// ADVANCED OPTIONS
// ============================================================================

EncodeJobBuilder& EncodeJobBuilder::container(const std::string& ext) {
    config_.container = ext;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::extraArgs(const std::vector<std::string>& args) {
    config_.extra_args = args;
    return *this;
}

EncodeJobBuilder& EncodeJobBuilder::addExtraArg(const std::string& arg) {
    config_.extra_args.push_back(arg);
    return *this;
}

// ============================================================================
// VALIDATION
// ============================================================================

void EncodeJobBuilder::validate() const {
    if (input_path_.empty()) {
        throw std::runtime_error("Input path is required");
    }
    
    if (output_path_.empty()) {
        throw std::runtime_error("Output path is required");
    }
    
    // Validation du rate control
    if (config_.rate_control == Encode::RateControl::VBR || 
        config_.rate_control == Encode::RateControl::CBR) {
        if (config_.bitrate_kbps <= 0) {
            throw std::runtime_error("Bitrate must be > 0 for VBR/CBR modes");
        }
    }
    
    // Validation du quality pour CRF/CQP
    if (config_.rate_control == Encode::RateControl::CRF || 
        config_.rate_control == Encode::RateControl::CQP) {
        if (config_.quality < 0 || config_.quality > 51) {
            throw std::runtime_error("Quality/CRF value must be between 0 and 51");
        }
    }
}

// ============================================================================
// BUILD
// ============================================================================

EncodeJob EncodeJobBuilder::build() {
    validate();
    
    EncodeJob job(input_path_, output_path_);
    job.setConfig(config_);
    
    return job;
}

} // namespace Jobs
} // namespace FFmpegMulti
