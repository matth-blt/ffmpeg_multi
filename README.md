# 🎬 ffmpeg.multi - Multi-FFmpeg Tool

[![Français](https://img.shields.io/badge/lang-Français-blue.svg)](README_FR.md)

A powerful command-line tool to manage your video encoding tasks with FFmpeg.

## 📋 Features

- ✅ **Frame Extraction** - Extract all frames from a video (PNG, TIFF, JPEG)
- ✅ **Video Encoding** - Encode image sequences into video with multiple codecs
- ✅ **Re-encoding** - Re-encode existing video files with different codecs and settings
- ✅ **Thumbnail Generation** - Extract thumbnails with automatic scene detection
- ✅ **SVT-AV1-Essential** - Optimized AV1 encoding via Auto-Boost-Essential
- ✅ **Concatenation** - Merge multiple videos losslessly (via MKVMerge)
- ✅ **FFprobe Analysis** - Detailed media analysis with JSON/TXT export

## 🚀 Installation / Build

### For Users (Portable)
1. Download the latest release.
2. Unzip the archive.
3. Run `ffmpeg_multi.exe`.
   * The application is portable: it automatically detects the `extern/` folder located next to the executable.

### For Developers

#### Prerequisites
- **Windows 10/11**
- **Visual Studio 2019+** or **MinGW-w64**
- **CMake 3.15+**

#### Compilation
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 📦 Project Structure

```
ffmpeg.multi/
├── extern/                      
│   ├── env/
│   │   ├── auto-boost/
│   │   │   └── Auto-Boost-Essential.py 
│   │   ├── mediainfo/
│   │   ├── mkvtoolnix/
│   │   ├── python/
│   │   └── vs/
│   ├── scripts/
│   │   └── ABE.ps1            
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

## 🛠️ Prerequisites for SVT-AV1-Essential

To use optimized SVT-AV1 encoding, ensure the following tools are present in the `extern/` folder:

- ✅ **Portable Python** (`extern/env/python/`)
- ✅ **VapourSynth** (`extern/env/vs/`)
- ✅ **Auto-Boost-Essential.py** (`extern/env/auto-boost/`)
- ✅ **SvtAv1EncApp.exe** (`extern/SvtAv1EncApp.exe`)
- ✅ **MKVToolNix** (`extern/env/mkvtoolnix/mkvmerge.exe`)
- ✅ **FFmpeg** (`extern/ffmpeg.exe`)
- ✅ **PowerShell Script** (`extern/scripts/ABE.ps1`)

## 🎨 Detailed Features

### 1️⃣ Frame Extraction
Extracts all frames from a video into individual images.
- **Formats**: PNG (Lossless), TIFF (Archive), JPEG (Lightweight).
- **Scaling**: High quality (`spline+accurate_rnd+full_chroma_int`).

### 2️⃣ Video Encoding (Images → Video)
Encodes a sequence of images into a video file.
- **Codecs**: X264, X265, AV1, NVENC (H264/H265), ProRes, FFV1.
- **Containers**: MKV, WEBM, MP4, MOV.

### 3️⃣ Re-encoding
Re-encodes an existing video with a different codec or settings.
- **Presets**: YouTube, Archival (FFV1), Custom.
- **Options**: ProRes profiles, Pixel format (8/10-bit), Rate control (CRF, CQP, VBR, CBR).

### 4️⃣ Concatenation
Merges multiple video files into a single MKV file without re-encoding.
- Uses **MKVMerge** for lossless merging.

### 5️⃣ Thumbnail Generation
Automatically generates thumbnails at scene changes.
- Smart detection (`select='gt(scene,threshold)'`).

### 6️⃣ SVT-AV1-Essential Encoding
Uses the **SVT-AV1** encoder via **Auto-Boost-Essential** for superior quality and automatic audio/muxing management.

### 7️⃣ Media Analysis (ffprobe)
In-depth analysis of video, audio, and subtitle streams with JSON/TXT export.

## 🙏 Acknowledgements

- **FFmpeg**
- **SVT-AV1-Essential and Auto-Boost-Essential Team**
- **VapourSynth**
- **MKVToolNix**
