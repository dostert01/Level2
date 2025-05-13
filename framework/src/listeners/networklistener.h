#ifndef NETWORK_LISTENER_H
#define NETWORK_LISTENER_H
#include <memory>
#include "pipelinefifo.h"
#include "pipeline.h"
#include "pipelineprocessor.h"

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
        void setIsListening(bool value) { listening = value; }
        bool initComplete = false;
        std::shared_ptr<PipelineFiFo> pipelineFifo;
        std::shared_ptr<PipeLineProcessor> pipeLineProcessor;
        ListenerProcessingMode processingMode;
    private:
        bool listening = false;
        std::string name;
};
} //namespace level2
#endif //#define NETWORK_LISTENER_H