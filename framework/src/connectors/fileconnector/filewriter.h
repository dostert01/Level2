#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <string>
#include "logger.h"
#include "pipelineapi.h"

namespace event_forge {

class FileWriter {
    public:
        FileWriter(PipelineStepInitData& initData);
        void writeFile(PipelineProcessingData& processData);
    private:
        std::string targetDirectory;
        std::string getNamedArgument(PipelineStepInitData& initData, std::string paramName);
        std::string createFileName(PipelineProcessingData& processData);
        void tryWritingTheFile(PipelineProcessingData& processData);
        std::string getPayloadForWriting(PipelineProcessingData& processData);
        bool ensureDirectoryExists(std::string path);
};

} // namespace event_forge
#endif //#ifndef FILE_WRITER_H