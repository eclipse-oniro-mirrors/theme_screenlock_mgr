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

#include "screenlock_dump_helper.h"

namespace OHOS {
namespace ScreenLock {
namespace {
constexpr int32_t MAX_RECORED_ERROR = 10;
constexpr int32_t SUB_CMD_NAME = 0;
constexpr int32_t SUB_CMD_PARAM = 1;
constexpr int32_t CMD_NO_PARAM = 1;
constexpr int32_t CMD_HAS_PARAM = 2;
constexpr const char *CMD_HELP = "-h";
constexpr const char *CMD_ALL = "all";
constexpr const char *CMD_ERROR_INFO = "-errorInfo";
constexpr const char *ILLEGAL_INFOMATION = "The arguments are illegal and you can enter '-h' for help.\n";
}

void DumpHelper::AddDumpOperation(const DumpNoParamFunc &dumpscreenlockInfo)
{
    if (dumpscreenlockInfo == nullptr) {
        return;
    }
    dumpscreenlockInfo_ = dumpscreenlockInfo;
}

void DumpHelper::AddErrorInfo(const std::string &error)
{
    std::lock_guard<std::mutex> lock(hidumperMutex_);
    if (g_errorInfo.size() + 1 > MAX_RECORED_ERROR) {
        g_errorInfo.pop_front();
        g_errorInfo.push_back(error);
    } else {
        g_errorInfo.push_back(error);
    }
}

void DumpHelper::ShowError(int fd)
{
    dprintf(fd, "The number of recent errors recorded is %d\n", g_errorInfo.size());
    int i = 0;
    for (const auto &it : g_errorInfo) {
        dprintf(fd, "Error ID: %d        ErrorInfo: %s\n", ++i, it.c_str());
    }
}

bool DumpHelper::Dump(int fd, const std::vector<std::string> &args)
{
    std::string command = "";
    std::string param = "";
    std::lock_guard<std::mutex> lock(hidumperMutex_);

    if (args.size() == CMD_NO_PARAM) {
        command = args.at(SUB_CMD_NAME);
    } else if (args.size() == CMD_HAS_PARAM) {
        command = args.at(SUB_CMD_NAME);
        param = args.at(SUB_CMD_PARAM);
    } else {
        ShowError(fd);
    }

    if (command == CMD_HELP) {
        ShowHelp(fd);
    } else if (command == CMD_ERROR_INFO) {
        ShowError(fd);
    } else if (command == CMD_ALL) {
        if (!dumpscreenlockInfo_) {
            return false;
        }
        dumpscreenlockInfo_(fd);
    } else {
        ShowIllealInfomation(fd);
    }
    return true;
}

void DumpHelper::ShowHelp(int fd)
{
    std::string result;
    result.append("Usage:dumper <command> [options]\n")
        .append("Description:\n")
        .append("all\t\t")
        .append("dump all screenlock information\n")
        .append("\t\tscreenLocked:\t\t")
        .append("whether there is lock screen status\n")
        .append("\t\tsystemReady:\t\t")
        .append("is the system in place\n")
        .append("\t\tscreenState:\t\t")
        .append("screen on / off status\n")
        .append("\t\toffReason:\t\t")
        .append("screen failure reason\n")
        .append("\t\tinteractiveState:\t")
        .append("screen interaction status\n");
    dprintf(fd, "%s\n", result.c_str());
}

void DumpHelper::ShowIllealInfomation(int fd)
{
    dprintf(fd, "%s\n", ILLEGAL_INFOMATION);
}
} // namespace ScreenLock
} // namespace OHOS
