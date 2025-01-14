#ifndef NETWORK_LISTENER_H
#define NETWORK_LISTENER_H
#include <memory>
#include "pipelinefifo.h"
#include "pipeline.h"

class NetworkListener {
    public:
        NetworkListener() = default;
        virtual ~NetworkListener() = default;
        bool isListening() { return listening;}
        bool isIniComplete() {return initComplete;}
        virtual void init(shared_ptr<PipelineFiFo> fillerPipe);
        virtual void startListening() = 0;
        virtual optional<shared_ptr<PipelineProcessingData>> getLastMessage();
    protected:
        void setIsListening(bool value) { listening = value; }
        bool initComplete = false;
        shared_ptr<PipelineFiFo> pipelineFifo;
    private:
        bool listening = false;
};

#endif //#define NETWORK_LISTENER_H