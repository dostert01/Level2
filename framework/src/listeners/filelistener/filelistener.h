#ifndef FILE_LISTENER_H
#define FILE_LISTENER_H

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>

#include "networklistener.h"

using namespace nlohmann::json_abi_v3_11_3;

namespace level2 {
class FileListener : public NetworkListener {
    public:
      FileListener() = default;
      FileListener(json jsonObject);
      ~FileListener();
      static std::shared_ptr<FileListener> getInstance();
      void init(std::shared_ptr<PipelineFiFo> fifo);
      void startListening();
      std::string getDirectory2Monitor();
    private:
      std::string directory2Monitor;
      std::atomic_bool keepThreadRunning;
      std::thread monitoringThread;
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
      void processFile(std::string fileName);
      void addContentAsNewPayload(const std::string& content, std::string fileName);
};
} // namespace level2
#endif //#define FILE_LISTENER_H