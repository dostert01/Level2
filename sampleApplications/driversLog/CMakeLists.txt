cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(driversLog VERSION 1.0)

execute_process(COMMAND ./getDependencies.sh)

include(CMakePrintHelpers)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

cmake_print_variables(CMAKE_BINARY_DIR)
cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
cmake_print_variables(CMAKE_BUILD_TYPE)
cmake_print_variables(CMAKE_CURRENT_SOURCE_DIR)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(logger IMPORTED SHARED)
set_target_properties(logger PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/lib/liblogger.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/src/logger/
)

add_library(common IMPORTED SHARED)
set_target_properties(common PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/lib/libcommon.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/src/common/
)

add_library(pipeline IMPORTED SHARED)
set_target_properties(pipeline PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/lib/libpipeline.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/src/pipeline/
)

add_library(dbinterface IMPORTED SHARED)
set_target_properties(dbinterface PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/lib/libdbinterface.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/src/dbinterface/
)

add_library(pipelineapi IMPORTED SHARED)
set_target_properties(pipelineapi PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/lib/pipelineapi.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/src/pipeline/api/
)

# add subdirs
add_subdirectory(_deps/googletest)
add_subdirectory(src)

# Tests hinzufügen
enable_testing()
add_subdirectory(tests)
