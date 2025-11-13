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

#include "ani_screenlock_util.h"

#include "ani.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
constexpr const char* CLASSNAME_BOOL = "Lstd/core/Boolean;";

ani_object AniScreenLockUtil::CreateBoolean(ani_env *env, ani_boolean value)
{
    if (env == nullptr) {
        SCLOCK_HILOGE("CreateBoolean null env");
        return nullptr;
    }
    ani_status status = ANI_ERROR;
    ani_class cls = nullptr;
    if ((status = env->FindClass(CLASSNAME_BOOL, &cls)) != ANI_OK) {
        SCLOCK_HILOGE("CreateBoolean FindClass status: %{public}d", status);
        return nullptr;
    }
    ani_method ctor = nullptr;
    if ((status = env->Class_FindMethod(cls, "<ctor>", "Z:V", &ctor)) != ANI_OK) {
        SCLOCK_HILOGE("CreateBoolean FindMethod status: %{public}d", status);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((status = env->Object_New(cls, ctor, &obj, value)) != ANI_OK) {
        SCLOCK_HILOGE("CreateBoolean Object_New status: %{public}d", status);
        return nullptr;
    }
    return obj;
}

ani_vm* AniScreenLockUtil::GetAniVm(ani_env *env)
{
    ani_vm* vm = nullptr;
    if (env->GetVM(&vm) != ANI_OK) {
        SCLOCK_HILOGE("GetVM failed");
        return nullptr;
    }
    return vm;
}

ani_env* AniScreenLockUtil::GetAniEnv(ani_vm *vm)
{
    ani_env* env = nullptr;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        SCLOCK_HILOGE("GetEnv failed");
        return nullptr;
    }
    return env;
}

ani_env* AniScreenLockUtil::AttachAniEnv(ani_vm *vm)
{
    ani_env *workerEnv = nullptr;
    ani_options aniArgs {0, nullptr};
    if (vm->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &workerEnv) != ANI_OK) {
        SCLOCK_HILOGE("Attach Env failed");
        return nullptr;
    }
    return workerEnv;
}

void AniScreenLockUtil::DetachAniEnv(ani_vm *vm)
{
    if (vm->DetachCurrentThread() != ANI_OK) {
        SCLOCK_HILOGE("Detach Env failed");
        return;
    }
}

void AniScreenLockUtil::GetNullLog(ani_env *env, ani_ref *result)
{
    if (env->GetNull(result) != ANI_OK) {
        SCLOCK_HILOGE("GetNull failed");
        return;
    }
}

void AniScreenLockUtil::GetUndefinedLog(ani_env *env, ani_ref *result)
{
    if (env->GetUndefined(result) != ANI_OK) {
        SCLOCK_HILOGE("GetNull failed");
        return;
    }
}

} // namespace ScreenLock
} // namespace OHOS