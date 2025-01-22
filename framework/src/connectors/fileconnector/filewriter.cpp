#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include "filewriter.h"
#include "applicationcontext.h"

#define PARAM_NAME_TARGET_DIRECTORY "targetDirectory"
namespace event_forge {

FileWriter::FileWriter(PipelineStepInitData& initData) {
  targetDirectory = getNamedArgument(initData, PARAM_NAME_TARGET_DIRECTORY);
  if (targetDirectory.empty()) {
    LOGGER.error("Failed loading init parameter for " +
                 std::string(PARAM_NAME_TARGET_DIRECTORY) +
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
  std::string fileName = createFileName(processData);
  std::string tmpFileName = fileName + ".tmp";
  std::ofstream outFile;
  std::string fileContent = getPayloadForWriting(processData);
  try {
    outFile.open(tmpFileName);
    if (outFile.is_open()) {
      outFile << fileContent;
      outFile.close();
    } else {
      LOGGER.debug("failed opening file for writing: " + tmpFileName);
    }
    std::filesystem::rename(tmpFileName, fileName);
  } catch (const std::exception& ex) {
    LOGGER.debug("failed opening file for writing: " + tmpFileName + " : " + ex.what());
  }
}

std::string FileWriter::getPayloadForWriting(PipelineProcessingData& processData) {
  std::string fileContent = "";
  auto payload = processData.getPayload(PAYLOAD_NAME_FILE_SEND_DATA);
  if (!payload.has_value()) {
    LOGGER.warn("no payload with name '" +
                std::string(PAYLOAD_NAME_FILE_SEND_DATA) +
                "' found in processing data. Output file will be empty!");
  } else {
    fileContent = payload.value()->payloadAsString();
  }
  return fileContent;
}

bool FileWriter::ensureDirectoryExists(std::string path) {
  struct stat fileStatus;
  bool dirExists =
      (stat(path.c_str(), &fileStatus) == 0 && (fileStatus.st_mode & S_IFDIR));
  if (!dirExists) {
    dirExists = std::filesystem::create_directories(path);
    LOGGER.debug("trying to create target diretory: " + path +
                 " success: " + (dirExists ? "true" : "false"));
  }
  return dirExists;
}

std::string FileWriter::createFileName(PipelineProcessingData& processData) {
  std::string fileName =
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

std::string FileWriter::getNamedArgument(PipelineStepInitData& initData,
                                    std::string paramName) {
  std::string value = initData.getNamedArgument(paramName).value_or("");
  LOGGER.info(paramName + ": " + value);
  return value;
}

} //namespace event_forge