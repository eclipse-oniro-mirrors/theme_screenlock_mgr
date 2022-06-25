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

#ifndef MISCSERVICES_SCREENLOCK_DFX_HITRACE_ADAPTER_H
#define MISCSERVICES_SCREENLOCK_DFX_HITRACE_ADAPTER_H

#include <string>
#include <ctime>
#include <sys/time.h>

namespace OHOS {
namespace MiscServicesDfx {
constexpr int64_t MICROSEC_TO_MILLISEC = 1000;
constexpr int64_t SEC_TO_MILLISEC = 1000;
uint64_t startTime_{ 0 };
void InitHiTrace();
void ValueTrace(const std::string& name, int64_t count);

class ScreenlockHiTraceAsyncTrace {
public:
    explicit ScreenlockHiTraceAsyncTrace(const std::string &value);
    virtual ~ScreenlockHiTraceAsyncTrace();
    uint64_t TimeConsuming()
    {
        struct timeval tv = { 0, 0 };
        gettimeofday(&tv, nullptr);
        uint64_t msecNeed = (tv.tv_sec * SEC_TO_MILLISEC) + (tv.tv_usec / MICROSEC_TO_MILLISEC);
        return msecNeed;
    }
};
} // MiscServicesDfx
} // OHOS
#endif // MISCSERVICES_SCREENLOCK_DFX_HITRACE_ADAPTER_H
