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
#include "napi_screenlock_ability.h"

#include <hitrace_meter.h>
#include <map>
#include <napi/native_api.h>
#include <pthread.h>
#include <unistd.h>
#include <uv.h>

#include "event_listener.h"
#include "ipc_skeleton.h"
#include "sclock_log.h"
#include "screenlock_app_manager.h"
#include "screenlock_callback.h"
#include "screenlock_common.h"
#include "screenlock_js_util.h"
#include "screenlock_manager.h"
#include "screenlock_system_ability_callback.h"

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

napi_status Init(napi_env env, napi_value exports)
{
    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_FUNCTION("isScreenLocked", OHOS::ScreenLock::NAPI_IsScreenLocked),
        DECLARE_NAPI_FUNCTION("isLocked", OHOS::ScreenLock::NAPI_IsLocked),
        DECLARE_NAPI_FUNCTION("lock", OHOS::ScreenLock::NAPI_Lock),
        DECLARE_NAPI_FUNCTION("unlockScreen", OHOS::ScreenLock::NAPI_UnlockScreen),
        DECLARE_NAPI_FUNCTION("unlock", OHOS::ScreenLock::NAPI_Unlock),
        DECLARE_NAPI_FUNCTION("isSecureMode", OHOS::ScreenLock::NAPI_IsSecureMode),
        DECLARE_NAPI_FUNCTION("onSystemEvent", NAPI_OnSystemEvent),
        DECLARE_NAPI_FUNCTION("sendScreenLockEvent", OHOS::ScreenLock::NAPI_ScreenLockSendEvent),
        DECLARE_NAPI_FUNCTION("isScreenLockDisabled", OHOS::ScreenLock::NAPI_IsScreenLockDisabled),
        DECLARE_NAPI_FUNCTION("setScreenLockDisabled", OHOS::ScreenLock::NAPI_SetScreenLockDisabled),
        DECLARE_NAPI_FUNCTION("setScreenLockAuthState", OHOS::ScreenLock::NAPI_SetScreenLockAuthState),
        DECLARE_NAPI_FUNCTION("getScreenLockAuthState", OHOS::ScreenLock::NAPI_GetScreenLockAuthState),
        DECLARE_NAPI_FUNCTION("requestStrongAuth", OHOS::ScreenLock::NAPI_RequestStrongAuth),
        DECLARE_NAPI_FUNCTION("getStrongAuth", OHOS::ScreenLock::NAPI_GetStrongAuth),
    };
    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return napi_ok;
}

napi_status IsValidEvent(const std::string &type)
{
    if (type == UNLOCK_SCREEN_RESULT || type == SCREEN_DRAWDONE || type == LOCK_SCREEN_RESULT) {
        return napi_ok;
    }
    return napi_invalid_arg;
}

napi_status CheckParamType(napi_env env, napi_value param, napi_valuetype jsType)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_typeof(env, param, &valueType);
    if (status != napi_ok || valueType != jsType) {
        return napi_invalid_arg;
    }
    return napi_ok;
}

napi_status CheckParamArrayType(napi_env env, napi_value param, napi_typedarray_type jsType)
{
    size_t length = 0;
    void *data = nullptr;
    napi_typedarray_type type = napi_biguint64_array;
    napi_value inputBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, param, &type, &length, &data, &inputBuffer, &byteOffset);
    if (type != jsType || data == nullptr) {
        SCLOCK_HILOGE("napi_get_typedarray_info err");
        return napi_invalid_arg;
    }
    return napi_ok;
}

napi_status CheckParamNumber(size_t argc, std::uint32_t paramNumber)
{
    if (argc < paramNumber) {
        return napi_invalid_arg;
    }
    return napi_ok;
}

void ThrowError(napi_env env, const uint32_t &code, const std::string &msg)
{
    SCLOCK_HILOGD("ThrowError start");
    napi_value message;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH, &message));
    napi_value error;
    NAPI_CALL_RETURN_VOID(env, napi_create_error(env, nullptr, message, &error));
    napi_value errorCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, code, &errorCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, error, "code", errorCode));
    NAPI_CALL_RETURN_VOID(env, napi_throw(env, error));
    SCLOCK_HILOGD("ThrowError end");
}

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

napi_value NAPI_IsScreenLocked(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsScreenLocked begin");
    AsyncScreenLockInfo *context = new AsyncScreenLockInfo();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(
            env, argc == ARGS_SIZE_ZERO || argc == ARGS_SIZE_ONE, " should 0 or 1 parameters!", napi_invalid_arg);
        SCLOCK_HILOGD("input ---- argc : %{public}zu", argc);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        context->allowed = ScreenLockManager::GetInstance()->IsScreenLocked();
        SCLOCK_HILOGD("NAPI_IsScreenLocked exec allowed = %{public}d ", context->allowed);
        context->SetStatus(napi_ok);
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context, ARGS_SIZE_ZERO);
    return asyncCall.Call(env, exec, "isScreenLocked");
}

