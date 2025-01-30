cmake_minimum_required(VERSION 3.10)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

cmake_print_variables(PIPELINE_API_INCLUDE_DIR)

add_subdirectory(api)

# Specify the library
add_library(pipeline SHARED
    pipeline.cpp
    pipelineprocessor.cpp
)

add_library(pipelinefifo SHARED
    pipelinefifo.cpp
)

target_include_directories(pipeline PUBLIC api ${CMAKE_CURRENT_SOURCE_DIR} ${COMMON_INCLUDE_DIR})
target_include_directories(pipelinefifo PUBLIC api ${COMMON_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pipeline PUBLIC logger common pipelinefifo nlohmann_json::nlohmann_json dl)
target_link_libraries(pipelinefifo PUBLIC logger nlohmann_json::nlohmann_json dl)

target_compile_definitions(pipeline PRIVATE BUILDING_DLL)
