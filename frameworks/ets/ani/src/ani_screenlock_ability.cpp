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
#include <hitrace_meter.h>
#include <map>
#include <pthread.h>
#include <unistd.h>

#include "ani_screenlock_ability.h"
#include "ani_error_handler.h"
#include "ani_event_listener.h"
#include "ani_screenlock_callback.h"
#include "ani_screenlock_system_ability_callback.h"
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
constexpr const char *USER_ID_INVALID = "The userId is not same as the caller, and is not allowed for the caller.";

const std::map<int, uint32_t> ERROR_CODE_CONVERSION = {
    { E_SCREENLOCK_NO_PERMISSION, JsErrorCode::ERR_NO_PERMISSION },
    { E_SCREENLOCK_PARAMETERS_INVALID, JsErrorCode::ERR_INVALID_PARAMS },
    { E_SCREENLOCK_WRITE_PARCEL_ERROR, JsErrorCode::ERR_SERVICE_ABNORMAL },
    { E_SCREENLOCK_NULLPTR, JsErrorCode::ERR_SERVICE_ABNORMAL },
    { E_SCREENLOCK_SENDREQUEST_FAILED, JsErrorCode::ERR_SERVICE_ABNORMAL },
    { E_SCREENLOCK_NOT_FOCUS_APP, JsErrorCode::ERR_ILLEGAL_USE },
    { E_SCREENLOCK_NOT_SYSTEM_APP, JsErrorCode::ERR_NOT_SYSTEM_APP },
    { E_SCREENLOCK_USER_ID_INVALID, JsErrorCode::ERR_USER_ID_INVALID },
};
const std::map<uint32_t, std::string> ERROR_INFO_MAP = {
    { JsErrorCode::ERR_NO_PERMISSION, PERMISSION_VALIDATION_FAILED },
    { JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED },
    { JsErrorCode::ERR_CANCEL_UNLOCK, CANCEL_UNLOCK_OPERATION },
    { JsErrorCode::ERR_SERVICE_ABNORMAL, SERVICE_IS_ABNORMAL },
    { JsErrorCode::ERR_ILLEGAL_USE, ILLEGAL_USE },
    { JsErrorCode::ERR_NOT_SYSTEM_APP, NON_SYSTEM_APP },
    { JsErrorCode::ERR_USER_ID_INVALID, USER_ID_INVALID },
};

