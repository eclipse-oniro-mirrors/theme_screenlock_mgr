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
#include <random>
#include "watch_applock_manager.h"
#include "sclock_log.h"
#include "ipc_skeleton.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "hce_service.h"
#include "setting_manager.h"
#include "user_auth_client_defines.h"
#include "user_auth_client.h"
#include "os_account_manager.h"
#include "app_mgr_client.h"
#include "syspara/parameters.h"
#include "screenlock_common.h"
#include "hisysevent_report.h"

namespace OHOS {
namespace ScreenLock {
WatchAppLockManager::WatchAppLockManager()
{}

WatchAppLockManager::~WatchAppLockManager()
{
    unlockedRecord.clear();
}

bool WatchAppLockManager::isSecureMode()
{
    bool hasPin = HasPin();
    SCLOCK_HILOGI("hasPin = %{public}s", boolToString(hasPin).c_str());
    return hasPin;
}

bool WatchAppLockManager::IsScreenLocked(bool isOHScreenLocked)
{
    bool isLeaveWrist = IsLeaveWrist();
    bool isScreenLocked = false;
    if (isLeaveWrist) {
        isScreenLocked = isOHScreenLocked;
    } else {
        uint32_t callingUid = IPCSkeleton::GetCallingUid();
        std::string bundleName = GetBundleNameByUid(callingUid);
        isScreenLocked = !unlockedRecord.contains(bundleName);
    }
    SCLOCK_HILOGI("isLeaveWrist = %{public}s, isOHScreenLocked = %{public}s,isScreenLocked = %{public}s",
        boolToString(isLeaveWrist).c_str(),
        boolToString(isOHScreenLocked).c_str(),
        boolToString(isScreenLocked).c_str());
    return isScreenLocked;
}

int32_t WatchAppLockManager::unlockScreen(bool isScreenLocked)
{
    if (!isScreenLocked || !HasPin()) {
        SCLOCK_HILOGI("screen does not need to be unlocked");
        return E_SCREENLOCK_NOT_FOCUS_APP;
    }
    uint32_t callingUid = IPCSkeleton::GetCallingUid();
    std::string bundleName = GetBundleNameByUid(callingUid);
    bool isStartIAM = BeginWidgetAuth();
    if (isStartIAM) {
        unlockedRecord.add(bundleName);
    }
    SCLOCK_HILOGI("isStartIAM = %{public}s", boolToString(isStartIAM).c_str());
    return isStartIAM ? E_SCREENLOCK_OK : E_SCREENLOCK_NOT_FOCUS_APP;
}

void WatchAppLockManager::AppStateObserver::OnProcessStateChanged(const AppExecFwk::ProcessData &processData)
{}

void WatchAppLockManager::AppStateObserver::OnProcessCreated(const AppExecFwk::ProcessData &processData)
{}

void WatchAppLockManager::AppStateObserver::OnProcessDied(const AppExecFwk::ProcessData &processData)
{
    if (WatchAppLockManager::GetInstance().unlockedRecord.remove(processData.bundleName)) {
        SCLOCK_HILOGI("process=%{public}s died", processData.bundleName.c_str());
    }
}

void WatchAppLockManager::AppStateObserver::OnWindowShow(const AppExecFwk::ProcessData &processData)
{}

void WatchAppLockManager::AppStateObserver::OnWindowHidden(const AppExecFwk::ProcessData &processData)
{}

void WatchAppLockManager::LeaveWristSettingObserver::OnChange()
{
    if (!WatchAppLockManager::GetInstance().IsLeaveWrist()) {
        SCLOCK_HILOGI("clear unlockedRecord");
        WatchAppLockManager::GetInstance().unlockedRecord.clear();
    }
}

bool WatchAppLockManager::UnlockedRecord::add(const std::string &element)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    if (elements.empty()) {
        WatchAppLockManager::GetInstance().registerAppStateObserver();
        WatchAppLockManager::GetInstance().registerLeaveWristSettingObserver();
    }
    auto result = elements.insert(element);
    return result.second;
}

bool WatchAppLockManager::UnlockedRecord::remove(const std::string &element)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    size_t count = elements.erase(element);
    if (elements.empty()) {
        WatchAppLockManager::GetInstance().unregisterAppStateObserver();
        WatchAppLockManager::GetInstance().unregisterLeaveWristSettingObserver();
    }
    return count > 0;
}

bool WatchAppLockManager::UnlockedRecord::contains(const std::string &element) const
{
    std::shared_lock<std::shared_mutex> lock(mtx);
    return elements.find(element) != elements.end();
}

