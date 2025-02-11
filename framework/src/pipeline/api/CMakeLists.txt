cmake_minimum_required(VERSION 3.10)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(./workerModules)

add_library(pipelineapi SHARED
    processingerror.cpp
    processingpayload.cpp
    pipelineprocessingdata.cpp
    pipelinestepinitdata.cpp
    binaryprocessingdata.cpp
    $<TARGET_OBJECTS:common>
)

target_include_directories(pipelineapi PUBLIC
    ${COMMON_INCLUDE_DIR}
)

target_link_libraries(pipelineapi PRIVATE 
    nlohmann_json::nlohmann_json
    dl
)

