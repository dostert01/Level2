#include "car.h"

/*
    Car
*/
Car::Car(string model, string licensePlate) {
    this->model = model;
    this->licensePlate = licensePlate;
}

string Car::getModel() {
    return model;
}

string Car::getLicensePlate() {
    return licensePlate;
}

bool Car::writeToDataBase(shared_ptr<Database> db) {
    bool returnValue = false;
    if(db->isOpen()) {

    }
    return returnValue;
};