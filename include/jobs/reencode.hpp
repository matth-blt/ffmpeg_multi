#pragma once

#include <string>
#include <vector>
#include <memory>
#include "encode_types.hpp"

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Job de réencodage vidéo avec FFmpeg
 * 
 * Cette classe représente un job de réencodage complet :
 * - Configuration des paramètres de réencodage
 * - Construction de la ligne de commande FFmpeg
 * - Exécution du réencodage
 */
class ReencodeJob {
public:
    /**
     * @brief Constructeur avec chemins d'entrée et de sortie
     * @param input_path Chemin du fichier d'entrée
     * @param output_path Chemin du fichier de sortie
     */
    explicit ReencodeJob(const std::string& input_path, const std::string& output_path);
    
    /**
     * @brief Constructeur par défaut
     */
    ReencodeJob() = default;
    
    ~ReencodeJob() = default;
    
    // ========================================================================
    // CONFIGURATION
    // ========================================================================
    
    /**
     * @brief Définit la configuration d'encodage complète
     * @param config Configuration à appliquer
     */
    void setConfig(const Encode::EncodeConfig& config);
    
    /**
     * @brief Récupère la configuration actuelle (modifiable)
     * @return Référence vers la configuration
     */
    Encode::EncodeConfig& config();
    
    /**
     * @brief Récupère la configuration actuelle (lecture seule)
     * @return Référence const vers la configuration
     */
    const Encode::EncodeConfig& config() const;
    
    // ========================================================================
    // CHEMINS D'ENTRÉE/SORTIE
    // ========================================================================
    
    void setInputPath(const std::string& path);
    void setOutputPath(const std::string& path);
    
    std::string getInputPath() const;
    std::string getOutputPath() const;
    
    // ========================================================================
    // CONSTRUCTION DE LA COMMANDE
    // ========================================================================
    
    /**
     * @brief Construit les arguments de la ligne de commande FFmpeg
     * @return Vecteur d'arguments prêts pour l'exécution
     */
    std::vector<std::string> buildCommand() const;
    
    /**
     * @brief Génère la commande complète sous forme de chaîne (pour debug)
     * @return Commande FFmpeg complète
     */
    std::string getCommandString() const;
    
    // ========================================================================
    // EXÉCUTION
    // ========================================================================
    
    /**
     * @brief Exécute l'encodage avec la configuration actuelle
     * @return true si l'encodage a réussi, false sinon
     */
    bool execute();
    
    /**
     * @brief Valide la configuration avant exécution
     * @return true si la configuration est valide
     * @throw std::runtime_error si la configuration est invalide
     */
    bool validate() const;

private:
    std::string input_path_{};
    std::string output_path_{};
    Encode::EncodeConfig config_{};
    
    // ========================================================================
    // MÉTHODES PRIVÉES DE CONSTRUCTION DE COMMANDE
    // ========================================================================
    
    void addInputArgs(std::vector<std::string>& args) const;
    void addVideoCodecArgs(std::vector<std::string>& args) const;
    void addRateControlArgs(std::vector<std::string>& args) const;
    void addEncodingParams(std::vector<std::string>& args) const;
    void addPixelFormatArgs(std::vector<std::string>& args) const;
    void addColorSpaceArgs(std::vector<std::string>& args) const;
    void addHDRMetadata(std::vector<std::string>& args) const;
    void addAudioArgs(std::vector<std::string>& args) const;
    void addOutputArgs(std::vector<std::string>& args) const;
    
    // ========================================================================
    // HELPERS DE CONVERSION
    // ========================================================================
    
    std::string getEncoderName() const;
    std::string getPixelFormatString() const;
    std::string getColorRangeString() const;
    std::string getColorPrimariesString() const;
    std::string getTransferString() const;
    std::string getColorMatrixString() const;
};

} // namespace Jobs
} // namespace FFmpegMulti
