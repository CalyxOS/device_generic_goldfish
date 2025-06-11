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

#include "ratUtils.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {
namespace ratUtils {

uint32_t supportedRadioTechBitmask(const ModemTechnology mtech) {
    static constexpr uint32_t kTechsBitmask[] = {
        kGSM, kWCDMA, kCDMA, kEVDO, kTDSCDMA, kLTE, kNR,
    };

    const size_t i = static_cast<size_t>(mtech);
    if (i < (sizeof(kTechsBitmask) / sizeof(kTechsBitmask[0]))) {
        return kTechsBitmask[i];
    } else {
        return 0;
    }
}

RadioTechnology currentRadioTechnology(const ModemTechnology mtech) {
    static constexpr RadioTechnology kCurrentTech[] = {
        RadioTechnology::EDGE,
        RadioTechnology::HSPA,
        RadioTechnology::IS95B,
        RadioTechnology::EVDO_B,
        RadioTechnology::TD_SCDMA,
        RadioTechnology::LTE,
        RadioTechnology::NR,
    };

    const size_t i = static_cast<size_t>(mtech);
    if (i < (sizeof(kCurrentTech) / sizeof(kCurrentTech[0]))) {
        return kCurrentTech[i];
    } else {
        return RadioTechnology::EDGE;
    }
}

ModemTechnology modemTechnologyFromRadioTechnologyBitmask(
        const uint32_t radioTechnologyBitmask) {
    if (radioTechnologyBitmask & kNR) {
        return ModemTechnology::NR;
    }
    if (radioTechnologyBitmask & kLTE) {
        return ModemTechnology::LTE;
    }
    if (radioTechnologyBitmask & kTDSCDMA) {
        return ModemTechnology::TDSCDMA;
    }
    if (radioTechnologyBitmask & kEVDO) {
        return ModemTechnology::EVDO;
    }
    if (radioTechnologyBitmask & kCDMA) {
        return ModemTechnology::CDMA;
    }
    if (radioTechnologyBitmask & kWCDMA) {
        return ModemTechnology::WCDMA;
    }
    return ModemTechnology::GSM;
}

uint32_t modemTechnologyBitmaskFromRadioTechnologyBitmask(
    const uint32_t radioTechnologyBitmask) {
    uint32_t modemTechnologyBitmask = 0;

    const auto mtechBit = [](const ModemTechnology mtech){
        return 1U << static_cast<unsigned>(mtech);
    };

    if (radioTechnologyBitmask & kNR) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::NR);
    }
    if (radioTechnologyBitmask & kLTE) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::LTE);
    }
    if (radioTechnologyBitmask & kTDSCDMA) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::TDSCDMA);
    }
    if (radioTechnologyBitmask & kEVDO) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::EVDO);
    }
    if (radioTechnologyBitmask & kCDMA) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::CDMA);
    }
    if (radioTechnologyBitmask & kWCDMA) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::WCDMA);
    }

    if (radioTechnologyBitmask & kGSM) {
        modemTechnologyBitmask |=
            mtechBit(ModemTechnology::GSM);
    }

    return modemTechnologyBitmask;
}


}  // namespace ratUtils
}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
