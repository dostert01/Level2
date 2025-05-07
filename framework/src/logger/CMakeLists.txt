cmake_minimum_required(VERSION 3.10)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Specify the library
add_library(logger SHARED
    logger.cpp 
    timeproviders.cpp 
    loggingdestinations.cpp
    loglevels.cpp
)

# Specify the include directories for the library
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(logger PRIVATE BUILDING_DLL)
