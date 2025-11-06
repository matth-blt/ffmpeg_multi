// Exemple d'utilisation de la nouvelle classe EncodeJob
// Compile avec: g++ -std=c++17 example_encode_usage.cpp -I../include

#include <iostream>
#include "../include/jobs/encode.hpp"
#include "../include/jobs/encode_types.hpp"

using namespace FFmpegMulti::Jobs;
using namespace FFmpegMulti::Encode;

// ============================================================================
// EXEMPLE 1 : Encodage H.264 Simple (YouTube)
// ============================================================================
void example_simple_h264() {
    std::cout << "\n=== Exemple 1: Encodage H.264 Simple ===\n" << std::endl;
    
    EncodeJob job("input.mp4", "output_h264.mp4");
    
    // Configuration basique
    job.config().codec = Codec::X264;
    job.config().rate_control = RateControl::CRF;
    job.config().quality = 23;
    job.config().preset = "medium";
    job.config().pixel_format = PixelFormat::YUV420P8;
    
    // Audio
    job.config().audio.copy_audio = true;
    
    // Afficher la commande générée
    std::cout << "Commande FFmpeg :" << std::endl;
    std::cout << job.getCommandString() << std::endl;
    
    // Exécuter (décommenter pour exécuter réellement)
    // job.execute();
}

// ============================================================================
// EXEMPLE 2 : Encodage H.265 Haute Qualité (Archivage)
// ============================================================================
void example_archive_h265() {
    std::cout << "\n=== Exemple 2: Encodage H.265 Haute Qualité ===\n" << std::endl;
    
    EncodeJob job("source.mov", "archive.mkv");
    
    // Codec H.265 avec qualité maximale
    job.config().codec = Codec::X265;
    job.config().rate_control = RateControl::CRF;
    job.config().quality = 18;  // Très haute qualité
    job.config().preset = "slow";  // Compression optimale
    
    // Format 10-bit pour meilleure qualité
    job.config().pixel_format = PixelFormat::YUV420P10;
    
    // GOP et B-frames
    job.config().gop_size = 250;
    job.config().bframes = 4;
    
    // Audio non compressé
    job.config().audio.codec = "flac";
    job.config().audio.sample_rate = 48000;
    job.config().audio.channels = 2;
    
    std::cout << "Commande FFmpeg :" << std::endl;
    std::cout << job.getCommandString() << std::endl;
}

// ============================================================================
// EXEMPLE 3 : Encodage HDR10 (Netflix-like)
// ============================================================================
void example_hdr10() {
    std::cout << "\n=== Exemple 3: Encodage HDR10 ===\n" << std::endl;
    
    EncodeJob job("hdr_source.mov", "hdr_output.mkv");
    
    // Codec H.265 requis pour HDR
    job.config().codec = Codec::X265;
    job.config().rate_control = RateControl::CRF;
    job.config().quality = 18;
    job.config().preset = "slow";
    
    // Format 10-bit obligatoire pour HDR
    job.config().pixel_format = PixelFormat::YUV420P10;
    
    // Configuration HDR color space
    job.config().passthrough_color = false;
    job.config().color_profile.primaries = ColorPrimaries::BT2020;
    job.config().color_profile.transfer = TransferCharacteristic::PQ;
    job.config().color_profile.matrix = MatrixCoefficients::BT2020NCL;
    job.config().color_profile.range = Range::Limited;
    
    // HDR Metadata - Content Light Level
    ContentLightLevel cll;
    cll.max_cll = 1000;   // Max brightness in nits
    cll.max_fall = 400;   // Frame average in nits
    job.config().content_light_level = cll;
    
    // HDR Metadata - Mastering Display
    MasteringDisplay md;
    md.red_x = 0.708f;    md.red_y = 0.292f;
    md.green_x = 0.170f;  md.green_y = 0.797f;
    md.blue_x = 0.131f;   md.blue_y = 0.046f;
    md.white_x = 0.3127f; md.white_y = 0.3290f;
    md.min_luminance = 0.0f;
    md.max_luminance = 1000.0f;
    job.config().mastering_display = md;
    
    // Audio AAC haute qualité
    job.config().audio.copy_audio = false;
    job.config().audio.codec = "aac";
    job.config().audio.bitrate_kbps = 256;
    
    std::cout << "Commande FFmpeg :" << std::endl;
    std::cout << job.getCommandString() << std::endl;
}

