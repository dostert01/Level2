#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>
#include <vector>

#include "../matchable.h"
#include "../../dbinterface/dbinterface.h"

using namespace std;
using namespace second_take;

struct PipelineStepInitData
{
    std::multimap<std::string, std::string> namedArguments;
    std::optional<std::string> getNamedArgument(const std::string& argumentName);
};

class SerializableDB {
    public:
        SerializableDB() = default;
        virtual ~SerializableDB() = default;
        virtual bool writeToDataBase(shared_ptr<Database> db) = 0;
        virtual bool initFromDataBase(shared_ptr<Database> db) = 0;
};

class SerializableJson {
    public:
        SerializableJson() = default;
        virtual ~SerializableJson() = default;
        virtual string toJson() = 0;
        virtual void initFromJson(string jsonString) = 0;
};

class BinaryProcessingData {
    public:
        BinaryProcessingData() = default;
        virtual ~BinaryProcessingData();
};

class ProcessingError : public BinaryProcessingData {
    public:
        ProcessingError(std::string errorCode, std::string errorMessage);
        ProcessingError(ProcessingError* other);
        std::string getErrorCode();
        std::string getErrorMessage();
    private:
        std::string errorCode;
        std::string errorMessage;
};

class ProcessingPayload {
    private:
        std::string mimetype;
        std::string stringPayloadData;
        std::shared_ptr<BinaryProcessingData> binaryPayloadData;
    public:
        ProcessingPayload() = default;
        ProcessingPayload(std::string mimetype, std::string payload);
        ProcessingPayload(std::string mimetype, std::shared_ptr<BinaryProcessingData> payload);
        ~ProcessingPayload();
        void setMimeType(std::string mimetype);
        void setPayload(std::string payload);
        void setPayload(std::shared_ptr<BinaryProcessingData> payload);
        std::string payloadAsString();
        std::shared_ptr<BinaryProcessingData> payloadAsBinaryData();
};

#define PAYLOAD_NAME_PROCESSING_ERROR "___processingError___"
#define PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM "application/octet-stream"

class PipelineProcessingData : public second_take::Matchable {
    private:
        std::multimap<std::string, std::shared_ptr<ProcessingPayload>> namedPayloadData;
        std::string lastProcessedPipelineName;
        uint processingCounter = 0;
    public:
        PipelineProcessingData() = default;
        ~PipelineProcessingData();
        void addPayloadData(std::string payloadName, std::string mimetype, std::string data);
        void addPayloadData(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> data);
        void addError(std::string errorCode, std::string errorMessage);
        bool hasError();
        std::vector<ProcessingError> getAllErrors();
        std::optional<std::shared_ptr<ProcessingPayload>> getPayload(std::string payloadName);
        uint getCountOfPayloads();
        uint getProcessingCounter();
        void increaseProcessingCounter();
        void setLastProcessedPipelineName(std::string pipelineName);
        std::string getLastProcessedPipelineName();
};

#endif //#ifndef API_HELPERS_H