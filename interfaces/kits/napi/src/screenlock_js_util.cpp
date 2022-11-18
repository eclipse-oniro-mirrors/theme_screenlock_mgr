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
#define LOG_TAG "ScreenLockJsUtil"

#include "screenlock_js_util.h"

#include <cstddef>
#include <new>
#include <string>

namespace OHOS::ScreenLock {
std::string ScreenLockJsUtil::Convert2String(const napi_env env, napi_value jsString)
{
    size_t maxLen = ScreenLockJsUtil::MAX_LEN;
    napi_status status = napi_get_value_string_utf8(env, jsString, nullptr, 0, &maxLen);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        maxLen = ScreenLockJsUtil::MAX_LEN;
    }
    if (maxLen == 0) {
        return std::string();
    }
    char *buf = new (std::nothrow) char[maxLen + 1];
    if (buf == nullptr) {
        return std::string();
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, jsString, buf, maxLen + 1, &len);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
    }
    buf[len] = 0;
    std::string value(buf);
    delete[] buf;
    return value;
}
} // namespace OHOS::ScreenLock