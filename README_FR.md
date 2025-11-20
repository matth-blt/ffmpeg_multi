# 🎬 ffmpeg.multi - Outil Multi-FFmpeg

Un outil en ligne de commande puissant pour gérer vos tâches d'encodage vidéo avec FFmpeg.

## 📋 Fonctionnalités

- ✅ **Extraction de frames** - Extrait toutes les images d'une vidéo (PNG, TIFF, JPEG)
- ✅ **Encodage vidéo** - Encode des séquences d'images en vidéo avec plusieurs codecs
- ✅ **Réencodage** - Réencode des fichiers vidéo existants avec différents codecs et paramètres
- ✅ **Génération de thumbnails** - Extrait des miniatures avec détection automatique de scènes
- ✅ **SVT-AV1-Essential** - Encodage AV1 optimisé via Auto-Boost-Essential
- ✅ **Concaténation** - Fusionne plusieurs vidéos sans perte (via MKVMerge)
- ✅ **Analyse ffprobe** - Analyse détaillée de médias avec export JSON/TXT

## 🚀 Installation / Build

### Pour les Utilisateurs (Portable)
1. Téléchargez la dernière release.
2. Décompressez l'archive.
3. Lancez `ffmpeg_multi.exe`.
   * L'application est portable : elle détecte automatiquement le dossier `extern/` situé à côté de l'exécutable.

### Pour les Développeurs

#### Prérequis
- **Windows 10/11**
- **Visual Studio 2019+** ou **MinGW-w64**
- **CMake 3.15+**

#### Compilation
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 📦 Structure du projet

```
ffmpeg.multi/
├── extern/                      
│   ├── env/
│   │   ├── auto-boost/
│   │   ├── mediainfo/
│   │   ├── mkvtoolnix/
│   │   ├── python/
│   │   └── vs/
│   ├── scripts/                 
│   ├── ffmpeg.exe
│   ├── ffplay.exe
│   ├── ffprobe.exe
│   └── SvtAv1EncApp.exe
├── include/
│   ├── core/
│   │   ├── app.hpp
│   │   ├── colors.hpp
│   │   ├── command.hpp
│   │   ├── ffmpeg_process.hpp
│   │   ├── input.hpp
│   │   ├── job.hpp
│   │   ├── logger.hpp
│   │   ├── path_utils.hpp
│   │   └── string_utils.hpp
│   └── jobs/
│       ├── codec_utils.hpp
│       ├── concat.hpp
│       ├── encode.hpp
│       ├── encode_types.hpp
│       ├── extract_frames.hpp
│       ├── probe.hpp
│       ├── reencode.hpp
│       ├── reencode_builder.hpp
│       ├── svt_av1_essential.hpp
│       └── thumbnails.hpp
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── app.cpp
│   │   ├── command.cpp
│   │   ├── ffmpeg_process.cpp
│   │   ├── input.cpp
│   │   ├── job.cpp
│   │   ├── logger.cpp
│   │   └── path_utils.cpp
│   └── jobs/
│       ├── codec_utils.cpp
│       ├── concat.cpp
│       ├── encode.cpp
│       ├── encode_builder.cpp
│       ├── extract_frames.cpp
│       ├── extract_frames_builder.cpp
│       ├── probe.cpp
│       ├── reencode.cpp
│       ├── reencode_builder.cpp
│       ├── svt_av1_essential.cpp
│       ├── thumbnails.cpp
│       └── thumbnails_builder.cpp
├── README.md
└── CMakeLists.txt
```

## 🛠️ Prérequis pour SVT-AV1-Essential

Pour utiliser l'encodage SVT-AV1 optimisé, assurez-vous que les outils suivants sont présents dans le dossier `extern/` :

- ✅ **Python portable** (`extern/env/python/`)
- ✅ **VapourSynth** (`extern/env/vs/`)
- ✅ **Auto-Boost-Essential.py** (`extern/env/auto-boost/`)
- ✅ **SvtAv1EncApp.exe** (`extern/SvtAv1EncApp.exe`)
- ✅ **MKVToolNix** (`extern/env/mkvtoolnix/mkvmerge.exe`)
- ✅ **FFmpeg** (`extern/ffmpeg.exe`)
- ✅ **Script PowerShell** (`extern/scripts/ABE.ps1`)

## 🎨 Fonctionnalités détaillées

### 1️⃣ Extraction de frames
Extrait toutes les frames d'une vidéo en images individuelles.
- **Formats** : PNG (Lossless), TIFF (Archive), JPEG (Léger).
- **Scaling** : Haute qualité (`spline+accurate_rnd+full_chroma_int`).

### 2️⃣ Encodage vidéo (Images → Vidéo)
Encode une séquence d'images en fichier vidéo.
- **Codecs** : X264, X265, AV1, NVENC (H264/H265), ProRes, FFV1.
- **Conteneurs** : MKV, WEBM, MP4, MOV.

### 3️⃣ Réencoder un fichier
Réencode une vidéo existante avec un autre codec ou paramètres.
- **Presets** : YouTube, Archivage (FFV1), Custom.
- **Options** : Profils ProRes, Pixel format (8/10-bit), Rate control (CRF, CQP, VBR, CBR).

### 4️⃣ Concaténer plusieurs vidéos
Fusionne plusieurs fichiers vidéo en un seul fichier MKV sans réencodage.
- Utilise **MKVMerge** pour une fusion sans perte.

### 5️⃣ Générer des miniatures
Génère automatiquement des thumbnails aux changements de scènes.
- Détection intelligente (`select='gt(scene,threshold)'`).

### 6️⃣ Encodage SVT-AV1-Essential
Utilise l'encodeur **SVT-AV1** via **Auto-Boost-Essential** pour une qualité supérieure et une gestion automatique de l'audio/muxing.

### 7️⃣ Analyser un média (ffprobe)
Analyse approfondie des flux vidéo, audio et sous-titres avec export JSON/TXT.

## 🙏 Remerciements

- **FFmpeg**
- **SVT-AV1-Essential and Auto-Boost-Essential Team**
- **VapourSynth**
- **MKVToolNix**