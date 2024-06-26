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

#include <cuda.h>
#include <raft/core/device_mdarray.hpp>
#include <raft/core/handle.hpp>
#include <raft/random/rng.cuh>

#include "ann_utils.cuh"
#include <cuvs/neighbors/brute_force.h>

extern "C" void run_brute_force(int64_t n_rows,
                                int64_t n_queries,
                                int64_t n_dim,
                                uint32_t n_neighbors,
                                float* index_data,
                                float* query_data,
                                float* distances_data,
                                int64_t* neighbors_data,
                                cuvsDistanceType metric);

template <typename T>
void generate_random_data(T* devPtr, size_t size)
{
  raft::handle_t handle;
  raft::random::RngState r(1234ULL);
  raft::random::uniform(handle, r, devPtr, size, T(0.1), T(2.0));
};

template <typename T, typename IdxT>
void recall_eval(T* query_data,
                 T* index_data,
                 IdxT* neighbors,
                 T* distances,
                 size_t n_queries,
                 size_t n_rows,
                 size_t n_dim,
                 size_t n_neighbors,
                 cuvsDistanceType metric)
{
  raft::handle_t handle;
  auto distances_ref = raft::make_device_matrix<T, IdxT>(handle, n_queries, n_neighbors);
  auto neighbors_ref = raft::make_device_matrix<IdxT, IdxT>(handle, n_queries, n_neighbors);
  cuvs::neighbors::naive_knn<T, T, IdxT>(
    handle,
    distances_ref.data_handle(),
    neighbors_ref.data_handle(),
    query_data,
    index_data,
    n_queries,
    n_rows,
    n_dim,
    n_neighbors,
    static_cast<cuvs::distance::DistanceType>((uint16_t)metric));

  size_t size = n_queries * n_neighbors;
  std::vector<IdxT> neighbors_h(size);
  std::vector<T> distances_h(size);
  std::vector<IdxT> neighbors_ref_h(size);
  std::vector<T> distances_ref_h(size);

  auto stream = raft::resource::get_cuda_stream(handle);
  raft::copy(neighbors_h.data(), neighbors, size, stream);
  raft::copy(distances_h.data(), distances, size, stream);
  raft::copy(neighbors_ref_h.data(), neighbors_ref.data_handle(), size, stream);
  raft::copy(distances_ref_h.data(), distances_ref.data_handle(), size, stream);

  // verify output
  double min_recall = 0.95;
  ASSERT_TRUE(cuvs::neighbors::eval_neighbours(neighbors_ref_h,
                                               neighbors_h,
                                               distances_ref_h,
                                               distances_h,
                                               n_queries,
                                               n_neighbors,
                                               0.001,
                                               min_recall));
};

TEST(BruteForceC, BuildSearch)
{
  int64_t n_rows       = 8096;
  int64_t n_queries    = 128;
  int64_t n_dim        = 32;
  uint32_t n_neighbors = 8;

  cuvsDistanceType metric = L2Expanded;

  float *index_data, *query_data, *distances_data;
  int64_t* neighbors_data;
  cudaMalloc(&index_data, sizeof(float) * n_rows * n_dim);
  cudaMalloc(&query_data, sizeof(float) * n_queries * n_dim);
  cudaMalloc(&neighbors_data, sizeof(int64_t) * n_queries * n_neighbors);
  cudaMalloc(&distances_data, sizeof(float) * n_queries * n_neighbors);

  generate_random_data(index_data, n_rows * n_dim);
  generate_random_data(query_data, n_queries * n_dim);

  run_brute_force(n_rows,
                  n_queries,
                  n_dim,
                  n_neighbors,
                  index_data,
                  query_data,
                  distances_data,
                  neighbors_data,
                  metric);

  recall_eval(query_data,
              index_data,
              neighbors_data,
              distances_data,
              n_queries,
              n_rows,
              n_dim,
              n_neighbors,
              metric);

  // delete device memory
  cudaFree(index_data);
  cudaFree(query_data);
  cudaFree(neighbors_data);
  cudaFree(distances_data);
}
