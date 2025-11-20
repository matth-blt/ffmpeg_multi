cmake_minimum_required(VERSION 3.15)
project(ffmpeg_multi VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# Configuration du compilateur
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Sources
# ============================================================================

# Core
set(CORE_SOURCES
    src/core/app.cpp
    src/core/ffmpeg_process.cpp
    src/core/path_utils.cpp
    src/core/input.cpp
)

# Jobs
set(JOBS_SOURCES
    src/jobs/codec_utils.cpp
    src/jobs/concat.cpp
    src/jobs/encode.cpp
    src/jobs/encode_builder.cpp
    src/jobs/reencode.cpp
    src/jobs/reencode_builder.cpp
    src/jobs/svt_av1_essential.cpp
    src/jobs/extract_frames.cpp
    src/jobs/extract_frames_builder.cpp
    src/jobs/probe.cpp
    src/jobs/thumbnails.cpp
    src/jobs/thumbnails_builder.cpp
)

# Main
set(MAIN_SOURCE
    src/main.cpp
)

# ============================================================================
# Headers
# ============================================================================
set(HEADERS_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${HEADERS_DIR})

# ============================================================================
# Executable
# ============================================================================
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCE}
    ${CORE_SOURCES}
    ${CLI_SOURCES}
    ${JOBS_SOURCES}
    ${PIPELINE_SOURCES}
)

# ============================================================================
# Options de compilation
# ============================================================================
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ============================================================================
# Installation
# ============================================================================
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# ============================================================================
# Messages de statut
# ============================================================================
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
