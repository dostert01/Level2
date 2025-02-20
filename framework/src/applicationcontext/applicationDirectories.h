#pragma once

#include <nlohmann/json.hpp>
#include <string>

using namespace nlohmann::json_abi_v3_11_3;
namespace level2 {

class ApplicationDirectories {
    public:
        ApplicationDirectories();
        ApplicationDirectories(json jsonObject);
        std::string getApplicationRootDir();
        std::string getPipelinesDir();
        std::string getProcessesDir();
        std::string getWorkerModulesDir();
        bool createApplicationDirectories();

       private:
        std::string applicationRoot;
        std::string pipelines;
        std::string processes;
        std::string workerModules;
        bool allDirectoriesExist;
        int createSingleDirectory(std::string path);
};

} // namespace level2
