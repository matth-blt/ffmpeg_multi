#include "../../include/core/path_utils.hpp"

namespace FFmpegMulti {
namespace PathUtils {

std::filesystem::path getExternPath() {
    // Obtenir le chemin de l'exécutable actuel
    std::filesystem::path exe_path = std::filesystem::current_path();
    
    // Remonter au répertoire racine du projet
    // Si on est dans build/, remonter d'un niveau
    if (exe_path.filename() == "build" || exe_path.filename() == "Release" || exe_path.filename() == "Debug") {
        exe_path = exe_path.parent_path();
    }

    return exe_path / "extern";
}

} // namespace PathUtils
} // namespace FFmpegMulti