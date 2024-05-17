/*
 * Copyright (c) 2021-2023, NVIDIA CORPORATION.
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

/*
 * NOTE: this file is generated by dispatch_00_generate.py
 *
 * Make changes there and run in this directory:
 *
 * > python dispatch_00_generate.py
 *
 */

#include "../distance_ops/all_ops.cuh"  // ops::*
#include "dispatch-inl.cuh"             // dispatch
#include "dispatch_sm60.cuh"
#include <raft/core/operators.hpp>  // raft::identity_op
#define instantiate_raft_distance_detail_pairwise_matrix_dispatch(                     \
  OpT, DataT, AccT, OutT, FinOpT, IdxT)                                                \
  template void cuvs::distance::detail::                                               \
    pairwise_matrix_dispatch<OpT<DataT, AccT, IdxT>, DataT, AccT, OutT, FinOpT, IdxT>( \
      OpT<DataT, AccT, IdxT> distance_op,                                              \
      IdxT m,                                                                          \
      IdxT n,                                                                          \
      IdxT k,                                                                          \
      const DataT* x,                                                                  \
      const DataT* y,                                                                  \
      const DataT* x_norm,                                                             \
      const DataT* y_norm,                                                             \
      OutT* out,                                                                       \
      FinOpT fin_op,                                                                   \
      cudaStream_t stream,                                                             \
      bool is_row_major)

instantiate_raft_distance_detail_pairwise_matrix_dispatch(
  cuvs::distance::detail::ops::russel_rao_distance_op,
  double,
  double,
  double,
  raft::identity_op,
  int);

#undef instantiate_raft_distance_detail_pairwise_matrix_dispatch
