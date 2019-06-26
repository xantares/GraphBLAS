/*
 * Copyright (c) 2019, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include <rmm/device_buffer.hpp>
#include <rmm/mr/cnmem_memory_resource.hpp>
#include <rmm/mr/cuda_memory_resource.hpp>
#include <rmm/mr/default_memory_resource.hpp>
#include <rmm/mr/device_memory_resource.hpp>
#include <rmm/mr/managed_memory_resource.hpp>

#include <cuda_runtime_api.h>
#include <cstddef>
#include <random>

void sync_stream(cudaStream_t stream) {
  EXPECT_EQ(cudaSuccess, cudaStreamSynchronize(stream));
}

template <typename MemoryResourceType>
struct DeviceBufferTest : public ::testing::Test {
  cudaStream_t stream{};
  std::size_t size{};
  MemoryResourceType mr{};

  DeviceBufferTest() {
    std::default_random_engine generator;
    std::uniform_int_distribution<std::size_t> distribution(1000, 100000);
    size = distribution(generator);
  }

  void SetUp() override { EXPECT_EQ(cudaSuccess, cudaStreamCreate(&stream)); }

  void TearDown() override {
    EXPECT_EQ(cudaSuccess, cudaStreamDestroy(stream));
  };
};

using resources = ::testing::Types<rmm::mr::cuda_memory_resource,
                                   rmm::mr::managed_memory_resource,
                                   rmm::mr::cnmem_memory_resource>;

TYPED_TEST_CASE(DeviceBufferTest, resources);

TYPED_TEST(DeviceBufferTest, DefaultMemoryResource) {
  rmm::device_buffer buff(this->size);
  EXPECT_NE(nullptr, buff.data());
  EXPECT_EQ(this->size, buff.size());
  EXPECT_EQ(this->size, buff.capacity());
  EXPECT_EQ(rmm::mr::get_default_resource(), buff.memory_resource());
  EXPECT_EQ(0, buff.stream());
}

TYPED_TEST(DeviceBufferTest, DefaultMemoryResourceStream) {
  rmm::device_buffer buff(this->size, this->stream);
  sync_stream(this->stream);
  EXPECT_NE(nullptr, buff.data());
  EXPECT_EQ(this->size, buff.size());
  EXPECT_EQ(this->size, buff.capacity());
  EXPECT_EQ(rmm::mr::get_default_resource(), buff.memory_resource());
  EXPECT_EQ(this->stream, buff.stream());
}

TYPED_TEST(DeviceBufferTest, ExplicitMemoryResource) {
  rmm::device_buffer buff(this->size, 0, &this->mr);
  EXPECT_NE(nullptr, buff.data());
  EXPECT_EQ(this->size, buff.size());
  EXPECT_EQ(this->size, buff.capacity());
  EXPECT_EQ(&this->mr, buff.memory_resource());
  EXPECT_TRUE(this->mr.is_equal(*buff.memory_resource()));
  EXPECT_EQ(0, buff.stream());
}

TYPED_TEST(DeviceBufferTest, ExplicitMemoryResourceStream) {
  rmm::device_buffer buff(this->size, this->stream, &this->mr);
  sync_stream(this->stream);
  EXPECT_NE(nullptr, buff.data());
  EXPECT_EQ(this->size, buff.size());
  EXPECT_EQ(this->size, buff.capacity());
  EXPECT_EQ(&this->mr, buff.memory_resource());
  EXPECT_TRUE(this->mr.is_equal(*buff.memory_resource()));
  EXPECT_EQ(this->stream, buff.stream());
}

TYPED_TEST(DeviceBufferTest, CopyConstructor) {
  rmm::device_buffer buff(this->size, 0, &this->mr);
  // Can't do this until RMM cmake is setup to build cuda files
  // thrust::sequence(thrust::device, static_cast<signed char *>(buff.data()),
  //                 static_cast<signed char *>(buffer.data()) + buff.size(),
  //                 0);
  rmm::device_buffer buff_copy(buff);
  EXPECT_NE(nullptr, buff_copy.data());
  EXPECT_NE(buff.data(), buff_copy.data());
  EXPECT_EQ(buff.size(), buff_copy.size());
  EXPECT_EQ(buff.capacity(), buff_copy.capacity());
  EXPECT_EQ(buff.memory_resource(), buff_copy.memory_resource());
  EXPECT_TRUE(buff.memory_resource()->is_equal(*buff_copy.memory_resource()));
  EXPECT_EQ(buff.stream(), buff_copy.stream());

  // EXPECT_TRUE(
  //    thrust::equal(thrust::device, static_cast<signed char *>(buff.data()),
  //                  static_cast<signed char *>(buff.data()) + buff.size(),
  //                  static_cast<signed char *>(buff_copy.data())));
}

TYPED_TEST(DeviceBufferTest, CopyConstructorStream) {
  rmm::device_buffer buff(this->size, this->stream, &this->mr);
  sync_stream(this->stream);
  // Can't do this until RMM cmake is setup to build cuda files
  // thrust::sequence(thrust::device, static_cast<signed char *>(buff.data()),
  //                 static_cast<signed char *>(buffer.data()) + buff.size(),
  //                 0);
  rmm::device_buffer buff_copy(buff);
  sync_stream(this->stream);
  EXPECT_NE(nullptr, buff_copy.data());
  EXPECT_NE(buff.data(), buff_copy.data());
  EXPECT_EQ(buff.size(), buff_copy.size());
  EXPECT_EQ(buff.capacity(), buff_copy.capacity());
  EXPECT_EQ(buff.stream(), buff_copy.stream());
  EXPECT_EQ(buff.memory_resource(), buff_copy.memory_resource());
  EXPECT_TRUE(buff.memory_resource()->is_equal(*buff_copy.memory_resource()));
  EXPECT_EQ(buff.stream(), buff_copy.stream());

  // EXPECT_TRUE(
  //    thrust::equal(thrust::device, static_cast<signed char *>(buff.data()),
  //                  static_cast<signed char *>(buff.data()) + buff.size(),
  //                  static_cast<signed char *>(buff_copy.data())));
}

TYPED_TEST(DeviceBufferTest, MoveConstructor) {
  rmm::device_buffer buff(this->size, 0, &this->mr);
  auto p = buff.data();
  auto size = buff.size();
  auto capacity = buff.capacity();
  auto mr = buff.memory_resource();
  auto stream = buff.stream();

  // New buffer should have the same contents as the original
  rmm::device_buffer buff_new(std::move(buff));
  EXPECT_NE(nullptr, buff_new.data());
  EXPECT_EQ(p, buff_new.data());
  EXPECT_EQ(size, buff_new.size());
  EXPECT_EQ(capacity, buff_new.capacity());
  EXPECT_EQ(stream, buff_new.stream());
  EXPECT_EQ(mr, buff_new.memory_resource());

  // Original buffer should be empty
  EXPECT_EQ(nullptr, buff.data());
  EXPECT_EQ(0, buff.size());
  EXPECT_EQ(0, buff.capacity());
  EXPECT_EQ(0, buff.stream());
  EXPECT_NE(nullptr, buff.memory_resource());
}

TYPED_TEST(DeviceBufferTest, MoveConstructorStream) {
  rmm::device_buffer buff(this->size, this->stream, &this->mr);
  sync_stream(this->stream);
  auto p = buff.data();
  auto size = buff.size();
  auto capacity = buff.capacity();
  auto mr = buff.memory_resource();
  auto stream = buff.stream();

  // New buffer should have the same contents as the original
  rmm::device_buffer buff_new(std::move(buff));
  sync_stream(this->stream);
  EXPECT_NE(nullptr, buff_new.data());
  EXPECT_EQ(p, buff_new.data());
  EXPECT_EQ(size, buff_new.size());
  EXPECT_EQ(capacity, buff_new.capacity());
  EXPECT_EQ(stream, buff_new.stream());
  EXPECT_EQ(mr, buff_new.memory_resource());

  // Original buffer should be empty
  EXPECT_EQ(nullptr, buff.data());
  EXPECT_EQ(0, buff.size());
  EXPECT_EQ(0, buff.capacity());
  EXPECT_EQ(0, buff.stream());
  EXPECT_NE(nullptr, buff.memory_resource());
}

TYPED_TEST(DeviceBufferTest, ResizeSmaller) {
  rmm::device_buffer buff(this->size, 0, &this->mr);
  auto old_data = buff.data();
  auto new_size = this->size - 1;
  buff.resize(new_size);
  EXPECT_EQ(new_size, buff.size());
  EXPECT_EQ(this->size, buff.capacity());  // Capacity should be unchanged
  // Resizing smaller means the existing allocation should remain unchanged
  EXPECT_EQ(old_data, buff.data());
}

TYPED_TEST(DeviceBufferTest, ResizeBigger) {
  rmm::device_buffer buff(this->size, 0, &this->mr);
  auto old_data = buff.data();
  auto new_size = this->size + 1;
  buff.resize(new_size);
  EXPECT_EQ(new_size, buff.size());
  EXPECT_EQ(new_size, buff.capacity());
  // Resizing bigger means the data should point to a new allocation
  EXPECT_NE(old_data, buff.data());
}