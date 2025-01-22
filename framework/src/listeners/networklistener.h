#ifndef NETWORK_LISTENER_H
#define NETWORK_LISTENER_H
#include <memory>
#include "pipelinefifo.h"
#include "pipeline.h"

namespace event_forge {
class NetworkListener {
    public:
        NetworkListener() = default;
        virtual ~NetworkListener() = default;
        bool isListening() { return listening;}
        bool isIniComplete() {return initComplete;}
        virtual void init(std::shared_ptr<PipelineFiFo> fillerPipe);
        virtual void startListening() = 0;
        virtual std::optional<std::shared_ptr<PipelineProcessingData>> getLastMessage();
    protected:
        void setIsListening(bool value) { listening = value; }
        bool initComplete = false;
        std::shared_ptr<PipelineFiFo> pipelineFifo;
    private:
        bool listening = false;
};
} //namespace event_forge
#endif //#define NETWORK_LISTENER_H