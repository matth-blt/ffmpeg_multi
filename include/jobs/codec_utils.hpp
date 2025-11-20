#pragma once

#include <string>
#include <vector>
#include "encode_types.hpp"

namespace FFmpegMulti {
namespace Codec {

/**
 * @brief Shared utilities for codec management
 */
class CodecUtils {
public:
    // ========================================================================
    // CODEC MAPPING
    // ========================================================================
    
    /**
     * @brief Gets the FFmpeg encoder name corresponding to the codec
     * @param codec Codec type
     * @param encoder_override Custom encoder (optional)
     * @return FFmpeg encoder name
     */
    static std::string getEncoderName(Encode::Codec codec, const std::string& encoder_override = "");
    
    // ========================================================================
    // CODEC ARGUMENTS
    // ========================================================================
    
    /**
     * @brief Adds codec-specific arguments to the args vector
     * @param args FFmpeg arguments vector to fill
     * @param codec Codec type
     * @param quality Quality value (CRF/QP)
     * @param preset Encoding preset
     */
    static void addCodecArgs(std::vector<std::string>& args, Encode::Codec codec, int quality, const std::string& preset);
    
    /**
     * @brief Adds arguments for ProRes
     */
    static void addProResArgs(std::vector<std::string>& args, int profile = 4, const std::string& vendor = "apl0", int bits_per_mb = 8000);
    
    /**
     * @brief Adds arguments for FFV1
     */
    static void addFFV1Args(std::vector<std::string>& args, int level = 3, int coder = 1, int context = 1, int slices = 24);
    
    // ========================================================================
    // CONTAINER UTILS
    // ========================================================================
    
    /**
     * @brief Gets the file extension according to the container format
     * @param format Container format (MKV, MP4, WEBM, MOV)
     * @return Extension with dot (e.g., ".mkv")
     */
    static std::string getContainerExtension(const std::string& container);
    
    /**
     * @brief Checks if a codec is compatible with a container
     * @param codec Codec type
     * @param container Container format
     * @return true if compatible, false otherwise
     */
    static bool isCodecCompatibleWithContainer(Encode::Codec codec, const std::string& container);
    
    // ========================================================================
    // VALIDATION
    // ========================================================================
    
    /**
     * @brief Validates quality parameters according to the codec
     * @param codec Codec type
     * @param quality Quality value
     * @return true if valid, false otherwise
     */
    static bool validateQuality(Encode::Codec codec, int quality);
    
    /**
     * @brief Validates the preset according to the codec
     * @param codec Codec type
     * @param preset Encoding preset
     * @return true if valid, false otherwise
     */
    static bool validatePreset(Encode::Codec codec, const std::string& preset);
};

} // namespace Codec
} // namespace FFmpegMulti
