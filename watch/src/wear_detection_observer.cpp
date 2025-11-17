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

namespace OHOS {
namespace ScreenLock {
void WearDetectionObserver ::RegisterSensorListener()
{
    if (!registered_.exchange(true)) {
        (void)strcpy_s(sensorUser_.name, sizeof(sensorUser_.name), SENSOR_NAME);
        sensorUser_.userData = nullptr;
        sensorUser_.callback = &WearSensorCallback;
        int32_t result = SubscribeSensor(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_);
        SCLOCK_HILOGI("registered result = %{public}d", result);
        if (result != 0) {
            SCLOCK_HILOGW("registered fail");
            registered_.exchange(false);
            return;
        }
        SetBatch(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_, SAMPLING_INTERVAL_100MS, SAMPLING_INTERVAL_100MS);
        ActivateSensor(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_);
        SetMode(SENSOR_TYPE_ID_WEAR_DETECTION, &sensorUser_, SENSOR_ON_CHANGE);
        SCLOCK_HILOGI("registered success");
    } else {
        SCLOCK_HILOGI("already registered");
    }
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
}
}