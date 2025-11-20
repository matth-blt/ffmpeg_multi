#pragma once

#include <string>
#include <vector>
#include "../core/job.hpp"

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Image format for thumbnails
 */
enum class ThumbnailFormat {
    PNG,
    TIFF,
    JPEG
};

/**
 * @brief Configuration for thumbnail extraction with scene detection
 */
struct ThumbnailsConfig {
    std::string input_path;
    std::string output_dir;
    bool create_subfolder{true};
    std::string subfolder_name;
    ThumbnailFormat format{ThumbnailFormat::PNG};
    float scene_threshold{0.15f};
};

/**
 * @brief Thumbnail extraction job with scene detection
 * 
 * This job automatically extracts thumbnails only at scene changes,
 * avoiding duplicate or very similar images.
 */
class ThumbnailsJob : public FFmpegMulti::Core::Job {
public:
    ThumbnailsJob() = default;
    explicit ThumbnailsJob(const ThumbnailsConfig& config);

    // Configuration
    void setConfig(const ThumbnailsConfig& config);
    ThumbnailsConfig& config();
    const ThumbnailsConfig& config() const;

    // Execution
    bool execute() override;

    // Command construction
    std::vector<std::string> buildCommand() const;
    std::string getCommandString() const;

private:
    ThumbnailsConfig config_;

    // Helpers
    bool validatePaths() const;
    bool createOutputDirectory() const;
    std::string getOutputPattern() const;
    std::string getFileExtension() const;
    std::string getSceneFilter() const;
};

/**
 * @brief Builder for ThumbnailsJob
 */
class ThumbnailsBuilder {
public:
    ThumbnailsBuilder() = default;

    ThumbnailsBuilder& input(const std::string& path);
    ThumbnailsBuilder& outputDir(const std::string& path);
    ThumbnailsBuilder& createSubfolder(bool create);
    ThumbnailsBuilder& subfolderName(const std::string& name);
    ThumbnailsBuilder& format(ThumbnailFormat fmt);
    ThumbnailsBuilder& sceneThreshold(float threshold);

    ThumbnailsBuilder& png();
    ThumbnailsBuilder& tiff();
    ThumbnailsBuilder& jpeg();

    ThumbnailsJob build() const;

private:
    ThumbnailsConfig config_;
};

} // namespace Jobs
} // namespace FFmpegMulti
