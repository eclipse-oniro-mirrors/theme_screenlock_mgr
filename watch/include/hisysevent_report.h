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
#ifndef DFX_HI_SYS_EVENT_REPORT
#define DFX_HI_SYS_EVENT_REPORT
#include <variant>
#include <string>
#include <map>
#include "hisysevent_c.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {

class HiSysEventReport {
public:
    static constexpr char WATCH_LOCKSCREEN[] = "WATCH_LOCKSCREEN";
    static constexpr char SCREEN_LOCK_ERROR[] = "SCREEN_LOCK_ERROR";
    static constexpr char BUNDLE_NAME[] = "BUNDLE_NAME";
    static constexpr char ERROR_REASON[] = "ERROR_REASON";
    static constexpr char ERROR_CODE[] = "ERROR_CODE";

    /**
     * 返回单例的模块对象
     */
    static HiSysEventReport &GetInstance()
    {
        static HiSysEventReport instance;
        return instance;
    }

    /**
     * 发送打点事件
     *
     * @param type 事件类型
     * @param eventName 事件名称
     * @param myMap 事件参数，键值对形式存储事件的附加信息
     */
    void SendEvent(HiSysEventEventType type, std::string eventName,
        std::map<std::string, std::variant<int32_t, std::string>>& myMap);

    /**
     * 上报故障码
     *
     * @param bundleName 应用包名
     * @param code 故障码
     * @param reason 故障原因
     */
    void ReportFaultCode(const std::string& bundleName, int32_t code, const std::string& reason);
};
} // namespace ScreenLock
} // namespace OHOS
#endif // DFX_HI_SYS_EVENT_REPORT