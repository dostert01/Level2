#ifndef FILLER_PIPE_H
#define FILLER_PIPE_H

#include <memory>
#include <optional>
#include <shared_mutex>

#include "apistructs.h"

namespace event_forge {

class FillerPipe {  
    public:
        FillerPipe() = default;
        virtual ~FillerPipe() = default;
        virtual void fillIntoPipeline4AsynchronousProcessing(shared_ptr<PipelineProcessingData> data) = 0;
        virtual optional<shared_ptr<PipelineProcessingData>> fillIntoPipeline4SynchronousProcessing(shared_ptr<PipelineProcessingData> data) = 0;
};

} // namespace event_forge

#endif //#define FILLER_PIPE_H
