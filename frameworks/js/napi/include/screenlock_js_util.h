/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef SCREENLOCK_JS_UTIL_H
#define SCREENLOCK_JS_UTIL_H

#include <cstdint>
#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS::ScreenLock {
class ScreenLockJsUtil {
public:
    static constexpr int32_t MAX_LEN = 4096;
    static std::string Convert2String(const napi_env env, napi_value jsString);
};
} // namespace OHOS::ScreenLock
#endif // SCREENLOCK_JS_UTIL_H
