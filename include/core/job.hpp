#pragma once

namespace FFmpegMulti {
namespace Core {

/**
 * @brief Base interface for all jobs
 */
class Job {
public:
    virtual ~Job() = default;

    /**
     * @brief Executes the job
     * @return true if the job succeeded, false otherwise
     */
    virtual bool execute() = 0;
};

} // namespace Core
} // namespace FFmpegMulti
