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
#define private public
#define protected public
#include "screenlock_system_ability.h"
#undef private
#undef protected

#include <cstdint>
#include <list>
#include <string>
#include <sys/time.h>

#include "accesstoken_kit.h"
#include "sclock_log.h"
#include "screenlock_callback_test.h"
#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_notify_test_instance.h"
#include "screenlock_service_test.h"
#include "screenlock_system_ability.h"
#include "screenlock_system_ability_stub.h"
#include "securec.h"
#include "token_setproc.h"


namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Security::AccessToken;
constexpr const uint16_t EACH_LINE_LENGTH = 100;
constexpr const uint16_t TOTAL_LENGTH = 1000;
constexpr const char *CMD1 = "hidumper -s 3704";
constexpr const char *CMD2 = "hidumper -s 3704 -a -h";
constexpr const char *CMD3 = "hidumper -s 3704 -a -all";
uint64_t g_selfTokenID = 0;
static EventListenerTest g_unlockTestListener;

static HapPolicyParams g_policyParams = { .apl = APL_SYSTEM_CORE,
    .domain = "test.domain",
    .permList = { { .permissionName = "ohos.permission.ACCESS_SCREEN_LOCK_INNER",
                      .bundleName = "ohos.screenlock_test.demo",
                      .grantMode = 1,
                      .availableLevel = APL_NORMAL,
                      .label = "label",
                      .labelId = 1,
                      .description = "test",
                      .descriptionId = 1 },
        { .permissionName = "ohos.permission.DUMP",
            .bundleName = "ohos.screenlock_test.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_CORE,
            .label = "label",
            .labelId = 1,
            .description = "test",
            .descriptionId = 1 } },
    .permStateList = { { .permissionName = "ohos.permission.ACCESS_SCREEN_LOCK_INNER",
                           .isGeneral = true,
                           .resDeviceID = { "local" },
                           .grantStatus = { PermissionState::PERMISSION_GRANTED },
                           .grantFlags = { 1 } },
        { .permissionName = "ohos.permission.DUMP",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };

HapInfoParams g_infoParams = { .userID = 1,
    .bundleName = "screenlock_service",
    .instIndex = 0,
    .appIDDesc = "test",
    .apiVersion = 9,
    .isSystemApp = true };

void GrantNativePermission()
{
    g_selfTokenID = GetSelfTokenID();
    AccessTokenIDEx tokenIdEx = { 0 };
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoParams, g_policyParams);
    int32_t ret = SetSelfTokenID(tokenIdEx.tokenIDEx);
    if (ret == 0) {
        SCLOCK_HILOGI("SetSelfTokenID success!");
    } else {
        SCLOCK_HILOGE("SetSelfTokenID fail!");
    }
}

void ScreenLockServiceTest::SetUpTestCase()
{
    GrantNativePermission();
}

void ScreenLockServiceTest::TearDownTestCase()
{
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    SetSelfTokenID(g_selfTokenID);
}

void ScreenLockServiceTest::SetUp()
{
}

void ScreenLockServiceTest::TearDown()
{
}

bool ScreenLockServiceTest::ExecuteCmd(const std::string &cmd, std::string &result)
{
    char buff[EACH_LINE_LENGTH] = { 0x00 };
    char output[TOTAL_LENGTH] = { 0x00 };
    FILE *ptr = popen(cmd.c_str(), "r");
    if (ptr != nullptr) {
        while (fgets(buff, sizeof(buff), ptr) != nullptr) {
            if (strcat_s(output, sizeof(output), buff) != 0) {
                pclose(ptr);
                ptr = nullptr;
                return false;
            }
        }
        pclose(ptr);
        ptr = nullptr;
    } else {
        return false;
    }
    result = std::string(output);
    return true;
}

/**
* @tc.name: ScreenLockTest001
* @tc.desc: beginWakeUp event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginWakeUp");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::WAKE_UP;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_BeginWakeUp retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_WAKEUP));
}

/**
* @tc.name: ScreenLockTest002
* @tc.desc: OnsystemEvent
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest002, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnsystemEvent");
    sptr<ScreenLockSystemAbilityInterface> listener = new (std::nothrow)
        ScreenlockNotifyTestInstance(g_unlockTestListener);
    ASSERT_NE(listener, nullptr);
    ScreenLockSystemAbility::GetInstance()->OnSystemEvent(listener);
    EXPECT_NE(ScreenLockSystemAbility::GetInstance()->systemEventListener_, nullptr);
}

/**
* @tc.name: ScreenLockTest003
* @tc.desc: beginSleep event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginsleep");
    ScreenLockSystemAbility::GetInstance()->state_ = ServiceRunningState::STATE_NOT_START;
    ScreenLockSystemAbility::GetInstance()->OnStart();
    DisplayPowerEvent event = DisplayPowerEvent::SLEEP;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_BeginSleep retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_SLEEP));
}

/**
* @tc.name: ScreenLockTest004
* @tc.desc: beginScreenOn event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginscreenon");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_ON;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_BeginScreenOn retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_ON));
}

/**
* @tc.name: ScreenLockTest005
* @tc.desc: beginScreenOff event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest005, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginscreenoff");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_OFF;
    EventStatus status = EventStatus::BEGIN;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_BeginScreenOff retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_OFF));
}

/**
* @tc.name: ScreenLockTest006
* @tc.desc: endWakeUp event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest006, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endwakeup");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::WAKE_UP;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_EndWakeUp retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_WAKEUP));
}

/**
* @tc.name: ScreenLockTest007
* @tc.desc: endSleep event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest007, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endsleep");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::SLEEP;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetInteractiveState();
    SCLOCK_HILOGD("Test_EndSleep retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_SLEEP));
}

/**
* @tc.name: ScreenLockTest008
* @tc.desc: endScreenOn event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest008, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endscreenon");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_ON;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_EndScreenOn retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(ScreenState::SCREEN_STATE_END_ON));
}

/**
* @tc.name: ScreenLockTest009
* @tc.desc: endScreenOff and begin desktopready event.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest009, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endscreenoff");
    ScreenLockSystemAbility::GetInstance();
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_OFF;
    EventStatus status = EventStatus::END;
    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener = new (std::nothrow)
        ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    ASSERT_NE(displayPowerEventListener, nullptr);
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    event = DisplayPowerEvent::DESKTOP_READY;
    status = EventStatus::BEGIN;
    displayPowerEventListener->OnDisplayPowerEvent(event, status);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetState().GetScreenState();
    SCLOCK_HILOGD("Test_EndScreenOff retVal=%{public}d", retVal);
    EXPECT_EQ(retVal, static_cast<int>(ScreenState::SCREEN_STATE_END_OFF));
}

/**
* @tc.name: ScreenLockDumperTest013
* @tc.desc: dump showhelp.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockDumperTest013, TestSize.Level0)
{
    SCLOCK_HILOGD("Test hidumper of showhelp");
    std::string result;
    auto ret = ScreenLockServiceTest::ExecuteCmd(CMD1, result);
    EXPECT_TRUE(ret);
    EXPECT_NE(result.find("Option"), std::string::npos);
    EXPECT_NE(result.find("-all"), std::string::npos);
}

/**
* @tc.name: ScreenLockDumperTest014
* @tc.desc: dump showhelp.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockDumperTest014, TestSize.Level0)
{
    SCLOCK_HILOGD("Test hidumper of -h");
    std::string result;
    auto ret = ScreenLockServiceTest::ExecuteCmd(CMD2, result);
    EXPECT_TRUE(ret);
    EXPECT_NE(result.find("Description"), std::string::npos);
    EXPECT_NE(result.find("dump all screenlock information"), std::string::npos);
}

/**
* @tc.name: ScreenLockDumperTest015
* @tc.desc: dump screenlock information.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockDumperTest015, TestSize.Level0)
{
    SCLOCK_HILOGD("Test hidumper of -all");
    std::string result;
    auto ret = ScreenLockServiceTest::ExecuteCmd(CMD3, result);
    EXPECT_TRUE(ret);
    EXPECT_NE(result.find("screenLocked"), std::string::npos);
    EXPECT_NE(result.find("screenState"), std::string::npos);
    EXPECT_NE(result.find("offReason"), std::string::npos);
    EXPECT_NE(result.find("interactiveState"), std::string::npos);
}

/**
* @tc.name: ScreenLockTest016
* @tc.desc: Test Lock.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest016, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestLock");
    ScreenLockSystemAbility::GetInstance()->state_ = ServiceRunningState::STATE_NOT_START;
    sptr<ScreenLockCallbackInterface> listener = new (std::nothrow) ScreenlockCallbackTest(g_unlockTestListener);
    ASSERT_NE(listener, nullptr);

    ScreenLockSystemAbility::GetInstance()->stateValue_.SetScreenlocked(true);
    bool isLocked = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    EXPECT_EQ(isLocked, true);
    int32_t result = ScreenLockSystemAbility::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_OK);
    ScreenLockSystemAbility::GetInstance()->stateValue_.SetScreenlocked(false);
    result = ScreenLockSystemAbility::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_OK);
}

/**
* @tc.name: ScreenLockTest017
* @tc.desc: Test Unlock and UnlockScreen.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest017, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestUnlock");
    ScreenLockSystemAbility::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    sptr<ScreenLockCallbackInterface> listener = new (std::nothrow) ScreenlockCallbackTest(g_unlockTestListener);
    ASSERT_NE(listener, nullptr);
    int32_t result = ScreenLockSystemAbility::GetInstance()->UnlockScreen(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_FOCUS_APP);
    result = ScreenLockSystemAbility::GetInstance()->Unlock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_FOCUS_APP);
    ScreenLockSystemAbility::GetInstance()->state_ = ServiceRunningState::STATE_NOT_START;
    result = ScreenLockSystemAbility::GetInstance()->Unlock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_FOCUS_APP);
}

/**
* @tc.name: ScreenLockTest018
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest018, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(UNLOCK_SCREEN_RESULT, SCREEN_SUCC);
    bool isLocked = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    EXPECT_EQ(isLocked, false);
}

/**
* @tc.name: ScreenLockTest019
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest019, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(UNLOCK_SCREEN_RESULT, SCREEN_FAIL);
    bool isLocked = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    EXPECT_EQ(isLocked, false);
}

/**
* @tc.name: ScreenLockTest020
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest020, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(UNLOCK_SCREEN_RESULT, SCREEN_CANCEL);
    bool isLocked = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    EXPECT_EQ(isLocked, false);
}

/**
* @tc.name: ScreenLockTest021
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest021, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(LOCK_SCREEN_RESULT, SCREEN_SUCC);
    bool isLocked;
    ScreenLockSystemAbility::GetInstance()->IsLocked(isLocked);
    EXPECT_EQ(isLocked, true);
}

/**
* @tc.name: ScreenLockTest022
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest022, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(LOCK_SCREEN_RESULT, SCREEN_FAIL);
    bool isLocked;
    ScreenLockSystemAbility::GetInstance()->IsLocked(isLocked);
    EXPECT_EQ(isLocked, true);
}

/**
* @tc.name: ScreenLockTest023
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest023, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(SCREEN_DRAWDONE, SCREEN_SUCC);
    ScreenLockSystemAbility::GetInstance()->SendScreenLockEvent(LOCK_SCREEN_RESULT, SCREEN_CANCEL);
    bool isLocked;
    ScreenLockSystemAbility::GetInstance()->IsLocked(isLocked);
    EXPECT_EQ(isLocked, true);
}

/**
* @tc.name: ScreenLockTest025
* @tc.desc: Test Onstop and OnStart.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest025, TestSize.Level0)
{
    SCLOCK_HILOGD("Test Onstop");
    ScreenLockSystemAbility::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    ScreenLockSystemAbility::GetInstance()->OnStart();
    ScreenLockSystemAbility::GetInstance()->OnStop();
    ScreenLockSystemAbility::GetInstance()->OnStart();
    EXPECT_EQ(ScreenLockSystemAbility::GetInstance()->state_, ServiceRunningState::STATE_NOT_START);
    int times = 0;
    ScreenLockSystemAbility::GetInstance()->RegisterDisplayPowerEventListener(times);
    bool isLocked;
    ScreenLockSystemAbility::GetInstance()->IsLocked(isLocked);
    SCLOCK_HILOGD("Test_SendScreenLockEvent of screendrawdone isLocked=%{public}d", isLocked);
    EXPECT_EQ(isLocked, false);
}

/**
* @tc.name: ScreenLockTest026
* @tc.desc: Test GetSecure.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest026, TestSize.Level0)
{
    SCLOCK_HILOGD("Test GetSecure.");
    ScreenLockSystemAbility::GetInstance()->state_ = ServiceRunningState::STATE_NOT_START;
    bool ret = ScreenLockSystemAbility::GetInstance()->GetSecure();
    EXPECT_EQ(ret, false);
}

/**
* @tc.name: ScreenLockTest027
* @tc.desc: Test UnlockScreenEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest027, TestSize.Level0)
{
    SCLOCK_HILOGD("Test UnlockScreenEvent.");
    ScreenLockSystemAbility::GetInstance()->unlockVecListeners_.clear();
    ScreenLockSystemAbility::GetInstance()->UnlockScreenEvent(SCREEN_CANCEL);
    bool isLocked;
    ScreenLockSystemAbility::GetInstance()->IsLocked(isLocked);
    EXPECT_EQ(isLocked, false);
}

/**
* @tc.name: LockTest028
* @tc.desc: Test Lock Screen.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, LockTest028, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestLock.");
    int32_t userId = 0;
    int32_t result = ScreenLockSystemAbility::GetInstance()->Lock(userId);
    EXPECT_EQ(result, E_SCREENLOCK_OK);
}
} // namespace ScreenLock
} // namespace OHOS