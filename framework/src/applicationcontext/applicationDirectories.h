#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>

using namespace nlohmann::json_abi_v3_11_3;
namespace level2 {

class ApplicationDirectories {
    public:
        ApplicationDirectories();
        ApplicationDirectories(json jsonObject);
        std::filesystem::path& getApplicationRootDir();
        std::filesystem::path& getPipelinesDir();
        std::filesystem::path& getProcessesDir();
        std::filesystem::path& getWorkerModulesDir();
        bool createApplicationDirectories();

       private:
        std::filesystem::path applicationRoot;
        std::filesystem::path pipelines;
        std::filesystem::path processes;
        std::filesystem::path workerModules;
        bool allDirectoriesExist;
        int createSingleDirectory(const std::string path);
        void setInternalVariables(const std::string& pipelinesDir, const std::string& processesDir, const std::string& workerModulesDir);
};

} // namespace level2
