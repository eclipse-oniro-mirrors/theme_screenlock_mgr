/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_screenlock_ability.h"

#include <hitrace_meter.h>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include "ipc_skeleton.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "screenlock_manager.h"


using namespace OHOS;
using namespace OHOS::ScreenLock;

namespace OHOS {
namespace ScreenLock {
constexpr const char *PERMISSION_VALIDATION_FAILED = "Permission verification failed.";
constexpr const char *PARAMETER_VALIDATION_FAILED = "Parameter verification failed.";
constexpr const char *CANCEL_UNLOCK_OPERATION = "The user canceled the unlock operation.";
constexpr const char *SERVICE_IS_ABNORMAL = "The screenlock management service is abnormal.";
constexpr const char *ILLEGAL_USE = "Invalid use.";
constexpr const char *NON_SYSTEM_APP = "Permission verification failed, application which is not a system application "
                                       "uses system API.";
const std::map<int, uint32_t> ERROR_CODE_CONVERSION = {
    { E_SCREENLOCK_NO_PERMISSION, JsErrorCode::ERR_NO_PERMISSION },
    { E_SCREENLOCK_PARAMETERS_INVALID, JsErrorCode::ERR_INVALID_PARAMS },
    { E_SCREENLOCK_WRITE_PARCEL_ERROR, JsErrorCode::ERR_SERVICE_ABNORMAL },
    { E_SCREENLOCK_NULLPTR, JsErrorCode::ERR_SERVICE_ABNORMAL },
    { E_SCREENLOCK_SENDREQUEST_FAILED, JsErrorCode::ERR_SERVICE_ABNORMAL },
    { E_SCREENLOCK_NOT_FOCUS_APP, JsErrorCode::ERR_ILLEGAL_USE },
    { E_SCREENLOCK_NOT_SYSTEM_APP, JsErrorCode::ERR_NOT_SYSTEM_APP },
};
const std::map<uint32_t, std::string> ERROR_INFO_MAP = {
    { JsErrorCode::ERR_NO_PERMISSION, PERMISSION_VALIDATION_FAILED },
    { JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED },
    { JsErrorCode::ERR_CANCEL_UNLOCK, CANCEL_UNLOCK_OPERATION },
    { JsErrorCode::ERR_SERVICE_ABNORMAL, SERVICE_IS_ABNORMAL },
    { JsErrorCode::ERR_ILLEGAL_USE, ILLEGAL_USE },
    { JsErrorCode::ERR_NOT_SYSTEM_APP, NON_SYSTEM_APP },
};

void GetErrorInfo(int32_t errorCode, ErrorInfo &errorInfo)
{
    std::map<int, uint32_t>::const_iterator iter = ERROR_CODE_CONVERSION.find(errorCode);
    if (iter != ERROR_CODE_CONVERSION.end()) {
        errorInfo.errorCode_ = iter->second;
        errorInfo.message_ = GetErrorMessage(errorInfo.errorCode_);
        SCLOCK_HILOGD("GetErrorInfo errorInfo.code: %{public}d, errorInfo.message: %{public}s", errorInfo.errorCode_,
            errorInfo.message_.c_str());
    } else {
        SCLOCK_HILOGD("GetErrorInfo errCode: %{public}d", errorCode);
    }
}

std::string GetErrorMessage(const uint32_t &code)
{
    std::string message;
    std::map<uint32_t, std::string>::const_iterator iter = ERROR_INFO_MAP.find(code);
    if (iter != ERROR_INFO_MAP.end()) {
        message = iter->second;
    }
    SCLOCK_HILOGD("GetErrorMessage: message is %{public}s", message.c_str());
    return message;
}

static void ThrowError([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, std::string msg)
{
    static const char *errorClsName = "Lescompat/Error;";
    ani_class cls {};
    if (ANI_OK != env->FindClass(errorClsName, &cls)) {
        SCLOCK_HILOGE("Not found Lescompat/Error.");
        return;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;:V", &ctor)) {
        SCLOCK_HILOGE("Not found ctor.");
        return;
    }
    ani_object errorObject;
    ani_string error_msg;
    const char m[] = "error from native";
    env->String_NewUTF8(m, 17U, &error_msg);
    // todo.
    env->Object_New(cls, ctor, &errorObject, error_msg);
    env->ThrowError(static_cast<ani_error>(errorObject));
    return;
}

ani_boolean ANI_IsLocked(ani_env* env, ani_object obj)
{
    bool isLocked = false;
    int32_t status = ScreenLockManager::GetInstance()->IsLocked(isLocked);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ThrowError(env, obj, errInfo.message_);
        return true;
    }
    return isLocked;
}

static int32_t unlockexecute([[maybe_unused]]ani_env* env, [[maybe_unused]]ani_object obj, ani_object callback)
{
    ani_boolean isUndefined;
    env->Reference_IsUndefined(callback, &isUndefined);
    if (isUndefined) {
        SCLOCK_HILOGI("ANI_Unlock create promise.");
    }
    EventListener *eventListener = nullptr;
    eventListener = new (std::nothrow)
            EventListener{ .env = env, .callback = nullptr, .action = Action::UNLOCK };
    if (!eventListener) {
        SCLOCK_HILOGE("new object failed");
        return -1;
    }
    env->GlobalReference_Create(callback, &eventListener->callback);
    sptr<ScreenlockCallback> scallback = new (std::nothrow) ScreenlockCallback(*eventListener);
    int32_t status = ScreenLockManager::GetInstance()->Unlock(eventListener->action, scallback);
    SCLOCK_HILOGI("Unlock end. status:%{public}d", status);
    return 0;
}


ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    SCLOCK_HILOGI("ANI_Constructor call.");
    ani_env* env;
    ani_status status = ANI_ERROR;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        SCLOCK_HILOGE("Unsupported ANI_VERSION_1.");
        return ANI_ERROR;
    }
    ani_namespace kitNs;
    status = env->FindNamespace("Lsts_screenlock/ScreenLock;", &kitNs);
    if (ANI_OK != status) {
        SCLOCK_HILOGE("Not found Lsts_screenlock/ScreenLock.");
        return ANI_INVALID_ARGS;
    }

    std::array methods = {
        ani_native_function { "islocked", ":Z", reinterpret_cast<void*>(ANI_IsLocked) },
        ani_native_function { "unlockexecute", ":I", reinterpret_cast<void*>(unlockexecute) },
    };
    status = env->Namespace_BindNativeFunctions(kitNs, methods.data(), methods.size());
    if (ANI_OK != status) {
        SCLOCK_HILOGE("Cannot bind native methods in Lsts_screenlock/ScreenLock.");
        return ANI_INVALID_TYPE;
    };

    *result = ANI_VERSION_1;
    return ANI_OK;
}

}
}