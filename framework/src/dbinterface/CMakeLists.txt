cmake_minimum_required(VERSION 3.10)

include(CMakePrintHelpers)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SQLite3 REQUIRED)
cmake_print_variables(SQLite3_FOUND)
cmake_print_variables(SQLite3_INCLUDE_DIRS)
cmake_print_variables(SQLite3_LIBRARIES)
cmake_print_variables(SQLite3_VERSION)

# Specify the library
add_library(dbinterface SHARED
    dbinterface.cpp)

target_link_libraries(dbinterface PRIVATE ${SQLite3_LIBRARIES} logger)

# Specify the include directories for the library
target_include_directories(dbinterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SQLite3_INCLUDE_DIRS})
target_compile_definitions(dbinterface PRIVATE BUILDING_DLL)
