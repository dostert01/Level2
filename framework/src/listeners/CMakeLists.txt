cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(eventForge VERSION 1.0)

# Festlegen des Standards für C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(listeners STATIC
    networklistener.cpp
    generic_server.cpp
)

target_include_directories(listeners PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(listeners PUBLIC
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
