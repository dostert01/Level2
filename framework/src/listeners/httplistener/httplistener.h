#pragma once

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

#include "networklistener.h"

using namespace nlohmann::json_abi_v3_11_3;

namespace event_forge {

class HTTPListener : public NetworkListener {
    public:
        HTTPListener() = default;
        HTTPListener(json jsonObject);
        static std::shared_ptr<HTTPListener> getInstance();
        void init(std::shared_ptr<PipelineFiFo> fifo);
        void startListening();
        std::string getHostName();
        int getPort();
    private:
        int port = 8888;
};

} // namespace event_forge
