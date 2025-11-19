#pragma once

#include <string>
#include <vector>
#include "encode_types.hpp"
#include "../core/job.hpp"

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Format de conteneur pour la vidéo de sortie
 */
enum class ContainerFormat {
    MKV, // Matroska
    WEBM, // WebM
    MP4, // MPEG-4
    MOV // QuickTime
};

/**
 * @brief Configuration pour l'encodage d'images vers vidéo
 */
struct EncodeConfig {
    std::string input_dir; // Dossier contenant les images
    std::string output_dir; // Dossier de sortie
    std::string output_filename; // Nom du fichier vidéo de sortie
    ContainerFormat format{ContainerFormat::MKV};
    Encode::Codec codec{Encode::Codec::X264};

    int quality{23}; // CRF/QP par défaut
    std::string preset{"medium"}; // Preset d'encodage
    int framerate{24}; // FPS par défaut
    std::string input_pattern{"%08d.png"}; // Pattern des images (ex: 00000001.png)
};

/**
 * @brief Job d'encodage d'images vers vidéo
 */
class EncodeJob : public FFmpegMulti::Core::Job {
public:
    EncodeJob() = default;
    explicit EncodeJob(const EncodeConfig& config);

    void setConfig(const EncodeConfig& config);
    EncodeConfig& config();
    const EncodeConfig& config() const;

    bool execute() override;

    std::vector<std::string> buildCommand() const;
    std::string getCommandString() const;

private:
    EncodeConfig config_;

    bool validatePaths() const;
    std::string getOutputPath() const;
    std::string getContainerExtension() const;
    std::string getCodecName() const;
    void addCodecSpecificArgs(std::vector<std::string>& args) const;
};

/**
 * @brief Builder pour EncodeJob
 */
class EncodeJobBuilder {
public:
    EncodeJobBuilder() = default;

    EncodeJobBuilder& inputDir(const std::string& path);
    EncodeJobBuilder& outputDir(const std::string& path);
    EncodeJobBuilder& outputFilename(const std::string& name);
    EncodeJobBuilder& inputPattern(const std::string& pattern);
    EncodeJobBuilder& framerate(int fps);

    EncodeJobBuilder& format(ContainerFormat fmt);
    EncodeJobBuilder& mkv();
    EncodeJobBuilder& webm();
    EncodeJobBuilder& mp4();
    EncodeJobBuilder& mov();

    EncodeJobBuilder& codec(Encode::Codec c);
    EncodeJobBuilder& x264();
    EncodeJobBuilder& x265();
    EncodeJobBuilder& av1();
    EncodeJobBuilder& h264_nvenc();
    EncodeJobBuilder& h265_nvenc();
    EncodeJobBuilder& prores();
    EncodeJobBuilder& ffv1();

    EncodeJobBuilder& quality(int crf);
    EncodeJobBuilder& preset(const std::string& p);

    EncodeJob build() const;

private:
    EncodeConfig config_;
};

} // namespace Jobs
} // namespace FFmpegMulti
