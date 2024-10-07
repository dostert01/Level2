include_directories(../_deps/json-src/include/)

# Tests hinzufügen
add_executable(test_secondTake
    test_secondTake.cpp
    test_logger.cpp
    test_timeproviders.cpp
    test_loggingdestinations.cpp
    test_pipeline.cpp
    test_pipelineprocessor.cpp
    $<TARGET_OBJECTS:pipeline>)

# Include the necessary directories
target_include_directories(test_secondTake PRIVATE
    ${CMAKE_SOURCE_DIR}/framework/src/logger
    ${CMAKE_SOURCE_DIR}/framework/src/pipeline
    ${CMAKE_SOURCE_DIR}/framework/src/pipeline/api
    ${CMAKE_SOURCE_DIR}/_deps/json-src/single_include/
)

# GTest und GTest Main hinzufügen
target_link_libraries(test_secondTake
    pipelineapi
    gtest
    gtest_main
    sndtakelogger
    dl)

# Tests registrieren
add_test(NAME test_secondTake COMMAND test_secondTake)