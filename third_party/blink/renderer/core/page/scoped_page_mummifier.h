// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_SCOPED_PAGE_MUMMIFIER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_SCOPED_PAGE_MUMMIFIER_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread_scheduler.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class Page;
class ScopedPagePauser;

class CORE_EXPORT ScopedPageMummifier final {
  USING_FAST_MALLOC(ScopedPageMummifier);

 public:
  explicit ScopedPageMummifier();
  ~ScopedPageMummifier();

  static bool IsMummified();

 private:
  friend class Page;

  std::unique_ptr<ScopedPagePauser> pauser_;
  DISALLOW_COPY_AND_ASSIGN(ScopedPageMummifier);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_SCOPED_PAGE_MUMMIFIER_H_
