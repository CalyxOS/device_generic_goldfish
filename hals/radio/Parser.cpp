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

#include <charconv>

#include "debug.h"
#include "Parser.h"

Parser& Parser::skip(const char c) {
    if (mBegin) {
        if (c == ' ') {
            while ((mBegin != mEnd) && (*mBegin <= ' ')) {
                ++mBegin;
            }
        } else if ((mBegin != mEnd) && (*mBegin == c)) {
            ++mBegin;
        } else {
            mBegin = FAILURE(nullptr);
        }
    }

    return *this;
}

Parser& Parser::skip(const char* s) {
    if (mBegin) {
        while (mBegin != mEnd) {
            const char c = *s;
            if (!c) {
                return *this;
            } else if (c == *mBegin) {
                ++mBegin;
                ++s;
            } else {
                mBegin = FAILURE(nullptr);
                return *this;
            }
        }

        if (*s) {
            mBegin = FAILURE(nullptr);
        }
    }

    return *this;
}

Parser& Parser::operator()(char* result) {
    if (mBegin) {
        if (mBegin != mEnd) {
            *result = *mBegin;
            ++mBegin;
        } else {
            mBegin = FAILURE(nullptr);
        }
    }

    return *this;
}

Parser& Parser::operator()(int* result, const int base) {
    if (mBegin) {
        if (mBegin != mEnd) {
            const auto [unconsumed, ec] =
                std::from_chars(mBegin, mEnd, *result, base);
            if (ec == std::errc()) {
                mBegin = unconsumed;
            } else {
                mBegin = FAILURE(nullptr);
            }
        } else {
            mBegin = FAILURE(nullptr);
        }
    }

    return *this;
}

Parser& Parser::operator()(std::string_view* result, const char end) {
    if (mBegin) {
        for (const char* i = mBegin; i != mEnd; ++i) {
            if (*i == end) {
                *result = std::string_view(mBegin, i - mBegin);
                mBegin = i + 1;
                return *this;
            }
        }

        mBegin = FAILURE(nullptr);
    }

    return *this;
}

Parser& Parser::operator()(std::string* result, const char end) {
    std::string_view view;
    if ((*this)(&view, end).matchSoFar()) {
        *result = std::string(view.data(), view.size());
    }
    return *this;
}

std::string_view Parser::remaining() {
    const char* begin = mBegin;
    if (begin) {
        mBegin = mEnd;
        return std::string_view(begin, mEnd - begin);
    } else {
        return {};
    }
}

std::string Parser::remainingAsString() {
    std::string_view rem = remaining();
    return std::string(rem.data(), rem.size());
}

int Parser::consumed() const {
    return mBegin ? int(mBegin - mImmutableBegin) : -1;
}
