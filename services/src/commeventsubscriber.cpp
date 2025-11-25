/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "commeventsubscriber.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "preferences_util.h"

namespace OHOS {
namespace ScreenLock {
const std::string AUTH_PIN = "1";
const std::string HAS_NO_CREDENTIAL = "0";
const std::string TAG_USERID = "userId";
const std::string TAG_AUTHTYPE = "authType";
const std::string TAG_CREDENTIALCOUNT = "credentialCount";
const std::string USER_CREDENTIAL_UPDATED_EVENT = "USER_CREDENTIAL_UPDATED_EVENT";

CommeventMgr::CommeventMgr() {}

CommeventMgr::~CommeventMgr()
{
    UnSubscribeEvent();
}

void CommeventMgr::OnReceiveEvent(const AAFwk::Want &want)
{
    std::string action = want.GetAction();
    SCLOCK_HILOGI("recive param update event: %{public}s", action.c_str());
    if (action == USER_CREDENTIAL_UPDATED_EVENT) {
        std::string userId = want.GetStringParam(TAG_USERID);
        std::string authType = want.GetStringParam(TAG_AUTHTYPE);
        std::string credentialCount = want.GetStringParam(TAG_CREDENTIALCOUNT);
        if (authType == AUTH_PIN && credentialCount != HAS_NO_CREDENTIAL) {
            SCLOCK_HILOGI("set passwd");
            auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
            if (preferencesUtil == nullptr) {
                SCLOCK_HILOGE("preferencesUtil is nullptr!");
                return;
            }
            preferencesUtil->RemoveKey(userId);
            preferencesUtil->Refresh();
        }
    }
}

void CommeventMgr::SubscribeEvent()
{
    SCLOCK_HILOGD("SubscribeEvent start.");
    std::lock_guard<std::mutex> autoLock(lock_);
    if (subscriber_) {
        SCLOCK_HILOGI("Common Event is already subscribered.");
        return;
    }

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(USER_CREDENTIAL_UPDATED_EVENT);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPermission("ohos.permission.MANAGE_USER_IDM");
    subscriber_ = std::make_shared<CommEventSubscriber>(subscribeInfo, *this);

    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
    if (!subscribeResult) {
        SCLOCK_HILOGE("SubscribeEvent failed.");
        subscriber_ = nullptr;
    }
    return;
}

void CommeventMgr::UnSubscribeEvent()
{
    std::lock_guard<std::mutex> autoLock(lock_);
    if (subscriber_) {
        bool subscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        SCLOCK_HILOGI("subscribeResult = %{public}d", subscribeResult);
        subscriber_ = nullptr;
    }
    return;
}
} // namespace ScreenLock
} // namespace OHOS