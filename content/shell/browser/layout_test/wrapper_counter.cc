// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/layout_test/wrapper_counter.h"

#include <utility>

#include "base/bind_helpers.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/values.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/bind_interface_helpers.h"

namespace content {
  WrapperCounter::WrapperCounter() : weak_factory_(this) {}
  WrapperCounter::~WrapperCounter() {}

  void WrapperCounter::BindInterface(RenderProcessHost* host,
    ReportCallback callback) {
    content::BindInterface(host, &wrapper_counter_);
    callback_ = std::move(callback);
  }

  void WrapperCounter::TryWrapperCounts() {
    wrapper_counter_->CountWrapperObjects();
    wrapper_counter_->Mummify(base::BindOnce(
      &WrapperCounter::OnCountWrappersComplete, weak_factory_.GetWeakPtr()));
  }

  void WrapperCounter::OnCountWrappersComplete(
      blink::mojom::WrapperCounterResultPtr result) {
    WrapperCounterReport report;
    report.none = result->number_of_remaining_wrappers <= 3;
    base::DictionaryValue detail;
    auto list = std::make_unique<base::ListValue>();
    list->AppendInteger(result->number_of_remaining_wrappers);
    detail.Set("numberOfRemainingWrappers", std::move(list));

    std::string detail_str;
    base::JSONWriter::Write(detail, &detail_str);
    report.detail = detail_str;

    std::move(callback_).Run(report);
  }
}
