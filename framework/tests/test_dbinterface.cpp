#include <gtest/gtest.h>
#include "../logger/logger.h"
#include "dbinterface.h"

#include <filesystem>
#include <map>

using namespace level2;
using namespace std;

#define TEST_DB_FILE_NAME "test.db"

namespace test_db_interface {
    string workingDir;
    string testFilesDir;
    string testDBFileName = "./testDBFile.db";

    void configureLogger() {
        Logger& logger = Logger::getInstance();
        logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
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
    connectionParams.insert(pair{DB_CONNECTION_PARAM_FILE_NAME, TEST_DB_FILE_NAME});
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
    connectionParams.insert(pair{DB_CONNECTION_PARAM_FILE_NAME, "no/valid/file/path"});
    EXPECT_FALSE(db->isOpen());
    EXPECT_FALSE(db->open(connectionParams));
    EXPECT_FALSE(db->isOpen());
    delete db;
}

TEST(DBInterface, doesNotOpenIfConnectionParamIsMissing) {
    Database* db = new DatabaseSQLite();
    map<string, string> connectionParams;
    EXPECT_FALSE(db->isOpen());
    EXPECT_FALSE(db->open(connectionParams));
    EXPECT_FALSE(db->isOpen());
    delete db;
}

TEST(DBInterface, canExecuteSomeSQLs) {
    Database* db = new DatabaseSQLite();
    map<string, string> connectionParams;
    connectionParams.insert(pair{DB_CONNECTION_PARAM_FILE_NAME, test_db_interface::testDBFileName});
    db->open(connectionParams);
    db->executeQuery("create table cars(" \
        "id integer primary key, " \
        "model char(50) not null," \
        "license_plate char(30))");
    db->executeQuery("insert into cars" \
        "(id, model, license_plate) values" \
        "(null, 'A4', 'GÖ BL 4711')");
    db->executeQuery("insert into cars" \
        "(id, model, license_plate) values" \
        "(null, 'California', 'GÖ CA 0815')");
    shared_ptr<RecordSet> result = db->executeQuery("select * from cars order by id");
    EXPECT_EQ(2, result->getRowCount());
    EXPECT_EQ("California", result->getValue("model", 1));
    EXPECT_EQ("GÖ BL 4711", result->getValue("license_plate", 0));
    EXPECT_EQ(false, result->getIsNull("id", 0));
    db->close();
    filesystem::remove(test_db_interface::testDBFileName);
    delete db;
}