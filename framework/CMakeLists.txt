cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(eventForge VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add subdirs
add_subdirectory(src)
add_subdirectory(src/logger)
add_subdirectory(src/pipeline)
add_subdirectory(src/dbinterface)

# Tests hinzufügen
enable_testing()
add_subdirectory(tests)
