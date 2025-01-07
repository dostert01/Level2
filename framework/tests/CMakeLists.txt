include(CMakePrintHelpers)

find_package(SQLite3 REQUIRED)
cmake_print_variables(SQLite3_FOUND)
cmake_print_variables(SQLite3_INCLUDE_DIRS)
cmake_print_variables(SQLite3_LIBRARIES)
cmake_print_variables(SQLite3_VERSION)

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
    ${SQLite3_INCLUDE_DIRS}
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
    $<TARGET_OBJECTS:pipeline>)

add_executable(test_applicationcontext
    test_applicationcontext.cpp
    $<TARGET_OBJECTS:applicationcontext>)

add_executable(test_pipelineprocessor
    test_pipelineprocessor.cpp
    $<TARGET_OBJECTS:pipeline>)

add_executable(test_pipeline
    test_pipeline.cpp
    $<TARGET_OBJECTS:pipeline>)

add_executable(test_pipeline_payload_matching
    test_pipeline_payload_matching.cpp
    $<TARGET_OBJECTS:pipeline>)

add_executable(test_dbinterface
    test_dbinterface.cpp
    $<TARGET_OBJECTS:dbinterface>)
target_link_libraries(test_dbinterface
    ${SQLite3_LIBRARIES})

add_executable(test_mqttconnector
    test_mqttconnector.cpp
    $<TARGET_OBJECTS:pipeline>)

    # Register tests
add_test(NAME test_eventForge COMMAND test_eventForge)
set_tests_properties(test_eventForge PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_applicationcontext COMMAND test_applicationcontext)
set_tests_properties(test_applicationcontext PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipelineprocessor COMMAND test_pipelineprocessor)
set_tests_properties(test_pipelineprocessor PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipeline COMMAND test_pipeline)
set_tests_properties(test_pipeline PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_pipeline_payload_matching COMMAND test_pipeline_payload_matching)
set_tests_properties(test_pipeline_payload_matching PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_dbinterface COMMAND test_dbinterface)
set_tests_properties(test_dbinterface PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_test(NAME test_mqttconnector COMMAND test_mqttconnector)
set_tests_properties(test_mqttconnector PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")
