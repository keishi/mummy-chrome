// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_LAYOUT_TEST_WRAPPER_COUNTER_H_
#define CONTENT_SHELL_BROWSER_LAYOUT_TEST_WRAPPER_COUNTER_H_

#include <memory>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "third_party/blink/public/mojom/leak_detector/wrapper_counter.mojom.h"

namespace content {

class RenderProcessHost;

// WrapperCounter counts wrapper objects and compare them between two pages.
class WrapperCounter {
 public:
  WrapperCounter();
  ~WrapperCounter();

  struct WrapperCounterReport {
    bool none;
    std::string detail;
  };
  using ReportCallback = base::OnceCallback<void(const WrapperCounterReport&)>;

  void BindInterface(RenderProcessHost* host, ReportCallback callback);
  void TryWrapperCounts();

 private:
  void OnCountWrappersComplete(blink::mojom::WrapperCounterResultPtr result);

  blink::mojom::WrapperCounterPtr wrapper_counter_;
  ReportCallback callback_;
  base::WeakPtrFactory<WrapperCounter> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(WrapperCounter);
};

}  // namespace content

#endif  // CONTENT_SHELL_BROWSER_LAYOUT_TEST_WRAPPER_COUNTER_H_
