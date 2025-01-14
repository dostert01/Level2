cmake_minimum_required(VERSION 3.10)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

cmake_print_variables(PIPELINE_API_INCLUDE_DIR)

add_subdirectory(api)

# Specify the library
add_library(pipeline SHARED
    pipeline.cpp
    pipelineprocessor.cpp
    matchable.cpp)

add_library(fillerpipe SHARED
    pipelinefifo.cpp
    fillerpipe.cpp)

target_include_directories(pipeline PRIVATE api)
target_include_directories(fillerpipe PRIVATE api)

target_link_libraries(pipeline PRIVATE logger nlohmann_json::nlohmann_json dl)
target_link_libraries(fillerpipe PRIVATE logger)

# Specify the include directories for the library
target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pipeline PRIVATE BUILDING_DLL)
