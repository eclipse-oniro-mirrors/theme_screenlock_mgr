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
#include "screenlock_notify_test_instance.h"

#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
ScreenlockNotifyTestInstance::ScreenlockNotifyTestInstance(
    int32_t eventType, std::list<EventListenerTest> &listenerList)
{
    eventType_ = eventType;
    listenerList_ = &listenerList;
}

ScreenlockNotifyTestInstance::~ScreenlockNotifyTestInstance()
{
}

int32_t ScreenlockNotifyTestInstance::GetEventType(const std::string &type)
{
    if (MatchEventType(type, BEGIN_WAKEUP)) {
        return LISTEN_MASK_BEGIN_WAKEUP;
    }
    if (MatchEventType(type, END_WAKEUP)) {
        return LISTEN_MASK_END_WAKEUP;
    }
    if (MatchEventType(type, BEGIN_SCREEN_ON)) {
        return LISTEN_MASK_BEGIN_SCREEN_ON;
    }
    if (MatchEventType(type, END_SCREEN_ON)) {
        return LISTEN_MASK_END_SCREEN_ON;
    }
    if (MatchEventType(type, BEGIN_SCREEN_OFF)) {
        return LISTEN_MASK_BEGIN_SCREEN_OFF;
    }
    if (MatchEventType(type, END_SCREEN_OFF)) {
        return LISTEN_MASK_END_SCREEN_OFF;
    }
    if (MatchEventType(type, EXIT_ANIMATION)) {
        return LISTEN_MASK_EXIT_ANIMATION;
    }
    if (MatchEventType(type, UNLOCKSCREEN)) {
        return LISTEN_MASK_UNLOCK_SCREEN;
    }
    if (MatchEventType(type, BEGIN_SLEEP)) {
        return LISTEN_MASK_BEGIN_SLEEP;
    }
    if (MatchEventType(type, END_SLEEP)) {
        return LISTEN_MASK_END_SLEEP;
    }
    if (MatchEventType(type, CHANGE_USER)) {
        return LISTEN_MASK_CHANGE_USER;
    }
    if (MatchEventType(type, SCREENLOCK_ENABLED)) {
        return LISTEN_MASK_SCREENLOCK_ENABLED;
    }
    if (MatchEventType(type, SYSTEM_READY)) {
        return static_cast<uint32_t>(SCREENLOCK_SYSTEM_READY);
    }
    return NONE_EVENT_TYPE;
}

bool ScreenlockNotifyTestInstance::MatchEventType(const std::string &type, const std::string &goalTypeStr)
{
    return goalTypeStr.compare(type) == 0;
}

void ScreenlockNotifyTestInstance::OnCallBack(const std::string &event, bool result)
{
    SCLOCK_HILOGD("ScreenlockNotifyTestInstance  ONCALLBACK_BOOL event----》%{public}s", event.c_str());
    SCLOCK_HILOGD("ScreenlockNotifyTestInstance  ONCALLBACK_BOOL result----》%{public}d", result);
    for (auto iter = listenerList_->begin(); iter != listenerList_->end(); iter++) {
        if (iter->eventType == GetEventType(event)) {
            SCLOCK_HILOGD(
                "ScreenlockNotifyTestInstance  ONCALLBACK_BOOL eventType----》%{public}d", iter->eventType);
        }
    }
}

void ScreenlockNotifyTestInstance::OnCallBack(const std::string &event)
{
    for (auto iter = listenerList_->begin(); iter != listenerList_->end(); iter++) {
        if (iter->eventType == GetEventType(event)) {
            SCLOCK_HILOGD("ScreenlockNotifyTestInstance  OnCallBack eventType----》%{public}d", iter->eventType);
        }
    }
}

void ScreenlockNotifyTestInstance::OnCallBack(const std::string &event, int result)
{
    for (auto iter = listenerList_->begin(); iter != listenerList_->end(); iter++) {
        if (iter->eventType == GetEventType(event)) {
            SCLOCK_HILOGD("ScreenlockNotifyTestInstance  OnCallBack eventType----》%{public}d", iter->eventType);
        }
    }
}
} // namespace ScreenLock
} // namespace OHOS
