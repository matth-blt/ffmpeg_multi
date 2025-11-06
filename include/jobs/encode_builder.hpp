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
    EncodeJobBuilder& h264();
    EncodeJobBuilder& h265();
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
    EncodeJobBuilder& archivePreset();
    EncodeJobBuilder& webPreset();
    EncodeJobBuilder& streamingPreset(int bitrate_kbps);
    
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