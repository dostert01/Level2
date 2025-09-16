#ifndef NETWORK_LISTENER_H
#define NETWORK_LISTENER_H
#include <memory>
#include "pipelinefifo.h"
#include "pipeline.h"
#include "pipelineprocessor.h"
#include "logger.h"

#include <format>

namespace level2 {

enum ListenerProcessingMode {not_set, synchronous, asynchronous};

class NetworkListener {
    public:
        NetworkListener();
        NetworkListener(json jsonObject);
        virtual ~NetworkListener() = default;
        bool isListening();
        bool isIniComplete();
        std::string getName();
        virtual void init(std::shared_ptr<PipelineFiFo> fillerPipe);
        virtual void init(std::shared_ptr<PipeLineProcessor> processor);
        virtual void startListening() = 0;
        virtual std::optional<std::shared_ptr<PipelineProcessingData>> getLastMessage();
    protected:
        bool initComplete = false;
        std::shared_ptr<PipelineFiFo> pipelineFifo;
        std::shared_ptr<PipeLineProcessor> pipeLineProcessor;
        ListenerProcessingMode processingMode;
        void setIsListening(bool value) { listening = value; }
        template <typename T> T getFromJson(std::string fieldName, json jsonObject, T defaultValue) {
            T returnValue = defaultValue;
            try {
                returnValue = jsonObject[fieldName];
            } catch (const std::exception& e) {
                LOGGER.error(std::format("Construction of NetworkListener failed {} "
                    "Make sure field '{}' is contained in the json structure! "
                    "Using default value: '{}'", e.what(), fieldName, defaultValue));
            }
            return returnValue;
        }
    private:
        bool listening = false;
        std::string name;
};
} //namespace level2
#endif //#define NETWORK_LISTENER_H