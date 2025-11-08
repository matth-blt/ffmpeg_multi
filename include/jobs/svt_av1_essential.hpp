#pragma once

#include <string>
#include <filesystem>

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Job d'encodage avec SVT-AV1-Essential via Auto-Boost
 * 
 * Utilise Auto-Boost-Essential.py pour un encodage AV1 optimisé
 * avec gestion automatique de l'audio et du muxing final.
 */
class SvtAv1EssentialJob {
public:
    enum class Quality {
        LOW, // Rapide, qualité acceptable
        MEDIUM, // Équilibré
        HIGH // Lent, excellente qualité
    };
    
    struct Config {
        std::string input_path;
        std::string output_path;
        Quality quality{Quality::HIGH};
        bool aggressive{false}; // Mode agressif (plus de compression)
        bool unshackle{false}; // Libère les limites de vitesse
        bool verbose{true}; // Affichage détaillé
        bool cleanup{true}; // Nettoyage auto des fichiers temp
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

    bool execute();
    
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
    std::filesystem::path getExternPath() const;
};

/**
 * @brief Builder pour SvtAv1EssentialJob
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
