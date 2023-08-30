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

#include "async_call.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "node_api.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "string_ex.h"
#include "uv_queue.h"

namespace OHOS {
namespace ScreenLock {
enum class ARG_INFO { ARG_ERROR, ARG_DATA, ARG_BUTT };
constexpr const char *CANCEL_UNLOCK_OPERATION = "The user canceled the unlock operation.";
constexpr const char *SCREENLOCK_FAIL = "ScreenLock failed.";
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

void ScreenlockCallback::UvWorkOnCallBack(uv_work_t *work, int32_t status)
{
    if (work == nullptr) {
        SCLOCK_HILOGE("UvWorkOnCallBack, work is null");
        return;
    }
    ScreenlockOnCallBack *callBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (callBackPtr == nullptr) {
        SCLOCK_HILOGE("UvWorkOnCallBack, callBackPtr is null");
        SAFE_DELETE(work);
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(callBackPtr->env, &scope);
    napi_value result[ARGS_SIZE_TWO] = { 0 };
    bool screenLockSuccess = callBackPtr->screenLockResult == SCREEN_SUCC;
    if (callBackPtr->action == Action::UNLOCKSCREEN) {
        napi_get_undefined(callBackPtr->env, &result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
    } else {
        napi_get_boolean(callBackPtr->env, screenLockSuccess, &result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
    }
    if (screenLockSuccess) {
        napi_get_null(callBackPtr->env, &result[static_cast<int32_t>(ARG_INFO::ARG_ERROR)]);
    } else {
        AsyncCall::GenerateBusinessError(callBackPtr->env, callBackPtr->errorInfo,
            &result[static_cast<int32_t>(ARG_INFO::ARG_ERROR)]);
    }
    if (callBackPtr->deferred) {
        if (screenLockSuccess) {
            napi_resolve_deferred(callBackPtr->env, callBackPtr->deferred,
                result[static_cast<int32_t>(ARG_INFO::ARG_DATA)]);
        } else {
            napi_reject_deferred(callBackPtr->env, callBackPtr->deferred,
                result[static_cast<int32_t>(ARG_INFO::ARG_ERROR)]);
        }
    } else {
        napi_value callbackFunc = nullptr;
        napi_value callbackResult = nullptr;
        napi_get_reference_value(callBackPtr->env, callBackPtr->callbackRef, &callbackFunc);
        napi_call_function(callBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_TWO, result, &callbackResult);
        napi_delete_reference(callBackPtr->env, callBackPtr->callbackRef);
    }
    napi_close_handle_scope(callBackPtr->env, scope);
    SAFE_DELETE(callBackPtr);
    SAFE_DELETE(work);
    SCLOCK_HILOGI("UvWorkOnCallBack end");
}

void ScreenlockCallback::OnCallBack(const int32_t screenLockResult)
{
    ScreenlockOnCallBack *screenlockOnCallBack = new (std::nothrow) ScreenlockOnCallBack;
    if (screenlockOnCallBack == nullptr) {
        SCLOCK_HILOGE("new  ScreenlockOnCallBack failed");
        return;
    }
    if (screenLockResult == SCREEN_CANCEL) {
        errorInfo_.message_ = CANCEL_UNLOCK_OPERATION;
    } else if (screenLockResult == SCREEN_FAIL) {
        errorInfo_.message_ = SCREENLOCK_FAIL;
    }
    screenlockOnCallBack->env = eventListener_.env;
    screenlockOnCallBack->callbackRef = eventListener_.callbackRef;
    screenlockOnCallBack->deferred = eventListener_.deferred;
    screenlockOnCallBack->action = eventListener_.action;
    screenlockOnCallBack->errorInfo = errorInfo_;
    screenlockOnCallBack->screenLockResult = screenLockResult;
    bool bRet = UvQueue::Call(eventListener_.env, screenlockOnCallBack, UvWorkOnCallBack);
    if (!bRet) {
        SCLOCK_HILOGE("ScreenlockCallback::OnCallBack g_screenLockCallback failed");
    }
}
} // namespace ScreenLock
} // namespace OHOS
