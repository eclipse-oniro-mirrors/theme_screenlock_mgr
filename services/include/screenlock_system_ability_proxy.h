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

#ifndef I_SCREENLOCK_CALLBACK_LISTENER_PROXY_H
#define I_SCREENLOCK_CALLBACK_LISTENER_PROXY_H

#include <string>

#include "iremote_proxy.h"
#include "refbase.h"
#include "screenlock_system_ability_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockSystemAbilityProxy : public IRemoteProxy<ScreenLockSystemAbilityInterface> {
public:
    explicit ScreenLockSystemAbilityProxy(const sptr<IRemoteObject> &impl);
    ~ScreenLockSystemAbilityProxy() = default;
    void OnCallBack(const SystemEvent &systemEvent) override;
    void SetErrorInfo(const ErrorInfo &errorInfo) override;

private:
    static inline BrokerDelegator<ScreenLockSystemAbilityProxy> delegator_;
};
} // namespace ScreenLock
} // namespace OHOS

#endif // I_SCREENLOCK_CALLBACK_LISTENER_PROXY_H
