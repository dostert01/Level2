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
#include "httplistener.h"

namespace event_forge {

HTTPListener::HTTPListener(json jsonObject) : GenericServer(jsonObject) {
};

HTTPListener::~HTTPListener() {
}

std::shared_ptr<HTTPListener> HTTPListener::getInstance() {
  return std::make_shared<HTTPListener>();
}

void HTTPListener::handleClientConnection(int clientSocket, std::string clientHost) {

}

} // namespace event_forge