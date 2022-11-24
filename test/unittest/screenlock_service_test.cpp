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
#include "screenlock_service_test.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <sys/time.h>

#include "sclock_log.h"
#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_manager.h"
#include "screenlock_notify_test_instance.h"
#include "screenlock_system_ability.h"
#include "screenlock_system_ability_stub.h"
#include "screenlock_app_manager.h"
#include "screenlock_callback_test.h"

namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::ScreenLock;
using namespace OHOS::Rosen;

static EventListenerTest g_unlockTestListener;

void ScreenLockServiceTest::SetUpTestCase()
{
}

void ScreenLockServiceTest::TearDownTestCase()
{
}

void ScreenLockServiceTest::SetUp()
{
}

void ScreenLockServiceTest::TearDown()
{
}

/**
* @tc.name: SetScreenLockTest001
* @tc.desc: get unlockstate, IsScreenLocked state.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, SetScreenLockTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  IsScreenLocked state ,get unlockstate");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    ScreenLockSystemAbility::GetInstance()->SetScreenlocked(false);
    bool result = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, false);
}

/**
* @tc.name: SetScreenLockTest002
* @tc.desc: get lockstate, IsScreenLocked state.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, SetScreenLockTest002, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  IsScreenLocked state ,get lockstate");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    ScreenLockSystemAbility::GetInstance()->SetScreenlocked(true);
    bool result = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, true);
}

/**
* @tc.name: GetSecureTest003
* @tc.desc: get secure.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, GetSecureTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  secure");
    bool result = ScreenLockManager::GetInstance()->GetSecure();
    SCLOCK_HILOGD(" result is-------->%{public}d", result);
    EXPECT_EQ(result, false);
}

/**
* @tc.name: RequestLockTest004
* @tc.desc: can not get foucs, lock fail.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, RequestLockTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("Test can not get foucs,expect lock fail");
    bool ret = ScreenLockManager::GetInstance()->IsScreenLocked();
    sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockCallbackTest(g_unlockTestListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener object is nullptr");
        EXPECT_EQ(false, true);
        return ;
    }
    ScreenLockManager::GetInstance()->RequestLock(listener);
    std::string event = LOCK_SCREEN_RESULT;
    ScreenLockAppManager::GetInstance()->SendScreenLockEvent(event, SCREEN_FAIL);
    bool result = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("get not foucs IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, ret);
}

/**
* @tc.name: RequestUnlockTest005
* @tc.desc: can not get foucs, unlock fail.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, RequestUnlockTest005, TestSize.Level0)
{
    SCLOCK_HILOGD("Test can not get foucs,expect unlock fail");
    bool ret = ScreenLockManager::GetInstance()->IsScreenLocked();
    sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockCallbackTest(g_unlockTestListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener object is nullptr");
        EXPECT_EQ(false, true);
        return ;
    }
    ScreenLockManager::GetInstance()->RequestUnlock(listener);
    std::string event = UNLOCK_SCREEN_RESULT;
    ScreenLockAppManager::GetInstance()->SendScreenLockEvent(event, SCREEN_FAIL);
    bool result = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("get not foucs IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, ret);
}

/**
* @tc.name: ScreenLockTest006
* @tc.desc: test negative value.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest006, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  userid -2");
    const int MINUSERID = 0;
    int param = -2;
    EXPECT_EQ(param >= MINUSERID, false);
}

/**
* @tc.name: ScreenLockTest007
* @tc.desc: test large values.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest007, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  userid 999999999");
    const int MAXUSERID = 999999999;
    int param = 999999999;
    EXPECT_EQ(param < MAXUSERID, false);
}

/**
* @tc.name: ScreenLockTest008
* @tc.desc: beginWakeUp event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest008, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginWakeUp");
    DisplayPowerEvent event = DisplayPowerEvent::WAKE_UP;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_BeginWakeUp retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_WAKEUP), true);
}

/**
* @tc.name: ScreenLockTest009
* @tc.desc: beginSleep event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest009, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginsleep");
    DisplayPowerEvent event = DisplayPowerEvent::SLEEP;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_BeginSleep retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_SLEEP), true);
}

/**
* @tc.name: ScreenLockTest010
* @tc.desc: beginScreenOn event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest010, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginscreenon");
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_ON;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_BeginScreenOn retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_ON), true);
}

/**
* @tc.name: ScreenLockTest011
* @tc.desc: beginScreenOff event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest011, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginscreenoff");
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_OFF;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_BeginScreenOff retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_OFF), true);
}

/**
* @tc.name: ScreenLockTest012
* @tc.desc: endWakeUp event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest012, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endwakeup");
    DisplayPowerEvent event = DisplayPowerEvent::WAKE_UP;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_EndWakeUp retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_WAKEUP), true);
}

/**
* @tc.name: ScreenLockTest013
* @tc.desc: endSleep event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest013, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endsleep");
    DisplayPowerEvent event = DisplayPowerEvent::SLEEP;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_EndSleep retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_SLEEP), true);
}

/**
* @tc.name: ScreenLockTest014
* @tc.desc: endScreenOn event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest014, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endscreenon");
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_ON;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_EndScreenOn retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_END_ON), true);
}

/**
* @tc.name: ScreenLockTest015
* @tc.desc: endScreenOff event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest015, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endscreenoff");
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_OFF;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    displayPowerEventListener_->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_EndScreenOff retVal=%{public}d", retVal);
    EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_END_OFF), true);
}
} // namespace ScreenLock
} // namespace OHOS