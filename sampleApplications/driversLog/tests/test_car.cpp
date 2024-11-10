#include <gtest/gtest.h>
#include <typeinfo>
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
