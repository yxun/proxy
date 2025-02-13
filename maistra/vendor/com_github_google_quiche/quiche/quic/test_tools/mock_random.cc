// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "quiche/quic/test_tools/mock_random.h"

#include <string.h>

namespace quic {
namespace test {

MockRandom::MockRandom() : base_(0xDEADBEEF), increment_(0) {}

MockRandom::MockRandom(uint32_t base) : base_(base), increment_(0) {}

void MockRandom::RandBytes(void* data, size_t len) {
  memset(data, increment_ + static_cast<uint8_t>('r'), len);
}

uint64_t MockRandom::RandUint64() {
  return base_ + increment_;
}

void MockRandom::InsecureRandBytes(void* data, size_t len) {
  RandBytes(data, len);
}

uint64_t MockRandom::InsecureRandUint64() {
  return RandUint64();
}

void MockRandom::ChangeValue() {
  increment_++;
}

void MockRandom::ResetBase(uint32_t base) {
  base_ = base;
  increment_ = 0;
}

}  // namespace test
}  // namespace quic
