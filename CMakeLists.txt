cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(eventForge VERSION 1.0)

execute_process(COMMAND ./getDependencies.sh)

include(FetchContent)
include(CMakePrintHelpers)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add sub projects
add_subdirectory(_deps/googletest)
add_subdirectory(framework)

# Tests hinzufügen
enable_testing()
