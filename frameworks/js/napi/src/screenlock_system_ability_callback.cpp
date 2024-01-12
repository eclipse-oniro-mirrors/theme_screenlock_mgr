/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "screenlock_system_ability_callback.h"

#include <memory>
#include <new>

#include "js_native_api.h"
#include "js_native_api_types.h"
#include "node_api.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "uv_queue.h"

namespace OHOS {
namespace ScreenLock {
std::mutex ScreenlockSystemAbilityCallback::eventHandlerMutex_;
std::shared_ptr<AppExecFwk::EventHandler> ScreenlockSystemAbilityCallback::handler_{ nullptr };
ScreenlockSystemAbilityCallback::ScreenlockSystemAbilityCallback(const EventListener &eventListener)
    : eventListener_(eventListener)
{
}

ScreenlockSystemAbilityCallback::~ScreenlockSystemAbilityCallback()
{
}

void ScreenlockSystemAbilityCallback::OnCallBack(const SystemEvent &systemEvent)
{
    if (handler_ == nullptr) {
        SCLOCK_HILOGE("eventHandler is nullptr");
        return;
    }
    auto entry = std::make_shared<ScreenlockOnCallBack>();
    entry->env = eventListener_.env;
    entry->callbackRef = eventListener_.callbackRef;
    entry->systemEvent = systemEvent;
    auto task = [entry]() {
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(entry->env, &scope);
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(entry->env, entry->callbackRef, &callbackFunc);
        napi_value result = nullptr;
        napi_create_object(entry->env, &result);
        napi_value eventType = nullptr;
        napi_value params = nullptr;
        napi_create_string_utf8(entry->env, entry->systemEvent.eventType_.c_str(), NAPI_AUTO_LENGTH, &eventType);
        napi_create_string_utf8(entry->env, entry->systemEvent.params_.c_str(), NAPI_AUTO_LENGTH, &params);
        napi_set_named_property(entry->env, result, "eventType", eventType);
        napi_set_named_property(entry->env, result, "params", params);
        napi_value output = nullptr;
        napi_call_function(entry->env, nullptr, callbackFunc, ARGS_SIZE_ONE, &result, &output);
        SCLOCK_HILOGI("OnCallBack eventType:%{public}s", entry->systemEvent.eventType_.c_str());
        napi_close_handle_scope(entry->env, scope);
    };
    handler_->PostTask(task, "ScreenlockSystemAbilityCallback");
}

std::shared_ptr<AppExecFwk::EventHandler> ScreenlockSystemAbilityCallback::GetEventHandler()
{
    std::lock_guard<std::mutex> lock(eventHandlerMutex_);
    if (handler_ == nullptr) {
        handler_ = AppExecFwk::EventHandler::Current();
    }
    return handler_;
}
} // namespace ScreenLock
} // namespace OHOS
