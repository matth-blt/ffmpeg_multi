#include "../../include/jobs/encode.hpp"
#include "../../include/jobs/codec_utils.hpp"
#include "../../include/core/path_utils.hpp"
#include "../../include/core/ffmpeg_process.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONSTRUCTEURS
// ============================================================================

EncodeJob::EncodeJob(const EncodeConfig& config) : config_(config) {}

// ============================================================================
// CONFIGURATION
// ============================================================================

void EncodeJob::setConfig(const EncodeConfig& config) {
    config_ = config;
}

EncodeConfig& EncodeJob::config() {
    return config_;
}

const EncodeConfig& EncodeJob::config() const {
    return config_;
}

// ============================================================================
// HELPERS
// ============================================================================

bool EncodeJob::validatePaths() const {
    if (!fs::exists(config_.input_dir)) {
        std::cerr << "Erreur : Le dossier d'entrée n'existe pas : " << config_.input_dir << std::endl;
        return false;
    }

    if (config_.output_dir.empty()) {
        std::cerr << "Erreur : Le dossier de sortie n'est pas défini." << std::endl;
        return false;
    }

    if (config_.output_filename.empty()) {
        std::cerr << "Erreur : Le nom du fichier de sortie n'est pas défini." << std::endl;
        return false;
    }

    return true;
}

std::string EncodeJob::getOutputPath() const {
    std::string extension = getContainerExtension();
    std::string filename = config_.output_filename;
    
    // Ajouter l'extension si elle n'est pas déjà présente
    if (filename.find(extension) == std::string::npos) {
        filename += extension;
    }
    
    return (fs::path(config_.output_dir) / filename).string();
}

std::string EncodeJob::getContainerExtension() const {
    switch (config_.format) {
        case ContainerFormat::MKV: return ".mkv";
        case ContainerFormat::WEBM: return ".webm";
        case ContainerFormat::MP4: return ".mp4";
        case ContainerFormat::MOV: return ".mov";
        default: return ".mkv";
    }
}

std::string EncodeJob::getCodecName() const {
    return Codec::CodecUtils::getEncoderName(config_.codec);
}

void EncodeJob::addCodecSpecificArgs(std::vector<std::string>& args) const {
    Codec::CodecUtils::addCodecArgs(args, config_.codec, config_.quality, config_.preset);
}

// ============================================================================
// CONSTRUCTION DE LA COMMANDE
// ============================================================================

std::vector<std::string> EncodeJob::buildCommand() const {
    std::vector<std::string> args;

    // Options globales
    args.push_back("-hide_banner");
    
    // Framerate
    args.push_back("-framerate");
    args.push_back(std::to_string(config_.framerate));
    
    // Pattern d'entrée
    args.push_back("-i");
    std::string input_path = (fs::path(config_.input_dir) / config_.input_pattern).string();
    args.push_back(input_path);
    
    // Codec vidéo
    args.push_back("-c:v");
    args.push_back(getCodecName());
    
    // Paramètres spécifiques au codec
    addCodecSpecificArgs(args);
    
    // Pixel format par défaut (sauf pour ProRes et FFV1 qui ont leur propre pix_fmt)
    if (config_.codec != Encode::Codec::ProRes && config_.codec != Encode::Codec::FFV1) {
        args.push_back("-pix_fmt");
        args.push_back("yuv420p");
    }
    
    // Fichier de sortie
    args.push_back(getOutputPath());

    return args;
}

std::string EncodeJob::getCommandString() const {
    auto args = buildCommand();
    std::ostringstream oss;
    oss << "ffmpeg";
    for (const auto& arg : args) {
        oss << " ";
        if (arg.find(' ') != std::string::npos) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }
    return oss.str();
}

// ============================================================================
// EXÉCUTION
// ============================================================================

bool EncodeJob::execute() {
    if (!validatePaths()) {
        return false;
    }

    // Créer le dossier de sortie si nécessaire
    try {
        if (!fs::exists(config_.output_dir)) {
            fs::create_directories(config_.output_dir);
            std::cout << "[INFO] Dossier de sortie créé : " << config_.output_dir << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Impossible de créer le dossier de sortie : " << e.what() << std::endl;
        return false;
    }

    // Construction de la commande
    auto args = buildCommand();
    
    // Log de la commande (comme dans reencode.cpp)
    std::cout << "[INFO] Encode command: " << getCommandString() << std::endl;
    
    // Exécution via FFmpegProcess
    std::filesystem::path extern_path = FFmpegMulti::PathUtils::getExternPath();
    std::filesystem::path ffmpeg_path = extern_path / "ffmpeg.exe";
    
    ffmpegProcess process(ffmpeg_path, args);
    bool success = process.execute();

    if (success) {
        std::cout << "[SUCCESS] Encodage terminé avec succès!" << std::endl;
        std::cout << "[INFO] Fichier créé : " << getOutputPath() << std::endl;
    } else {
        std::cerr << "[ERROR] L'encodage a échoué!" << std::endl;
    }

    return success;
}

} // namespace Jobs
} // namespace FFmpegMulti