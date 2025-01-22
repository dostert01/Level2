#pragma once 

#include "pipelineapi.h"
#include "apistructs.h"
#include "serializables.h"

using namespace event_forge;
class BusinessObject : public BinaryProcessingData, public SerializableJson {
    public:
        BusinessObject() = default;
        virtual ~BusinessObject() = default;
};

