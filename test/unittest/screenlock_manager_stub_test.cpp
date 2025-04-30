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

#include <cstdint>
#include <list>
#include <string>
#include <sys/time.h>

#include "accesstoken_kit.h"
#include "sclock_log.h"
#include "screenlock_callback_test.h"
#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_notify_test_instance.h"
#include "screenlock_manager_stub_test.h"
#include "screenlock_system_ability.h"
#include "screenlock_system_ability_stub.h"
#include "securec.h"
#include "token_setproc.h"
#include "message_option.h"
#include "message_parcel.h"
#include "iremote_stub.h"

#include "ipc_skeleton.h"
#include "parcel.h"
#include "screenlock_callback_interface.h"
#include "screenlock_server_ipc_interface_code.h"

namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;
const std::u16string SLMGRSTUB_INTERFACE_TOKEN = u"ohos.screenlock.ScreenLockManagerInterface";
const std::u16string SLMGRSTUB_INTERFACE_BAD_TOKEN = u"ohos.screenlock.ScreenLockManagerInterfaceA";
const int ERR_INVALID_DATA = 305;
void ScreenLockManagerStubWrapTest::SetUpTestCase()
{}

void ScreenLockManagerStubWrapTest::TearDownTestCase()
{}

void ScreenLockManagerStubWrapTest::SetUp()
{}

void ScreenLockManagerStubWrapTest::TearDown()
{}

/**
 * @tc.name: ScreenLockManagerStubTest
 * @tc.desc: OnRemoteRequest.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrap001, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrap001");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_BAD_TOKEN);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrap001, result = %{public}d", result);
    EXPECT_EQ(result, ERR_INVALID_DATA);
}

/**
 * @tc.name: ScreenLockTest003
 * @tc.desc: beginSleep event.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrap002, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrap002");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrap002, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrap002_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrap002_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest(true);
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrap002_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: ScreenLockTest004
 * @tc.desc: beginScreenOn event.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrap003, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrap003");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREEN_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrap003, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: ScreenLockTest005
 * @tc.desc: beginScreenOff event.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest004");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SECURE_MODE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest004, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: ScreenLockTest005
 * @tc.desc: beginScreenOff event.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest005, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest005");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(nullptr);
    int result =
        instance->OnRemoteRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest005, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest006, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest006");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    EventListenerTest eventListener;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    sptr<ScreenlockCallbackTest> callback = new ScreenlockCallbackTest(eventListener);
    data.WriteRemoteObject(callback->AsObject());
    int result =
        instance->OnRemoteRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK), data, reply, option);
    SCLOCK_HILOGE("ScreenLockManagerStubWrapTest006, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest007, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest007");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(instance->AsObject());
    int result =
        instance->OnRemoteRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest007, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest008, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest008");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(nullptr);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK_SCREEN), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest008, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest009, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest009");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    EventListenerTest eventListener;
    sptr<ScreenlockCallbackTest> callback = new ScreenlockCallbackTest(eventListener);
    data.WriteRemoteObject(callback->AsObject());
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK_SCREEN), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest009, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest010, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest010");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(instance->AsObject());
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK_SCREEN), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest010, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest011, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest011");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(nullptr);
    int result =
        instance->OnRemoteRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest011, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest012, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest012");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    EventListenerTest eventListener;
    sptr<ScreenlockCallbackTest> callback = new ScreenlockCallbackTest(eventListener);
    data.WriteRemoteObject(callback->AsObject());
    int result =
        instance->OnRemoteRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest012, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest013, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest013");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(instance->AsObject());
    int result =
        instance->OnRemoteRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest013, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest014, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest014");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteString("{invalid json}");
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SEND_SCREENLOCK_EVENT), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest014, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest015, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest015");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(nullptr);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::ONSYSTEMEVENT), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest015, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest0151, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest0151");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteRemoteObject(instance->AsObject());
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::ONSYSTEMEVENT), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest0151, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest016, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest016");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK_SCREEN), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest016, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest017, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest017");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREENLOCK_DISABLED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest017, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest017_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest017_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest(true);
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREENLOCK_DISABLED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest017_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest018, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest018");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteBool(true);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SET_SCREENLOCK_DISABLED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest018, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest019, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest019");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteString("{invalid json}");
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SET_SCREENLOCK_AUTHSTATE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest019, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest020, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest020");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_SCREENLOCK_AUTHSTATE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest020, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest021, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest021");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REQUEST_STRONG_AUTHSTATE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest021, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest021_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest021_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest(true);
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REQUEST_STRONG_AUTHSTATE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest021_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest022, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest022");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_STRONG_AUTHSTATE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest022, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest022_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest022_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest(true);
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_STRONG_AUTHSTATE), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest022_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest023, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest023");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_DEVICE_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest023, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest023_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest023_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest(true);
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_DEVICE_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest023_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest024, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest024");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteRemoteObject(nullptr);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REGISTER_INNER_LISTENER), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest024, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest024_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest024_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteRemoteObject(instance->AsObject());
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REGISTER_INNER_LISTENER), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest024_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest025, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest025");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteRemoteObject(nullptr);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNREGISTER_INNER_LISTENER), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest025, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest025_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest025_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteRemoteObject(instance->AsObject());
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNREGISTER_INNER_LISTENER), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest025_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest026, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest026");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_USER_SCREEN_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest026, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest026_1, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest026_1");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest(true);
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_USER_SCREEN_LOCKED), data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest026_1, result = %{public}d  %{public}d", result, ERR_NONE);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(ScreenLockManagerStubWrapTest, ScreenLockManagerStubWrapTest027, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockManagerStubWrapTest027");
    sptr<ScreenLockManagerStubTest> instance = new ScreenLockManagerStubTest();
    if (instance == nullptr) {
        SCLOCK_HILOGE("instance is nullptr!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SLMGRSTUB_INTERFACE_TOKEN);
    data.WriteInt32(0);
    int result = instance->OnRemoteRequest(100, data, reply, option);
    SCLOCK_HILOGI("ScreenLockManagerStubWrapTest027, result = %{public}d  %{public}d", result, ERR_INVALID_DATA);
    EXPECT_EQ(result, ERR_INVALID_DATA);
}

}  // namespace ScreenLock
}  // namespace OHOS