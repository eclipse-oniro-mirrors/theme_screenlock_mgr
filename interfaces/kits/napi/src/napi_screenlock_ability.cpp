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
#include "screenlock_bundlename.h"
#include "screenlock_common.h"
#include "screenlock_js_util.h"
#include "screenlock_manager.h"
#include "screenlock_system_ability_callback.h"
#include "screenlock_unlock_callback.h"

using namespace OHOS;
using namespace OHOS::ScreenLock;

namespace OHOS {
namespace ScreenLock {
static thread_local uint32_t g_eventMasks = 0;
static thread_local std::list<EventListener> g_eventListenerList;
static thread_local EventListener g_unlockListener;

static bool AddEventListener(uint32_t eventType, const std::string &event)
{
    if ((eventType & g_eventMasks) == 0) {
        g_eventMasks += eventType;
        sptr<ScreenLockSystemAbilityInterface> listener =
            new ScreenlockSystemAbilityCallback(eventType, g_eventListenerList);
        if (listener != nullptr) {
            SCLOCK_HILOGD("Exec AddEventListener  observer--------》%{public}p", listener.GetRefPtr());
            return ScreenLockAppManager::GetInstance()->On(listener, event);
        }
    }
    return true;
}

napi_status Init(napi_env env, napi_value exports)
{
    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_FUNCTION("isScreenLocked", OHOS::ScreenLock::NAPI_IsScreenLocked),
        DECLARE_NAPI_FUNCTION("unlockScreen", OHOS::ScreenLock::NAPI_UnlockScreen),
        DECLARE_NAPI_FUNCTION("isSecureMode", OHOS::ScreenLock::NAPI_IsSecureMode),
        DECLARE_NAPI_FUNCTION("on", NAPI_On),
        DECLARE_NAPI_FUNCTION("off", NAPI_Off),
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
        type == SYSTEM_READY) {
        return true;
    }
    return false;
}

bool IsCheckedTypeSendEventMessage(const std::string &type)
{
    if (type == UNLOCK_SCREEN_RESULT || type == SCREEN_DRAWDONE) {
        return true;
    }
    return false;
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

napi_value NAPI_UnlockScreen(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_UnlockScreen begin");
    StartAsyncTrace(HITRACE_TAG_MISC, "NAPI_UnlockScreen start", HITTACE_UNLOCKSCREEN);
    napi_value ret = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
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
            g_unlockListener = {env, RESULT_ZERO, thisVar, callbackRef};
        }
    }
    if (callbackRef == nullptr) {
        SCLOCK_HILOGD("NAPI_UnlockScreen create promise");
        napi_deferred deferred;
        napi_create_promise(env, &deferred, &ret);
        g_unlockListener = {env, RESULT_ZERO, thisVar, nullptr, deferred};
    } else {
        SCLOCK_HILOGD("NAPI_UnlockScreen create callback");
        napi_get_undefined(env, &ret);
    }
    sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockUnlockCallback(g_unlockListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("NAPI_UnlockScreen create callback object fail");
        return ret;
    }
    ScreenLockManager::GetInstance()->RequestUnlock(listener);
    return ret;
}

napi_value NAPI_IsSecureMode(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_IsSecureMode begin");
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
        SCLOCK_HILOGD("exec ---- NAPI_IsSecureMode begin");
        context->allowed = ScreenLockManager::GetInstance()->GetSecure();
        SCLOCK_HILOGD("NAPI_IsSecureMode exec allowed = %{public}d ", context->allowed);
        context->status = napi_ok;
    };
    context->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(context), ARGS_SIZE_ZERO);
    return asyncCall.Call(env, exec);
}

