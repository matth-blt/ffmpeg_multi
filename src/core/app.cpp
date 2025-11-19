#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include "../../include/core/app.hpp"
#include "../../include/core/string_utils.hpp"
#include "../../include/core/colors.hpp"
#include "../../include/core/input.hpp"
#include "../../include/jobs/encode.hpp"
#include "../../include/jobs/extract_frames.hpp"
#include "../../include/jobs/probe.hpp"
#include "../../include/jobs/reencode.hpp"
#include "../../include/jobs/reencode_builder.hpp"
#include "../../include/jobs/svt_av1_essential.hpp"
#include "../../include/jobs/thumbnails.hpp"
#include "../../include/jobs/extract_frames.hpp"
#include "../../include/jobs/probe.hpp"
#include "../../include/jobs/reencode.hpp"
#include "../../include/jobs/reencode_builder.hpp"
#include "../../include/jobs/svt_av1_essential.hpp"
#include "../../include/jobs/thumbnails.hpp"
#include "../../include/jobs/concat.hpp"

using namespace FFmpegMulti::Jobs;
using namespace FFmpegMulti::Encode;
using namespace FFmpegMulti;

namespace App {

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Calculer la longueur d'affichage réelle (UTF-8)
// Gère approximativement les caractères larges (CJK, Emojis)
int getDisplayLength(const std::string& str) {
    int length = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if ((c & 0x80) == 0) { // ASCII (1 octet)
            length += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) { // 2 octets
            length += 1;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3 octets (souvent CJK -> large)
            length += 2; 
            i += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4 octets (Emojis -> large)
            length += 2;
            i += 4;
        } else {
            i += 1; // Caractère invalide
        }
    }
    return length;
}

void printHeader(const std::string& title) {
    const int TOTAL_WIDTH = 60;
    const int INNER_WIDTH = TOTAL_WIDTH - 2; // -2 pour les bordures
    
    int titleLen = getDisplayLength(title);
    int padding = std::max(0, INNER_WIDTH - titleLen);
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;
    
    std::cout << Colors::BLUE << "╭";
    for(int i=0; i<INNER_WIDTH; i++) std::cout << "─";
    std::cout << "╮" << Colors::RESET << std::endl;
    
    std::cout << Colors::BLUE << "│" << Colors::RESET;
    std::cout << std::string(leftPad, ' ');
    std::cout << Colors::BOLD << Colors::LAVENDER << title << Colors::RESET;
    std::cout << std::string(rightPad, ' ');
    std::cout << Colors::BLUE << "│" << Colors::RESET << std::endl;
    
    std::cout << Colors::BLUE << "╰";
    for(int i=0; i<INNER_WIDTH; i++) std::cout << "─";
    std::cout << "╯" << Colors::RESET << std::endl;
}

void printSeparator() {
    const int TOTAL_WIDTH = 60;
    std::cout << Colors::BLUE;
    for(int i=0; i<TOTAL_WIDTH; i++) std::cout << "─";
    std::cout << Colors::RESET << std::endl;
}

void printOption(int id, const std::string& description) {
    const int TOTAL_WIDTH = 60;
    // Structure: "│ ID  │  Description      │"
    // Prefix: "│ " (2) + ID (2) + "  │  " (5) = 9 chars
    // Suffix: "│" (1)
    // Available for description + padding: TOTAL_WIDTH - 10 = 50
    
    std::cout << Colors::BLUE << "│ " << Colors::MAUVE << Colors::BOLD;
    if (id < 10) std::cout << " ";
    std::cout << id << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << description;
    
    int descLen = getDisplayLength(description);
    int availableSpace = TOTAL_WIDTH - 10; // 50
    int paddingNeeded = std::max(0, availableSpace - descLen);
    
    std::cout << std::string(paddingNeeded, ' ') << Colors::BLUE << "│" << Colors::RESET << std::endl;
}

// Fonction pour demander les fichiers d'entrée et de sortie
void promptFiles(std::string& inputFile, std::string& outputFile, const std::string& outputHint = "") {
    inputFile = Input::getString("Fichier d'entrée");
    outputFile = Input::getString("Fichier de sortie", outputHint);
}

