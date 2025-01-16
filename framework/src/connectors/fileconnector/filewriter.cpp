#include "filewriter.h"

#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include "applicationcontext.h"

#define PARAM_NAME_TARGET_DIRECTORY "targetDirectory"

FileWriter::FileWriter(PipelineStepInitData& initData) {
  targetDirectory = getNamedArgument(initData, PARAM_NAME_TARGET_DIRECTORY);
  if (targetDirectory.empty()) {
    LOGGER.error("Failed loading init parameter for " +
                 string(PARAM_NAME_TARGET_DIRECTORY) +
                 ". Storing of files will stay disabled!");
  } else {
    LOGGER.info("file connector configured to write files to: " +
                targetDirectory);
  }
}

void FileWriter::writeFile(PipelineProcessingData& processData) {
    ensureDirectoryExists(targetDirectory);  
    tryWritingTheFile(processData);
}

void FileWriter::tryWritingTheFile(PipelineProcessingData& processData) {
  string fileName = createFileName(processData);
  string tmpFileName = fileName + ".tmp";
  ofstream outFile;
  try {
    outFile.open(tmpFileName);
    if (outFile.is_open()) {
      outFile << processData.getPayload("FILE_SEND_TEXT_DATA")
                     .value()
                     ->payloadAsString();
      outFile.close();
    } else {
      LOGGER.debug("failed opening file for writing: " + tmpFileName);
    }
    filesystem::rename(tmpFileName, fileName);
  } catch (const exception& ex) {
    LOGGER.debug("failed opening file for writing: " + tmpFileName + " : " + ex.what());
  }
}

bool FileWriter::ensureDirectoryExists(string path) {
  struct stat fileStatus;
  bool dirExists =
      (stat(path.c_str(), &fileStatus) == 0 && (fileStatus.st_mode & S_IFDIR));
  if (!dirExists) {
    dirExists = filesystem::create_directories(path);
    LOGGER.debug("trying to create target diretory: " + path +
                 " success: " + (dirExists ? "true" : "false"));
  }
  return dirExists;
}

string FileWriter::createFileName(PipelineProcessingData& processData) {
  string fileName =
    processData.getMatchingPattern(PAYLOAD_MATCHING_PATTERN_OUTPUT_FILE_NAME).value_or("");
  if(fileName.empty())
    fileName =
        processData.getMatchingPattern(PAYLOAD_MATCHING_PATTERN_TRANSACTION_ID).value_or("");
  if(fileName.empty())
    fileName = "unnamed";
  fileName =
      targetDirectory + "/" + fileName + ".dat";
  LOGGER.debug("writing file as: " + fileName);
  return fileName;
}

string FileWriter::getNamedArgument(PipelineStepInitData& initData,
                                    string paramName) {
  string value = initData.getNamedArgument(paramName).value_or("");
  LOGGER.info(paramName + ": " + value);
  return value;
}
