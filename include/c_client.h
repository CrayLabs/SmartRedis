/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021, Hewlett Packard Enterprise
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

#ifndef SMARTREDIS_C_CLIENT_H
#define SMARTREDIS_C_CLIENT_H
///@file
///\brief C-wrappers for the C++ Client class
#include <stdlib.h>
#include <stdbool.h>
#include "client.h"
#include "sr_enums.h"
#include "srexception.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
*   \brief C-client constructor
*   \param cluster Flag to indicate if a database cluster is being used
*   \param new_client Receives the new client
*   \return Returns SRNoError on success or an error code on failure
*/
SRError SmartRedisCClient(bool cluster, void **new_client);

/*!
*   \brief C-client destructor
*   \param c_client A pointer to a pointer to the c-client to destroy.
*                   The client is set to NULL on completion
*   \return Returns SRNoError on success or an error code on failure
*/
SRError DeleteCClient(void** c_client);

/*!
*   \brief Put a DataSet object into the database
*   \param c_client The client object to use for communication
*   \param dataset The DataSet object to send
*   \return Returns SRNoError on success or an error code on failure
*/
SRError put_dataset(void* c_client, void* dataset);

/*!
*   \brief Get a DataSet object from the database
*   \param c_client The client object to use for communication
*   \param name The name key of the dataset object to fetch
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param dataset Receives the DataSet
*   \return Returns SRNoError on success or an error code on failure
*/
SRError get_dataset(void* c_client,
                    const char* name,
                    const size_t name_length,
                    void** dataset);

/*!
*   \brief Move a DataSet to a new name key
*   \param c_client The client object to use for communication
*   \param name The current name key of the dataset object
*   \param name_length The length of the current name string,
*                      excluding null terminating character
*   \param new_name The new name key for the dataset object
*   \param new_name_length The length of the new name string,
*                          excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError rename_dataset(void* c_client,
                       const char* name,
                       const size_t name_length,
                       const char* new_name,
                       const size_t new_name_length);

/*!
*   \brief Copy a DataSet to a new name key
*   \param c_client The client object to use for communication
*   \param src_name The source name key of the dataset object
*   \param src_name_length The length of the src_name string,
*                          excluding null terminating character
*   \param dest_name The destination name key for the dataset object
*   \param dest_name_length The length of the dest_name string,
*                           excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError copy_dataset(void* c_client,
                     const char* src_name,
                     const size_t src_name_length,
                     const char* dest_name,
                     const size_t dest_name_length);

/*!
*   \brief Delete a DataSet
*   \param c_client The client object to use for communication
*   \param name The name key of the dataset object
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError delete_dataset(void* c_client,
                       const char* name,
                       const size_t name_length);

/*!
*   \brief Put a tensor into the database
*   \param c_client The client object to use for communication
*   \param key The key by which the tensor should be accessed
*   \param key_length The length of the tensor key string,
*                     excluding null terminating character
*   \param data The data to store with the tensor
*   \param dims The number of elements for each dimension of the tensor
*   \param n_dims The number of dimensions of the tensor
*   \param type The data type of the tensor
*   \param mem_layout The memory layout of the data
*   \return Returns SRNoError on success or an error code on failure
*/
SRError put_tensor(void* c_client,
                   const char* key,
                   const size_t key_length,
                   void* data,
                   const size_t* dims,
                   const size_t n_dims,
                   SRTensorType type,
                   SRMemoryLayout mem_layout);

