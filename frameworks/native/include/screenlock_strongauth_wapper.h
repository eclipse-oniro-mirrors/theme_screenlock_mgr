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
#ifndef SCREENLOCK_STRONGAUTH_WAPPER_H
#define SCREENLOCK_STRONGAUTH_WAPPER_H

#include "event_handler.h"
#include "screenlock_strongauth_listener.h"
#include "screenlock_strongauth_listener_stub.h"

namespace OHOS {
namespace ScreenLock {
class StrongAuthListenerWrapper : public ScreenLockStrongAuthListenerStub {
public:
    explicit StrongAuthListenerWrapper(const sptr<StrongAuthListener>& listener);
    virtual ~StrongAuthListenerWrapper();
    void OnStrongAuthChanged(int userId, int authenticated) override;
    static std::shared_ptr<AppExecFwk::EventHandler> GetEventHandler();

private:
    sptr<StrongAuthListener> listener_;
    static std::shared_ptr<AppExecFwk::EventHandler> handler_;
};
} // namespace ScreenLock
} // namespace OHOS

#endif // SCREENLOCK_STRONGAUTH_WAPPER_H