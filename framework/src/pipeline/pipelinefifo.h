#ifndef PIPELINE_FIFO_H
#define PIPELINE_FIFO_H

#include <memory>
#include <queue>
#include <mutex>
#include <optional>
#include <shared_mutex>

#include "apistructs.h"

using namespace std;


class PipelineFiFo {
    public:
        PipelineFiFo() = default;
        static shared_ptr<PipelineFiFo> getInstance();
        optional<shared_ptr<PipelineProcessingData>> dequeue();
        void enqueue(string payloadName, string mimetype, string payload);
        void enqueue(shared_ptr<PipelineProcessingData> data);
    private:        
        std::shared_mutex mutex;
        queue<shared_ptr<PipelineProcessingData>> processingDataFiFo;
};


#endif //#ifndef PIPELINE_FIFO_H