/*!
*   \brief Get the data, dimensions, and type for a tensor in the
*          database. This function will allocate and retain management of the
*          memory for the tensor data. The number of dimensions and
*          the tensor type will be set based on the tensor retrieved from the
*          database.  The requested memory layout will be used to shape the
*          returned memory space pointed to by the data pointer.
*   \details The memory returned in data is valid until the dataset is
*            destroyed. This method is meant to be used when the dimensions
*            and type of the tensor are unknown or the user does not want to
*            manage memory.  However, given that the memory associated with the
*            return data is valid until dataset destruction, this method should
*            not be used repeatedly for large tensor data.  Instead  it is
*            recommended that the user use unpack_tensor() for large tensor
*            data and to limit memory use by the dataset.
*   \param c_client The client object to use for communication
*   \param key The key from which the tensor should be accessed
*   \param key_length The length of the tensor key string,
*                     excluding null terminating character
*   \param data Receives tensor data in newly allocated memory
*   \param dims Receives the number of elements in each dimension of the
*               tensor in newly allocated memory
*   \param n_dims Receives the number of dimensions for the tensor
*   \param type Receives the data type for the tensor as retrieved from
*               the database
*   \param mem_layout The layout requested for the allocated memory space
*   \return Returns SRNoError on success or an error code on failure
*/
SRError get_tensor(void* c_client,
                   const char* key,
                   const size_t key_length,
                   void** data,
                   size_t** dims,
                   size_t* n_dims,
                   SRTensorType* type,
                   SRMemoryLayout mem_layout);
/*!
*   \brief Retrieve a tensor from the database into memory provided
*          by the caller
*   \param c_client The client object to use for communication
*   \param key The key from which the tensor should be accessed
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param result The data buffer into which the tensor data should
*                 be written
*   \param dims The number of elements in each dimension of the
*               provided memory space
*   \param n_dims The number of dimensions in the provided memory space
*   \param type The data type for the provided memory space.
*   \param mem_layout The memory layout for the provided memory space.
*   \return Returns SRNoError on success or an error code on failure
*/
SRError unpack_tensor(void* c_client,
                     const char* key,
                     const size_t key_length,
                     void* result,
                     const size_t* dims,
                     const size_t n_dims,
                     SRTensorType type,
                     SRMemoryLayout mem_layout);

/*!
*   \brief Move a tensor to a new tensor key
*   \param c_client The client object to use for communication
*   \param key The original key from which the tensor should be accessed
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param new_key The new tensor key
*   \param new_key_length The length of the new_key string,
                          excluding null terminating characters
*   \return Returns SRNoError on success or an error code on failure
*/
SRError rename_tensor(void* c_client,
                      const char* key,
                      const size_t key_length,
                      const char* new_key,
                      const size_t new_key_length);

/*!
*   \brief Delete a tensor. This also removes the associated tensor key
*   \param c_client The client object to use for communication
*   \param key The tensor key for the tensor to delete
*   \param key_length The length of the tensor key string,
*                     excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError delete_tensor(void* c_client,
                      const char* key,
                      const size_t key_length);

/*!
*   \brief Copy a tensor to a destination tensor key
*   \param c_client The client object to use for communication
*   \param src_name The source key from which the tensor should be copied
*   \param src_name_length The length of the src_name string,
*                          excluding null terminating character
*   \param dest_name The destination key to which the tensor should be copied
*   \param dest_name_length The length of the dest_name string,
*                           excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError copy_tensor(void* c_client,
                    const char* src_name,
                    const size_t src_name_length,
                    const char* dest_name,
                    const size_t dest_name_length);

/*!
*   \brief Set a model (from file) in the database for future execution
*   \param c_client The client object to use for communication
*   \param key The key to associate with the model
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param model_file The source file for the model
*   \param model_file_length The length of the model_file string,
*                            excluding null terminating character
*   \param backend The name of the backend (TF, TFLITE, TORCH, ONNX)
*   \param backend_length The length of the backend string,
*                        excluding null terminating character
*   \param device The name of the device for execution (e.g. CPU or GPU)
*   \param device_length The length of the device string,
*                        excluding null terminating character
*   \param batch_size The batch size for model execution
*   \param min_batch_size The minimum batch size for model execution
*   \param tag A tag to attach to the model for information purposes
*   \param tag_length The length of the tag string,
*                     excluding null terminating character
*   \param inputs One or more names of model input nodes (TF models only)
*   \param input_lengths The length of each input name string,
*                        excluding null terminating character
*   \param n_inputs The number of inputs
*   \param outputs One or more names of model output nodes (TF models only)
*   \param output_lengths The length of each output name string,
*                         excluding null terminating character
*   \param n_outputs The number of outputs
*   \return Returns SRNoError on success or an error code on failure
*/
SRError set_model_from_file(void* c_client,
                            const char* key,
                            const size_t key_length,
                            const char* model_file,
                            const size_t model_file_length,
                            const char* backend,
                            const size_t backend_length,
                            const char* device,
                            const size_t device_length,
                            const int batch_size,
                            const int min_batch_size,
                            const char* tag,
                            const size_t tag_length,
                            const char** inputs,
                            const size_t* input_lengths,
                            const size_t n_inputs,
                            const char** outputs,
                            const size_t* output_lengths,
                            const size_t n_outputs);

