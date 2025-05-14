#include <applicationDirectories.h>
#include <common.h>
#include <logger.h>

namespace level2 {

ApplicationDirectories::ApplicationDirectories() {
    applicationRoot = StaticStringFunctions::getCurrentWorkingDirectory().value_or(".");
    setInternalVariables("pipelines.d", "processes.d", "lib");
};

ApplicationDirectories::ApplicationDirectories(json jsonObject) {
    applicationRoot = std::filesystem::path(StaticStringFunctions::replaceEnvVariablesInPath(jsonObject["root"]));
    setInternalVariables(std::string(jsonObject["pipelines"]), std::string(jsonObject["processes"]), std::string(jsonObject["workerModules"]));
};

void ApplicationDirectories::setInternalVariables(const std::string& pipelinesDir, const std::string& processesDir, const std::string& workerModulesDir) {
    pipelines = std::filesystem::path(applicationRoot.string() + PATH_SEPARATOR + pipelinesDir);
    processes = std::filesystem::path(applicationRoot.string() + PATH_SEPARATOR + processesDir);
    workerModules = std::filesystem::path(applicationRoot.string() + PATH_SEPARATOR + workerModulesDir);
    allDirectoriesExist = false;
}

std::filesystem::path& ApplicationDirectories::getApplicationRootDir() {
    return applicationRoot;
}

std::filesystem::path& ApplicationDirectories::getPipelinesDir() {
    return pipelines;
}

std::filesystem::path& ApplicationDirectories::getProcessesDir() {
    return processes;
}

std::filesystem::path& ApplicationDirectories::getWorkerModulesDir() {
    return workerModules;
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