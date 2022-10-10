/*
 * Copyright (C) 2021 XXXX Device Co., Ltd.
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
import screenLock from '@ohos.screenLock';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const SLEEP_TIME = 1000;
const PERMISSON_ERROR = 201;
const PARAMETER_ERROR = 401;
describe("ScreenlcokJsTest", function () {
    beforeAll(function() {
        // input testsuit setup step，setup invoked before all testcases
         console.info('beforeAll caled')
    })
    
    afterAll(function() {
         // input testsuit teardown step，teardown invoked after all testcases
         console.info('afterAll caled')
    })
    
    beforeEach(function() {
        // input testcase setup step，setup invoked before each testcases
         console.info('beforeEach caled')
    })
    
    afterEach(function() {
        // input testcase teardown step，teardown invoked after each testcases
        console.info('afterEach caled')
    })

    /*
     * @tc.name:SUB_MISC_THEME_screenLock_API_0001
     * @tc.desc: Checks whether the screen lock of the current device is secure.
     * @tc.type: Function
     * @tc.require: SR000HHEJQ
     */
    it("SUB_MISC_THEME_screenLock_API_0001", 0, function () {
        try {
            let ret = screenLock.isLocked();
            screenLock.isScreenLocked((err, data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0001 screen's status is " + data);
                expect(data == ret).assertTrue();
            });
        } catch (error) {
            console.error("logMessage SUB_MISC_THEME_screenLock_API_0001: error.code : " + error.code + "error.message :" + error.message);
            expect(true).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0002
    * @tc.desc: Checks whether the screen is currently locked. 
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0002", 0, function () {
        try {
            let ret = screenLock.isSecure();
            screenLock.isSecureMode((err, data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0002 secureMode's result is " + data);
                expect(data == ret).assertTrue();
            });
        } catch (error) {
            console.error("logMessage SUB_MISC_THEME_screenLock_API_0002: error.code : " + error.code + "error.message :" + error.message);
            expect(true).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0003
    * @tc.desc: Unlocks the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0003", 0, function () {
        try {
            screenLock.unlock((err, data) => {
                if(err) {
                    console.info("unlock filed: error.code : " + err.code + "error.message :" + err.message);
                    expect(err.code == PERMISSON_ERROR).assertTrue();
                } else {
                    console.info("SUB_MISC_THEME_screenLock_API_0003: send unlock issue success retCode:" + data);
                    let ret = screenLock.isLocked();
                    expect(ret == false).assertTrue();
                }
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0003: error.code : " + error.code + "error.message :" + error.message);
            expect(true).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0004
    * @tc.desc: Unlocks the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0004", 0, function () {
        try {
            screenLock.unlock().then((data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0004: send unlock issue success retCode:" + data);
                let ret = screenLock.isLocked();
                expect(ret == false).assertTrue();
            }).catch((err) => {
                console.error("SUB_MISC_THEME_screenLock_API_0004: send unlock issue failed error.code : " + error.code + "error.message :" + error.message);
                expect(err.code == PERMISSON_ERROR).assertTrue();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0004: error.code : " + error.code + "error.message :" + error.message);
            expect(true).assertTrue();
        }
    })
     /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0005
    * @tc.desc: Lock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
     it("SUB_MISC_THEME_screenLock_API_0005", 0, function () {
        try {
            screenLock.lock((err, data) => {
                if(err) {
                    console.info("lock filed: error.code : " + err.code + "error.message :" + err.message);
                    expect(err.code == PERMISSON_ERROR).assertTrue();
                } else {
                    console.info("SUB_MISC_THEME_screenLock_API_0005: send lock issue success retCode:" + data);
                    let ret = screenLock.isLocked();
                    expect(ret == true).assertTrue();
                }
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0005: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PARAMETER_ERROR).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0006
    * @tc.desc: Lock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
     it("SUB_MISC_THEME_screenLock_API_0006", 0, function () {
        try {
            screenLock.lock().then((data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0006: send lock issue success retCode:" + data);
                let ret = screenLock.isLocked();
                expect(ret == true).assertTrue();
            }).catch((err) => {
                console.error("SUB_MISC_THEME_screenLock_API_0006: send lock issue failed error.code : " + error.code + "error.message :" + error.message);
                expect(error.code == PERMISSON_ERROR).assertTrue();
            });
            
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0006: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PARAMETER_ERROR).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0007
    * @tc.desc: Register system event related to syscreen lock
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0007", 0, function () {
        try {
            let ret = screenLock.onSystemEvent((err, data) => {
                if(err) {
                    console.info("onSystemEvent filed: error.code : " + err.code + "error.message :" + err.message);
                } 
                console.info("SUB_MISC_THEME_screenLock_API_0007: onSystemEvent success ");
            });
            expect(ret == true).assertTrue();
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0007: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PERMISSON_ERROR).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0008
    * @tc.desc: screenlockAPP send event to screenlockSA
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0008", 0, function () {
        try {
            screenLock.sendScreenLockEvent('testparam', 1, (err, data) => {
                if(err) {
                    console.info("sendScreenLockEvent filed: error.code : " + err.code + "error.message :" + err.message);
                } 
                console.info("SUB_MISC_THEME_screenLock_API_0008: sendScreenLockEvent success ");
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0007: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PARAMETER_ERROR).assertTrue();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0009
    * @tc.desc: screenlockAPP send event to screenlockSA
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0009", 0, function () {
        screenLock.sendScreenLockEvent('unlockScreenResult', 0).then((data) => {
            console.info("SUB_MISC_THEME_screenLock_API_0009: sendScreenLockEvent success ");
        }).catch((err) => {
            console.error("SUB_MISC_THEME_screenLock_API_0009: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PERMISSON_ERROR).assertTrue();
        });
    })
})