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
#ifndef WEAR_DETECTION_OBSERVER_H
#define WEAR_DETECTION_OBSERVER_H
#include "sensor_agent_type.h"
#include "sensor_agent.h"
#include "refbase.h"
#include <chrono>
#include <thread>
#include <iostream>

namespace OHOS {
namespace ScreenLock {
const uint32_t SAMPLING_INTERVAL_100MS = 100 * 1000 * 1000;
const int MAX_RETRIES = 5;
const int RETRY_DELAY_SECOND = 10;
const char SENSOR_NAME[] = "ScreenLockWatch";
/**
 * 佩戴状态，在腕
 */
const float WEAR_ON = 1;
class WearDetectionObserver : public RefBase {
public:
    void RegisterSensorListener();
    void UnRegisterSensorListener();

private:
    int retryCount_ = 0;
    SensorUser sensorUser_{};
    std::atomic<bool> registered_{false};
    static void WearSensorCallback(SensorEvent *event);
    void RetryRegistration();
};
} // namespace ScreenLock
} // namespace OHOS
#endif // WEAR_DETECTION_OBSERVER_H