// Fonction pour demander un CRF/QP
int promptQuality(const std::string& type, const std::string& examples) {
    std::cout << Colors::TEXT << "  Qualité " << type << " " << Colors::SUBTEXT << examples << Colors::RESET << std::endl;
    return Input::getInt("Valeur");
}

// Fonction pour demander un preset
std::string promptPreset(const std::string& options) {
    std::cout << Colors::TEXT << "  Preset " << Colors::SUBTEXT << options << Colors::RESET << std::endl;
    return Input::getString("Choix");
}

// Fonction pour confirmer et exécuter un job
template<typename JobType> bool confirmAndExecute(JobType& job, const std::string& outputFile = "") {
    std::cout << std::endl;
    
    bool confirm = Input::getConfirm("Lancer l'opération ?");
    
    if (confirm) {
        std::cout << std::endl;
        std::cout << Colors::BLUE << Colors::BOLD << ">>> Lancement de l'opération..." << Colors::RESET << std::endl;
        printSeparator();
        
        bool success = job.execute();
        
        printSeparator();
        std::cout << std::endl;
        if (success) {
            std::cout << Colors::GREEN << Colors::BOLD << "[OK] Opération terminée avec succès!" << Colors::RESET << std::endl;
            if (!outputFile.empty()) {
                std::cout << Colors::TEAL << "> Fichier de sortie : " << Colors::TEXT << outputFile << Colors::RESET << std::endl;
            }
        } else {
            std::cerr << Colors::RED << Colors::BOLD << "[ERREUR] L'opération a échoué. Vérifiez les erreurs ci-dessus." << Colors::RESET << std::endl;
        }
        return success;
    } else {
        std::cout << std::endl;
        std::cout << Colors::YELLOW << "[INFO] Opération annulée." << Colors::RESET << std::endl;
        return false;
    }
}

// Fonction pour gérer les erreurs
void handleError(const std::exception& e) {
    std::cerr << std::endl;
    std::cerr << Colors::RED << Colors::BOLD << "[ERREUR] : " << Colors::RESET << Colors::RED << e.what() << Colors::RESET << std::endl;
}

void affiche() {
    const int TOTAL_WIDTH = 60;
    const int INNER_WIDTH = TOTAL_WIDTH - 2;

    std::cout << std::endl;
    printHeader("MULTI-FFMPEG TOOL");
    std::cout << std::endl;
    
    // Top border of the menu table
    std::cout << Colors::BLUE << "╭─────┬";
    for(int i=0; i<INNER_WIDTH - 6; i++) std::cout << "─";
    std::cout << "╮" << Colors::RESET << std::endl;

    // Header row
    std::cout << Colors::BLUE << "│" << Colors::MAUVE << Colors::BOLD << " ID  " << Colors::RESET << Colors::BLUE << "│  " << Colors::TEXT << "Action" << std::string(INNER_WIDTH - 6 - 8, ' ') << Colors::BLUE << "│" << Colors::RESET << std::endl;
    
    // Separator
    std::cout << Colors::BLUE << "├─────┼";
    for(int i=0; i<INNER_WIDTH - 6; i++) std::cout << "─";
    std::cout << "┤" << Colors::RESET << std::endl;
    
    printOption(1, "Extraire des frames");
    printOption(2, "Encoder une vidéo");
    printOption(3, "Réencoder un fichier");
    printOption(4, "Concaténer plusieurs vidéos");
    printOption(5, "Générer des miniatures");
    printOption(6, "Encoder avec SVT-AV1-Essential");
    printOption(7, "Analyser un média (ffprobe)");
    
    // Separator
    std::cout << Colors::BLUE << "├─────┼";
    for(int i=0; i<INNER_WIDTH - 6; i++) std::cout << "─";
    std::cout << "┤" << Colors::RESET << std::endl;

    printOption(0, "Quitter");
    
    // Bottom border
    std::cout << Colors::BLUE << "╰─────┴";
    for(int i=0; i<INNER_WIDTH - 6; i++) std::cout << "─";
    std::cout << "╯" << Colors::RESET << std::endl;
}

