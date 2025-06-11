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

#include <cstdint>
#include <aidl/android/hardware/radio/RadioTechnology.h>

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {
namespace ratUtils {

constexpr uint32_t ratbit(const RadioTechnology r) {
    return 1U << static_cast<unsigned>(r);
}

enum class ModemTechnology {
    GSM, WCDMA, CDMA, EVDO, TDSCDMA, LTE, NR
};

constexpr uint32_t kGSM =   ratbit(RadioTechnology::GSM) |
                            ratbit(RadioTechnology::GPRS) |
                            ratbit(RadioTechnology::EDGE);
constexpr uint32_t kWCDMA = ratbit(RadioTechnology::HSUPA) |
                            ratbit(RadioTechnology::HSDPA) |
                            ratbit(RadioTechnology::HSPA) |
                            ratbit(RadioTechnology::HSPAP) |
                            ratbit(RadioTechnology::UMTS);
constexpr uint32_t kCDMA =  ratbit(RadioTechnology::IS95A) |
                            ratbit(RadioTechnology::IS95B) |
                            ratbit(RadioTechnology::ONE_X_RTT);
constexpr uint32_t kEVDO =  ratbit(RadioTechnology::EVDO_0) |
                            ratbit(RadioTechnology::EVDO_A) |
                            ratbit(RadioTechnology::EVDO_B) |
                            ratbit(RadioTechnology::EHRPD);
constexpr uint32_t kTDSCDMA =
                            ratbit(RadioTechnology::TD_SCDMA);
constexpr uint32_t kLTE =   ratbit(RadioTechnology::LTE);
constexpr uint32_t kNR =    ratbit(RadioTechnology::NR);

uint32_t supportedRadioTechBitmask(const ModemTechnology mtech);
RadioTechnology currentRadioTechnology(const ModemTechnology mtech);

ModemTechnology modemTechnologyFromRadioTechnologyBitmask(
    uint32_t radioTechnologyBitmask);

uint32_t modemTechnologyBitmaskFromRadioTechnologyBitmask(
    uint32_t radioTechnologyBitmask);

}  // namespace ratUtils
}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
