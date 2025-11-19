#pragma once

#include <string>
#include <vector>
#include "../core/job.hpp"

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Format d'image pour l'extraction de frames
 */
enum class ImageFormat {
    PNG,
    TIFF,
    JPEG
};

/**
 * @brief Configuration pour l'extraction de frames
 */
struct ExtractFramesConfig {
    std::string input_path;
    std::string output_dir;
    bool create_subfolder{true};
    std::string subfolder_name;
    ImageFormat format{ImageFormat::PNG};
};

/**
 * @brief Job d'extraction de frames d'une vidéo
 */
class ExtractFramesJob : public FFmpegMulti::Core::Job {
public:
    ExtractFramesJob() = default;
    explicit ExtractFramesJob(const ExtractFramesConfig& config);

    void setConfig(const ExtractFramesConfig& config);
    ExtractFramesConfig& config();
    const ExtractFramesConfig& config() const;

    bool execute() override;

    std::vector<std::string> buildCommand() const;
    std::string getCommandString() const;

private:
    ExtractFramesConfig config_;

    bool validatePaths() const;
    bool createOutputDirectory() const;
    std::string getOutputPattern() const;
    std::string getFileExtension() const;
};

/**
 * @brief Builder pour ExtractFramesJob
 */
class ExtractFramesBuilder {
public:
    ExtractFramesBuilder() = default;

    ExtractFramesBuilder& input(const std::string& path);
    ExtractFramesBuilder& outputDir(const std::string& path);
    ExtractFramesBuilder& createSubfolder(bool create);
    ExtractFramesBuilder& subfolderName(const std::string& name);
    ExtractFramesBuilder& format(ImageFormat fmt);

    ExtractFramesBuilder& png();
    ExtractFramesBuilder& tiff();
    ExtractFramesBuilder& jpeg();

    ExtractFramesJob build() const;

private:
    ExtractFramesConfig config_;
};

} // namespace Jobs
} // namespace FFmpegMulti
