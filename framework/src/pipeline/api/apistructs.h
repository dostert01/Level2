#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <limits.h>

#include "../matchable.h"
#include "../../dbinterface/dbinterface.h"

using namespace std;
using namespace event_forge;

struct PipelineStepInitData
{
    multimap<string, string> namedArguments;
    optional<string> getNamedArgument(const string& argumentName);
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
        ProcessingError(string errorCode, string errorMessage);
        ProcessingError(ProcessingError* other);
        string getErrorCode();
        string getErrorMessage();
    private:
        string errorCode;
        string errorMessage;
};

class ProcessingPayload {
    private:
        string payloadName;
        string mimetype;
        string stringPayloadData;
        shared_ptr<BinaryProcessingData> binaryPayloadData;
    public:
        ProcessingPayload() = default;
        ProcessingPayload(string payloadName, string mimetype, string payload);
        ProcessingPayload(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> payload);
        ~ProcessingPayload();
        void setPayloadName(string payloadName);
        string getPayloadName();
        void setMimeType(string mimetype);
        void setPayload(string payload);
        void setPayload(shared_ptr<BinaryProcessingData> payload);
        string payloadAsString();
        shared_ptr<BinaryProcessingData> payloadAsBinaryData();
};

#define PIPELINE_PROCESSING_DATA_COUNTER_MAX (INT_MAX / 2)

class PipelineProcessingData : public Matchable {
    public:
        PipelineProcessingData();
        ~PipelineProcessingData();
        static shared_ptr<PipelineProcessingData> getInstance();
        void addPayloadData(string payloadName, string mimetype, string data);
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