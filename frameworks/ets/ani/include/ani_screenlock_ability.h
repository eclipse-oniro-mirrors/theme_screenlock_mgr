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
#ifndef ANI_SCREENLOCK_ABILITY_H
#define ANI_SCREENLOCK_ABILITY_H

#include <cstdint>
#include <iostream>

#include "ani.h"
#include "screenlock_callback_interface.h"

namespace OHOS {
namespace ScreenLock {
void GetErrorInfo(int32_t errorCode, ErrorInfo &errorInfo);
std::string GetErrorMessage(const uint32_t &code);
ani_boolean ANI_IsScreenLocked(ani_env *env);
ani_boolean ANI_IsLocked(ani_env *env);
ani_boolean ANI_IsSecureMode(ani_env *env);
void ANI_UnlockScreen(ani_env *env);
ani_boolean ANI_Unlock(ani_env *env);
ani_boolean ANI_Lock(ani_env *env);
ani_boolean ANI_OnSystemEvent(ani_env *env, ani_ref callback);
ani_boolean ANI_SendScreenLockEvent(ani_env *env, ani_string event, ani_double parameter);
ani_boolean ANI_SetScreenLockDisabled(ani_env *env, ani_boolean disable, ani_double userId);
ani_boolean ANI_IsScreenLockDisabled(ani_env *env, ani_double userId);
ani_boolean ANI_SetScreenLockAuthState(ani_env *env, ani_enum_item state, ani_double userId, ani_object authToken);
ani_enum_item ANI_GetScreenLockAuthState(ani_env *env, ani_double userId);
ani_double ANI_GetStrongAuth(ani_env *env, ani_double userId);

} // namespace ScreenLock
} // namespace OHOS
#endif //  ANI_SCREENLOCK_ABILITY_H