/*!
*   \brief Set a model (from buffer) in the database for future execution
*   \param c_client The client object to use for communication
*   \param key The key to associate with the model
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param model The model as a continuous buffer
*   \param model_length The length of the model string,
*                       excluding null terminating character
*   \param backend The name of the backend (TF, TFLITE, TORCH, ONNX)
*   \param backend_length The length of the backend string,
*                        excluding null terminating character
*   \param device The name of the device for execution (e.g. CPU or GPU)
*   \param device_length The length of the device string,
*                        excluding null terminating character
*   \param batch_size The batch size for model execution
*   \param min_batch_size The minimum batch size for model execution
*   \param tag A tag to attach to the model for information purposes
*   \param tag_length The length of the tag string,
*                     excluding null terminating character
*   \param inputs One or more names of model input nodes (TF models only)
*   \param input_lengths The length of each input name string,
*                        excluding null terminating character
*   \param n_inputs The number of inputs
*   \param outputs One or more names of model output nodes (TF models only)
*   \param output_lengths The length of each output name string,
*                         excluding null terminating character
*   \param n_outputs The number of outputs
*   \return Returns SRNoError on success or an error code on failure
*/
SRError set_model(void* c_client,
                  const char* key,
                  const size_t key_length,
                  const char* model,
                  const size_t model_length,
                  const char* backend,
                  const size_t backend_length,
                  const char* device,
                  const size_t device_length,
                  const int batch_size,
                  const int min_batch_size,
                  const char* tag,
                  const size_t tag_length,
                  const char** inputs,
                  const size_t* input_lengths,
                  const size_t n_inputs,
                  const char** outputs,
                  const size_t* output_lengths,
                  const size_t n_outputs);

/*!
*   \brief Get a model in the database.   The memory associated with the
*          model string is valid until the client is destroyed.
*   \param c_client The client object to use for communication
*   \param key The key to use to get the model
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param model_length The length of the model buffer string,
*                       excluding null terminating character
*   \param model Receives the model as a string
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError get_model(void* c_client,
                  const char* key,
                  const size_t key_length,
                  size_t* model_length,
                  const char** model);

/*!
*   \brief Set a script from file in the database for future execution
*   \param c_client The client object to use for communication
*   \param key The key to associate with the script
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param device The name of the device for execution e.g. CPU or GPU)
*   \param device_length The length of the device name string,
*                        excluding null terminating character
*   \param script_file The source file for the script
*   \param script_file_length The length of the script file name string,
*                             excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError set_script_from_file(void* c_client,
                             const char* key,
                             const size_t key_length,
                             const char* device,
                             const size_t device_length,
                             const char* script_file,
                             const size_t script_file_length);

/*!
*   \brief Set a script (from buffer) in the database for future execution
*   \param key The key to associate with the script
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param device The name of the device for execution (e.g. CPU or GPU)
*   \param device_length The length of the device name string,
*                        excluding null terminating character
*   \param script The script as a string buffer
*   \param script_length The length of the script string,
*                        excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError set_script(void* c_client,
                   const char* key,
                   const size_t key_length,
                   const char* device,
                   const size_t device_length,
                   const char* script,
                   const size_t script_length);

/*!
*   \brief Get a script from the database.  The memory associated with the
*          script string is valid until the client is destroyed.
*   \param c_client The client object to use for communication
*   \param key The key to use to get the script
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param script Receives the script in an allocated memory buffer
*   \param script_length The length of the script buffer string,
*                        excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError get_script(void* c_client,
                   const char* key,
                   const size_t key_length,
                   const char** script,
                   size_t* script_length);

/*!
*   \brief Run a script function in the database using the specificed input
*          and output tensors
*   \param c_client The client object to use for communication
*   \param key The key associated with the script
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param function The name of the function in the script to run
*   \param function_length The length of the function name string,
*                          excluding null terminating character
*   \param inputs The tensor keys of inputs tensors to use in the script
*   \param input_lengths The length of each input name string,
*                        excluding null terminating character
*   \param n_inputs The number of inputs
*   \param outputs The tensor keys of output tensors that will be used
*                  to save script results
*   \param output_lengths The length of each output name string,
*                         excluding null terminating character
*   \param n_outputs The number of outputs
*   \return Returns SRNoError on success or an error code on failure
*/
SRError run_script(void* c_client,
                   const char* key,
                   const size_t key_length,
                   const char* function,
                   const size_t function_length,
                   const char** inputs,
                   const size_t* input_lengths,
                   const size_t n_inputs,
                   const char** outputs,
                   const size_t* output_lengths,
                   const size_t n_outputs);

