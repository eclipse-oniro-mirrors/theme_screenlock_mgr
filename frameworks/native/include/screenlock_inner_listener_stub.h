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

#ifndef SCREENLOCK_INNER_LISTENER_STUB_H
#define SCREENLOCK_INNER_LISTENER_STUB_H

#include <cstdint>
#include <string>

#include "visibility.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "screenlock_inner_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockInnerListenerStub : public IRemoteStub<InnerListenerIf> {
public:
    ScreenLockInnerListenerStub() = default;
    ~ScreenLockInnerListenerStub() override;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
                                           MessageOption& option) override;
                                           
    void OnStateChanged(int32_t userId, int32_t state) override;

private:
    int32_t OnStateChangedStub(MessageParcel &data, MessageParcel &reply);
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SCREENLOCK_INNER_LISTENER_STUB_H