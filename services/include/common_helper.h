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

#ifndef SCREENLOCK_COMMON_HELPER_H
#define SCREENLOCK_COMMON_HELPER_H

#include "ipc_skeleton.h"
#include "os_account_manager.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {

static int32_t GetUserIdFromCallingUid()
{
    int callingUid = IPCSkeleton::GetCallingUid();
    SCLOCK_HILOGD("callingUid=%{public}d", callingUid);
    int userId = 0;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (userId == 0) {
        AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
    }
    SCLOCK_HILOGD("userId=%{public}d", userId);
    return userId;
}
}
}
#endif