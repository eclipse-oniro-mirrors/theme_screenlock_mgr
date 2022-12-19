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
#include "screenlock_get_info_callback.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
void ScreenLockGetInfoCallback::OnCredentialInfo(const std::vector<OHOS::UserIam::UserAuth::CredentialInfo> &infoList)
{
    SCLOCK_HILOGI("I have been called.");
    if (infoList.size() > 0) {
        SCLOCK_HILOGD("infoList.size() = %{public}zu", infoList.size());
        isSecure = true;
    }
}
bool ScreenLockGetInfoCallback::IsSecure() const
{
    return isSecure;
}
} // namespace ScreenLock
} // namespace OHOS