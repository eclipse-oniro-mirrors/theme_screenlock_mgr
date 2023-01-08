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
#include "screenlock_callback.h"

#include <cstdint>
#include <new>
#include <string>

#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi_screenlock_ability.h"
#include "node_api.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "string_ex.h"
#include "uv_queue.h"

namespace OHOS {
namespace ScreenLock {
enum class ARG_INFO { ARG_ERROR, ARG_DATA, ARG_BUTT };
ScreenlockCallback::ScreenlockCallback(const EventListener &eventListener)
{
    eventListener_ = eventListener;
}

ScreenlockCallback::~ScreenlockCallback()
{
}

void ScreenlockCallback::SetErrorInfo(const ErrorInfo &errorInfo)
{
    errorInfo_ = errorInfo;
}

void UvWorkOnCallBackInt(uv_work_t *work, int status)
{
    if (work == nullptr) {
        SCLOCK_HILOGE("UvWorkNotifyMissionChanged, work is null");
        return;
    }
    ScreenlockOnCallBack *callBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (callBackPtr == nullptr) {
        SCLOCK_HILOGE("UvWorkOnCallBackInt, callBackPtr is null");
        SAFE_DELETE(work);
        return;
    }
    int32_t onCallbackResult = -1;
    if (!StrToInt(callBackPtr->systemEvent.params_, onCallbackResult)) {
        SAFE_DELETE(callBackPtr);
        SAFE_DELETE(work);
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(callBackPtr->env, &scope);
    napi_value result[ARGS_SIZE_TWO] = { 0 };
    napi_get_undefined(callBackPtr->env, &result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
    if (onCallbackResult == SCREEN_SUCC) {
        napi_get_undefined(callBackPtr->env, &result[static_cast<int32_t>(ARG_INFO::ARG_ERROR)]);
        if (callBackPtr->callBackResult) {
            napi_get_boolean(callBackPtr->env, true, &result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
        }
    } else {
        AsyncCall::GenerateBusinessError(
            callBackPtr->env, callBackPtr->errorInfo, &result[static_cast<int32_t>(ARG_INFO::ARG_ERROR)]);
        if (callBackPtr->callBackResult) {
            napi_get_boolean(callBackPtr->env, false, &result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
        }
    }
    if (callBackPtr->deferred) {
        if (onCallbackResult == SCREEN_SUCC) {
            napi_resolve_deferred(
                callBackPtr->env, callBackPtr->deferred, result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
        } else {
            napi_reject_deferred(
                callBackPtr->env, callBackPtr->deferred, result[static_cast<int32_t>(ARG_INFO::ARG_ERROR)]);
        }
    } else {
        napi_value callbackFunc = nullptr;
        napi_value callbackResult = nullptr;
        napi_get_reference_value(callBackPtr->env, callBackPtr->callbackref, &callbackFunc);
        napi_call_function(callBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_TWO, result, &callbackResult);
        napi_delete_reference(callBackPtr->env, callBackPtr->callbackref);
    }
    napi_close_handle_scope(callBackPtr->env, scope);
    SAFE_DELETE(callBackPtr);
    SAFE_DELETE(work);
}

void ScreenlockCallback::OnCallBack(const SystemEvent &systemEvent)
{
    ScreenlockOnCallBack *screenlockOnCallBack = new (std::nothrow) ScreenlockOnCallBack;
    if (screenlockOnCallBack == nullptr) {
        SCLOCK_HILOGE("new  ScreenlockOnCallBack failed");
        return;
    }
    screenlockOnCallBack->env = eventListener_.env;
    screenlockOnCallBack->callbackref = eventListener_.callbackRef;
    screenlockOnCallBack->systemEvent = systemEvent;
    screenlockOnCallBack->deferred = eventListener_.deferred;
    screenlockOnCallBack->errorInfo = errorInfo_;
    screenlockOnCallBack->callBackResult = eventListener_.callBackResult;
    bool bRet = UvQueue::Call(eventListener_.env, static_cast<void *>(screenlockOnCallBack), UvWorkOnCallBackInt);
    if (!bRet) {
        SCLOCK_HILOGE("ScreenlockCallback::OnCallBack failed, event=%{public}s,result=%{public}s",
            systemEvent.eventType_.c_str(), systemEvent.params_.c_str());
    }
}
} // namespace ScreenLock
} // namespace OHOS
