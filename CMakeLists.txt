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
    src/core/command.cpp
    src/core/ffmpeg_process.cpp
    src/core/job.cpp
    src/core/logger.cpp
    src/core/path_utils.cpp
)

# CLI
set(CLI_SOURCES
    src/cli/cli_parser.cpp
    src/cli/commands.cpp
)

# Jobs
set(JOBS_SOURCES
    src/jobs/concat.cpp
    src/jobs/encode.cpp
    src/jobs/encode_builder.cpp
    src/jobs/svt_av1_essential.cpp
    src/jobs/extract_frames.cpp
    src/jobs/probe.cpp
    src/jobs/reencode.cpp
    src/jobs/thumbnails.cpp
)

# Pipelines
set(PIPELINE_SOURCES
    src/pipelines/pipeline_base.cpp
    src/pipelines/pipeline_batch.cpp
    src/pipelines/pipeline_custom.cpp
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
# Tests (optionnel)
# ============================================================================
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    
    add_executable(test_encode tests/test_encode.cpp src/jobs/encode.cpp src/jobs/encode_builder.cpp)
    target_include_directories(test_encode PRIVATE ${HEADERS_DIR})
    add_test(NAME test_encode COMMAND test_encode)
    
    add_executable(test_extract_frames tests/test_extract_frames.cpp src/jobs/extract_frames.cpp)
    target_include_directories(test_extract_frames PRIVATE ${HEADERS_DIR})
    add_test(NAME test_extract_frames COMMAND test_extract_frames)
    
    add_executable(test_pipeline_batch tests/test_pipeline_batch.cpp src/pipelines/pipeline_batch.cpp)
    target_include_directories(test_pipeline_batch PRIVATE ${HEADERS_DIR})
    add_test(NAME test_pipeline_batch COMMAND test_pipeline_batch)
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
