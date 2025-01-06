#include <gtest/gtest.h>
#include <typeinfo>
#include <filesystem>
#include "car.h"
#include "businessObject.h"

using namespace std;

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

TEST(Car, isInstanceOfSerializableDB) {
    Car car("California", "Gö BL 0815");
    EXPECT_FALSE(dynamic_cast<SerializableDB*>(&car) == NULL);
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
