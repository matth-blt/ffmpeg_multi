#include "../../include/jobs/codec_utils.hpp"
#include <stdexcept>

namespace FFmpegMulti {
namespace Codec {

// ============================================================================
// CODEC MAPPING
// ============================================================================

std::string CodecUtils::getEncoderName(Encode::Codec codec, const std::string& encoder_override) {
    // Encodeur personnalisé prioritaire
    if (!encoder_override.empty()) {
        return encoder_override;
    }
    
    // Mapping standard
    switch (codec) {
        case Encode::Codec::X264:
            return "libx264";
        case Encode::Codec::X265:
            return "libx265";
        case Encode::Codec::H264_NVENC:
            return "h264_nvenc";
        case Encode::Codec::H265_NVENC:
            return "hevc_nvenc";
        case Encode::Codec::AV1:
            return "libaom-av1";
        case Encode::Codec::SVT_AV1:
        case Encode::Codec::SVT_AV1_ESSENTIAL:
            return "libsvtav1";
        case Encode::Codec::ProRes:
            return "prores_ks";
        case Encode::Codec::FFV1:
            return "ffv1";
        default:
            return "libx264";
    }
}

// ============================================================================
// CODEC ARGUMENTS
// ============================================================================

void CodecUtils::addCodecArgs(std::vector<std::string>& args, Encode::Codec codec, int quality, const std::string& preset) {
    switch (codec) {
        case Encode::Codec::X264:
        case Encode::Codec::X265:
        case Encode::Codec::AV1:
        case Encode::Codec::SVT_AV1:
        case Encode::Codec::SVT_AV1_ESSENTIAL:
            // CRF mode
            args.push_back("-crf");
            args.push_back(std::to_string(quality));
            if (!preset.empty()) {
                args.push_back("-preset");
                args.push_back(preset);
            }
            break;
            
        case Encode::Codec::H264_NVENC:
        case Encode::Codec::H265_NVENC:
            // CQ mode (NVENC)
            args.push_back("-cq");
            args.push_back(std::to_string(quality));
            if (!preset.empty()) {
                args.push_back("-preset");
                args.push_back(preset);
            }
            break;
            
        case Encode::Codec::ProRes:
            addProResArgs(args);
            break;
            
        case Encode::Codec::FFV1:
            addFFV1Args(args);
            break;
    }
}

void CodecUtils::addProResArgs(std::vector<std::string>& args, int profile, const std::string& vendor, int bits_per_mb) {
    args.push_back("-profile:v");
    args.push_back(std::to_string(profile));
    
    args.push_back("-vendor");
    args.push_back(vendor);
    
    args.push_back("-bits_per_mb");
    args.push_back(std::to_string(bits_per_mb));
    
    args.push_back("-pix_fmt");
    args.push_back("yuva444p10le");
}

void CodecUtils::addFFV1Args(std::vector<std::string>& args, int level, int coder, int context, int slices) {
    args.push_back("-level");
    args.push_back(std::to_string(level));
    
    args.push_back("-coder");
    args.push_back(std::to_string(coder));
    
    args.push_back("-context");
    args.push_back(std::to_string(context));
    
    args.push_back("-g");
    args.push_back("1"); // Intra-only pour lossless
    
    args.push_back("-slices");
    args.push_back(std::to_string(slices));
}

// ============================================================================
// CONTAINER UTILS
// ============================================================================

std::string CodecUtils::getContainerExtension(const std::string& container) {
    if (container == "mkv") return ".mkv";
    if (container == "webm") return ".webm";
    if (container == "mp4") return ".mp4";
    if (container == "mov") return ".mov";
    return ".mkv";
}

bool CodecUtils::isCodecCompatibleWithContainer(Encode::Codec codec, const std::string& container) {
    if (container == "webm") {
        return codec == Encode::Codec::AV1 || codec == Encode::Codec::SVT_AV1;
    }

    if (container == "mov") {
        return true;
    }

    if (container == "mkv") {
        return true;
    }

    if (container == "mp4") {
        return codec != Encode::Codec::FFV1;
    }
    
    return true;
}

// ============================================================================
// VALIDATION
// ============================================================================

bool CodecUtils::validateQuality(Encode::Codec codec, int quality) {
    // CRF/QP : 0-51 pour la plupart
    if (codec == Encode::Codec::X264 || codec == Encode::Codec::X265 || 
        codec == Encode::Codec::AV1 || codec == Encode::Codec::SVT_AV1 ||
        codec == Encode::Codec::H264_NVENC || codec == Encode::Codec::H265_NVENC) {
        return quality >= 0 && quality <= 51;
    }

    return true;
}

bool CodecUtils::validatePreset(Encode::Codec codec, const std::string& preset) {
    if (preset.empty()) return true;

    if (codec == Encode::Codec::X264 || codec == Encode::Codec::X265) {
        return preset == "ultrafast" || preset == "superfast" || preset == "veryfast" || preset == "faster" || preset == "fast" || 
               preset == "medium" || 
               preset == "slow" || preset == "slower" || 
               preset == "veryslow";
    }

    if (codec == Encode::Codec::H264_NVENC || codec == Encode::Codec::H265_NVENC) {
        return preset == "fast" || preset == "medium" || preset == "slow" || preset == "hq" || preset.rfind("p", 0) == 0;
    }
    
    return true;
}

} // namespace Codec
} // namespace FFmpegMulti
