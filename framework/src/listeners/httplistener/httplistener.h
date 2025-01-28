#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <shared_mutex>
#include <nlohmann/json.hpp>

#include "generic_server.h"

using namespace nlohmann::json_abi_v3_11_3;

namespace event_forge {

class HTTPListener : public GenericServer {
    public:
        HTTPListener() = default;
        HTTPListener(json jsonObject);
        ~HTTPListener();
        static std::shared_ptr<HTTPListener> getInstance();
    private:
        void handleClientConnection(int clientSocket, std::string clientHost);
};

} // namespace event_forge
