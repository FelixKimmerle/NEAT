cmake_minimum_required(VERSION 3.24)
project(NEAT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# SFML is downloaded and built together with the project, so nothing has to be installed.
include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(neat
    main.cpp
    Gen.cpp
    Genome.cpp
    InnovationManager.cpp
    NN.cpp
    Population.cpp
    Tasks.cpp
    NetworkView.cpp
    DecisionPlot.cpp
    ThickLine.cpp
    Logger/Logger.cpp)
target_include_directories(neat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(neat PRIVATE SFML::Graphics)
# Lets the binary find the font no matter where it is started from.
target_compile_definitions(neat PRIVATE NEAT_FONT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/arial.ttf")

if(MSVC)
    target_compile_options(neat PRIVATE /W4)
else()
    target_compile_options(neat PRIVATE -Wall -Wextra -Wpedantic)
endif()
