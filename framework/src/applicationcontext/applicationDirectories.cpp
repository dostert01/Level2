#include <applicationDirectories.h>
#include <common.h>

namespace level2 {

ApplicationDirectories::ApplicationDirectories(json jsonObject) {
    applicationRoot = StaticStringFunctions::replaceEnvVariablesInPath(jsonObject["root"]);
    pipelines = jsonObject["pipelines"];
    processes = jsonObject["processes"];
    workerModules = jsonObject["workerModules"];
};

std::string ApplicationDirectories::getApplicationRootDir() {
    return applicationRoot;
}

std::string ApplicationDirectories::getPipelinesDir() {
    return applicationRoot + PATH_SEPARATOR + pipelines;
}

std::string ApplicationDirectories::getProcessesDir() {
    return applicationRoot + PATH_SEPARATOR + processes;
}

std::string ApplicationDirectories::getWorkerModulesDir() {
    return applicationRoot + PATH_SEPARATOR + workerModules;
}

} // namespace level2