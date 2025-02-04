#include <gtest/gtest.h>
#include <typeinfo>
#include <filesystem>
#include "logger.h"
#include "dbinterface.h"
#include "car.h"

using namespace std;
using namespace level2;

namespace test_car {
    string workingDir;
    string testFilesDir;

    void configureLogger() {
        Logger& logger = Logger::getInstance();
        logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }

    void configureTestVariables() {
        workingDir = filesystem::current_path();
        LOGGER.info("Running test in directory: " + workingDir);
        if(getenv("TEST_FILES_DIR") != NULL) {
            testFilesDir = getenv("TEST_FILES_DIR");
            LOGGER.info("Testfiles expected to be present in directory: " + testFilesDir);
        } else {
            LOGGER.error("Environment variable TEST_FILES_DIR must be set to where the tests expect the testfiles to live!");
        }  
    }
}

void configureTest() {
    test_car::configureLogger();
    test_car::configureTestVariables();
}


TEST(Car, testTheTest) {
    configureTest();
    LOGGER.info("Hello from the logger");
    EXPECT_EQ("hallo", "hallo");
}


TEST(Car, canCreateAnInstance) {
    Car car("California", "Gö BL 0815");
    EXPECT_EQ("California", car.getModel());
    EXPECT_EQ("Gö BL 0815", car.getLicensePlate());
}

TEST(Car, isInstanceOfBusinessObject) {
    Car car("California", "Gö BL 0815");
    EXPECT_FALSE(dynamic_cast<BusinessObject*>(&car) == NULL);
}

TEST(Car, isInstanceOfBinaryProcessingData) {
    Car car("California", "Gö BL 0815");
    EXPECT_FALSE(dynamic_cast<BinaryProcessingData*>(&car) == NULL);
}

TEST(Car, isInstanceOfSerializableJson) {
    Car car("California", "Gö BL 0815");
    EXPECT_FALSE(dynamic_cast<SerializableJson*>(&car) == NULL);
}

TEST(Car, doesNotWriteToClosedDataBase) {
    Car car("California", "Gö BL 0815");
    shared_ptr<Database> db = make_shared<DatabaseSQLite>();
    EXPECT_FALSE(car.writeToDataBase(db));
}

TEST(Car, canWriteToDB) {
    Car car("California", "Gö BL 0815");
    shared_ptr<Database> db = make_shared<DatabaseSQLite>();
    map<string, string> connectionParams;
    connectionParams.emplace(pair{"fileName", "./test.db"});
    db->open(connectionParams);
    EXPECT_TRUE(car.writeToDataBase(db));
    filesystem::remove("./test.db");
}
