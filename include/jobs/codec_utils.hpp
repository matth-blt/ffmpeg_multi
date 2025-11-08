#pragma once

#include <string>
#include <vector>
#include "encode_types.hpp"

namespace FFmpegMulti {
namespace Codec {

/**
 * @brief Utilitaires partagés pour la gestion des codecs
 */
class CodecUtils {
public:
    // ========================================================================
    // CODEC MAPPING
    // ========================================================================
    
    /**
     * @brief Récupère le nom de l'encodeur FFmpeg correspondant au codec
     * @param codec Type de codec
     * @param encoder_override Encodeur personnalisé (optionnel)
     * @return Nom de l'encodeur FFmpeg
     */
    static std::string getEncoderName(
        Encode::Codec codec, 
        const std::string& encoder_override = ""
    );
    
    // ========================================================================
    // CODEC ARGUMENTS
    // ========================================================================
    
    /**
     * @brief Ajoute les arguments spécifiques au codec dans le vecteur d'args
     * @param args Vecteur d'arguments FFmpeg à remplir
     * @param codec Type de codec
     * @param quality Valeur de qualité (CRF/QP)
     * @param preset Preset d'encodage
     */
    static void addCodecArgs(
        std::vector<std::string>& args,
        Encode::Codec codec,
        int quality,
        const std::string& preset
    );
    
    /**
     * @brief Ajoute les arguments pour ProRes
     */
    static void addProResArgs(
        std::vector<std::string>& args,
        int profile = 4,
        const std::string& vendor = "apl0",
        int bits_per_mb = 8000
    );
    
    /**
     * @brief Ajoute les arguments pour FFV1
     */
    static void addFFV1Args(
        std::vector<std::string>& args,
        int level = 3,
        int coder = 1,
        int context = 1,
        int slices = 24
    );
    
    // ========================================================================
    // CONTAINER UTILS
    // ========================================================================
    
    /**
     * @brief Récupère l'extension du fichier selon le format de conteneur
     * @param format Format de conteneur (MKV, MP4, WEBM, MOV)
     * @return Extension avec le point (ex: ".mkv")
     */
    static std::string getContainerExtension(const std::string& container);
    
    /**
     * @brief Vérifie si un codec est compatible avec un conteneur
     * @param codec Type de codec
     * @param container Format de conteneur
     * @return true si compatible, false sinon
     */
    static bool isCodecCompatibleWithContainer(
        Encode::Codec codec, 
        const std::string& container
    );
    
    // ========================================================================
    // VALIDATION
    // ========================================================================
    
    /**
     * @brief Valide les paramètres de qualité selon le codec
     * @param codec Type de codec
     * @param quality Valeur de qualité
     * @return true si valide, false sinon
     */
    static bool validateQuality(Encode::Codec codec, int quality);
    
    /**
     * @brief Valide le preset selon le codec
     * @param codec Type de codec
     * @param preset Preset d'encodage
     * @return true si valide, false sinon
     */
    static bool validatePreset(Encode::Codec codec, const std::string& preset);
};

} // namespace Codec
} // namespace FFmpegMulti
