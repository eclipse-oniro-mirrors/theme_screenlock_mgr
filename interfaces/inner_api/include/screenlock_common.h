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

#ifndef SERVICES_INCLUDE_SCREENLOCK_COMMON_H
#define SERVICES_INCLUDE_SCREENLOCK_COMMON_H

#include <string>

#include "errors.h"

namespace OHOS {
namespace ScreenLock {
#define SCREENLOCK_SERVICE_NAME "ScreenLockService"
#define PARAM_ZERO 0
#define PARAM_ONE 1
#define PARAM_TWO 2

const std::string BEGIN_WAKEUP = "beginWakeUp";
const std::string END_WAKEUP = "endWakeUp";
const std::string BEGIN_SCREEN_ON = "beginScreenOn";
const std::string END_SCREEN_ON = "endScreenOn";
const std::string BEGIN_SLEEP = "beginSleep";
const std::string END_SLEEP = "endSleep";
const std::string BEGIN_SCREEN_OFF = "beginScreenOff";
const std::string END_SCREEN_OFF = "endScreenOff";
const std::string STRONG_AUTH_CHANGED = "strongAuthChanged";
const std::string CHANGE_USER = "changeUser";
const std::string SCREENLOCK_ENABLED = "screenlockEnabled";
const std::string EXIT_ANIMATION = "beginExitAnimation";
const std::string UNLOCKSCREEN = "unlockScreen";
const std::string UNLOCK_SCREEN_RESULT = "unlockScreenResult";
const std::string LOCKSCREEN = "lockScreen";
const std::string LOCK_SCREEN_RESULT = "lockScreenResult";
const std::string SCREEN_DRAWDONE = "screenDrawDone";
const std::string SYSTEM_READY = "systemReady";
const std::string SERVICE_RESTART = "serviceRestart";
const int USER_NULL = -10000;
enum ScreenLockModule {
    SCREENLOCK_MODULE_SERVICE_ID = 0x04,
};
// time error offset, used only in this file.
constexpr ErrCode SCREENLOCK_ERR_OFFSET = ErrCodeOffset(SUBSYS_SMALLSERVICES, SCREENLOCK_MODULE_SERVICE_ID);

enum ScreenLockError {
    E_SCREENLOCK_OK = SCREENLOCK_ERR_OFFSET,
    E_SCREENLOCK_SA_DIED,
    E_SCREENLOCK_READ_PARCEL_ERROR,
    E_SCREENLOCK_WRITE_PARCEL_ERROR,
    E_SCREENLOCK_PUBLISH_FAIL,
    E_SCREENLOCK_TRANSACT_ERROR,
    E_SCREENLOCK_DEAL_FAILED,
    E_SCREENLOCK_PARAMETERS_INVALID,
    E_SCREENLOCK_SET_RTC_FAILED,
    E_SCREENLOCK_NOT_FOUND,
    E_SCREENLOCK_NO_PERMISSION,
    E_SCREENLOCK_NULLPTR,
    E_SCREENLOCK_SENDREQUEST_FAILED,
    E_SCREENLOCK_NOT_SYSTEM_APP,
    E_SCREENLOCK_NOT_FOCUS_APP,
    E_SCREENLOCK_USER_ID_INVALID,
};

enum TraceTaskId : int32_t {
    HITRACE_UNLOCKSCREEN,
    HITRACE_LOCKSCREEN,
    HITRACE_BUTT,
};

enum ScreenChange {
    SCREEN_SUCC = 0,
    SCREEN_FAIL,
    SCREEN_CANCEL,
};

enum JsErrorCode : uint32_t {
    ERR_NO_PERMISSION = 201,
    ERR_NOT_SYSTEM_APP = 202,
    ERR_INVALID_PARAMS = 401,
    ERR_CANCEL_UNLOCK = 13200001,
    ERR_SERVICE_ABNORMAL = 13200002,
    ERR_ILLEGAL_USE = 13200003,
    ERR_USER_ID_INVALID = 13200004,
};

enum class Action : uint8_t {
    LOCK = 0,
    UNLOCK,
    UNLOCKSCREEN,
};

enum class StrongAuthReasonFlags : int32_t {
    NONE = 0x00000000,
    AFTER_BOOT = 0x00000001,
    AFTER_TIMEOUT = 0x00000002,
    ACTIVE_REQUEST = 0x00000004,
    DPM_RESTRICT = 0x00000008,
};

enum class SpecialUserId : int32_t {
    USER_ALL = -1,
    USER_CURRENT = -2,
    USER_UNDEFINED = -10000,
};

constexpr int BEGIN_SLEEP_DEVICE_ADMIN_REASON = 1;
constexpr int BEGIN_SLEEP_USER_REASON = 2;
constexpr int BEGIN_SLEEP_LONG_TIME_UNOPERATOR = 3;

constexpr int END_SLEEP_DEVICE_ADMIN_REASON = 1;
constexpr int END_SLEEP_USER_REASON = 2;
constexpr int END_SLEEP_LONG_TIME_UNOPERATE = 3;

constexpr int SCREENLOCK_APP_CAN_USE = 1;
constexpr int SCREENLOCK_APP_CAN_NOT_USE = 0;

constexpr std::int32_t DEFAULT_ERROR = -1;
constexpr int32_t NONE_EVENT_TYPE = 0;
constexpr int ARGV_ZERO = 0;
constexpr int ARGV_ONE = 1;
constexpr int ARGV_TWO = 2;
constexpr int ARGV_THREE = 3;
constexpr int ARGV_NORMAL = -100;
constexpr std::int32_t MAX_VALUE_LEN = 4096;
constexpr const std::int32_t STR_MAX_SIZE = 256;
constexpr int RESULT_COUNT = 2;
constexpr int PARAMTWO = 2;
constexpr std::int32_t ARGS_SIZE_ZERO = 0;
constexpr std::int32_t ARGS_SIZE_ONE = 1;
constexpr std::int32_t ARGS_SIZE_TWO = 2;
constexpr std::int32_t ARGS_SIZE_THREE = 3;
constexpr std::int32_t ARGS_SIZE_FOUR = 4;
constexpr std::int32_t RESULT_ZERO = 0;
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCREENLOCK_COMMON_H