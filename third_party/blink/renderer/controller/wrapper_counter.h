// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CONTROLLER_WRAPPER_COUNTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CONTROLLER_WRAPPER_COUNTER_H_

#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/public/mojom/leak_detector/wrapper_counter.mojom-blink.h"
#include "third_party/blink/renderer/controller/controller_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/core/dom/document.h"

namespace blink {

class ScopedPageMummifier;

// Implementation of Leak Detector.
class CONTROLLER_EXPORT WrapperCounter : public mojom::blink::WrapperCounter {
 public:
  static void Create(mojom::blink::WrapperCounterRequest);

  WrapperCounter();
  ~WrapperCounter() override;

 private:
  // mojom::blink::WrapperCounter implementation.
  void CountWrapperObjects() override;
  void Mummify(MummifyCallback) override;
  void MummifyImpl(blink::Document* doc);
  void TimerFiredGC(TimerBase*);

  TaskRunnerTimer<WrapperCounter> delayed_gc_timer_;
  int number_of_gc_needed_ = 0;
  MummifyCallback callback_;
  std::unique_ptr<ScopedPageMummifier> scoped_page_mummifier_;

  DISALLOW_COPY_AND_ASSIGN(WrapperCounter);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CONTROLLER_WRAPPER_COUNTER_H_
