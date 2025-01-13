#ifndef PIPELINE_FIFO_H
#define PIPELINE_FIFO_H

#include <memory>
#include <queue>
#include <mutex>
#include <optional>
#include <shared_mutex>

#include "fillerpipe.h"

using namespace std;

namespace event_forge {

class PipelineFiFo : public FillerPipe {
    public:
        PipelineFiFo() = default;
        static shared_ptr<PipelineFiFo> getInstance();
        optional<shared_ptr<PipelineProcessingData>> dequeue();
        void fillIntoPipeline4AsynchronousProcessing(shared_ptr<PipelineProcessingData> data);
        optional<shared_ptr<PipelineProcessingData>> fillIntoPipeline4SynchronousProcessing(shared_ptr<PipelineProcessingData> data);
    private:
        void enqueue(shared_ptr<PipelineProcessingData> data);
        std::shared_mutex mutex;
        queue<shared_ptr<PipelineProcessingData>> processingDataFiFo;
};

} //namespace event_forge

#endif //#ifndef PIPELINE_FIFO_H