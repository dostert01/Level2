# Tests hinzufügen
add_executable(test_secondTake
    test_secondTake.cpp
    test_logger.cpp
    test_timeproviders.cpp
    test_loggingdestinations.cpp)

# Include the necessary directories
target_include_directories(test_secondTake PRIVATE
    ${CMAKE_SOURCE_DIR}/src/logger
)

# GTest und GTest Main hinzufügen
target_link_libraries(test_secondTake gtest gtest_main sndtakelogger)

# Tests registrieren
add_test(NAME test_secondTake COMMAND test_secondTake)