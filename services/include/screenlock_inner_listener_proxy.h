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

#ifndef I_SCREENLOCK_INNER_LISTENER_PROXY_H
#define I_SCREENLOCK_INNER_LISTENER_PROXY_H

#include <string>

#include "iremote_proxy.h"
#include "refbase.h"
#include "screenlock_inner_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockInnerListenerProxy : public IRemoteProxy<InnerListenerIf> {
public:
    explicit ScreenLockInnerListenerProxy(const sptr<IRemoteObject> &impl);
    ~ScreenLockInnerListenerProxy() = default;
    void OnStateChanged(int32_t userId, int32_t state) override;
private:
    bool SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply);
private:
    static inline BrokerDelegator<ScreenLockInnerListenerProxy> delegator_;
};
} // namespace ScreenLock
} // namespace OHOS

#endif // I_SCREENLOCK_INNER_LISTENER_PROXY_H