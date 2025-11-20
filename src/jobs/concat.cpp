#include "../../include/jobs/concat.hpp"
#include "../../include/core/colors.hpp"
#include "../../include/core/path_utils.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

namespace FFmpegMulti {
namespace Jobs {

// ============================================================================
// CONCAT JOB
// ============================================================================

ConcatJob::ConcatJob(const std::vector<std::string>& inputs, const std::string& output) : m_inputs(inputs), m_output(output) {}

std::string ConcatJob::getMkvMergePath() const {
    // Use PathUtils to find the extern folder
    fs::path extern_path = PathUtils::getExternPath();
    fs::path mkvmerge_path = extern_path / "env" / "mkvtoolnix" / "mkvmerge.exe";

    if (fs::exists(mkvmerge_path))
        return mkvmerge_path.string();
    
    // Fallback: Hope it is in the PATH
    return "mkvmerge"; 
}

bool ConcatJob::execute() {
    if (m_inputs.size() < 2) {
        std::cerr << Colors::RED << "[ERROR] At least 2 files are required to concatenate." << Colors::RESET << std::endl;
        return false;
    }

    std::string mkvmerge = getMkvMergePath();
    
    // Command construction
    // mkvmerge -o "output.mkv" "input1" + "input2" + "input3"
    
    std::string command = mkvmerge + " -o \"" + m_output + "\"";
    
    // First file
    command += " \"" + m_inputs[0] + "\"";
    
    // Subsequent files with +
    for (size_t i = 1; i < m_inputs.size(); ++i) {
        command += " + \"" + m_inputs[i] + "\"";
    }

    std::cout << std::endl;
    std::cout << Colors::BLUE << "[CMD] " << command << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    int result = std::system(command.c_str());
    return result == 0;
}

// ============================================================================
// CONCAT BUILDER
// ============================================================================

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
