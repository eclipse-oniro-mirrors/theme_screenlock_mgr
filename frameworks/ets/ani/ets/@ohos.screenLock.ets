/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { AsyncCallback, BusinessError, Callback } from '@ohos.base';

export namespace screenLock {

  loadLibrary("screenlock_ani.z");

  enum AuthState {
    NOT_AUTHED = 0,
    PRE_AUTHED_BY_CREDENTIAL = 1,
    PRE_AUTHED_BY_FINGERPRINT = 2,
    PRE_AUTHED_BY_FACE = 3,
    AUTHED_BY_CREDENTIAL = 4,
    AUTHED_BY_FINGERPRINT = 5,
    AUTHED_BY_FACE = 6,
  }

  type EventType = 
  'beginWakeUp'
    | 'endWakeUp'
    | 'beginScreenOn'
    | 'endScreenOn'
    | 'beginScreenOff'
    | 'endScreenOff'
    | 'unlockScreen'
    | 'lockScreen'
    | 'beginExitAnimation'
    | 'beginSleep'
    | 'endSleep'
    | 'changeUser'
    | 'screenlockEnabled'
    | 'serviceRestart';

  enum StrongAuthReasonFlags {
    NONE = 0x00000000,
    AFTER_BOOT = 0x00000001,
    AFTER_TIMEOUT = 0x00000002,
    ACTIVE_REQUEST = 0x00000004,
    POLICY_RESTRICTION = 0x00000008
  }

  interface SystemEvent {
    eventType: EventType;
    params: string;
  }

  class SystemEventImpl implements SystemEvent {
    eventType: EventType;
    params: string;
  }

  native function isScreenLocked_inner(): boolean;

  native function isSecureMode_inner(): boolean;

  native function unlockScreen_inner(): void;

  native function unlock_inner(): boolean;

  native function lock_inner(): boolean;

  native function sendScreenLockEvent_inner(event: string, parameter: int): boolean;

  native function requestStrongAuth_inner(reasonFlag: StrongAuthReasonFlags, userId: int): void;

  native function setScreenLockDisabled_inner(disable: boolean, userId: int): boolean;

  native function setScreenLockAuthState_inner(state: AuthState, userId: int, authToken: Uint8Array): boolean;

  export native function isLocked(): boolean;

  export native function onSystemEvent(callback: Callback<SystemEvent>): boolean;

  export native function isScreenLockDisabled(userId: int): boolean;

  export native function getScreenLockAuthState(userId: int): AuthState;

  export native function getStrongAuth(userId: int): int;

  export native function isDeviceLocked(userId: int): boolean;

  export function isScreenLocked(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return isScreenLocked_inner();
    })
    .then((ret: Any): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: Any): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function isScreenLocked(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return isScreenLocked_inner();
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function isSecureMode(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return isSecureMode_inner();
    })
    .then((ret: Any): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: Any): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function isSecureMode(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return isSecureMode_inner();
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function unlockScreen(callback: AsyncCallback<void>): void {
    taskpool.execute((): void => {
      return unlockScreen_inner();
    })
    .then((ret: Any): void => {
      callback(null, ret as undefined);
    })
    .catch((ret: Any): void => {
      let data: undefined;
      callback(ret as BusinessError, data);
    });
  }

  export function unlockScreen(): Promise<void> {
    return new Promise<void>((resolve, reject): void => {
      taskpool.execute((): void => {
        return unlockScreen_inner();
      })
      .then((ret: Any): void => {
        resolve(ret as undefined);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function unlock(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return unlock_inner();
    })
    .then((ret: Any): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: Any): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function unlock(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return unlock_inner();
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function lock(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return lock_inner();
    })
    .then((ret: Any): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: Any): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function lock(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return lock_inner();
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function sendScreenLockEvent(event: string, parameter: int, callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return sendScreenLockEvent_inner(event, parameter);
    })
    .then((ret: Any): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: Any): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function sendScreenLockEvent(event: string, parameter: int): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return sendScreenLockEvent_inner(event, parameter);
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function requestStrongAuth(reasonFlag: StrongAuthReasonFlags, userId: int): Promise<void> {
    return new Promise<void>((resolve, reject): void => {
      taskpool.execute((): void => {
        return requestStrongAuth_inner(reasonFlag, userId);
      })
      .then((ret: Any): void => {
        resolve(ret as undefined);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function setScreenLockDisabled(disable: boolean, userId: int, callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return setScreenLockDisabled_inner(disable, userId);
    })
    .then((ret: Any): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: Any): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function setScreenLockDisabled(disable: boolean, userId: int): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return setScreenLockDisabled_inner(disable, userId);
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }

  export function setScreenLockAuthState(state: AuthState, userId: int, authToken: Uint8Array): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return setScreenLockAuthState_inner(state, userId, authToken);
      })
      .then((ret: Any): void => {
        resolve(ret as boolean);
      })
      .catch((ret: Any): void => {
        reject(ret as Error);
      });
    })
  }
}