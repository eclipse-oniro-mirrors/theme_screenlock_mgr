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

#include <string>
#include "wear_detection_observer.h"
#include "sclock_log.h"
#include "watch_applock_manager.h"
#include "display_manager.h"

using namespace OHOS::Rosen;
namespace OHOS {
namespace ScreenLock {
void WearDetectionObserver ::RegisterSensorListener()
{
    if (!registered_.exchange(true)) {
        (void)strcpy_s(sensorUser_.name, sizeof(sensorUser_.name), SENSOR_NAME);
        sensorUser_.userData = nullptr;
        sensorUser_.callback = &WearSensorCallback;
        RetryRegistration();
    } else {
        SCLOCK_HILOGI("already registered");
    }
}

void WearDetectionObserver ::RetryRegistration()
{
    const int maxRetries = MAX_RETRIES;
    const std::chrono::seconds retryDelay(RETRY_DELAY_SECOND);
    retryCount_ = 0;
    auto retryTask = [this, maxRetries, retryDelay]() {
        while (retryCount_ < maxRetries) {
            int ret = SubscribeSensor(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_);
            SCLOCK_HILOGI("registered ret = %{public}d", ret);
            if (ret != 0) {
                retryCount_++;
                SCLOCK_HILOGI("registered fail, retry = %{public}d", retryCount_);
                std::this_thread::sleep_for(retryDelay);
                continue;
            }
            ret =
                SetBatch(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_, SAMPLING_INTERVAL_100MS, SAMPLING_INTERVAL_100MS);
            if (ret != 0) {
                retryCount_++;
                SCLOCK_HILOGI("SetBatch fail, retry = %{public}d", retryCount_);
                std::this_thread::sleep_for(retryDelay);
                continue;
            }
            ret = ActivateSensor(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_);
            if (ret != 0) {
                retryCount_++;
                SCLOCK_HILOGI("ActivateSensor fail, retry = %{public}d", retryCount_);
                std::this_thread::sleep_for(retryDelay);
                continue;
            }
            SCLOCK_HILOGI("registered success");
            return;
        }
        registered_.exchange(false);
        SCLOCK_HILOGW("registered fail after %{public}d retries", maxRetries);
    };
    std::thread taskThread(retryTask);
    taskThread.detach();
}

void WearDetectionObserver ::UnRegisterSensorListener()
{
    if (registered_.exchange(false)) {
        DeactivateSensor(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_);
        UnsubscribeSensor(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_);
        SCLOCK_HILOGI("unregistered success");
    } else {
        SCLOCK_HILOGI("not registered");
    }
}

void WearDetectionObserver ::WearSensorCallback(SensorEvent *event)
{
    if (event == nullptr) {
        SCLOCK_HILOGI("Sensor event is nullptr");
        return;
    }
    auto id = event->sensorTypeId;
    if (id == SENSOR_TYPE_ID_WEAR_DETECTION) {
        if (event->data == nullptr) {
            SCLOCK_HILOGI("Sensor data is nullptr");
            return;
        }
        WearDetectionData *wearDetectionData = reinterpret_cast<WearDetectionData *>(event->data);
        bool isWearOn = wearDetectionData->value == WEAR_ON;
        WatchAppLockManager::GetInstance().WearStateChange(isWearOn);
    }
}

void WearDisplayPowerEventObserver ::WearDisplayPowerEventListener::OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    SCLOCK_HILOGI(
        "OnDisplayPowerEvent event=%{public}d,status= %{public}d", static_cast<int>(event), static_cast<int>(status));
    if (event == DisplayPowerEvent::SLEEP && status == EventStatus::END) {
        WatchAppLockManager::GetInstance().onScreenOffEnd();
    }
}

void WearDisplayPowerEventObserver ::RegisterDisplayPowerEventListener()
{
    if (!registered_.exchange(true)) {
        RetryRegistration();
    } else {
        SCLOCK_HILOGI("already registered");
    }
}

void WearDisplayPowerEventObserver ::RetryRegistration()
{
    const int maxRetries = MAX_RETRIES;
    const std::chrono::seconds retryDelay(RETRY_DELAY_SECOND);
    retryCount_ = 0;
    auto retryTask = [this, maxRetries, retryDelay]() {
        while (retryCount_ < maxRetries) {
            DMError ret = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(displayPowerEventListener_);
            SCLOCK_HILOGI("registered ret = %{public}d", ret);
            if (ret != 0) {
                retryCount_++;
                SCLOCK_HILOGI("registered fail, retry = %{public}d", retryCount_);
                std::this_thread::sleep_for(retryDelay);
                continue;
            }
            SCLOCK_HILOGI("registered success");
            return;
        }
        registered_.exchange(false);
        SCLOCK_HILOGW("registered fail after %{public}d retries", maxRetries);
    };
    std::thread taskThread(retryTask);
    taskThread.detach();
}

void WearDisplayPowerEventObserver ::UnRegisterDisplayPowerEventListener()
{
    if (registered_.exchange(false)) {
        DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(displayPowerEventListener_);
        SCLOCK_HILOGI("unregistered success");
    } else {
        SCLOCK_HILOGI("not registered");
    }
}
}
}