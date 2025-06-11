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
#include <mutex>

#include <aidl/android/hardware/radio/modem/BnRadioModem.h>
#include "AtChannel.h"
#include "AtResponse.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {
using ::ndk::ScopedAStatus;

struct RadioModem : public modem::BnRadioModem {
    RadioModem(std::shared_ptr<AtChannel> atChannel);

    ScopedAStatus enableModem(int32_t serial, bool on) override;
    ScopedAStatus getBasebandVersion(int32_t serial) override;
    ScopedAStatus getDeviceIdentity(int32_t serial) override;
    ScopedAStatus getImei(int32_t serial) override;
    ScopedAStatus getHardwareConfig(int32_t serial) override;
    ScopedAStatus getModemActivityInfo(int32_t serial) override;
    ScopedAStatus getModemStackStatus(int32_t serial) override;
    ScopedAStatus getRadioCapability(int32_t serial) override;
    ScopedAStatus nvReadItem(
            int32_t serial, modem::NvItem itemId) override;
    ScopedAStatus nvResetConfig(
            int32_t serial, modem::ResetNvType type) override;
    ScopedAStatus nvWriteCdmaPrl(int32_t serial, const std::vector<uint8_t>& prl) override;
    ScopedAStatus nvWriteItem(
            int32_t serial, const modem::NvWriteItem& i) override;
    ScopedAStatus requestShutdown(int32_t serial) override;
    ScopedAStatus sendDeviceState(
            int32_t serial, modem::DeviceStateType stateType,
            bool state) override;
    ScopedAStatus setRadioCapability(
            int32_t s, const modem::RadioCapability& rc) override;
    ScopedAStatus setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
                                bool preferredForEmergencyCall) override;

    void atResponseSink(const AtResponsePtr& response);
    void handleUnsolicited(const AtResponse::CFUN& cfun);
    template <class IGNORE> void handleUnsolicited(const IGNORE&) {}

    ScopedAStatus responseAcknowledgement() override;
    ScopedAStatus setResponseFunctions(
            const std::shared_ptr<modem::IRadioModemResponse>& radioModemResponse,
            const std::shared_ptr<modem::IRadioModemIndication>& radioModemIndication) override;

private:
    std::pair<RadioError, uint32_t> getSupportedRadioTechs(
            const AtChannel::RequestPipe requestPipe,
            AtChannel::Conversation& atConversation);

    bool setRadioPowerImpl(const AtChannel::RequestPipe requestPipe,
                           bool powerOn);

    const std::shared_ptr<AtChannel> mAtChannel;
    AtChannel::Conversation mAtConversation;
    std::shared_ptr<modem::IRadioModemResponse> mRadioModemResponse;
    std::shared_ptr<modem::IRadioModemIndication> mRadioModemIndication;

    modem::RadioState   mRadioState = modem::RadioState::OFF;
    std::mutex          mMtx;
};

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