napi_value NAPI_On(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_On in");
    napi_value result = nullptr;
    int callingUid = IPCSkeleton::GetCallingUid();
    SCLOCK_HILOGD("NAPI_On callingUid=%{public}d", callingUid);
    std::string bundleName = "";
    if (!ScreenLockBundleName::GetBundleNameByUid(callingUid, bundleName)) {
        return result;
    }
    if (bundleName.empty()) {
        SCLOCK_HILOGI("NAPI_On calling app is null");
        return result;
    }
    if (bundleName != BUNDLE_NAME) {
        SCLOCK_HILOGI("NAPI_On calling app is not Screenlock APP");
        return result;
    }
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGV_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, argc == ARGS_SIZE_TWO, "Wrong number of arguments, requires 2");
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_ZERO], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "type is not a string");
    char event[MAX_VALUE_LEN] = {0};
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[ARGV_ZERO], event, MAX_VALUE_LEN, &len);
    std::string type = event;
    SCLOCK_HILOGD("NAPI_On type : %{public}s", type.c_str());
    if (!IsCheckedTypeRegisterMessage(type)) {
        SCLOCK_HILOGD("NAPI_On type : %{public}s not support", type.c_str());
        return result;
    }
    valuetype = napi_undefined;
    napi_typeof(env, argv[ARGV_ONE], &valuetype);
    NAPI_ASSERT(env, valuetype == napi_function, "callback is not a function");
    napi_ref callbackRef = nullptr;
    napi_create_reference(env, argv[ARGV_ONE], 1, &callbackRef);
    SCLOCK_HILOGD("NAPI_On callbackRef = %{public}p", callbackRef);
    int32_t eventType = ScreenlockSystemAbilityCallback::GetEventType(type);
    if (eventType != NONE_EVENT_TYPE) {
        EventListener listener = {env, eventType, thisVar, callbackRef};
        SCLOCK_HILOGD("env 5555 = %{public}p", (void*)(env));
        SCLOCK_HILOGD("NAPI_On  type=%{public}s,callbackRef=%{public}p", type.c_str(), callbackRef);
        g_eventListenerList.push_back(listener);
    }
    bool status = AddEventListener(eventType, type);
    SCLOCK_HILOGD("NAPI_On  status=%{public}d", status);
    return result;
}

napi_value NAPI_Off(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_Off in");
    napi_value result = nullptr;
    int callingUid = IPCSkeleton::GetCallingUid();
    std::string bundleName = "";
    if (!ScreenLockBundleName::GetBundleNameByUid(callingUid, bundleName)) {
        return result;
    }
    if (bundleName.empty()) {
        SCLOCK_HILOGI("NAPI_Off calling app is null");
        return result;
    }
    if (bundleName != BUNDLE_NAME) {
        SCLOCK_HILOGI("NAPI_Off calling app is not Screenlock APP");
        return result;
    }
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGV_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, argc == ARGS_SIZE_TWO, "Wrong number of arguments, requires 1 or 2");
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_ZERO], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "type is not a string");
    char event[MAX_VALUE_LEN] = {0};
    size_t len;
    napi_get_value_string_utf8(env, argv[ARGV_ZERO], event, MAX_VALUE_LEN, &len);
    std::string type = event;
    SCLOCK_HILOGD("type : %{public}s", type.c_str());
    if (!IsCheckedTypeRegisterMessage(type)) {
        SCLOCK_HILOGD("type : %{public}s not support", type.c_str());
        return result;
    }
    int32_t eventType = ScreenlockSystemAbilityCallback::GetEventType(type);
    if (eventType == NONE_EVENT_TYPE) {
        return result;
    }
    for (std::list<EventListener>::iterator it = g_eventListenerList.begin(); it != g_eventListenerList.end(); ++it) {
        if (it->eventType == eventType) {
            ScreenLockAppManager::GetInstance()->Off(type);
            SCLOCK_HILOGD("Exec ObserverOff after RemoveStateObserver eventType = %{public}d", it->eventType);
        }
    }
    g_eventListenerList.remove_if(
        [eventType](EventListener listener) -> bool { return listener.eventType == eventType; });
    return result;
}

napi_value NAPI_ScreenLockSendEvent(napi_env env, napi_callback_info info)
{
    SCLOCK_HILOGD("NAPI_ScreenLockSendEvent begin");
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
        std::string type = event;
        if (!IsCheckedTypeSendEventMessage(type)) {
            SCLOCK_HILOGD("event : %{public}s not support", event);
            return napi_generic_failure;
        }
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
        context->allowed = ScreenLockAppManager::GetInstance()->SendScreenLockEvent(context->eventInfo, context->param);
        SCLOCK_HILOGD("NAPI_ScreenLockSendEvent exec allowed = %{public}d ", context->allowed);
        context->status = napi_ok;
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