bool choice() {
    int userChoice = 0;
    try {
        userChoice = Input::getInt("Entrez votre choix");
    } catch (const BackException&) {
        return true; // Exit if back at main menu
    }
    
    std::cout << std::endl;
    
    switch (userChoice) {
        case 1: {
            try {
                std::string inputFile, outputDir;
                bool createSubfolder;
                std::string subfolderName;
                int formatChoice;

                printHeader("EXTRAIRE DES FRAMES");
                std::cout << std::endl;

                // Demander le fichier d'entrée
                inputFile = Input::getString("Fichier d'entrée");
                std::cout << std::endl;

                // Demander le dossier de sortie
                outputDir = Input::getString("Dossier de sortie");
                std::cout << std::endl;

                // Demander si créer un sous-dossier
                createSubfolder = Input::getConfirm("Créer un sous-dossier");
                
                if (createSubfolder) {
                    subfolderName = Input::getString("Nom du sous-dossier");
                }
                
                std::cout << std::endl;

                // Menu de choix de format
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choisissez un format d'image ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " PNG " << Colors::SUBTEXT << "- Sans perte, RGB24" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " TIFF " << Colors::SUBTEXT << "- Compression Deflate, RGB24" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " JPEG " << Colors::SUBTEXT << "- Qualité maximale, YUV420p" << Colors::RESET << std::endl;
                
                printSeparator();
                formatChoice = Input::getIntRange("Votre choix", 1, 3);
                std::cout << std::endl;
                
                try {
                    ExtractFramesBuilder builder;
                    builder.input(inputFile).outputDir(outputDir).createSubfolder(createSubfolder);
                    
                    if (createSubfolder && !subfolderName.empty()) {
                        builder.subfolderName(subfolderName);
                    }
                    
                    // Configuration selon le format choisi
                    switch (formatChoice) {
                        case 1:
                            builder.png();
                            std::cout << Colors::GREEN << "[OK] Format PNG sélectionné" << Colors::RESET << std::endl;
                            break;
                        case 2:
                            builder.tiff();
                            std::cout << Colors::GREEN << "[OK] Format TIFF sélectionné" << Colors::RESET << std::endl;
                            break;
                        case 3:
                            builder.jpeg();
                            std::cout << Colors::GREEN << "[OK] Format JPEG sélectionné" << Colors::RESET << std::endl;
                            break;
                    }
                    
                    std::cout << std::endl;
                    
                    // Confirmation et exécution
                    auto job = builder.build();
                    confirmAndExecute<ExtractFramesJob>(job);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 2: {
            try {
                std::string inputDir, outputDir, outputFilename, inputPattern;
                int codecChoice, qualityChoice, formatChoice, framerate;
                std::string presetChoice;

                printHeader("ENCODER UNE VIDEO");
                std::cout << std::endl;

                // Demander le dossier d'entrée (images)
                inputDir = Input::getString("Dossier d'entrée (images)");
                std::cout << std::endl;

                // Demander le pattern des images
                inputPattern = Input::getString("Pattern des images", "%08d.png", true);
                if (inputPattern.empty()) {
                    inputPattern = "%08d.png";
                }
                std::cout << std::endl;

                // Demander le framerate
                framerate = Input::getInt("Framerate (FPS)", "24");
                std::cout << std::endl;

                // Demander le dossier de sortie
                outputDir = Input::getString("Dossier de sortie");
                std::cout << std::endl;

                // Demander le nom du fichier de sortie
                outputFilename = Input::getString("Nom du fichier de sortie");
                std::cout << std::endl;

                // Menu de choix de format
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choisissez un format de conteneur ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " MKV " << Colors::SUBTEXT << "- Matroska (recommandé)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " WEBM " << Colors::SUBTEXT << "- WebM (web)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " MP4 " << Colors::SUBTEXT << "- MPEG-4 (universel)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " MOV " << Colors::SUBTEXT << "- QuickTime" << Colors::RESET << std::endl;
                
                printSeparator();
                formatChoice = Input::getIntRange("Votre choix", 1, 4);
                std::cout << std::endl;

                // Menu de choix de codec (réutilisé de reencode)
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choisissez un codec ::" << Colors::RESET << std::endl;
                printSeparator();
                
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " X264 " << Colors::SUBTEXT << "- Universel, bonne compatibilité" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " X265 " << Colors::SUBTEXT << "- Meilleure compression, qualité supérieure" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " AV1 " << Colors::SUBTEXT << "- Compression maximale pour le web" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " H264_NVENC " << Colors::SUBTEXT << "- Hardware H.264 (NVIDIA)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " H265_NVENC " << Colors::SUBTEXT << "- Hardware H.265 (NVIDIA)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  6." << Colors::TEXT << " ProRes " << Colors::SUBTEXT << "- Apple ProRes 4444 (Production)" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  7." << Colors::TEXT << " FFV1 " << Colors::SUBTEXT << "- Codec lossless pour archivage" << Colors::RESET << std::endl;
                
                printSeparator();
                codecChoice = Input::getIntRange("Votre choix", 1, 7);
                std::cout << std::endl;
                
                try {
                    EncodeJobBuilder builder;
                    builder.inputDir(inputDir).outputDir(outputDir).outputFilename(outputFilename).inputPattern(inputPattern).framerate(framerate);
                    
                    // Configuration du format
                    switch (formatChoice) {
                        case 1:
                            builder.mkv();
                            std::cout << Colors::GREEN << "[OK] Format MKV sélectionné" << Colors::RESET << std::endl;
                            break;
                        case 2:
                            builder.webm();
                            std::cout << Colors::GREEN << "[OK] Format WEBM sélectionné" << Colors::RESET << std::endl;
                            break;
                        case 3:
                            builder.mp4();
                            std::cout << Colors::GREEN << "[OK] Format MP4 sélectionné" << Colors::RESET << std::endl;
                            break;
                        case 4:
                            builder.mov();
                            std::cout << Colors::GREEN << "[OK] Format MOV sélectionné" << Colors::RESET << std::endl;
                            break;
                    }
                    
                    // Configuration selon le codec
                    switch (codecChoice) {
                        case 1: { // X264
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration X264" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(18=excellent, 23=bon, 28=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x264().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] X264 configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 2: { // X265
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration X265" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(20=excellent, 28=bon, 35=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x265().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] X265 configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 3: { // AV1
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration AV1" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(25=excellent, 35=bon, 45=acceptable)");
                            
                            builder.av1().quality(qualityChoice);
                            std::cout << Colors::GREEN << "[OK] AV1 configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 4: { // H264_NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration H264_NVENC" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CQ", "(18=excellent, 23=bon, 28=acceptable)");
                            presetChoice = promptPreset("(fast/medium/slow/hq)");
                            
                            builder.h264_nvenc().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] H264_NVENC configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 5: { // H265_NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration H265_NVENC" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CQ", "(20=excellent, 28=bon, 35=acceptable)");
                            presetChoice = promptPreset("(fast/medium/slow/hq)");
                            
                            builder.h265_nvenc().quality(qualityChoice).preset(presetChoice);
                            std::cout << Colors::GREEN << "[OK] H265_NVENC configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 6: { // ProRes
                            builder.prores();
                            std::cout << Colors::GREEN << "[OK] ProRes 4444 configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 7: { // FFV1
                            builder.ffv1();
                            std::cout << Colors::GREEN << "[OK] FFV1 lossless configuré" << Colors::RESET << std::endl;
                            break;
                        }
                    }
                    
                    std::cout << std::endl;
                    
                    // Confirmation et exécution
                    auto job = builder.build();
                    confirmAndExecute<EncodeJob>(job);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 3: {
            try {
                std::string inputFile, outputFile;
                int codecChoice, qualityChoice;
                std::string presetChoice;

                printHeader("REENCODER UN FICHIER");
                std::cout << std::endl;

                // Demander les fichiers d'entrée et de sortie
                promptFiles(inputFile, outputFile);
                
                std::cout << std::endl;
                
                // Menu de choix de codec
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Choisissez un codec ::" << Colors::RESET << std::endl;
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
                codecChoice = Input::getIntRange("Votre choix", 1, 8);
                std::cout << std::endl;
                
                try {
                    ReencodeJobBuilder builder;
                    builder.input(inputFile).output(outputFile);
                    
                    // Configuration selon le choix
                    switch (codecChoice) {
                        case 1: { // X264 personnalisé
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration X264" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(18=excellent, 23=bon, 28=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x264().crf(qualityChoice).preset(presetChoice).copyAudio();
                            std::cout << Colors::GREEN << "[OK] X264 configuré" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 2: { // X265 personnalisé
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration X265" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(18=excellent, 23=bon, 28=acceptable)");
                            presetChoice = promptPreset("(ultrafast/fast/medium/slow/veryslow)");
                            
                            builder.x265().crf(qualityChoice).preset(presetChoice).tenBit().copyAudio();
                            std::cout << Colors::GREEN << "[OK] X265 configuré (10-bit)" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 3: { // AV1
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration AV1" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("CRF", "(20=excellent, 30=bon, 35=acceptable)");
                            
                            builder.svtav1().crf(qualityChoice).preset("5").copyAudio();
                            std::cout << Colors::GREEN << "[OK] AV1 configuré (SVT-AV1)" << Colors::RESET << std::endl;
                            break;
                        }

                        case 4: { // H264 NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration H264_NVENC" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("QP", "(17=excellent, 22=bon, 27=acceptable)");
                            presetChoice = promptPreset("(p1/p2/p3/p4/p5/p6/p7)");

                            builder.h264_nvenc().qp(qualityChoice).preset(presetChoice).pixelFormat(FFmpegMulti::Encode::PixelFormat::P010).copyAudio();
                            std::cout << Colors::GREEN << "[OK] H264_NVENC configuré (Hardware)" << Colors::RESET << std::endl;
                            break;
                        }

                        case 5: { // H265 NVENC
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Configuration H265_NVENC" << Colors::RESET << std::endl;
                            qualityChoice = promptQuality("QP", "(17=excellent, 22=bon, 27=acceptable)");
                            presetChoice = promptPreset("(p1/p2/p3/p4/p5/p6/p7)");

                            builder.h265_nvenc().qp(qualityChoice).preset(presetChoice).pixelFormat(FFmpegMulti::Encode::PixelFormat::P010).tune("hq").copyAudio();
                            std::cout << Colors::GREEN << "[OK] H265_NVENC configuré (Hardware, HQ)" << Colors::RESET << std::endl;
                            break;
                        }

                        case 6: { // ProRes
                            int profileChoice;
                            std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Profil ProRes" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  0." << Colors::TEXT << " Proxy " << Colors::SUBTEXT << "- Faible qualité, taille réduite" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " LT " << Colors::SUBTEXT << "- Light, qualité moyenne" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " Standard " << Colors::SUBTEXT << "- Bonne qualité" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " HQ " << Colors::SUBTEXT << "- High Quality, excellente qualité" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  4." << Colors::TEXT << " 4444 " << Colors::SUBTEXT << "- Qualité maximale avec alpha" << Colors::RESET << std::endl;
                            std::cout << Colors::MAUVE << "  5." << Colors::TEXT << " 4444 XQ " << Colors::SUBTEXT << "- Qualité extrême" << Colors::RESET << std::endl;
                            
                            profileChoice = Input::getIntRange("Votre choix", 0, 5);
                            
                            builder.prores().proresProfile(profileChoice).copyAudio().container("mov");
                            
                            std::cout << Colors::GREEN << "[OK] ProRes configuré (Profile " << profileChoice << ")" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 7: { // FFV1
                            builder.ffv1Preset();
                            std::cout << Colors::GREEN << "[OK] FFV1 configuré (Lossless, Level 3)" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        case 8: { // Preset YouTube
                            builder.youtubePreset();
                            std::cout << Colors::GREEN << "[OK] Preset YouTube appliqué (H.264, CRF 23, medium)" << Colors::RESET << std::endl;
                            break;
                        }
                        
                        default:
                            std::cout << Colors::YELLOW << "[WARN] Choix invalide, utilisation du preset YouTube par défaut" << Colors::RESET << std::endl;
                            builder.youtubePreset();
                            break;
                    }
                    
                    // Construire le job
                    std::cout << std::endl;
                    std::cout << Colors::BLUE << ">>> Construction du job de réencodage..." << Colors::RESET << std::endl;
                    ReencodeJob job = builder.build();
                    
                    // Afficher la commande qui sera exécutée
                    std::cout << std::endl;
                    std::cout << Colors::SAPPHIRE << Colors::BOLD << ":: Commande FFmpeg" << Colors::RESET << std::endl;
                    printSeparator();
                    std::cout << Colors::SUBTEXT << job.getCommandString() << Colors::RESET << std::endl;
                    printSeparator();
                    
                    // Demander confirmation et exécuter
                    confirmAndExecute(job, outputFile);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 4: {
            try {
                std::vector<std::string> inputs;
                std::string outputFile;
                
                printHeader("CONCATENER DES VIDEOS");
                std::cout << std::endl;
                
                // Boucle pour ajouter des fichiers
                while (true) {
                    std::string prompt = "Fichier d'entrée #" + std::to_string(inputs.size() + 1);
                    std::string input = Input::getString(prompt);
                    inputs.push_back(input);
                    
                    std::cout << std::endl;
                    if (!Input::getConfirm("Ajouter un autre fichier")) {
                        break;
                    }
                    std::cout << std::endl;
                }
                
                std::cout << std::endl;
                
                // Demander le fichier de sortie
                outputFile = Input::getString("Fichier de sortie (.mkv)");
                
                // Forcer l'extension .mkv si absente
                if (outputFile.length() < 4 || outputFile.substr(outputFile.length() - 4) != ".mkv") {
                    outputFile += ".mkv";
                    std::cout << Colors::SUBTEXT << "[INFO] Extension .mkv ajoutée automatiquement." << Colors::RESET << std::endl;
                }
                
                std::cout << std::endl;
                
                try {
                    ConcatBuilder builder;
                    for (const auto& input : inputs) {
                        builder.addInput(input);
                    }
                    builder.output(outputFile);
                    
                    ConcatJob job = builder.build();
                    
                    // Afficher le résumé
                    std::cout << Colors::SAPPHIRE << ":: Résumé de l'opération :" << Colors::RESET << std::endl;
                    for (size_t i = 0; i < inputs.size(); ++i) {
                        std::cout << Colors::TEAL << "  • Entrée " << (i+1) << " : " << Colors::TEXT << inputs[i] << Colors::RESET << std::endl;
                    }
                    std::cout << Colors::TEAL << "  • Sortie   : " << Colors::TEXT << outputFile << Colors::RESET << std::endl;
                    std::cout << std::endl;
                    
                    confirmAndExecute(job, outputFile);
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
                
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 5: {
            try {
                std::string inputFile, outputDir;
                bool createSubfolder;
                std::string subfolderName;
                int formatChoice;
                float sceneThreshold;

                printHeader("GENERER DES THUMBNAILS");
                std::cout << std::endl;

                // Demander le fichier d'entrée
                inputFile = Input::getString("Fichier d'entrée");
                std::cout << std::endl;

                // Demander le dossier de sortie
                outputDir = Input::getString("Dossier de sortie");
                std::cout << std::endl;

                // Demander si création d'un sous-dossier
                createSubfolder = Input::getConfirm("Créer un sous-dossier");
                std::cout << std::endl;

                if (createSubfolder) {
                    subfolderName = Input::getString("Nom du sous-dossier");
                    std::cout << std::endl;
                }

                // Choix du format
                std::cout << Colors::LAVENDER << "Formats disponibles :" << Colors::RESET << std::endl;
                std::cout << Colors::BLUE << "  1. PNG  (rgb24, sans perte)" << Colors::RESET << std::endl;
                std::cout << Colors::BLUE << "  2. TIFF (rgb24, compression deflate)" << Colors::RESET << std::endl;
                std::cout << Colors::BLUE << "  3. JPEG (yuvj420p, qualité maximale)" << Colors::RESET << std::endl;
                std::cout << std::endl;
                formatChoice = Input::getIntRange("Format de sortie", 1, 3);
                std::cout << std::endl;

                ThumbnailFormat format;
                switch (formatChoice) {
                    case 1: format = ThumbnailFormat::PNG; break;
                    case 2: format = ThumbnailFormat::TIFF; break;
                    case 3: format = ThumbnailFormat::JPEG; break;
                    default: format = ThumbnailFormat::PNG;
                }

                // Seuil de détection de scène
                sceneThreshold = Input::getFloat("Seuil de détection de scène (0.0-1.0)", "0.15");
                std::cout << std::endl;

                // Construire le job
                try {
                    ThumbnailsBuilder builder;
                    builder.input(inputFile).outputDir(outputDir).format(format).sceneThreshold(sceneThreshold);

                    if (createSubfolder && !subfolderName.empty()) {
                        builder.createSubfolder(true).subfolderName(subfolderName);
                    } else {
                        builder.createSubfolder(false);
                    }

                    ThumbnailsJob job = builder.build();

                    // Afficher le résumé
                    std::cout << Colors::SAPPHIRE << ":: Résumé de l'opération :" << Colors::RESET << std::endl;
                    std::cout << Colors::TEAL << "  • Entrée      : " << Colors::TEXT << inputFile << Colors::RESET << std::endl;
                    std::cout << Colors::TEAL << "  • Sortie      : " << Colors::TEXT << outputDir;
                    if (createSubfolder && !subfolderName.empty()) {
                        std::cout << "/" << subfolderName;
                    }
                    std::cout << Colors::RESET << std::endl;
                    
                    std::string formatName;
                    switch (format) {
                        case ThumbnailFormat::PNG: formatName = "PNG"; break;
                        case ThumbnailFormat::TIFF: formatName = "TIFF"; break;
                        case ThumbnailFormat::JPEG: formatName = "JPEG"; break;
                    }
                    std::cout << Colors::TEAL << "  • Format      : " << Colors::TEXT << formatName << Colors::RESET << std::endl;
                    std::cout << Colors::TEAL << "  • Seuil scène : " << Colors::TEXT << sceneThreshold << Colors::RESET << std::endl;
                    std::cout << std::endl;

                    // Demander confirmation
                    bool confirm = Input::getConfirm("Lancer l'extraction");
                    
                    if (confirm) {
                        std::cout << std::endl;
                        std::cout << Colors::BLUE << Colors::BOLD << ">>> Lancement de l'extraction..." << Colors::RESET << std::endl;
                        printSeparator();
                        
                        bool success = job.execute();
                        
                        printSeparator();
                        std::cout << std::endl;
                        if (success) {
                            std::cout << Colors::GREEN << Colors::BOLD << "[OK] Thumbnails générés avec succès!" << Colors::RESET << std::endl;
                        } else {
                            std::cerr << Colors::RED << Colors::BOLD << "[ERREUR] L'extraction a échoué." << Colors::RESET << std::endl;
                        }
                    } else {
                        std::cout << std::endl;
                        std::cout << Colors::YELLOW << "[INFO] Extraction annulée." << Colors::RESET << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            }
            break;
        }        
        
        case 6: { // SVT-AV1-Essential
            try {
                std::string inputFile, outputFile;
                int qualityChoice;
                bool aggressive, unshackle, cleanup;

                printHeader("ENCODER SVT-AV1-ESSENTIAL");
                std::cout << std::endl;

                // Fichiers
                promptFiles(inputFile, outputFile, "(.mkv)");
                
                std::cout << std::endl;
                
                // Qualité
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Qualité :" << Colors::RESET << std::endl;
                printSeparator();
                std::cout << Colors::MAUVE << "  1." << Colors::TEXT << " Low " << Colors::SUBTEXT << "- Rapide, qualité acceptable" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  2." << Colors::TEXT << " Medium " << Colors::SUBTEXT << "- Équilibré" << Colors::RESET << std::endl;
                std::cout << Colors::MAUVE << "  3." << Colors::TEXT << " High " << Colors::SUBTEXT << "- Lent, excellente qualité" << Colors::RESET << std::endl;
                printSeparator();
                qualityChoice = Input::getIntRange("Votre choix", 1, 3);
                
                std::cout << std::endl;
                
                // Options avancées
                std::cout << Colors::LAVENDER << Colors::BOLD << ":: Options avancées" << Colors::RESET << std::endl;
                printSeparator();
                aggressive = Input::getConfirm("Mode agressif (plus de compression)");
                unshackle = Input::getConfirm("Unshackle (libère les limites)");
                cleanup = Input::getConfirm("Nettoyer les fichiers temporaires");
                
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
                    if (aggressive) builder.aggressive();
                    if (unshackle) builder.unshackle();
                    builder.cleanup(cleanup);
                    
                    // Construire et exécuter
                    SvtAv1EssentialJob job = builder.build();
                    
                    // Confirmation et exécution (version sans outputFile pour SVT-AV1)
                    std::cout << std::endl;
                    bool confirm = Input::getConfirm("Lancer l'encodage");
                    
                    if (confirm) {
                        std::cout << std::endl;
                        bool success = job.execute();
                        
                        if (!success) {
                            std::cerr << std::endl;
                            std::cerr << Colors::RED << Colors::BOLD << "[ERREUR] L'encodage a échoué." << Colors::RESET << std::endl;
                        }
                    } else {
                        std::cout << std::endl;
                        std::cout << Colors::YELLOW << "[INFO] Encodage annulé." << Colors::RESET << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    handleError(e);
                }
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            }
            break;
        }
        
        case 7: {
            try {
                std::string inputFile;
                
                printHeader("ANALYSER UN MEDIA");
                std::cout << std::endl;
                
                // Demander le fichier d'entrée
                inputFile = Input::getString("Fichier d'entrée");
                std::cout << std::endl;
                
                // Créer le job FFProbe
                ::Jobs::ProbeJob job(inputFile);
                
                // Exécuter l'analyse
                printHeader("ANALYSE FFPROBE");
                std::cout << std::endl;
                
                job.execute();
                
                std::cout << std::endl;
                
                // Demander si on veut exporter
                bool exportChoice = Input::getConfirm("Exporter le résultat");
                
                if (exportChoice) {
                    std::cout << std::endl;
                    
                    // Demander le format
                    std::cout << Colors::BLUE << "┌────────────────────────────────────────────┐" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "│" << Colors::TEXT << "  Format d'export :                        " << Colors::BLUE << "│" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "├────────────────────────────────────────────┤" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "│  " << Colors::MAUVE << "1" << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << "JSON (sortie brute FFProbe)      " << Colors::BLUE << "│" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "│  " << Colors::MAUVE << "2" << Colors::RESET << Colors::BLUE << "  │  " << Colors::TEXT << "TXT  (sortie formatée lisible)   " << Colors::BLUE << "│" << Colors::RESET << std::endl;
                    std::cout << Colors::BLUE << "└────────────────────────────────────────────┘" << Colors::RESET << std::endl;
                    std::cout << std::endl;
                    
                    int formatChoice = Input::getIntRange("Choix", 1, 2);
                    std::cout << std::endl;
                    
                    bool isJson = (formatChoice == 1);
                    std::string exportPath = job.generateExportPath(isJson);
                    
                    if (isJson) {
                        // Exporter le JSON brut
                        job.writeToFile(exportPath, job.getOutput());
                        std::cout << Colors::GREEN << "[OK] Analyse exportée (JSON) : " << Colors::TEAL << exportPath << Colors::RESET << std::endl;
                    } else {
                        // Exporter le texte formaté
                        job.writeToFile(exportPath, job.getFormattedOutput());
                        std::cout << Colors::GREEN << "[OK] Analyse exportée (TXT)  : " << Colors::TEAL << exportPath << Colors::RESET << std::endl;
                    }
                } else {
                    std::cout << std::endl;
                    std::cout << Colors::SUBTEXT << "[INFO] Résultat non exporté." << Colors::RESET << std::endl;
                }
                
            } catch (const BackException&) {
                std::cout << Colors::YELLOW << "[INFO] Retour au menu principal." << Colors::RESET << std::endl;
            } catch (const std::exception& e) {
                handleError(e);
            }
            
            break;
        }
        
        case 0: {
            std::cout << std::endl;
            std::cout << Colors::LAVENDER << "Au revoir !" << Colors::RESET << std::endl;
            return false;
        }
        
        default: {
            std::cout << std::endl;
            std::cout << Colors::YELLOW << "[WARN] Fonctionnalité non encore implémentée." << Colors::RESET << std::endl;
            break;
        }
    }
    return true;
}
} // namespace App
