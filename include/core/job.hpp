#pragma once

namespace FFmpegMulti {
namespace Core {

/**
 * @brief Interface de base pour tous les jobs
 */
class Job {
public:
    virtual ~Job() = default;

    /**
     * @brief Exécute le job
     * @return true si le job a réussi, false sinon
     */
    virtual bool execute() = 0;
};

} // namespace Core
} // namespace FFmpegMulti
