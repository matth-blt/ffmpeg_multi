#pragma once
#include <string>
#include <vector>
#include "../core/job.hpp"

namespace FFmpegMulti {
namespace Jobs {

class ConcatJob : public Core::Job {
public:
    ConcatJob(const std::vector<std::string>& inputs, const std::string& output);
    bool execute() override;

private:
    std::vector<std::string> m_inputs;
    std::string m_output;
    
    std::string getMkvMergePath() const;
};

class ConcatBuilder {
public:
    ConcatBuilder& addInput(const std::string& input);
    ConcatBuilder& output(const std::string& output);
    ConcatJob build();

private:
    std::vector<std::string> m_inputs;
    std::string m_output;
};

} // namespace Jobs
} // namespace FFmpegMulti
