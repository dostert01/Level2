#ifndef SECOND_TAKE_PIPELINE_PROCESSOR
#define SECOND_TAKE_PIPELINE_PROCESSOR

#include <memory>
#include <optional>

#include "pipeline.h"

namespace second_take {

    class PipeLineProcessor {
        public:
            PipeLineProcessor() = default;
            ~PipeLineProcessor();
            static std::unique_ptr<PipeLineProcessor> getInstance();
            static std::optional<std::unique_ptr<PipeLineProcessor>> getInstance(const std::string configFilePath);
            uint getCountOfPipelines();
        private:
            void loadProcessConfig(const std::string& configFilePath);
    };
}

#endif //SECOND_TAKE_PIPELINE_PROCESSOR
