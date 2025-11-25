/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef SCREENLOCK_COMMEVENT_SUBSCRIBE_H
#define SCREENLOCK_COMMEVENT_SUBSCRIBE_H

#include <string>
#include <singleton.h>

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"

namespace OHOS {
namespace ScreenLock {
using CommonEventSubscriber = OHOS::EventFwk::CommonEventSubscriber;
using CommonEventData = OHOS::EventFwk::CommonEventData;
using CommonEventSubscribeInfo = OHOS::EventFwk::CommonEventSubscribeInfo;

class CommeventMgr {
    DECLARE_SINGLETON(CommeventMgr)
public:
    void SubscribeEvent();
    void UnSubscribeEvent();
    void OnReceiveEvent(const AAFwk::Want &want);

private:
    class CommEventSubscriber : public CommonEventSubscriber {
    public:
        explicit CommEventSubscriber(CommonEventSubscribeInfo &subscriberInfo, CommeventMgr &registry)
            : CommonEventSubscriber(subscriberInfo), registry_(registry)
        {}
        ~CommEventSubscriber() = default;

        void OnReceiveEvent(const CommonEventData &data) override
        {
            registry_.OnReceiveEvent(data.GetWant());
        }

    private:
        CommeventMgr &registry_;
    };

    std::shared_ptr<CommEventSubscriber> subscriber_ = nullptr;
    std::mutex lock_;
};
} // namespace ScreenLock
} // namespace OHOS

#endif  //SCREENLOCK_COMMEVENT_SUBSCRIBE_H