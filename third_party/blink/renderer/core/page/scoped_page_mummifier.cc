// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/page/scoped_page_mummifier.h"

#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/loader/frame_loader.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread_scheduler.h"
#include "third_party/blink/renderer/platform/wtf/std_lib_extras.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "third_party/blink/renderer/core/page/scoped_page_pauser.h"
#include "third_party/blink/renderer/bindings/core/v8/window_proxy_manager.h"

namespace blink {

namespace {

int g_is_mummified_counter = 0;

}  // namespace

ScopedPageMummifier::ScopedPageMummifier() {
  CHECK(!IsMummified());
  
  g_is_mummified_counter++;

  HeapVector<Member<Page>> pages;
  CopyToVector(Page::OrdinaryPages(), pages);
  for (Page* page : pages) {
    for (Frame* frame = page->MainFrame(); frame; frame = frame->Tree().TraverseNext()) {
      if (!frame->IsLocalFrame())
        continue;
      LocalFrame* local_frame = ToLocalFrame(frame);
      WindowProxyManager* window_proxy_manager = local_frame->GetWindowProxyManager();
      v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
      WindowProxyManager::GlobalProxyVector global_proxies;
      window_proxy_manager->ClearForMummification();
      // FIXME: What is GlobalProxy? Should we set empty ones?
      window_proxy_manager->ClearGlobalProxies();
      local_frame->Loader().StopAllLoaders();
    }
  }
}

ScopedPageMummifier::~ScopedPageMummifier() {
  g_is_mummified_counter--;
}

bool ScopedPageMummifier::IsMummified() {
  return g_is_mummified_counter > 0;
}

}  // namespace blink
