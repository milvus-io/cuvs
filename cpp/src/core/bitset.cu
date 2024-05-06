/*
 * Copyright (c) 2023-2024, NVIDIA CORPORATION.
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

#include "bitset.cuh"

namespace cuvs::core {

template struct bitset_view<uint8_t, uint32_t>;
template struct bitset_view<uint16_t, uint32_t>;
template struct bitset_view<uint32_t, uint32_t>;
template struct bitset_view<uint32_t, int64_t>;
template struct bitset_view<uint64_t, int64_t>;
template struct bitset<uint8_t, uint32_t>;
template struct bitset<uint16_t, uint32_t>;
template struct bitset<uint32_t, uint32_t>;
template struct bitset<uint32_t, int64_t>;
template struct bitset<uint64_t, int64_t>;
}  // namespace cuvs::core