cmake_minimum_required(VERSION 3.25)

project(snake LANGUAGES CXX)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.deps" CACHE PATH "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_compile_options(-Wall -Wextra -Wpedantic)

# Download font at configure time
include(cmake/DownloadFont.cmake)

# Fetch SFML 3.0.2
include(FetchContent)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SFML)

add_executable(snake
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/Board.cpp
    src/Renderer.cpp
    src/Settings.cpp
    src/HighScore.cpp
)

target_compile_features(snake PRIVATE cxx_std_23)
target_link_libraries(snake PRIVATE SFML::Graphics SFML::Window SFML::System)

# Copy assets to build output directory
add_custom_command(TARGET snake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:snake>/../assets"
    COMMENT "Copying assets to build directory"
)

# Unit tests with Catch2
enable_testing()
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

add_executable(snake_tests
    tests/test_snake.cpp
    tests/test_board.cpp
    src/Snake.cpp
    src/Board.cpp
    src/Settings.cpp
    src/HighScore.cpp
)
target_compile_features(snake_tests PRIVATE cxx_std_23)
target_link_libraries(snake_tests PRIVATE Catch2::Catch2WithMain SFML::Graphics SFML::System)

include(CTest)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(snake_tests)
