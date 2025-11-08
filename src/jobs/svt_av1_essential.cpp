#include "../../include/jobs/svt_av1_essential.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONSTRUCTEUR
// ============================================================================

SvtAv1EssentialJob::SvtAv1EssentialJob(const std::string& input, const std::string& output) {
    config_.input_path = input;
    config_.output_path = output;
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void SvtAv1EssentialJob::setQuality(Quality q) {
    config_.quality = q;
}

void SvtAv1EssentialJob::setAggressive(bool enabled) {
    config_.aggressive = enabled;
}

void SvtAv1EssentialJob::setUnshackle(bool enabled) {
    config_.unshackle = enabled;
}

void SvtAv1EssentialJob::setVerbose(bool enabled) {
    config_.verbose = enabled;
}

void SvtAv1EssentialJob::setCleanup(bool enabled) {
    config_.cleanup = enabled;
}

SvtAv1EssentialJob::Config& SvtAv1EssentialJob::config() {
    return config_;
}

const SvtAv1EssentialJob::Config& SvtAv1EssentialJob::config() const {
    return config_;
}

// ============================================================================
// HELPERS
// ============================================================================

std::string SvtAv1EssentialJob::getQualityString() const {
    switch (config_.quality) {
        case Quality::LOW:     return "low";
        case Quality::MEDIUM:  return "medium";
        case Quality::HIGH:    return "high";
        default:               return "high";
    }
}

std::filesystem::path SvtAv1EssentialJob::getExternPath() const {
    // Obtenir le chemin de l'exécutable actuel
    std::filesystem::path exe_path = std::filesystem::current_path();
    
    // Remonter au répertoire racine du projet
    // Si on est dans build/, remonter d'un niveau
    if (exe_path.filename() == "build" || exe_path.filename() == "Release" || exe_path.filename() == "Debug") {
        exe_path = exe_path.parent_path();
    }
    
    // Vérifier si on est dans un sous-dossier de build
    if (exe_path.parent_path().filename() == "build") {
        exe_path = exe_path.parent_path().parent_path();
    }
    
    return exe_path / "extern";
}

std::filesystem::path SvtAv1EssentialJob::getTempDir() const {
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::string stem = input.stem().string();
    return parent / stem; // Dossier du même nom que la vidéo
}

std::filesystem::path SvtAv1EssentialJob::getAviPath() const {
    // Auto-Boost génère le .ivf à côté de la vidéo source, pas dans le sous-dossier
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::string stem = input.stem().string();
    return parent / (stem + ".ivf");
}

std::filesystem::path SvtAv1EssentialJob::getAudioPath() const {
    // Mettre l'audio à côté de la vidéo source, pas dans le dossier qui sera écrasé par Auto-Boost
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::string stem = input.stem().string();
    return parent / (stem + "_audio.mka");
}

std::string SvtAv1EssentialJob::buildABECommand() const {
    std::ostringstream cmd;
    
    std::filesystem::path extern_path = getExternPath();
    std::filesystem::path abe_script = extern_path / "scripts" / "ABE.ps1";
    
    // Convertir les chemins avec forward slashes
    std::string abe_str = abe_script.string();
    std::string input_str = config_.input_path;
    
    std::replace(abe_str.begin(), abe_str.end(), '\\', '/');
    std::replace(input_str.begin(), input_str.end(), '\\', '/');
    
    // Commande PowerShell (utiliser -inputFile au lieu de -input)
    cmd << "powershell -ExecutionPolicy Bypass -File \"" << abe_str << "\"";
    cmd << " -inputFile \"" << input_str << "\"";
    cmd << " -quality " << getQualityString();
    
    if (config_.aggressive) {
        cmd << " -aggressive";
    }
    
    if (config_.unshackle) {
        cmd << " -unshackle";
    }
    
    if (config_.verbose) {
        cmd << " -verboseOutput";
    }
    
    return cmd.str();
}

// ============================================================================
// VALIDATION
// ============================================================================

bool SvtAv1EssentialJob::validatePaths() {
    // Vérifier que le fichier d'entrée existe
    if (!std::filesystem::exists(config_.input_path)) {
        std::cerr << "[ERROR] Le fichier d'entrée n'existe pas: " 
                  << config_.input_path << std::endl;
        return false;
    }
    
    // Vérifier que le répertoire de sortie existe
    std::filesystem::path output(config_.output_path);
    std::filesystem::path output_dir = output.parent_path();
    
    if (!output_dir.empty() && !std::filesystem::exists(output_dir)) {
        std::cout << "[INFO] Création du répertoire de sortie: " 
                  << output_dir << std::endl;
        std::filesystem::create_directories(output_dir);
    }
    
    return true;
}

// ============================================================================
// ÉTAPE 1: EXTRACTION AUDIO
// ============================================================================

bool SvtAv1EssentialJob::extractAudio() {
    std::cout << "\n[ÉTAPE 1/4] Extraction de l'audio..." << std::endl;
    std::cout << "============================================" << std::endl;
    
    // Créer le dossier temporaire
    std::filesystem::path temp_dir = getTempDir();
    if (!std::filesystem::exists(temp_dir)) {
        std::filesystem::create_directories(temp_dir);
    }
    
    std::filesystem::path audio_path = getAudioPath();
    std::filesystem::path extern_path = getExternPath();
    std::filesystem::path ffmpeg_exe = extern_path / "ffmpeg.exe";
    
    // Convertir les chemins en string avec des slashes normaux pour la commande
    std::string ffmpeg_str = ffmpeg_exe.string();
    std::string input_str = config_.input_path;
    std::string audio_str = audio_path.string();
    
    // Remplacer les backslashes par des forward slashes
    std::replace(ffmpeg_str.begin(), ffmpeg_str.end(), '\\', '/');
    std::replace(input_str.begin(), input_str.end(), '\\', '/');
    std::replace(audio_str.begin(), audio_str.end(), '\\', '/');
    
    // Commande FFmpeg pour extraire l'audio
    // NE PAS mettre de guillemets autour de l'exécutable sous Windows
    std::ostringstream cmd;
    cmd << ffmpeg_str
        << " -i \"" << input_str << "\""
        << " -vn" // Pas de vidéo
        << " -c:a copy" // Copier l'audio sans réencodage
        << " \"" << audio_str << "\""
        << " -y"; // Écraser si existe
    
    std::cout << "[CMD] " << cmd.str() << std::endl;
    
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        std::cerr << "[ERROR] Échec de l'extraction audio" << std::endl;
        return false;
    }
    
    std::cout << "[OK] Audio extrait: " << audio_path << std::endl;
    return true;
}

