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
#include <string>
#include <string_view>

struct Parser {
    Parser(std::string_view str)
        : mImmutableBegin(str.data())
        , mBegin(str.data())
        , mEnd(str.data() + str.size()) {}

    bool matchSoFar() const { return mBegin != nullptr; }
    bool fullMatch() const { return matchSoFar() && (mBegin == mEnd); }
    bool hasMore() const { return matchSoFar() && (mBegin != mEnd); }
    char front() const { return hasMore() ? *mBegin : char(-1); }

    Parser& skip(char c);
    Parser& skip(const char* s);
    Parser& operator()(char* result);
    Parser& operator()(int* result, int base = 10);
    Parser& operator()(std::string_view* result, char end);
    Parser& operator()(std::string* result, char end);

    std::string_view remaining();
    std::string remainingAsString();
    int consumed() const;

private:
    const char* const mImmutableBegin;
    const char* mBegin;
    const char* mEnd;
};
