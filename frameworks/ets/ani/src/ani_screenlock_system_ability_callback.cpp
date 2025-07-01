/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include <memory>
#include <new>

#include "ani_screenlock_system_ability_callback.h"
#include "sclock_log.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
std::mutex ScreenlockSystemAbilityCallback::eventHandlerMutex_;
std::shared_ptr<AppExecFwk::EventHandler> ScreenlockSystemAbilityCallback::handler_{ nullptr };
ScreenlockSystemAbilityCallback::ScreenlockSystemAbilityCallback(const EventListener &eventListener)
    : eventListener_(eventListener)
{
}

ScreenlockSystemAbilityCallback::~ScreenlockSystemAbilityCallback()
{
}

std::shared_ptr<AppExecFwk::EventHandler> ScreenlockSystemAbilityCallback::GetEventHandler()
{
    std::lock_guard<std::mutex> lock(eventHandlerMutex_);
    if (handler_ == nullptr) {
        handler_ = AppExecFwk::EventHandler::Current();
    }
    return handler_;
}
} // namespace ScreenLock
} // namespace OHOS
