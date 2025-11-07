#include <iostream>
#include <sstream>
#include <iomanip>

#include "../../include/core/app.hpp"
#include "../../include/core/string_utils.hpp"
#include "../../include/jobs/encode.hpp"
#include "../../include/jobs/encode_builder.hpp"
#include "../../include/jobs/svt_av1_essential.hpp"

using namespace FFmpegMulti::Jobs;
using namespace FFmpegMulti::Encode;

namespace App {

// ============================================================================
// ANSI COLOR CODES - Catppuccin Mocha Theme
// ============================================================================
namespace Colors {
    // Style codes
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* DIM = "\033[2m";
    
    // Catppuccin Mocha colors (using closest ANSI equivalents)
    const char* LAVENDER = "\033[96m";      // Bright Cyan - Titres principaux
    const char* BLUE = "\033[34m";          // Blue - Bordures
    const char* MAUVE = "\033[95m";         // Bright Magenta - Numéros
    const char* TEXT = "\033[37m";          // White - Texte normal
    const char* SUBTEXT = "\033[90m";       // Bright Black/Gray - Sous-texte
    const char* GREEN = "\033[92m";         // Bright Green - Succès
    const char* RED = "\033[91m";           // Bright Red - Erreurs
    const char* YELLOW = "\033[93m";        // Bright Yellow - Avertissements
    const char* PEACH = "\033[33m";         // Yellow/Orange - Prompts
    const char* SAPPHIRE = "\033[96m";      // Bright Cyan - Infos
    const char* TEAL = "\033[36m";          // Cyan - Fichiers
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================
void printHeader(const std::string& title) {
    std::cout << Colors::BLUE << "╔══════════════════════════════════════════════╗" << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "║  " << Colors::BOLD << Colors::LAVENDER << title;
    std::cout << "                       " << Colors::RESET << Colors::BLUE << "║" << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "╚══════════════════════════════════════════════╝" << Colors::RESET << std::endl;
}

void printSeparator() {
    std::cout << Colors::BLUE << "────────────────────────────────────────────────" << Colors::RESET << std::endl;
}

void printOption(int id, const std::string& description) {
    std::cout << Colors::BLUE << "│ " << Colors::MAUVE << Colors::BOLD;
    if (id < 10) std::cout << " ";
    std::cout << id << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << description;
    
    int baseLength = 9;
    int totalWidth = 45;
    int descLength = 0;

    for (char c : description) {
        if ((unsigned char)c >= 0x80) {
            descLength += 2;
        } else {
            descLength += 1;
        }
    }
    
    std::string padding;
    if (description == "Extraire des frames") padding = "                   ";
    else if (description == "Encoder une vidéo") padding = "                     ";
    else if (description == "Réencoder un fichier") padding = "                  ";
    else if (description == "Concaténer plusieurs vidéos") padding = "           ";
    else if (description == "Générer des miniatures") padding = "                ";
    else if (description == "Encoder avec SVT-AV1-Essential") padding = "        ";
    else if (description == "Analyser un média (ffprobe)") padding = "           ";
    else if (description == "Quitter") padding = "                               ";
    else padding = std::string(totalWidth - baseLength - description.length(), ' ');
    
    std::cout << padding << Colors::BLUE << "│" << Colors::RESET << std::endl;
}

// Fonction pour demander les fichiers d'entrée et de sortie
void promptFiles(std::string& inputFile, std::string& outputFile, const std::string& outputHint = "") {
    std::cout << Colors::TEAL << "📁 Fichier d'entrée : " << Colors::RESET;
    std::cin >> std::ws;
    std::getline(std::cin, inputFile);
    inputFile = StringUtils::clean(inputFile);
    
    std::cout << Colors::TEAL << "📁 Fichier de sortie";
    if (!outputHint.empty()) {
        std::cout << " " << outputHint;
    }
    std::cout << " : " << Colors::RESET;
    std::getline(std::cin, outputFile);
    outputFile = StringUtils::clean(outputFile);
}

// Fonction pour demander un CRF/QP
int promptQuality(const std::string& type, const std::string& examples) {
    std::cout << Colors::TEXT << "  Qualité " << type << " " << Colors::SUBTEXT << examples << Colors::RESET << std::endl;
    std::cout << Colors::PEACH << "👉 Valeur : " << Colors::RESET;
    int value;
    std::cin >> value;
    return value;
}

// Fonction pour demander un preset
std::string promptPreset(const std::string& options) {
    std::cout << Colors::TEXT << "  Preset " << Colors::SUBTEXT << options << Colors::RESET << std::endl;
    std::cout << Colors::PEACH << "👉 Choix : " << Colors::RESET;
    std::string preset;
    std::cin >> preset;
    return preset;
}

// Fonction pour confirmer et exécuter un job d'encodage
template<typename JobType> bool confirmAndExecute(JobType& job, const std::string& outputFile = "") {
    std::cout << std::endl;
    std::cout << Colors::PEACH << "❓ Lancer l'encodage ? (o/n) : " << Colors::RESET;
    char confirm;
    std::cin >> confirm;
    
    if (confirm == 'o' || confirm == 'O') {
        std::cout << std::endl;
        std::cout << Colors::BLUE << Colors::BOLD << "🚀 Lancement de l'encodage..." << Colors::RESET << std::endl;
        printSeparator();
        
        bool success = job.execute();
        
        printSeparator();
        std::cout << std::endl;
        if (success) {
            std::cout << Colors::GREEN << Colors::BOLD << "✅ Encodage terminé avec succès!" << Colors::RESET << std::endl;
            if (!outputFile.empty()) {
                std::cout << Colors::TEAL << "📁 Fichier de sortie : " << Colors::TEXT << outputFile << Colors::RESET << std::endl;
            }
        } else {
            std::cerr << Colors::RED << Colors::BOLD << "❌ L'encodage a échoué. Vérifiez les erreurs ci-dessus." << Colors::RESET << std::endl;
        }
        return success;
    } else {
        std::cout << std::endl;
        std::cout << Colors::YELLOW << "🚫 Encodage annulé." << Colors::RESET << std::endl;
        return false;
    }
}

// Fonction pour gérer les erreurs
void handleError(const std::exception& e) {
    std::cerr << std::endl;
    std::cerr << Colors::RED << Colors::BOLD << "❌ Erreur : " << Colors::RESET << Colors::RED << e.what() << Colors::RESET << std::endl;
}

void affiche() {
    std::cout << std::endl;
    printHeader("⚙️  MULTI-FFMPEG TOOL");
    std::cout << std::endl;
    
    std::cout << Colors::BLUE << "┌─────┬────────────────────────────────────────┐" << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "│" << Colors::MAUVE << Colors::BOLD << "ID" << Colors::RESET << Colors::BLUE << "   │  " << Colors::TEXT << "Action                                " << Colors::BLUE << "│" << Colors::RESET << std::endl;
    std::cout << Colors::BLUE << "├─────┼────────────────────────────────────────┤" << Colors::RESET << std::endl;
    
    printOption(1, "Extraire des frames");
    printOption(2, "Encoder une vidéo");
    printOption(3, "Réencoder un fichier");
    printOption(4, "Concaténer plusieurs vidéos");
    printOption(5, "Générer des miniatures");
    printOption(6, "Encoder avec SVT-AV1-Essential");
    printOption(7, "Analyser un média (ffprobe)");
    
    std::cout << Colors::BLUE << "├─────┼────────────────────────────────────────┤" << Colors::RESET << std::endl;
    printOption(0, "Quitter");
    std::cout << Colors::BLUE << "└─────┴────────────────────────────────────────┘" << Colors::RESET << std::endl;
    
    std::cout << std::endl;
    std::cout << Colors::PEACH << "👉  Entrez votre choix : " << Colors::RESET;
}

void choice() {
    int userChoice{};
    std::cin >> userChoice;
    std::cout << std::endl;
    
    switch (userChoice) {
        case 2: {
            std::string inputFile, outputFile;
            int codecChoice, qualityChoice;
            std::string presetChoice;

            printHeader("⚙️  ENCODER UNE VIDEO");
            std::cout << std::endl;

            // Demander les fichiers d'entrée et de sortie
            promptFiles(inputFile, outputFile);
            
            std::cout << std::endl;
            
            // Menu de choix de codec
            std::cout << Colors::LAVENDER << Colors::BOLD << "🎬 Choisissez un codec :" << Colors::RESET << std::endl;
            printSeparator();
            
            std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " X264 " << Colors::SUBTEXT << "- Universel, bonne compatibilité" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " X265 " << Colors::SUBTEXT << "- Meilleure compression, qualité supérieure" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " AV1 " << Colors::SUBTEXT << "- Compression maximale pour le web" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " H264_NVENC " << Colors::SUBTEXT << "- Hardware H.264 (NVIDIA)" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " H265_NVENC " << Colors::SUBTEXT << "- Hardware H.265 (NVIDIA)" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  6." << Colors::TEXT << " ProRes " << Colors::SUBTEXT << "- Apple ProRes 4444 (Production)" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  7." << Colors::TEXT << " FFV1 " << Colors::SUBTEXT << "- Codec lossless pour archivage" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  8." << Colors::TEXT << " Preset YouTube " << Colors::SUBTEXT << "- H.264, qualité optimale" << Colors::RESET << std::endl;
            
            printSeparator();
            std::cout << Colors::PEACH << "👉 Votre choix : " << Colors::RESET;
            std::cin >> codecChoice;
            std::cout << std::endl;
            
            try {
                EncodeJobBuilder builder;
                builder.input(inputFile).output(outputFile);
                
                // Configuration selon le choix
                switch (codecChoice) {
                    case 1: { // X264 personnalisé
                        std::cout << Colors::SAPPHIRE << Colors::BOLD << "⚙️  Configuration X264" << Colors::RESET << std::endl;
                        qualityChoice = promptQuality("CRF", "(18=excellent, 23=bon, 28=acceptable)");
                        presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                        
                        builder.x264().crf(qualityChoice).preset(presetChoice).copyAudio();
                        std::cout << Colors::GREEN << "✅ X264 configuré" << Colors::RESET << std::endl;
                        break;
                    }
                    
                    case 2: { // X265 personnalisé
                        std::cout << Colors::SAPPHIRE << Colors::BOLD << "⚙️  Configuration X265" << Colors::RESET << std::endl;
                        qualityChoice = promptQuality("CRF", "(18=excellent, 23=bon, 28=acceptable)");
                        presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                        
                        builder.x265().crf(qualityChoice).preset(presetChoice).tenBit().copyAudio();
                        std::cout << Colors::GREEN << "✅ X265 configuré (10-bit)" << Colors::RESET << std::endl;
                        break;
                    }
                    
                    case 3: { // AV1
                        std::cout << Colors::SAPPHIRE << Colors::BOLD << "⚙️  Configuration AV1" << Colors::RESET << std::endl;
                        qualityChoice = promptQuality("CRF", "(20=excellent, 30=bon, 35=acceptable)");
                        
                        builder.svtav1().crf(qualityChoice).preset("5").copyAudio();
                        std::cout << Colors::GREEN << "✅ AV1 configuré (SVT-AV1)" << Colors::RESET << std::endl;
                        break;
                    }

                    case 4: { // H264 NVENC
                        std::cout << Colors::SAPPHIRE << Colors::BOLD << "⚙️  Configuration H264_NVENC" << Colors::RESET << std::endl;
                        qualityChoice = promptQuality("QP", "(17=excellent, 22=bon, 27=acceptable)");
                        presetChoice = promptPreset("(p1/p2/p3/p4/p5/p6/p7)");

                        builder.h264_nvenc().qp(qualityChoice).preset(presetChoice).pixelFormat(FFmpegMulti::Encode::PixelFormat::P010).copyAudio();
                        std::cout << Colors::GREEN << "✅ H264_NVENC configuré (Hardware)" << Colors::RESET << std::endl;
                        break;
                    }

                    case 5: { // H265 NVENC
                        std::cout << Colors::SAPPHIRE << Colors::BOLD << "⚙️  Configuration H265_NVENC" << Colors::RESET << std::endl;
                        qualityChoice = promptQuality("QP", "(17=excellent, 22=bon, 27=acceptable)");
                        presetChoice = promptPreset("(p1/p2/p3/p4/p5/p6/p7)");

                        builder.h265_nvenc().qp(qualityChoice).preset(presetChoice).pixelFormat(FFmpegMulti::Encode::PixelFormat::P010).tune("hq").copyAudio();
                        std::cout << Colors::GREEN << "✅ H265_NVENC configuré (Hardware, HQ)" << Colors::RESET << std::endl;
                        break;
                    }

                    case 6: { // ProRes
                        int profileChoice;
                        std::cout << Colors::SAPPHIRE << Colors::BOLD << "⚙️  Profil ProRes" << Colors::RESET << std::endl;
                        std::cout << Colors::MAUVE << "  0." << Colors::TEXT << " Proxy " << Colors::SUBTEXT << "- Faible qualité, taille réduite" << Colors::RESET << std::endl;
                        std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " LT " << Colors::SUBTEXT << "- Light, qualité moyenne" << Colors::RESET << std::endl;
                        std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " Standard " << Colors::SUBTEXT << "- Bonne qualité" << Colors::RESET << std::endl;
                        std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " HQ " << Colors::SUBTEXT << "- High Quality, excellente qualité" << Colors::RESET << std::endl;
                        std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " 4444 " << Colors::SUBTEXT << "- Qualité maximale avec alpha" << Colors::RESET << std::endl;
                        std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " 4444 XQ " << Colors::SUBTEXT << "- Qualité extrême" << Colors::RESET << std::endl;
                        std::cout << Colors::PEACH << "👉 Votre choix : " << Colors::RESET;
                        std::cin >> profileChoice;
                        
                        builder.prores().proresProfile(profileChoice).copyAudio().container("mov");
                        
                        std::cout << Colors::GREEN << "✅ ProRes configuré (Profile " << profileChoice << ")" << Colors::RESET << std::endl;
                        break;
                    }
                    
                    case 7: { // FFV1
                        builder.ffv1Preset();
                        std::cout << Colors::GREEN << "✅ FFV1 configuré (Lossless, Level 3)" << Colors::RESET << std::endl;
                        break;
                    }
                    
                    case 8: { // Preset YouTube
                        builder.youtubePreset();
                        std::cout << Colors::GREEN << "✅ Preset YouTube appliqué (H.264, CRF 23, medium)" << Colors::RESET << std::endl;
                        break;
                    }
                    
                    default:
                        std::cout << Colors::YELLOW << "⚠️  Choix invalide, utilisation du preset YouTube par défaut" << Colors::RESET << std::endl;
                        builder.youtubePreset();
                        break;
                }
                
                // Construire le job
                std::cout << std::endl;
                std::cout << Colors::BLUE << "🔨 Construction du job d'encodage..." << Colors::RESET << std::endl;
                EncodeJob job = builder.build();
                
                // Afficher la commande qui sera exécutée
                std::cout << std::endl;
                std::cout << Colors::SAPPHIRE << Colors::BOLD << "📝 Commande FFmpeg" << Colors::RESET << std::endl;
                printSeparator();
                std::cout << Colors::SUBTEXT << job.getCommandString() << Colors::RESET << std::endl;
                printSeparator();
                
                // Demander confirmation et exécuter
                confirmAndExecute(job, outputFile);
                
            } catch (const std::exception& e) {
                handleError(e);
            }
            
            break;
        }
        
        case 6: { // SVT-AV1-Essential
            std::string inputFile, outputFile;
            int qualityChoice;
            char aggressive, unshackle, cleanup;

            printHeader("🚀 ENCODER SVT-AV1-ESSENTIAL");
            std::cout << std::endl;

            // Fichiers
            promptFiles(inputFile, outputFile, "(.mkv)");
            
            std::cout << std::endl;
            
            // Qualité
            std::cout << Colors::LAVENDER << Colors::BOLD << "⚙️  Qualité :" << Colors::RESET << std::endl;
            printSeparator();
            std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " Low " << Colors::SUBTEXT << "- Rapide, qualité acceptable" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " Medium " << Colors::SUBTEXT << "- Équilibré" << Colors::RESET << std::endl;
            std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " High " << Colors::SUBTEXT << "- Lent, excellente qualité" << Colors::RESET << std::endl;
            printSeparator();
            std::cout << Colors::PEACH << "👉 Votre choix : " << Colors::RESET;
            std::cin >> qualityChoice;
            
            std::cout << std::endl;
            
            // Options avancées
            std::cout << Colors::LAVENDER << Colors::BOLD << "🔧 Options avancées" << Colors::RESET << std::endl;
            printSeparator();
            std::cout << Colors::TEXT << "  Mode agressif " << Colors::SUBTEXT << "(plus de compression)" << Colors::TEXT << " ? (o/n) : " << Colors::RESET;
            std::cin >> aggressive;
            std::cout << Colors::TEXT << "  Unshackle " << Colors::SUBTEXT << "(libère les limites)" << Colors::TEXT << " ? (o/n) : " << Colors::RESET;
            std::cin >> unshackle;
            std::cout << Colors::TEXT << "  Nettoyer les fichiers temporaires ? (o/n) : " << Colors::RESET;
            std::cin >> cleanup;
            
            std::cout << std::endl;
            
            try {
                SvtAv1EssentialBuilder builder;
                builder.input(inputFile).output(outputFile);
                
                // Qualité
                switch (qualityChoice) {
                    case 1: builder.quality(SvtAv1EssentialJob::Quality::LOW); break;
                    case 2: builder.quality(SvtAv1EssentialJob::Quality::MEDIUM); break;
                    case 3: builder.quality(SvtAv1EssentialJob::Quality::HIGH); break;
                    default: builder.quality(SvtAv1EssentialJob::Quality::HIGH);
                }
                
                // Options
                if (aggressive == 'o' || aggressive == 'O') {
                    builder.aggressive();
                }
                if (unshackle == 'o' || unshackle == 'O') {
                    builder.unshackle();
                }
                builder.cleanup(cleanup == 'o' || cleanup == 'O');
                
                // Construire et exécuter
                SvtAv1EssentialJob job = builder.build();
                
                // Confirmation et exécution (version sans outputFile pour SVT-AV1)
                std::cout << std::endl;
                std::cout << Colors::PEACH << "❓ Lancer l'encodage ? (o/n) : " << Colors::RESET;
                char confirm;
                std::cin >> confirm;
                
                if (confirm == 'o' || confirm == 'O') {
                    std::cout << std::endl;
                    bool success = job.execute();
                    
                    if (!success) {
                        std::cerr << std::endl;
                        std::cerr << Colors::RED << Colors::BOLD << "❌ L'encodage a échoué." << Colors::RESET << std::endl;
                    }
                } else {
                    std::cout << std::endl;
                    std::cout << Colors::YELLOW << "🚫 Encodage annulé." << Colors::RESET << std::endl;
                }
                
            } catch (const std::exception& e) {
                handleError(e);
            }
            
            break;
        }
        
        case 0: {
            std::cout << std::endl;
            std::cout << Colors::LAVENDER << "👋 Au revoir !" << Colors::RESET << std::endl;
            break;
        }
        
        default: {
            std::cout << std::endl;
            std::cout << Colors::YELLOW << "⚠️  Fonctionnalité non encore implémentée." << Colors::RESET << std::endl;
            break;
        }
    }
}
} // namespace App
