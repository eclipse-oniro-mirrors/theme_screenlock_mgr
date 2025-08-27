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
#ifndef NAPI_SCREENLOCK_ABILITY_H
#define NAPI_SCREENLOCK_ABILITY_H

#include "async_call.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ScreenLock {
struct AsyncScreenLockInfo : public AsyncCall::Context {
    napi_status status = napi_generic_failure;
    bool allowed;
    AsyncScreenLockInfo() : Context(nullptr, nullptr), allowed(false){};
    AsyncScreenLockInfo(InputAction input, OutputAction output)
        : Context(std::move(input), std::move(output)), allowed(false){};
    ~AsyncScreenLockInfo() override{};
};

struct SendEventInfo : public AsyncCall::Context {
    int32_t param;
    std::string eventInfo;
    bool flag;
    napi_status status;
    bool allowed;
    SendEventInfo()
        : Context(nullptr, nullptr), param(0), eventInfo(""), flag(false), status(napi_generic_failure),
          allowed(false){};
    SendEventInfo(InputAction input, OutputAction output)
        : Context(std::move(input), std::move(output)), param(0), eventInfo(""), flag(false),
          status(napi_generic_failure), allowed(false){};
    ~SendEventInfo() override{};
};

struct ScreenLockDisableInfo : public AsyncCall::Context {
    bool disable;
    int32_t userId;
    napi_status status;
    bool allowed;
    ScreenLockDisableInfo()
        : Context(nullptr, nullptr), disable(false), userId(-1), status(napi_generic_failure),
          allowed(false){};
    ScreenLockDisableInfo(InputAction input, OutputAction output)
        : Context(std::move(input), std::move(output)), disable(false), userId(-1),
          status(napi_generic_failure), allowed(false){};
    ~ScreenLockDisableInfo() override{};
};

struct ScreenLockAuthStatInfo : public AsyncCall::Context {
    int32_t userId;
    int32_t authState;
    std::string authToken;
    napi_status status;
    bool allowed;
    ScreenLockAuthStatInfo()
        : Context(nullptr, nullptr), userId(-1), authState(-1), authToken(""), status(napi_generic_failure),
          allowed(false){};
    ScreenLockAuthStatInfo(InputAction input, OutputAction output)
        : Context(std::move(input), std::move(output)), userId(-1), authState(-1), authToken(""),
          status(napi_generic_failure), allowed(false){};
    ~ScreenLockAuthStatInfo() override{};
};

struct ScreenLockStrongAuthInfo : public AsyncCall::Context {
    int32_t reasonFlag;
    int32_t userId;
    napi_status status;
    bool allowed;
    ScreenLockStrongAuthInfo()
        : Context(nullptr, nullptr), reasonFlag(-1), userId(-1), status(napi_generic_failure),
          allowed(false){};
    ScreenLockStrongAuthInfo(InputAction input, OutputAction output)
        : Context(std::move(input), std::move(output)), reasonFlag(-1), userId(-1),
          status(napi_generic_failure), allowed(false){};
    ~ScreenLockStrongAuthInfo() override{};
};

napi_status IsValidEvent(const std::string &type);
napi_status CheckParamNumber(size_t argc, std::uint32_t paramNumber);
napi_status CheckParamType(napi_env env, napi_value jsType, napi_status status);
void ThrowError(napi_env env, const uint32_t &code, const std::string &msg);
void GetErrorInfo(int32_t errorCode, ErrorInfo &errorInfo);
std::string GetErrorMessage(const uint32_t &code);
napi_status Init(napi_env env, napi_value exports);
napi_value NAPI_IsScreenLocked(napi_env env, napi_callback_info info);
napi_value NAPI_IsLocked(napi_env env, napi_callback_info info);
napi_value NAPI_UnlockScreen(napi_env env, napi_callback_info info);
napi_value NAPI_Unlock(napi_env env, napi_callback_info info);
napi_value NAPI_Lock(napi_env env, napi_callback_info info);
napi_value NAPI_IsSecureMode(napi_env env, napi_callback_info info);
napi_value NAPI_ScreenLockSendEvent(napi_env env, napi_callback_info info);
napi_value NAPI_OnSystemEvent(napi_env env, napi_callback_info info);
napi_value NAPI_IsScreenLockDisabled(napi_env env, napi_callback_info info);
napi_value NAPI_SetScreenLockDisabled(napi_env env, napi_callback_info info);
napi_value NAPI_SetScreenLockAuthState(napi_env env, napi_callback_info info);
napi_value NAPI_GetScreenLockAuthState(napi_env env, napi_callback_info info);
napi_value NAPI_RequestStrongAuth(napi_env env, napi_callback_info info);
napi_value NAPI_GetStrongAuth(napi_env env, napi_callback_info info);
napi_value NAPI_IsDeviceLocked(napi_env env, napi_callback_info info);
} // namespace ScreenLock
} // namespace OHOS
#endif //  NAPI_SCREENLOCK_ABILITY_H