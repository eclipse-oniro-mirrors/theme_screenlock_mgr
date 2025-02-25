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

#ifndef SCREENLOCK_STRONGAUTH_LISTENER_STUB_H
#define SCREENLOCK_STRONGAUTH_LISTENER_STUB_H

#include "visibility.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "screenlock_strongauth_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockStrongAuthListenerStub : public IRemoteStub<StrongAuthListenerInterface> {
public:
    SCREENLOCK_API ScreenLockStrongAuthListenerStub() = default;
    SCREENLOCK_API ~ScreenLockStrongAuthListenerStub() override;
    SCREENLOCK_API int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    SCREENLOCK_API void OnStrongAuthChanged(int32_t userId, int32_t authenticated) override;

private:
    int32_t OnStrongAuthChangedStub(MessageParcel &data, MessageParcel &reply);
};
} // namespace ScreenLock
} // namespace OHOS
#endif // USER_AUTH_EVENT_LISTENER_STUB_H