/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef I_SCREENLOCK_CALLBACK_INTERFACE_H
#define I_SCREENLOCK_CALLBACK_INTERFACE_H

#include "iremote_broker.h"
namespace OHOS {
namespace ScreenLock {
struct ErrorInfo {
    uint32_t errorCode_ = 0;
    std::string message_;
};

class ScreenLockCallbackInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ScreenLock.ScreenLockCallbackInterface");
    virtual void OnCallBack(int32_t screenLockResult) = 0;
    enum Message { ON_CALLBACK = 0 };
};
} // namespace ScreenLock
} // namespace OHOS

#endif // I_SCREENLOCK_CALLBACK_INTERFACE_H