// ============================================================================
// EXEMPLE 4 : Encodage AV1 (Web moderne)
// ============================================================================
void example_av1_web() {
    std::cout << "\n=== Exemple 4: Encodage AV1 pour le Web ===\n" << std::endl;
    
    EncodeJob job("video.mp4", "web_av1.webm");
    
    // AV1 pour compression maximale
    job.config().codec = Codec::SVT_AV1;  // Plus rapide que libaom-av1
    job.config().rate_control = RateControl::CRF;
    job.config().quality = 30;  // AV1 utilise des valeurs différentes
    job.config().preset = "5";  // Preset numérique pour SVT-AV1
    
    // 8-bit suffisant pour le web
    job.config().pixel_format = PixelFormat::YUV420P8;
    
    // Audio Opus optimisé pour le web
    job.config().audio.codec = "libopus";
    job.config().audio.bitrate_kbps = 128;
    job.config().audio.sample_rate = 48000;
    
    // Container WebM
    job.config().container = "webm";
    
    std::cout << "Commande FFmpeg :" << std::endl;
    std::cout << job.getCommandString() << std::endl;
}

// ============================================================================
// EXEMPLE 5 : Encodage avec Bitrate Constant (Streaming)
// ============================================================================
void example_cbr_streaming() {
    std::cout << "\n=== Exemple 5: Encodage CBR pour Streaming ===\n" << std::endl;
    
    EncodeJob job("input.mp4", "stream_1080p.mp4");
    
    // H.264 pour compatibilité maximale
    job.config().codec = Codec::X264;
    job.config().rate_control = RateControl::CBR;
    job.config().bitrate_kbps = 5000;  // 5 Mbps pour 1080p
    job.config().buffer_size_kbps = 10000;  // 2x bitrate
    job.config().preset = "veryfast";  // Encodage rapide pour streaming live
    job.config().tune = "zerolatency";  // Latence minimale
    
    // GOP court pour seeking rapide
    job.config().gop_size = 60;  // 2 secondes à 30fps
    job.config().bframes = 0;  // Pas de B-frames pour latence
    
    // 8-bit standard
    job.config().pixel_format = PixelFormat::YUV420P8;
    
    // Audio AAC pour streaming
    job.config().audio.codec = "aac";
    job.config().audio.bitrate_kbps = 128;
    
    std::cout << "Commande FFmpeg :" << std::endl;
    std::cout << job.getCommandString() << std::endl;
}

// ============================================================================
// EXEMPLE 6 : Encodage ProRes (Post-production)
// ============================================================================
void example_prores() {
    std::cout << "\n=== Exemple 6: Encodage ProRes ===\n" << std::endl;
    
    EncodeJob job("footage.mp4", "prores_422hq.mov");
    
    // ProRes 422 HQ
    job.config().codec = Codec::ProRes;
    job.config().encoder_override = "prores_ks";  // Kostya's encoder
    
    // ProRes utilise un profil au lieu de CRF
    job.config().extra_args.push_back("-profile:v");
    job.config().extra_args.push_back("3");  // 3 = ProRes 422 HQ
    
    // Format 10-bit
    job.config().pixel_format = PixelFormat::YUV422P10;
    
    // Audio PCM non compressé
    job.config().audio.codec = "pcm_s24le";
    job.config().audio.sample_rate = 48000;
    
    // Container MOV
    job.config().container = "mov";
    
    std::cout << "Commande FFmpeg :" << std::endl;
    std::cout << job.getCommandString() << std::endl;
}

// ============================================================================
// EXEMPLE 7 : Validation des paramètres
// ============================================================================
void example_validation() {
    std::cout << "\n=== Exemple 7: Validation des paramètres ===\n" << std::endl;
    
    try {
        EncodeJob job;  // Pas de chemins définis
        job.config().codec = Codec::X264;
        
        // Tentative d'exécution sans chemins -> erreur
        job.execute();
        
    } catch (const std::runtime_error& e) {
        std::cout << "Erreur détectée : " << e.what() << std::endl;
    }
    
    try {
        EncodeJob job("input.mp4", "output.mp4");
        job.config().codec = Codec::X264;
        job.config().rate_control = RateControl::CBR;
        job.config().bitrate_kbps = 0;  // Bitrate invalide pour CBR
        
        // Validation échouera
        job.validate();
        
    } catch (const std::runtime_error& e) {
        std::cout << "Erreur détectée : " << e.what() << std::endl;
    }
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    std::cout << "╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   Exemples d'utilisation de EncodeJob         ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    
    example_simple_h264();
    example_archive_h265();
    example_hdr10();
    example_av1_web();
    example_cbr_streaming();
    example_prores();
    example_validation();
    
    std::cout << "\n✅ Tous les exemples ont été générés avec succès!" << std::endl;
    std::cout << "\n💡 Décommentez job.execute() dans chaque exemple pour exécuter l'encodage." << std::endl;
    
    return 0;
}
