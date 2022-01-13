//===-- sanitizer_procmaps_onyx.cpp
//----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Information about the process mappings (Onyx-specific parts).
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"
#if SANITIZER_ONYX
#include <onyx/public/process.h>
#include <onyx/public/handle.h>

#include "sanitizer_errno.h"
#include "sanitizer_common.h"
#include "sanitizer_procmaps.h"

namespace __sanitizer {

MemoryMappingLayout::MemoryMappingLayout(bool) { Reset(); }

void MemoryMappingLayout::Reset() {
  data_.data.clear();
  data_.current = 0;

  size_t count = 0;
  int handle = internal_onx_handle_open(ONX_HANDLE_TYPE_PROCESS, internal_getpid(), ONX_HANDLE_CLOEXEC);
  if (handle < 0) {
    return;
  }

  size_t filled;
  do {
    data_.data.resize(count);
    auto status = internal_onx_handle_query(handle, data_.data.data(),
        count, PROCESS_GET_VM_REGIONS, &count, nullptr);
    if (status == -1 && errno != errno_ENOSPC) {
      data_.data.clear();
      internal_onx_handle_close(handle);
      return;
    }

    filled = status;
  } while (filled < count);

  internal_onx_handle_close(handle);
}

MemoryMappingLayout::~MemoryMappingLayout() {}

bool MemoryMappingLayout::Error() const { return data_.data.empty(); }

bool MemoryMappingLayout::Next(MemoryMappedSegment *segment) {
  while (data_.current < data_.data.size()) {
    const onx_process_vm_region *reg = (onx_process_vm_region *) &data_.data[data_.current];
    segment->start = reg->start;
    segment->end = reg->length;
    segment->offset = reg->offset;
    const auto flags = reg->protection;
    segment->protection =
        ((flags & VM_REGION_PROT_READ) ? kProtectionRead : 0) |
        ((flags & VM_REGION_PROT_WRITE) ? kProtectionWrite : 0) |
        ((flags & VM_REGION_PROT_EXEC) ? kProtectionExecute : 0);
    if (segment->filename && segment->filename_size > 0) {
      uptr len = Min(reg->size - sizeof(onx_process_vm_region), segment->filename_size) - 1;
      internal_strncpy(segment->filename, reg->name, len);
      segment->filename[len] = 0;
    }

    data_.current += reg->size;
    return true;
  }
  return false;
}

void MemoryMappingLayout::DumpListOfModules(
    InternalMmapVectorNoCtor<LoadedModule> *modules) {
  Reset();
  InternalMmapVector<char> module_name(kMaxPathLength);
  MemoryMappedSegment segment(module_name.data(), module_name.size());
  for (uptr i = 0; Next(&segment); i++) {
    const char *cur_name = segment.filename;
    if (cur_name[0] == '\0')
      continue;
    // Don't subtract 'cur_beg' from the first entry:
    // * If a binary is compiled w/o -pie, then the first entry in
    //   process maps is likely the binary itself (all dynamic libs
    //   are mapped higher in address space). For such a binary,
    //   instruction offset in binary coincides with the actual
    //   instruction address in virtual memory (as code section
    //   is mapped to a fixed memory range).
    // * If a binary is compiled with -pie, all the modules are
    //   mapped high at address space (in particular, higher than
    //   shadow memory of the tool), so the module can't be the
    //   first entry.
    uptr base_address = (i ? segment.start : 0) - segment.offset;
    LoadedModule cur_module;
    cur_module.set(cur_name, base_address);
    segment.AddAddressRanges(&cur_module);
    modules->push_back(cur_module);
  }
}

void MemoryMappedSegment::AddAddressRanges(LoadedModule *module) {
  // data_ should be unused on this platform
  CHECK(!data_);
  module->addAddressRange(start, end, IsExecutable(), IsWritable());
}

void GetMemoryProfile(fill_profile_f cb, uptr *stats, uptr stats_size)
{
  // TODO: Implement
}

}  // namespace __sanitizer

#endif  // SANITIZER_ONYX
