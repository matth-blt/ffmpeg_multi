#include <iostream>
#include <sstream>
#include <cstdlib>

#include "../../include/core/ffmpeg_process.hpp"

ffmpegProcess::ffmpegProcess(const std::filesystem::path& ExecutablePath_init, const std::vector<std::string>& args_init) :  ExecutablePath{ExecutablePath_init}, args{args_init} {}

std::filesystem::path ffmpegProcess::getExecutablePath() {
    return ExecutablePath;
}

std::vector<std::string> ffmpegProcess::getArgs() {
    return args;
}

bool ffmpegProcess::execute() {
    std::ostringstream command;
    command << ExecutablePath.string();
    
    for (const auto& arg : args) {
        command << " ";
        if (arg.find(' ') != std::string::npos) {
            command << "\"" << arg << "\"";
        } else {
            command << arg;
        }
    }
    
    std::string cmd = command.str();
    std::cout << "\n[EXECUTE] " << cmd << "\n" << std::endl;

    int result = std::system(cmd.c_str());
    
    return result == 0;
}
