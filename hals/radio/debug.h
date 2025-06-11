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
#include <log/log_main.h>
#include <log/log_radio.h>

#ifdef FAILURE_DEBUG_PREFIX

#define FAILURE(X) \
    (RLOGE("%s:%s:%d failure: %s", FAILURE_DEBUG_PREFIX, __func__, __LINE__, #X), X)

#define FAILURE_V(X, FMT, ...) \
    (RLOGE("%s:%s:%d failure: " FMT, FAILURE_DEBUG_PREFIX, __func__, __LINE__, __VA_ARGS__), X)

#define NOT_NULL(P) (LOG_ALWAYS_FATAL_IF(!(P), "%s:%d %s is nullptr", __func__, __LINE__, #P), P)

#else

#define FAILURE(X) \
    (RLOGE("%s:%d failure: %s", __func__, __LINE__, #X), X)

#define FAILURE_V(X, FMT, ...) \
    (RLOGE("%s:%d failure: " FMT, __func__, __LINE__, __VA_ARGS__), X)

#define NOT_NULL(P) (LOG_ALWAYS_FATAL_IF(!(P), "%s:%s:%d %s is nullptr", \
                                         FAILURE_DEBUG_PREFIX, __func__, __LINE__, #P), \
                     P)

#endif  // ifdef FAILURE_DEBUG_PREFIX
