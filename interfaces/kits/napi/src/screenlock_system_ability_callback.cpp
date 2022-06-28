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

#include <uv.h>

#include "sclock_log.h"
#include "screenlock_common.h"
#include <hitrace_meter.h>

namespace OHOS {
namespace ScreenLock {
ScreenlockSystemAbilityCallback::ScreenlockSystemAbilityCallback(
    int32_t eventType, std::list<EventListener> &listenerList) : eventType_(eventType)
{
    listenerList_ = &listenerList;
    SCLOCK_HILOGD("eventType_----> %{public}d", eventType_);
}

ScreenlockSystemAbilityCallback::~ScreenlockSystemAbilityCallback()
{
}

int32_t ScreenlockSystemAbilityCallback::GetEventType(const std::string &type)
{
    if (MatchEventType(type, BEGIN_WAKEUP)) {
        return static_cast<uint32_t>(SCREENLOCK_BEGIN_WAKEUP);
    }
    if (MatchEventType(type, END_WAKEUP)) {
        return static_cast<uint32_t>(SCREENLOCK_END_WAKEUP);
    }
    if (MatchEventType(type, BEGIN_SCREEN_ON)) {
        return static_cast<uint32_t>(SCREENLOCK_BEGIN_SCREEN_ON);
    }
    if (MatchEventType(type, END_SCREEN_ON)) {
        return static_cast<uint32_t>(SCREENLOCK_END_SCREEN_ON);
    }
    if (MatchEventType(type, BEGIN_SCREEN_OFF)) {
        return static_cast<uint32_t>(SCREENLOCK_BEGIN_SCREEN_OFF);
    }
    if (MatchEventType(type, END_SCREEN_OFF)) {
        return static_cast<uint32_t>(SCREENLOCK_END_SCREEN_OFF);
    }
    if (MatchEventType(type, EXIT_ANIMATION)) {
        return static_cast<uint32_t>(SCREENLOCK_EXIT_ANIMATION);
    }
    if (MatchEventType(type, UNLOCKSCREEN)) {
        return static_cast<uint32_t>(SCREENLOCK_UNLOCK_SCREEN);
    }
    if (MatchEventType(type, BEGIN_SLEEP)) {
        return static_cast<uint32_t>(SCREENLOCK_BEGIN_SLEEP);
    }
    if (MatchEventType(type, END_SLEEP)) {
        return static_cast<uint32_t>(SCREENLOCK_END_SLEEP);
    }
    if (MatchEventType(type, CHANGE_USER)) {
        return static_cast<uint32_t>(SCREENLOCK_CHANGE_USER);
    }
    if (MatchEventType(type, SCREENLOCK_ENABLED)) {
        return static_cast<uint32_t>(SCREENLOCK_SCREENLOCK_ENABLED);
    }
    if (MatchEventType(type, SYSTEM_READY)) {
        return static_cast<uint32_t>(SCREENLOCK_SYSTEM_READY);
    }
    return static_cast<uint32_t>(NONE_EVENT_TYPE);
}

bool ScreenlockSystemAbilityCallback::MatchEventType(const std::string &type, const std::string &goalTypeStr)
{
    return goalTypeStr.compare(type) == 0;
}

auto OnUvWorkBoolCallback = [](uv_work_t *work, int status) {
    SCLOCK_HILOGD("OnUvWorkBoolCallback status = %{public}d", status);
    if (work == nullptr) {
        return;
    }
    ScreenlockOnCallBack *screenlockOnCallBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (screenlockOnCallBackPtr == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    napi_value undefined = 0;
    napi_get_undefined(screenlockOnCallBackPtr->env, &undefined);
    napi_value callbackFunc = nullptr;
    napi_get_reference_value(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->callbackref, &callbackFunc);
    napi_value callbackResult = nullptr;
    napi_value callbackValues[2] = {0};
    napi_get_undefined(screenlockOnCallBackPtr->env, &callbackValues[0]);
    napi_get_boolean(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->boolCallBackValue, &callbackValues[1]);
    napi_call_function(
        screenlockOnCallBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_TWO, callbackValues, &callbackResult);
    if (screenlockOnCallBackPtr != nullptr) {
        delete screenlockOnCallBackPtr;
        screenlockOnCallBackPtr = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
};

void ScreenlockSystemAbilityCallback::OnCallBack(const std::string &event, bool result)
{
    SCLOCK_HILOGD("ONCALLBACK_BOOL event---->%{public}s,result---->%{public}d", event.c_str(), result);
    for (auto iter = listenerList_->begin(); iter != listenerList_->end(); iter++) {
        if (iter->eventType == GetEventType(event)) {
            uv_loop_s *loop = nullptr;
            napi_get_uv_event_loop(iter->env, &loop);
            if (loop == nullptr) {
                return;
            }
            uv_work_t *work = std::make_unique<uv_work_t>().release();
            if (work == nullptr) {
                return;
            }
            ScreenlockOnCallBack *screenlockOnCallBack = std::make_unique<ScreenlockOnCallBack>().release();
            if (screenlockOnCallBack == nullptr) {
                delete work;
                work = nullptr;
                return;
            }
            screenlockOnCallBack->env = iter->env;
            screenlockOnCallBack->callbackref = iter->callbackRef;
            screenlockOnCallBack->boolCallBackValue = result;
            screenlockOnCallBack->thisVar = iter->thisVar;
            work->data = (void *)screenlockOnCallBack;
            int ret = uv_queue_work(
                loop, work, [](uv_work_t *work) {}, OnUvWorkBoolCallback);
            if (ret != 0) {
                delete screenlockOnCallBack;
                screenlockOnCallBack = nullptr;
                delete work;
                work = nullptr;
            }
        }
    }
}

auto OnVoidUvWorkCallback = [](uv_work_t *work, int status) {
    SCLOCK_HILOGD("OnVoidUvWorkCallback status = %{public}d", status);
    if (work == nullptr) {
        return;
    }
    ScreenlockOnCallBack *screenlockOnCallBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (screenlockOnCallBackPtr == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    napi_value undefined = 0;
    napi_get_undefined(screenlockOnCallBackPtr->env, &undefined);
    napi_value callbackFunc = nullptr;
    napi_get_reference_value(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->callbackref, &callbackFunc);
    napi_value callbackResult = nullptr;
    napi_value callbackValues[1] = {0};
    napi_get_undefined(screenlockOnCallBackPtr->env, &callbackValues[0]);
    napi_call_function(
        screenlockOnCallBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_ONE, callbackValues, &callbackResult);
    if (screenlockOnCallBackPtr != nullptr) {
        delete screenlockOnCallBackPtr;
        screenlockOnCallBackPtr = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
};

void ScreenlockSystemAbilityCallback::OnCallBack(const std::string &event)
{
    SCLOCK_HILOGD("ScreenlockSystemAbilityCallback  ONCALLBACK_VOID event---->%{public}s", event.c_str());
    for (auto iter = listenerList_->begin(); iter != listenerList_->end(); iter++) {
        if (iter->eventType == GetEventType(event)) {
            uv_loop_s *loop = nullptr;
            napi_get_uv_event_loop(iter->env, &loop);
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
            screenlockOnCallBack->env = iter->env;
            screenlockOnCallBack->callbackref = iter->callbackRef;
            screenlockOnCallBack->thisVar = iter->thisVar;
            work->data = (void *)screenlockOnCallBack;
            int ret = uv_queue_work(
                loop, work, [](uv_work_t *work) {}, OnVoidUvWorkCallback);
            if (ret != 0) {
                delete screenlockOnCallBack;
                screenlockOnCallBack = nullptr;
                delete work;
                work = nullptr;
            }
        }
    }
}

auto OnIntUvWorkCallback = [](uv_work_t *work, int status) {
    SCLOCK_HILOGD("OnIntUvWorkCallback status = %{public}d", status);
    if (work == nullptr) {
        return;
    }
    ScreenlockOnCallBack *screenlockOnCallBackPtr = static_cast<ScreenlockOnCallBack *>(work->data);
    if (screenlockOnCallBackPtr == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    napi_value undefined = 0;
    napi_get_undefined(screenlockOnCallBackPtr->env, &undefined);
    napi_value callbackFunc = nullptr;
    napi_get_reference_value(screenlockOnCallBackPtr->env, screenlockOnCallBackPtr->callbackref, &callbackFunc);
    napi_value callbackResult = nullptr;
    napi_value callbackValues[2] = {0};
    napi_get_undefined(screenlockOnCallBackPtr->env, &callbackValues[0]);
    napi_create_int32(screenlockOnCallBackPtr->env, static_cast<int32_t>(screenlockOnCallBackPtr->intCallbackValue),
        &callbackValues[1]);
    napi_call_function(
        screenlockOnCallBackPtr->env, nullptr, callbackFunc, ARGS_SIZE_TWO, callbackValues, &callbackResult);
    if (screenlockOnCallBackPtr != nullptr) {
        delete screenlockOnCallBackPtr;
        screenlockOnCallBackPtr = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
};

void ScreenlockSystemAbilityCallback::OnCallBack(const std::string &event, int result)
{
    SCLOCK_HILOGD("ONCALLBACK_INT event---->%{public}s,result---->%{public}d", event.c_str(), result);
    for (auto iter = listenerList_->begin(); iter != listenerList_->end(); iter++) {
        if (iter->eventType == GetEventType(event)) {
            uv_loop_s *loop = nullptr;
            napi_get_uv_event_loop(iter->env, &loop);
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
            screenlockOnCallBack->env = iter->env;
            screenlockOnCallBack->callbackref = iter->callbackRef;
            screenlockOnCallBack->intCallbackValue = result;
            screenlockOnCallBack->thisVar = iter->thisVar;
            work->data = (void *)screenlockOnCallBack;
            int ret = uv_queue_work(
                loop, work, [](uv_work_t *work) {}, OnIntUvWorkCallback);
            if (ret != 0) {
                delete screenlockOnCallBack;
                screenlockOnCallBack = nullptr;
                delete work;
                work = nullptr;
            }
        }
    }
    FinishAsyncTrace(HITRACE_TAG_MISC, "NAPI_UnlockScreen finish", HITTACE_UNSCREENLOCK_FIRST);
    FinishAsyncTrace(HITRACE_TAG_MISC, "Services_UnlockScreen finish", HITTACE_UNSCREENLOCK_SECOND);
}
} // namespace ScreenLock
} // namespace OHOS
