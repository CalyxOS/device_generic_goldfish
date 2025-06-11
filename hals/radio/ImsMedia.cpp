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

#define FAILURE_DEBUG_PREFIX "ImsMedia"

#include "ImsMedia.h"
#include "debug.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {

ImsMedia::ImsMedia(std::shared_ptr<AtChannel> /*atChannel*/) {
}

ScopedAStatus ImsMedia::openSession(
        const int32_t sessionId, const ims::media::LocalEndPoint& /*localEndPoint*/,
        const ims::media::RtpConfig& /*config*/) {
    NOT_NULL(mMediaListener)->onOpenSessionFailure(
        sessionId, FAILURE(ims::media::RtpError::NOT_SUPPORTED));
    return ScopedAStatus::ok();
}

ScopedAStatus ImsMedia::closeSession(const int32_t sessionId) {
    NOT_NULL(mMediaListener)->onSessionClosed(sessionId);
    return ScopedAStatus::ok();
}

void ImsMedia::atResponseSink(const AtResponsePtr& /*response*/) {}

ScopedAStatus ImsMedia::setListener(
        const std::shared_ptr<ims::media::IImsMediaListener>& mediaListener) {
    mMediaListener = NOT_NULL(mediaListener);
    return ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
