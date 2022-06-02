/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021-2022, Hewlett Packard Enterprise
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "c_client.h"
#include "c_dataset.h"
#include "c_client_test_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include "srexception.h"

bool cluster = true;

int append_dataset(void* client, char* list_name, int list_name_length, char* ds_name, int ds_name_length)
{
  void *dataset = NULL;
  char *t1 = "tensor_1";
  const size_t n_dims = 1;
  size_t dims[n_dims];
  uint16_t ***tensor = NULL;
  int i;
  bool exists = false;

  // Initialize dataset
  if (SRNoError != CDataSet(ds_name, ds_name_length, &dataset) || NULL == dataset)
    return -1;

  // Allocate memory for tensor
  dims[0] = 10;
  tensor = (uint16_t *)malloc(dims[0] * sizeof(uint16_t));

  // Add tensors to the DataSet
  for (i = 0; i < dims[0]; i++) {
    tensor[i] = 1 << i;
  }
  if (SRNoError != add_tensor(dataset, t1, strlen(t1), tensor, dims, n_dims, SRTensorTypeInt16, SRMemLayoutNested))
    return -1;

  // Put the DataSet into the database
  if (SRNoError != put_dataset(client, dataset))
    return -1;

  // Add it to the list
  if (SRNoError != append_to_list(client, list_name, list_name_length, dataset))
    return -1;

  return 0;
}

int count_datasets(void* client, char* list_name, int list_name_length, int expected_count)
{
  int list_length = 0;
  if (SRNoError != get_list_length(client, list_name, list_name_length, &list_length)) {
    printf("Unable to get list length!\n");
    return -1;
  }
  if (list_length != expected_count) {
    printf("Got wrong number of entries in list!\n");
    return -1;
  }
  return 0;
}

int check_dataset(void* client, void* dataset)
{
  return 0;
}

int main(int argc, char* argv[])
{
  int i;
  int result = 0;
  int ndatasets = 4;
  char* dbl_suffix = "_dbl_c";
  char* dataset_name[] = {"agg_dataset_0", "agg_dataset_1", "agg_dataset_2",  "agg_dataset_2"};
  char* list_name = "my_aggregation";
  void* datasets = NULL;
  size_t num_datasets = 0;

  // Initialize client
  void *client = NULL;
  if (SRNoError != SmartRedisCClient(use_cluster(), &client) || NULL == client) {
    printf("Failed to initialize client!\n");
    printf("Test passed: NO\n");
    return -1;
  }

  // Make sure the list is cleared
  (void)delete_list(client, list_name, strlen(list_name));

  // test dataset append
  for (i = 0; i < ndatasets; i++) {
    result += append_dataset(
      client, list_name, strlen(list_name), dataset_name[i], strlen(dataset_name[i]));
  }

  // test dataset count
  result += count_datasets(client, list_name, strlen(list_name), ndatasets);

  // Retrieve datasets via the aggregation list
  if (SRNoError != get_datasets_from_list(
    client, list_name, strlen(list_name), &datasets, &num_datasets)) {
      printf("Retrieval of datasets failed!\n");
      return -1;
  }
  if (num_datasets != (size_t)ndatasets) {
    printf("Retrieval of datasets got the wrong number of datasets!\n");
    return -1;
  }

  // Check datasets
  for (i = 0; i < ndatasets; i++) {
    result += check_dataset(client, &datasets[i]);
  }

  // Done
  printf("Test passed: %s\n", result == 0 ? "YES" : "NO");
  return result;
}
