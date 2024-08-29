cmake_minimum_required(VERSION 3.10)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Specify the library
add_library(pipeline OBJECT 
    pipeline.cpp)

target_link_libraries(pipeline PRIVATE nlohmann_json::nlohmann_json)

# Specify the include directories for the library
target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pipeline PRIVATE BUILDING_DLL)
