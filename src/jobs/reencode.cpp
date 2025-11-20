#include "../../include/jobs/reencode.hpp"
#include "../../include/jobs/codec_utils.hpp"
#include "../../include/core/ffmpeg_process.hpp"
#include "../../include/core/path_utils.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONSTRUCTORS
// ============================================================================

ReencodeJob::ReencodeJob(const std::string& input_path, const std::string& output_path): input_path_(input_path), output_path_(output_path) {}

// ============================================================================
// CONFIGURATION
// ============================================================================

void ReencodeJob::setConfig(const Encode::EncodeConfig& config) {
    config_ = config;
}

Encode::EncodeConfig& ReencodeJob::config() {
    return config_;
}

const Encode::EncodeConfig& ReencodeJob::config() const {
    return config_;
}

// ============================================================================
// I/O PATHS
// ============================================================================

void ReencodeJob::setInputPath(const std::string& path) { 
    input_path_ = path; 
}

void ReencodeJob::setOutputPath(const std::string& path) { 
    output_path_ = path; 
}

std::string ReencodeJob::getInputPath() const { 
    return input_path_; 
}

std::string ReencodeJob::getOutputPath() const { 
    return output_path_; 
}

// ============================================================================
// COMMAND CONSTRUCTION
// ============================================================================

std::vector<std::string> ReencodeJob::buildCommand() const {
    std::vector<std::string> args;
    
    // Standard FFmpeg order:
    // [global options] -i input [video options] [audio options] output
    
    addInputArgs(args);
    addVideoCodecArgs(args);
    addRateControlArgs(args);
    addEncodingParams(args);
    addPixelFormatArgs(args);
    addColorSpaceArgs(args);
    addHDRMetadata(args);
    addAudioArgs(args);
    
    // Custom extra arguments
    for (const auto& extra : config_.extra_args) {
        args.push_back(extra);
    }
    
    addOutputArgs(args);
    
    return args;
}

std::string ReencodeJob::getCommandString() const {
    auto args = buildCommand();
    std::ostringstream oss;
    oss << "ffmpeg";
    for (const auto& arg : args) {
        oss << " ";
        if (arg.find(' ') != std::string::npos) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }
    return oss.str();
}

// ============================================================================
// ADD ARGUMENTS - INPUT
// ============================================================================

void ReencodeJob::addInputArgs(std::vector<std::string>& args) const {
    args.push_back("-i");
    args.push_back(input_path_);
}

// ============================================================================
// ADD ARGUMENTS - VIDEO CODEC
// ============================================================================

void ReencodeJob::addVideoCodecArgs(std::vector<std::string>& args) const {
    args.push_back("-c:v");
    args.push_back(getEncoderName());
    
    // ProRes specific parameters
    if (config_.codec == Encode::Codec::ProRes) {
        args.push_back("-profile:v");
        args.push_back(std::to_string(config_.prores_profile));
        
        args.push_back("-vendor");
        args.push_back(config_.prores_vendor);
        
        args.push_back("-bits_per_mb");
        args.push_back(std::to_string(config_.bits_per_mb));
    }
    
    // FFV1 specific parameters
    if (config_.codec == Encode::Codec::FFV1) {
        args.push_back("-coder");
        args.push_back(std::to_string(config_.ffv1_coder));
        
        args.push_back("-context");
        args.push_back(std::to_string(config_.ffv1_context));
        
        args.push_back("-level");
        args.push_back(std::to_string(config_.ffv1_level));
        
        args.push_back("-slices");
        args.push_back(std::to_string(config_.ffv1_slices));
    }
    
    // x264 specific parameters
    if (config_.codec == Encode::Codec::X264 && !config_.x264_params.empty()) {
        args.push_back("-x264-params");
        args.push_back(config_.x264_params);
    }
    
    // NVENC specific parameters
    if (config_.codec == Encode::Codec::H264_NVENC || config_.codec == Encode::Codec::H265_NVENC) {
        args.push_back("-b_adapt");
        args.push_back(std::to_string(config_.b_adapt));
        
        args.push_back("-rc-lookahead");
        args.push_back(std::to_string(config_.rc_lookahead));
        
        if (config_.qp_cb_offset != 0) {
            args.push_back("-qp_cb_offset");
            args.push_back(std::to_string(config_.qp_cb_offset));
        }
        
        if (config_.qp_cr_offset != 0) {
            args.push_back("-qp_cr_offset");
            args.push_back(std::to_string(config_.qp_cr_offset));
        }
    }
}

// ============================================================================
// ADD ARGUMENTS - RATE CONTROL
// ============================================================================