/*!
*   \brief Run a model in the database using the specificed input and
*          output tensors
*   \param c_client The client object to use for communication
*   \param key The key associated with the model
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param inputs The keys of inputs tensors to use in the script
*   \param input_lengths The length of each input name string,
*                        excluding null terminating character
*   \param n_inputs The number of inputs
*   \param outputs The tensor keys of output tensors to be used
*                  to save script results
*   \param output_lengths The length of each output name string,
*                         excluding null terminating character
*   \param n_outputs The number of outputs
*   \return Returns SRNoError on success or an error code on failure
*/
SRError run_model(void* c_client,
                  const char* key,
                  const size_t key_length,
                  const char** inputs,
                  const size_t* input_lengths,
                  const size_t n_inputs,
                  const char** outputs,
                  const size_t* output_lengths,
                  const size_t n_outputs);

/*!
*   \brief Check if a key exists in the database
*   \param c_client The client object to use for communication
*   \param key The key that will be checked in the database
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param exists Receives whether the key exists
*   \return Returns SRNoError on success or an error code on failure
*/
SRError key_exists(void* c_client,
                   const char* key,
                   const size_t key_length,
                   bool* exists);

/*!
*   \brief Check if a tensor key exists in the database
*   \param c_client The client object to use for communication
*   \param name The name of the tensor that will be checked in the database.
*               The full tensor key corresponding to \p name will be formed
*               in accordance with the current prefixing behavior
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param exists Receives whether the tensor exists
*   \return Returns SRNoError on success or an error code on failure
*/
SRError tensor_exists(void* c_client,
                      const char* name,
                      const size_t name_length,
                      bool* exists);

/*!
*   \brief Check if a model or script key exists in the database
*   \param c_client The client object to use for communication
*   \param name The name of the entity that will be checked in the database.
*               The full model/script key corresponding to \p name will be
*               formed in accordance with the current prefixing behavior
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param exists Receives whether the model/script exists
*   \return Returns SRNoError on success or an error code on failure
*/
SRError model_exists(void* c_client,
                     const char* name,
                     const size_t name_length,
                     bool* exists);

/*!
*   \brief Check if a dataset name key exists in the database
*   \param c_client The client object to use for communication
*   \param name The name of the dataset that will be checked in the database.
*               The full key corresponding to \p name will be formed
*               in accordance with the current prefixing behavior
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param exists Receives whether the dataset exists
*   \return Returns SRNoError on success or an error code on failure
*/
SRError dataset_exists(void* c_client,
                       const char* name,
                       const size_t name_length,
                       bool* exists);

/*!
*   \brief Check if a key exists in the database, repeating the check
*          at a specified frequency and number of repetitions
*   \param c_client The client object to use for communication
*   \param key The key that will be checked in the database
*   \param key_length The length of the key string,
*                     excluding null terminating character
*   \param poll_frequency_ms The time delay between checks, in milliseconds
*   \param num_tries The total number of times to check for the key
*   \param exists Receives whether the key is found within the
*                 specified number of tries
*   \return Returns SRNoError on success or an error code on failure
*/
SRError poll_key(void* c_client,
                 const char* key,
                 const size_t key_length,
                 const int poll_frequency_ms,
                 const int num_tries,
                 bool* exists);

