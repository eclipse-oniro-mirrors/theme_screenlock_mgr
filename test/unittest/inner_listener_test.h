/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NAPI_INNER_LISTENER_TEST_H
#define NAPI_INNER_LISTENER_TEST_H

#include "screenlock_inner_listener.h"
#include "screenlock_inner_listener_interface.h"
#include "visibility.h"
#include "iremote_stub.h"
#include "message_parcel.h"

namespace OHOS {
namespace ScreenLock {
class StrongAuthListenerTest : public StrongAuthListener {
public:
    explicit StrongAuthListenerTest(int32_t userId) : StrongAuthListener(userId) {};
    virtual ~StrongAuthListenerTest() = default;
    void OnStrongAuthChanged(int32_t userId, int32_t authenticated) override;
};

class DeviceLockedListenerTest : public DeviceLockedListener {
public:
    explicit DeviceLockedListenerTest(int32_t userId) : DeviceLockedListener(userId) {};
    virtual ~DeviceLockedListenerTest() = default;
    void OnDeviceLockStateChanged(int userId, bool isDeviceLocked) override;
};

class InnerListenerIfTest : public IRemoteStub<InnerListenerIf> {
public:
    InnerListenerIfTest() = default;
    virtual ~InnerListenerIfTest() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
                                           MessageOption& option) override;
                                           
    void OnStateChanged(int32_t userId, int32_t state) override;
};

}
}
#endif