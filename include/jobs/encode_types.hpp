#pragma once

#include <string>
#include <optional>
#include <cstdint>
#include <vector>

namespace FFmpegMulti {
namespace Encode {

// ============================================================================
// VIDEO CODECS
// ============================================================================
enum class Codec { 
    X264, // x264
    X265, // x265/HEVC
    H264_NVENC, // NVIDIA Hardware H.264
    H265_NVENC, // NVIDIA Hardware H.265/HEVC
    AV1, // libaom-av1
    SVT_AV1, // SVT-AV1 (faster)
    SVT_AV1_ESSENTIAL, // SVT-AV1 via Auto-Boost-Essential
    ProRes, // Apple ProRes
    FFV1 // Lossless FFV1
};

// ============================================================================
// RATE CONTROL MODES
// ============================================================================
enum class RateControl { 
    CRF, // Constant Rate Factor (quality-based)
    CQP, // Constant Quantization Parameter
    VBR, // Variable Bitrate
    CBR // Constant Bitrate
};

// ============================================================================
// PIXEL FORMATS
// ============================================================================
enum class PixelFormat { 
    RGB24, // 8-bit RGB
    RGB48, // 16-bit RGB
    RGBF16, // 16-bit float RGB
    YUV420P8, // 8-bit 4:2:0
    YUV420P10, // 10-bit 4:2:0
    P010, // 10-bit 4:2:0 (hardware)
    YUV422P10, // 10-bit 4:2:2
    YUV444P10 // 10-bit 4:4:4
};

// ============================================================================
// COLOR SPACE SPECIFICATIONS
// ============================================================================
enum class Range { 
    Limited, // TV range (16-235)
    Full // PC range (0-255)
};

enum class ColorPrimaries { 
    BT601, // SD standard
    BT709, // HD standard
    BT2020, // UHD/HDR standard
    P3D65 // Digital Cinema
};

enum class TransferCharacteristic { 
    SRGB, // Standard sRGB
    BT1886, // Rec.709 gamma
    PQ, // HDR10 (SMPTE ST 2084)
    HLG // Hybrid Log-Gamma
};

enum class MatrixCoefficients { 
    BT601, // SD color matrix
    BT709, // HD color matrix
    BT2020NCL // UHD color matrix (non-constant luminance)
};

// ============================================================================
// COLOR PROFILE
// ============================================================================
struct ColorProfile {
    ColorPrimaries primaries{ColorPrimaries::BT709};
    TransferCharacteristic transfer{TransferCharacteristic::BT1886};
    MatrixCoefficients matrix{MatrixCoefficients::BT709};
    Range range{Range::Full};
};

// ============================================================================
// HDR METADATA
// ============================================================================
struct ContentLightLevel { 
    uint16_t max_cll{0}; // Maximum Content Light Level
    uint16_t max_fall{0}; // Maximum Frame Average Light Level
};

struct MasteringDisplay {
    // Color primaries (x, y coordinates)
    float red_x{0.708f}, red_y{0.292f};
    float green_x{0.170f}, green_y{0.797f};
    float blue_x{0.131f}, blue_y{0.046f};
    float white_x{0.3127f}, white_y{0.3290f};
    
    // Luminance (cd/m²)
    float min_luminance{0.0f};
    float max_luminance{1000.0f};
};

// ============================================================================
// AUDIO CONFIGURATION
// ============================================================================
struct AudioConfig {
    bool copy_audio{false}; // If true, copy audio stream as-is
    std::string source_path{}; // Source file for audio (if different from video)
    std::string codec{"aac"}; // Audio codec if not copying
    uint32_t sample_rate{48000}; // Sample rate in Hz
    uint16_t channels{2}; // Number of audio channels
    std::string layout{"stereo"}; // Channel layout
    int bitrate_kbps{192}; // Audio bitrate
};

// ============================================================================
// MAIN ENCODE CONFIGURATION
// ============================================================================
struct EncodeConfig {
    // --- Video Codec ---
    Codec codec{Codec::X264};
    std::string encoder_override{}; // Force specific encoder name (optional)
    
    // --- Rate Control ---
    RateControl rate_control{RateControl::CRF};
    int quality{18}; // CRF/CQP value (lower = better quality)
    int bitrate_kbps{0}; // For VBR/CBR modes
    int max_bitrate_kbps{0}; // Maximum bitrate (for CBR)
    int buffer_size_kbps{0}; // VBV buffer size
    
    // --- Encoding Parameters ---
    std::string preset{"slow"}; // Encoding speed preset
    std::string tune{}; // Tune for specific content (film, grain, etc.)
    int gop_size{250}; // Group of Pictures size
    int bframes{3}; // Number of B-frames
    int threads{0}; // Thread count (0 = auto)
    
    // --- Pixel Format ---
    PixelFormat pixel_format{PixelFormat::YUV420P8};
    
    // --- Color Space & HDR ---
    bool passthrough_color{true}; // Keep input color space
    ColorProfile color_profile{}; // Override color profile
    std::optional<MasteringDisplay> mastering_display{};
    std::optional<ContentLightLevel> content_light_level{};
    
    // --- Output Container ---
    std::string container{"mp4"}; // Output container format
    
    // --- Audio ---
    AudioConfig audio{};
    
    // --- Advanced Options ---
    std::vector<std::string> extra_args{}; // Additional FFmpeg arguments
};

} // namespace Encode
} // namespace FFmpegMulti