// ============================================================================
// ÉTAPE 2: ENCODAGE AUTO-BOOST
// ============================================================================

bool SvtAv1EssentialJob::runAutoBoost() {
    std::cout << "\n[ÉTAPE 2/4] Encodage SVT-AV1 via Auto-Boost..." << std::endl;
    std::cout << "============================================" << std::endl;
    
    std::string cmd = buildABECommand();
    std::cout << "[CMD] " << cmd << std::endl;
    std::cout << "\n⏳ Encodage en cours (cela peut prendre du temps)...\n" << std::endl;
    
    int result = std::system(cmd.c_str());
    
    if (result != 0) {
        std::cerr << "[ERROR] Échec de l'encodage Auto-Boost" << std::endl;
        return false;
    }
    
    // Vérifier que le fichier IVF a été créé
    std::filesystem::path ivf_path = getAviPath();
    if (!std::filesystem::exists(ivf_path)) {
        std::cerr << "[ERROR] Le fichier IVF n'a pas été généré: " 
                  << ivf_path << std::endl;
        return false;
    }
    
    std::cout << "[OK] Encodage terminé: " << ivf_path << std::endl;
    return true;
}

// ============================================================================
// ÉTAPE 3: MUXING FINAL
// ============================================================================

bool SvtAv1EssentialJob::muxFinal() {
    std::cout << "\n[ÉTAPE 3/4] Muxing final (vidéo AV1 + audio)..." << std::endl;
    std::cout << "============================================" << std::endl;
    
    std::filesystem::path ivf_path = getAviPath();  // C'est un .ivf maintenant
    std::filesystem::path audio_path = getAudioPath();
    
    // Créer output_temp.mkv à côté de la vidéo source, pas dans le sous-dossier
    std::filesystem::path input(config_.input_path);
    std::filesystem::path parent = input.parent_path();
    std::filesystem::path temp_mkv = parent / "output_temp.mkv";
    
    std::filesystem::path extern_path = getExternPath();
    std::filesystem::path mkvmerge_exe = extern_path / "env" / "mkvtoolnix" / "mkvmerge.exe";
    
    // Convertir les chemins en string avec des slashes normaux
    std::string mkvmerge_str = mkvmerge_exe.string();
    std::string temp_mkv_str = temp_mkv.string();
    std::string ivf_str = ivf_path.string();
    std::string audio_str = audio_path.string();
    
    std::replace(mkvmerge_str.begin(), mkvmerge_str.end(), '\\', '/');
    std::replace(temp_mkv_str.begin(), temp_mkv_str.end(), '\\', '/');
    std::replace(ivf_str.begin(), ivf_str.end(), '\\', '/');
    std::replace(audio_str.begin(), audio_str.end(), '\\', '/');
    
    // Commande mkvmerge pour fusionner vidéo et audio
    // NE PAS mettre de guillemets autour de l'exécutable sous Windows
    std::ostringstream cmd;
    cmd << mkvmerge_str
        << " -o \"" << temp_mkv_str << "\""
        << " \"" << ivf_str << "\""
        << " \"" << audio_str << "\"";
    
    std::cout << "[CMD] " << cmd.str() << std::endl;
    
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        std::cerr << "[ERROR] Échec du muxing" << std::endl;
        return false;
    }
    
    // Déplacer le fichier final vers la destination
    try {
        std::filesystem::copy_file(
            temp_mkv, 
            config_.output_path,
            std::filesystem::copy_options::overwrite_existing
        );
        std::cout << "[OK] Fichier final créé: " << config_.output_path << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Échec de la copie du fichier final: " 
                  << e.what() << std::endl;
        return false;
    }
    
    return true;
}

