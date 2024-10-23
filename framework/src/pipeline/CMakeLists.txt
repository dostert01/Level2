cmake_minimum_required(VERSION 3.10)

include(FetchContent)
include(CMakePrintHelpers)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(PIPELINE_API_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/api)
cmake_print_variables(PIPELINE_API_INCLUDE_DIR)

add_subdirectory(api)

# Specify the library
add_library(pipeline OBJECT 
    pipeline.cpp
    pipelineprocessor.cpp
    selectable.cpp)

target_include_directories(pipeline PRIVATE api)

target_link_libraries(pipeline PRIVATE nlohmann_json::nlohmann_json dl)

# Specify the include directories for the library
target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pipeline PRIVATE BUILDING_DLL)
