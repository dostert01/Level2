#include <applicationDirectories.h>
#include <common.h>
#include <logger.h>

namespace level2 {

ApplicationDirectories::ApplicationDirectories() {
    applicationRoot = StaticStringFunctions::getCurrentWorkingDirectory().value_or(".");
    pipelines = "pipelines.d";
    processes = "processes.d";
    workerModules = "lib";
    allDirectoriesExist = false;
};

ApplicationDirectories::ApplicationDirectories(json jsonObject) {
    applicationRoot = StaticStringFunctions::replaceEnvVariablesInPath(jsonObject["root"]);
    pipelines = jsonObject["pipelines"];
    processes = jsonObject["processes"];
    workerModules = jsonObject["workerModules"];
    allDirectoriesExist = false;
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

bool ApplicationDirectories::createApplicationDirectories() {
    int i = createSingleDirectory(getApplicationRootDir());
    i += createSingleDirectory(getPipelinesDir());
    i += createSingleDirectory(getProcessesDir());
    i += createSingleDirectory(getWorkerModulesDir());
    return i == 0;
}

int ApplicationDirectories::createSingleDirectory(std::string path)
{
    int returnValue = 0;
    auto[success, message] = StaticFileFunctions::createDirectory(path);
    if(!success) {
        LOGGER.error(message);
        returnValue = 1;
    }
    return returnValue;
}

} // namespace level2