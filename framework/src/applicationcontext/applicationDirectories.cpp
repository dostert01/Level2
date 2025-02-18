#include <applicationDirectories.h>
#include <common.h>

namespace level2 {

ApplicationDirectories::ApplicationDirectories(json jsonObject) {
    applicationRoot = common::replaceEnvVariablesInPath(jsonObject["root"]);
    pipelines = jsonObject["pipelines"];
    processes = jsonObject["processes"];
    workerModules = jsonObject["workerModules"];
};

std::string ApplicationDirectories::getApplicationRootDir() {
    return applicationRoot;
}

std::string ApplicationDirectories::getPipelinesDir() {
    return pipelines;
}

std::string ApplicationDirectories::getProcessesDir() {
    return processes;
}

std::string ApplicationDirectories::getWorkerModulesDir() {
    return workerModules;
}

} // namespace level2