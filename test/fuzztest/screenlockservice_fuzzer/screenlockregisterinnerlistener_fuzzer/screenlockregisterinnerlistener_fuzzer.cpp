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

#include "screenlockregisterinnerlistener_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "accesstoken_kit.h"
#include "fuzzer/FuzzedDataProvider.h"
#include "innerListener_fuzz_utils.h"
#include "message_parcel.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "screenlock_common.h"
#include "screenlock_server_ipc_interface_code.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;
const std::u16string SCREENLOCK_MANAGER_INTERFACE_TOKEN = u"ohos.screenlock.ScreenLockManagerInterface";

static void NativeTokenGet()
{
    uint64_t tokenId;
    const char *perms[] = { "ohos.permission.ACCESS_SCREEN_LOCK" };

    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = static_cast<int32_t>(sizeof(perms) / sizeof(perms[0])),
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };
    infoInstance.processName = "foundation";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

static void FuzzOnRegistInnerListener(FuzzedDataProvider &fdp)
{
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    ListenType listenType = static_cast<ListenType>(fdp.ConsumeIntegral<uint8_t>() % 2);
    sptr<InnerListenerIfTest> listener = new InnerListenerIfTest();

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(userId);
    data.WriteInt32(static_cast<int32_t>(listenType));
    data.WriteRemoteObject(listener);
    MessageParcel reply;
    MessageOption option;
    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REGISTER_INNER_LISTENER), data, reply, option);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }

    OHOS::NativeTokenGet();
    FuzzedDataProvider fdp(data, size);
    OHOS::FuzzOnRegistInnerListener(fdp);
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    return 0;
}