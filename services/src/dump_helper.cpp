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
#include <iostream>
#include <memory>

#include "command.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
DumpHelper &DumpHelper::GetInstance()
{
    static DumpHelper instance;
    return instance;
}

void DumpHelper::AddCmdProcess(Command &cmd)
{
    cmdHandler.insert(std::make_pair(cmd.GetOption(), cmd));
}

bool DumpHelper::Dump(int fd, const std::vector<std::string> &args)
{
    if (args.empty() || args.at(0) == "-h") {
        dprintf(fd, "\n%-15s  %-20s", "Option", "Description");
        for (auto &[key, handler] : cmdHandler) {
            dprintf(fd, "\n%-15s: %-20s", handler.GetFormat().c_str(), handler.ShowHelp().c_str());
        }
        return false;
    }

    auto handler = cmdHandler.find(args.at(0));
    if (handler != cmdHandler.end()) {
        std::string output;
        auto ret = handler->second.DoAction(args, output);
        if (!ret) {
            SCLOCK_HILOGE(" failed");
        }
        dprintf(fd, "\n%s", output.c_str());
        return ret;
    }
    return false;
}
} // namespace ScreenLock
} // namespace OHOS
