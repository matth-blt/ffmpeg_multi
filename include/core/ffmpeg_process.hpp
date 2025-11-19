#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

class ffmpegProcess {
public:
    explicit ffmpegProcess(const std::filesystem::path& ExecutablePath_init, const std::vector<std::string>& args_init);
    ~ffmpegProcess() = default;

    std::filesystem::path getExecutablePath();
    std::vector<std::string> getArgs();
    
    /**
     * @brief Execute la commande FFmpeg avec les arguments
     * @return true si l'exécution a réussi, false sinon
     */
    bool execute();
    
private:
    std::filesystem::path ExecutablePath{"./extern/ffmpeg.exe"};
    std::vector<std::string> args;
};