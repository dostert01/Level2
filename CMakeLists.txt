cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(level2 VERSION 1.0)

execute_process(COMMAND ./getDependencies.sh)

include(FetchContent)
include(CMakePrintHelpers)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
# Credits go to Niels Lohmann for nlohmann/json

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

cmake_print_variables(PROJECT_ROOT_DIR)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add sub projects
add_subdirectory(_deps/googletest)
add_subdirectory(framework)

# Tests hinzufügen
enable_testing()
