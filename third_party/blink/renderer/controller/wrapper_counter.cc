// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/controller/wrapper_counter.h"

#include "mojo/public/cpp/bindings/strong_binding.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_gc_controller.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_gc_for_context_dispose.h"
#include "third_party/blink/renderer/bindings/core/v8/window_proxy_manager.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/core_initializer.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/page/scoped_page_mummifier.h"
#include "third_party/blink/renderer/platform/heap/heap_stats_collector.h"
#include "third_party/blink/renderer/platform/heap/thread_state.h"
#include "third_party/blink/renderer/platform/heap/heap.h"

#include "third_party/blink/renderer/bindings/core/v8/v8_gc_controller.h"
#include "third_party/blink/renderer/platform/bindings/v8_per_isolate_data.h"

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

#include "third_party/blink/renderer/core/page/scoped_page_pauser.h"

#include "third_party/blink/renderer/core/workers/dedicated_worker_messaging_proxy.h"

namespace blink {

WrapperCounter::WrapperCounter()
 : delayed_gc_timer_(Thread::Current()->GetTaskRunner(),
                    this,
                    &WrapperCounter::TimerFiredGC) {
  LOG(ERROR) << "WrapperCounter::WrapperCounter =====================================";
}

WrapperCounter::~WrapperCounter() = default;

// static
void WrapperCounter::Create(mojom::blink::WrapperCounterRequest request) {
  mojo::MakeStrongBinding(std::make_unique<WrapperCounter>(),
                          std::move(request));
}

void WrapperCounter::CountWrapperObjects() {
  LOG(ERROR) << "WrapperCounter::CountWrapperObjects " << V8GCController::CountDOMWrappers(v8::Isolate::GetCurrent());
}

void WrapperCounter::Mummify(MummifyCallback callback) {
  LOG(ERROR) << "WrapperCounter::Mummify =====================================";
  callback_ = std::move(callback);
  for (const auto& page : Page::OrdinaryPages()) {
      if (page->MainFrame()->IsLocalFrame()) {
        MummifyImpl(ToLocalFrame(page->MainFrame())->GetDocument());
      }
  }
  number_of_gc_needed_ = 10;
  delayed_gc_timer_.StartOneShot( TimeDelta::FromMilliseconds(100), FROM_HERE);
}

void WrapperCounter::MummifyImpl(Document* doc){
  LOG(ERROR) << "WrapperCounter::MummifyImpl =====================================";
  scoped_page_mummifier_ = std::make_unique<ScopedPageMummifier>();
}

void WrapperCounter::TimerFiredGC(TimerBase*) {

  LOG(ERROR) << "WrapperCounter::TimerFiredGC =====================================";
  // Force GC
  V8GCController::CollectAllGarbageForTesting(
      V8PerIsolateData::MainThreadIsolate(),
      v8::EmbedderHeapTracer::EmbedderStackState::kEmpty);

  if (--number_of_gc_needed_ > 0) {
    delayed_gc_timer_.StartOneShot( TimeDelta::FromMilliseconds(1000), FROM_HERE);
  }  else if (number_of_gc_needed_ > -1 &&
             DedicatedWorkerMessagingProxy::ProxyCount()) {
    delayed_gc_timer_.StartOneShot( TimeDelta::FromMilliseconds(1000), FROM_HERE);
  } else {
    //showLiveSWInstances();
    mojom::blink::WrapperCounterResultPtr result = mojom::blink::WrapperCounterResult::New();
    result->number_of_remaining_wrappers = V8GCController::CountDOMWrappers(v8::Isolate::GetCurrent());
    V8GCController::FindPersistentLeaks(v8::Isolate::GetCurrent());
    LOG(ERROR) << result->number_of_remaining_wrappers;
    std::move(callback_).Run(std::move(result));
    scoped_page_mummifier_ = nullptr;
  }
}



}  // namespace blink