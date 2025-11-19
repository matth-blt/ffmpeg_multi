#include "../../include/jobs/concat.hpp"
#include "../../include/core/logger.hpp"
#include "../../include/core/colors.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

namespace FFmpegMulti {
namespace Jobs {

ConcatJob::ConcatJob(const std::vector<std::string>& inputs, const std::string& output) : m_inputs(inputs), m_output(output) {}

std::string ConcatJob::getMkvMergePath() const {
    // Tentative de détection de mkvmerge
    std::vector<std::string> potentialPaths = {
        "extern/env/mkvtoolnix/mkvmerge.exe",
        "../extern/env/mkvtoolnix/mkvmerge.exe",
        "../../extern/env/mkvtoolnix/mkvmerge.exe",
        "C:/Program Files/MKVToolNix/mkvmerge.exe"
    };

    for (const auto& path : potentialPaths) {
        if (fs::exists(path)) {
            return fs::absolute(path).string();
        }
    }
    
    return "mkvmerge"; // Espérons qu'il soit dans le PATH
}

bool ConcatJob::execute() {
    if (m_inputs.size() < 2) {
        std::cerr << Colors::RED << "[ERREUR] Il faut au moins 2 fichiers pour concaténer." << Colors::RESET << std::endl;
        return false;
    }

    std::string mkvmerge = getMkvMergePath();
    
    // Construction de la commande
    // mkvmerge -o "output.mkv" "input1" + "input2" + "input3"
    
    std::string command = mkvmerge + " -o \"" + m_output + "\"";
    
    // Premier fichier
    command += " \"" + m_inputs[0] + "\"";
    
    // Fichiers suivants avec +
    for (size_t i = 1; i < m_inputs.size(); ++i) {
        command += " + \"" + m_inputs[i] + "\"";
    }

    std::cout << std::endl;
    std::cout << Colors::BLUE << "[CMD] " << command << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    int result = std::system(command.c_str());
    return result == 0;
}

ConcatBuilder& ConcatBuilder::addInput(const std::string& input) {
    m_inputs.push_back(input);
    return *this;
}

ConcatBuilder& ConcatBuilder::output(const std::string& output) {
    m_output = output;
    return *this;
}

ConcatJob ConcatBuilder::build() {
    return ConcatJob(m_inputs, m_output);
}

} // namespace Jobs
} // namespace FFmpegMulti
