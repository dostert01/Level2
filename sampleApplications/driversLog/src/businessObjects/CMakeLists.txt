cmake_minimum_required(VERSION 3.10)

include(CMakePrintHelpers)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the library
add_library(businessObjects STATIC 
    car.cpp)
target_link_libraries(businessObjects pipeline pipelineapi common dbinterface)


# Specify the include directories for the library
target_include_directories(businessObjects PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/../../framework/src/pipeline/api/)
target_compile_definitions(businessObjects PRIVATE BUILDING_DLL)
