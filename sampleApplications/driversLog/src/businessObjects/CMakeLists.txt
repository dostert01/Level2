cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(driversLog VERSION 1.0)

include(CMakePrintHelpers)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

cmake_print_variables(CMAKE_BINARY_DIR)
cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_BUILD_TYPE)

# Specify the library
add_library(businessObjects STATIC 
    car.cpp)
target_link_libraries(businessObjects pipeline pipelineapi)


# Specify the include directories for the library
target_include_directories(businessObjects PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/../../framework/src/pipeline/api/)
target_compile_definitions(businessObjects PRIVATE BUILDING_DLL)
