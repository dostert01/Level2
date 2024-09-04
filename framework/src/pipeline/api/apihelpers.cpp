#include "apihelpers.h"
#include <iostream>

std::optional<std::string> getNamedArgument(LibInitData* initData, const std::string& argumentName) {    
    if(initData->namedArguments.contains(argumentName)) {
        std::string s = initData->namedArguments[argumentName];
        return initData->namedArguments[argumentName];
    }
    return nullptr;
}