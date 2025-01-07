#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include "applicationcontext.h"

using namespace event_forge;
using namespace std;

#define ENV_VAR_NAME "MY_TEST_VARIABLE"

TEST(ApplicationContext, AlwaysCreatesASingletonInstanceAsPointer) {
    ApplicationContext* ctx1 = ApplicationContext::getInstanceAsPointer();
    ApplicationContext* ctx2 = ApplicationContext::getInstanceAsPointer();
    EXPECT_EQ(ctx1, ctx2);
}

TEST(ApplicationContext, AlwaysCreatesASingletonInstanceAsReference) {
    ApplicationContext& ctx1 = ApplicationContext::getInstance();
    ApplicationContext& ctx2 = ApplicationContext::getInstance();
    EXPECT_EQ(ctx1, ctx2);
}

TEST(ApplicationContext, CanReadFromEnvironment) {
    setenv(ENV_VAR_NAME, "Hello World", 1);
    optional<string> value = APP_CONTEXT.readEnv(ENV_VAR_NAME);
    unsetenv(ENV_VAR_NAME);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ("Hello World", value);
}

TEST(ApplicationContext, ReturnsEmptyOptionalIfVariableIsNotDefined) {
    optional<string> value = APP_CONTEXT.readEnv(ENV_VAR_NAME);
    EXPECT_FALSE(value.has_value());
}

TEST(ApplicationContext, CanReadTheCurrentWorkingDirectory) {
    optional<string> value = APP_CONTEXT.getCurrentWorkingDirectory();
    EXPECT_TRUE(value.has_value());
}
