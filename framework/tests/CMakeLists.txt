include(CMakePrintHelpers)

find_package(SQLite3 REQUIRED)
cmake_print_variables(SQLite3_FOUND)
cmake_print_variables(SQLite3_INCLUDE_DIRS)
cmake_print_variables(SQLite3_LIBRARIES)
cmake_print_variables(SQLite3_VERSION)

cmake_print_variables(MOSQUITTO_LIBRARY)
cmake_print_variables(COMMON_INCLUDE_DIR)

# Include the necessary directories
include_directories(
    ${CMAKE_SOURCE_DIR}/framework/src/logger
    ${CMAKE_SOURCE_DIR}/framework/src/pipeline
    ${CMAKE_SOURCE_DIR}/framework/src/pipeline/api
    ${CMAKE_SOURCE_DIR}/framework/src/dbinterface
    ${CMAKE_SOURCE_DIR}/framework/src/applicationcontext
    ${CMAKE_SOURCE_DIR}/framework/src/connectors
    ${CMAKE_SOURCE_DIR}/_deps/json-src/single_include/
    ${CMAKE_SOURCE_DIR}/framework/src/pipeline/api/samplelibs/binaryDataProcessor/
    ${CMAKE_SOURCE_DIR}/_deps/googletest/googletest/include/
    ${CMAKE_SOURCE_DIR}/_deps/json-src/include/
    ${MQTT_CONNECTOR_DIR}
    ${MQTT_LISTENER_DIR}
    ${FILE_LISTENER_DIR}
    ${SQLite3_INCLUDE_DIRS}
    ${LISTENERS_DIR}
    ${COMMON_INCLUDE_DIR}
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib/
)

# GTest und GTest Main hinzufügen
link_libraries(
    pipelineapi
    gtest
    gtest_main
    logger
    dl)

# set variables
set(TEST_FILES_DIR "${CMAKE_SOURCE_DIR}/framework/tests/testdata/" CACHE STRING "Directory where test data is expected by the tests")

# Adding tests
add_executable(test_eventForge
    test_eventForge.cpp
    test_logger.cpp
    test_timeproviders.cpp
    test_loggingdestinations.cpp
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>)

add_executable(test_common
    test_common.cpp
    $<TARGET_OBJECTS:common>)

add_executable(test_applicationcontext
    test_applicationcontext.cpp
    $<TARGET_OBJECTS:applicationcontext>)

add_executable(test_apistructs
    test_apistructs.cpp
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>)

    add_executable(test_pipelineprocessor
    test_pipelineprocessor.cpp
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>)

add_executable(test_pipeline
    test_pipeline.cpp
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>)

add_executable(test_pipeline_payload_matching
    test_pipeline_payload_matching.cpp
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>)

add_executable(test_pipelinefifo
    test_pipelinefifo.cpp
    $<TARGET_OBJECTS:pipelinefifo>
    $<TARGET_OBJECTS:pipeline>)

add_executable(test_dbinterface
    test_dbinterface.cpp
    $<TARGET_OBJECTS:dbinterface>)
target_link_libraries(test_dbinterface
    ${SQLite3_LIBRARIES})

add_executable(test_fileconnector
    test_fileconnector.cpp
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>)

add_executable(test_filelistener
    test_filelistener.cpp
    $<TARGET_OBJECTS:filelistener>
    $<TARGET_OBJECTS:pipeline>
    $<TARGET_OBJECTS:pipelinefifo>
    $<TARGET_OBJECTS:applicationcontext>)

if(MOSQUITTO_FOUND)
    add_executable(test_mqttconnector
        test_mqttconnector.cpp
        $<TARGET_OBJECTS:pipeline>
        $<TARGET_OBJECTS:mqttconnector>
    )
    target_link_libraries(test_mqttconnector
        ${MOSQUITTO_LIBRARY}
        mosquittowrapper
    )

    add_executable(test_mqttlistener
        test_mqttlistener.cpp
        $<TARGET_OBJECTS:pipeline>
        $<TARGET_OBJECTS:mqttlistener>
        $<TARGET_OBJECTS:applicationcontext>
    )
    target_link_libraries(test_mqttlistener
        ${MOSQUITTO_LIBRARY}
        mosquittowrapper
    )
else()
    message("INFO: Building without support for MQTT")
endif()

# Register tests
add_test(NAME test_eventForge COMMAND test_eventForge)
set_tests_properties(test_eventForge PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_applicationcontext COMMAND test_applicationcontext)
set_tests_properties(test_applicationcontext PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_common COMMAND test_common)
set_tests_properties(test_common PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_apistructs COMMAND test_apistructs)
set_tests_properties(test_apistructs PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipelineprocessor COMMAND test_pipelineprocessor)
set_tests_properties(test_pipelineprocessor PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipeline COMMAND test_pipeline)
set_tests_properties(test_pipeline PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipeline_payload_matching COMMAND test_pipeline_payload_matching)
set_tests_properties(test_pipeline_payload_matching PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipelinefifo COMMAND test_pipelinefifo)
set_tests_properties(test_pipelinefifo PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_dbinterface COMMAND test_dbinterface)
set_tests_properties(test_dbinterface PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_fileconnector COMMAND test_fileconnector)
set_tests_properties(test_fileconnector PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_filelistener COMMAND test_filelistener)
set_tests_properties(test_filelistener PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

if(MOSQUITTO_FOUND)
    add_test(NAME test_mqttconnector COMMAND test_mqttconnector)
    set_tests_properties(test_mqttconnector PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

    add_test(NAME test_mqttlistener COMMAND test_mqttlistener)
    set_tests_properties(test_mqttlistener PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")
endif()
