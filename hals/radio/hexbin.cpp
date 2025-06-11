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

#include "debug.h"
#include "hexbin.h"

#include <openssl/base64.h>

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {

uint8_t hex2bin1(const char c) {
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    } else if ((c >= 'a') && (c <= 'f')) {
        return c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    } else {
        return 0;
    }
}

void hex2binImpl(const char* s, uint8_t* b, size_t sz) {
    for (; sz > 0; s += 2, ++b, --sz) {
        *b = (hex2bin1(s[0]) << 4) | hex2bin1(s[1]);
    }
}

bool hex2bin(const std::string_view hex, std::vector<uint8_t>* bin) {
    if (hex.size() & 1) {
        return FAILURE_V(false, "%s", "odd length");
    }

    const size_t sz = hex.size() / 2;
    bin->resize(sz);
    hex2binImpl(hex.data(), bin->data(), sz);
    return true;
}

char bin2hex1(const unsigned x) {
    return (x < 10) ? char(x + '0') : char(x - 10 + 'A');
}

void bin2hexImpl(const uint8_t* b, char* s, size_t sz) {
    for (; sz > 0; s += 2, ++b, --sz) {
        const unsigned bb = *b;
        s[0] = bin2hex1(bb >> 4);
        s[1] = bin2hex1(bb & 0xF);
    }
}

std::string bin2hex(const uint8_t* b, const size_t sz) {
    std::string str(sz + sz, '?');
    bin2hexImpl(b, str.data(), sz);
    return str;
}

std::string base64encode(const void* const binaryData, const size_t binarySize) {
    int size = ((binarySize + 2) / 3 * 4) + 1;  // `+1` is for the "trailing NUL"
    std::string encoded(size, '?');
    size = EVP_EncodeBlock(reinterpret_cast<uint8_t *>(encoded.data()),
                           static_cast<const uint8_t*>(binaryData),
                           binarySize);
    encoded.resize(size);  // without "trailing NUL"
    LOG_ALWAYS_FATAL_IF(size < 0);
    return encoded;
}

std::optional<std::vector<uint8_t>> base64decode(const char* const encodedData, const size_t encodedSize) {
    if (encodedSize % 4) {
        return std::nullopt;
    }

    int size = encodedSize / 4 * 3;
    std::vector<uint8_t> decoded(size);
    size = EVP_DecodeBlock(decoded.data(),
                           reinterpret_cast<const uint8_t*>(encodedData),
                           encodedSize);
    if (size < 0) {
        return std::nullopt;
    } else {
        decoded.resize(size);
        return decoded;
    }
}

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
