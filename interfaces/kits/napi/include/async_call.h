/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef SCREENLOK_ASYNC_CALL_H
#define SCREENLOK_ASYNC_CALL_H

#include <functional>
#include <memory>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "screenlock_system_ability_interface.h"

namespace OHOS::ScreenLock {
class AsyncCall final {
public:
    class Context {
    public:
        using InputAction = std::function<napi_status(napi_env, size_t, napi_value[], napi_value)>;
        using OutputAction = std::function<napi_status(napi_env, napi_value *)>;
        using ExecAction = std::function<void(Context *)>;
        Context(InputAction input, OutputAction output) : input_(std::move(input)), output_(std::move(output)) {};
        virtual ~Context() {};
        void SetAction(const InputAction input, const OutputAction output = nullptr)
        {
            input_ = input;
            output_ = output;
        }
        void SetAction(OutputAction output)
        {
            SetAction(nullptr, std::move(output));
        }
        void SetErrorInfo(const ErrorInfo &errorInfo)
        {
            errorInfo_ = errorInfo;
        }
        napi_status operator()(const napi_env env, size_t argc, napi_value argv[], napi_value self)
        {
            if (input_ == nullptr) {
                return napi_ok;
            }
            if (self == nullptr) {
                NAPI_ASSERT_BASE(env, self != nullptr, "self is nullptr", napi_invalid_arg);
            }
            return input_(env, argc, argv, self);
        }
        napi_status operator()(const napi_env env, napi_value *result)
        {
            if (output_ == nullptr) {
                *result = nullptr;
                return napi_ok;
            }
            if (status_ != napi_ok) {
                return status_;
            }
            return output_(env, result);
        }
        virtual void Exec()
        {
            if (exec_ == nullptr) {
                return;
            }
            exec_(this);
        };
        void SetStatus(napi_status status)
        {
            status_ = status;
        }

    protected:
        friend class AsyncCall;
        InputAction input_ = nullptr;
        OutputAction output_ = nullptr;
        ExecAction exec_ = nullptr;
        ErrorInfo errorInfo_;
        napi_status status_ = napi_generic_failure;
    };

    // The default AsyncCallback in the parameters is at the end position.
    static constexpr size_t ASYNC_DEFAULT_POS = -1;
    AsyncCall(napi_env env, napi_callback_info info, Context *context, size_t pos = ASYNC_DEFAULT_POS);
    ~AsyncCall();
    napi_value Call(const napi_env env, Context::ExecAction exec = nullptr);
    napi_value SyncCall(const napi_env env, Context::ExecAction exec = nullptr);
    static void GenerateBusinessError(napi_env env, const ErrorInfo &errorInfo, napi_value *result);

private:
    enum class ARG_INFO { ARG_ERROR, ARG_DATA, ARG_BUTT };
    static void OnExecute(const napi_env env, void *data);
    static void OnComplete(const napi_env env, napi_status status, void *data);
    struct AsyncContext {
        ~AsyncContext();
        Context *ctx = nullptr;
        napi_env env = nullptr;
        napi_ref callback = nullptr;
        napi_ref self = nullptr;
        napi_deferred defer = nullptr;
        napi_async_work work = nullptr;
    };
    AsyncContext *context_ = nullptr;
};
} // namespace OHOS::ScreenLock

#endif // SCREENLOK_ASYNC_CALL_H
