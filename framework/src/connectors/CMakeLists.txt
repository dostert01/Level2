cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(level2 VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(fileconnector)
if(MOSQUITTO_FOUND)
    add_subdirectory(mqttconnector)
else()
    message("INFO: Building without support for MQTT")
endif()

