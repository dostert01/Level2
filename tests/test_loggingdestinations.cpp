#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <fstream>
#include <filesystem>
#include <vector>
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

void readSyslog(std::vector<std::string>& syslogBuffered) {
    std::ifstream syslogFile("/var/log/syslog");
    if(!syslogFile.is_open()){
        syslogFile.open("/var/log/messages");
    }
    if(syslogFile.is_open()){
        std::string currentLine;
        while(std::getline(syslogFile, currentLine)){
            if(!currentLine.empty()){
                syslogBuffered.push_back(currentLine);
            }
        }
        syslogFile.close();
    }
}

bool stringPresentInSyslog(const std::string logString) {
    std::vector<std::string> syslog;
    readSyslog(syslog);
    std::regex regex(logString);
    bool found = false;
    for(std::string currentLine : syslog){
        found = std::regex_search(currentLine, regex);
        if(found) {
            std::cout << "line found in syslog: " << currentLine << std::endl;
            break;
        }
    }
    return found;
}

TEST(LoggingDestinations, CanLogToSyslog) {
    std::string fileName = "./testFile.log";
    second_take::Logger& logger = second_take::Logger::getInstance();
    second_take::LoggingDestinationFactory factory = second_take::LoggingDestinationFactory();
    logger.setLoggingDestination(factory.createDestinationSyslog("testapp"));
    second_take::DefaultTimeProvider timeProvider;
    std::string logString = "Hello Log at " + timeProvider.getTimeStampOfNow();
    logger.error(logString);
    EXPECT_TRUE(stringPresentInSyslog(logString));
}
