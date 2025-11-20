#pragma once

namespace FFmpegMulti {
    namespace Colors {
        // Style codes
        inline const char* RESET = "\033[0m";
        inline const char* BOLD = "\033[1m";
        inline const char* DIM = "\033[2m";
        
        // Palette "Universal" (High compatibility)
        inline const char* LAVENDER = "\033[1;37m";    // Bold White (Main titles)
        inline const char* BLUE = "\033[36m";          // Cyan (Borders)
        inline const char* MAUVE = "\033[1;35m";       // Bold Magenta (IDs, Options)
        inline const char* TEXT = "\033[0m";           // Default (Standard text)
        inline const char* SUBTEXT = "\033[90m";       // Bright Black/Gray (Secondary infos)
        inline const char* GREEN = "\033[32m";         // Green (Success)
        inline const char* RED = "\033[31m";           // Red (Errors)
        inline const char* YELLOW = "\033[33m";        // Yellow (Warnings)
        inline const char* PEACH = "\033[1;33m";       // Bold Yellow (Prompts/Questions)
        inline const char* SAPPHIRE = "\033[1;36m";    // Bold Cyan (Subtitles)
        inline const char* TEAL = "\033[32m";          // Green (Input fields)
    }
}