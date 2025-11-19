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

## 🚀 Installation

### Prérequis

- **Windows 10/11**
- **Visual Studio 2019+** ou **MinGW-w64**
- **CMake 3.15+**
- **FFmpeg** (fourni dans `extern/`)
- **MKVToolNix** (fourni dans `extern/env/mkvtoolnix/`)

### Compilation

```powershell
# Utiliser le script de build
.\scripts\build.ps1

# Ou manuellement avec CMake
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 📦 Structure du projet

```
ffmpeg.multi/
├── extern/                      # ⚠️ NON INCLUS dans Git - À créer manuellement
│   ├── env/                     # Environnement portable
│   │   ├── python/              # Python portable
│   │   ├── vs/                  # VapourSynth portable
│   │   ├── auto-boost/          # Auto-Boost-Essential
│   │   │   └── Auto-Boost-Essential.py
│   │   └── mkvtoolnix/
│   │       └── mkvmerge.exe
│   ├── scripts/
│   │   └── ABE.ps1              # Script PowerShell pour Auto-Boost
│   ├── ffmpeg.exe
│   └── SvtAv1EncApp.exe
├── include/                     # En-têtes
│   ├── core/
│   │   ├── app.hpp
│   │   ├── command.hpp
│   │   ├── ffmpeg_process.hpp
│   │   ├── job.hpp
│   │   ├── logger.hpp
│   │   ├── path_utils.hpp
│   │   └── string_utils.hpp
│   ├── jobs/
│   │   ├── codec_utils.hpp      # Utilitaires codecs (partagé)
│   │   ├── concat.hpp           # 🆕 Concaténation vidéo
│   │   ├── encode.hpp           # Encodage images→vidéo
│   │   ├── encode_types.hpp     # Types communs
│   │   ├── extract_frames.hpp   # Extraction de frames
│   │   ├── probe.hpp            # 🆕 Analyse FFprobe
│   │   ├── reencode.hpp         # Réencodage vidéo
│   │   ├── svt_av1_essential.hpp
│   │   └── thumbnails.hpp       # Génération de miniatures
│   └── pipelines/
│       ├── pipeline_base.hpp
│       ├── pipeline_batch.hpp
│       └── pipeline_custom.hpp
├── src/                         # Sources
│   ├── main.cpp
│   ├── core/
│   │   ├── app.cpp              # Interface utilisateur (Catppuccin Mocha)
│   │   ├── command.cpp
│   │   ├── ffmpeg_process.cpp
│   │   ├── job.cpp
│   │   ├── logger.cpp
│   │   └── path_utils.cpp
│   ├── jobs/
│   │   ├── codec_utils.cpp      # Logique codecs centralisée
│   │   ├── concat.cpp           # 🆕 Concaténation vidéo
│   │   ├── encode.cpp           # Images→Vidéo
│   │   ├── encode_builder.cpp
│   │   ├── extract_frames.cpp   # Extraction frames
│   │   ├── extract_frames_builder.cpp
│   │   ├── probe.cpp            # 🆕 Analyse FFprobe
│   │   ├── reencode.cpp         # Réencodage vidéo
│   │   ├── reencode_builder.cpp
│   │   ├── svt_av1_essential.cpp
│   │   ├── thumbnails.cpp       # Miniatures avec détection scènes
│   │   └── thumbnails_builder.cpp
│   └── pipelines/
│       ├── pipeline_base.cpp
│       ├── pipeline_batch.cpp
│       └── pipeline_custom.cpp
├── docs/                        # Documentation
│   ├── BUILDER_PATTERN_EXPLANATION.md
│   ├── ENCODE_DELIVERY_SUMMARY.md
│   ├── ENCODE_MIGRATION_GUIDE.md
│   ├── ENCODE_MODULE_README.md
│   ├── ENCODE_REFACTORING_SUMMARY.md
│   ├── README_INDEX.md
│   ├── SVT_AV1_ESSENTIAL_INTEGRATION.md
│   └── SVT_AV1_IMPLEMENTATION_SUMMARY.md
├── presets/                     # Presets d'encodage
├── .gitignore                   # Ignore extern/ et build/
└── CMakeLists.txt
```

## 🎯 Utilisation

### Lancement

```powershell
.\build\Release\ffmpeg_multi.exe
```

### Menu principal

```
╔══════════════════════════════════════════════╗
║           ⚙️  MULTI-FFMPEG TOOL              ║
╚══════════════════════════════════════════════╝

