#pragma once

#include <string>
#include <filesystem>
#include "../core/job.hpp"

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Encoding job with SVT-AV1-Essential via Auto-Boost
 * 
 * Uses Auto-Boost-Essential.py for optimized AV1 encoding
 * with automatic audio handling and final muxing.
 */
class SvtAv1EssentialJob : public FFmpegMulti::Core::Job {
public:
    enum class Quality {
        LOW, // Fast, acceptable quality
        MEDIUM, // Balanced
        HIGH // Slow, excellent quality
    };
    
    struct Config {
        std::string input_path;
        std::string output_path;
        Quality quality{Quality::HIGH};
        bool aggressive{false}; // Aggressive mode (more compression)
        bool unshackle{false}; // Unshackle speed limits
        bool verbose{true}; // Detailed output
        bool cleanup{true}; // Auto cleanup of temp files
    };
    
    explicit SvtAv1EssentialJob(const std::string& input, const std::string& output);
    ~SvtAv1EssentialJob() = default;

    void setQuality(Quality q);
    void setAggressive(bool enabled);
    void setUnshackle(bool enabled);
    void setVerbose(bool enabled);
    void setCleanup(bool enabled);
    
    Config& config();
    const Config& config() const;

    bool execute() override;
    
private:
    Config config_;

    bool validatePaths();
    bool extractAudio();
    bool runAutoBoost();
    bool muxFinal();
    bool cleanup();
    
    // Helpers
    std::string getQualityString() const;
    std::filesystem::path getTempDir() const;
    std::filesystem::path getAviPath() const;
    std::filesystem::path getAudioPath() const;
    std::string buildABECommand() const;
};

/**
 * @brief Builder for SvtAv1EssentialJob
 */
class SvtAv1EssentialBuilder {
public:
    SvtAv1EssentialBuilder& input(const std::string& path);
    SvtAv1EssentialBuilder& output(const std::string& path);
    SvtAv1EssentialBuilder& quality(SvtAv1EssentialJob::Quality q);
    SvtAv1EssentialBuilder& aggressive();
    SvtAv1EssentialBuilder& unshackle();
    SvtAv1EssentialBuilder& verbose(bool enabled = true);
    SvtAv1EssentialBuilder& cleanup(bool enabled = true);
    
    SvtAv1EssentialJob build();
    
private:
    SvtAv1EssentialJob::Config config_;
};

} // namespace Jobs
} // namespace FFmpegMulti
