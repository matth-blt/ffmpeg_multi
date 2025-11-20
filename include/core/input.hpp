#pragma once
#include <string>
#include <stdexcept>
#include <optional>

namespace FFmpegMulti {

    class BackException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Back to menu requested";
        }
    };

    class Input {
    public:
        // Request a string
        static std::string getString(const std::string& prompt, const std::string& hint = "", bool allowEmpty = false);
        
        // Request an integer
        static int getInt(const std::string& prompt, const std::string& hint = "");
        
        // Request an integer within a range
        static int getIntRange(const std::string& prompt, int min, int max, const std::string& hint = "");
        
        // Request a float
        static float getFloat(const std::string& prompt, const std::string& hint = "");
        
        // Request confirmation (y/n)
        static bool getConfirm(const std::string& prompt);
        
        // Pause to read
        static void wait();
    };
}