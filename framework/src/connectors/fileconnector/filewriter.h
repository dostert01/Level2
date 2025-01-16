#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <string>
#include "logger.h"
#include "pipelineapi.h"

using namespace std;

class FileWriter {
    public:
        FileWriter(PipelineStepInitData& initData);
        void writeFile(PipelineProcessingData& processData);
    private:
        string targetDirectory;
        string getNamedArgument(PipelineStepInitData& initData, string paramName);
        string createFileName(PipelineProcessingData& processData);
        void tryWritingTheFile(PipelineProcessingData& processData);
        bool ensureDirectoryExists(string path);
};

#endif //#ifndef FILE_WRITER_H