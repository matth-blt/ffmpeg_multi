#pragma once

#include <string>
#include <vector>
#include <memory>
#include "encode_types.hpp"
#include "../core/job.hpp"

namespace FFmpegMulti {
namespace Jobs {

/**
 * @brief Video re-encoding job with FFmpeg
 * 
 * This class represents a complete re-encoding job:
 * - Configuration of re-encoding parameters
 * - Construction of the FFmpeg command line
 * - Execution of the re-encoding
 */
class ReencodeJob : public FFmpegMulti::Core::Job {
public:
    /**
     * @brief Constructor with input and output paths
     * @param input_path Input file path
     * @param output_path Output file path
     */
    explicit ReencodeJob(const std::string& input_path, const std::string& output_path);
    
    /**
     * @brief Default constructor
     */
    ReencodeJob() = default;
    
    ~ReencodeJob() = default;
    
    // ========================================================================
    // CONFIGURATION
    // ========================================================================
    
    /**
     * @brief Sets the complete encoding configuration
     * @param config Configuration to apply
     */
    void setConfig(const Encode::EncodeConfig& config);
    
    /**
     * @brief Gets the current configuration (modifiable)
     * @return Reference to the configuration
     */
    Encode::EncodeConfig& config();
    
    /**
     * @brief Gets the current configuration (read-only)
     * @return Const reference to the configuration
     */
    const Encode::EncodeConfig& config() const;
    
    // ========================================================================
    // I/O PATHS
    // ========================================================================
    
    void setInputPath(const std::string& path);
    void setOutputPath(const std::string& path);
    
    std::string getInputPath() const;
    std::string getOutputPath() const;
    
    // ========================================================================
    // COMMAND CONSTRUCTION
    // ========================================================================
    
    /**
     * @brief Builds the FFmpeg command line arguments
     * @return Vector of arguments ready for execution
     */
    std::vector<std::string> buildCommand() const;
    
    /**
     * @brief Generates the complete command as a string (for debug)
     * @return Complete FFmpeg command
     */
    std::string getCommandString() const;
    
    // ========================================================================
    // EXECUTION
    // ========================================================================
    
    /**
     * @brief Executes the encoding with the current configuration
     * @return true if encoding succeeded, false otherwise
     */
    bool execute() override;
    
    /**
     * @brief Validates the configuration before execution
     * @return true if the configuration is valid
     * @throw std::runtime_error if the configuration is invalid
     */
    bool validate() const;

private:
    std::string input_path_{};
    std::string output_path_{};
    Encode::EncodeConfig config_{};
    
    // ========================================================================
    // PRIVATE COMMAND CONSTRUCTION METHODS
    // ========================================================================
    
    void addInputArgs(std::vector<std::string>& args) const;
    void addVideoCodecArgs(std::vector<std::string>& args) const;
    void addRateControlArgs(std::vector<std::string>& args) const;
    void addEncodingParams(std::vector<std::string>& args) const;
    void addPixelFormatArgs(std::vector<std::string>& args) const;
    void addColorSpaceArgs(std::vector<std::string>& args) const;
    void addHDRMetadata(std::vector<std::string>& args) const;
    void addAudioArgs(std::vector<std::string>& args) const;
    void addOutputArgs(std::vector<std::string>& args) const;
    
    // ========================================================================
    // CONVERSION HELPERS
    // ========================================================================
    
    std::string getEncoderName() const;
    std::string getPixelFormatString() const;
    std::string getColorRangeString() const;
    std::string getColorPrimariesString() const;
    std::string getTransferString() const;
    std::string getColorMatrixString() const;
};

} // namespace Jobs
} // namespace FFmpegMulti
