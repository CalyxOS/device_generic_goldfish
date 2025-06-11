/*
 * Copyright (C) 2025 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <memory>

#include <aidl/android/hardware/radio/ims/media/BnImsMedia.h>
#include "AtChannel.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {
using ::ndk::ScopedAStatus;

struct ImsMedia : public ims::media::BnImsMedia {
    ImsMedia(std::shared_ptr<AtChannel> atChannel);

    ScopedAStatus openSession(
            int32_t sessionId, const ims::media::LocalEndPoint& localEndPoint,
            const ims::media::RtpConfig& config) override;
    ScopedAStatus closeSession(int32_t in_sessionId) override;

    void atResponseSink(const AtResponsePtr& response);

    ScopedAStatus setListener(
            const std::shared_ptr<ims::media::IImsMediaListener>& mediaListener) override;

    std::shared_ptr<ims::media::IImsMediaListener> mMediaListener;
};

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
