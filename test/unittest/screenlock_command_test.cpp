/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "screenlock_command_test.h"

#include <memory>

#include <command.h>
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;

void ScreenLockCommandTest::SetUpTestCase()
{
}

void ScreenLockCommandTest::TearDownTestCase()
{
}

void ScreenLockCommandTest::SetUp()
{
}

void ScreenLockCommandTest::TearDown()
{
}

/**
* @tc.name: SetScreenLockCommandTest001
* @tc.desc: do action
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockCommandTest, SetScreenLockCommandTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockCommandTest001");
    auto cmd = std::make_shared<Command>(std::vector<std::string>({ "-all" }), "Show all",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            output.append("SetScreenLockCommandTest001");
            return true;
        });
    std::string output("");
    cmd->DoAction(std::vector<std::string>{ "-all" }, output);
    bool isOk = output.compare("SetScreenLockCommandTest001") == 0;
    EXPECT_EQ(isOk, true);
}

/**
* @tc.name: SetScreenLockCommandTest002
* @tc.desc: get option.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockCommandTest, SetScreenLockCommandTest002, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockCommandTest002 begin");
    auto cmd = std::make_shared<Command>(std::vector<std::string>({ "-all" }), "Show all",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            output.append("SetScreenLockCommandTest002");
            return true;
        });
    std::string option = cmd->GetOption();
    bool isOk = option.compare("-all") == 0;
    EXPECT_EQ(isOk, true);
}

/**
* @tc.name: SetScreenLockCommandTest003
* @tc.desc: Test GetFormat
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockCommandTest, SetScreenLockCommandTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockCommandTest003 begin");
    auto cmd = std::make_shared<Command>(std::vector<std::string>({ "-all" }), "Show all",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            output.append("SetScreenLockCommandTest001");
            return true;
        });
    std::string format = cmd->GetFormat();
    bool isOk = format.compare("-all ") == 0;
    EXPECT_EQ(isOk, true);
}

/**
* @tc.name: SetScreenLockCommandTest004
* @tc.desc: Test GetFormat.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockCommandTest, SetScreenLockCommandTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockCommandTest004 begin");
    auto cmd = std::make_shared<Command>(std::vector<std::string>({ "-all" }), "Show all");
    std::string help = cmd->ShowHelp();
    bool isOk = help.compare("Show all") == 0;
    EXPECT_EQ(isOk, true);
}

} // namespace ScreenLock
} // namespace OHOS