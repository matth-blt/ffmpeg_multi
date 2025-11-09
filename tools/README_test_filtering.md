# Test Auto-Boost-Filtering

Programme de test pour valider le script Python `Auto-Boost-Filtering.py`.

## 🚀 Utilisation

### Méthode 1 : Script PowerShell (Recommandé)

```powershell
# Sans paramètre (mode interactif)
.\scripts\test_filtering.ps1

# Avec fichier vidéo en paramètre
.\scripts\test_filtering.ps1 "C:\path\to\video.mkv"
```

### Méthode 2 : Script Batch

```cmd
REM Sans paramètre
.\scripts\test_filtering.bat

REM Avec fichier vidéo
.\scripts\test_filtering.bat "C:\path\to\video.mkv"
```

### Méthode 3 : Compilation et exécution manuelles

```powershell
# Compiler
cd tools
g++ -std=c++17 -O2 -o test_filtering.exe test_filtering.cpp

# Exécuter
.\test_filtering.exe
# ou
.\test_filtering.exe "C:\path\to\video.mkv"
```

## 📋 Fonctionnalités

Le programme de test permet de :

1. **Vérifier les chemins** : Script Python, interpréteur Python
2. **Mode interactif** : Sélection du profil, codec, qualité, preset
3. **Validation** : Vérification fichier d'entrée, génération nom sortie
4. **Exécution** : Construction et exécution de la commande Python
5. **Rapport** : Affichage résultat, taille fichier sortie

## 🎨 Profils disponibles

- **Light** : Filtrage léger pour scènes propres uniquement
- **Medium** (défaut) : Sélection automatique selon analyse de scène
- **Heavy** : Filtrage agressif pour scènes complexes
- **Extreme** : Qualité maximum (très lent)
- **Debug** : Analyse uniquement (pas d'encodage)

## 🎥 Codecs supportés

- **x264** (H.264) - Défaut, excellent compromis
- **x265** (H.265/HEVC) - Meilleure compression
- **AV1** (SVT-AV1) - Compression moderne
- **ProRes** - Haute qualité pour montage
- **FFV1** - Archivage lossless

## ⚙️ Paramètres

### Qualité (CRF)
- `0-51` : 0 = lossless, 51 = très compressé
- `18` (défaut) : Haute qualité
- `23` : Qualité standard
- `28` : Compression élevée

### Preset
- `ultrafast` → `veryslow` : Vitesse vs compression
- `slow` (défaut) : Bon équilibre
- `medium` : Plus rapide
- `veryslow` : Maximum compression

## 📝 Exemple de commande générée

```bash
python Auto-Boost-Filtering.py \
  --input "video.mkv" \
  --output "video_filtered.mkv" \
  --profile medium \
  --codec x264 \
  --quality 18 \
  --preset slow \
  --verbose
```

## 🔍 Mode Debug

Le mode debug permet d'analyser une vidéo sans encodage :

```powershell
# Lancer le test et choisir option 5 (Debug)
.\scripts\test_filtering.ps1 "video.mkv"
```

Résultat : fichier `video_analysis.log` avec statistiques par frame :
```
Frame,Category,Contrast,Aliasing,Noise
0,medium,12.345,0.123,0.045
1,clean,19.567,0.089,0.023
2,heavy,6.123,0.456,0.234
...
```

## 📊 Interprétation des métriques

- **Contrast** : Dynamique de l'image
  - `> 18.0` : Scène propre
  - `8.0 - 18.0` : Scène moyenne
  - `< 8.0` : Scène complexe

- **Aliasing** : Défauts d'échantillonnage
  - `> 0.25` : Aliasing élevé → filtrage heavy

- **Noise** : Grain/bruit
  - `> 0.12` : Bruit élevé → filtrage heavy

## 🛠️ Dépendances

### Compilation
- `g++` avec support C++17
- Standard library (filesystem)

### Exécution
- Python 3.12+ avec VapourSynth
- Plugins : `vs-placebo`, `LSMASH`
- FFmpeg avec codecs requis
- Shaders GLSL dans `extern/env/vs/vs-plugins/GLSL/parameterized/`

## ⚠️ Notes

1. **Première exécution** : L'analyse peut prendre du temps (toutes les frames)
2. **Mémoire** : VapourSynth utilise beaucoup de RAM pour le cache
3. **Temp** : Répertoire temporaire créé automatiquement
4. **Logs** : Fichier d'analyse conservé après encodage

## 🐛 Dépannage

### Python non trouvé
```
⚠ Python not found at: extern/env/python/python.exe
Using system Python: python
```
Solution : Installer Python ou ajuster le chemin dans `test_filtering.cpp`

### Plugin VapourSynth manquant
```
ERROR: Cannot load vs-placebo plugin
```
Solution : Installer `vs-placebo` dans `extern/env/vs/vs-plugins/`

### Shader GLSL introuvable
```
WARNING: Shader not found: .../Anime4K_Darken_HQ.glsl
```
Solution : Vérifier `extern/env/vs/vs-plugins/GLSL/parameterized/`

### Encodage échoue
```
ERROR: Encoding failed
```
Solution : Vérifier installation FFmpeg et codecs disponibles

## 📚 Voir aussi

- `Auto-Boost-Filtering.py` : Script Python principal
- `Auto-Boost-Essential.py` : Script d'upscaling (architecture similaire)
- Documentation VapourSynth : http://www.vapoursynth.com/
