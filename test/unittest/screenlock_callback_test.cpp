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
#include "screenlock_callback_test.h"
#include "sclock_log.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
ScreenlockCallbackTest::ScreenlockCallbackTest(const EventListenerTest &eventListener)
{
}

ScreenlockCallbackTest::~ScreenlockCallbackTest()
{
}

void ScreenlockCallbackTest::OnCallBack(const SystemEvent &systemEvent)
{
    SCLOCK_HILOGD("event=%{public}s,params=%{public}s", systemEvent.eventType_.c_str(), systemEvent.params_.c_str());
}
} // namespace ScreenLock
} // namespace OHOS
