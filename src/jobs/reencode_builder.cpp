#include "../../include/jobs/reencode_builder.hpp"
#include <stdexcept>

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// I/O PATHS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::input(const std::string& path) {
    input_path_ = path;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::output(const std::string& path) {
    output_path_ = path;
    return *this;
}

// ============================================================================
// CODEC SHORTCUTS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::x264() {
    config_.codec = Encode::Codec::X264;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::x265() {
    config_.codec = Encode::Codec::X265;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::hevc() {
    return x265(); // Alias
}

ReencodeJobBuilder& ReencodeJobBuilder::av1() {
    config_.codec = Encode::Codec::AV1;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::svtav1() {
    config_.codec = Encode::Codec::SVT_AV1;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::prores() {
    config_.codec = Encode::Codec::ProRes;
    // Paramètres par défaut pour ProRes 4444
    config_.prores_profile = 4;
    config_.prores_vendor = "apl0";
    config_.bits_per_mb = 8000;
    config_.pixel_format = Encode::PixelFormat::YUVA444P10LE;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::ffv1() {
    config_.codec = Encode::Codec::FFV1;
    // Paramètres par défaut pour FFV1
    config_.ffv1_coder = 2;
    config_.ffv1_context = 1;
    config_.ffv1_level = 3;
    config_.ffv1_slices = 12;
    config_.gop_size = 1; // Intra-only pour lossless
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::h264_nvenc() {
    config_.codec = Encode::Codec::H264_NVENC;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::h265_nvenc() {
    config_.codec = Encode::Codec::H265_NVENC;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::codec(Encode::Codec c) {
    config_.codec = c;
    return *this;
}

// ============================================================================
// RATE CONTROL
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::crf(int value) {
    config_.rate_control = Encode::RateControl::CRF;
    config_.quality = value;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::qp(int value) {
    config_.rate_control = Encode::RateControl::CQP;
    config_.quality = value;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::bitrate(int kbps) {
    config_.rate_control = Encode::RateControl::VBR;
    config_.bitrate_kbps = kbps;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::cbr(int kbps) {
    config_.rate_control = Encode::RateControl::CBR;
    config_.bitrate_kbps = kbps;
    config_.buffer_size_kbps = kbps * 2; // Buffer = 2x bitrate par défaut
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::vbr(int kbps) {
    return bitrate(kbps);
}

// ============================================================================
// ENCODING PARAMETERS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::preset(const std::string& p) {
    config_.preset = p;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::tune(const std::string& t) {
    config_.tune = t;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::gopSize(int frames) {
    config_.gop_size = frames;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::bframes(int count) {
    config_.bframes = count;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::threads(int count) {
    config_.threads = count;
    return *this;
}

// ============================================================================
// PIXEL FORMAT SHORTCUTS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::eightBit() {
    config_.pixel_format = Encode::PixelFormat::YUV420P8;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::tenBit() {
    config_.pixel_format = Encode::PixelFormat::YUV420P10;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::pixelFormat(Encode::PixelFormat fmt) {
    config_.pixel_format = fmt;
    return *this;
}

// ============================================================================
// PRORES SPECIFIC PARAMETERS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::proresProfile(int profile) {
    config_.prores_profile = profile;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::proresVendor(const std::string& vendor) {
    config_.prores_vendor = vendor;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::proresBitsPerMB(int bits) {
    config_.bits_per_mb = bits;
    return *this;
}

// ============================================================================
// FFV1 SPECIFIC PARAMETERS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::ffv1Coder(int coder) {
    config_.ffv1_coder = coder;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::ffv1Context(int context) {
    config_.ffv1_context = context;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::ffv1Level(int level) {
    config_.ffv1_level = level;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::ffv1Slices(int slices) {
    config_.ffv1_slices = slices;
    return *this;
}

// ============================================================================
// X264 SPECIFIC PARAMETERS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::x264Params(const std::string& params) {
    config_.x264_params = params;
    return *this;
}

// ============================================================================
// NVENC SPECIFIC PARAMETERS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::nvencBAdapt(int value) {
    config_.b_adapt = value;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::nvencRcLookahead(int frames) {
    config_.rc_lookahead = frames;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::nvencQpCbOffset(int offset) {
    config_.qp_cb_offset = offset;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::nvencQpCrOffset(int offset) {
    config_.qp_cr_offset = offset;
    return *this;
}

// ============================================================================
// COLOR SPACE PRESETS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::sdr() {
    config_.passthrough_color = false;
    config_.color_profile.primaries = Encode::ColorPrimaries::BT709;
    config_.color_profile.transfer = Encode::TransferCharacteristic::BT1886;
    config_.color_profile.matrix = Encode::MatrixCoefficients::BT709;
    config_.color_profile.range = Encode::Range::Limited;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::hdr10() {
    config_.passthrough_color = false;
    config_.color_profile.primaries = Encode::ColorPrimaries::BT2020;
    config_.color_profile.transfer = Encode::TransferCharacteristic::PQ;
    config_.color_profile.matrix = Encode::MatrixCoefficients::BT2020NCL;
    config_.color_profile.range = Encode::Range::Limited;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::hlg() {
    config_.passthrough_color = false;
    config_.color_profile.primaries = Encode::ColorPrimaries::BT2020;
    config_.color_profile.transfer = Encode::TransferCharacteristic::HLG;
    config_.color_profile.matrix = Encode::MatrixCoefficients::BT2020NCL;
    config_.color_profile.range = Encode::Range::Limited;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::colorSpace(
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

ReencodeJobBuilder& ReencodeJobBuilder::maxCLL(uint16_t max_cll, uint16_t max_fall) {
    Encode::ContentLightLevel cll;
    cll.max_cll = max_cll;
    cll.max_fall = max_fall;
    config_.content_light_level = cll;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::masteringDisplay(float rx, float ry, 
    float gx, float gy, 
    float bx, float by, 
    float wx, float wy, 
    float min_lum, float max_lum) 
{
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

ReencodeJobBuilder& ReencodeJobBuilder::copyAudio() {
    config_.audio.copy_audio = true;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::audioCodec(const std::string& codec) {
    config_.audio.copy_audio = false;
    config_.audio.codec = codec;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::audioBitrate(int kbps) {
    config_.audio.bitrate_kbps = kbps;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::audioSampleRate(int hz) {
    config_.audio.sample_rate = hz;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::audioChannels(int channels) {
    config_.audio.channels = channels;
    return *this;
}

// ============================================================================
// COMPLETE PRESETS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::youtubePreset() {
    x264();
    crf(23);
    preset("medium");
    eightBit();
    copyAudio();
    config_.container = "mp4";
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::x264Preset() {
    x264();
    crf(16);
    preset("slow");
    x264Params("direct=spatial:me=umh");
    pixelFormat(Encode::PixelFormat::YUV420P8);
    copyAudio();
    config_.container = "mp4";
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::x265Preset() {
    x265();
    crf(18);
    preset("medium");
    pixelFormat(Encode::PixelFormat::YUV420P8);
    copyAudio();
    config_.container = "mp4";
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::h264NvencPreset() {
    h264_nvenc();
    qp(18);
    preset("p6");
    nvencBAdapt(1);
    nvencRcLookahead(30);
    nvencQpCbOffset(-2);
    nvencQpCrOffset(-2);
    pixelFormat(Encode::PixelFormat::NV12);
    copyAudio();
    config_.container = "mp4";
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::h265NvencPreset() {
    h265_nvenc();
    qp(18);
    preset("p6");
    nvencBAdapt(1);
    nvencRcLookahead(30);
    nvencQpCbOffset(-2);
    nvencQpCrOffset(-2);
    pixelFormat(Encode::PixelFormat::NV12);
    copyAudio();
    config_.container = "mp4";
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::proresPreset(int profile) {
    prores();
    proresProfile(profile);
    proresVendor("apl0");
    proresBitsPerMB(8000);
    pixelFormat(Encode::PixelFormat::YUVA444P10LE);
    copyAudio();
    config_.container = "mov";
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::ffv1Preset() {
    ffv1();
    ffv1Coder(2);
    ffv1Context(1);
    ffv1Level(3);
    ffv1Slices(12);
    gopSize(1);
    copyAudio();
    config_.container = "mkv";
    return *this;
}

// ============================================================================
// ADVANCED OPTIONS
// ============================================================================

ReencodeJobBuilder& ReencodeJobBuilder::container(const std::string& ext) {
    config_.container = ext;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::extraArgs(const std::vector<std::string>& args) {
    config_.extra_args = args;
    return *this;
}

ReencodeJobBuilder& ReencodeJobBuilder::addExtraArg(const std::string& arg) {
    config_.extra_args.push_back(arg);
    return *this;
}

// ============================================================================
// VALIDATION
// ============================================================================

void ReencodeJobBuilder::validate() const {
    if (input_path_.empty()) {
        throw std::runtime_error("Input path is required");
    }
    
    if (output_path_.empty()) {
        throw std::runtime_error("Output path is required");
    }
    
    // Validation du rate control
    if (config_.rate_control == Encode::RateControl::VBR || config_.rate_control == Encode::RateControl::CBR) {
        if (config_.bitrate_kbps <= 0) {
            throw std::runtime_error("Bitrate must be > 0 for VBR/CBR modes");
        }
    }
    
    // Validation du quality pour CRF/CQP
    if (config_.rate_control == Encode::RateControl::CRF || config_.rate_control == Encode::RateControl::CQP) {
        if (config_.quality < 0 || config_.quality > 51) {
            throw std::runtime_error("Quality/CRF value must be between 0 and 51");
        }
    }
}

// ============================================================================
// BUILD
// ============================================================================

ReencodeJob ReencodeJobBuilder::build() {
    validate();
    
    ReencodeJob job(input_path_, output_path_);
    job.setConfig(config_);
    
    return job;
}

} // namespace Jobs
} // namespace FFmpegMulti
