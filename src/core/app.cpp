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

void affiche() {
    std::cout << "==============================================" << std::endl;
    std::cout << "             ⚙️  MULTI-FFMPEG TOOL             " << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "|  ID  |              Action                 |" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "|  1   | Extraire des frames                 |" << std::endl;
    std::cout << "|  2   | Encoder une vidéo                   |" << std::endl;
    std::cout << "|  3   | Réencoder un fichier                |" << std::endl;
    std::cout << "|  4   | Concaténer plusieurs vidéos         |" << std::endl;
    std::cout << "|  5   | Générer des miniatures              |" << std::endl;
    std::cout << "|  6   | Encoder avec SVT-AV1-Essential      |" << std::endl;
    std::cout << "|  7   | Analyser un média (ffprobe)         |" << std::endl;
    std::cout << "|  0   | Quitter                             |" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << std::endl;

    std::cout << "👉  Entrez votre choix : ";
}

void choice() {
    int userChoice{};
    std::cin >> userChoice;
    
    switch (userChoice) {
        case 2: {
            std::string inputFile, outputFile;
            int codecChoice, qualityChoice;
            std::string presetChoice;

            std::cout << "==============================================" << std::endl;
            std::cout << "           ⚙️  ENCODER UNE VIDEO              " << std::endl;
            std::cout << "==============================================" << std::endl;
            std::cout << std::endl;

            // Demander les fichiers d'entrée et de sortie
            std::cout << "📁 Fichier d'entrée : ";
            std::cin >> std::ws;
            std::getline(std::cin, inputFile);
            inputFile = StringUtils::clean(inputFile);
            
            std::cout << "📁 Fichier de sortie : ";
            std::getline(std::cin, outputFile);
            outputFile = StringUtils::clean(outputFile);
            
            // Menu de choix de codec
            std::cout << "\n🎬 Choisissez un codec :" << std::endl;
            std::cout << "  1. H.264 (x264) - Universel, bonne compatibilité" << std::endl;
            std::cout << "  2. H.265 (x265) - Meilleure compression, qualité supérieure" << std::endl;
            std::cout << "  3. AV1 - Compression maximale pour le web" << std::endl;
            std::cout << "  4. Preset YouTube (H.264, qualité optimale)" << std::endl;
            std::cout << "  5. Preset Archive (H.265, qualité maximale)" << std::endl;
            std::cout << "👉 Votre choix : ";
            std::cin >> codecChoice;
            
            try {
                EncodeJobBuilder builder;
                builder.input(inputFile).output(outputFile);
                
                // Configuration selon le choix
                switch (codecChoice) {
                    case 1: { // H.264 personnalisé
                        std::cout << "\n  Qualité CRF (18=excellent, 23=bon, 28=acceptable) : ";
                        std::cin >> qualityChoice;
                        std::cout << "⚙️  Preset (ultrafast/fast/medium/slow/veryslow) : ";
                        std::cin >> presetChoice;
                        
                        builder.h264().crf(qualityChoice).preset(presetChoice).copyAudio();
                        break;
                    }
                    
                    case 2: { // H.265 personnalisé
                        std::cout << "\n  Qualité CRF (18=excellent, 23=bon, 28=acceptable) : ";
                        std::cin >> qualityChoice;
                        std::cout << "⚙️  Preset (ultrafast/fast/medium/slow/veryslow) : ";
                        std::cin >> presetChoice;
                        
                        builder.h265().crf(qualityChoice).preset(presetChoice).tenBit().copyAudio();
                        break;
                    }
                    
                    case 3: { // AV1
                        std::cout << "\n  Qualité CRF (20=excellent, 30=bon, 35=acceptable) : ";
                        std::cin >> qualityChoice;
                        
                        builder.svtav1().crf(qualityChoice).preset("5").copyAudio();
                        break;
                    }
                    
                    case 4: { // Preset YouTube
                        builder.youtubePreset();
                        std::cout << "\n✅ Preset YouTube appliqué (H.264, CRF 23, medium)" << std::endl;
                        break;
                    }
                    
                    case 5: { // Preset Archive
                        builder.archivePreset();
                        std::cout << "\n✅ Preset Archive appliqué (H.265, CRF 18, slow, 10-bit)" << std::endl;
                        break;
                    }
                    
                    default:
                        std::cout << "\n  Choix invalide, utilisation du preset YouTube par défaut" << std::endl;
                        builder.youtubePreset();
                        break;
                }
                
                // Construire le job
                std::cout << "\n🔨 Construction du job d'encodage..." << std::endl;
                EncodeJob job = builder.build();
                
                // Afficher la commande qui sera exécutée
                std::cout << "\n📝 Commande FFmpeg :" << std::endl;
                std::cout << job.getCommandString() << std::endl;
                
                // Demander confirmation
                std::cout << "\n❓ Lancer l'encodage ? (o/n) : ";
                char confirm;
                std::cin >> confirm;
                
                if (confirm == 'o' || confirm == 'O') {
                    std::cout << "\n🚀 Lancement de l'encodage..." << std::endl;
                    std::cout << "============================================" << std::endl;
                    
                    bool success = job.execute();
                    
                    std::cout << "============================================" << std::endl;
                    if (success) {
                        std::cout << "\n✅ Encodage terminé avec succès!" << std::endl;
                        std::cout << "📁 Fichier de sortie : " << outputFile << std::endl;
                    } else {
                        std::cerr << "\n❌ L'encodage a échoué. Vérifiez les erreurs ci-dessus." << std::endl;
                    }
                } else {
                    std::cout << "\n🚫 Encodage annulé." << std::endl;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "\n❌ Erreur : " << e.what() << std::endl;
            }
            
            break;
        }
        
        case 6: { // SVT-AV1-Essential
            std::string inputFile, outputFile;
            int qualityChoice;
            char aggressive, unshackle, cleanup;

            std::cout << "==============================================" << std::endl;
            std::cout << "      🚀  ENCODER SVT-AV1-ESSENTIAL          " << std::endl;
            std::cout << "==============================================" << std::endl;
            std::cout << std::endl;

            // Fichiers
            std::cout << "📁 Fichier d'entrée : ";
            std::cin >> std::ws;
            std::getline(std::cin, inputFile);
            inputFile = StringUtils::clean(inputFile);
            
            std::cout << "📁 Fichier de sortie (.mkv) : ";
            std::getline(std::cin, outputFile);
            outputFile = StringUtils::clean(outputFile);
            
            // Qualité
            std::cout << "\n⚙️  Qualité :" << std::endl;
            std::cout << "  1. Low      - Rapide, qualité acceptable" << std::endl;
            std::cout << "  2. Medium   - Équilibré" << std::endl;
            std::cout << "  3. High     - Lent, excellente qualité" << std::endl;
            std::cout << "👉 Votre choix : ";
            std::cin >> qualityChoice;
            
            // Options avancées
            std::cout << "\n🔧 Options avancées :" << std::endl;
            std::cout << "  Mode agressif (plus de compression) ? (o/n) : ";
            std::cin >> aggressive;
            std::cout << "  Unshackle (libère les limites) ? (o/n) : ";
            std::cin >> unshackle;
            std::cout << "  Nettoyer les fichiers temporaires ? (o/n) : ";
            std::cin >> cleanup;
            
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
                
                std::cout << "\n❓ Lancer l'encodage ? (o/n) : ";
                char confirm;
                std::cin >> confirm;
                
                if (confirm == 'o' || confirm == 'O') {
                    bool success = job.execute();
                    
                    if (!success) {
                        std::cerr << "\n❌ L'encodage a échoué." << std::endl;
                    }
                } else {
                    std::cout << "\n🚫 Encodage annulé." << std::endl;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "\n❌ Erreur : " << e.what() << std::endl;
            }
            
            break;
        }
        
        case 0: {
            std::cout << "\n👋 Au revoir !" << std::endl;
            break;
        }
        
        default: {
            std::cout << "\n  Fonctionnalité non encore implémentée." << std::endl;
            break;
        }
    }
}
} // namespace App
