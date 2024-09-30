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

#include <cstdint>
#include <list>
#include <string>
#include <sys/time.h>

#include "sclock_log.h"
#include "screenlock_common.h"
#include "securec.h"
#include "strongauthmanager.h"
#include "commeventsubscriber.h"
#include "screenlock_strongauth_test.h"


namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;

void ScreenLockStrongAuthTest::SetUpTestCase()
{
}

void ScreenLockStrongAuthTest::TearDownTestCase()
{
}

void ScreenLockStrongAuthTest::SetUp()
{
}

void ScreenLockStrongAuthTest::TearDown()
{
}

/**
* @tc.name: ScreenLockStrongAuthTest001
* @tc.desc: ScreenLockStrongAuthTest RmvAll.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockStrongAuthTest");
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        SCLOCK_HILOGE("authmanager is nullptr!");
        return;
    }

    int32_t userId = 100;
    int32_t defaulVal = 1;
    authmanager->RegistUserAuthSuccessEventListener();
    authmanager->StartStrongAuthTimer(userId);
    authmanager->GetTimerId(userId);
    authmanager->ResetStrongAuthTimer(userId);
    authmanager->DestroyStrongAuthTimer(userId);
    authmanager->DestroyAllStrongAuthTimer();
    authmanager->UnRegistUserAuthSuccessEventListener();
    authmanager->SetStrongAuthStat(userId, defaulVal);
    int32_t val = authmanager->GetStrongAuthStat(userId);

    Singleton<CommeventMgr>::GetInstance().SubscribeEvent();
    Singleton<CommeventMgr>::GetInstance().UnSubscribeEvent();
    EXPECT_EQ(defaulVal, val);
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest002, TestSize.Level0)
{
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
    EXPECT_EQ(timer.interval, 1000);
}

} // namespace ScreenLock
} // namespace OHOS
