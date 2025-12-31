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
#ifndef SCREENLOCK_WATCH_APPLOCK_MANAGER_H
#define SCREENLOCK_WATCH_APPLOCK_MANAGER_H

#include <mutex>
#include <string>
#include <singleton.h>
#include "application_state_observer_stub.h"
#include "user_auth_client.h"
#include "user_auth_client_callback.h"
#include "user_auth_client_defines.h"
#include "ffrt.h"
#include "data_ability_observer_stub.h"

namespace OHOS {
namespace ScreenLock {
// 设备属性共享域
const std::string SETTING_DEVICE_SHARED_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
// 用户安全属性域（仅对系统应用开放）
const std::string SETTING_USER_SECURE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_100?Proxy=true";
const std::string AND_KEY = "&key=";
// 属性启用
const std::string SETTING_ENABLE_VALUE = "1";
// 属性禁用
const std::string SETTING_DISENABLE_VALUE = "0";
// 密码生效范围
const std::string PASSWORD_SCOPE = "password_scope";
// 离腕
const std::string HW_LEAVE_WRIST = "hw_leave_wrist";
// 是否设置密码
const std::string IS_PIN_ENROLLED = "persist.useriam.isPinEnrolled";
// 未设置密码
const std::string STATE_FALSE = "false";
// 已设置密码
const std::string STATE_TRUE = "true";
// 获取付款应用列表故障码
const int32_t GET_PAYMENT_ERROR_CODE = 1;
// 拉起IAM页面故障码
const int32_t START_IAM_ERROR_CODE = 2;

class WatchAppLockManager : public RefBase {
public:
    /**
     * 获取WatchAppLockManager的单实例
     *
     * @return 返回WatchAppLockManager的单例指针
     */
    static WatchAppLockManager &GetInstance()
    {
        static WatchAppLockManager instance;
        return instance;
    }

    /**
     * 析构函数，释放资源
     */
    ~WatchAppLockManager() override;

    /**
     * 检查是否属于安全模式
     *
     * @return 如果处于安全模式返回true,否则返回false
     */
    bool isSecureMode();

    /**
     * 检查屏幕是否被锁定
     *
     * @param isOHScreenLocked 屏幕是否被锁定
     * @return 返回屏幕的锁定状态，如果屏幕被锁定返回true,否则返回false
     */
    bool IsScreenLocked(bool isOHScreenLocked);

    /**
     * 解锁屏幕
     *
     * @param isScreenLocked 屏幕是否被锁定
     * @return 返回解锁操作的结果，成功返回0，失败返回错误码
     */
    int32_t unlockScreen(bool isScreenLocked);

    /**
     * 是否为支付应用
     *
     * @return 如果是支付应用返回true,否则返回false
     */
    bool IsPaymentApp();

    /**
     * 佩戴状态变更
     *
     * @param isWearOn 是否佩戴
     */
    void WearStateChange(bool isWearOn);

    /**
     * 灭屏
     */
    void OnScreenOffEnd();

public:
    class AppStateObserver : public AppExecFwk::ApplicationStateObserverStub {
    public:
        AppStateObserver() = default;
        virtual ~AppStateObserver() = default;
        void OnProcessStateChanged(const AppExecFwk::ProcessData &processData) override;
        void OnProcessCreated(const AppExecFwk::ProcessData &processData) override;
        void OnProcessDied(const AppExecFwk::ProcessData &processData) override;
        void OnWindowShow(const AppExecFwk::ProcessData &processData) override;
        void OnWindowHidden(const AppExecFwk::ProcessData &processData) override;
    };

public:
    struct AuthCallback : public UserIam::UserAuth::AuthenticationCallback {
    public:
        AuthCallback() = default;
        virtual ~AuthCallback() = default;
        void OnAcquireInfo(int32_t module, uint32_t acquireInfo, const UserIam::UserAuth::Attributes &extraInfo)
        {}
        void OnResult(int32_t result, const UserIam::UserAuth::Attributes &extraInfo)
        {
            std::unique_lock<ffrt::mutex> lock(authCallbackMutex);
            authResult = result;
            dataReady = true;
            conditionVar.notify_one();
        }
        int32_t WaitAuthFinish()
        {
            std::unique_lock<ffrt::mutex> lock(authCallbackMutex);
            conditionVar.wait(lock, [this] { return dataReady; });
            return authResult;
        }
    private:
        int32_t authResult = 0;
        bool dataReady = false;
        ffrt::condition_variable conditionVar;
        ffrt::mutex authCallbackMutex;
    };

public:
    class LeaveWristSettingObserver : public AAFwk::DataAbilityObserverStub {
    public:
        /**
         * setting回调类构造函数
         */
        LeaveWristSettingObserver() = default;

        /**
         * setting回调类析构函数
         */
        ~LeaveWristSettingObserver() = default;

        /**
         * 监听数据变化回调方法
         */
        void OnChange() override;
    };

public:
   class UnlockedRecord {
   public:
       /**
        * 向记录中添加一个元素
        *
        * @param element 要添加的元素
        * @return 如果添加成功返回true,如果元素已存在返回false
        */
        bool add(const std::string &element);

       /**
        * 从记录中移除一个元素
        *
        * @param element 要移除的元素
        * @return 如果移除成功返回true,如果元素不存在返回false
        */
        bool remove(const std::string &element);

       /**
        * 检查记录中是否包含指定元素
        *
        * @param 要检查的元素
        * @return 如果包含该元素返回true,否则返回false
        */
        bool contains(const std::string &element) const;

       /**
        * 清空记录中的所有元素
        */
        void clear();

    private:
        std::set<std::string> elements;
        mutable std::shared_mutex mtx;
    };

private:
    std::string GetBundleNameByUid(uint32_t uid);
    std::vector<uint8_t> GenerateRandom(int32_t len);
    bool GetPaymentServices(std::vector<AppExecFwk::AbilityInfo> &paymentAbilityInfos);
    std::string GetSettingsValue(
        const std::string &addressUrl, const std::string &key, const std::string &defaultValue);
    bool IsDeviceScope();
    bool IsLeaveWrist();
    bool BeginWidgetAuth();
    void registerAppStateObserver();
    void unregisterAppStateObserver();
    void registerLeaveWristSettingObserver();
    void unregisterLeaveWristSettingObserver();
    bool HasPin();
    int32_t GetUserIdFromCallingUid();
    std::string boolToString(bool value);
    WatchAppLockManager();
    bool IsFinishUnlock();

    bool isWearOn_ = false;
    bool isWristUnlock_ = false;
    sptr<AppStateObserver> appStateObserver_;
    sptr<LeaveWristSettingObserver> leaveWristSettingObserver_;
    UnlockedRecord unlockedRecord;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SCREENLOCK_WATCH_APPLOCK_MANAGER_H