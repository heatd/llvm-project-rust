//===-- sanitizer_stoptheworld_onyx.cpp -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
//
// See sanitizer_stoptheworld.h for details.
//
//===---------------------------------------------------------------------===//

#include "sanitizer_platform.h"

#if SANITIZER_ONYX

#include "sanitizer_stoptheworld.h"

namespace __sanitizer {

void StopTheWorld(StopTheWorldCallback callback, void *argument) {
    // TODO: Implement
}  // namespace __sanitizer

}

#endif  // SANITIZER_ONYX
