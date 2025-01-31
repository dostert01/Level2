cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

cmake_print_variables(PIPELINE_API_INCLUDE_DIR)
cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

# Specify the library
add_library(httplistener SHARED
    httplistener.cpp
    httpexception.cpp
    httprequest.cpp
    httpresponse.cpp
    http11.cpp
    headerfieldowner.cpp
)

target_link_libraries(httplistener PUBLIC
    pipelineapi
    pipeline
    applicationcontext
    listeners
    nlohmann_json::nlohmann_json dl
)

# Specify the include directories for the library
target_include_directories(httplistener PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PIPELINE_API_INCLUDE_DIR}
    ${LOGGER_INCLUDE_DIR}
    ${LISTENERS_DIR}
)
target_compile_definitions(httplistener PRIVATE BUILDING_DLL)
