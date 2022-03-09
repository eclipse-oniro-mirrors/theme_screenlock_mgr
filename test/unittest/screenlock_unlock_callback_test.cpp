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
#include "screenlock_unlock_callback_test.h"

#include "sclock_log.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
ScreenlockUnlockCallbackTest::ScreenlockUnlockCallbackTest(const EventListenerTest &eventListener)
{
    unlockListener_ = &eventListener;
}

ScreenlockUnlockCallbackTest::~ScreenlockUnlockCallbackTest()
{
}

void ScreenlockUnlockCallbackTest::OnCallBack(const std::string &event, bool result)
{
}

void ScreenlockUnlockCallbackTest::OnCallBack(const std::string &event)
{
}

void ScreenlockUnlockCallbackTest::OnCallBack(const std::string &event, int result)
{
    SCLOCK_HILOGD("event=%{public}s,result=%{public}d", event.c_str(), result);
}
} // namespace ScreenLock
} // namespace OHOS
