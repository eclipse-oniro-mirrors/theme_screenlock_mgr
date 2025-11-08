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
#ifndef WATCH_SETTINGS_MANAGER_H
#define WATCH_SETTINGS_MANAGER_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "data_ability_observer_stub.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "uri.h"

namespace OHOS {
namespace ScreenLock {

class SettingManager {
public:
    enum ResultCode {
        SUCCESS = 0,
        FAIL = -1
    };

    /**
     * 获取单实例句柄
     */
    static SettingManager &GetInstance()
    {
        static SettingManager settingManager;
        return settingManager;
    }

    /**
     * 注册观察者事件
     *
     * @param uri 相关模块的uri
     * @param dataObserver 相关模块观察者实例
     * @return true:注册成功 false:注册失败
     */
    bool RegisterSettingObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);

    /**
     * 解注册观察者事件
     *
     * @param uri 相关模块的uri
     * @param dataObserver 相关模块的观察者实例
     * @return true:解注册成功 false:解注册失败
     */
    bool UnRegisterSettingObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);

    /**
     * 查询数据
     *
     * @param uri 相关模块的uri
     * @param key 相关模块的键
     * @param value 默认值
     * @return 查询结果
     */
    ResultCode Query(Uri &uri, const std::string &key, std::string &value);

private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper();
    SettingManager();
};
} // namespace ScreenLock
} // namespace OHOS
#endif // WATCH_SETTINGS_MANAGER_H