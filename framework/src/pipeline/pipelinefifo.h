#ifndef PIPELINE_FIFO_H
#define PIPELINE_FIFO_H

#include <memory>
#include <queue>
#include <mutex>
#include <optional>
#include <shared_mutex>

#include "fillerpipe.h"

using namespace std;


class PipelineFiFo : public FillerPipe {
    public:
        PipelineFiFo() = default;
        static shared_ptr<PipelineFiFo> getInstance();
        optional<shared_ptr<PipelineProcessingData>> dequeue();
    protected:
        void add2Fifo(shared_ptr<PipelineProcessingData> data);;
    private:
        void enqueue(shared_ptr<PipelineProcessingData> data);
        std::shared_mutex mutex;
        queue<shared_ptr<PipelineProcessingData>> processingDataFiFo;
};


#endif //#ifndef PIPELINE_FIFO_H