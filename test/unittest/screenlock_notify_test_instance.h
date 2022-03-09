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
#ifndef NAPI_SCREENLOCK_NOTIFY_TEST_INSTANCE_H
#define NAPI_SCREENLOCK_NOTIFY_TEST_INSTANCE_H
#include <list>

#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_system_ability_stub.h"

namespace OHOS {
namespace ScreenLock {
constexpr int32_t LISTEN_MASK_BEGIN_WAKEUP = SCREENLOCK_BEGIN_WAKEUP;
constexpr int32_t LISTEN_MASK_END_WAKEUP = SCREENLOCK_END_WAKEUP;
constexpr int32_t LISTEN_MASK_BEGIN_SCREEN_ON = SCREENLOCK_BEGIN_SCREEN_ON;
constexpr int32_t LISTEN_MASK_END_SCREEN_ON = SCREENLOCK_END_SCREEN_ON;
constexpr int32_t LISTEN_MASK_BEGIN_SCREEN_OFF = SCREENLOCK_BEGIN_SCREEN_OFF;
constexpr int32_t LISTEN_MASK_END_SCREEN_OFF = SCREENLOCK_END_SCREEN_OFF;
constexpr int32_t LISTEN_MASK_UNLOCK_SCREEN = SCREENLOCK_UNLOCK_SCREEN;
constexpr int32_t LISTEN_MASK_BEGIN_SLEEP = SCREENLOCK_BEGIN_SLEEP;
constexpr int32_t LISTEN_MASK_END_SLEEP = SCREENLOCK_END_SLEEP;
constexpr int32_t LISTEN_MASK_CHANGE_USER = SCREENLOCK_CHANGE_USER;
constexpr int32_t LISTEN_MASK_SCREENLOCK_ENABLED = SCREENLOCK_SCREENLOCK_ENABLED;
constexpr int32_t LISTEN_MASK_EXIT_ANIMATION = SCREENLOCK_EXIT_ANIMATION;
class ScreenlockNotifyTestInstance : public ScreenLockSystemAbilityStub {
public:
    ScreenlockNotifyTestInstance(int32_t eventType, std::list<EventListenerTest> &eventListener);
    virtual ~ScreenlockNotifyTestInstance();
    void OnCallBack(const std::string &event, bool result)  override;
    void OnCallBack(const std::string &event)  override;
    void OnCallBack(const std::string &event, int result)  override;
    static int32_t GetEventType(const std::string &type);
    static bool MatchEventType(const std::string &type, const std::string &goalTypeStr);

private:
    int32_t eventType_;
    std::list<EventListenerTest> *listenerList_;
};
} // namespace ScreenLock
} // namespace OHOS
#endif //  NAPI_SCREENLOCK_NOTIFY_TEST_INSTANCE_H