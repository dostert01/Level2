#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <fstream>
#include <filesystem>
#include <vector>
#include <unistd.h>

#include "logger.h"

#define ONE_SECOND 1000000

using namespace level2;
using namespace std;

TEST(LoggingDestinations, DefaultIsStdOut) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(LoggingDestinations, CanRemoveAllDestinations) {
    Logger& logger = Logger::getInstance();
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
    logger.removeAllDestinations();
    EXPECT_EQ(0, logger.getCountOfLoggingDestinations());
}

TEST(LoggingDestinations, CanAddNewDestinations) {
    Logger& logger = Logger::getInstance();
    EXPECT_EQ(0, logger.getCountOfLoggingDestinations());
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    logger.addLoggingDestination(factory.createDestinationStdOut());
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
}

TEST(LoggingDestinations, CanLogToStdErr) {
    Logger& logger = Logger::getInstance();
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    logger.addLoggingDestination(factory.createDestinationStdErr());
    EXPECT_EQ(2, logger.getCountOfLoggingDestinations());
    testing::internal::CaptureStderr();
    logger.error("Hello Log");
    string output = testing::internal::GetCapturedStderr();
    cout << "output: " << output << endl;
    regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(LoggingDestinations, CanReplaceLoggingDestinations) {
    Logger& logger = Logger::getInstance();
    EXPECT_EQ(2, logger.getCountOfLoggingDestinations());
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    logger.setLoggingDestination(factory.createDestinationStdOut());
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
}

string readFirstLineFromFile(const string fileName) {
    ifstream inputFile(fileName);
    string line;
    getline(inputFile, line);
    return line;
}

TEST(LoggingDestinations, CanLogToAFile) {
    string fileName = "./testFile.log";
    Logger& logger = Logger::getInstance();
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    logger.setLoggingDestination(factory.createDestinationFile(fileName));
    logger.error("Hello Log");
    string output = readFirstLineFromFile(fileName);    
    regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
    if(!filesystem::remove(fileName)) {
        cerr << "Waring: failed to remove test log file at: " << fileName << endl;
    }
}

void readSyslog(vector<string>& syslogBuffered) {
    ifstream syslogFile("/var/log/syslog");
    if(!syslogFile.is_open()){
        syslogFile.open("/var/log/messages");
    }
    if(syslogFile.is_open()){
        string currentLine;
        while(getline(syslogFile, currentLine)){
            if(!currentLine.empty()){
                syslogBuffered.push_back(currentLine);
            }
        }
        syslogFile.close();
    }
}

bool stringPresentInSyslog(const string logString) {
    vector<string> syslog;
    usleep(ONE_SECOND);
    readSyslog(syslog);
    regex regex(logString);
    bool found = false;
    for(string currentLine : syslog){
        found = regex_search(currentLine, regex);
        if(found) {
            cout << "line found in syslog: " << currentLine << endl;
            break;
        }
    }
    return found;
}

TEST(LoggingDestinations, CanLogToSyslog) {
    string fileName = "./testFile.log";
    Logger& logger = Logger::getInstance();
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    logger.setLoggingDestination(factory.createDestinationSyslog("testapp"));
    DefaultTimeProvider timeProvider;
    string logString = "Hello Log at " + timeProvider.getTimeStampOfNow();
    logger.error(logString);
    EXPECT_TRUE(stringPresentInSyslog(logString));
}

TEST(LoggingDestinationByType, returnsEmptyOptionalForEmptyParamsMap) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    std::map<std::string, std::string> params;
    auto destination = factory.createDestinationFromParamsMap(params);
    EXPECT_FALSE(destination.has_value());
}

TEST(LoggingDestinationByType, returnsEmptyOptionalForUnknownType) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    std::map<std::string, std::string> params;
    params["type"] = "unknown type";
    auto destination = factory.createDestinationFromParamsMap(params);
    EXPECT_FALSE(destination.has_value());
}

TEST(LoggingDestinationByType, canCreateLoggingDestinationByType01) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    std::map<std::string, std::string> params;
    params["type"] = "stdout";
    auto destination = factory.createDestinationFromParamsMap(params);
    EXPECT_TRUE(destination.has_value());
    EXPECT_FALSE(dynamic_cast<LoggingDestinationStdOut*>(destination.value().get()) == NULL);
}

TEST(LoggingDestinationByType, canCreateLoggingDestinationByType02) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    std::map<std::string, std::string> params;
    params["type"] = "stderr";
    auto destination = factory.createDestinationFromParamsMap(params);
    EXPECT_TRUE(destination.has_value());
    EXPECT_FALSE(dynamic_cast<LoggingDestinationStdErr*>(destination.value().get()) == NULL);
}

TEST(LoggingDestinationByType, canCreateLoggingDestinationByType03) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    std::map<std::string, std::string> params;
    params["type"] = "file";
    params["fileName"] = "someLogFile";
    auto destination = factory.createDestinationFromParamsMap(params);
    EXPECT_TRUE(destination.has_value());
    EXPECT_FALSE(dynamic_cast<LoggingDestinationFile*>(destination.value().get()) == NULL);
    EXPECT_EQ("someLogFile", dynamic_cast<LoggingDestinationFile*>(destination.value().get())->getLogFileName());
}

TEST(LoggingDestinationByType, canCreateLoggingDestinationByType04) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    std::map<std::string, std::string> params;
    params["type"] = "syslog";
    params["applicationName"] = "testApp";
    auto destination = factory.createDestinationFromParamsMap(params);
    EXPECT_TRUE(destination.has_value());
    EXPECT_FALSE(dynamic_cast<LoggingDestinationSyslog*>(destination.value().get()) == NULL);
    EXPECT_EQ("testApp", dynamic_cast<LoggingDestinationSyslog*>(destination.value().get())->getSyslogIdent());
}
