/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "hisysevent_report.h"

namespace OHOS {
namespace ScreenLock {
void HiSysEventReport::SendEvent(HiSysEventEventType type, std::string eventName,
    std::map<std::string, std::variant<int32_t, std::string>> &eventMap)
{
    SCLOCK_HILOGI("SendEvent myMap size: %{public}lu, eventName: %{public}s, type: %{public}d",
        eventMap.size(),
        eventName.c_str(),
        type);
    std::vector<HiSysEventParam> hiSysEventParams;
    std::vector<std::vector<char>> stringBuffers;
    for (const auto &event : eventMap) {
        HiSysEventParam param;
        size_t count = event.first.size();
        if (count >= sizeof(param.name)) {
            count = sizeof(param.name) - 1;
            SCLOCK_HILOGW("key truncated: %{public}s", event.first.c_str());
        }
        errno_t ret = strncpy_s(param.name, sizeof(param.name), event.first.c_str(), count);
        if (ret != EOK) {
            SCLOCK_HILOGI("strncpy_s error");
            continue;
        }
        if (const int32_t *value = std::get_if<int32_t>(&event.second)) {
            SCLOCK_HILOGI("SendEvent eventValue: %{public}d", *value);
            param.t = HISYSEVENT_INT32;
            param.v.i32 = *value;
        } else if (const std::string *value = std::get_if<std::string>(&event.second)) {
            SCLOCK_HILOGI("SendEvent eventValue: %{public}s", value->c_str());
            param.t = HISYSEVENT_STRING;
            stringBuffers.emplace_back(value->begin(), value->end());
            stringBuffers.back().push_back('\0');
            param.v.s = stringBuffers.back().data();
        } else {
            SCLOCK_HILOGW("not support param type");
            continue;
        }
        hiSysEventParams.push_back(param);
    }
    HiSysEventParam eventParamArr[hiSysEventParams.size()];
    std::copy(hiSysEventParams.begin(), hiSysEventParams.end(), eventParamArr);
    SCLOCK_HILOGI("SendEvent OH_HiSysEvent_Write enter");
    int ret = OH_HiSysEvent_Write(WATCH_LOCKSCREEN, eventName.c_str(), type, eventParamArr, hiSysEventParams.size());
    SCLOCK_HILOGI("SendEvent write sysevent ret: %{public}d", ret);
}

void HiSysEventReport::ReportFaultCode(const std::string &bundleName, int32_t code, const std::string &reason)
{
    std::map<std::string, std::variant<int32_t, std::string>> eventMap;
    eventMap[HiSysEventReport::BUNDLE_NAME] = bundleName;
    eventMap[HiSysEventReport::ERROR_CODE] = code;
    eventMap[HiSysEventReport::ERROR_REASON] = reason;
    HiSysEventReport::GetInstance().SendEvent(
        HiSysEventEventType::HISYSEVENT_FAULT, HiSysEventReport::SCREEN_LOCK_ERROR, eventMap);
}
} // namespace ScreenLock
} // namespace OHOS