cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(secondTake VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)


# Hinzufügen der ausführbaren Datei
add_executable(secondTake main.cpp)

# Include the necessary directories
target_include_directories(secondTake PRIVATE
    ${CMAKE_SOURCE_DIR}/src/logger
)

target_link_libraries(secondTake PRIVATE sndtakelogger)
