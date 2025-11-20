#pragma once

#include <filesystem>

namespace FFmpegMulti {
namespace PathUtils {

// Obtenir le chemin du dossier contenant l'exécutable
std::filesystem::path getExecutableDir();

// Obtenir le chemin absolu vers le dossier extern/
std::filesystem::path getExternPath();

} // namespace PathUtils
} // namespace FFmpegMulti