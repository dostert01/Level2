#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <limits.h>

#include "../matchable.h"
#include "common.h"
#include "../../dbinterface/dbinterface.h"

using namespace std;
using namespace event_forge;

struct PipelineStepInitData
{
    multimap<string, string> namedArguments;
    optional<string> getNamedArgument(const string& argumentName);
};

class BinaryProcessingData {
    public:
        BinaryProcessingData() = default;
        virtual ~BinaryProcessingData();
};

class ProcessingError : public BinaryProcessingData, public SerializableJson {
    public:
        ProcessingError(string errorCode, string errorMessage);
        ProcessingError(ProcessingError* other);
        string getErrorCode();
        string getErrorMessage();
        void toJson(void* jsonData) override;
    private:
        string errorCode;
        string errorMessage;
};

class ProcessingPayload : public SerializableJson {
    private:
        string payloadName;
        string mimetype;
        string stringPayloadData;
        shared_ptr<BinaryProcessingData> binaryPayloadData;
    public:
        ProcessingPayload() = default;
        ProcessingPayload(string payloadName, string mimetype, const string& payload);
        ProcessingPayload(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> payload);
        ~ProcessingPayload();
        void setPayloadName(string payloadName);
        string getPayloadName();
        void setMimeType(string mimetype);
        void setPayload(const string& payload);
        void setPayload(shared_ptr<BinaryProcessingData> payload);
        string payloadAsString();
        string payloadAsBase64String();
        shared_ptr<BinaryProcessingData> payloadAsBinaryData();
        void toJson(void* jsonData) override;
};

#define PIPELINE_PROCESSING_DATA_COUNTER_MAX (INT_MAX / 2)

class PipelineProcessingData : public Matchable, public SerializableJson {
    public:
        PipelineProcessingData();
        ~PipelineProcessingData();
        static shared_ptr<PipelineProcessingData> getInstance();
        void addPayloadData(string payloadName, string mimetype, const string& data);
        void addPayloadData(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> data);
        void addError(string errorCode, string errorMessage);
        bool hasError();
        vector<shared_ptr<ProcessingError>> getAllErrors();
        optional<shared_ptr<ProcessingPayload>> getPayload(string payloadName);
        optional<shared_ptr<ProcessingPayload>> getLastPayload();
        int getCountOfPayloads();
        int getProcessingCounter();
        void increaseProcessingCounter();
        void setLastProcessedPipelineName(string pipelineName);
        string getLastProcessedPipelineName();
        void toJson(void* jsonData) override;
    private:
        static atomic_int counter;
        vector<shared_ptr<ProcessingPayload>> payloadDataCollection;
        vector<shared_ptr<ProcessingPayload>> errors;
        string lastProcessedPipelineName;
        int processingCounter = 0;
        void setDefaultProperties();
        string getTimeStampOfNow(const string& pattern);
        string getFormattedCounter();
};

#endif //#ifndef API_HELPERS_H