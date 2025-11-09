#pragma once

#include <string>
#include <vector>

namespace Jobs {

class ProbeJob {
public:
    ProbeJob(const std::string& inputFile);
    
    void execute();
    
    // Getters
    std::string getInputFile() const { return inputFile_; }
    std::string getOutput() const { return output_; }
    std::string getFormattedOutput() const { return formattedOutput_; }
    bool shouldExport() const { return shouldExport_; }
    std::string getExportPath() const { return exportPath_; }
    
    // Setters
    void setShouldExport(bool value) { shouldExport_ = value; }
    void setExportPath(const std::string& path) { exportPath_ = path; }
    
    // Public helpers
    std::string generateExportPath(bool isJson) const;
    void writeToFile(const std::string& filePath, const std::string& content) const;

private:
    std::string inputFile_;
    std::string output_;
    std::string formattedOutput_;
    bool shouldExport_;
    std::string exportPath_;
    
    // Helper methods
    void executeFFProbe();
    std::string buildFFProbeCommand() const;
    std::string readFileContent(const std::string& filePath) const;
    void parseAndFormatOutput();
    std::string extractJsonValue(const std::string& json, const std::string& key) const;
    std::string formatBytes(long long bytes) const;
    std::string formatDuration(double seconds) const;
};

} // namespace Jobs