┌─────┬────────────────────────────────────────┐
│ ID  │  Action                                │
├─────┼────────────────────────────────────────┤
│  1  │  Extraire des frames                   │
│  2  │  Encoder une vidéo                     │
│  3  │  Réencoder un fichier                  │
│  4  │  Concaténer plusieurs vidéos           │
│  5  │  Générer des miniatures                │
│  6  │  Encoder avec SVT-AV1-Essential        │
│  7  │  Analyser un média (ffprobe)           │
├─────┼────────────────────────────────────────┤
│  0  │  Quitter                               │
└─────┴────────────────────────────────────────┘

👉  Entrez votre choix :
```

## 🎨 Fonctionnalités détaillées

### 1️⃣ Extraction de frames

Extrait toutes les frames d'une vidéo en images individuelles.

**Formats supportés :**
- **PNG** - Sans perte, RGB24, recommandé pour la qualité
- **TIFF** - Compression Deflate, RGB24, pour l'archivage
- **JPEG** - Qualité maximale, YUV420p, pour économiser de l'espace

**Caractéristiques :**
- Scaling de haute qualité (`spline+accurate_rnd+full_chroma_int`)
- Gestion automatique de l'espace colorimétrique
- Création automatique de sous-dossiers
- Numérotation séquentielle des fichiers

### 2️⃣ Encoder une vidéo (Images → Vidéo)

Encode une séquence d'images en fichier vidéo.

**Codecs supportés :**
- **X264** - H.264, universel, bonne compatibilité
- **X265** - H.265/HEVC, meilleure compression
- **AV1** - Compression maximale pour le web
- **H264_NVENC** - Hardware H.264 (NVIDIA GPU)
- **H265_NVENC** - Hardware H.265 (NVIDIA GPU)
- **ProRes** - Apple ProRes 4444 (Production)
- **FFV1** - Codec lossless pour archivage

**Conteneurs supportés :**
- **MKV** - Matroska (recommandé, universel)
- **WEBM** - WebM (web, compatible AV1 uniquement)
- **MP4** - MPEG-4 (universel, incompatible FFV1)
- **MOV** - QuickTime (production)

**Paramètres configurables :**
- Pattern d'entrée des images (`%08d.png`)
- Framerate (FPS)
- Qualité CRF/CQ selon le codec
- Preset d'encodage

### 3️⃣ Réencoder un fichier

Réencode une vidéo existante avec un autre codec ou paramètres.

**Presets disponibles :**
- **Preset YouTube** - H.264, CRF 23, medium, optimisé pour YouTube
- **FFV1 Preset** - Lossless, Level 3, pour archivage
- **Configuration personnalisée** - Tous les codecs avec paramètres fins

**Options avancées :**
- Profils ProRes (Proxy, LT, Standard, HQ, 4444, 4444 XQ)
- Pixel format (8-bit, 10-bit, P010)
- Rate control (CRF, CQP, VBR, CBR)
- Tune options (film, animation, grain, etc.)
- Copie audio sans réencodage

### 4️⃣ Concaténer plusieurs vidéos

Fusionne plusieurs fichiers vidéo en un seul fichier MKV sans réencodage.

**Technologie :**
- Utilise **MKVMerge** (MKVToolNix) pour une fusion sans perte (lossless).
- Beaucoup plus rapide que le réencodage.
- Préserve la qualité originale des flux vidéo et audio.

**Fonctionnement :**
- Accepte un nombre illimité de fichiers d'entrée.
- Détecte automatiquement `mkvmerge.exe` dans l'environnement portable.
- Sortie forcée en `.mkv`.

### 5️⃣ Générer des miniatures

Génère automatiquement des thumbnails aux changements de scènes.

**Détection de scènes :**
- Filtre FFmpeg `select='gt(scene,threshold)'`
- Seuil configurable (0.0-1.0, défaut: 0.15)
- Évite automatiquement les images dupliquées
- Variable frame rate (`-vsync vfr`)

**Formats supportés :**
- PNG, TIFF, JPEG (mêmes paramètres que l'extraction de frames)

**Avantages :**
- ✅ Extraction intelligente uniquement aux changements de scènes
- ✅ Moins de fichiers générés
- ✅ Thumbnails représentatifs du contenu

### 6️⃣ Encoder avec SVT-AV1-Essential

Voir section dédiée ci-dessous.

### 7️⃣ Analyser un média (ffprobe)

Analyse approfondie des flux vidéo, audio et sous-titres d'un fichier.

**Fonctionnalités :**
- Affichage des métadonnées complètes (codec, bitrate, résolution, pixel format, etc.).
- **Export JSON** : Sortie brute de FFprobe pour traitement automatisé.
- **Export TXT** : Rapport formaté lisible par un humain.

## 🚀 Encodage SVT-AV1-Essential

### Qu'est-ce que SVT-AV1-Essential ?

**SVT-AV1-Essential** utilise l'encodeur **SVT-AV1** via **Auto-Boost-Essential**, un wrapper Python optimisé qui offre :

- 🎯 **Qualité supérieure** grâce à des paramètres optimisés
- ⚡ **Encodage plus rapide** que libaom-av1
- 🔧 **Gestion automatique** de l'audio et du muxing
- 📊 **Presets adaptés** (Low, Medium, High, Extreme)

### Workflow d'encodage

```
1. Extraction de l'audio (FFmpeg)
   └─> audio.mka

