cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

cmake_print_variables(PIPELINE_API_INCLUDE_DIR)
cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

# Specify the library
add_library(fileconnector SHARED
    fileconnector.cpp
    filewriter.cpp
)

target_link_libraries(fileconnector
    pipelineapi
    nlohmann_json::nlohmann_json
    dl
    applicationcontext
    pipeline
)

# Specify the include directories for the library
target_include_directories(fileconnector PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PIPELINE_API_INCLUDE_DIR}
    ${LOGGER_INCLUDE_DIR}
    ${APPLICATION_CONTEXT_INCLUDE_DIR}
)
target_compile_definitions(fileconnector PRIVATE BUILDING_DLL)
