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
    UNAUTH = 0,
    PRE_AUTHED_BY_CREDENTIAL = 1,
    PRE_AUTHED_BY_BIOMIETRIC = 2,
    AUTHED_BY_CREDENTIAL = 3,
    AUTHED_BY_BIOMIETRIC = 4,
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

  native function sendScreenLockEvent_inner(event: string, parameter: number): boolean;

  native function setScreenLockDisabled_inner(disable: boolean, userId: number): boolean;

  native function setScreenLockAuthState_inner(state: AuthState, userId: number, authToken: Uint8Array): boolean;

  export native function isLocked(): boolean;

  export native function onSystemEvent(callback: Callback<SystemEvent>): boolean;

  export native function isScreenLockDisabled(userId: number): boolean;

  export native function getScreenLockAuthState(userId: number): AuthState;

  export native function getStrongAuth(userId: number): number;

  export function isScreenLocked(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return isScreenLocked_inner();
    })
    .then((ret: NullishType): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: NullishType): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function isScreenLocked(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return isScreenLocked_inner();
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function isSecureMode(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return isSecureMode_inner();
    })
    .then((ret: NullishType): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: NullishType): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function isSecureMode(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return isSecureMode_inner();
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function unlockScreen(callback: AsyncCallback<void>): void {
    taskpool.execute((): void => {
      return unlockScreen_inner();
    })
    .then((ret: NullishType): void => {
      callback(null, ret as undefined);
    })
    .catch((ret: NullishType): void => {
      let data: undefined;
      callback(ret as BusinessError, data);
    });
  }

  export function unlockScreen(): Promise<void> {
    return new Promise<void>((resolve, reject): void => {
      taskpool.execute((): void => {
        return unlockScreen_inner();
      })
      .then((ret: NullishType): void => {
        resolve(ret as undefined);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function unlock(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return unlock_inner();
    })
    .then((ret: NullishType): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: NullishType): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function unlock(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return unlock_inner();
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function lock(callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return lock_inner();
    })
    .then((ret: NullishType): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: NullishType): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function lock(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return lock_inner();
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function sendScreenLockEvent(event: string, parameter: number, callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return sendScreenLockEvent_inner(event, parameter);
    })
    .then((ret: NullishType): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: NullishType): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function sendScreenLockEvent(event: string, parameter: number): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return sendScreenLockEvent_inner(event, parameter);
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function setScreenLockDisabled(disable: boolean, userId: number, callback: AsyncCallback<boolean>): void {
    taskpool.execute((): boolean => {
      return setScreenLockDisabled_inner(disable, userId);
    })
    .then((ret: NullishType): void => {
      callback(null, ret as boolean);
    })
    .catch((ret: NullishType): void => {
      let data: boolean;
      callback(ret as BusinessError, data);
    });
  }

  export function setScreenLockDisabled(disable: boolean, userId: number): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return setScreenLockDisabled_inner(disable, userId);
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }

  export function setScreenLockAuthState(state: AuthState, userId: number, authToken: Uint8Array): Promise<boolean> {
    return new Promise<boolean>((resolve, reject): void => {
      taskpool.execute((): boolean => {
        return setScreenLockAuthState_inner(state, userId, authToken);
      })
      .then((ret: NullishType): void => {
        resolve(ret as boolean);
      })
      .catch((ret: NullishType): void => {
        reject(ret as Error);
      });
    })
  }
}