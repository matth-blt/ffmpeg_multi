#include "../include/jobs/encode_builder.hpp"
#include <iostream>

using namespace FFmpegMulti::Jobs;
using namespace FFmpegMulti::Encode;

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "     TEST ENCODEUR PRORES - FFmpeg Multi         " << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;

    // Test 1: ProRes avec paramètres par défaut
    std::cout << "Test 1: ProRes 4444 (paramètres par défaut)" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    
    EncodeJob job1 = EncodeJobBuilder()
        .input("test_input.mp4")
        .output("test_output_prores4444.mov")
        .prores()
        .copyAudio()
        .build();
    
    std::cout << "Commande générée:" << std::endl;
    std::cout << job1.getCommandString() << std::endl;
    std::cout << std::endl;

    // Test 2: ProRes HQ avec preset
    std::cout << "Test 2: ProRes HQ (preset)" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    
    EncodeJob job2 = EncodeJobBuilder()
        .input("test_input.mp4")
        .output("test_output_prores_hq.mov")
        .proresPreset(3)  // ProRes HQ
        .build();
    
    std::cout << "Commande générée:" << std::endl;
    std::cout << job2.getCommandString() << std::endl;
    std::cout << std::endl;

    // Test 3: ProRes LT (profil léger)
    std::cout << "Test 3: ProRes LT (profil léger)" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    
    EncodeJob job3 = EncodeJobBuilder()
        .input("test_input.mp4")
        .output("test_output_prores_lt.mov")
        .prores()
        .proresProfile(1)  // LT
        .proresBitsPerMB(5000)  // Moins de bits pour taille réduite
        .copyAudio()
        .build();
    
    std::cout << "Commande générée:" << std::endl;
    std::cout << job3.getCommandString() << std::endl;
    std::cout << std::endl;

    // Test 4: ProRes 4444 XQ (qualité maximale)
    std::cout << "Test 4: ProRes 4444 XQ (qualité extrême)" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    
    EncodeJob job4 = EncodeJobBuilder()
        .input("test_input.mp4")
        .output("test_output_prores_xq.mov")
        .proresPreset(5)  // 4444 XQ
        .audioCodec("pcm_s24le")  // Audio non compressé
        .build();
    
    std::cout << "Commande générée:" << std::endl;
    std::cout << job4.getCommandString() << std::endl;
    std::cout << std::endl;

    // Test 5: ProRes avec configuration complète personnalisée
    std::cout << "Test 5: ProRes personnalisé complet" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    
    EncodeJob job5 = EncodeJobBuilder()
        .input("test_input.mp4")
        .output("test_output_custom.mov")
        .prores()
        .proresProfile(4)
        .proresVendor("apl0")
        .proresBitsPerMB(10000)  // Qualité encore plus élevée
        .pixelFormat(PixelFormat::YUVA444P10LE)
        .audioCodec("aac")
        .audioBitrate(320)
        .container("mov")
        .build();
    
    std::cout << "Commande générée:" << std::endl;
    std::cout << job5.getCommandString() << std::endl;
    std::cout << std::endl;

    std::cout << "==================================================" << std::endl;
    std::cout << "Tous les tests de génération de commande réussis!" << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
