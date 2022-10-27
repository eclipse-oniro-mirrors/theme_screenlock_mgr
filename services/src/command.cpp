/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * miscservices under the License is miscservices on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "command.h"

namespace OHOS {
namespace ScreenLock {
Command::Command(const std::vector<std::string> &argsFormat, const std::string &help, const Command::Action &action)
    : format(argsFormat), help(help), action(action)
{
}

Command::Command(const std::vector<std::string> &argsFormat, const std::string &help) : format(argsFormat), help(help)
{
}

std::string Command::ShowHelp()
{
    return help;
}

bool Command::DoAction(const std::vector<std::string> &input, std::string &output)
{
    return action(input, output);
}

std::string Command::GetOption()
{
    return format.at(0);
}

std::string Command::GetFormat()
{
    std::string formatStr;
    for (const auto &seg : format) {
        formatStr += seg;
        formatStr += " ";
    }
    return formatStr;
}
} // namespace ScreenLock
} // namespace OHOS