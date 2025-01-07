cmake_minimum_required(VERSION 3.10)

include(CMakePrintHelpers)

# Projektname und Version
project(eventForge VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(PIPELINE_API_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pipeline/api)
cmake_print_variables(PIPELINE_API_INCLUDE_DIR)

# add subdirs
add_subdirectory(logger)
add_subdirectory(pipeline)
add_subdirectory(dbinterface)
add_subdirectory(applicationcontext)
add_subdirectory(connectors)

# Hinzufügen der ausführbaren Datei
add_executable(eventForge main.cpp)

# Include the necessary directories
target_include_directories(eventForge PRIVATE
    ${CMAKE_SOURCE_DIR}/src/logger
    ${CMAKE_SOURCE_DIR}/src/pipeline
)

target_link_libraries(eventForge PRIVATE logger)
