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

#include <log/log_radio.h>

#include "makeRadioResponseInfo.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {

RadioResponseInfo makeRadioResponseInfo(const int32_t serial,
                                        const RadioError e) {
    return {
        .type = RadioResponseType::SOLICITED,
        .serial = serial,
        .error = e,
    };
}

RadioResponseInfo makeRadioResponseInfo(const int32_t serial) {
    return makeRadioResponseInfo(serial, RadioError::NONE);
}

RadioResponseInfo makeRadioResponseInfoUnsupported(const int32_t serial,
                                                   const char* const klass,
                                                   const char* const method) {
    RLOGE("%s::%s is not supported", klass, method);
    return makeRadioResponseInfo(serial, RadioError::REQUEST_NOT_SUPPORTED);
}

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
