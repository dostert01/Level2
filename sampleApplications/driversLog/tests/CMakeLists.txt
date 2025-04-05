cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(level2 VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(INCLUDE_DIR_GTEST ${CMAKE_BINARY_DIR}/../../../_deps/googletest/googletest/include/)
#set(INCLUDE_DIR_BUSINESS_OBJECTS ${CMAKE_BINARY_DIR}/src/businessObjects)
#set(INCLUDE_DIR_PIPELINE_API ${CMAKE_BINARY_DIR}/../../framework/src/pipeline/api/)
#set(LINK_DIR_THIS_PROJECT ${CMAKE_BINARY_DIR}/../lib)
#set(LINK_DIR_FRAMEWORK_PROJECT ${CMAKE_BINARY_DIR}/../../lib)

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_BINARY_DIR)
#cmake_print_variables(INCLUDE_DIR_GTEST)
#cmake_print_variables(INCLUDE_DIR_BUSINESS_OBJECTS)
#cmake_print_variables(INCLUDE_DIR_PIPELINE_API)
#cmake_print_variables(LINK_DIR_THIS_PROJECT)
#cmake_print_variables(LINK_DIR_FRAMEWORK_PROJECT)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib/)

# GTest und GTest Main hinzufügen
link_libraries(
    gtest
    gtest_main
    dl)

# Tests hinzufügen
add_executable(test_car
    test_car.cpp)

target_link_libraries(test_car gtest gtest_main dl logger dbinterface businessObjects)

# Tests registrieren
add_test(NAME test_car COMMAND test_car)