2. Encodage vidéo (Auto-Boost-Essential)
   └─> video.avi (AV1)

3. Muxing final (mkvmerge)
   └─> output.mkv (vidéo AV1 + audio original)

4. Nettoyage (optionnel)
   └─> Suppression des fichiers temporaires
```

### Options disponibles

| Option | Description |
|--------|-------------|
| **Quality** | `low`, `medium`, `high`, `extreme` |
| **Aggressive** | Plus de compression (fichier plus petit) |
| **Unshackle** | Libère les limites de vitesse |
| **Cleanup** | Nettoyage automatique des fichiers temp |

### Exemple d'utilisation

```
👉 Choisir l'option 6 dans le menu

📁 Fichier d'entrée : C:\Videos\input.mp4
📁 Fichier de sortie : C:\Videos\output.mkv

⚙️  Qualité : 3 (High)

🔧 Options avancées :
  Mode agressif ? n
  Unshackle ? n
  Nettoyer les fichiers temporaires ? o

❓ Lancer l'encodage ? o

🚀 Encodage en cours...
```

### Prérequis pour SVT-AV1-Essential

Assurez-vous que les outils suivants sont installés dans `extern/` :

- ✅ **Python portable** (`extern/env/python/`)
- ✅ **VapourSynth** (`extern/env/vs/`)
- ✅ **Auto-Boost-Essential.py** (`extern/env/auto-boost/`)
- ✅ **SvtAv1EncApp.exe** (`extern/SvtAv1EncApp.exe`)
- ✅ **MKVToolNix** (`extern/env/mkvtoolnix/mkvmerge.exe`)
- ✅ **FFmpeg** (`extern/ffmpeg.exe`)
- ✅ **Script PowerShell** (`extern/scripts/ABE.ps1`)

## 📚 Documentation

- [📖 Guide d'intégration SVT-AV1-Essential](docs/SVT_AV1_ESSENTIAL_INTEGRATION.md)
- [🔧 Documentation du module Encode](docs/ENCODE_MODULE_README.md)
- [🏗️ Guide du Builder Pattern](docs/BUILDER_PATTERN_EXPLANATION.md)
- [📋 Index de la documentation](docs/README_INDEX.md)
- [♻️ Résumé du refactoring Encode/Reencode](docs/ENCODE_REFACTORING_SUMMARY.md)

## 🎨 Interface utilisateur

L'interface utilise le **thème Catppuccin Mocha** avec :
- 🎨 Couleurs ANSI optimisées pour la lisibilité
- 📐 Alignement automatique UTF-8 (émojis, caractères accentués)
- 🖼️ En-têtes centrés dynamiquement
- 📊 Menus avec bordures élégantes

## 🏗️ Architecture

### Pattern Builder

Tous les jobs utilisent le **Builder Pattern** pour une configuration fluide :

```cpp
// Exemple : Extraction de frames
auto job = ExtractFramesBuilder()
    .input("video.mp4")
    .outputDir("frames")
    .subfolderName("output")
    .png()
    .build();

