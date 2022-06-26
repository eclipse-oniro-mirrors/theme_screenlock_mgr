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

#ifndef MISCSERVICES_SCREENLOCK_DFX_HIDUMPER_ADAPTER_H
#define MISCSERVICES_SCREENLOCK_DFX_HIDUMPER_ADAPTER_H

#include <string>
#include <vector>
#include <list>
#include <functional>
#include "singleton.h"

namespace OHOS {
namespace ScreenLock {
class DumpHelper : public Singleton<DumpHelper> {
public:
    using DumpNoParamFunc = std::function<void(int)>;
    DumpHelper() = default;
    virtual ~DumpHelper() = default;
    void AddDumpOperation(const DumpNoParamFunc &dumpscreenlockInfo);
    void AddErrorInfo(const std::string &error);
    void ShowError(int fd);
    bool Dump(int fd, const std::vector<std::string> &args);
private:
    void ShowHelp(int fd);
    void ShowIllealInfomation(int fd);
    mutable std::mutex hidumperMutex_;
    std::list<std::string> g_errorInfo;
    DumpNoParamFunc dumpscreenlockInfo_;
};
}  // namespace ScreenLock
}  // namespace OHOS

#endif  // MISCSERVICES_SCREENLOCK_DFX_HIDUMPER_ADAPTER_H
