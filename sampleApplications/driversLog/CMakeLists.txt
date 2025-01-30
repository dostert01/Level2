cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(driversLog VERSION 1.0)

execute_process(COMMAND ./getDependencies.sh)

include(CMakePrintHelpers)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

cmake_print_variables(CMAKE_BINARY_DIR)
cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_BUILD_TYPE)
cmake_print_variables(CMAKE_CURRENT_SOURCE_DIR)
cmake_print_variables(PROJECT_ROOT_DIR)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add subdirs
add_subdirectory(src)

# Tests hinzufügen
enable_testing()
add_subdirectory(tests)
