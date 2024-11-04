/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021-2024, Hewlett Packard Enterprise
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

#include "../../../third-party/catch/single_include/catch2/catch.hpp"
#include "client.h"
#include "dataset.h"
#include "logger.h"
#include "../client_test_utils.h"

unsigned long get_time_offset();

using namespace SmartRedis;

// helper function for resetting environment
// variables to their original state
void reset_env_vars(const char* old_keyin, const char* old_keyout);

SCENARIO("Testing Client prefixing")
{
    std::cout << std::to_string(get_time_offset()) << ": Testing Client prefixing" << std::endl;
    std::string context("test_client_prefixing");
    log_data(context, LLDebug, "***Beginning Client Prefixing testing***");

    GIVEN("Variables that will be used by the producer and consumer")
    {
        const char* old_keyin = std::getenv("SSKEYIN");
        const char* old_keyout = std::getenv("SSKEYOUT");
        char keyin_env_put[] = "prefix_test";
        char keyout_env_put[] = "prefix_test";
        char keyin_env_get[] = "prefix_test";
        char keyout_env_get[] = "prefix_test";
        size_t dim1 = 10;
        std::vector<size_t> dims = {dim1};
        std::string prefix = "prefix_test";
        std::string dataset_tensor_key = "dataset_tensor";
        std::string dataset_key = "test_dataset";
        std::string tensor_key = "test_tensor";
        std::string bytes_key = "test_bytes";

        SRTensorType g_type;
        std::vector<size_t> g_dims;
        void* g_result;

        THEN("Client prefixing can be used for tensors")
        {
            // Set env vars to enable key prefixing
            setenv("SSKEYIN", keyin_env_put, (old_keyin != NULL));
            setenv("SSKEYOUT", keyout_env_put, (old_keyout != NULL));

            // Initialize a client
            Client client(context);

            // Enable prefixing for tensors
            client.use_tensor_ensemble_prefix(true);

            // Create tensor data
            float* array = (float*)malloc(dims[0]*sizeof(float));
            for(int i=0; i<dims[0]; i++)
                array[i] = (float)(rand()/((float)RAND_MAX/10.0));

            // Put tensor into database
            client.put_tensor(tensor_key, (void*)array,
                              dims, SRTensorTypeFloat,
                              SRMemLayoutNested);

            // Check that the tensor exists
            CHECK(client.tensor_exists(tensor_key));

            // Check that the tensor key exists
            CHECK(client.key_exists(prefix + "." + tensor_key));

            // Reset environment variables to their original state
            reset_env_vars(old_keyin, old_keyout);

            // Free test memory
            free(array);
        }
        THEN("Client prefixing can be used for datasets")
        {
            // Set env vars to enable key prefixing
            setenv("SSKEYIN", keyin_env_put, (old_keyin != NULL));
            setenv("SSKEYOUT", keyout_env_put, (old_keyout != NULL));

            // Initialize a client
            Client client(context);

            // Enable prefixing for datasets
            client.use_dataset_ensemble_prefix(true);

            // Create a dataset with a tensor
            float* array = (float*)malloc(dims[0]*sizeof(float));
            for(int i=0; i<dims[0]; i++)
                array[i] = (float)(rand()/((float)RAND_MAX/10.0));
            DataSet dataset(dataset_key);
            dataset.add_tensor(dataset_tensor_key, (void*)array, dims,
                               SRTensorTypeFloat, SRMemLayoutNested);

            // Put dataset into the database
            client.put_dataset(dataset);

            // Check that the dataset exists
            CHECK(client.dataset_exists(dataset_key));

            // Check that the dataset key exists
            CHECK(client.key_exists(prefix + ".{" + dataset_key + "}.meta"));

            // Reset environment variables to their original state
            reset_env_vars(old_keyin, old_keyout);

            // Free test memory
            free(array);
        }
        THEN("Client prefixing can be used for bytes")
        {
            // Set env vars to enable key prefixing
            setenv("SSKEYIN", keyin_env_put, (old_keyin != NULL));
            setenv("SSKEYOUT", keyout_env_put, (old_keyout != NULL));

            // Initialize a client
            Client client(context);

            // Enable prefixing for datasets
            client.use_bytes_ensemble_prefix(true);

            // Create a bytes array and put into the database
            const size_t n_bytes = 5;
            unsigned char bytes_value[n_bytes] = {0x00, 0x65, 0x6C, 0x6C, 0x6F};

            // Put bytes into the database
            client.put_bytes(bytes_key, (void*)bytes_value, n_bytes);

            // Check that the bytes exists
            CHECK(client.bytes_exists(bytes_key));

            // Check that the bytes key exists
            CHECK(client.key_exists(prefix + "." + bytes_key));

            // Reset environment variables to their original state
            reset_env_vars(old_keyin, old_keyout);

        }

    }
    log_data(context, LLDebug, "***End Client Prefixing testing***");
}