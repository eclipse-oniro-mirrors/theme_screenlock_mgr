/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef NAPI_SCREENLOCK_CALL_BACK_H
#define NAPI_SCREENLOCK_CALL_BACK_H

#include "event_listener.h"
#include "screenlock_callback_stub.h"

namespace OHOS {
namespace ScreenLock {
class ScreenlockCallback : public ScreenLockCallbackStub {
public:
    explicit ScreenlockCallback(const EventListener &eventListener);
    ~ScreenlockCallback() override;
    void OnCallBack(const int32_t screenLockResult) override;

private:
    EventListener eventListener_;
};

} // namespace ScreenLock
} // namespace OHOS
#endif //  NAPI_SCREENLOCK_CALL_BACK_H