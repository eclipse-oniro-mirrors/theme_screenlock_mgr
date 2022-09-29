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
#include <napi/native_api.h>
#include <pthread.h>
#include <unistd.h>
#include <uv.h>

#include "event_listener.h"
#include "ipc_skeleton.h"
#include "sclock_log.h"
#include "screenlock_app_manager.h"
#include "screenlock_common.h"
#include "screenlock_js_util.h"
#include "screenlock_manager.h"
#include "screenlock_system_ability_callback.h"
#include "screenlock_callback.h"

using namespace OHOS;
using namespace OHOS::ScreenLock;

namespace OHOS {
namespace ScreenLock {
static thread_local EventListener g_systemEventListener;
static thread_local EventListener g_unlockListener;
static thread_local EventListener g_lockListener;
napi_status Init(napi_env env, napi_value exports)
{
    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_FUNCTION("isScreenLocked", OHOS::ScreenLock::NAPI_IsScreenLocked),
        DECLARE_NAPI_FUNCTION("isLocked", OHOS::ScreenLock::NAPI_IsLocked),
        DECLARE_NAPI_FUNCTION("lock", OHOS::ScreenLock::NAPI_Lock),
        DECLARE_NAPI_FUNCTION("unlockScreen", OHOS::ScreenLock::NAPI_UnlockScreen),
        DECLARE_NAPI_FUNCTION("unlock", OHOS::ScreenLock::NAPI_Unlock),
        DECLARE_NAPI_FUNCTION("isSecureMode", OHOS::ScreenLock::NAPI_IsSecureMode),
        DECLARE_NAPI_FUNCTION("isSecure", OHOS::ScreenLock::NAPI_IsSecure),
        DECLARE_NAPI_FUNCTION("onSystemEvent", NAPI_OnSystemEvent),
        DECLARE_NAPI_FUNCTION("sendScreenLockEvent", OHOS::ScreenLock::NAPI_ScreenLockSendEvent),
        DECLARE_NAPI_FUNCTION("test_setScreenLocked", OHOS::ScreenLock::NAPI_TestSetScreenLocked),
        DECLARE_NAPI_FUNCTION("test_runtimeNotify", OHOS::ScreenLock::NAPI_TestRuntimeNotify),
        DECLARE_NAPI_FUNCTION("test_getRuntimeState", OHOS::ScreenLock::NAPI_TestGetRuntimeState),
    };
    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return napi_ok;
}

bool IsCheckedTypeRegisterMessage(const std::string &type)
{
    if (type == BEGIN_WAKEUP || type == END_WAKEUP || type == BEGIN_SCREEN_ON || type == END_SCREEN_ON ||
        type == BEGIN_SLEEP || type == END_SLEEP || type == BEGIN_SCREEN_OFF || type == END_SCREEN_OFF ||
        type == CHANGE_USER || type == SCREENLOCK_ENABLED || type == EXIT_ANIMATION || type == UNLOCKSCREEN ||
        type == SYSTEM_READY || type == LOCKSCREEN) {
        return true;
    }
    return false;
}

bool IsCheckedTypeSendEventMessage(napi_env env, const std::string &type)
{
    if (type == UNLOCK_SCREEN_RESULT || type == SCREEN_DRAWDONE || type == LOCK_SCREEN_RESULT) {
        return true;
    }
    std::string errMsg = EVENT_TYPE_NOT_SUPPORT;
    napi_throw_error(env, std::to_string(BussinessErrorCode::ERR_INVALID_PARAMS).c_str(), errMsg.c_str());
    SCLOCK_HILOGE("IsCheckedTypeSendEventMessage : %{public}s not support", type.c_str());
    return false;
}

bool CheckArgsType(napi_env env, bool isNoException, const std::string &type)
{
    if (!isNoException) {
        std::string errMsg = PARAMETER_TYPE_VALIDATION_FAILED + type;
        napi_throw_error(env, std::to_string(BussinessErrorCode::ERR_INVALID_PARAMS).c_str(), errMsg.c_str());
        SCLOCK_HILOGE("CheckArgsType:  %{public}s", errMsg.c_str());
        return false;
    }
    return true;
}

bool CheckArgsCount(napi_env env, bool isNoException, const std::string &argsCount)
{
    if (!isNoException) {
        std::string errMsg = PARAMETER_COUNT_VALIDATION_FAILED + argsCount;
        napi_throw_error(env, std::to_string(BussinessErrorCode::ERR_INVALID_PARAMS).c_str(), errMsg.c_str());
        SCLOCK_HILOGE("CheckArgsCount:  %{public}s", errMsg.c_str());
        return false;
    }
    return true;
}

std::string GetErrMessage(int32_t errorCode)
{
    std::string message;
    switch (errorCode) {
        case BussinessErrorCode::ERR_NO_PERMISSION:
            message = PERMISSION_VALIDATION_FAILED;
            break;
        case BussinessErrorCode::ERR_INVALID_PARAMS:
            message = PARAMETER_VALIDATION_FAILED;
            break;
        case BussinessErrorCode::ERR_CANCEL_UNLOCK:
            message = CANCEL_UNLOCK_OPENATION;
            break;
        case BussinessErrorCode::ERR_SERVICE_ABNORMAL:
            message = SERVICE_IS_ABNORMAL;
            break;
        default:
            break;
    }
    SCLOCK_HILOGE("GetErrMessage: message is %{public}s", message.c_str());
    return message;
}

napi_value NAPI_IsScreenLocked(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsScreenLocked begin");
    auto context = std::make_shared<AsyncScreenLockInfo>();
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
        SCLOCK_HILOGD("exec ---- NAPI_IsScreenLocked begin");
        context->allowed = ScreenLockManager::GetInstance()->IsScreenLocked();
        SCLOCK_HILOGD("NAPI_IsScreenLocked exec allowed = %{public}d ", context->allowed);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGS_SIZE_ZERO);
    return asyncCall.Call(env, exec);
}