// ============================================================================
// ÉTAPE 4: NETTOYAGE
// ============================================================================

bool SvtAv1EssentialJob::cleanup() {
    if (!config_.cleanup) {
        std::cout << "\n[ÉTAPE 4/4] Nettoyage désactivé, fichiers temporaires conservés." << std::endl;
        return true;
    }
    
    std::cout << "\n[ÉTAPE 4/4] Nettoyage des fichiers temporaires..." << std::endl;
    std::cout << "============================================" << std::endl;
    
    std::filesystem::path temp_dir = getTempDir();
    
    try {
        if (std::filesystem::exists(temp_dir)) {
            std::filesystem::remove_all(temp_dir);
            std::cout << "[OK] Dossier temporaire supprimé: " << temp_dir << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[WARNING] Impossible de supprimer les fichiers temporaires: " 
                  << e.what() << std::endl;
        return false;
    }
    
    return true;
}

// ============================================================================
// EXÉCUTION PRINCIPALE
// ============================================================================

bool SvtAv1EssentialJob::execute() {
    std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   SVT-AV1-ESSENTIAL via Auto-Boost             ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n📄 Configuration:" << std::endl;
    std::cout << "  • Entrée    : " << config_.input_path << std::endl;
    std::cout << "  • Sortie    : " << config_.output_path << std::endl;
    std::cout << "  • Qualité   : " << getQualityString() << std::endl;
    std::cout << "  • Aggressive: " << (config_.aggressive ? "Oui" : "Non") << std::endl;
    std::cout << "  • Unshackle : " << (config_.unshackle ? "Oui" : "Non") << std::endl;
    std::cout << "  • Cleanup   : " << (config_.cleanup ? "Oui" : "Non") << std::endl;
    
    try {
        // Validation
        if (!validatePaths()) {
            return false;
        }
        
        // Étape 1: Extraction audio
        if (!extractAudio()) {
            return false;
        }
        
        // Étape 2: Encodage Auto-Boost
        if (!runAutoBoost()) {
            return false;
        }
        
        // Étape 3: Muxing final
        if (!muxFinal()) {
            return false;
        }
        
        // Étape 4: Nettoyage
        cleanup();
        
        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║   ✅ ENCODAGE TERMINÉ AVEC SUCCÈS              ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
        std::cout << "\n📁 Fichier de sortie: " << config_.output_path << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] Exception: " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// BUILDER IMPLEMENTATION
// ============================================================================

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::input(const std::string& path) {
    config_.input_path = path;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::output(const std::string& path) {
    config_.output_path = path;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::quality(SvtAv1EssentialJob::Quality q) {
    config_.quality = q;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::aggressive() {
    config_.aggressive = true;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::unshackle() {
    config_.unshackle = true;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::verbose(bool enabled) {
    config_.verbose = enabled;
    return *this;
}

SvtAv1EssentialBuilder& SvtAv1EssentialBuilder::cleanup(bool enabled) {
    config_.cleanup = enabled;
    return *this;
}

SvtAv1EssentialJob SvtAv1EssentialBuilder::build() {
    if (config_.input_path.empty()) {
        throw std::runtime_error("Input path is required");
    }
    if (config_.output_path.empty()) {
        throw std::runtime_error("Output path is required");
    }
    
    SvtAv1EssentialJob job(config_.input_path, config_.output_path);
    job.config() = config_;
    return job;
}

} // namespace Jobs
} // namespace FFmpegMulti
