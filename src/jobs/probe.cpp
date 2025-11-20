#include "jobs/probe.hpp"
#include "core/path_utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cstdlib>
#include <array>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace Jobs {

ProbeJob::ProbeJob(const std::string& inputFile)
    : inputFile_(inputFile)
    , shouldExport_(false)
{
}

void ProbeJob::execute() {
    executeFFProbe();
    parseAndFormatOutput();
    
    // Display formatted output
    std::cout << formattedOutput_ << std::endl;
}

std::string ProbeJob::buildFFProbeCommand() const {
    std::vector<std::string> args;
    
    // Path to ffprobe (using PathUtils to get correct path)
    std::filesystem::path externPath = FFmpegMulti::PathUtils::getExternPath();
    std::string ffprobePath = (externPath / "ffprobe.exe").string();
    
    args.push_back(ffprobePath);
    args.push_back("-v");
    args.push_back("quiet");
    args.push_back("-print_format");
    args.push_back("json");
    args.push_back("-show_format");
    args.push_back("-show_streams");
    args.push_back("\"" + inputFile_ + "\"");
    
    std::string cmd;
    for (const auto& arg : args) {
        if (!cmd.empty()) cmd += " ";
        cmd += arg;
    }
    
    return cmd;
}

std::string ProbeJob::readFileContent(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void ProbeJob::writeToFile(const std::string& filePath, const std::string& content) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create file: " + filePath);
    }
    
    file << content;
    file.close();
}

std::string ProbeJob::generateExportPath(bool isJson) const {
    fs::path inputPath(inputFile_);
    std::string filename = inputPath.stem().string();
    std::string extension = isJson ? "_ffprobe.json" : "_ffprobe.txt";
    
    fs::path exportPath = inputPath.parent_path() / (filename + extension);
    return exportPath.string();
}

std::string ProbeJob::extractJsonValue(const std::string& json, const std::string& key) const {
    std::string searchKey = "\"" + key + "\":";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "N/A";
    
    pos += searchKey.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    
    if (pos >= json.length()) return "N/A";
    
    // Handle string values
    if (json[pos] == '"') {
        pos++;
        size_t endPos = json.find('"', pos);
        if (endPos == std::string::npos) return "N/A";
        return json.substr(pos, endPos - pos);
    }
    
    // Handle numeric values
    size_t endPos = pos;
    while (endPos < json.length() && (isdigit(json[endPos]) || json[endPos] == '.' || json[endPos] == '-')) {
        endPos++;
    }
    return json.substr(pos, endPos - pos);
}

std::string ProbeJob::formatBytes(long long bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return oss.str();
}

std::string ProbeJob::formatDuration(double seconds) const {
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;
    int millis = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setw(2) << minutes << ":"
        << std::setw(2) << secs << "."
        << std::setw(3) << millis;
    return oss.str();
}