napi_value NAPI_IsLocked(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsScreenLocked begin");
    napi_value result = nullptr;
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("isScreenlocked  status=%{public}d", status);
    napi_get_boolean(env, status, &result);
    return result;
}

void AsyncCallLockScreen(napi_env env)
{
    napi_async_work work;
    napi_value resource = nullptr;
    auto execute = [](napi_env env, void *data) {
        EventListener *eventListener = reinterpret_cast<EventListener *>(data);
        if (eventListener == nullptr) {
            return;
        }

        sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockCallback(*eventListener);
        if (listener == nullptr) {
            SCLOCK_HILOGE("NAPI_Lock create callback object fail");
            return;
        }
        int32_t status = ScreenLockManager::GetInstance()->RequestLock(listener);
        if (status != ERR_NONE) {
            ErrorInfo errInfo(status, GetErrMessage(status));
            SCLOCK_HILOGD("ScreenLockManager errInfo %{public}s", GetErrMessage(status).c_str());
            listener->SetErrorInfo(errInfo);
            SystemEvent systemEvent("", std::to_string(status));
            listener->OnCallBack(systemEvent);
        }
    };
    auto complete = [](napi_env env, napi_status status, void *data) {};
    napi_create_string_utf8(env, "AsyncCall", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, execute, complete, &g_lockListener, &work);
    napi_queue_async_work(env, work);
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

    napi_valuetype valueType = napi_undefined;
    if (argc == ARGS_SIZE_ONE) {
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        SCLOCK_HILOGD("NAPI_Lock callback");
        if (!CheckArgsType(env, valueType == napi_function, "function")) {
            return ret;
        }
        if (valueType == napi_function) {
            SCLOCK_HILOGD("NAPI_Lock create callback");
            napi_create_reference(env, argv[ARGV_ZERO], 1, &callbackRef);
            g_lockListener = { env, thisVar, callbackRef };
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_Lock create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        g_lockListener = { env, thisVar, nullptr, deferred };
    } else {
        SCLOCK_HILOGD("NAPI_Lock create callback");
        napi_get_undefined(env, &ret);
    }
    AsyncCallLockScreen(env);
    return ret;
}

void AsyncCallUnlockScreen(napi_env env)
{
    napi_async_work work;
    napi_value resource = nullptr;
    auto execute = [](napi_env env, void *data) {
        EventListener *eventListener = reinterpret_cast<EventListener *>(data);
        if (eventListener == nullptr) {
            SCLOCK_HILOGE("EventListener is nullptr");
            return;
        }
        sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockCallback(*eventListener);
        if (listener == nullptr) {
            SCLOCK_HILOGE("ScreenlockCallback create callback object fail");
            return;
        }
        int32_t status = ScreenLockManager::GetInstance()->RequestUnlock(listener);
        if (status != ERR_NONE) {
            ErrorInfo errInfo(status, GetErrMessage(status));
            listener->SetErrorInfo(errInfo);
            SystemEvent systemEvent("", std::to_string(status));
            listener->OnCallBack(systemEvent);
        }
    };
    auto complete = [](napi_env env, napi_status status, void *data) {};
    napi_create_string_utf8(env, "AsyncCall", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, execute, complete, &g_unlockListener, &work);
    napi_queue_async_work(env, work);
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

    napi_valuetype valueType = napi_undefined;
    if (argc == ARGS_SIZE_ONE) {
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        SCLOCK_HILOGD("NAPI_UnlockScreen callback");
        NAPI_ASSERT(env, valueType == napi_function, "callback is not a function");
        if (valueType == napi_function) {
            SCLOCK_HILOGD("NAPI_UnlockScreen create callback");
            napi_create_reference(env, argv[ARGV_ZERO], 1, &callbackRef);
            g_unlockListener = { env, thisVar, callbackRef };
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_UnlockScreen create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        g_unlockListener = { env, thisVar, nullptr, deferred };
    } else {
        SCLOCK_HILOGD("NAPI_UnlockScreen create callback");
        napi_get_undefined(env, &ret);
    }
    AsyncCallUnlockScreen(env);
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

    napi_valuetype valueType = napi_undefined;
    if (argc == ARGS_SIZE_ONE) {
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        SCLOCK_HILOGD("NAPI_Unlock callback");
        if (!CheckArgsType(env, valueType == napi_function, "function")) {
            return nullptr;
        }
        if (valueType == napi_function) {
            SCLOCK_HILOGD("NAPI_Unlock create callback");
            napi_create_reference(env, argv[ARGV_ZERO], 1, &callbackRef);
            g_unlockListener = { env, thisVar, callbackRef };
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_Unlock create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        g_unlockListener = { env, thisVar, nullptr, deferred };
    } else {
        SCLOCK_HILOGD("NAPI_Unlock create callback");
        napi_get_undefined(env, &ret);
    }
    AsyncCallUnlockScreen(env);
    return ret;
}

napi_value NAPI_IsSecureMode(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsSecureMode begin");
    auto context = std::make_shared<AsyncScreenLockInfo>();
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
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGS_SIZE_ZERO);
    return asyncCall.Call(env, exec);
}

napi_value NAPI_IsSecure(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsSecure begin");
    napi_value result = nullptr;
    bool status = ScreenLockManager::GetInstance()->GetSecure();
    SCLOCK_HILOGD("isSecureMode  status=%{public}d", status);
    napi_get_boolean(env, status, &result);
    return result;
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
    if (!CheckArgsCount(env, argc >= ARGS_SIZE_ONE, "one.")) {
        return result;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (!CheckArgsType(env, valueType == napi_function, "function")) {
        return result;
    }
    napi_ref callbackRef = nullptr;
    napi_create_reference(env, argv, ARGS_SIZE_ONE, &callbackRef);

    g_systemEventListener = { env, thisVar, callbackRef };
    sptr<ScreenLockSystemAbilityInterface> listener = new (std::nothrow)
        ScreenlockSystemAbilityCallback(g_systemEventListener);
    if (listener != nullptr) {
        SCLOCK_HILOGD("on system event,listener %{public}p", listener.GetRefPtr());
        status = ScreenLockAppManager::GetInstance()->OnSystemEvent(listener);
    }
    SCLOCK_HILOGD("on system event  status=%{public}d", status);
    napi_get_boolean(env, status, &result);
    return result;
}

napi_value NAPI_ScreenLockSendEvent(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_ScreenLockSendEvent begin");
    auto context = std::make_shared<SendEventInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        SCLOCK_HILOGD("input ---- argc : %{public}zu", argc);
        if (!CheckArgsCount(env, argc >= ARGS_SIZE_TWO, "two or three.")) {
            return napi_invalid_arg;
        }
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        if (!CheckArgsType(env, valueType == napi_string, "string")) {
            return napi_invalid_arg;
        }
        char event[MAX_VALUE_LEN] = { 0 };
        size_t len;
        napi_get_value_string_utf8(env, argv[ARGV_ZERO], event, MAX_VALUE_LEN, &len);
        context->eventInfo = event;
        std::string type = event;
        if (!IsCheckedTypeSendEventMessage(env, type)) {
            return napi_invalid_arg;
        }
        valueType = napi_undefined;
        napi_typeof(env, argv[ARGV_ONE], &valueType);
        if (!CheckArgsType(env, valueType == napi_number, "number")) {
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
        if (retCode != ERR_NONE) {
            ErrorInfo errInfo(retCode, GetErrMessage(retCode));
            context->SetErrorInfo(errInfo);
        } else {
            context->status = napi_ok;
        }
        context->allowed = retCode != 0 ? false : true;
        SCLOCK_HILOGD("NAPI_ScreenLockSendEvent exec allowed = %{public}d ", retCode);
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGV_TWO);
    return asyncCall.Call(env, exec);
}

napi_value NAPI_TestSetScreenLocked(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<SendEventInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(
            env, argc == ARGS_SIZE_ONE || argc == ARGS_SIZE_TWO, " should 1 or 2 parameters!", napi_invalid_arg);
        SCLOCK_HILOGD("input ---- argc : %{public}zu", argc);
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        NAPI_ASSERT_BASE(env, valueType == napi_boolean, "type is not a boolean type", napi_invalid_arg);
        napi_get_value_bool(env, argv[ARGV_ZERO], &context->flag);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        context->allowed = ScreenLockManager::GetInstance()->Test_SetScreenLocked(context->flag);
        SCLOCK_HILOGD("NAPI_TestSetScreenLocked exec allowed = %{public}d ", context->allowed);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGS_SIZE_ONE);
    return asyncCall.Call(env, exec);
}

napi_value NAPI_TestRuntimeNotify(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<SendEventInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(
            env, argc == ARGS_SIZE_TWO || argc == ARGS_SIZE_THREE, " should 2 or 3 parameters!", napi_invalid_arg);
        SCLOCK_HILOGD("input ---- argc : %{public}zu", argc);
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        NAPI_ASSERT_BASE(env, valueType == napi_string, "type is not a string type", napi_invalid_arg);
        char event[MAX_VALUE_LEN] = {0};
        size_t len;
        napi_get_value_string_utf8(env, argv[ARGV_ZERO], event, MAX_VALUE_LEN, &len);
        context->eventInfo = event;
        valueType = napi_undefined;
        napi_typeof(env, argv[ARGV_ONE], &valueType);
        NAPI_ASSERT_BASE(env, valueType == napi_number, "type is not a int type", napi_invalid_arg);
        napi_get_value_int32(env, argv[ARGV_ONE], &context->param);
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        context->allowed = ScreenLockManager::GetInstance()->Test_RuntimeNotify(context->eventInfo, context->param);
        SCLOCK_HILOGD("NAPI_TestRuntimeNotify exec allowed = %{public}d ", context->allowed);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGS_SIZE_TWO);
    return asyncCall.Call(env, exec);
}

napi_value NAPI_TestGetRuntimeState(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<SendEventInfo>();
    auto input = [context](napi_env env, size_t argc, napi_value argv[], napi_value self) -> napi_status {
        NAPI_ASSERT_BASE(
            env, argc == ARGS_SIZE_ONE || argc == ARGS_SIZE_TWO, " should 1 or 2 parameters!", napi_invalid_arg);
        SCLOCK_HILOGD("input ---- argc : %{public}zu", argc);
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[ARGV_ZERO], &valueType);
        NAPI_ASSERT_BASE(env, valueType == napi_string, "type is not a string type", napi_invalid_arg);
        char event[MAX_VALUE_LEN] = {0};
        size_t len;
        napi_get_value_string_utf8(env, argv[ARGV_ZERO], event, MAX_VALUE_LEN, &len);
        context->eventInfo = event;
        return napi_ok;
    };
    auto output = [context](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, context->allowed, result);
        SCLOCK_HILOGD("output ---- napi_get_boolean[%{public}d]", status);
        return napi_ok;
    };
    auto exec = [context](AsyncCall::Context *ctx) {
        context->allowed = ScreenLockManager::GetInstance()->Test_GetRuntimeState(context->eventInfo);
        SCLOCK_HILOGD("NAPI_TestGetRuntimeState exec allowed = %{public}d ", context->allowed);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGS_SIZE_ONE);
    return asyncCall.Call(env, exec);
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
    napi_module module = {.nm_version = 1, // NAPI v1
        .nm_flags = 0,                     // normal
        .nm_filename = nullptr,
        .nm_register_func = ScreenlockInit,
        .nm_modname = "screenLock",
        .nm_priv = nullptr,
        .reserved = {}};
    napi_module_register(&module);
}
} // namespace ScreenLock
} // namespace OHOS
