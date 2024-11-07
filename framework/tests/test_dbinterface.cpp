#include <gtest/gtest.h>
#include "../logger/logger.h"
#include "dbinterface.h"

#include <filesystem>
#include <map>

using namespace second_take;
using namespace std;

#define TEST_DB_FILE_NAME "test.db"

namespace test_db_interface {
    string workingDir;
    string testFilesDir;

    void configureLogger() {
        Logger& logger = second_take::Logger::getInstance();
        logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }
}

void configureTest() {
    test_db_interface::configureLogger();
}

TEST(DBInterface, canOpenAndCloseSQLiteDb) {
    Database* db = new DatabaseSQLite();
    map<string, string> connectionParams;
    string dbFileName = TEST_DB_FILE_NAME;
    filesystem::remove(dbFileName);
    connectionParams.insert(pair{SQLITE_DB_CONNECTION_PARAM_FILE_NAME, TEST_DB_FILE_NAME});
    EXPECT_FALSE(db->isOpen());
    EXPECT_TRUE(db->open(connectionParams));
    EXPECT_TRUE(db->isOpen());
    EXPECT_TRUE(filesystem::exists(dbFileName));
    db->close();
    EXPECT_FALSE(db->isOpen());
    filesystem::remove(dbFileName);
    delete db;
}

TEST(DBInterface, doesNotOpenIfFilePathIsInvalid) {
    Database* db = new DatabaseSQLite();
    map<string, string> connectionParams;
    connectionParams.insert(pair{SQLITE_DB_CONNECTION_PARAM_FILE_NAME, "no/valid/file/path"});
    EXPECT_FALSE(db->isOpen());
    EXPECT_FALSE(db->open(connectionParams));
    EXPECT_FALSE(db->isOpen());
    delete db;
}