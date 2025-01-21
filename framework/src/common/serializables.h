#pragma once

class SerializableJson {
    public:
        SerializableJson() = default;
        virtual ~SerializableJson() = default;
        virtual void toJson(void* jsonData) = 0;
};


