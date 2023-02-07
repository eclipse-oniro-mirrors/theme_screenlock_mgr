/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "screenlock_system_ability_callback.h"

#include <memory>
#include <new>

#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi_screenlock_ability.h"
#include "node_api.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "uv_queue.h"

namespace OHOS {
namespace ScreenLock {
ScreenlockSystemAbilityCallback::ScreenlockSystemAbilityCallback(const EventListener &eventListener)
    : eventListener_(eventListener)
{
}

ScreenlockSystemAbilityCallback::~ScreenlockSystemAbilityCallback()
{
}

auto g_onUvWorkCallback  = [](uv_work_t *work, int status) {
    SCLOCK_HILOGD("g_onUvWorkCallback  status = %{public}d", status);
    if (work == nullptr) {
        return;
    }
    ScreenlockOnCallBack *screenlockOnCallBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (screenlockOnCallBackPtr == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(screenlockOnCallBackPtr->env, &scope);
    napi_value undefined = nullptr;
    napi_get_undefined(screenlockOnCallBackPtr->env, &undefined);
    napi_value callbackFunc = nullptr;
    napi_get_reference_value(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->callbackref, &callbackFunc);
    napi_value callbackResult = nullptr;
    napi_value callbackValues[ARGS_SIZE_TWO] = { 0 };
    napi_get_undefined(screenlockOnCallBackPtr->env, &callbackValues[0]);

    napi_value result = nullptr;
    napi_create_object(screenlockOnCallBackPtr->env, &result);
    napi_value eventType = nullptr;
    napi_value params = nullptr;
    napi_create_string_utf8(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->systemEvent.eventType_.c_str(),
        NAPI_AUTO_LENGTH, &eventType);
    napi_create_string_utf8(
        screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->systemEvent.params_.c_str(), NAPI_AUTO_LENGTH, &params);
    napi_set_named_property(screenlockOnCallBackPtr->env, result, "eventType", eventType);
    napi_set_named_property(screenlockOnCallBackPtr->env, result, "params", params);
    callbackValues[1] = result;
    napi_call_function(
        screenlockOnCallBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_TWO, callbackValues, &callbackResult);
    napi_close_handle_scope(screenlockOnCallBackPtr->env, scope);
    if (screenlockOnCallBackPtr != nullptr) {
        delete screenlockOnCallBackPtr;
        screenlockOnCallBackPtr = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
};

void ScreenlockSystemAbilityCallback::OnCallBack(const SystemEvent &systemEvent)
{
    SCLOCK_HILOGD("ScreenlockSystemAbilityCallback  ONCALLBACK");
    ScreenlockOnCallBack *screenlockOnCallBack = new (std::nothrow) ScreenlockOnCallBack;
    if (screenlockOnCallBack == nullptr) {
        return;
    }
    screenlockOnCallBack->env = eventListener_.env;
    screenlockOnCallBack->callbackref = eventListener_.callbackRef;
    screenlockOnCallBack->thisVar = eventListener_.thisVar;
    screenlockOnCallBack->systemEvent = systemEvent;
    bool bRet = UvQueue::Call(eventListener_.env, static_cast<void *>(screenlockOnCallBack), g_onUvWorkCallback);
    if (!bRet) {
        SCLOCK_HILOGE("ScreenlockCallback::OnCallBack failed, event=%{public}s,result=%{public}s",
            systemEvent.eventType_.c_str(), systemEvent.params_.c_str());
    }
}
} // namespace ScreenLock
} // namespace OHOS
