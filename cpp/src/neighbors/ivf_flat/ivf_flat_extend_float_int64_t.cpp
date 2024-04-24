/*
 * Copyright (c) 2024, NVIDIA CORPORATION.
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
 * NOTE: this file is generated by generate_ivf_flat.py
 *
 * Make changes there and run in this directory:
 *
 * > python generate_ivf_flat.py
 *
 */

#include <cuvs/neighbors/ivf_flat.hpp>

namespace cuvs::neighbors::ivf_flat {

#define CUVS_INST_IVF_FLAT_EXTEND(T, IdxT)                                           \
  auto extend(raft::resources const& handle,                                         \
              raft::device_matrix_view<const T, IdxT, raft::row_major> new_vectors,  \
              std::optional<raft::device_vector_view<const IdxT, IdxT>> new_indices, \
              const cuvs::neighbors::ivf_flat::index<T, IdxT>& orig_index)           \
    ->cuvs::neighbors::ivf_flat::index<T, IdxT>                                      \
  {                                                                                  \
    return cuvs::neighbors::ivf_flat::index<T, IdxT>(                                \
      std::move(cuvs::neighbors::ivf_flat::extend(                          \
        handle, new_vectors, new_indices, orig_index)));           \
  }                                                                                  \
                                                                                     \
  void extend(raft::resources const& handle,                                         \
              raft::device_matrix_view<const T, IdxT, raft::row_major> new_vectors,  \
              std::optional<raft::device_vector_view<const IdxT, IdxT>> new_indices, \
              cuvs::neighbors::ivf_flat::index<T, IdxT>* idx)                        \
  {                                                                                  \
    cuvs::neighbors::ivf_flat::extend(                                      \
      handle, new_vectors, new_indices, idx);                      \
  }                 
CUVS_INST_IVF_FLAT_EXTEND(float, int64_t);

#undef CUVS_INST_IVF_FLAT_EXTEND

}  // namespace cuvs::neighbors::ivf_flat
