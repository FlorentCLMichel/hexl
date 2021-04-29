// Copyright (C) 2020-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"
#include "logging/logging.hpp"
#include "test-util.hpp"
#include "util/aligned-allocator.hpp"
#include "util/types.hpp"

namespace intel {
namespace hexl {

TEST(AlignedVector64, alloc) {
  AlignedVector64<uint64_t> x{1, 2, 3, 4};
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
}

TEST(AlignedVector64, assignment) {
  AlignedVector64<uint64_t> x{1, 2, 3, 4};
  AlignedVector64<uint64_t> y = x;
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(x, y);
}

TEST(AlignedVector64, move_assignment) {
  AlignedVector64<uint64_t> x{1, 2, 3, 4};
  AlignedVector64<uint64_t> y = std::move(x);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(y, (AlignedVector64<uint64_t>{1, 2, 3, 4}));
}

TEST(AlignedVector64, copy_constructor) {
  AlignedVector64<uint64_t> x{1, 2, 3, 4};
  AlignedVector64<uint64_t> y{x};
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(y, (AlignedVector64<uint64_t>{1, 2, 3, 4}));
}

TEST(AlignedVector64, move_constructor) {
  AlignedVector64<uint64_t> x{1, 2, 3, 4};
  AlignedVector64<uint64_t> y{std::move(x)};
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(y, (AlignedVector64<uint64_t>{1, 2, 3, 4}));
}

struct CustomAllocator {
  using T = int;
  T* invoke_allocation(int size) { return new T[size]; }

  void lets_deallocate(T* ptr) { delete[] ptr; }
};

struct CustomAllocatorAdapter
    : public AllocatorInterface<CustomAllocatorAdapter> {
  explicit CustomAllocatorAdapter(CustomAllocator&& a_) : a(std::move(a_)) {}

  // interface implementations
  void* allocate_impl(size_t bytes_count) {
    return a.invoke_allocation(bytes_count);
  }
  void deallocate_impl(void* p, size_t n) {
    (void)n;
    a.lets_deallocate(static_cast<CustomAllocator::T*>(p));
  }

  CustomAllocator a;
};

TEST(AlignedVectorCustomAllocator64, alloc) {
  std::shared_ptr<AllocatorBase> adapter_allocator;
  {
    CustomAllocator outter_allocator;
    adapter_allocator =
        std::make_shared<CustomAllocatorAdapter>(std::move(outter_allocator));
  }

  AlignedAllocator<uint64_t, 64> hexl_alloc(adapter_allocator);

  AlignedVector64<uint64_t> x({1, 2, 3, 4}, hexl_alloc);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
}

TEST(AlignedVectorCustomAllocator64, assignment) {
  std::shared_ptr<AllocatorBase> adapter_allocator;
  {
    CustomAllocator outter_allocator;
    adapter_allocator =
        std::make_shared<CustomAllocatorAdapter>(std::move(outter_allocator));
  }

  AlignedAllocator<uint64_t, 64> hexl_alloc(adapter_allocator);

  AlignedVector64<uint64_t> x({1, 2, 3, 4}, hexl_alloc);
  AlignedVector64<uint64_t> y = x;
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(x, y);
}

TEST(AlignedVectorCustomAllocator64, move_assignment) {
  std::shared_ptr<AllocatorBase> adapter_allocator;
  {
    CustomAllocator outter_allocator;
    adapter_allocator =
        std::make_shared<CustomAllocatorAdapter>(std::move(outter_allocator));
  }

  AlignedAllocator<uint64_t, 64> hexl_alloc(adapter_allocator);

  AlignedVector64<uint64_t> x({1, 2, 3, 4}, hexl_alloc);
  AlignedVector64<uint64_t> y = std::move(x);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(y, (AlignedVector64<uint64_t>({1, 2, 3, 4}, hexl_alloc)));
}

TEST(AlignedVectorCustomAllocator64, copy_constructor) {
  std::shared_ptr<AllocatorBase> adapter_allocator;
  {
    CustomAllocator outter_allocator;
    adapter_allocator =
        std::make_shared<CustomAllocatorAdapter>(std::move(outter_allocator));
  }

  AlignedAllocator<uint64_t, 64> hexl_alloc(adapter_allocator);
  AlignedVector64<uint64_t> x({1, 2, 3, 4}, hexl_alloc);
  AlignedVector64<uint64_t> y{x};
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(y, (AlignedVector64<uint64_t>({1, 2, 3, 4}, hexl_alloc)));
}

TEST(AlignedVectorCustomAllocator64, move_constructor) {
  std::shared_ptr<AllocatorBase> adapter_allocator;
  {
    CustomAllocator outter_allocator;
    adapter_allocator =
        std::make_shared<CustomAllocatorAdapter>(std::move(outter_allocator));
  }

  AlignedAllocator<uint64_t, 64> hexl_alloc(adapter_allocator);
  AlignedVector64<uint64_t> x({1, 2, 3, 4}, hexl_alloc);
  AlignedVector64<uint64_t> y{std::move(x)};
  ASSERT_EQ(reinterpret_cast<uintptr_t>(x.data()) % 64, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(y.data()) % 64, 0);
  ASSERT_EQ(y, (AlignedVector64<uint64_t>({1, 2, 3, 4}, hexl_alloc)));
}
}  // namespace hexl
}  // namespace intel
