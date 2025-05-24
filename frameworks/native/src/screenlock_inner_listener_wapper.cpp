/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "screenlock_inner_listener_wapper.h"

#include "sclock_log.h"
#include "screenlock_common.h"


namespace OHOS {
namespace ScreenLock {
std::shared_ptr<AppExecFwk::EventHandler> InnerListenerWrapper::handler_{ nullptr };


InnerListenerWrapper::InnerListenerWrapper(const sptr<InnerListener>& listener)
    : listener_(listener)
{
}

InnerListenerWrapper::~InnerListenerWrapper()
{
}

void InnerListenerWrapper::OnStateChanged(int userId, int state)
{
    if (listener_ == nullptr) {
        SCLOCK_HILOGE("eventHandler is nullptr");
        return;
    }

    auto ptrTemp = GetEventHandler();
    if (ptrTemp == nullptr) {
        SCLOCK_HILOGI("EventHandler nullptr");
        listener_->OnStateChanged(userId, state);
    } else {
        // 避免了 this 指针带来的潜在问题, 捕获必要的成员变量
        sptr<InnerListener> localListener = listener_;
        int localUserId = userId;
        int localState = state;

        auto task = [localListener, localUserId, localState]() {
            localListener->OnStateChanged(localUserId, localState);
        };
        ptrTemp->PostTask(task, "InnerListenerWrapper");
    }

    SCLOCK_HILOGD("OnStateChanged end");
    return;
}

std::shared_ptr<AppExecFwk::EventHandler> InnerListenerWrapper::GetEventHandler()
{
    if (handler_ == nullptr) {
        handler_ = AppExecFwk::EventHandler::Current();
    }
    return handler_;
}
}
}