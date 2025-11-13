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

#ifndef ANI_SCREENLOCK_UTIL_H
#define ANI_SCREENLOCK_UTIL_H

#include <string>
#include <cstdint>

#include "ani.h"

namespace OHOS {
namespace ScreenLock {
class AniScreenLockUtil {
public:
    static ani_object CreateBoolean(ani_env *env, ani_boolean value);
    static ani_vm *GetAniVm(ani_env *env);
    static ani_env *GetAniEnv(ani_vm *vm);
    static ani_env *AttachAniEnv(ani_vm *vm);
    static void DetachAniEnv(ani_vm *vm);
    static void GetNullLog(ani_env *env, ani_ref *result);
    static void GetUndefinedLog(ani_env *env, ani_ref *result);
};

} // namespace ScreenLock
} // namespace OHOS

#endif