#ifndef FILE_LISTENER_H
#define FILE_LISTENER_H

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>

#include "networklistener.h"

using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

class FileListener : public NetworkListener {
    public:
      FileListener() = default;
      FileListener(json jsonObject);
      ~FileListener();
      static shared_ptr<FileListener> getInstance();
      void init(shared_ptr<PipelineFiFo> fifo);
      void startListening();
      string getDirectory2Monitor();
    private:
      string directory2Monitor;
      atomic_bool keepThreadRunning;
      thread monitoringThread;
      int inotifyFd;
      int inotifyWatch;
      void monitoringLoop();
      void stopMonitoringThread();
      bool directoryExists();
      bool ensureDirectoryExists();
      bool tryToCreateDirectory();
      bool initINotify();
      bool isINotifyInitialized();
      void stopINotify();
      void processFile(string fileName);
      void addContentAsNewPayload(const string& content, string fileName);
};

#endif //#define FILE_LISTENER_H