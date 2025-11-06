#include <iostream>
#include <sstream>
#include <cstdlib>

#include "../../include/core/ffmpeg_process.hpp"

ffmpegProcess::ffmpegProcess(std::filesystem::path ExecutablePath_init, std::vector<std::string> args_init) :  ExecutablePath{ExecutablePath_init}, args{args_init} {}

std::filesystem::path ffmpegProcess::getExecutablePath() {
    return ExecutablePath;
}

std::vector<std::string> ffmpegProcess::getArgs() {
    return args;
}

bool ffmpegProcess::execute() {
    // Construire la commande complète
    std::ostringstream command;
    // NE PAS mettre de guillemets autour de l'exécutable sous Windows
    command << ExecutablePath.string();
    
    for (const auto& arg : args) {
        command << " ";
        // Ajouter des guillemets si l'argument contient des espaces
        if (arg.find(' ') != std::string::npos) {
            command << "\"" << arg << "\"";
        } else {
            command << arg;
        }
    }
    
    std::string cmd = command.str();
    std::cout << "\n[EXECUTE] " << cmd << "\n" << std::endl;
    
    // Exécuter la commande
    int result = std::system(cmd.c_str());
    
    return result == 0;
}
