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
#include "screenlock_unlock_callback.h"

#include <uv.h>

#include "sclock_log.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
ScreenlockUnlockCallback::ScreenlockUnlockCallback(const EventListener &eventListener)
{
    unlockListener_ = &eventListener;
}

ScreenlockUnlockCallback::~ScreenlockUnlockCallback()
{
}

void ScreenlockUnlockCallback::OnCallBack(const std::string &event, bool result)
{
}

void ScreenlockUnlockCallback::OnCallBack(const std::string &event)
{
}

void UvWorkOnCallBackInt(uv_work_t *work, int status)
{
    SCLOCK_HILOGD("UvWorkOnCallBackInt begin");
    if (work == nullptr) {
        SCLOCK_HILOGD("UvWorkNotifyMissionChanged, work is null");
        return;
    }
    ScreenlockOnCallBack *screenlockOnCallBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (screenlockOnCallBackPtr == nullptr) {
        SCLOCK_HILOGD("UvWorkOnCallBackInt, screenlockOnCallBackPtr is null");
        delete work;
        return;
    }
    napi_value isResult = 0;
    if (screenlockOnCallBackPtr->deferred) {
        napi_get_undefined(screenlockOnCallBackPtr->env, &isResult);
        if (screenlockOnCallBackPtr->intCallbackValue == UNLOCKSCREEN_SUCC) {
            napi_resolve_deferred(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->deferred, isResult);
        } else {
            napi_reject_deferred(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->deferred, isResult);
        }
    } else {
        SCLOCK_HILOGD("unlock callback style");
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->callbackref, &callbackFunc);
        napi_value callbackResult = nullptr;
        napi_value callBackValue[ARGS_SIZE_TWO] = {0};
        if (screenlockOnCallBackPtr->intCallbackValue == UNLOCKSCREEN_SUCC) {
            napi_get_undefined(screenlockOnCallBackPtr->env, &callBackValue[0]);
            napi_create_int32(screenlockOnCallBackPtr->env,
                static_cast<int32_t>(screenlockOnCallBackPtr->intCallbackValue), &callBackValue[1]);
        } else {
            const char *str = "UnlockScreen failed";
            napi_create_string_utf8(screenlockOnCallBackPtr->env, str, strlen(str), &callBackValue[0]);
            napi_get_undefined(screenlockOnCallBackPtr->env, &callBackValue[1]);
        }
        napi_call_function(
            screenlockOnCallBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_TWO, callBackValue, &callbackResult);
    }
    if (screenlockOnCallBackPtr != nullptr) {
        delete screenlockOnCallBackPtr;
        screenlockOnCallBackPtr = nullptr;
    }
    if (work != nullptr) {
        delete work;
    }
    SCLOCK_HILOGD("UvWorkOnCallBackInt end");
}

void ScreenlockUnlockCallback::OnCallBack(const std::string &event, int result)
{
    SCLOCK_HILOGD("event=%{public}s,result=%{public}d", event.c_str(), result);
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(unlockListener_->env, &loop);
    if (loop == nullptr) {
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        return;
    }
    ScreenlockOnCallBack *screenlockOnCallBack = new (std::nothrow) ScreenlockOnCallBack;
    if (screenlockOnCallBack == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    screenlockOnCallBack->env = unlockListener_->env;
    screenlockOnCallBack->callbackref = unlockListener_->callbackRef;
    screenlockOnCallBack->intCallbackValue = result;
    screenlockOnCallBack->thisVar = unlockListener_->thisVar;
    screenlockOnCallBack->deferred = unlockListener_->deferred;
    work->data = (void *)screenlockOnCallBack;
    int rev = uv_queue_work(
        loop, work, [](uv_work_t *work) {}, UvWorkOnCallBackInt);
    if (rev != 0) {
        delete screenlockOnCallBack;
        screenlockOnCallBack = nullptr;
        delete work;
    }
}
} // namespace ScreenLock
} // namespace OHOS