void ReencodeJob::addRateControlArgs(std::vector<std::string>& args) const {
    switch (config_.rate_control) {
        case Encode::RateControl::CRF:
            args.push_back("-crf");
            args.push_back(std::to_string(config_.quality));
            break;
            
        case Encode::RateControl::CQP:
            args.push_back("-qp");
            args.push_back(std::to_string(config_.quality));
            break;
            
        case Encode::RateControl::VBR:
            if (config_.bitrate_kbps > 0) {
                args.push_back("-b:v");
                args.push_back(std::to_string(config_.bitrate_kbps) + "k");
            }
            break;
            
        case Encode::RateControl::CBR:
            if (config_.bitrate_kbps > 0) {
                args.push_back("-b:v");
                args.push_back(std::to_string(config_.bitrate_kbps) + "k");
                args.push_back("-maxrate");
                args.push_back(std::to_string(config_.bitrate_kbps) + "k");
                
                int buffer_size = config_.buffer_size_kbps > 0 ? config_.buffer_size_kbps : config_.bitrate_kbps * 2;
                args.push_back("-bufsize");
                args.push_back(std::to_string(buffer_size) + "k");
            }
            break;
    }
}

// ============================================================================
// ADD ARGUMENTS - ENCODING PARAMETERS
// ============================================================================

void ReencodeJob::addEncodingParams(std::vector<std::string>& args) const {
    // Preset
    if (!config_.preset.empty()) {
        args.push_back("-preset");
        args.push_back(config_.preset);
    }
    
    // Tune
    if (!config_.tune.empty()) {
        args.push_back("-tune");
        args.push_back(config_.tune);
    }
    
    // GOP size
    if (config_.gop_size > 0) {
        args.push_back("-g");
        args.push_back(std::to_string(config_.gop_size));
    }
    
    // B-frames
    args.push_back("-bf");
    args.push_back(std::to_string(config_.bframes));
    
    // Threads
    if (config_.threads > 0) {
        args.push_back("-threads");
        args.push_back(std::to_string(config_.threads));
    }
}

// ============================================================================
// ADD ARGUMENTS - PIXEL FORMAT
// ============================================================================

void ReencodeJob::addPixelFormatArgs(std::vector<std::string>& args) const {
    args.push_back("-pix_fmt");
    args.push_back(getPixelFormatString());
}

// ============================================================================
// ADD ARGUMENTS - COLOR SPACE
// ============================================================================

void ReencodeJob::addColorSpaceArgs(std::vector<std::string>& args) const {
    if (config_.passthrough_color) {
        return; // Keep input color space
    }
    
    // Range
    args.push_back("-color_range");
    args.push_back(getColorRangeString());
    
    // Primaries
    args.push_back("-colorspace");
    args.push_back(getColorMatrixString());
    
    args.push_back("-color_primaries");
    args.push_back(getColorPrimariesString());
    
    // Transfer
    args.push_back("-color_trc");
    args.push_back(getTransferString());
}

// ============================================================================
// ADD ARGUMENTS - HDR METADATA
// ============================================================================

void ReencodeJob::addHDRMetadata(std::vector<std::string>& args) const {
    // Content Light Level
    if (config_.content_light_level.has_value()) {
        const auto& cll = config_.content_light_level.value();
        std::ostringstream oss;
        oss << cll.max_cll << "," << cll.max_fall;
        args.push_back("-max-cll");
        args.push_back(oss.str());
    }
    
    // Mastering Display
    if (config_.mastering_display.has_value()) {
        const auto& md = config_.mastering_display.value();
        std::ostringstream oss;
        oss << "G(" << md.green_x << "," << md.green_y << ")"
            << "B(" << md.blue_x << "," << md.blue_y << ")"
            << "R(" << md.red_x << "," << md.red_y << ")"
            << "WP(" << md.white_x << "," << md.white_y << ")"
            << "L(" << md.max_luminance << "," << md.min_luminance << ")";
        args.push_back("-master-display");
        args.push_back(oss.str());
    }
}

// ============================================================================
// ADD ARGUMENTS - AUDIO
// ============================================================================

void ReencodeJob::addAudioArgs(std::vector<std::string>& args) const {
    if (config_.audio.copy_audio) {
        args.push_back("-c:a");
        args.push_back("copy");
    } else {
        args.push_back("-c:a");
        args.push_back(config_.audio.codec);
        
        if (config_.audio.bitrate_kbps > 0) {
            args.push_back("-b:a");
            args.push_back(std::to_string(config_.audio.bitrate_kbps) + "k");
        }
        
        args.push_back("-ar");
        args.push_back(std::to_string(config_.audio.sample_rate));
        
        args.push_back("-ac");
        args.push_back(std::to_string(config_.audio.channels));
    }
}

