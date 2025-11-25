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
#include <cstdint>
#include <new>
#include <string>

#include "ani_screenlock_callback.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "string_ex.h"
#include "ani_screenlock_util.h"
#include "ani_error_handler.h"

namespace OHOS {
namespace ScreenLock {
constexpr const char *CANCEL_UNLOCK_OPERATION = "The user canceled the unlock operation.";
constexpr const char *SCREENLOCK_FAIL = "ScreenLock failed.";
enum class ARG_INFO { ARG_ERROR, ARG_DATA, ARG_BUTT };
std::shared_ptr<AppExecFwk::EventHandler> ScreenlockCallback::handler_{ nullptr };

ScreenlockCallback::ScreenlockCallback(const EventListener &eventListener)
{
    eventListener_ = eventListener;
}

ScreenlockCallback::~ScreenlockCallback()
{
}

void ScreenlockCallback::OnCallBack(const int32_t screenLockResult)
{
    SCLOCK_HILOGE("ScreenlockCallback OnCallBack in");
    std::shared_ptr<ScreenlockOnCallBack> screenlockOnCallBack = std::make_shared<ScreenlockOnCallBack>();
    if (screenlockOnCallBack == nullptr) {
        SCLOCK_HILOGE("new  ScreenlockOnCallBack failed");
        return;
    }
    if (screenLockResult == SCREEN_CANCEL) {
        errorInfo_.message_ = CANCEL_UNLOCK_OPERATION;
    } else if (screenLockResult == SCREEN_FAIL) {
        errorInfo_.message_ = SCREENLOCK_FAIL;
    }
    screenlockOnCallBack->vm = eventListener_.vm;
    screenlockOnCallBack->callbackRef = eventListener_.callbackRef;
    screenlockOnCallBack->resolver = eventListener_.resolver;
    screenlockOnCallBack->action = eventListener_.action;
    screenlockOnCallBack->errorInfo = errorInfo_;
    screenlockOnCallBack->screenLockResult = screenLockResult;
    SendCallBackEvent(screenlockOnCallBack);
}

void ScreenlockCallback::SendCallBackEvent(std::shared_ptr<ScreenlockOnCallBack> screenlockOnCallBack)
{
    auto task = [screenlockOnCallBack]() {
        SCLOCK_HILOGE("task excute");
        if (screenlockOnCallBack->vm == nullptr) {
            SCLOCK_HILOGE("VM is nullptr");
            return;
        }

        ani_env *env = nullptr;
        env = AniScreenLockUtil::GetAniEnv(screenlockOnCallBack->vm);
        if (env == nullptr) {
            SCLOCK_HILOGE("failed to GetAniEnv");
            return;
        }

        ani_size nr_refs = 16;
        env->CreateLocalScope(nr_refs);

        bool screenLockSuccess = screenlockOnCallBack->screenLockResult == SCREEN_SUCC;
        bool cancelUnlock = (screenlockOnCallBack->action == Action::UNLOCK && screenlockOnCallBack->screenLockResult == SCREEN_CANCEL);
        ani_status status = ANI_OK;

        if (screenLockSuccess || cancelUnlock) {
            ani_object ret = AniScreenLockUtil::CreateBoolean(env, screenLockSuccess);
            if (ANI_OK != (status = env->PromiseResolver_Resolve(screenlockOnCallBack->resolver, static_cast<ani_ref>(ret)))) {
                SCLOCK_HILOGE("PromiseResolver_Resolve faild. status %{public}d", status);
            }
        } else {
            ani_error rejection = static_cast<ani_error>(
                ErrorHandler::CreateError(env, screenlockOnCallBack->errorInfo.errorCode_, screenlockOnCallBack->errorInfo.message_));
            if (ANI_OK != (status = env->PromiseResolver_Reject(screenlockOnCallBack->resolver, rejection))) {
                SCLOCK_HILOGE("PromiseResolver_Resolve faild. status %{public}d", status);
            }
        }
        env->DestroyLocalScope();
    };
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
    if (runner == nullptr) {
        SCLOCK_HILOGE("invalid main event runner.");
    }
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    auto state = handler_->PostTask(task, "", 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    SCLOCK_HILOGE("task PostTask ret: %{public}d", state);
}

void ScreenlockCallback::SetErrorInfo(const ErrorInfo &errorInfo)
{
    errorInfo_ = errorInfo;
}
} // namespace ScreenLock
} // namespace OHOS
