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

#include "dump_helper.h"

#include <cstdio>

namespace OHOS {
namespace ScreenLock {
DumpHelper &DumpHelper::GetInstance()
{
    static DumpHelper instance;
    return instance;
}

void DumpHelper::RegisterCommand(std::shared_ptr<Command> &cmd)
{
    cmdHandler.insert(std::make_pair(cmd->GetOption(), cmd));
}

bool DumpHelper::Dispatch(int fd, const std::vector<std::string> &args)
{
    dprintf(fd, "\n---------------------------------\n");
    if (args.empty() || args.at(0) == "-h") {
        dprintf(fd, "\n%-15s  %-20s", "Option", "Description");
        for (auto &[key, handler] : cmdHandler) {
            dprintf(fd, "\n%-15s: %-20s\n", handler->GetFormat().c_str(), handler->ShowHelp().c_str());
        }
        return false;
    }

    auto handler = cmdHandler.find(args.at(0));
    if (handler != cmdHandler.end()) {
        std::string output;
        handler->second->DoAction(args, output);
        dprintf(fd, "%s\n", output.c_str());
        return true;
    }
    return false;
}
} // namespace ScreenLock
} // namespace OHOS
