/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "screenlock_hisysevent_adapter.h"
#include "sclock_log.h"
#include "hisysevent.h"

namespace OHOS {
namespace ScreenLock {
namespace {
using HiSysEventNameSpace = OHOS::HiviewDFX::HiSysEvent;
const std::string DOMAIN_STR = std::string(HiSysEventNameSpace::Domain::ACCOUNT);
}

void ReportServiceFault(int32_t errType, int32_t errCode, int32_t userId, const std::string& msg)
{
    int ret = HiSysEventNameSpace::Write(DOMAIN_STR, "SERVICE_FAULT",
        HiSysEventNameSpace::EventType::FAULT,
        "ERROR_TYPE", errType,
        "ERROR_COED", errCode,
        "USER_ID", userId,
        "MSG", msg);
    if (ret != 0) {
        SCLOCK_HILOGE("hisysevent write failed! errType %{public}d. errCode %{public}d userId %{public}d msg %{public}s" , errType, errCode, userId, msg.c_str());
    }
}

void ReportRuntimeFault(int32_t errType, int32_t errCode)
{
    int ret = HiSysEventNameSpace::Write(DOMAIN_STR, "RUNTIME_FAULT",
        HiSysEventNameSpace::EventType::FAULT,
        "ERROR_TYPE", errType,
        "ERROR_CODE", errCode);
    if (ret != 0) {
        SCLOCK_HILOGE("hisysevent write failed! errType %{public}d. errCode %{public}d", errType, errCode);
    }
}
} // ScreenLock
} // OHOS
