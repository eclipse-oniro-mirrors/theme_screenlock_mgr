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

#include <memory>
#include <new>

#include "ani_screenlock_system_ability_callback.h"
#include "ani_screenlock_callback.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "ani_screenlock_util.h"

namespace OHOS {
namespace ScreenLock {
std::mutex ScreenlockSystemAbilityCallback::eventHandlerMutex_;
std::shared_ptr<AppExecFwk::EventHandler> ScreenlockSystemAbilityCallback::handler_{ nullptr };
ScreenlockSystemAbilityCallback::ScreenlockSystemAbilityCallback(const EventListener &eventListener)
    : eventListener_(eventListener)
{
}

ani_object GetSystemEventImpl(ani_env *env, std::string eventTypeStr, std::string paramsStr)
{
    if (env == nullptr) {
        SCLOCK_HILOGE("env is nullptr %{public}s", __func__);
        return nullptr;
    }

    ani_class cls;
    const char *className = "@ohos.screenLock.screenLock.SystemEventImpl";
    if (ANI_OK != env->FindClass(className, &cls)) {
        SCLOCK_HILOGE("Not found class name '%{public}s'", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        SCLOCK_HILOGE("Get ctor Failed");
        return nullptr;
    }

    ani_object object;
    if (ANI_OK != env->Object_New(cls, ctor, &object)) {
        SCLOCK_HILOGE("Object_New Failed");
        return nullptr;
    }

    ani_string eventType;
    env->String_NewUTF8(eventTypeStr.c_str(), eventTypeStr.size(), &eventType);
    if (ANI_OK != env->Object_SetPropertyByName_Ref(object, "eventType", eventType)) {
        SCLOCK_HILOGE("Object_SetPropertyByName_Ref eventType Failed");
        return nullptr;
    }
    ani_string params;
    env->String_NewUTF8(paramsStr.c_str(), paramsStr.size(), &params);
    if (ANI_OK != env->Object_SetPropertyByName_Ref(object, "params", params)) {
        SCLOCK_HILOGE("Object_SetPropertyByName_Ref params Failed");
        return nullptr;
    }

    return object;
}

void ScreenlockSystemAbilityCallback::OnCallBack(const SystemEvent &systemEvent)
{
    if (handler_ == nullptr) {
        SCLOCK_HILOGE("eventHandler is nullptr");
        return;
    }
    auto entry = std::make_shared<ScreenlockOnCallBack>();
    entry->vm = eventListener_.vm;
    entry->callbackRef = eventListener_.callbackRef;
    entry->systemEvent = systemEvent;
    auto task = [entry]() {
        ani_env *env = nullptr;
        env = AniScreenLockUtil::GetAniEnv(entry->vm);
        ani_size nr_refs = 16;
        env->CreateLocalScope(nr_refs);

        ani_ref result;
        auto fnObj = static_cast<ani_fn_object>(entry->callbackRef);
        if (fnObj == nullptr) {
            SCLOCK_HILOGE("%{public}s: fnObj == nullptr", __func__);
            return;
        }

        std::vector<ani_ref> args;
        auto argsObj = GetSystemEventImpl(env, entry->systemEvent.eventType_, entry->systemEvent.params_);
        args.push_back(argsObj);
        ani_status callStatus = env->FunctionalObject_Call(fnObj, args.size(), args.data(), &result);
        if (ANI_OK != callStatus) {
            SCLOCK_HILOGE("ani_call_function failed status : %{public}d", callStatus);
        }
        SCLOCK_HILOGI("OnCallBack eventType:%{public}s", entry->systemEvent.eventType_.c_str());
        env->DestroyLocalScope();
    };
    handler_->PostTask(task, "ScreenlockSystemAbilityCallback");
}

ScreenlockSystemAbilityCallback::~ScreenlockSystemAbilityCallback()
{
}

std::shared_ptr<AppExecFwk::EventHandler> ScreenlockSystemAbilityCallback::GetEventHandler()
{
    std::lock_guard<std::mutex> lock(eventHandlerMutex_);
    if (handler_ == nullptr) {
        handler_ = AppExecFwk::EventHandler::Current();
    }
    return handler_;
}
}  // namespace ScreenLock
} // namespace OHOS
