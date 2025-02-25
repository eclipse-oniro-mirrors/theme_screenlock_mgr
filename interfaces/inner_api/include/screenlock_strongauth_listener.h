/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef I_SCREENLOCK_STRONG_AUTH_LISTENER_H
#define I_SCREENLOCK_STRONG_AUTH_LISTENER_H

#include "iremote_broker.h"
#include "iremote_object.h"

namespace OHOS {
namespace ScreenLock {

class StrongAuthListener : public virtual RefBase {
public:
    explicit StrongAuthListener(int32_t userId) : userId_(userId) {}
    virtual ~StrongAuthListener() = default;
    virtual void OnStrongAuthChanged(int32_t userId, int32_t authenticated) = 0;

public:
    int32_t GetUserId() { return userId_; }

private:
    int32_t userId_ = -10000;
};
} // namespace ScreenLock
} // namespace OHOS

#endif // I_SCREENLOCK_STRONG_AUTH_LISTENER_H