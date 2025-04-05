cmake_minimum_required(VERSION 3.10)

include(CMakePrintHelpers)

# Projektname und Version
project(level2 VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(PIPELINE_API_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pipeline/api)
cmake_print_variables(PIPELINE_API_INCLUDE_DIR)
set(LOGGER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/logger)
cmake_print_variables(LOGGER_INCLUDE_DIR)

# add subdirs
add_subdirectory(logger)
add_subdirectory(pipeline)
add_subdirectory(dbinterface)
add_subdirectory(applicationcontext)
add_subdirectory(connectors)
add_subdirectory(listeners)
add_subdirectory(common)

# Hinzufügen der ausführbaren Datei
add_executable(level2 main.cpp)

# Include the necessary directories
target_include_directories(level2 PRIVATE
    ${CMAKE_SOURCE_DIR}/src/logger
    ${CMAKE_SOURCE_DIR}/src/pipeline
)

target_link_libraries(level2 PRIVATE logger)
