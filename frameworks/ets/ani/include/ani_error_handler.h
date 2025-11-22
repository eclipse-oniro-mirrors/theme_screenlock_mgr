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

#ifndef ANI_ERROR_HANDLER_H
#define ANI_ERROR_HANDLER_H

#include <string>
#include <cstdint>

#include "ani.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
using namespace std;
constexpr const char *BUSINESS_ERROR_CLASS = "@ohos.base.BusinessError";
class ErrorHandler {
public:
    static ani_object CreateError(ani_env *env, int32_t code, const string &errMsg)
    {
        if (env == nullptr) {
            SCLOCK_HILOGE("Invalid env");
            return nullptr;
        }
        ani_class cls;
        if (ANI_OK != env->FindClass(BUSINESS_ERROR_CLASS, &cls)) {
            SCLOCK_HILOGE("Not found class BusinessError");
            return nullptr;
        }
        ani_method method;
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "iC{escompat.Error}:", &method)) {
            SCLOCK_HILOGE("Not found method of BusinessError");
            return nullptr;
        }
        ani_object error = WrapError(env, errMsg);
        if (error == nullptr) {
            SCLOCK_HILOGE("WrapError failed");
            return nullptr;
        }
        ani_object obj;
        if (env->Object_New(cls, method, &obj, code, error) != ANI_OK) {
            SCLOCK_HILOGE("Object_New error fail");
            return nullptr;
        }
        return obj;
    }

    static ani_status Throw(ani_env *env, int32_t code, const string &errMsg)
    {
        ani_object obj = CreateError(env, code, errMsg);
        if (obj == nullptr) {
            return ANI_ERROR;
        }
        return env->ThrowError(static_cast<ani_error>(obj));
    }

private:
    static ani_object WrapError(ani_env *env, const std::string &msg)
    {
        if (env == nullptr) {
            return nullptr;
        }
        ani_class cls = nullptr;
        ani_method method = nullptr;
        ani_object obj = nullptr;
        ani_string aniMsg = nullptr;
        if (env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg) != ANI_OK) {
            SCLOCK_HILOGE("StringToAniStr failed");
            return nullptr;
        }
        ani_ref undefRef;
        env->GetUndefined(&undefRef);
        ani_status status = env->FindClass("escompat.Error", &cls);
        if (status != ANI_OK) {
            SCLOCK_HILOGE("FindClass : %{public}d", status);
            return nullptr;
        }
        status = env->Class_FindMethod(cls, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:", &method);
        if (status != ANI_OK) {
            SCLOCK_HILOGE("Class_FindMethod : %{public}d", status);
            return nullptr;
        }
        status = env->Object_New(cls, method, &obj, aniMsg, undefRef);
        if (status != ANI_OK) {
            SCLOCK_HILOGE("Object_New : %{public}d", status);
            return nullptr;
        }
        return obj;
    }
};
}  // namespace ScreenLock
}  // namespace OHOS

#endif