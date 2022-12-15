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
#include "screenlock_appinfo.h"

#include "accesstoken_kit.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
using namespace Security::AccessToken;
bool ScreenLockAppInfo::GetAppInfoByToken(std::uint32_t tokenId, AppInfo &appInfo)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    switch (tokenType) {
        case ATokenTypeEnum::TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                SCLOCK_HILOGE("get hap token info fail");
                return false;
            }
            appInfo.bundleName = hapInfo.bundleName;
            appInfo.appId = hapInfo.appID;
            return true;
        }
        case ATokenTypeEnum::TOKEN_NATIVE:
        case ATokenTypeEnum::TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                SCLOCK_HILOGE("get native token info fail");
                return false;
            }
            appInfo.bundleName = tokenInfo.processName;
            appInfo.appId = tokenInfo.processName;
            return true;
        }
        default: {
            SCLOCK_HILOGI("token type not match");
            break;
        }
    }
    return false;
}
} // namespace ScreenLock
} // namespace OHOS