/*!
*   \brief Check if a model or script exists in the database, repeating the
*          check at a specified frequency and number of repetitions
*   \param c_client The client object to use for communication
*   \param name The name of the entity that will be checked in the database.
*               The full key associated to \p name will be formed according
*               to the prefixing behavior
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param poll_frequency_ms The time delay between checks, in milliseconds
*   \param num_tries The total number of times to check for the model key
*   \param exists Receives whether the model is found within the
*                 specified number of tries
*   \return Returns SRNoError on success or an error code on failure
*/
SRError poll_model(void* c_client,
                   const char* name,
                   const size_t name_length,
                   const int poll_frequency_ms,
                   const int num_tries,
                   bool* exists);

/*!
*   \brief Check if a tensor exists in the database, repeating the check
*          at a specified frequency and number of repetitions
*   \param c_client The client object to use for communication
*   \param name The name of the entity that will be checked in the database.
*               The full key associated to \p name will be formed according
*               to the prefixing behavior
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param poll_frequency_ms The time delay between checks, in milliseconds
*   \param num_tries The total number of times to check for the tensor key
*   \param exists Receives whether the tensor is found within the
*                 specified number of tries
*   \return Returns SRNoError on success or an error code on failure
*/
SRError poll_tensor(void* c_client,
                    const char* name,
                    const size_t name_length,
                    const int poll_frequency_ms,
                    const int num_tries,
                    bool* exists);

/*!
*   \brief Check if a dataset exists in the database, repeating the check
*          at a specified frequency and number of repetitions
*   \param c_client The client object to use for communication
*   \param name The name of the entity that will be checked in the database.
*               The full key associated to \p name will be formed according
*               to the prefixing behavior
*   \param name_length The length of the name string,
*                      excluding null terminating character
*   \param poll_frequency_ms The time delay between checks, in milliseconds
*   \param num_tries The total number of times to check for the dataset key
*   \param exists Receives whether the tensor is found within the
*                 specified number of tries
*   \return Returns sr_ok on success
*/
SRError poll_dataset(void* c_client,
                     const char* name,
                     const size_t name_length,
                     const int poll_frequency_ms,
                     const int num_tries,
                     bool* exists);

/*!
*   \brief Set the data source (i.e. key prefix for get functions)
*   \param c_client The client object to use for communication
*   \param source_id The prefix for retrieval commands
*   \param source_id_length The length of the source_id string,
*                           excluding null terminating character
*   \return Returns SRNoError on success or an error code on failure
*/
SRError set_data_source(void* c_client,
                        const char* source_id,
                        const size_t source_id_length);

/*!
*   \brief Set whether names of tensor and dataset keys should be
*          prefixed (e.g. in an ensemble) when forming database keys.
*          Prefixes will only be used if they were previously set through
*          the environment variables SSKEYOUT and SSKEYIN.
*          Keys for entities created before this function is called
*          will not be retroactively prefixed.
*          By default, the client prefixes tensor and dataset keys
*          with the first prefix specified with the SSKEYIN
*          and SSKEYOUT environment variables.
*
*   \param c_client The client object to use for communication
*   \param use_prefix If true, all future operations on tensors and
*                     datasets will use a prefix, if available.
*   \return Returns SRNoError on success or an error code on failure
*/
SRError use_tensor_ensemble_prefix(void* c_client, bool use_prefix);

/*!
*   \brief Set whether names of model and script keys should be
*          prefixed (e.g. in an ensemble) to form database keys.
*          Prefixes will only be used if they were previously set through
*          the environment variables SSKEYOUT and SSKEYIN.
*          Keys for entities created before this function is called
*          will not be retroactively prefixed.
*          By default, the client does not prefix model and script keys.
*
*   \param use_prefix If set to true, all future operations on models and
*                     scripts will use a prefix, if available.
*   \return Returns SRNoError on success or an error code on failure
*/
SRError use_model_ensemble_prefix(void* c_client, bool use_prefix);

#ifdef __cplusplus
}

#endif
#endif // SMARTREDIS_C_CLIENT_H
