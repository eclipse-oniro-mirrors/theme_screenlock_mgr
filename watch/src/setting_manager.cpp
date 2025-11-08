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
#include "setting_manager.h"
#include "sclock_log.h"
#include "values_bucket.h"

namespace OHOS {
namespace ScreenLock {
using namespace AppExecFwk;
const std::string SETTINGS_DATA_BASE_URI = "datashare:///com.ohos.settingsdata.DataAbility";
const std::string SEARCH_SETTING_URI = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
const std::string SETTING_KEY = "KEYWORD";
const std::string SETTING_VALUE = "VALUE";
std::recursive_mutex dataShareOperatorLock;

SettingManager::SettingManager()
{}

std::shared_ptr<DataShare::DataShareHelper> SettingManager::CreateDataShareHelper()
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        SCLOCK_HILOGE("GetSystemAibityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        SCLOCK_HILOGE("GetSystemAbility Service Failed");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, SEARCH_SETTING_URI, SETTINGS_DATA_BASE_URI);
}

bool SettingManager::UnRegisterSettingObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    std::lock_guard<std::recursive_mutex> lock(dataShareOperatorLock);
    std::shared_ptr<DataShare::DataShareHelper> settingHelper = CreateDataShareHelper();
    if (settingHelper == nullptr) {
        SCLOCK_HILOGE("settingHelper is null");
        return false;
    }
    int result = settingHelper->UnregisterObserver(uri, dataObserver);
    if (result != DataShare::E_OK) {
        SCLOCK_HILOGW("UnregisterObserver code:%{public}d", result);
        return false;
    }
    SCLOCK_HILOGI("UnRegisterObserver Success");
    return true;
}

bool SettingManager::RegisterSettingObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    std::lock_guard<std::recursive_mutex> lock(dataShareOperatorLock);
    std::shared_ptr<DataShare::DataShareHelper> settingHelper = CreateDataShareHelper();
    if (settingHelper == nullptr) {
        SCLOCK_HILOGE("settingHelper is null");
        return false;
    }
    int result = settingHelper->RegisterObserver(uri, dataObserver);
    if (result != DataShare::E_OK) {
        SCLOCK_HILOGW("RegisterObserver code:%{public}d", result);
        return false;
    }
    SCLOCK_HILOGD("RegisterObserver Success");
    return true;
}

SettingManager::ResultCode SettingManager::Query(Uri &uri, const std::string &key, std::string &value)
{
    std::lock_guard<std::recursive_mutex> lock(dataShareOperatorLock);
    std::shared_ptr<DataShare::DataShareHelper> settingHelper = CreateDataShareHelper();
    if (settingHelper == nullptr) {
        SCLOCK_HILOGE("settingHelper is null");
        return FAIL;
    }
    DataShare::DatashareBusinessError errorCode;
    std::vector<std::string> columns = {SETTING_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_KEY, key);
    auto result = settingHelper->Query(uri, predicates, columns, &errorCode);
    if (result == nullptr) {
        const char *message = errorCode.GetMessage().c_str();
        SCLOCK_HILOGW("query error, code:%{public}d message:%{public}s", errorCode.GetCode(), message);
        return FAIL;
    }
    int code = result->GoToFirstRow();
    if (code != DataShare::E_OK) {
        SCLOCK_HILOGW("query error, code:%{public}d", code);
        result->Close();
        return FAIL;
    }
    int columnIndex = 0;
    code = result->GetColumnIndex(SETTING_VALUE, columnIndex);
    if (code != DataShare::E_OK) {
        SCLOCK_HILOGW("GetColumnIndex error, code:%{public}d", code);
        result->Close();
        return FAIL;
    }
    try {
        code = result->GetString(columnIndex, value);
        if (code != DataShare::E_OK) {
            SCLOCK_HILOGW("GetString error, code:%{public}d", code);
            result->Close();
            return FAIL;
        }
        result->Close();
        SCLOCK_HILOGD("query success");
        return SUCCESS;
    } catch (...) {
        SCLOCK_HILOGE("query fail");
        result->Close();
        return FAIL;
    }
}
} // namespace ScreenLock
} // namespace OHOS