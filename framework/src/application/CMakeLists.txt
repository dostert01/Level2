cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

cmake_print_variables(PIPELINE_API_INCLUDE_DIR)
cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

# Specify the library
add_library(application SHARED
    application.cpp
)

target_link_libraries(application PUBLIC
    applicationcontext
    logger
)

# Specify the include directories for the library
target_include_directories(application PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(application PRIVATE BUILDING_DLL)