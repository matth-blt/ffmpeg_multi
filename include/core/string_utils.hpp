#pragma once

#include <string>
#include <algorithm>

namespace StringUtils {

/**
 * @brief Removes quotes from the beginning and end of a string
 * @param str The string to clean
 * @return The string without quotes
 */
inline std::string removeQuotes(const std::string& str) {
    std::string result = str;

    if (!result.empty() && (result.front() == '"' || result.front() == '\'')) {
        result.erase(0, 1);
    }

    if (!result.empty() && (result.back() == '"' || result.back() == '\'')) {
        result.pop_back();
    }
    
    return result;
}

/**
 * @brief Removes spaces from the beginning and end of a string
 * @param str The string to clean
 * @return The string without spaces
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
 * @brief Cleans a string (removes quotes and spaces)
 * @param str The string to clean
 * @return The cleaned string
 */
inline std::string clean(const std::string& str) {
    return removeQuotes(trim(str));
}

} // namespace StringUtils
