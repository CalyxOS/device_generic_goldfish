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

#include <optional>
#include <string>
#include <vector>

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {

uint8_t hex2bin1(const char c);
void hex2binImpl(const char* s, uint8_t* b, size_t sz);
bool hex2bin(const std::string_view hex, std::vector<uint8_t>* bin);

char bin2hex1(const unsigned x);
void bin2hexImpl(const uint8_t* b, char* s, size_t sz);
std::string bin2hex(const uint8_t* b, size_t sz);

std::string base64encode(const void* binaryData, size_t binarySize);
std::optional<std::vector<uint8_t>> base64decode(const char* encodedData, size_t encodedSize);

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
