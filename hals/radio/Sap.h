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

#include <aidl/android/hardware/radio/sap/BnSap.h>
#include "AtChannel.h"
#include "AtResponse.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {
using ::ndk::ScopedAStatus;

struct Sap : public sap::BnSap {
    Sap(std::shared_ptr<AtChannel> atChannel);

    ScopedAStatus apduReq(int32_t serial, sap::SapApduType type,
                          const std::vector<uint8_t>& command) override;
    ScopedAStatus connectReq(int32_t serial, int32_t maxMsgSize) override;
    ScopedAStatus disconnectReq(int32_t serial) override;
    ScopedAStatus powerReq(int32_t serial, bool state) override;
    ScopedAStatus resetSimReq(int32_t serial) override;
    ScopedAStatus setTransferProtocolReq(
            int32_t serial, sap::SapTransferProtocol transferProtocol) override;
    ScopedAStatus transferAtrReq(int32_t serial) override;
    ScopedAStatus transferCardReaderStatusReq(int32_t serial) override;

    void atResponseSink(const AtResponsePtr& response);

    ScopedAStatus setCallback(const std::shared_ptr<sap::ISapCallback>& sapCallback) override;

private:
    std::shared_ptr<sap::ISapCallback> mSapCallback;
};

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