void GetErrorInfo(int32_t errorCode, ErrorInfo &errorInfo)
{
    std::map<int, uint32_t>::const_iterator iter = ERROR_CODE_CONVERSION.find(errorCode);
    if (iter != ERROR_CODE_CONVERSION.end()) {
        errorInfo.errorCode_ = iter->second;
        errorInfo.message_ = GetErrorMessage(errorInfo.errorCode_);
        SCLOCK_HILOGD("GetErrorInfo errorInfo.code: %{public}d, errorInfo.message: %{public}s",
            errorInfo.errorCode_,
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

std::string ANIUtils_ANIStringToStdString(ani_env *env, ani_string aniStr)
{
    SCLOCK_HILOGI("Call");
    ani_size strSize;
    if (env->String_GetUTF8Size(aniStr, &strSize) != ANI_OK) {
        SCLOCK_HILOGE("String_GetUTF8Size Failed");
        return "";
    }
    std::vector<char> buffer(strSize + 1);
    char *utf8Buffer = buffer.data();

    ani_size bytesWritten = 0;
    if (env->String_GetUTF8(aniStr, utf8Buffer, strSize + 1, &bytesWritten) != ANI_OK) {
        SCLOCK_HILOGE("String_GetUTF8 Failed");
        return "";
    }
    utf8Buffer[bytesWritten] = '\0';
    std::string content = std::string(utf8Buffer, bytesWritten);
    SCLOCK_HILOGI("End");
    return content;
}

ani_boolean ANI_IsScreenLocked(ani_env *env)
{
    SCLOCK_HILOGD("ANI_IsScreenLocked begin");
    int32_t status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("ANI_IsScreenLocked exec status = %{public}d ", status);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return false;
    }
    return true;
}

ani_boolean ANI_IsLocked(ani_env *env)
{
    SCLOCK_HILOGD("ANI_IsLocked begin");
    ani_boolean result = false;
    bool isLocked = false;
    int32_t status = ScreenLockManager::GetInstance()->IsLocked(isLocked);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    result = isLocked;
    return result;
}

ani_boolean ANI_IsSecureMode(ani_env *env)
{
    SCLOCK_HILOGD("ANI_IsSecureMode begin");
    int32_t status = ScreenLockManager::GetInstance()->GetSecure();
    SCLOCK_HILOGD("ANI_IsSecureMode exec status = %{public}d ", status);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return false;
    }
    return true;
}

void ANI_UnlockScreen(ani_env *env)
{
    SCLOCK_HILOGD("ANI_UnlockScreen begin");
    EventListener *eventListener = new (std::nothrow) EventListener{.env = env, .action = Action::UNLOCKSCREEN};
    if (eventListener == nullptr) {
            return;
        }
    sptr<ScreenlockCallback> callback = new (std::nothrow) ScreenlockCallback(*eventListener);
    if (callback == nullptr) {
            SCLOCK_HILOGE("ANI_UnlockScreen create callback object fail");
            return;
        }
    int32_t status = ScreenLockManager::GetInstance()->Unlock(eventListener->action, callback);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        callback->SetErrorInfo(errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
    }
    if (eventListener) {
        delete eventListener;
        eventListener = nullptr;
    }
}

ani_boolean ANI_Unlock(ani_env *env)
{
    SCLOCK_HILOGD("ANI_Unlock begin");
    EventListener *eventListener = new (std::nothrow) EventListener{.env = env, .action = Action::UNLOCK};
    if (eventListener == nullptr) {
            return;
        }
    sptr<ScreenlockCallback> callback = new (std::nothrow) ScreenlockCallback(*eventListener);
    if (callback == nullptr) {
            SCLOCK_HILOGE("ANI_Unlock create callback object fail");
            return;
        }
    int32_t status = ScreenLockManager::GetInstance()->Unlock(eventListener->action, callback);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        callback->SetErrorInfo(errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        if (eventListener) {
            delete eventListener;
            eventListener = nullptr;
        }
        return false;
    }
    if (eventListener) {
        delete eventListener;
        eventListener = nullptr;
    }
    return true;
}

ani_boolean ANI_Lock(ani_env *env)
{
    SCLOCK_HILOGD("ANI_Lock begin");
    EventListener *eventListener = new (std::nothrow) EventListener{.env = env, .action = Action::LOCK};
    if (eventListener == nullptr) {
            return;
        }
    sptr<ScreenlockCallback> callback = new (std::nothrow) ScreenlockCallback(*eventListener);
    if (callback == nullptr) {
            SCLOCK_HILOGE("ANI_Lock create callback object fail");
            return;
        }
    int32_t status = ScreenLockManager::GetInstance()->Lock(callback);
    SCLOCK_HILOGD("ANI_Lock exec status = %{public}d ", status);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        callback->SetErrorInfo(errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        if (eventListener) {
            delete eventListener;
            eventListener = nullptr;
        }
        return false;
    }
    if (eventListener) {
        delete eventListener;
        eventListener = nullptr;
    }
    return true;
}

ani_boolean ANI_OnSystemEvent(ani_env *env, ani_ref callback)
{
    SCLOCK_HILOGD("ANI_OnSystemEvent begin");
    bool status = false;

    ani_ref callbackRef;
    if (ANI_OK != env->GlobalReference_Create(callback, &callbackRef)) {
        SCLOCK_HILOGE("GlobalReference_Create failed");
        return status;
    }

    EventListener eventListener{ .env = env, .callbackRef = callbackRef };
    sptr<ScreenlockSystemAbilityCallback> listener = new (std::nothrow) ScreenlockSystemAbilityCallback(eventListener);
    if (listener != nullptr) {
        ScreenlockSystemAbilityCallback::GetEventHandler();
        int32_t retCode = ScreenLockManager::GetInstance()->OnSystemEvent(listener);
        SCLOCK_HILOGD("ANI_OnSystemEvent exec retCode = %{public}d ", retCode);
        if (retCode != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(retCode);
            GetErrorInfo(retCode, errInfo);
            ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
            status = false;
        } else {
            status = true;
        }
    }
    SCLOCK_HILOGD("on system event status=%{public}d", status);
    return status;
}

ani_boolean ANI_SendScreenLockEvent(ani_env *env, ani_string event, ani_double parameter)
{
    SCLOCK_HILOGD("ANI_SendScreenLockEvent begin");
    std::string stdEvent = ANIUtils_ANIStringToStdString(env, static_cast<ani_string>(event));
    if (stdEvent.empty()) {
        SCLOCK_HILOGE("ANIUtils_ANIStringToStdString convert failed");
        return false;
    }
    int32_t retCode = ScreenLockManager::GetInstance()->SendScreenLockEvent(stdEvent, parameter);
    if (retCode != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(retCode);
        GetErrorInfo(retCode, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return false;
    }
    return true;
}

ani_boolean ANI_SetScreenLockDisabled(ani_env *env, ani_boolean disable, ani_double userId)
{
    SCLOCK_HILOGD("ANI_SetScreenLockDisabled begin");
    int32_t retCode = ScreenLockManager::GetInstance()->SetScreenLockDisabled(disable, userId);
    if (retCode != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(retCode);
        GetErrorInfo(retCode, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return false;
    }
    return true;
}

ani_boolean ANI_IsScreenLockDisabled(ani_env *env, ani_double userId)
{
    SCLOCK_HILOGD("ANI_IsScreenLockDisabled begin");
    ani_boolean result = false;
    bool isDisabled = false;
    int32_t status = ScreenLockManager::GetInstance()->IsScreenLockDisabled(userId, isDisabled);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    SCLOCK_HILOGI("ANI_IsScreenLockDisabled [isDisabled]=%{public}d", isDisabled);
    result = isDisabled;
    return result;
}

ani_boolean ANI_SetScreenLockAuthState(ani_env *env, ani_enum_item state, ani_double userId, ani_object authToken)
{
    SCLOCK_HILOGD("ANI_SetScreenLockAuthState begin");
    ani_int stateInt;
    env->EnumItem_GetValue_Int(state, &stateInt);
    ani_string authTokenANIStr = static_cast<ani_string>(authToken);
    std::string authTokenStr = ANIUtils_ANIStringToStdString(env, authTokenANIStr);
    int32_t retCode = ScreenLockManager::GetInstance()->SetScreenLockAuthState(stateInt, userId, authTokenStr);
    if (retCode != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(retCode);
        GetErrorInfo(retCode, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return false;
    }
    return true;
}

ani_enum_item ANI_GetScreenLockAuthState(ani_env *env, ani_double userId)
{
    SCLOCK_HILOGD("ANI_GetScreenLockAuthState begin");
    ani_enum enumType;
    env->FindEnum("L@ohos/screenLock/screenLock/AuthState;", &enumType);
    ani_enum_item result = nullptr;
    int32_t authState = -1;
    int32_t status = ScreenLockManager::GetInstance()->GetScreenLockAuthState(userId, authState);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    SCLOCK_HILOGI("ANI_GetScreenLockAuthState [authState]=%{public}d", authState);
    env->Enum_GetEnumItemByIndex(enumType, ani_size(authState), &result);
    return result;
}

ani_double ANI_GetStrongAuth(ani_env *env, ani_double userId)
{
    SCLOCK_HILOGD("ANI_GetStrongAuth begin");
    ani_double result = 0;
    int32_t reasonFlag = -1;
    int32_t status = ScreenLockManager::GetInstance()->GetStrongAuth(userId, reasonFlag);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ErrorHandler::Throw(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    SCLOCK_HILOGI("ANI_GetStrongAuth [reasonFlag]=%{public}d", reasonFlag);
    result = ani_double(reasonFlag);
    return result;
}

} // namespace ScreenLock
} // namespace OHOS

static ani_boolean BindMethods(ani_env *env)
{
    const char *spaceName = "L@ohos/screenLock/screenLock;";
    ani_namespace spc;

    ani_status ret = env->FindNamespace(spaceName, &spc);
    if (ret != ANI_OK) {
        SCLOCK_HILOGE("Not found %{public}s, ret = %{public}d", spaceName, ret);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function{
            "isScreenLocked_inner", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_IsScreenLocked)},
        ani_native_function{"isLocked", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_IsLocked)},
        ani_native_function{
            "isSecureMode_inner", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_IsSecureMode)},
        ani_native_function{
            "unlockScreen_inner", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_UnlockScreen)},
        ani_native_function{"unlock_inner", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_Unlock)},
        ani_native_function{"lock_inner", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_Lock)},
        ani_native_function{"onSystemEvent", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_OnSystemEvent)},
        ani_native_function{
            "sendScreenLockEvent_inner", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_SendScreenLockEvent)},
        ani_native_function{"setScreenLockDisabled_inner",
            nullptr,
            reinterpret_cast<void *>(OHOS::ScreenLock::ANI_SetScreenLockDisabled)},
        ani_native_function{
            "isScreenLockDisabled", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_IsScreenLockDisabled)},
        ani_native_function{"setScreenLockAuthState_inner",
            nullptr,
            reinterpret_cast<void *>(OHOS::ScreenLock::ANI_SetScreenLockAuthState)},
        ani_native_function{
            "getScreenLockAuthState", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_GetScreenLockAuthState)},
        ani_native_function{"getStrongAuth", nullptr, reinterpret_cast<void *>(OHOS::ScreenLock::ANI_GetStrongAuth)}};

    if (env->Namespace_BindNativeFunctions(spc, methods.data(), methods.size()) != ANI_OK) {
        SCLOCK_HILOGE("Cannot bind native methods to %{public}s ", spaceName);
        return ANI_ERROR;
    }
    return ANI_OK;
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    SCLOCK_HILOGI("Call");
    if (vm == nullptr || result == nullptr) {
        SCLOCK_HILOGE("vm or result is nullptr");
        return ANI_ERROR;
    }

    ani_env *env = nullptr;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        SCLOCK_HILOGE("Unsupported ANI_VERSION_1");
        return ANI_OUT_OF_REF;
    }

    if (env == nullptr) {
        SCLOCK_HILOGE("env is nullptr");
        return ANI_ERROR;
    }
    if (BindMethods(env) != ANI_OK) {
        return ANI_ERROR;
    }
    *result = ANI_VERSION_1;
    SCLOCK_HILOGI("End");
    return ANI_OK;
}
}