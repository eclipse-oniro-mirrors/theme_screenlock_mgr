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
ScreenlockNotifyTestInstance::ScreenlockNotifyTestInstance(const EventListenerTest &eventListener)
    : systemEventlistener_(eventListener)
{
}

ScreenlockNotifyTestInstance::~ScreenlockNotifyTestInstance()
{
}

void ScreenlockNotifyTestInstance::OnCallBack(const SystemEvent &systemEvent)
{
    SCLOCK_HILOGD("ScreenlockNotifyTestInstance  ONCALLBACK event is%{public}s", systemEvent.eventType_.c_str());
    SCLOCK_HILOGD("system event is %{public}d", systemEventlistener_.eventType);
}
} // namespace ScreenLock
} // namespace OHOS
