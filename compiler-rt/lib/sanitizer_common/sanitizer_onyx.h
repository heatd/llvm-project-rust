//===-- sanitizer_onyx.h ------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
//
// Onyx-specific sanitizer support.
//
//===---------------------------------------------------------------------===//
#ifndef SANITIZER_ONYX_H
#define SANITIZER_ONYX_H

#include "sanitizer_platform.h"
#if SANITIZER_ONYX

#include "sanitizer_common.h"

#include <onyx/public/process.h>

namespace __sanitizer {

struct MemoryMappingLayoutData {
  InternalMmapVector<char> data;
  size_t current;  // Current offset into the vector.
};

}

#endif

#endif
