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

#ifndef SCREENLOCK_DFX_DUMP_HELPER_H
#define SCREENLOCK_DFX_DUMP_HELPER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "command.h"

namespace OHOS {
namespace ScreenLock {
class DumpHelper {
public:
    static DumpHelper &GetInstance();
    void RegisterCommand(std::shared_ptr<Command> &cmd);
    bool Dispatch(int fd, const std::vector<std::string>& args);

private:
    std::map<std::string, std::shared_ptr<Command>> cmdHandler;
};
} // namespace ScreenLock
} // namespace OHOS

#endif // SCREENLOCK_DFX_DUMP_HELPER_H