void ProbeJob::parseAndFormatOutput() {
    std::ostringstream formatted;
    
    // ANSI colors (Catppuccin Mocha)
    const char* RESET = "\033[0m";
    const char* BLUE = "\033[34m";
    const char* CYAN = "\033[96m";
    const char* GREEN = "\033[92m";
    const char* YELLOW = "\033[93m";
    const char* MAGENTA = "\033[95m";
    const char* BOLD = "\033[1m";
    
    formatted << "\n" << BLUE << "╔═══════════════════════════════════════════════════════╗" << RESET << "\n";
    formatted << BLUE << "║  " << CYAN << BOLD << "📊 MEDIA INFORMATION" << RESET << BLUE << "                            ║" << RESET << "\n";
    formatted << BLUE << "╚═══════════════════════════════════════════════════════╝" << RESET << "\n\n";
    
    // Extract format info
    std::string filename = extractJsonValue(output_, "filename");
    std::string formatName = extractJsonValue(output_, "format_name");
    std::string formatLongName = extractJsonValue(output_, "format_long_name");
    std::string durationStr = extractJsonValue(output_, "duration");
    std::string sizeStr = extractJsonValue(output_, "size");
    std::string bitRateStr = extractJsonValue(output_, "bit_rate");
    
    formatted << GREEN << "📁 File" << RESET << "\n";
    formatted << "   " << filename << "\n\n";
    
    formatted << GREEN << "🎬 Format" << RESET << "\n";
    formatted << "   Type      : " << formatName << "\n";
    formatted << "   Name      : " << formatLongName << "\n";
    
    if (durationStr != "N/A") {
        double duration = std::stod(durationStr);
        formatted << "   Duration  : " << formatDuration(duration) << "\n";
    }
    
    if (sizeStr != "N/A") {
        long long size = std::stoll(sizeStr);
        formatted << "   Size      : " << formatBytes(size) << "\n";
    }
    
    if (bitRateStr != "N/A") {
        long long bitRate = std::stoll(bitRateStr);
        formatted << "   Bitrate   : " << (bitRate / 1000) << " kb/s\n";
    }
    
    // Count streams
    int videoStreams = 0;
    int audioStreams = 0;
    int subtitleStreams = 0;
    
    size_t pos = 0;
    while ((pos = output_.find("\"codec_type\"", pos)) != std::string::npos) {
        std::string codecType = extractJsonValue(output_.substr(pos), "codec_type");
        if (codecType == "video") videoStreams++;
        else if (codecType == "audio") audioStreams++;
        else if (codecType == "subtitle") subtitleStreams++;
        pos += 12;
    }
    
    formatted << "\n" << GREEN << "🎞️  Streams" << RESET << "\n";
    formatted << "   Video     : " << videoStreams << "\n";
    formatted << "   Audio     : " << audioStreams << "\n";
    formatted << "   Subtitle  : " << subtitleStreams << "\n";
    
    // Extract first video stream info
    pos = output_.find("\"codec_type\": \"video\"");
    if (pos != std::string::npos) {
        std::string videoSection = output_.substr(pos, 1000);
        std::string codecName = extractJsonValue(videoSection, "codec_name");
        std::string codecLongName = extractJsonValue(videoSection, "codec_long_name");
        std::string width = extractJsonValue(videoSection, "width");
        std::string height = extractJsonValue(videoSection, "height");
        std::string pixFmt = extractJsonValue(videoSection, "pix_fmt");
        std::string rFrameRate = extractJsonValue(videoSection, "r_frame_rate");
        
        formatted << "\n" << YELLOW << "📹 Video" << RESET << "\n";
        formatted << "   Codec     : " << codecName;
        if (codecLongName != "N/A") formatted << " (" << codecLongName << ")";
        formatted << "\n";
        
        if (width != "N/A" && height != "N/A") {
            formatted << "   Resolution: " << width << "x" << height << "\n";
        }
        
        if (pixFmt != "N/A") {
            formatted << "   Format    : " << pixFmt << "\n";
        }
        
        if (rFrameRate != "N/A" && rFrameRate.find('/') != std::string::npos) {
            size_t slashPos = rFrameRate.find('/');
            double num = std::stod(rFrameRate.substr(0, slashPos));
            double den = std::stod(rFrameRate.substr(slashPos + 1));
            formatted << "   FPS       : " << std::fixed << std::setprecision(2) << (num / den) << "\n";
        }
    }
    
    // Extract first audio stream info
    pos = output_.find("\"codec_type\": \"audio\"");
    if (pos != std::string::npos) {
        std::string audioSection = output_.substr(pos, 1000);
        std::string codecName = extractJsonValue(audioSection, "codec_name");
        std::string codecLongName = extractJsonValue(audioSection, "codec_long_name");
        std::string sampleRate = extractJsonValue(audioSection, "sample_rate");
        std::string channels = extractJsonValue(audioSection, "channels");
        std::string channelLayout = extractJsonValue(audioSection, "channel_layout");
        
        formatted << "\n" << MAGENTA << "🔊 Audio" << RESET << "\n";
        formatted << "   Codec     : " << codecName;
        if (codecLongName != "N/A") formatted << " (" << codecLongName << ")";
        formatted << "\n";
        
        if (sampleRate != "N/A") {
            formatted << "   Sample Rate: " << sampleRate << " Hz\n";
        }
        
        if (channels != "N/A") {
            formatted << "   Channels  : " << channels;
            if (channelLayout != "N/A") formatted << " (" << channelLayout << ")";
            formatted << "\n";
        }
    }
    
    formatted << "\n";
    formattedOutput_ = formatted.str();
}

void ProbeJob::executeFFProbe() {
    std::cout << "[INFO] Analyzing media with FFProbe..." << std::endl;
    
    std::string cmd = buildFFProbeCommand();
    
    std::cout << "[CMD] " << cmd << std::endl;
    
    // Execute command and capture output
    std::string output;
    
#ifdef _WIN32
    // Create pipe for reading output
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        throw std::runtime_error("Failed to create pipe");
    }
    
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    
    PROCESS_INFORMATION pi = { 0 };
    
    if (!CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, TRUE, 
                        CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        throw std::runtime_error("Failed to execute FFProbe");
    }
    
    CloseHandle(hWritePipe);
    
    // Read output
    char buffer[4096];
    DWORD bytesRead;
    
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    if (exitCode != 0) {
        throw std::runtime_error("FFProbe failed with exit code: " + std::to_string(exitCode));
    }
#else
    // Unix-like systems
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to execute FFProbe");
    }
    
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
    int exitCode = pclose(pipe);
    if (exitCode != 0) {
        throw std::runtime_error("FFProbe failed with exit code: " + std::to_string(exitCode));
    }
#endif
    
    output_ = output;
}

} // namespace Jobs
