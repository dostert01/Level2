#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>
#include <vector>

struct PipelineStepInitData
{
    std::multimap<std::string, std::string> namedArguments;
    std::optional<std::string> getNamedArgument(const std::string& argumentName);
};

class BinaryProcessingData {
    public:
        BinaryProcessingData() = default;
        virtual ~BinaryProcessingData();
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

class PipelineProcessingData
{
    private:
        std::multimap<std::string, ProcessingPayload*> namedPayloadData;
    public:
        PipelineProcessingData() = default;
        ~PipelineProcessingData();
        void addPayloadData(std::string payloadName, std::string mimetype, std::string data);
        void addPayloadData(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> data);
        std::optional<ProcessingPayload*> getPayload(std::string payloadName);
        uint getCountOfPayloads();
};

#endif //#ifndef API_HELPERS_H