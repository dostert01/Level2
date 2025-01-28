#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <memory>

#include "applicationcontext.h"
#include "generic_server.h"

#define MAX_EVENTS 1
#define HALF_A_SECOND 500000

using namespace std::chrono_literals;
namespace event_forge {

GenericServer::GenericServer(json jsonObject) {
  port = jsonObject["port"];
  maxClients = jsonObject["maxClients"];
  serverSocket = -1;
};

GenericServer::~GenericServer() {
  stopListening();
}

void GenericServer::init(std::shared_ptr<PipelineFiFo> fifo) {
    NetworkListener::init(fifo);
    initComplete = true;
}

void GenericServer::startListening() {
  keepListeningThreadRunning.store(true);
  listeningThread = std::thread(&GenericServer::listeningThreadFunction, this);
  setIsListening(listeningThread.joinable());
  LOGGER.info("staring listening thread " + std::string(isListening() ? "succeeded" : "failed"));
  workingThreadsCleaner = std::thread(&GenericServer::workingThreadsCleanerFunction, this);
}

void GenericServer::stopListening() {
  LOGGER.info("stopping to listen for incoming HTTP calls on port: " + std::to_string(port));
  if(serverSocket != -1)
  {
    close(serverSocket);
    serverSocket = -1;
  }
  if(keepListeningThreadRunning.load()) {
    keepListeningThreadRunning.store(false);
  }
  if(listeningThread.joinable()) {
    listeningThread.join();
    LOGGER.info("stopped to listen for incoming HTTP calls on port: " + std::to_string(port));
  }
  if(workingThreadsCleaner.joinable()) {
    workingThreadsCleaner.join();
    LOGGER.info("all working threads have been ended");
  }
}

void GenericServer::createServerSocket() {
  if(initComplete) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1) {
      handleListeningError("failed creating listening socket: " + std::string(strerror(errno)));
    }
  }
}

void GenericServer::setSocketOptions() {
  if(initComplete) {
    int socketOptionValue = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &socketOptionValue, sizeof(socketOptionValue)) == -1) {
      handleListeningError("failed setting socket options: " + std::string(strerror(errno)));
    }
    setNonBlocking(serverSocket);
  }
}

void GenericServer::setNonBlocking(int fileDescriptor) {
    int flags = fcntl(fileDescriptor, F_GETFL, 0);
    if(flags == -1) {
      handleListeningError("failed reading flags from file descriptor: " + std::string(strerror(errno)));
    }
    flags = flags | O_NONBLOCK;
    if(fcntl(fileDescriptor, F_SETFL, flags) == -1) {
      handleListeningError("failed setting the file descriptor to non blocking: " + std::string(strerror(errno)));
    }
}

void GenericServer::createReusableServerSocket() {
  createServerSocket();
  setSocketOptions();
}

void GenericServer::bindServerSocket() {
  if(initComplete) {
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons((uint16_t)port);
    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
      handleListeningError("failed to bind listening socket to port " +
        std::to_string(port) +
        ". Error: " + std::to_string(errno) + " - " +
        std::string(strerror(errno)));
    }
  }
}

void GenericServer::listenOnServerSocket() {
  if(initComplete) {
    if(listen(serverSocket, maxClients) == -1) {
      handleListeningError("failed listening on server socket on port " + std::to_string(port) + ". Error: " + std::string(strerror(errno)));
    }
  }
}

void GenericServer::workingThreadsCleanerFunction() {
  LOGGER.info("starting working thread monitoring");
  while (keepListeningThreadRunning.load() || haveWorkingThreads()) {
    usleep(HALF_A_SECOND * 4);
    eraseFinishedWorkingThreads();
  }
}

bool GenericServer::haveWorkingThreads() {
  std::unique_lock<std::shared_mutex> lock(workingThreadsMutex);
  return !workingThreads.empty();
}

void GenericServer::eraseFinishedWorkingThreads() {
  std::unique_lock<std::shared_mutex> lock(workingThreadsMutex);
  for (auto it = workingThreads.begin(); it != workingThreads.end();) {
    if (it->get()->wait_for(0s) == std::future_status::ready) {
      LOGGER.info("removing finished working thread");
      it->get()->get();
      workingThreads.erase(it);
    }
  }
}

void GenericServer::workingThreadFunction(int clientSocket, std::string clientHost) {
  LOGGER.trace("start processing incoming connection");
  setNonBlocking(clientSocket);
  usleep(HALF_A_SECOND * 10);
  close(clientSocket);
  LOGGER.trace("finished processing incoming connection");
}

void GenericServer::listeningThreadFunction() {
  createReusableServerSocket();
  bindServerSocket();
  listenOnServerSocket();
  if(initComplete) {
    LOGGER.info("server is listening on port " + std::to_string(port) + ". Error: " + std::string(strerror(errno)));
  }
  
  struct epoll_event ev, events[MAX_EVENTS];
  int epollfd = epoll_create1(0);
  ev.events = EPOLLIN;
  ev.data.fd = serverSocket;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &ev);
  while(keepListeningThreadRunning.load()) {
    int countEvents = epoll_wait(epollfd, &ev, MAX_EVENTS, 1000);
    if(countEvents == -1) {
      LOGGER.error("error encountered during waiting for an event to occurre on the server socket: " + std::string(strerror(errno)));
    } else if (countEvents > 0) {
      for(int i = 0; i < countEvents; i++) {
        struct sockaddr_in clientAddress;
        socklen_t addressStructLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &addressStructLen);
        LOGGER.info("New connection from " + std::string(inet_ntoa(clientAddress.sin_addr)) + ":" + std::to_string(ntohs(clientAddress.sin_port)));
        std::future<void> workingThread = std::async(std::launch::async, &GenericServer::workingThreadFunction, this, clientSocket, std::string(inet_ntoa(clientAddress.sin_addr)));
        auto workingThreadPointer = std::make_shared<std::future<void>>(std::move(workingThread));
        addToListOfWorkingThreads(workingThreadPointer);
      }
    }
  }
}

void GenericServer::addToListOfWorkingThreads(std::shared_ptr<std::future<void>> &workingThreadPointer) {
  std::unique_lock<std::shared_mutex> lock(workingThreadsMutex);
  workingThreads.push_back(workingThreadPointer);
}

void GenericServer::handleListeningError(std::string message) {
    LOGGER.error(message);
    initComplete = false;
    setIsListening(false);
}

int GenericServer::getPort() {
    return port;
}

} // namespace event_forge