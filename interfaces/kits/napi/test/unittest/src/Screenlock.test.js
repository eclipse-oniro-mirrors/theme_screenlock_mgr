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
const PERMISSION_ERROR = 201;
const PARAMETER_ERROR = 401;
describe("ScreenlcokJsTest", function () {
    beforeAll(function () {
        // input testsuit setup step，setup invoked before all testcases
        console.info('beforeAll caled')
    })

    afterAll(function () {
        // input testsuit teardown step，teardown invoked after all testcases
        console.info('afterAll caled')
    })

    beforeEach(function () {
        // input testcase setup step，setup invoked before each testcases
        console.info('beforeEach caled')
    })

    afterEach(function () {
        // input testcase teardown step，teardown invoked after each testcases
        console.info('afterEach caled')
    })

    /*
     * @tc.name:SUB_MISC_THEME_screenLock_API_0001
     * @tc.desc: Checks whether the screen lock of the current device is secure.
     * @tc.type: Function
     * @tc.require: SR000HHEJQ
     */
    it("SUB_MISC_THEME_screenLock_API_0001", 0, function (done) {
        try {
            let ret = screenLock.isLocked();
            screenLock.isScreenLocked((err, data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0001 screen's status is " + data);
                expect(data == ret).assertTrue();
                done();
            });
        } catch (error) {
            console.error("logMessage SUB_MISC_THEME_screenLock_API_0001: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0002
    * @tc.desc: Checks whether the screen is currently locked. 
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0002", 0, function (done) {
        try {
            screenLock.isSecureMode((err, data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0002 secureMode's result is " + data);
                expect(data === false).assertTrue();
                done();
            });
        } catch (error) {
            console.error("logMessage SUB_MISC_THEME_screenLock_API_0002: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0003
    * @tc.desc: Unlock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0003", 0, function (done) {
        try {
            screenLock.unlock((err, data) => {
                if (err) {
                    console.info("unlock filed: error.code : " + err.code + "error.message :" + err.message);
                    expect(err.code === PERMISSION_ERROR).assertTrue();
                } else {
                    console.info("SUB_MISC_THEME_screenLock_API_0003 unlock screen success.");
                    let ret = screenLock.isLocked();
                    expect(ret).assertFalse();
                    expect(data).assertTrue();
                }
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0003: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0004
    * @tc.desc: Unlock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0004", 0, function (done) {
        try {
            screenLock.unlock().then((data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0004 unlock screen success.");
                let ret = screenLock.isLocked();
                expect(ret).assertFalse();
                expect(data).assertTrue();
                done();
            }).catch((err) => {
                console.error("SUB_MISC_THEME_screenLock_API_0004: error.code : " + err.code + "error.message :" + err.message);
                expect(err.code === PERMISSION_ERROR).assertTrue();
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0004: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0005
    * @tc.desc: Lock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0005", 0, function (done) {
        try {
            screenLock.lock((err, data) => {
                if (err) {
                    console.info("lock filed: error.code : " + err.code + "error.message :" + err.message);
                    expect(err.code == PERMISSION_ERROR).assertTrue();
                } else {
                    console.info("SUB_MISC_THEME_screenLock_API_0005 lock screen success.");
                    let ret = screenLock.isLocked();
                    expect(ret).assertTrue();
                    expect(data).assertTrue();
                }
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0005: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0006
    * @tc.desc: Lock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0006", 0, function (done) {
        try {
            screenLock.lock().then((data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0006 lock screen success.");
                let ret = screenLock.isLocked();
                expect(ret).assertTrue();
                expect(data).assertTrue();
                done();
            }).catch((err) => {
                console.error("SUB_MISC_THEME_screenLock_API_0006: error.code : " + err.code + "error.message :" + err.message);
                expect(err.code == PERMISSION_ERROR).assertTrue();
                done();
            });

        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0006: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0007
    * @tc.desc: Register system event to screenlockSA.
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0007", 0, function (done) {
        try {
            let ret = screenLock.onSystemEvent((data) => {});
            expect(ret).assertTrue();
            done();
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0007: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PERMISSION_ERROR).assertTrue();
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0008
    * @tc.desc: screenlockAPP send event to screenlockSA
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0008", 0, function (done) {
        try {
            screenLock.sendScreenLockEvent('testparam', 1, (err, data) => {
                if (err) {
                    console.info("SUB_MISC_THEME_screenLock_API_0008 : error.code : " + err.code + "error.message :" + err.message);
                    expect(err.code == PERMISSION_ERROR).assertTrue();
                } else {
                    console.info("SUB_MISC_THEME_screenLock_API_0008: sendScreenLockEvent success ");
                }
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0007: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PARAMETER_ERROR).assertTrue();
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0009
    * @tc.desc: screenlockAPP send event to screenlockSA
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0009", 0, function (done) {
        screenLock.sendScreenLockEvent('unlockScreenResult', 0).then((data) => {
            console.info("SUB_MISC_THEME_screenLock_API_0009: sendScreenLockEvent success ");
            done();
        }).catch((err) => {
            console.error("SUB_MISC_THEME_screenLock_API_0009: error.code : " + err.code + "error.message :" + err.message);
            expect(err.code == PERMISSION_ERROR).assertTrue();
            done();
        });
    })
    /*
     * @tc.name:SUB_MISC_THEME_screenLock_API_0010
     * @tc.desc: Checks whether the screen lock of the current device is secure.
     * @tc.type: Function
     * @tc.require: SR000HHEJQ
     */
    it("SUB_MISC_THEME_screenLock_API_0010", 0, function (done) {
        try {
            let ret = screenLock.isLocked();
            screenLock.isScreenLocked().then((result) => {
                expect(ret === result).assertTrue();
                done();
            }).catch((error) => {
                console.error("SUB_MISC_THEME_screenLock_API_0010: error.code : " + error.code + "error.message :" + error.message);
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0010: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
     * @tc.name:SUB_MISC_THEME_screenLock_API_0011
     * @tc.desc: Checks whether the screen is currently locked.
     * @tc.type: Function
     * @tc.require: SR000HHEJQ
     */
    it("SUB_MISC_THEME_screenLock_API_0011", 0, function (done) {
        try {
            screenLock.isSecureMode().then((result) => {
                expect(result === false).assertTrue();
                done();
            }).catch((error) => {
                console.error("SUB_MISC_THEME_screenLock_API_0011: error.code : " + error.code + "error.message :" + error.message);
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0011: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0012
    * @tc.desc: Unlock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0012", 0, function (done) {
        try {
            screenLock.unlockScreen((err, data) => {
                if (err) {
                    console.info("SUB_MISC_THEME_screenLock_API_0012: error.code : " + err.code + "error.message :" + err.message);
                    expect(err.code == PERMISSION_ERROR).assertTrue();
                } else {
                    console.info("SUB_MISC_THEME_screenLock_API_0012: unlock screen success.");
                    let ret = screenLock.isLocked();
                    expect(ret).assertFalse();
                }
                expect(data == undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0012: error.code : " + error.code + "error.message :" + error.message);
            done();
        }
    })
    /*
    * @tc.name:SUB_MISC_THEME_screenLock_API_0013
    * @tc.desc: Unlock the screen
    * @tc.type: Function
    * @tc.require: SR000HHEJQ
    */
    it("SUB_MISC_THEME_screenLock_API_0013", 0, function (done) {
        try {
            screenLock.unlockScreen().then((data) => {
                console.info("SUB_MISC_THEME_screenLock_API_0013: send unlock issue success retCode.");
                let ret = screenLock.isLocked();
                expect(ret).assertFalse();
                expect(data == undefined).assertTrue();
                done();
            }).catch((err) => {
                console.error("SUB_MISC_THEME_screenLock_API_0013: error.code : " + err.code + "error.message :" + err.message);
                expect(err.code == PERMISSION_ERROR).assertTrue();
                done();
            });
        } catch (error) {
            console.error("SUB_MISC_THEME_screenLock_API_0013: error.code : " + error.code + "error.message :" + error.message);
            expect(error.code == PARAMETER_ERROR).assertTrue();
            done();
        }
    })

})