napi_value NAPI_IsLocked(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool isLocked = false;
    int32_t status = ScreenLockManager::GetInstance()->IsLocked(isLocked);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ThrowError(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    napi_get_boolean(env, isLocked, &result);
    return result;
}

static void CompleteAsyncWork(napi_env env, napi_status status, void *data)
{
    EventListener *eventListener = reinterpret_cast<EventListener *>(data);
    if (eventListener == nullptr) {
        return;
    }
    if (eventListener->work != nullptr) {
        napi_delete_async_work(env, eventListener->work);
    }
    delete eventListener;
}

void AsyncCallFunc(napi_env env, EventListener *listener, const std::string &resourceName)
{
    napi_value resource = nullptr;
    auto execute = [](napi_env env, void *data) {
        EventListener *eventListener = reinterpret_cast<EventListener *>(data);
        if (eventListener == nullptr) {
            return;
        }

        sptr<ScreenlockCallback> callback = new (std::nothrow) ScreenlockCallback(*eventListener);
        if (callback == nullptr) {
            SCLOCK_HILOGE("NAPI_Lock create callback object fail");
            if (eventListener->callbackRef != nullptr) {
                napi_delete_reference(env, eventListener->callbackRef);
            }
            return;
        }
        int32_t status = 0;
        if (eventListener->action == Action::LOCK) {
            status = ScreenLockManager::GetInstance()->Lock(callback);
        } else if (eventListener->action == Action::UNLOCK || eventListener->action == Action::UNLOCKSCREEN) {
            status = ScreenLockManager::GetInstance()->Unlock(eventListener->action, callback);
        }
        if (status != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(status);
            GetErrorInfo(status, errInfo);
            callback->SetErrorInfo(errInfo);
            callback->OnCallBack(status);
        }
    };
    std::string name = "THEME_" + resourceName;
    napi_create_string_utf8(env, name.c_str(), NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource, execute, CompleteAsyncWork, static_cast<void *>(listener), &(listener->work));
    auto ret = napi_queue_async_work_with_qos(env, listener->work, napi_qos_user_initiated);
    if (ret != napi_ok) {
        CompleteAsyncWork(env, ret, listener);
        NAPI_CALL_RETURN_VOID(env, ret);
    }
}

napi_value NAPI_Lock(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_Lock begin");
    napi_value ret = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    napi_ref callbackRef = nullptr;
    EventListener *eventListener = nullptr;
    if (argc == ARGS_SIZE_ONE) {
        SCLOCK_HILOGD("NAPI_Lock callback");
        if (CheckParamType(env, argv[ARGV_ZERO], napi_function) != napi_ok) {
            std::string errMsg = "Parameter error. The type of \"callback\" must be function";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return ret;
        }
        SCLOCK_HILOGD("NAPI_Lock create callback");
        napi_create_reference(env, argv[ARGV_ZERO], 1, &callbackRef);
        eventListener = new (std::nothrow)
            EventListener{ .env = env, .thisVar = thisVar, .callbackRef = callbackRef, .action = Action::LOCK };

        if (eventListener == nullptr) {
            SCLOCK_HILOGE("eventListener is nullptr");
            return nullptr;
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_Lock create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        eventListener = new (std::nothrow)
            EventListener{ .env = env, .thisVar = thisVar, .deferred = deferred, .action = Action::LOCK };

        if (eventListener == nullptr) {
            SCLOCK_HILOGE("eventListener is nullptr");
            return nullptr;
        }
    } else {
        SCLOCK_HILOGD("NAPI_Lock create callback");
        napi_get_undefined(env, &ret);
    }
    AsyncCallFunc(env, eventListener, "lock");
    return ret;
}

napi_value NAPI_UnlockScreen(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_UnlockScreen begin");
    StartAsyncTrace(HITRACE_TAG_MISC, "NAPI_UnlockScreen start", HITRACE_UNLOCKSCREEN);
    napi_value ret = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, argc == ARGS_SIZE_ZERO || argc == ARGS_SIZE_ONE, "Wrong number of arguments, requires one");
    napi_ref callbackRef = nullptr;
    EventListener *eventListener = nullptr;
    napi_valuetype valueType = napi_undefined;
    if (argc == ARGS_SIZE_ONE) {
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        SCLOCK_HILOGD("NAPI_UnlockScreen callback");
        NAPI_ASSERT(env, valueType == napi_function, "callback is not a function");
        SCLOCK_HILOGD("NAPI_UnlockScreen create callback");
        napi_create_reference(env, argv[ARGV_ZERO], 1, &callbackRef);
        eventListener = new (std::nothrow)
            EventListener{ .env = env, .thisVar = thisVar, .callbackRef = callbackRef, .action = Action::UNLOCKSCREEN };
        if (eventListener == nullptr) {
            SCLOCK_HILOGE("eventListener is nullptr");
            return nullptr;
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_UnlockScreen create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        eventListener = new (std::nothrow)
            EventListener{ .env = env, .thisVar = thisVar, .deferred = deferred, .action = Action::UNLOCKSCREEN };
        if (eventListener == nullptr) {
            SCLOCK_HILOGE("eventListener is nullptr");
            return nullptr;
        }
    } else {
        SCLOCK_HILOGD("NAPI_UnlockScreen create callback");
        napi_get_undefined(env, &ret);
    }
    AsyncCallFunc(env, eventListener, "unLockScreen");
    return ret;
}

napi_value NAPI_Unlock(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_Unlock begin");
    napi_value ret = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    napi_ref callbackRef = nullptr;
    EventListener *eventListener = nullptr;
    if (argc == ARGS_SIZE_ONE) {
        SCLOCK_HILOGD("NAPI_Unlock callback");
        if (CheckParamType(env, argv[ARGV_ZERO], napi_function) != napi_ok) {
            std::string errMsg = "Parameter error. The type of \"callback\" must be function";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return nullptr;
        }
        SCLOCK_HILOGD("NAPI_Unlock create callback");
        napi_create_reference(env, argv[ARGV_ZERO], 1, &callbackRef);
        eventListener = new (std::nothrow)
            EventListener{ .env = env, .thisVar = thisVar, .callbackRef = callbackRef, .action = Action::UNLOCK };
        if (eventListener == nullptr) {
            SCLOCK_HILOGE("eventListener is nullptr");
            return nullptr;
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_Unlock create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        eventListener = new (std::nothrow)
            EventListener{ .env = env, .thisVar = thisVar, .deferred = deferred, .action = Action::UNLOCK };
        if (eventListener == nullptr) {
            SCLOCK_HILOGE("eventListener is nullptr");
            return nullptr;
        }
    } else {
        SCLOCK_HILOGD("NAPI_Unlock create callback");
        napi_get_undefined(env, &ret);
    }
    AsyncCallFunc(env, eventListener, "unLock");
    return ret;
}

napi_value NAPI_IsSecureMode(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsSecureMode begin");
    AsyncScreenLockInfo *context = new AsyncScreenLockInfo();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        SCLOCK_HILOGD("input ---- argc : %{public}zu", argc);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        SCLOCK_HILOGD("exec ---- NAPI_IsSecureMode begin");
        context->allowed = ScreenLockManager::GetInstance()->GetSecure();
        SCLOCK_HILOGD("NAPI_IsSecureMode exec allowed = %{public}d ", context->allowed);
        context->SetStatus(napi_ok);
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context, ARGS_SIZE_ZERO);
    return asyncCall.Call(env, exec, "isSecureMode");
}

napi_value NAPI_OnSystemEvent(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_OnSystemEvent in");
    napi_value result = nullptr;
    bool status = false;
    napi_get_boolean(env, status, &result);
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    if (CheckParamNumber(argc, ARGS_SIZE_ONE) != napi_ok) {
        std::string errMsg = "Parameter error. The number of parameters should 1";
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
        return result;
    }
    if (CheckParamType(env, argv, napi_function) != napi_ok) {
        std::string errMsg = "Parameter error. The type of \"callback\" must be function";
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
        return result;
    }
    napi_ref callbackRef = nullptr;
    napi_create_reference(env, argv, ARGS_SIZE_ONE, &callbackRef);
    EventListener eventListener{ .env = env, .thisVar = thisVar, .callbackRef = callbackRef };
    sptr<ScreenlockSystemAbilityCallback> listener = new (std::nothrow) ScreenlockSystemAbilityCallback(eventListener);
    if (listener != nullptr) {
        ScreenlockSystemAbilityCallback::GetEventHandler();
        int32_t retCode = ScreenLockAppManager::GetInstance()->OnSystemEvent(listener);
        if (retCode != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(retCode);
            GetErrorInfo(retCode, errInfo);
            ThrowError(env, errInfo.errorCode_, errInfo.message_);
            status = false;
        } else {
            status = true;
        }
    }
    SCLOCK_HILOGD("on system event  status=%{public}d", status);
    napi_get_boolean(env, status, &result);
    return result;
}

napi_value NAPI_ScreenLockSendEvent(napi_env env, napi_callback_info info)
{
    SendEventInfo *context = new SendEventInfo();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        if (CheckParamNumber(argc, ARGS_SIZE_TWO) != napi_ok) {
            std::string errMsg = "Parameter error. The number of parameters should 2";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return napi_invalid_arg;
        }
        if (CheckParamType(env, argv[ARGV_ZERO], napi_string) != napi_ok) {
            std::string errMsg = "Parameter error. The type of \"event\" must be string";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return napi_invalid_arg;
        }
        char event[MAX_VALUE_LEN] = { 0 };
        size_t len;
        napi_get_value_string_utf8(env, argv[ARGV_ZERO], event, MAX_VALUE_LEN, &len);
        context->eventInfo = event;
        if (IsValidEvent(context->eventInfo) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        if (CheckParamType(env, argv[ARGV_ONE], napi_number) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        napi_get_value_int32(env, argv[ARGV_ONE], &context->param);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        int32_t retCode = ScreenLockAppManager::GetInstance()->SendScreenLockEvent(context->eventInfo, context->param);
        if (retCode != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(retCode);
            GetErrorInfo(retCode, errInfo);
            context->SetErrorInfo(errInfo);
            context->allowed = false;
        } else {
            context->SetStatus(napi_ok);
            context->allowed = true;
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context, ARGV_TWO);
    return asyncCall.Call(env, exec, "screenLockSendEvent");
}

napi_value NAPI_IsScreenLockDisabled(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsScreenLockDisabled in");
    napi_value result = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    int userId = -1;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (CheckParamNumber(argc, ARGS_SIZE_ONE) != napi_ok) {
        std::string errMsg = "Parameter error. The number of parameters should 1";
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
        return result;
    }
    if (CheckParamType(env, argv[ARGV_ZERO], napi_number) != napi_ok) {
        std::string errMsg = "Parameter error. The type of \"userId\" must be number";
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
        return result;
    }
    napi_get_value_int32(env, argv[ARGV_ZERO], &userId);
    bool isDisabled = false;
    int32_t status = ScreenLockAppManager::GetInstance()->IsScreenLockDisabled(userId, isDisabled);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ThrowError(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    SCLOCK_HILOGI("NAPI_IsScreenLockDisabled [isDisabled]=%{public}d", isDisabled);
    napi_get_boolean(env, isDisabled, &result);
    return result;
}

napi_value NAPI_SetScreenLockDisabled(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_SetScreenLockDisabled begin");
    ScreenLockDisableInfo *context = new ScreenLockDisableInfo();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        if (CheckParamNumber(argc, ARGS_SIZE_TWO) != napi_ok) {
            std::string errMsg = "Parameter error. The number of parameters should be at least 2";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return napi_invalid_arg;
        }
        if (CheckParamType(env, argv[ARGV_ZERO], napi_boolean) != napi_ok) {
            std::string errMsg = "Parameter error. The type of \"disable\" must be boolean";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return napi_invalid_arg;
        }
        napi_get_value_bool(env, argv[ARGV_ZERO], &context->disable);
        if (CheckParamType(env, argv[ARGV_ONE], napi_number) != napi_ok) {
            std::string errMsg = "Parameter error. The type of \"userId\" must be number";
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, errMsg);
            return napi_invalid_arg;
        }
        napi_get_value_int32(env, argv[ARGV_ONE], &context->userId);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        int32_t retCode = ScreenLockAppManager::GetInstance()->SetScreenLockDisabled(context->disable, context->userId);
        if (retCode != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(retCode);
            GetErrorInfo(retCode, errInfo);
            context->SetErrorInfo(errInfo);
            context->allowed = false;
        } else {
            context->SetStatus(napi_ok);
            context->allowed = true;
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context, ARGV_TWO);
    return asyncCall.Call(env, exec, "setScreenLockDisabled");
}

napi_value NAPI_SetScreenLockAuthState(napi_env env, napi_callback_info info)
{
    ScreenLockAuthStatInfo *context = new ScreenLockAuthStatInfo();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        if (CheckParamNumber(argc, ARGS_SIZE_THREE) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        if (CheckParamType(env, argv[ARGV_ZERO], napi_number) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        napi_get_value_int32(env, argv[ARGV_ZERO], &context->authState);

        if (CheckParamType(env, argv[ARGV_ONE], napi_number) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        napi_get_value_int32(env, argv[ARGV_ONE], &context->userId);

        if (CheckParamArrayType(env, argv[ARGV_TWO], napi_uint8_array) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        char authToken[MAX_VALUE_LEN] = { 0 };
        size_t len;
        napi_get_value_string_utf8(env, argv[ARGV_TWO], authToken, MAX_VALUE_LEN, &len);
        context->authToken = authToken;

        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        int32_t retCode = ScreenLockAppManager::GetInstance()->SetScreenLockAuthState(context->authState,
            context->userId, context->authToken);
        if (retCode != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(retCode);
            GetErrorInfo(retCode, errInfo);
            context->SetErrorInfo(errInfo);
            context->allowed = false;
        } else {
            context->SetStatus(napi_ok);
            context->allowed = true;
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context, ARGS_SIZE_THREE);
    return asyncCall.Call(env, exec, "setScreenLockDisabled");
}

napi_value NAPI_GetScreenLockAuthState(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_GetScreenLockAuthState in");
    napi_value result = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    int userId = -1;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (CheckParamNumber(argc, ARGS_SIZE_ONE) != napi_ok) {
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
        return result;
    }
    if (CheckParamType(env, argv[ARGV_ZERO], napi_number) != napi_ok) {
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
        return result;
    }
    napi_get_value_int32(env, argv[ARGV_ZERO], &userId);
    int32_t authState = -1;
    int32_t status = ScreenLockAppManager::GetInstance()->GetScreenLockAuthState(userId, authState);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ThrowError(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    SCLOCK_HILOGI("NAPI_GetScreenLockAuthState [authState]=%{public}d", authState);
    napi_create_int32(env, authState, &result);
    return result;
}

napi_value NAPI_RequestStrongAuth(napi_env env, napi_callback_info info)
{
    ScreenLockStrongAuthInfo *context = new ScreenLockStrongAuthInfo();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        if (CheckParamNumber(argc, ARGS_SIZE_TWO) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        if (CheckParamType(env, argv[ARGV_ZERO], napi_number) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        napi_get_value_int32(env, argv[ARGV_ZERO], &context->reasonFlag);

        if (CheckParamType(env, argv[ARGV_ONE], napi_number) != napi_ok) {
            ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
            return napi_invalid_arg;
        }
        napi_get_value_int32(env, argv[ARGV_ONE], &context->userId);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        int32_t retCode = ScreenLockAppManager::GetInstance()->RequestStrongAuth(context->reasonFlag,
            context->userId);
        if (retCode != E_SCREENLOCK_OK) {
            ErrorInfo errInfo;
            errInfo.errorCode_ = static_cast<uint32_t>(retCode);
            GetErrorInfo(retCode, errInfo);
            context->SetErrorInfo(errInfo);
            context->allowed = false;
        } else {
            context->SetStatus(napi_ok);
            context->allowed = true;
        }
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, context, ARGS_SIZE_THREE);
    return asyncCall.Call(env, exec, "requestStrongAuth");
}

napi_value NAPI_GetStrongAuth(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_GetStrongAuth in");
    napi_value result = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    int userId = -1;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (CheckParamNumber(argc, ARGS_SIZE_ONE) != napi_ok) {
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
        return result;
    }
    if (CheckParamType(env, argv[ARGV_ZERO], napi_number) != napi_ok) {
        ThrowError(env, JsErrorCode::ERR_INVALID_PARAMS, PARAMETER_VALIDATION_FAILED);
        return result;
    }
    napi_get_value_int32(env, argv[ARGV_ZERO], &userId);
    int32_t reasonFlag = -1;
    int32_t status = ScreenLockAppManager::GetInstance()->GetStrongAuth(userId, reasonFlag);
    if (status != E_SCREENLOCK_OK) {
        ErrorInfo errInfo;
        errInfo.errorCode_ = static_cast<uint32_t>(status);
        GetErrorInfo(status, errInfo);
        ThrowError(env, errInfo.errorCode_, errInfo.message_);
        return result;
    }
    SCLOCK_HILOGI("NAPI_GetStrongAuth [reasonFlag]=%{public}d", reasonFlag);
    napi_create_int32(env, reasonFlag, &result);
    return result;
}

static napi_value ScreenlockInit(napi_env env, napi_value exports)
{
    napi_status ret = Init(env, exports);
    if (ret != napi_ok) {
        SCLOCK_HILOGE("ModuleInit failed!");
    }
    return exports;
}

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module module = { .nm_version = 1, // NAPI v1
        .nm_flags = 0,                      // normal
        .nm_filename = nullptr,
        .nm_register_func = ScreenlockInit,
        .nm_modname = "screenLock",
        .nm_priv = nullptr,
        .reserved = {} };
    napi_module_register(&module);
}
} // namespace ScreenLock
} // namespace OHOS