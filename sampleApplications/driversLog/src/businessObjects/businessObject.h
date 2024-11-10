#ifndef INC_BUSINESS_OBJECT_H
#define INC_BUSINESS_OBJECT_H

#include "pipelineapi.h"

class BusinessObject : public BinaryProcessingData, public SerializableDB, public SerializableJson {
    public:
        BusinessObject() = default;
        virtual ~BusinessObject() = default;
};

#endif //INC_BUSINESS_OBJECT_H