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
#define LOG_TAG "AsyncCall"

#include "async_call.h"

#include "sclock_log.h"

namespace OHOS::ScreenLock {
AsyncCall::AsyncCall(napi_env env, napi_callback_info info, Context *context, size_t pos)
{
    SCLOCK_HILOGD("AsyncCall begin");
    context_ = new AsyncContext();
    context_->env = env;
    if (context_ == nullptr) {
        SCLOCK_HILOGD("context_ is null");
        return;
    }
    size_t argc = 6;
    napi_value self = nullptr;
    napi_value argv[6] = {nullptr};
    NAPI_CALL_RETURN_VOID(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    pos = ((pos == ASYNC_DEFAULT_POS) ? (argc - 1) : pos);
    if (pos >= 0 && pos < argc) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[pos], &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv[pos], 1, &context_->callback);
            argc = pos;
        }
    }
    if (context == nullptr) {
        SCLOCK_HILOGD("context is null");
        return;
    }
    NAPI_CALL_RETURN_VOID(env, (*context)(env, argc, argv, self));
    context_->ctx = context;
    napi_create_reference(env, self, 1, &context_->self);
    SCLOCK_HILOGD("AsyncCall end");
}

AsyncCall::~AsyncCall()
{
    if (context_ == nullptr) {
        return;
    }
    delete context_;
}

napi_value AsyncCall::Call(const napi_env env, Context::ExecAction exec)
{
    if (context_ == nullptr) {
        SCLOCK_HILOGD("context_ is null");
        return nullptr;
    }
    if (context_->ctx == nullptr) {
        SCLOCK_HILOGD("context_->ctx is null");
        return nullptr;
    }
    SCLOCK_HILOGD("async call exec");
    context_->ctx->exec_ = std::move(exec);
    napi_value promise = nullptr;
    if (context_->callback == nullptr) {
        napi_create_promise(env, &context_->defer, &promise);
    } else {
        napi_get_undefined(env, &promise);
    }
    napi_async_work work = context_->work;
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "AsyncCall", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, AsyncCall::OnExecute, AsyncCall::OnComplete, context_, &work);
    context_->work = work;
    context_ = nullptr;
    napi_queue_async_work(env, work);
    SCLOCK_HILOGD("async call exec");
    return promise;
}

napi_value AsyncCall::SyncCall(const napi_env env, AsyncCall::Context::ExecAction exec)
{
    if ((context_ == nullptr) || (context_->ctx == nullptr)) {
        SCLOCK_HILOGD("context_ or context_->ctx is null");
        return nullptr;
    }
    context_->ctx->exec_ = std::move(exec);
    napi_value promise = nullptr;
    if (context_->callback == nullptr) {
        napi_create_promise(env, &context_->defer, &promise);
    } else {
        napi_get_undefined(env, &promise);
    }
    AsyncCall::OnExecute(env, context_);
    AsyncCall::OnComplete(env, napi_ok, context_);
    context_ = nullptr;
    return promise;
}

void AsyncCall::OnExecute(const napi_env env, void *data)
{
    SCLOCK_HILOGD("run the async runnable");
    AsyncContext *context = reinterpret_cast<AsyncContext *>(data);
    context->ctx->Exec();
}

void AsyncCall::OnComplete(const napi_env env, napi_status status, void *data)
{
    SCLOCK_HILOGD("run the js callback function");
    AsyncContext *context = reinterpret_cast<AsyncContext *>(data);
    napi_value output = nullptr;
    napi_status runStatus = (*context->ctx)(env, &output);
    napi_value result[static_cast<uint32_t>(ARG_INFO::ARG_BUTT)] = { nullptr };
    SCLOCK_HILOGD("run the js callback function:status[%{public}d]runStatus[%{public}d]", status, runStatus);
    if (status == napi_ok && runStatus == napi_ok) {
        napi_get_undefined(env, &result[static_cast<uint32_t>(ARG_INFO::ARG_ERROR)]);
        if (output != nullptr) {
            SCLOCK_HILOGD("AsyncCall::OnComplete output != nullptr");
            result[static_cast<uint32_t>(ARG_INFO::ARG_DATA)] = output;
        } else {
            SCLOCK_HILOGD("AsyncCall::OnComplete output == nullptr");
            napi_get_undefined(env, &result[static_cast<uint32_t>(ARG_INFO::ARG_DATA)]);
        }
    } else {
        napi_get_undefined(env, &result[static_cast<uint32_t>(ARG_INFO::ARG_DATA)]);
        GenerateBusinessError(env, context->ctx->errorInfo_, &result[static_cast<uint32_t>(ARG_INFO::ARG_ERROR)]);
    }
    SCLOCK_HILOGD("run the js callback function:(context->defer != nullptr)?[%{public}d]", context->defer != nullptr);
    if (context->defer != nullptr) {
        // promise
        SCLOCK_HILOGD("Promise to do!");
        if (status == napi_ok && runStatus == napi_ok) {
            napi_resolve_deferred(env, context->defer, result[static_cast<uint32_t>(ARG_INFO::ARG_DATA)]);
        } else {
            napi_reject_deferred(env, context->defer, result[static_cast<uint32_t>(ARG_INFO::ARG_ERROR)]);
        }
    } else {
        // callback
        SCLOCK_HILOGD("Callback to do!");
        napi_value callback = nullptr;
        napi_get_reference_value(env, context->callback, &callback);
        napi_value returnValue;
        napi_call_function(env, nullptr, callback, static_cast<size_t>(ARG_INFO::ARG_BUTT), result, &returnValue);
    }
    delete context;
}

AsyncCall::AsyncContext::~AsyncContext()
{
    if (env != nullptr) {
        napi_delete_reference(env, callback);
        napi_delete_reference(env, self);
        napi_delete_async_work(env, work);
    }
    delete ctx;
}

void AsyncCall::GenerateBusinessError(napi_env env, const ErrorInfo &errorInfo, napi_value *result)
{
    napi_value message = nullptr;
    napi_value code = nullptr;
    napi_create_int32(env, errorInfo.errorCode_, &code);
    napi_create_string_utf8(env, errorInfo.message_.c_str(), NAPI_AUTO_LENGTH, &message);
    napi_create_object(env, result);
    napi_set_named_property(env, *result, "code", code);
    napi_set_named_property(env, *result, "message", message);
}
} // namespace OHOS::ScreenLock