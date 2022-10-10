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

#ifndef I_SCREENLOCK_CALLBACK_LISTENER_H
#define I_SCREENLOCK_CALLBACK_LISTENER_H

#include "iremote_broker.h"
#include "iremote_object.h"

namespace OHOS {
namespace ScreenLock {
enum CALLBACK_TYPE {
    ONCALLBACK_BOOL,
    ONCALLBACK_VOID,
    ONCALLBACK_INT,
    ONCALLBACK,
};

struct SystemEvent {
    std::string eventType_;
    std::string params_;
    SystemEvent(std::string eventType = "", std::string params = "") : eventType_(eventType), params_(params)
    {
    }
};

struct ErrorInfo {
    uint32_t errorCode_;
    std::string message_;
    ErrorInfo(uint32_t errorCode = 0, std::string message = "") : errorCode_(errorCode), message_(message)
    {
    }
};

class ScreenLockSystemAbilityInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ScreenLock.ScreenLockSystemAbilityInterface");
    virtual void OnCallBack(const SystemEvent &systemEvent) = 0;
    virtual void SetErrorInfo(const ErrorInfo &errorInfo) = 0;
};
} // namespace ScreenLock
} // namespace OHOS

#endif // I_SCREENLOCK_CALLBACK_LISTENER_H
