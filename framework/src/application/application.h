#pragma once

#include "applicationcontext.h"
#include "listeners.h"
#include "applicationDirectories.h"

namespace level2 {
    class Application {
        public:
            Application(const std::string& configFilePath);
            static std::shared_ptr<Application> getInstance(const std::string& configFilePath);
            std::vector<std::shared_ptr<Listener>>& getListeners();
            std::map<std::string, std::shared_ptr<PipeLineProcessor>>& getProcesses();
            std::shared_ptr<ApplicationDirectories>& getApplicationDirectories();

        private:
            Application() = default;
            std::vector<std::shared_ptr<Listener>> listeners;
            std::map<std::string, std::shared_ptr<PipeLineProcessor>>processes;
            std::shared_ptr<ApplicationDirectories> applicationDirectories;
            void loadListeners();
            void loadProcesses();
            void configureApplicationDirectories();
            void initApplicationDirectories();

            template <typename ListenerType>
            void addListeners(const std::string& jsonPath) {
                auto foundListeners = APP_CONTEXT.createObjectsFromAppConfigJson<ListenerType>(jsonPath);
                for(auto& listener : foundListeners) {
                    if(!std::static_pointer_cast<Listener>(listener)->getName().empty())
                        listeners.push_back(std::move(listener));
                    else
                        LOGGER.error("Your app config json contains at least one listener without a 'name' attribute. "
                            "This listener will be skipped. Please fix you json by adding a 'name' to all listeners.");
                }
            }
    };
}