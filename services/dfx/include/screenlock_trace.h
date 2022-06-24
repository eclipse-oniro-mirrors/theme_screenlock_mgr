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

namespace OHOS {
namespace MiscServicesDfx {
void InitHiTrace();
void ValueTrace(const std::string& name, int64_t count);

class ScreenlockHiTraceAsyncTrace {
public:
    explicit ScreenlockHiTraceAsyncTrace(const std::string &value);
    virtual ~ScreenlockHiTraceAsyncTrace();
};
} // MiscServicesDfx
} // OHOS
#endif // MISCSERVICES_SCREENLOCK_DFX_HITRACE_ADAPTER_H
