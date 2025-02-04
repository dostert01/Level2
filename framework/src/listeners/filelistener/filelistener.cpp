
#include "filelistener.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <fstream>
#include <iostream>

#include "applicationcontext.h"

#define HALF_A_SECOND 500000
#define PATH_SEPARATOR "/"

using namespace std;
namespace level2 {

FileListener::FileListener(json jsonObject) {
  directory2Monitor = jsonObject["directory2Monitor"];
  inotifyFd = -1;
  inotifyWatch = -1;
};

FileListener::~FileListener() {
  stopMonitoringThread();
  stopINotify();
}

shared_ptr<FileListener> FileListener::getInstance() {
  return make_shared<FileListener>();
}

void FileListener::init(shared_ptr<PipelineFiFo> fifo) {
  NetworkListener::init(fifo);
  initComplete = ensureDirectoryExists();
  if(initComplete)
    initComplete = initINotify();

}

void FileListener::startListening() {
    keepThreadRunning.store(true);
    monitoringThread = thread(&FileListener::monitoringLoop, this);
    setIsListening(monitoringThread.joinable());
    LOGGER.info("staring monitoring thread " + string(isListening() ? "succeeded" : "failed"));
}

string FileListener::getDirectory2Monitor() { return directory2Monitor; }

bool FileListener::directoryExists() {
  struct stat fileStatus;
  return (stat(directory2Monitor.c_str(), &fileStatus) == 0 &&
          (fileStatus.st_mode & S_IFDIR));
}

bool FileListener::ensureDirectoryExists() {
  bool dirExists = directoryExists();
  if (!dirExists) {
    dirExists = tryToCreateDirectory();
    if (dirExists) {
      LOGGER.debug("monitoring directory created: " + directory2Monitor);
    } else {
      LOGGER.error("failed to create monitoring directory: " + directory2Monitor);
    }
  }
  return dirExists;
}

bool FileListener::tryToCreateDirectory() {
  bool dirExists;
  try {
    dirExists = filesystem::create_directories(directory2Monitor);
  } catch (const exception& e) {
    dirExists = false;
    LOGGER.error("failed to create monitoring directory: '" +
                 directory2Monitor + "' : " + e.what());
  }
  return dirExists;
}

void FileListener::monitoringLoop() {
    LOGGER.info("monitoring loop started for directory " + directory2Monitor);
    int sleepTime = HALF_A_SECOND;
    while(keepThreadRunning.load()) {
        char buffer[1024];
        int bytesRead = read(inotifyFd, buffer, sizeof(buffer));
        if(bytesRead > 0) {
            sleepTime = 1;
            inotify_event *event;
            for(char *p = buffer; p < buffer + bytesRead;) {
                event = (inotify_event*)p;
                processFile(string(event->name));
                p += sizeof(inotify_event) + event->len;
            }
        } else if(bytesRead == -1 && errno == EAGAIN) {
            usleep(HALF_A_SECOND);
        } else if(bytesRead == -1 && errno != EAGAIN) {
            LOGGER.error("error occurred while reading from inotify: " + string(strerror(errno)));
            usleep(HALF_A_SECOND);
        }
        usleep(sleepTime);
        if(sleepTime < HALF_A_SECOND)
          sleepTime *= 2;
    }
    LOGGER.debug("about to exit monitoring loop");
}

void FileListener::processFile(string fileName) {
    string fullFileName = directory2Monitor;
    if(!directory2Monitor.ends_with(PATH_SEPARATOR))
        fullFileName.append(PATH_SEPARATOR);
    fullFileName.append(fileName);
    LOGGER.debug("processing file '" + fullFileName + "'");
    ifstream inFile(fullFileName, std::ios::binary);
    if(inFile.is_open()) {
        std::string fileContent{std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>()};
        inFile.close();
        addContentAsNewPayload(fileContent, fileName);

    } else {
        LOGGER.error("failed to open '" + fullFileName + "' for reading");
    }  
}

void FileListener::addContentAsNewPayload(const string& content, string fileName) {
    map<string, string> params;
    params[PAYLOAD_MATCHING_PATTERN_DATA_ORIGIN] = directory2Monitor;
    params[PAYLOAD_MATCHING_PATTERN_RECEIVED_BY_LISTENER] = PAYLOAD_MATCHING_PATTERN_VALUE_FILE_LISTENER;
    params[PAYLOAD_MATCHING_PATTERN_RECEIVED_VIA_PROTOCOL] = PAYLOAD_MATCHING_PATTERN_VALUE_PROTOCOL_FILE;
    params[PAYLOAD_MATCHING_PATTERN_INPUT_FILE_NAME] = fileName;
    this->pipelineFifo->enqueue(
      PAYLOAD_NAME_FILE_RECEIVED_DATA,
      PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM,
      content, params);
}

void FileListener::stopMonitoringThread() {
  if (monitoringThread.joinable()) {
    LOGGER.info("stopping monitoring thread");
    keepThreadRunning.store(false);
    monitoringThread.join();
  }
}

bool FileListener::initINotify() {
    inotifyFd = inotify_init1(IN_NONBLOCK);
    if (inotifyFd == -1) {
        LOGGER.error("failed to initialize inotify for directory " + directory2Monitor + " : " + string(strerror(errno)));
    } else {
        inotifyWatch = inotify_add_watch(inotifyFd, directory2Monitor.c_str(), IN_CLOSE_WRITE);
        if (inotifyWatch == -1) {
            LOGGER.error("failed to establish the inotify watch on " + directory2Monitor + " : " + string(strerror(errno)));
        }   
    }
    LOGGER.info("inotify initialization for " + directory2Monitor + " " + string(isINotifyInitialized() ? "succeeded" : "failed"));
    return isINotifyInitialized();
}

bool FileListener::isINotifyInitialized() {
    return inotifyFd != -1 && inotifyWatch != -1;
}

void FileListener::stopINotify() {
    if(isINotifyInitialized()) {
        LOGGER.info("shutting down inotify for directory " + directory2Monitor);
        inotify_rm_watch(inotifyFd, inotifyWatch);
        close(inotifyFd);
    }
}
} // namespace level2