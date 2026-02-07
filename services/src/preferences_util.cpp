/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "preferences_util.h"

#include "preferences.h"
#include "preferences_helper.h"
#include "preferences_observer.h"
#include "string"

namespace OHOS {
namespace ScreenLock {
PreferencesUtil::PreferencesUtil() {}
PreferencesUtil::~PreferencesUtil() {}

std::shared_ptr<NativePreferences::Preferences> PreferencesUtil::GetProfiles(const std::string &path, int &errCode)
{
    return NativePreferences::PreferencesHelper::GetPreferences(path, errCode);
}

int PreferencesUtil::DeleteProfiles()
{
    return NativePreferences::PreferencesHelper::DeletePreferences(path_);
}

int PreferencesUtil::SaveString(const std::string &key, const std::string &value)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    int ret = ptr->PutString(key, value);
    ptr->Flush();
    return ret;
}

std::string PreferencesUtil::ObtainString(const std::string &key, const std::string &defValue)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return defValue;
    }
    return ptr->GetString(key, defValue);
}

int PreferencesUtil::SaveInt(const std::string &key, int value)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    int ret = ptr->PutInt(key, value);
    ptr->Flush();
    return ret;
}

int PreferencesUtil::ObtainInt(const std::string &key, int defValue)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return defValue;
    }
    return ptr->GetInt(key, defValue);
}

int PreferencesUtil::SaveBool(const std::string &key, bool value)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    int ret = ptr->PutBool(key, value);
    ptr->Flush();
    return ret;
}

bool PreferencesUtil::ObtainBool(const std::string &key, bool defValue)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return defValue;
    }
    return ptr->GetBool(key, defValue);
}

int PreferencesUtil::SaveLong(const std::string &key, int64_t value)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    int ret = ptr->PutLong(key, value);
    ptr->Flush();
    return ret;
}

int64_t PreferencesUtil::ObtainLong(const std::string &key, int64_t defValue)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return defValue;
    }
    return ptr->GetLong(key, defValue);
}

int PreferencesUtil::SaveFloat(const std::string &key, float value)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    int ret = ptr->PutFloat(key, value);
    ptr->Flush();
    return ret;
}

float PreferencesUtil::ObtainFloat(const std::string &key, float defValue)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return defValue;
    }
    return ptr->GetFloat(key, defValue);
}

bool PreferencesUtil::IsExistKey(const std::string &key)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return false;
    }
    return ptr->HasKey(key);
}

int PreferencesUtil::RemoveKey(const std::string &key)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    return ptr->Delete(key);
}

int PreferencesUtil::RemoveAll()
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    return ptr->Clear();
}

void PreferencesUtil::Refresh()
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr != nullptr) {
        ptr->Flush();
    }
}

int PreferencesUtil::RefreshSync()
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetProfiles(path_, errCode_);
    if (ptr == nullptr) {
        return NativePreferences::E_ERROR;
    }
    return ptr->FlushSync();
}
} // namespace Telephony
} // namespace OHOS