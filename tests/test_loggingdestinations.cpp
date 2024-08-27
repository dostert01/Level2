#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <fstream>
#include <filesystem>
#include "logger.h"

TEST(LoggingDestinations, DefaultIsStdOut) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(LoggingDestinations, CanRemoveAllDestinations) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
    logger.removeAllDestinations();
    EXPECT_EQ(0, logger.getCountOfLoggingDestinations());
}

TEST(LoggingDestinations, CanAddNewDestinations) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    EXPECT_EQ(0, logger.getCountOfLoggingDestinations());
    second_take::LoggingDestinationFactory factory = second_take::LoggingDestinationFactory();
    logger.addLoggingDestination(factory.createDestinationStdOut());
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
}

TEST(LoggingDestinations, CanLogToStdErr) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    second_take::LoggingDestinationFactory factory = second_take::LoggingDestinationFactory();
    logger.addLoggingDestination(factory.createDestinationStdErr());
    EXPECT_EQ(2, logger.getCountOfLoggingDestinations());
    testing::internal::CaptureStderr();
    logger.error("Hello Log");
    std::string output = testing::internal::GetCapturedStderr();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(LoggingDestinations, CanReplaceLoggingDestinations) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    EXPECT_EQ(2, logger.getCountOfLoggingDestinations());
    second_take::LoggingDestinationFactory factory = second_take::LoggingDestinationFactory();
    logger.setLoggingDestination(factory.createDestinationStdOut());
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
}

std::string readFirstLineFromFile(const std::string fileName) {
    std::ifstream inputFile(fileName);
    std::string line;
    std::getline(inputFile, line);
    return line;
}

TEST(LoggingDestinations, CanLogToAFile) {
    std::string fileName = "./testFile.log";
    second_take::Logger& logger = second_take::Logger::getInstance();
    second_take::LoggingDestinationFactory factory = second_take::LoggingDestinationFactory();
    logger.setLoggingDestination(factory.createDestinationFile(fileName));
    logger.error("Hello Log");
    std::string output = readFirstLineFromFile(fileName);    
    std::regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
    if(!std::filesystem::remove(fileName)) {
        std::cerr << "Waring: failed to remove test log file at: " << fileName << std::endl;
    }
}
