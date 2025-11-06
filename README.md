# 🎬 ffmpeg.multi - Outil Multi-FFmpeg

Un outil en ligne de commande puissant pour gérer vos tâches d'encodage vidéo avec FFmpeg.

## 📋 Fonctionnalités

- ✅ **Extraction de frames** - Extrait des images d'une vidéo
- ✅ **Encodage vidéo** - Encode avec plusieurs codecs (H.264, H.265, AV1, SVT-AV1)
- ✅ **SVT-AV1-Essential** - Encodage AV1 optimisé via Auto-Boost-Essential
- ✅ **Réencodage** - Réencode des fichiers existants
- ✅ **Concaténation** - Fusionne plusieurs vidéos
- ✅ **Miniatures** - Génère des vignettes
- ✅ **Analyse** - Analyse de médias avec ffprobe

## 🚀 Installation

### Prérequis

- **Windows 10/11**
- **Visual Studio 2019+** ou **MinGW-w64**
- **CMake 3.15+**
- **FFmpeg** (fourni dans `extern/`)

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
│   │   ├── path_utils.hpp       # Utilitaires de chemins
│   │   └── string_utils.hpp     # Utilitaires de strings
│   ├── jobs/
│   │   ├── encode.hpp
│   │   ├── encode_builder.hpp
│   │   ├── encode_types.hpp
│   │   └── svt_av1_essential.hpp
│   └── pipelines/
├── src/                         # Sources
│   ├── core/
│   │   ├── path_utils.cpp
│   │   └── ffmpeg_process.cpp
│   ├── jobs/
│   │   ├── encode.cpp
│   │   └── svt_av1_essential.cpp
│   └── pipelines/
├── docs/                        # Documentation
│   └── SVT_AV1_ESSENTIAL_INTEGRATION.md
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
==============================================
             ⚙️  MULTI-FFMPEG TOOL             
==============================================
|  ID  |              Action                 |
----------------------------------------------
|  1   | Extraire des frames                 |
|  2   | Encoder une vidéo                   |
|  3   | Réencoder un fichier                |
|  4   | Concaténer plusieurs vidéos         |
|  5   | Générer des miniatures              |
|  6   | Encoder avec SVT-AV1-Essential      |
|  7   | Analyser un média (ffprobe)         |
|  0   | Quitter                             |
==============================================
```

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
- [🔧 Documentation du module Encode](docs/encode/MODULE_README.md)
- [🏗️ Guide du Builder Pattern](docs/encode/BUILDER_PATTERN_EXPLANATION.md)

## 🛠️ Dépannage

### Problème : "Le fichier IVF n'a pas été généré"

**Solution** : Vérifiez que :
- Auto-Boost-Essential.py est bien présent dans `extern/env/auto-boost/`
- SvtAv1EncApp.exe est accessible dans `extern/`
- Consultez les logs d'Auto-Boost pour plus de détails
- Le fichier IVF est généré à côté de la vidéo source, pas dans un sous-dossier

### Problème : "Échec de l'extraction audio"

**Solution** : Vérifiez que le fichier d'entrée contient bien une piste audio.

### Problème : "Script PowerShell non trouvé"

**Solution** : Vérifiez que `extern/scripts/ABE.ps1` existe.

## 📜 License

MIT License - voir [LICENSE](LICENSE)

## 👥 Auteurs

- **Matthieu** - Développement principal

## 🙏 Remerciements

- FFmpeg
- SVT-AV1 Team
- Auto-Boost-Essential
- VapourSynth
- MKVToolNix

---

**Version**: 1.0.0  
**Dernière mise à jour**: 6 novembre 2025
