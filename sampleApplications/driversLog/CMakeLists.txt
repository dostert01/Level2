cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(driversLog VERSION 1.0)

execute_process(COMMAND ./getDependencies.sh)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add subdirs
add_subdirectory(_deps/googletest)
add_subdirectory(src/businessObjects)

# Tests hinzufügen
enable_testing()
add_subdirectory(tests)
