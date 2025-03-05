/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef STS_SCREENLOCK_ABILITY_H
#define STS_SCREENLOCK_ABILITY_H

#include <cstdint>
#include <string>
#include "ani.h"
#include "screenlock_callback_interface.h"
#include "event_listener.h"
#include "screenlock_callback.h"

namespace OHOS {
namespace ScreenLock {

void GetErrorInfo(int32_t errorCode, ErrorInfo &errorInfo);
std::string GetErrorMessage(const uint32_t &code);
ani_boolean ANI_IsLocked(ani_env* env, ani_object obj);

} // namespace ScreenLock
} // namespace OHOS
#endif //  STS_SCREENLOCK_ABILITY_H