job.execute();
```

```cpp
// Exemple : Encodage vidéo
auto job = EncodeJobBuilder()
    .inputDir("frames")
    .inputPattern("%08d.png")
    .framerate(24)
    .outputDir("output")
    .outputFilename("video.mkv")
    .x264()
    .quality(23)
    .preset("medium")
    .build();

job.execute();
```

### Refactoring : CodecUtils

La logique des codecs a été **centralisée** dans `codec_utils.hpp/cpp` pour éviter la duplication :

**Avant** : ~155 lignes dupliquées entre `encode.cpp` et `reencode.cpp`  
**Après** : Logique partagée via `CodecUtils::getEncoderName()` et `CodecUtils::addCodecArgs()`

**Bénéfices** :
- ✅ Une seule source de vérité pour les codecs
- ✅ Ajout d'un nouveau codec = modification d'un seul fichier
- ✅ Réduction de ~96 lignes de code dupliqué
- ✅ Maintenance simplifiée

## 🛠️ Dépannage

### Extraction de frames / Thumbnails

**Problème** : Les images ne sont pas générées

**Solution** : 
- Vérifiez que le fichier d'entrée existe et est accessible
- Vérifiez que le dossier de sortie a les permissions d'écriture
- Consultez les logs FFmpeg pour identifier l'erreur

### Encodage vidéo

**Problème** : "Pattern d'images introuvable"

**Solution** :
- Vérifiez que le pattern correspond aux fichiers (`%08d.png` pour `00000001.png`)
- Assurez-vous que les images commencent à 0 ou ajustez `-start_number`

**Problème** : "Codec incompatible avec le conteneur"

**Solution** :
- WebM ne supporte que AV1/SVT_AV1
- MP4 ne supporte pas FFV1
- Utilisez MKV pour une compatibilité universelle

### SVT-AV1-Essential

**Problème** : "Le fichier IVF n'a pas été généré"

**Solution** : Vérifiez que :
- Auto-Boost-Essential.py est bien présent dans `extern/env/auto-boost/`
- SvtAv1EncApp.exe est accessible dans `extern/`
- Consultez les logs d'Auto-Boost pour plus de détails
- Le fichier IVF est généré à côté de la vidéo source, pas dans un sous-dossier

### Problème : "Échec de l'extraction audio"

**Solution** : Vérifiez que le fichier d'entrée contient bien une piste audio.

**Problème** : "Script PowerShell non trouvé"

**Solution** : Vérifiez que `extern/scripts/ABE.ps1` existe.

## 🚧 Fonctionnalités à venir

- ⏳ **Pipeline batch** pour traiter plusieurs fichiers
- ⏳ **Presets personnalisés** sauvegardables
- ⏳ **Interface graphique** (optionnelle)

## 📝 Notes de version

### v1.1.0 (19 novembre 2025)

**Nouveautés :**
- ✅ **Concaténation vidéo** : Fusion sans perte via MKVMerge (Option 4)
- ✅ **Analyse FFprobe** : Analyse détaillée avec export JSON/TXT (Option 7)
- ✅ **Navigation** : Ajout de la commande `:q` pour revenir au menu précédent
- ✅ **Stabilité** : Correction du crash à la fermeture (boucle principale)

### v1.0.0 (9 novembre 2025)

**Nouveautés :**
- ✅ Extraction de frames (PNG, TIFF, JPEG)
- ✅ Encodage images→vidéo (9 codecs, 4 conteneurs)
- ✅ Réencodage vidéo avec presets
- ✅ Génération de thumbnails avec détection de scènes
- ✅ SVT-AV1-Essential intégration
- ✅ Interface Catppuccin Mocha
- ✅ Builder Pattern pour tous les jobs

**Refactoring :**
- ♻️ Centralisation des codecs dans `CodecUtils`
- ♻️ Réduction de ~96 lignes de code dupliqué
- ♻️ Amélioration de la maintenabilité

**Corrections :**
- 🐛 Alignement UTF-8 automatique dans les menus
- 🐛 Gestion correcte des caractères accentués et émojis

## 📜 License

MIT License - voir [LICENSE](LICENSE)

## 👥 Auteurs

- **Matthieu** - Développement principal

## 🙏 Remerciements

- FFmpeg
- SVT-AV1-Essential and Auto-Boost-Essential Team
- VapourSynth
- MKVToolNix

---

**Version**: 1.1.0  
**Dernière mise à jour**: 19 novembre 2025