void WatchAppLockManager::UnlockedRecord::clear()
{
    std::lock_guard<std::shared_mutex> lock(mtx);
    elements.clear();
    if (elements.empty()) {
        WatchAppLockManager::GetInstance().unregisterAppStateObserver();
        WatchAppLockManager::GetInstance().unregisterLeaveWristSettingObserver();
    }
}

std::string WatchAppLockManager::GetBundleNameByUid(uint32_t uid)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        SCLOCK_HILOGE("get SystemAbilityManager failed");
        return "";
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        SCLOCK_HILOGE("get Bundle Manager failed");
        return "";
    }
    auto bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgr == nullptr) {
        SCLOCK_HILOGE("get Bundle Manager nullptr");
        return "";
    }
    std::string callerBundleName;
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    ErrCode err = bundleMgr->GetNameForUid(uid, callerBundleName);
    IPCSkeleton::SetCallingIdentity(identity);
    SCLOCK_HILOGD("err=%{public}d, callerBundleName=%{public}s, uid=%{public}d", err, callerBundleName.c_str(), uid);
    return callerBundleName;
}

std::vector<uint8_t> WatchAppLockManager::GenerateRandom(int32_t len)
{
    std::random_device rd;
    std::vector<uint8_t> data;
    for (int index = 0; index < len; index++) {
        unsigned int randomValue = rd();
        // 256取模后，范围是0~255
        data.push_back(static_cast<uint8_t>(randomValue % 256));
    }
    return data;
}

void WatchAppLockManager::GetPaymentServices(std::vector<AppExecFwk::AbilityInfo> &paymentAbilityInfos)
{
    NFC::KITS::HceService &hceService = NFC::KITS::HceService::GetInstance();
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    int errCode = hceService.GetPaymentServices(paymentAbilityInfos);
    IPCSkeleton::SetCallingIdentity(identity);
    if (errCode != 0) {
        std::string bundleName = GetBundleNameByUid(IPCSkeleton::GetCallingUid());
        std::string errCodeStr = std::to_string(errCode);
        HiSysEventReport::GetInstance().ReportFaultCode(bundleName, GET_PAYMENT_ERROR_CODE, errCodeStr);
        SCLOCK_HILOGI("statusCode,%{public}d,size, %{public}zu", errCode, paymentAbilityInfos.size());
    }
}

std::string WatchAppLockManager::GetSettingsValue(
    const std::string &addressUrl, const std::string &key, const std::string &defaultValue)
{
    Uri hwUri(addressUrl);
    std::string value;
    if (SettingManager::GetInstance().Query(hwUri, key, value) != 0) {
        SCLOCK_HILOGE("Query key:%{public}s Fail", key.c_str());
        return defaultValue;
    } else {
        SCLOCK_HILOGI("Query key:%{public}s success, value:%{public}s", key.c_str(), value.c_str());
    }
    return value;
}

bool WatchAppLockManager::IsDeviceScope()
{
    std::string passwordScope = GetSettingsValue(SETTING_USER_SECURE_URI, PASSWORD_SCOPE, SETTING_DISENABLE_VALUE);
    SCLOCK_HILOGI("password scope, %{public}s", passwordScope.c_str());
    return passwordScope == SETTING_ENABLE_VALUE;
}

bool WatchAppLockManager::IsLeaveWrist()
{
    std::string hwLeaveWrist = GetSettingsValue(SETTING_DEVICE_SHARED_URI, HW_LEAVE_WRIST, SETTING_DISENABLE_VALUE);
    SCLOCK_HILOGI("leave wrist, %{public}s", hwLeaveWrist.c_str());
    return hwLeaveWrist == SETTING_ENABLE_VALUE;
}

bool WatchAppLockManager::BeginWidgetAuth()
{
    // 拉起授权页面
    UserIam::UserAuth::ReuseUnlockResult reuseUnlockResult_{
        .isReuse = false,
        .reuseMode = UserIam::UserAuth::ReuseMode::AUTH_TYPE_IRRELEVANT,
        .reuseDuration = 0,
    };
    UserIam::UserAuth::WidgetAuthParam authParamInner{
        .userId = GetUserIdFromCallingUid(),
        .challenge = GenerateRandom(16),
        .authTypes = {UserIam::UserAuth::AuthType::PIN},
        .authTrustLevel = UserIam::UserAuth::AuthTrustLevel::ATL3,
        .reuseUnlockResult = reuseUnlockResult_,
    };
    std::string titleStr = "  ";
    UserIam::UserAuth::WidgetParam widgetParam = {
        .title = titleStr,
        .windowMode = UserIam::UserAuth::WindowModeType::UNKNOWN_WINDOW_MODE,
    };
    auto callBack = std::make_shared<WatchAppLockManager::AuthCallback>();
    auto authContextId =
        UserIam::UserAuth::UserAuthClient::GetInstance().BeginWidgetAuth(authParamInner, widgetParam, callBack);
    auto isAuthing = true;
    int32_t resultCode = callBack->WaitAuthFinish();
    SCLOCK_HILOGI("resultCode %{public}d.", resultCode);
    bool isStart = resultCode == 0;
    if (!isStart) {
        std::string bundleName = GetBundleNameByUid(IPCSkeleton::GetCallingUid());
        std::string resultCodeStr = std::to_string(resultCode);
        HiSysEventReport::GetInstance().ReportFaultCode(bundleName, START_IAM_ERROR_CODE, resultCodeStr);
    }
    return isStart;
}

