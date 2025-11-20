#include "../../include/core/path_utils.hpp"
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

namespace FFmpegMulti {
namespace PathUtils {

std::filesystem::path getExecutableDir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#else
    // Fallback for Linux/Mac (improve if needed)
    return std::filesystem::current_path();
#endif
}

std::filesystem::path getExternPath() {
    // 1. Priority: Look for "extern" next to the executable (Release/Deployment Mode)
    std::filesystem::path exe_dir = getExecutableDir();
    std::filesystem::path extern_next_to_exe = exe_dir / "extern";
    
    if (std::filesystem::exists(extern_next_to_exe)) {
        return extern_next_to_exe;
    }

    // 2. Fallback: Look in the current directory (Dev/IDE Mode)
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path extern_cwd = cwd / "extern";
    
    if (std::filesystem::exists(extern_cwd)) {
        return extern_cwd;
    }

    // 3. Fallback: Go up one level (Build folder Mode)
    // If we are in build/Release, we want to go up to the project root
    if (exe_dir.filename() == "Release" || exe_dir.filename() == "Debug" || exe_dir.filename() == "build") {
        std::filesystem::path parent = exe_dir.parent_path();
        if (parent.filename() == "build") { // Case build/Release
            parent = parent.parent_path();
        }
        std::filesystem::path extern_parent = parent / "extern";
        if (std::filesystem::exists(extern_parent)) {
            return extern_parent;
        }
    }

    // By default, return the path next to the exe even if it doesn't exist
    return extern_next_to_exe;
}

} // namespace PathUtils
} // namespace FFmpegMulti