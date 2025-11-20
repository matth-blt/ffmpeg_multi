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
     * @brief Execute the FFmpeg command with arguments
     * @return true if execution succeeded, false otherwise
     */
    bool execute();
    
private:
    std::filesystem::path ExecutablePath;
    std::vector<std::string> args;
};