void WatchAppLockManager::registerAppStateObserver()
{
    appStateObserver_ = sptr<AppStateObserver>::MakeSptr();
    if (appStateObserver_ == nullptr) {
        SCLOCK_HILOGE("observer is null");
        return;
    }

    auto appMgrClient = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient == nullptr) {
        SCLOCK_HILOGE("failed to unregister observer, appMgrClient is nullptr");
        return;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto res = appMgrClient->RegisterApplicationStateObserver(appStateObserver_);
    IPCSkeleton::SetCallingIdentity(identity);
    if (res != ERR_OK) {
        SCLOCK_HILOGI("failed to register observer, res=%{public}d", res);
        return;
    }
    SCLOCK_HILOGI("succ to register observer");
}

void WatchAppLockManager::unregisterAppStateObserver()
{
    if (appStateObserver_ == nullptr) {
        return;
    }
    auto appMgrClient = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient == nullptr) {
        SCLOCK_HILOGE("failed to unregister observer, appMgrClient is nullptr");
        return;
    }
    auto res = appMgrClient->UnregisterApplicationStateObserver(appStateObserver_);
    if (res != ERR_OK) {
        SCLOCK_HILOGI("failed to unregister observer, res=%{public}d", res);
        return;
    }
    appStateObserver_ = nullptr;
    SCLOCK_HILOGI("succ to unregister observer");
}

void WatchAppLockManager::registerLeaveWristSettingObserver()
{
    leaveWristSettingObserver_  = sptr<LeaveWristSettingObserver>::MakeSptr();
    if (leaveWristSettingObserver_ == nullptr) {
        SCLOCK_HILOGE("observer is null");
        return;
    }
    std::string completeValue = SETTING_DEVICE_SHARED_URI + AND_KEY + HW_LEAVE_WRIST;
    Uri completeUri(completeValue);
    SettingManager::GetInstance().RegisterSettingObserver(completeUri, leaveWristSettingObserver_);
}

void WatchAppLockManager::unregisterLeaveWristSettingObserver()
{
    if (leaveWristSettingObserver_ == nullptr) {
        return;
    }
    std::string completeValue = SETTING_DEVICE_SHARED_URI + AND_KEY + HW_LEAVE_WRIST;
    Uri completeUri(completeValue);
    SettingManager::GetInstance().UnRegisterSettingObserver(completeUri, leaveWristSettingObserver_);
    leaveWristSettingObserver_ = nullptr;
}

bool WatchAppLockManager::HasPin()
{
    bool hasPin = OHOS::system::GetParameter(IS_PIN_ENROLLED, STATE_FALSE) == STATE_TRUE;
    SCLOCK_HILOGI("%{public}s", boolToString(hasPin).c_str());
    return hasPin;
}

int32_t WatchAppLockManager::GetUserIdFromCallingUid()
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

bool WatchAppLockManager::IsPaymentApp()
{
    uint32_t callingUid = IPCSkeleton::GetCallingUid();
    std::string bundleName = GetBundleNameByUid(callingUid);
    if (bundleName.empty()) {
        return false;
    }
    std::vector<AppExecFwk::AbilityInfo> abilityInfos;
    GetPaymentServices(abilityInfos);

    auto it = std::find_if(abilityInfos.begin(), abilityInfos.end(), [=](const AppExecFwk::AbilityInfo &abilityInfo) {
        SCLOCK_HILOGD("%{public}s", abilityInfo.bundleName.c_str());
        return abilityInfo.bundleName == bundleName;
    });
    bool isPaymentApp = it != abilityInfos.end();
    SCLOCK_HILOGD("%{public}s", boolToString(isPaymentApp).c_str());
    return isPaymentApp;
}
std::string WatchAppLockManager::boolToString(bool value)
{
    return value ? STATE_TRUE : STATE_FALSE;
}
} // namespace ScreenLock
} // namespace OHOS