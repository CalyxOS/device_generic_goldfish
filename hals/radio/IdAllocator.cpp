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

#include "IdAllocator.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace implementation {

int32_t IdAllocator::get() {
    const auto i = mReturnedIds.rbegin();

    if (i == mReturnedIds.rend()) {
        return ++mIdGenerator;
    } else {
        const int32_t id = *i;
        mReturnedIds.erase(std::next(i).base());
        return id;
    }
}

void IdAllocator::put(const int32_t id) {
    if (id == mIdGenerator) {
        --mIdGenerator;

        auto i = mReturnedIds.begin();
        while (i != mReturnedIds.end()) {
            if (*i == mIdGenerator) {
                --mIdGenerator;
                i = mReturnedIds.erase(i);
            } else {
                break;
            }
        }
    } else {
        mReturnedIds.insert(id);
    }
}

}  // namespace implementation
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
