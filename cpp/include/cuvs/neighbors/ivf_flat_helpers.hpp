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

#pragma once

#include <cuvs/neighbors/ivf_flat.hpp>
#include <raft/core/device_mdspan.hpp>
#include <raft/core/resources.hpp>

namespace cuvs::neighbors::ivf_flat {
namespace codepacker {

/**
 * @defgroup ivf_flat_codepacker Helper functions for IVF Flat codebook
 * @{
 */

/**
 * Write flat codes into an existing list by the given offset.
 *
 * NB: no memory allocation happens here; the list must fit the data (offset + n_vec).
 *
 * Usage example:
 * @code{.cpp}
 *   auto list_data  = index.lists()[label]->data.view();
 *   // allocate the buffer for the input codes
 *   auto codes = raft::make_device_matrix<T>(res, n_vec, index.dim());
 *   ... prepare n_vecs to pack into the list in codes ...
 *   // write codes into the list starting from the 42nd position
 *   ivf_pq::helpers::codepacker::pack(
 *       res, make_const_mdspan(codes.view()), index.veclen(), 42, list_data);
 * @endcode
 *
 * @tparam T
 * @tparam IdxT
 *
 * @param[in] res
 * @param[in] codes flat codes [n_vec, dim]
 * @param[in] veclen size of interleaved data chunks
 * @param[in] offset how many records to skip before writing the data into the list
 * @param[inout] list_data block to write into
 */
template <typename T, typename IdxT>
void pack(
  raft::resources const& res,
  raft::device_matrix_view<const T, uint32_t, raft::row_major> codes,
  uint32_t veclen,
  uint32_t offset,
  raft::device_mdspan<T, typename list_spec<uint32_t, T, IdxT>::list_extents, raft::row_major>
    list_data);

/**
 * @brief Unpack `n_take` consecutive records of a single list (cluster) in the compressed index
 * starting at given `offset`.
 *
 * Usage example:
 * @code{.cpp}
 *   auto list_data = index.lists()[label]->data.view();
 *   // allocate the buffer for the output
 *   uint32_t n_take = 4;
 *   auto codes = raft::make_device_matrix<T>(res, n_take, index.dim());
 *   uint32_t offset = 0;
 *   // unpack n_take elements from the list
 *   ivf_pq::helpers::codepacker::unpack(res, list_data, index.veclen(), offset, codes.view());
 * @endcode
 *
 * @tparam T
 * @tparam IdxT
 *
 * @param[in] res raft resource
 * @param[in] list_data block to read from
 * @param[in] veclen size of interleaved data chunks
 * @param[in] offset
 *   How many records in the list to skip.
 * @param[inout] codes
 *   the destination buffer [n_take, index.dim()].
 *   The length `n_take` defines how many records to unpack,
 *   it must be <= the list size.
 */
template <typename T, typename IdxT>
void unpack(
  raft::resources const& res,
  raft::device_mdspan<const T, typename list_spec<uint32_t, T, IdxT>::list_extents, raft::row_major>
    list_data,
  uint32_t veclen,
  uint32_t offset,
  raft::device_matrix_view<T, uint32_t, raft::row_major> codes);

/**
 * Write one flat code into a block by the given offset. The offset indicates the id of the record
 * in the list. This function interleaves the code and is intended to later copy the interleaved
 * codes over to the IVF list on device. NB: no memory allocation happens here; the block must fit
 * the record (offset + 1).
 *
 * @tparam T
 *
 * @param[in] flat_code input flat code
 * @param[out] block block of memory to write interleaved codes to
 * @param[in] dim dimension of the flat code
 * @param[in] veclen size of interleaved data chunks
 * @param[in] offset how many records to skip before writing the data into the list
 */
template <typename T>
void pack_1(const T* flat_code, T* block, uint32_t dim, uint32_t veclen, uint32_t offset);

/**
 * Unpack 1 record of a single list (cluster) in the index to fetch the flat code. The offset
 * indicates the id of the record. This function fetches one flat code from an interleaved code.
 *
 * @tparam T
 *
 * @param[in] block interleaved block. The block can be thought of as the whole inverted list in
 * interleaved format.
 * @param[out] flat_code output flat code
 * @param[in] dim dimension of the flat code
 * @param[in] veclen size of interleaved data chunks
 * @param[in] offset fetch the flat code by the given offset
 */
template <typename T>
void unpack_1(const T* block, T* flat_code, uint32_t dim, uint32_t veclen, uint32_t offset);

/** @} */

}  // namespace codepacker

namespace helpers {

/**
 * @defgroup ivf_flat_helpers Helper functions for manipulationg IVF Flat Index
 * @{
 */

/**
 * @brief Public helper API to reset the data and indices ptrs, and the list sizes. Useful for
 * externally modifying the index without going through the build stage. The data and indices of the
 * IVF lists will be lost.
 *
 * Usage example:
 * @code{.cpp}
 *   raft::resources res;
 *   using namespace raft::neighbors;
 *   // use default index parameters
 *   ivf_flat::index_params index_params;
 *   // initialize an empty index
 *   ivf_flat::index<int64_t> index(res, index_params, D);
 *   // reset the index's state and list sizes
 *   ivf_flat::helpers::reset_index(res, &index);
 * @endcode
 *
 * @tparam IdxT
 *
 * @param[in] res raft resource
 * @param[inout] index pointer to IVF-PQ index
 */
template <typename T, typename IdxT>
void reset_index(const raft::resources& res, index<T, IdxT>* index);
/** @} */

}  // namespace helpers
}  // namespace cuvs::neighbors::ivf_flat