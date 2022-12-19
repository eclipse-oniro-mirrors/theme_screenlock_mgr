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

#ifndef UTILS_INCLUDE_SCLOCK_APPINFO_H
#define UTILS_INCLUDE_SCLOCK_APPINFO_H

#include <string>

namespace OHOS {
namespace ScreenLock {
struct AppInfo {
    std::string bundleName;
    std::string appId;
};

class ScreenLockAppInfo {
public:
    ScreenLockAppInfo() = default;
    ~ScreenLockAppInfo() = default;
    static bool GetAppInfoByToken(std::uint32_t tokenId, AppInfo &appInfo);
};
} // namespace ScreenLock
} // namespace OHOS

#endif // UTILS_INCLUDE_SCLOCK_APPINFO_H
