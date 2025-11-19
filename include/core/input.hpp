#pragma once
#include <string>
#include <stdexcept>
#include <optional>

namespace FFmpegMulti {

    class BackException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Retour au menu demandé";
        }
    };

    class Input {
    public:
        // Demande une chaîne de caractères
        static std::string getString(const std::string& prompt, const std::string& hint = "", bool allowEmpty = false);
        
        // Demande un entier
        static int getInt(const std::string& prompt, const std::string& hint = "");
        
        // Demande un entier dans une plage
        static int getIntRange(const std::string& prompt, int min, int max, const std::string& hint = "");
        
        // Demande un float
        static float getFloat(const std::string& prompt, const std::string& hint = "");
        
        // Demande une confirmation (o/n)
        static bool getConfirm(const std::string& prompt);
        
        // Pause pour lire
        static void wait();
    };
}