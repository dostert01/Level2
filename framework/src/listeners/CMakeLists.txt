cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(level2 VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(listenersbase STATIC
    networklistener.cpp
    generic_server.cpp
)

target_include_directories(listenersbase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(listenersbase PUBLIC
    pipelinefifo
    applicationcontext
)

if(MOSQUITTO_FOUND)
    add_subdirectory(mqttlistener)
else()
    message("INFO: Building without support for MQTT")
endif()
add_subdirectory(filelistener)
add_subdirectory(httplistener)
