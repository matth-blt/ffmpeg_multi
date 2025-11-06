#pragma once

#include <string>
#include <algorithm>

namespace StringUtils {

/**
 * @brief Supprime les guillemets au début et à la fin d'une chaîne
 * @param str La chaîne à nettoyer
 * @return La chaîne sans guillemets
 */
inline std::string removeQuotes(const std::string& str) {
    std::string result = str;
    
    // Supprimer les guillemets au début
    if (!result.empty() && (result.front() == '"' || result.front() == '\'')) {
        result.erase(0, 1);
    }
    
    // Supprimer les guillemets à la fin
    if (!result.empty() && (result.back() == '"' || result.back() == '\'')) {
        result.pop_back();
    }
    
    return result;
}

/**
 * @brief Supprime les espaces au début et à la fin d'une chaîne
 * @param str La chaîne à nettoyer
 * @return La chaîne sans espaces
 */
inline std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }
    
    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    
    return std::string(start, end + 1);
}

/**
 * @brief Nettoie une chaîne (supprime guillemets et espaces)
 * @param str La chaîne à nettoyer
 * @return La chaîne nettoyée
 */
inline std::string clean(const std::string& str) {
    return removeQuotes(trim(str));
}

} // namespace StringUtils
