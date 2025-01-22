#pragma once

#include <string>
#include "businessObject.h"
#include "dbinterface.h"

using namespace std;

class Car : public BusinessObject {
    public:
        Car(string model, string licensePlate);
        ~Car() = default;
        string getModel();
        string getLicensePlate();
        bool writeToDataBase(shared_ptr<Database> db);
        bool initFromDataBase(shared_ptr<Database> db){return "";};
        string toJson(){return "";};
        void initFromJson(string jsonString){};
    private:
        string model;
        string licensePlate;

};
