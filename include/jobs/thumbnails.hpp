#pragma once

#include <string>
#include <vector>

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Format d'image pour les thumbnails
 */
enum class ThumbnailFormat {
    PNG,
    TIFF,
    JPEG
};

/**
 * @brief Configuration pour l'extraction de thumbnails avec détection de scènes
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
 * @brief Job d'extraction de thumbnails avec détection de scènes
 * 
 * Ce job extrait automatiquement des thumbnails uniquement lors des changements de scènes,
 * évitant ainsi les images dupliquées ou très similaires.
 */
class ThumbnailsJob {
public:
    ThumbnailsJob() = default;
    explicit ThumbnailsJob(const ThumbnailsConfig& config);

    // Configuration
    void setConfig(const ThumbnailsConfig& config);
    ThumbnailsConfig& config();
    const ThumbnailsConfig& config() const;

    // Exécution
    bool execute();

    // Construction de la commande
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
 * @brief Builder pour ThumbnailsJob
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