// ============================================================================
// ADD ARGUMENTS - OUTPUT
// ============================================================================

void ReencodeJob::addOutputArgs(std::vector<std::string>& args) const {
    args.push_back(output_path_);
}

// ============================================================================
// HELPERS - CODEC CONVERSION
// ============================================================================

std::string ReencodeJob::getEncoderName() const {
    // If a custom encoder is specified, use it
    if (!config_.encoder_override.empty())
        return config_.encoder_override;
    
    // Otherwise, map according to codec
    return Codec::CodecUtils::getEncoderName(config_.codec);
}

// ============================================================================
// HELPERS - PIXEL FORMAT CONVERSION
// ============================================================================

std::string ReencodeJob::getPixelFormatString() const {
    switch (config_.pixel_format) {
        case Encode::PixelFormat::RGB24:
            return "rgb24";
        case Encode::PixelFormat::RGB48:
            return "rgb48le";
        case Encode::PixelFormat::RGBF16:
            return "rgbf16le";
        case Encode::PixelFormat::YUV420P8:
            return "yuv420p";
        case Encode::PixelFormat::YUV420P10:
            return "yuv420p10le";
        case Encode::PixelFormat::P010:
            return "p010le";
        case Encode::PixelFormat::NV12:
            return "nv12";
        case Encode::PixelFormat::YUV422P10:
            return "yuv422p10le";
        case Encode::PixelFormat::YUV444P10:
            return "yuv444p10le";
        case Encode::PixelFormat::YUVA444P10LE:
            return "yuva444p10le";
        default:
            return "yuv420p";
    }
}

// ============================================================================
// HELPERS - COLOR SPACE CONVERSION
// ============================================================================

std::string ReencodeJob::getColorRangeString() const {
    switch (config_.color_profile.range) {
        case Encode::Range::Limited:
            return "tv";
        case Encode::Range::Full:
            return "pc";
        default:
            return "pc";
    }
}

std::string ReencodeJob::getColorPrimariesString() const {
    switch (config_.color_profile.primaries) {
        case Encode::ColorPrimaries::BT601:
            return "bt470bg";
        case Encode::ColorPrimaries::BT709:
            return "bt709";
        case Encode::ColorPrimaries::BT2020:
            return "bt2020";
        case Encode::ColorPrimaries::P3D65:
            return "smpte432";
        default:
            return "bt709";
    }
}

std::string ReencodeJob::getTransferString() const {
    switch (config_.color_profile.transfer) {
        case Encode::TransferCharacteristic::SRGB:
            return "iec61966-2-1";
        case Encode::TransferCharacteristic::BT1886:
            return "bt709";
        case Encode::TransferCharacteristic::PQ:
            return "smpte2084";
        case Encode::TransferCharacteristic::HLG:
            return "arib-std-b67";
        default:
            return "bt709";
    }
}

std::string ReencodeJob::getColorMatrixString() const {
    switch (config_.color_profile.matrix) {
        case Encode::MatrixCoefficients::BT601:
            return "bt470bg";
        case Encode::MatrixCoefficients::BT709:
            return "bt709";
        case Encode::MatrixCoefficients::BT2020NCL:
            return "bt2020nc";
        default:
            return "bt709";
    }
}

// ============================================================================
// VALIDATION
// ============================================================================

bool ReencodeJob::validate() const {
    if (input_path_.empty())
        throw std::runtime_error("Input path is empty");
    if (output_path_.empty())
        throw std::runtime_error("Output path is empty");
    
    if (config_.rate_control == Encode::RateControl::VBR || config_.rate_control == Encode::RateControl::CBR) {
        if (config_.bitrate_kbps <= 0)
            throw std::runtime_error("Bitrate must be > 0 for VBR/CBR modes");
    }
    
    return true;
}

// ============================================================================
// EXECUTION
// ============================================================================

bool ReencodeJob::execute() {
    try {
        // Validation
        validate();
        
        // Build command
        auto args = buildCommand();
        
        // Log command
        std::cout << "[INFO] Encode command: " << getCommandString() << std::endl;
        
        // Execution via FFmpegProcess with absolute path
        std::filesystem::path extern_path = FFmpegMulti::PathUtils::getExternPath();
        std::filesystem::path ffmpeg_path = extern_path / "ffmpeg.exe";
        ffmpegProcess process(ffmpeg_path, args);
        
        // Actually execute the command
        bool success = process.execute();
        
        if (success)
            std::cout << "[SUCCESS] Encoding finished successfully!" << std::endl;
        else
            std::cerr << "[ERROR] Encoding failed!" << std::endl;
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Encode failed: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Jobs
} // namespace FFmpegMulti
