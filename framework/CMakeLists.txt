cmake_minimum_required(VERSION 3.10)

include(CMakePrintHelpers)

# Projektname und Version
project(eventForge VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
cmake_print_variables(CMAKE_MODULE_PATH)
find_package(Mosquito REQUIRED)
cmake_print_variables(MOSQUITTO_LIBRARY)
cmake_print_variables(MOSQUITTO_INCLUDE_DIR)
cmake_print_variables(MOSQUITTO_FOUND)

if(NOT MOSQUITTO_FOUND)
    message("WARNING: mosquitto not found! Building without support for MQTT!")
endif()

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SRC_DIR $(CMAKE_CURRENT_SOURCE_DIR/src))

# add subdirs
add_subdirectory(src)

# Tests hinzufügen
enable_testing()
add_subdirectory(tests)
