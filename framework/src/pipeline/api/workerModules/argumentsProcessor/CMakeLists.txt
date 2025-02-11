cmake_minimum_required(VERSION 3.10)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Specify the library
add_library(argumentsProcessor SHARED
argumentsProcessor.cpp)
target_link_libraries(argumentsProcessor pipelineapi)

# Specify the include directories for the library
target_include_directories(argumentsProcessor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PIPELINE_API_INCLUDE_DIR})
target_compile_definitions(argumentsProcessor PRIVATE BUILDING_DLL)
