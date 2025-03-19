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

import { BusinessError, AsyncCallback } from '@ohos.base';

namespace ScreenLock {
    export native function islocked():boolean;
    export native function unlockexecute():int;

    export function unlock(callback: AsyncCallback<void, void>): void {
        console.info("unlock callback call.");
        let task = taskpool.execute(ScreenLock.unlockexecute);
        task.then((e: NullishType)=>{
            let resultCode: Int  = e as Int;
            console.log("subscribe result:" + resultCode.intValue());
            let r: BusinessError = { code: resultCode.intValue(), data: undefined };
            callback(r, undefined);
        }, (err:Object): void => {
            console.log("subscribe error:" + err);
        });
    }

    export function unlock():Promise<boolean>{
        console.info("createSubscriber Promise call.");
        let p = new Promise<boolean>((resolve: (v: boolean) => void,
            reject: (error: Object) => void) => {
            let p1 = taskpool.execute(ScreenLock.unlockexecute);
            p1.then((e :NullishType)=>{
                console.log("createSubscriber Promise result");
                let result: boolean  = e as boolean;
                resolve(result);
            }, (err:Object): void => {
                console.log("createSubscriber Promise error:" + err);
            });
        });
        return p;
    }
}
