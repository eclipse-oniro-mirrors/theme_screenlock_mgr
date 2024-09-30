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
#include "preferences_util.h"
#include "screenlock_preferences_test.h"


namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;

void ScreenLockPreferenceTest::SetUpTestCase()
{
}

void ScreenLockPreferenceTest::TearDownTestCase()
{
}

void ScreenLockPreferenceTest::SetUp()
{
}

void ScreenLockPreferenceTest::TearDown()
{
}

/**
* @tc.name: ScreenLockPreferenceTest001
* @tc.desc: ScreenLockPreferenceTest String.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockPreferenceTest, ScreenLockPreferenceTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockPreferenceTest String");
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    int userId = 0;
    std::string defaulVal = "test";
    int result = preferencesUtil->SaveString(std::to_string(userId), defaulVal);
    std::string val = preferencesUtil->ObtainString(std::to_string(userId), defaulVal);
    SCLOCK_HILOGD("String.[result]:%{public}d, [val]:%{public}s", result, val.c_str());
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    EXPECT_EQ(defaulVal, val);
}

/**
* @tc.name: ScreenLockPreferenceTest001
* @tc.desc: ScreenLockPreferenceTest Int.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockPreferenceTest, ScreenLockPreferenceTest002, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockPreferenceTest Int");
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    int userId = 0;
    int defaulVal = 0;
    int result = preferencesUtil->SaveInt(std::to_string(userId), defaulVal);
    int val = preferencesUtil->ObtainInt(std::to_string(userId), defaulVal);
    SCLOCK_HILOGD("String.[result]:%{public}d, [val]:%{public}d", result, val);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    EXPECT_EQ(defaulVal, val);
}

/**
* @tc.name: ScreenLockPreferenceTest001
* @tc.desc: ScreenLockPreferenceTest Bool.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockPreferenceTest, ScreenLockPreferenceTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockPreferenceTest Bool");
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    int userId = 0;
    bool defaulVal = false;
    int result = preferencesUtil->SaveBool(std::to_string(userId), defaulVal);
    bool val = preferencesUtil->ObtainBool(std::to_string(userId), defaulVal);
    SCLOCK_HILOGD("String.[result]:%{public}d, [val]:%{public}d", result, val);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    EXPECT_EQ(defaulVal, val);
}

/**
* @tc.name: ScreenLockPreferenceTest001
* @tc.desc: ScreenLockPreferenceTest Long.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockPreferenceTest, ScreenLockPreferenceTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockPreferenceTest Long");
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    int userId = 0;
    int64_t defaulVal = false;
    preferencesUtil->SaveLong(std::to_string(userId), defaulVal);
    int64_t val = preferencesUtil->ObtainLong(std::to_string(userId), defaulVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    EXPECT_EQ(defaulVal, val);
}

/**
* @tc.name: ScreenLockPreferenceTest001
* @tc.desc: ScreenLockPreferenceTest Float.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockPreferenceTest, ScreenLockPreferenceTest005, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockPreferenceTest Float");
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    int userId = 0;
    float defaulVal = 1.0;
    int result = preferencesUtil->SaveFloat(std::to_string(userId), defaulVal);
    float val = preferencesUtil->ObtainFloat(std::to_string(userId), defaulVal);
    SCLOCK_HILOGD("String.[result]:%{public}d, [val]:%{public}f", result, val);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    EXPECT_EQ(defaulVal, val);
}

/**
* @tc.name: ScreenLockPreferenceTest001
* @tc.desc: ScreenLockPreferenceTest RmvAll.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockPreferenceTest, ScreenLockPreferenceTest006, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockPreferenceTest RmvAll");
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    preferencesUtil->RemoveAll();
    preferencesUtil->RefreshSync();
    EXPECT_NE(preferencesUtil, nullptr);
}


} // namespace ScreenLock
} // namespace OHOS
