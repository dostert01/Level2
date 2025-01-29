#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <shared_mutex>
#include <nlohmann/json.hpp>

#include "networklistener.h"

using namespace nlohmann::json_abi_v3_11_3;

namespace event_forge {

class GenericServer : public NetworkListener {
    public:
        GenericServer() = default;
        GenericServer(json jsonObject);
        ~GenericServer();
        void init(std::shared_ptr<PipelineFiFo> fifo);
        void init(std::shared_ptr<PipeLineProcessor> processor);
        void startListening();
        int getPort();
    private:
        int port = 8888;
        int maxClients = 10;
        int serverSocket;
        std::thread listeningThread;
        std::thread workingThreadsCleaner;
        std::atomic_bool keepListeningThreadRunning;
        std::vector<std::shared_ptr<std::future<void>>> workingThreads;
        std::shared_mutex workingThreadsMutex;
        void listeningThreadFunction();
        void addToListOfWorkingThreads(std::shared_ptr<std::future<void>> &workingThreadPointer);
        void workingThreadFunction(int clientSocket, std::string clientHost);
        virtual void handleClientConnection(int clientSocket, std::string clientHost) = 0;
        void workingThreadsCleanerFunction();
        bool haveWorkingThreads();
        void eraseFinishedWorkingThreads();
        void createReusableServerSocket();
        void createServerSocket();
        void setSocketOptions();
        void stopListening();
        void bindServerSocket();
        void listenOnServerSocket();
        void handleListeningError(std::string message);
        void setNonBlocking(int fileDescriptor);
};

} // namespace event_forge
