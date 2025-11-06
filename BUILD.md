# 🔨 Guide de Compilation - ffmpeg.multi

## 📋 Compilation avec CMake

### Méthode 1: Script PowerShell (Recommandé)

```powershell
# Depuis la racine du projet
.\scripts\build.ps1
```

### Méthode 2: CMake Manuel

```powershell
# Créer le dossier de build
mkdir build
cd build

# Configurer le projet
cmake ..

# Compiler (Release)
cmake --build . --config Release

# L'exécutable sera dans: build\Release\ffmpeg_multi.exe
```

### Méthode 3: CMake avec options

```powershell
# Créer le dossier de build
mkdir build
cd build

# Configurer avec tests activés
cmake .. -DBUILD_TESTS=ON

# Compiler en Debug
cmake --build . --config Debug

# Ou en Release
cmake --build . --config Release
```

## 🚀 Lancer l'application

```powershell
# Depuis build/Release/
.\ffmpeg_multi.exe

# Ou depuis la racine
.\build\Release\ffmpeg_multi.exe
```

## 🧪 Tests (optionnel)

```powershell
# Dans le dossier build
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Release
ctest -C Release
```

## 🔧 Nettoyage

```powershell
# Supprimer le dossier build
Remove-Item -Recurse -Force build

# Recompiler depuis zéro
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
