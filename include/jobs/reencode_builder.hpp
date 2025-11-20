#pragma once
#include "reencode.hpp"

namespace FFmpegMulti {
namespace Jobs {

class ReencodeJobBuilder {
public:
    ReencodeJobBuilder() = default;
    
    // === I/O ===
    ReencodeJobBuilder& input(const std::string& path);
    ReencodeJobBuilder& output(const std::string& path);
    
    // === Codec Shortcuts ===
    ReencodeJobBuilder& x264();
    ReencodeJobBuilder& x265();
    ReencodeJobBuilder& hevc();
    ReencodeJobBuilder& av1();
    ReencodeJobBuilder& svtav1();
    ReencodeJobBuilder& prores();
    ReencodeJobBuilder& ffv1();
    ReencodeJobBuilder& h264_nvenc();
    ReencodeJobBuilder& h265_nvenc();
    ReencodeJobBuilder& codec(Encode::Codec c);
    
    // === Rate Control ===
    ReencodeJobBuilder& crf(int value);
    ReencodeJobBuilder& qp(int value);
    ReencodeJobBuilder& bitrate(int kbps);
    ReencodeJobBuilder& cbr(int kbps);
    ReencodeJobBuilder& vbr(int kbps);
    
    // === Encoding Parameters ===
    ReencodeJobBuilder& preset(const std::string& p);
    ReencodeJobBuilder& tune(const std::string& t);
    ReencodeJobBuilder& gopSize(int frames);
    ReencodeJobBuilder& bframes(int count);
    ReencodeJobBuilder& threads(int count);
    
    // === Pixel Format Shortcuts ===
    ReencodeJobBuilder& eightBit();
    ReencodeJobBuilder& tenBit();
    ReencodeJobBuilder& pixelFormat(Encode::PixelFormat fmt);
    
    // === ProRes Specific ===
    ReencodeJobBuilder& proresProfile(int profile); // 0=Proxy, 1=LT, 2=Standard, 3=HQ, 4=4444, 5=4444XQ
    ReencodeJobBuilder& proresVendor(const std::string& vendor);
    ReencodeJobBuilder& proresBitsPerMB(int bits);
    
    // === FFV1 Specific ===
    ReencodeJobBuilder& ffv1Coder(int coder); // 0=Golomb-Rice, 1=Range Coder, 2=Range Coder with custom state
    ReencodeJobBuilder& ffv1Context(int context); // 0=small, 1=large
    ReencodeJobBuilder& ffv1Level(int level); // FFV1 level (1 or 3)
    ReencodeJobBuilder& ffv1Slices(int slices); // Number of slices (for multithreading)
    
    // === x264 Specific ===
    ReencodeJobBuilder& x264Params(const std::string& params); // x264-params string
    
    // === NVENC Specific ===
    ReencodeJobBuilder& nvencBAdapt(int value); // B-frame adaptive mode
    ReencodeJobBuilder& nvencRcLookahead(int frames); // Rate control lookahead
    ReencodeJobBuilder& nvencQpCbOffset(int offset); // QP offset for Cb channel
    ReencodeJobBuilder& nvencQpCrOffset(int offset); // QP offset for Cr channel
    
    // === Color Space Presets ===
    ReencodeJobBuilder& sdr();
    ReencodeJobBuilder& hdr10();
    ReencodeJobBuilder& hlg();
    ReencodeJobBuilder& colorSpace(
        Encode::ColorPrimaries primaries,
        Encode::TransferCharacteristic transfer,
        Encode::MatrixCoefficients matrix,
        Encode::Range range = Encode::Range::Limited
    );
    
    // === HDR Metadata ===
    ReencodeJobBuilder& maxCLL(uint16_t max_cll, uint16_t max_fall);
    ReencodeJobBuilder& masteringDisplay(
        float rx, float ry, float gx, float gy,
        float bx, float by, float wx, float wy,
        float min_lum, float max_lum
    );
    
    // === Audio ===
    ReencodeJobBuilder& copyAudio();
    ReencodeJobBuilder& audioCodec(const std::string& codec);
    ReencodeJobBuilder& audioBitrate(int kbps);
    ReencodeJobBuilder& audioSampleRate(int hz);
    ReencodeJobBuilder& audioChannels(int channels);
    
    // === Complete Presets ===
    ReencodeJobBuilder& youtubePreset();
    ReencodeJobBuilder& x264Preset(); // x264 high quality preset
    ReencodeJobBuilder& x265Preset(); // x265 high quality preset
    ReencodeJobBuilder& h264NvencPreset(); // h264_nvenc high quality preset
    ReencodeJobBuilder& h265NvencPreset(); // h265_nvenc high quality preset
    ReencodeJobBuilder& proresPreset(int profile = 4);
    ReencodeJobBuilder& ffv1Preset();

    // === Advanced Options ===
    ReencodeJobBuilder& container(const std::string& ext);
    ReencodeJobBuilder& extraArgs(const std::vector<std::string>& args);
    ReencodeJobBuilder& addExtraArg(const std::string& arg);
    
    // === Final Construction ===
    ReencodeJob build();
    
private:
    std::string input_path_;
    std::string output_path_;
    Encode::EncodeConfig config_;
    
    void validate() const;
};

} // namespace Jobs
} // namespace FFmpegMulti
