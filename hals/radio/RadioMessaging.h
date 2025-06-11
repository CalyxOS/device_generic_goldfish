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

#include <aidl/android/hardware/radio/messaging/BnRadioMessaging.h>
#include "AtChannel.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {
using ::ndk::ScopedAStatus;

struct RadioMessaging : public messaging::BnRadioMessaging {
    RadioMessaging(std::shared_ptr<AtChannel> atChannel);

    ScopedAStatus acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                                   const std::string& ackPdu) override;
    ScopedAStatus acknowledgeLastIncomingCdmaSms(
            int32_t serial, const messaging::CdmaSmsAck& smsAck) override;
    ScopedAStatus acknowledgeLastIncomingGsmSms(
            int32_t serial, bool success,
            messaging::SmsAcknowledgeFailCause cause) override;
    ScopedAStatus deleteSmsOnRuim(int32_t serial, int32_t index) override;
    ScopedAStatus deleteSmsOnSim(int32_t serial, int32_t index) override;
    ScopedAStatus getCdmaBroadcastConfig(int32_t serial) override;
    ScopedAStatus getGsmBroadcastConfig(int32_t serial) override;
    ScopedAStatus getSmscAddress(int32_t serial) override;
    ScopedAStatus reportSmsMemoryStatus(int32_t serial, bool available) override;
    ScopedAStatus sendCdmaSms(
            int32_t serial, const messaging::CdmaSmsMessage& sms) override;
    ScopedAStatus sendCdmaSmsExpectMore(
            int32_t serial, const messaging::CdmaSmsMessage& sms) override;
    ScopedAStatus sendImsSms(
            int32_t serial, const messaging::ImsSmsMessage& message) override;
    ScopedAStatus sendSms(
            int32_t serial, const messaging::GsmSmsMessage& message) override;
    ScopedAStatus sendSmsExpectMore(
            int32_t serial, const messaging::GsmSmsMessage& message) override;
    ScopedAStatus setCdmaBroadcastConfig(
            int32_t serial, const std::vector<messaging::CdmaBroadcastSmsConfigInfo>&
                    configInfo) override;
    ScopedAStatus setCdmaBroadcastActivation(int32_t serial, bool activate) override;
    ScopedAStatus setGsmBroadcastConfig(
            int32_t serial, const std::vector<messaging::GsmBroadcastSmsConfigInfo>&
                    configInfo) override;
    ScopedAStatus setGsmBroadcastActivation(int32_t serial, bool activate) override;
    ScopedAStatus setSmscAddress(int32_t serial, const std::string& smsc) override;
    ScopedAStatus writeSmsToRuim(
            int32_t serial, const messaging::CdmaSmsWriteArgs& cdmaSms) override;
    ScopedAStatus writeSmsToSim(
            int32_t serial, const messaging::SmsWriteArgs& smsWriteArgs) override;

    void atResponseSink(const AtResponsePtr& response);
    void handleUnsolicited(const AtResponse::CMT&);
    void handleUnsolicited(const AtResponse::CDS&);
    template <class IGNORE> void handleUnsolicited(const IGNORE&) {}

    ScopedAStatus responseAcknowledgement() override;
    ScopedAStatus setResponseFunctions(
            const std::shared_ptr<messaging::IRadioMessagingResponse>& radioMessagingResponse,
            const std::shared_ptr<messaging::IRadioMessagingIndication>& radioMessagingIndication) override;

private:
    std::pair<RadioResponseInfo, messaging::SendSmsResult>
        sendSmsImpl(AtChannel::RequestPipe, int serial,
                    const messaging::GsmSmsMessage &);

    const std::shared_ptr<AtChannel> mAtChannel;
    AtChannel::Conversation mAtConversation;
    std::shared_ptr<messaging::IRadioMessagingResponse> mRadioMessagingResponse;
    std::shared_ptr<messaging::IRadioMessagingIndication> mRadioMessagingIndication;
};

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
