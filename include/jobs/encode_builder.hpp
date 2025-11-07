#pragma once
#include "encode.hpp"

namespace FFmpegMulti {
namespace Jobs {

class EncodeJobBuilder {
public:
    EncodeJobBuilder() = default;
    
    // === I/O ===
    EncodeJobBuilder& input(const std::string& path);
    EncodeJobBuilder& output(const std::string& path);
    
    // === Codec Shortcuts ===
    EncodeJobBuilder& x264();
    EncodeJobBuilder& x265();
    EncodeJobBuilder& hevc();
    EncodeJobBuilder& av1();
    EncodeJobBuilder& svtav1();
    EncodeJobBuilder& prores();
    EncodeJobBuilder& ffv1();
    EncodeJobBuilder& h264_nvenc();
    EncodeJobBuilder& h265_nvenc();
    EncodeJobBuilder& codec(Encode::Codec c);
    
    // === Rate Control ===
    EncodeJobBuilder& crf(int value);
    EncodeJobBuilder& qp(int value);
    EncodeJobBuilder& bitrate(int kbps);
    EncodeJobBuilder& cbr(int kbps);
    EncodeJobBuilder& vbr(int kbps);
    
    // === Encoding Parameters ===
    EncodeJobBuilder& preset(const std::string& p);
    EncodeJobBuilder& tune(const std::string& t);
    EncodeJobBuilder& gopSize(int frames);
    EncodeJobBuilder& bframes(int count);
    EncodeJobBuilder& threads(int count);
    
    // === Pixel Format Shortcuts ===
    EncodeJobBuilder& eightBit();
    EncodeJobBuilder& tenBit();
    EncodeJobBuilder& pixelFormat(Encode::PixelFormat fmt);
    
    // === ProRes Specific ===
    EncodeJobBuilder& proresProfile(int profile); // 0=Proxy, 1=LT, 2=Standard, 3=HQ, 4=4444, 5=4444XQ
    EncodeJobBuilder& proresVendor(const std::string& vendor);
    EncodeJobBuilder& proresBitsPerMB(int bits);
    
    // === FFV1 Specific ===
    EncodeJobBuilder& ffv1Coder(int coder); // 0=Golomb-Rice, 1=Range Coder, 2=Range Coder with custom state
    EncodeJobBuilder& ffv1Context(int context); // 0=small, 1=large
    EncodeJobBuilder& ffv1Level(int level); // FFV1 level (1 or 3)
    EncodeJobBuilder& ffv1Slices(int slices); // Number of slices (for multithreading)
    
    // === x264 Specific ===
    EncodeJobBuilder& x264Params(const std::string& params); // x264-params string
    
    // === NVENC Specific ===
    EncodeJobBuilder& nvencBAdapt(int value); // B-frame adaptive mode
    EncodeJobBuilder& nvencRcLookahead(int frames); // Rate control lookahead
    EncodeJobBuilder& nvencQpCbOffset(int offset); // QP offset for Cb channel
    EncodeJobBuilder& nvencQpCrOffset(int offset); // QP offset for Cr channel
    
    // === Color Space Presets ===
    EncodeJobBuilder& sdr();
    EncodeJobBuilder& hdr10();
    EncodeJobBuilder& hlg();
    EncodeJobBuilder& colorSpace(
        Encode::ColorPrimaries primaries,
        Encode::TransferCharacteristic transfer,
        Encode::MatrixCoefficients matrix,
        Encode::Range range = Encode::Range::Limited
    );
    
    // === HDR Metadata ===
    EncodeJobBuilder& maxCLL(uint16_t max_cll, uint16_t max_fall);
    EncodeJobBuilder& masteringDisplay(
        float rx, float ry, float gx, float gy,
        float bx, float by, float wx, float wy,
        float min_lum, float max_lum
    );
    
    // === Audio ===
    EncodeJobBuilder& copyAudio();
    EncodeJobBuilder& audioCodec(const std::string& codec);
    EncodeJobBuilder& audioBitrate(int kbps);
    EncodeJobBuilder& audioSampleRate(int hz);
    EncodeJobBuilder& audioChannels(int channels);
    
    // === Presets Complets ===
    EncodeJobBuilder& youtubePreset();
    EncodeJobBuilder& x264Preset(); // x264 high quality preset
    EncodeJobBuilder& x265Preset(); // x265 high quality preset
    EncodeJobBuilder& h264NvencPreset(); // h264_nvenc high quality preset
    EncodeJobBuilder& h265NvencPreset(); // h265_nvenc high quality preset
    EncodeJobBuilder& proresPreset(int profile = 4);
    EncodeJobBuilder& ffv1Preset();

    // === Advanced Options ===
    EncodeJobBuilder& container(const std::string& ext);
    EncodeJobBuilder& extraArgs(const std::vector<std::string>& args);
    EncodeJobBuilder& addExtraArg(const std::string& arg);
    
    // === Construction Finale ===
    EncodeJob build();
    
private:
    std::string input_path_;
    std::string output_path_;
    Encode::EncodeConfig config_;
    
    void validate() const;
};

} // namespace Jobs
} // namespace FFmpegMulti