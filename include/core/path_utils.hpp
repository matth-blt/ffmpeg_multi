#ifndef PATH_UTILS_HPP
#define PATH_UTILS_HPP

#include <filesystem>

namespace FFmpegMulti {
namespace PathUtils {

// Obtenir le chemin absolu vers le dossier extern/
std::filesystem::path getExternPath();

} // namespace PathUtils
} // namespace FFmpegMulti

#endif