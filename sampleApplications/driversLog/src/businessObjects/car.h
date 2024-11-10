#ifndef INC_BUSINESS_OBJECT_CAR
#define INC_BUSINESS_OBJECT_CAR

#include <string>
#include "businessObject.h"

using namespace std;

class Car : public BusinessObject {
    public:
        Car(string model, string licensePlate);
        string getModel();
        string getLicensePlate();
        bool writeToDataBase(shared_ptr<Database> db){};
        bool initFromDataBase(shared_ptr<Database> db){};
        string toJson(){};
        void initFromJson(string jsonString){};
    private:
        string model;
        string licensePlate;

};

#endif //INC_BUSINESS_OBJECT_CAR
