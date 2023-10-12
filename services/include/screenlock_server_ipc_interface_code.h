/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SCREENLOCK_SERVICE_IPC_INTERFACE_CODE_H
#define SCREENLOCK_SERVICE_IPC_INTERFACE_CODE_H

/* SAID: 3704*/
namespace OHOS {
namespace ScreenLock {
enum class ScreenLockServerIpcInterfaceCode {
    // before api version 9
    IS_SCREEN_LOCKED = 0,
    IS_SECURE_MODE,
    UNLOCK_SCREEN,

    // since 9, with exception or system app verification
    ONSYSTEMEVENT,
    LOCK,
    SEND_SCREENLOCK_EVENT,
    IS_LOCKED,
    UNLOCK,
    LOCK_SCREEN,
};
} // namespace ScreenLock
} // namespace OHOS

#endif // SCREENLOCK_SERVICE_IPC_INTERFACE_CODE_H