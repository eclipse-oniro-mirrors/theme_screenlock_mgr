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

#ifndef BASE_SMALLSERVICE_SCLOCK_LOG_H
#define BASE_SMALLSERVICE_SCLOCK_LOG_H

#include <cstdint>

#define CONFIG_SCLOCK_HILOG
#ifdef CONFIG_SCLOCK_HILOG
#include "hilog/log.h"

#ifdef SCLOCK_HILOGI
#undef SCLOCK_HILOGI
#endif

#ifdef SCLOCK_HILOGE
#undef SCLOCK_HILOGE
#endif

#ifdef SCLOCK_HILOGW
#undef SCLOCK_HILOGW
#endif

#ifdef SCLOCK_HILOGI
#undef SCLOCK_HILOGI
#endif

#ifdef SCLOCK_HILOGD
#undef SCLOCK_HILOGD
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {
    LOG_CORE,
    0xD001C00,
    "SclockKit"
};

#define FILENAME_PREFIX (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define SCLOCK_HILOGF(fmt, ...)            \
    (void)OHOS::HiviewDFX::HiLog::Fatal(            \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME_PREFIX, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SCLOCK_HILOGE(fmt, ...)            \
    (void)OHOS::HiviewDFX::HiLog::Error(            \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME_PREFIX, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SCLOCK_HILOGW(fmt, ...)            \
    (void)OHOS::HiviewDFX::HiLog::Warn(            \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME_PREFIX, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SCLOCK_HILOGI(fmt, ...)            \
    (void)OHOS::HiviewDFX::HiLog::Info(            \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME_PREFIX, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SCLOCK_HILOGD(fmt, ...)            \
    (void)OHOS::HiviewDFX::HiLog::Debug(            \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME_PREFIX, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else

#define SCLOCK_HILOGF(...)
#define SCLOCK_HILOGE(...)
#define SCLOCK_HILOGW(...)
#define SCLOCK_HILOGI(...)
#define SCLOCK_HILOGD(...)
#endif // CONFIG_HILOG
#endif // BASE_SMALLSERVICE_SCLOCK_LOG_H
