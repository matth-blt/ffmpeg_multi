#include "../../include/core/input.hpp"
#include "../../include/core/colors.hpp"
#include "../../include/core/string_utils.hpp"
#include <iostream>
#include <limits>

namespace FFmpegMulti {

    // Helper pour vérifier la commande de retour
    bool isBackCommand(const std::string& str) {
        return str == ":q" || str == ":Q";
    }

    std::string Input::getString(const std::string& prompt, const std::string& hint, bool allowEmpty) {
        while (true) {
            std::cout << Colors::PEACH << "> " << prompt;
            if (!hint.empty()) {
                std::cout << " " << Colors::SUBTEXT << hint;
            }
            std::cout << " : " << Colors::RESET;

            std::string line;
            std::getline(std::cin, line);

            // Vérification commande retour
            if (isBackCommand(line)) {
                throw BackException();
            }

            // Nettoyage
            std::string cleaned = StringUtils::clean(line);

            if (cleaned.empty() && !allowEmpty) {
                continue;
            }

            return cleaned;
        }
    }

    int Input::getInt(const std::string& prompt, const std::string& hint) {
        while (true) {
            std::string str = getString(prompt, hint);
            try {
                return std::stoi(str);
            } catch (...) {
                std::cout << Colors::RED << "Veuillez entrer un nombre valide." << Colors::RESET << std::endl;
            }
        }
    }

    int Input::getIntRange(const std::string& prompt, int min, int max, const std::string& hint) {
        while (true) {
            int val = getInt(prompt, hint);
            if (val >= min && val <= max) {
                return val;
            }
            std::cout << Colors::RED << "Veuillez entrer un nombre entre " << min << " et " << max << "." << Colors::RESET << std::endl;
        }
    }

    float Input::getFloat(const std::string& prompt, const std::string& hint) {
        while (true) {
            std::string str = getString(prompt, hint);
            try {
                return std::stof(str);
            } catch (...) {
                std::cout << Colors::RED << "Veuillez entrer un nombre décimal valide." << Colors::RESET << std::endl;
            }
        }
    }

    bool Input::getConfirm(const std::string& prompt) {
        while (true) {
            std::cout << Colors::PEACH << "? " << prompt << " (o/n) : " << Colors::RESET;
            std::string line;
            std::getline(std::cin, line);

            if (isBackCommand(line)) throw BackException();

            if (line == "o" || line == "O") return true;
            if (line == "n" || line == "N") return false;
        }
    }
    
    void Input::wait() {
        std::cout << Colors::DIM << "Appuyez sur Entrée pour continuer..." << Colors::RESET;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}