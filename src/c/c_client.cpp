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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "c_client.h"
#include "srexception.h"
#include "srassert.h"

using namespace SmartRedis;

// Return a pointer to a new Client.
// The caller is responsible for deleting the client via DeleteClient().
extern "C"
SRError SmartRedisCClient(bool cluster, void** new_client)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(new_client != NULL);

    Client* s = new Client(cluster);
    *new_client = reinterpret_cast<void*>(s);
  }
  catch (const std::bad_alloc& e) {
    *new_client = NULL;
    SRSetLastError(SRBadAllocException("client allocation"));
    result = SRBadAllocError;
  }
  catch (const Exception& e) {
    *new_client = NULL;
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    *new_client = NULL;
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Free the memory associated with the c client.
extern "C"
SRError DeleteCClient(void** c_client)
{
  SRError result = SRNoError;

  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL);

    delete reinterpret_cast<Client*>(*c_client);
    *c_client = NULL;
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Put a dataset into the database.
extern "C"
SRError put_dataset(void* c_client, void* dataset)
{
  SRError result = SRNoError;

  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && dataset != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    DataSet* d = reinterpret_cast<DataSet*>(dataset);

    s->put_dataset(*d);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Return a pointer to a new dataset.  The user is
// responsible for deleting the dataset via DeallocateeDataSet()
extern "C"
SRError get_dataset(void* c_client, const char* name,
                    const size_t name_length, void **dataset)
{
  SRError result = SRNoError;

  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && dataset != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string dataset_name(name, name_length);
    DataSet* d = NULL;

    try {
      d = new DataSet(s->get_dataset(dataset_name));
      *dataset = reinterpret_cast<void*>(d);
    } catch (const std::bad_alloc& e) {
      *dataset = NULL;
      throw SRBadAllocException("client allocation");
    }
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Rename a dataset in the database.
extern "C"
SRError rename_dataset(void* c_client, const char* old_name,
                       const size_t old_name_length, const char* new_name,
                       const size_t new_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && old_name != NULL && new_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(old_name, old_name_length);
    std::string new_name_str(new_name, new_name_length);

    s->rename_dataset(name_str, new_name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}


// Copy a dataset from the src_name to the dest_name
extern "C"
SRError copy_dataset(void* c_client, const char* src_name,
                    const size_t src_name_length, const char* dest_name,
                    const size_t dest_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && src_name != NULL && dest_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string src_name_str(src_name, src_name_length);
    std::string dest_name_str(dest_name, dest_name_length);

    s->copy_dataset(src_name_str, dest_name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delete a dataset (all metadata and tensors) from the database.
extern "C"
SRError delete_dataset(void* c_client, const char* name, const size_t name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string dataset_name(name, name_length);
    s->delete_dataset(dataset_name);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Put a tensor of a specified type into the database
extern "C"
SRError put_tensor(void* c_client,
                  const char* name,
                  const size_t name_length,
                  void* data,
                  const size_t* dims,
                  const size_t n_dims,
                  const SRTensorType type,
                  const SRMemoryLayout mem_layout)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL &&
                    data != NULL && dims != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    std::vector<size_t> dims_vec;
    dims_vec.assign(dims, dims + n_dims);

    s->put_tensor(name_str, data, dims_vec, type, mem_layout);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Get a tensor of a specified type from the database
extern "C"
SRError get_tensor(void* c_client,
                  const char* name,
                  const size_t name_length,
                  void** result,
                  size_t** dims,
                  size_t* n_dims,
                  SRTensorType* type,
                  const SRMemoryLayout mem_layout)
{
  SRError outcome = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && result != NULL &&
                    dims != NULL && n_dims != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    s->get_tensor(name_str, *result, *dims, *n_dims, *type, mem_layout);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    outcome = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    outcome = SRInternalError;
  }

  return outcome;
}

// Get a tensor of a specified type from the database
// and put the values into the user provided memory space.
extern "C"
SRError unpack_tensor(void* c_client,
                     const char* name,
                     const size_t name_length,
                     void* result,
                     const size_t* dims,
                     const size_t n_dims,
                     const SRTensorType type,
                     const SRMemoryLayout mem_layout)
{
  SRError outcome = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && result != NULL &&
                    dims != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    std::vector<size_t> dims_vec;
    dims_vec.assign(dims, dims + n_dims);

    s->unpack_tensor(name_str, result, dims_vec, type, mem_layout);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    outcome = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    outcome = SRInternalError;
  }

  return outcome;
}

// Rename a tensor from old_name to new_name
extern "C"
SRError rename_tensor(void* c_client,
                      const char* old_name, const size_t old_name_length,
                      const char* new_name, const size_t new_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && old_name != NULL && new_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string old_name_str(old_name, old_name_length);
    std::string new_name_str(new_name, new_name_length);

    s->rename_tensor(old_name_str, new_name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delete a tensor from the database.
extern "C"
SRError delete_tensor(void* c_client, const char* name,
                      const size_t name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    s->delete_tensor(name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Copy a tensor from src_name to dest_name.
extern "C"
SRError copy_tensor(void* c_client,
                   const char* src_name,
                   const size_t src_name_length,
                   const char* dest_name,
                   const size_t dest_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && src_name != NULL && dest_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string src_str(src_name, src_name_length);
    std::string dest_str(dest_name, dest_name_length);

    s->copy_tensor(src_str, dest_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

bool CompareCaseInsensitive(const char* a,const char* b) {
  while (*a != '\0' && *b != '\0') {
    // Check current character
    if (toupper(*a) != toupper(*b))
      return false;

    // Advance to next character
    a++;
    b++;
  }

  // Make sure there is no additional data in b
  return (*a == *b);
}

// Return True if the backend is TF or TFLITE
bool _isTensorFlow(const char* backend)
{
  return CompareCaseInsensitive(backend, "TF") || CompareCaseInsensitive(backend, "TFLITE");
}

// Check the parameters common to all set_model functions
void _check_params_set_model(void* c_client,
                            const char* name, const char* backend,
                            const char** inputs, const size_t* input_lengths, const size_t n_inputs,
                            const char** outputs, const size_t* output_lengths, const size_t n_outputs)
{
  // Sanity check params. Tag is strictly optional, and inputs/outputs are
  // mandatory IFF backend is TensorFlow (TF or TFLITE)
  SR_CHECK_PARAMS(c_client != NULL && name != NULL && backend != NULL);

  if (_isTensorFlow(backend)) {
    if (inputs == NULL || input_lengths == NULL ||
        outputs == NULL || output_lengths == NULL) {
      throw SRParameterException("Inputs and outputs are required with TensorFlow");
    }
  }

  // For the inputs and outputs arrays, a single empty string is ok (this means
  // that the array should be skipped) but if more than one entry is present, the
  // strings must be nonzero length
  if (_isTensorFlow(backend)) {
    if (n_inputs != 1 && input_lengths[0] != 0) {
      for (size_t i = 0; i < n_inputs; i++){
        if (inputs[i] == NULL || input_lengths[i] == 0) {
          throw SRParameterException(
            "inputs[" + std::to_string(i) + "] is NULL or empty");
        }
      }
    }
    if (n_outputs != 1 && output_lengths[0] != 0) {
      for (size_t i = 0; i < n_outputs; i++) {
        if (outputs[i] == NULL || output_lengths[i] == 0) {
          throw SRParameterException(
            "outputs[" + std::to_string(i) + "] is NULL or empty");
        }
      }
    }
  }
}

// Set a model stored in a binary file.
extern "C"
SRError set_model_from_file(void* c_client,
                           const char* name, const size_t name_length,
                           const char* model_file, const size_t model_file_length,
                           const char* backend, const size_t backend_length,
                           const char* device, const size_t device_length,
                           const int batch_size, const int min_batch_size,
                           const char* tag, const size_t tag_length,
                           const char** inputs, const size_t* input_lengths,
                           const size_t n_inputs,
                           const char** outputs, const size_t* output_lengths,
                           const size_t n_outputs)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params. Tag is strictly optional, and inputs/outputs are
    // mandatory IFF backend is TensorFlow (TF or TFLITE)
    _check_params_set_model(c_client, name, backend, inputs, input_lengths, n_inputs,
                          outputs, output_lengths, n_outputs);
    SR_CHECK_PARAMS(model_file != NULL && device != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string model_file_str(model_file, model_file_length);
    std::string backend_str(backend, backend_length);
    std::string device_str(device, device_length);
    std::string tag_str(tag, tag_length);

    // Catch the case where an empty string was sent (default C++ client behavior)
    std::vector<std::string> input_vec;
    if (_isTensorFlow(backend)) {
      if (n_inputs != 1 || input_lengths[0] != 0) {
        for (size_t i = 0; i < n_inputs; i++) {
          input_vec.push_back(std::string(inputs[i], input_lengths[i]));
        }
      }
    }

    std::vector<std::string> output_vec;
    if (_isTensorFlow(backend)) {
      if (n_outputs != 1 || output_lengths[0] != 0) {
        for (size_t i = 0; i < n_outputs; i++) {
          output_vec.push_back(std::string(outputs[i], output_lengths[i]));
        }
      }
    }

    s->set_model_from_file(name_str, model_file_str, backend_str, device_str,
                           batch_size, min_batch_size, tag_str, input_vec,
                           output_vec);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}
extern "C"
SRError set_model_from_file_multigpu(void* c_client,
                                     const char* name, const size_t name_length,
                                     const char* model_file, const size_t model_file_length,
                                     const char* backend, const size_t backend_length,
                                     const int first_gpu, const int num_gpus,
                                     const int batch_size, const int min_batch_size,
                                     const char* tag, const size_t tag_length,
                                     const char** inputs, const size_t* input_lengths,
                                     const size_t n_inputs, const char** outputs,
                                     const size_t* output_lengths, const size_t n_outputs)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params. Tag is strictly optional, and inputs/outputs are
    // mandatory IFF backend is TensorFlow (TF or TFLITE)
    _check_params_set_model(c_client, name, backend, inputs, input_lengths, n_inputs,
                          outputs, output_lengths, n_outputs);
    SR_CHECK_PARAMS(model_file != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string model_file_str(model_file, model_file_length);
    std::string backend_str(backend, backend_length);
    std::string tag_str(tag, tag_length);

    // Catch the case where an empty string was sent (default C++ client behavior)
    std::vector<std::string> input_vec;
    if (_isTensorFlow(backend)) {
      if (n_inputs != 1 || input_lengths[0] != 0) {
        for (size_t i = 0; i < n_inputs; i++) {
          input_vec.push_back(std::string(inputs[i], input_lengths[i]));
        }
      }
    }

    std::vector<std::string> output_vec;
    if (_isTensorFlow(backend)) {
      if (n_outputs != 1 || output_lengths[0] != 0) {
        for (size_t i = 0; i < n_outputs; i++) {
          output_vec.push_back(std::string(outputs[i], output_lengths[i]));
        }
      }
    }

    s->set_model_from_file_multigpu(name_str, model_file_str, backend_str, first_gpu,
                                   num_gpus, batch_size, min_batch_size, tag_str,
                                   input_vec, output_vec);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Set a model stored in a buffer c-string.
extern "C"
SRError set_model(void* c_client,
                 const char* name, const size_t name_length,
                 const char* model, const size_t model_length,
                 const char* backend, const size_t backend_length,
                 const char* device, const size_t device_length,
                 const int batch_size, const int min_batch_size,
                 const char* tag, const size_t tag_length,
                 const char** inputs, const size_t* input_lengths,
                 const size_t n_inputs,
                 const char** outputs, const size_t* output_lengths,
                 const size_t n_outputs)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params. Tag is strictly optional, and inputs/outputs are
    // mandatory IFF backend is TensorFlow (TF or TFLITE)
    _check_params_set_model(c_client, name, backend, inputs, input_lengths, n_inputs,
                          outputs, output_lengths, n_outputs);
    SR_CHECK_PARAMS(model != NULL && device != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string model_str(model, model_length);
    std::string backend_str(backend, backend_length);
    std::string device_str(device, device_length);
    std::string tag_str(tag, tag_length);

    // Catch the case where an empty string was sent (default C++ client behavior)
    std::vector<std::string> input_vec;
    if (_isTensorFlow(backend)) {
      if (n_inputs != 1 || input_lengths[0] != 0) {
        for (size_t i = 0; i < n_inputs; i++) {
          input_vec.push_back(std::string(inputs[i], input_lengths[i]));
        }
      }
    }

    std::vector<std::string> output_vec;
    if (_isTensorFlow(backend)) {
      if (n_outputs != 1 || output_lengths[0] != 0) {
        for (size_t i = 0; i < n_outputs; i++) {
          output_vec.push_back(std::string(outputs[i], output_lengths[i]));
        }
      }
    }

    s->set_model(name_str, model_str, backend_str, device_str,
                batch_size, min_batch_size, tag_str, input_vec,
                output_vec);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Set a model stored in a buffer c-string.
extern "C"
SRError set_model_multigpu(void* c_client,
                          const char* name, const size_t name_length,
                          const char* model, const size_t model_length,
                          const char* backend, const size_t backend_length,
                          const int first_gpu, const int num_gpus,
                          const int batch_size, const int min_batch_size,
                          const char* tag, const size_t tag_length,
                          const char** inputs, const size_t* input_lengths,
                          const size_t n_inputs,
                          const char** outputs, const size_t* output_lengths,
                          const size_t n_outputs)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params. Tag is strictly optional, and inputs/outputs are
    // mandatory IFF backend is TensorFlow (TF or TFLITE)
    _check_params_set_model(c_client, name, backend, inputs, input_lengths, n_inputs,
                          outputs, output_lengths, n_outputs);
    SR_CHECK_PARAMS(model != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string model_str(model, model_length);
    std::string backend_str(backend, backend_length);
    std::string tag_str(tag, tag_length);

    // Catch the case where an empty string was sent (default C++ client behavior)
    std::vector<std::string> input_vec;
    if (_isTensorFlow(backend)) {
      if (n_inputs != 1 || input_lengths[0] != 0) {
        for (size_t i = 0; i < n_inputs; i++) {
          input_vec.push_back(std::string(inputs[i], input_lengths[i]));
        }
      }
    }

    std::vector<std::string> output_vec;
    if (_isTensorFlow(backend)) {
      if (n_outputs != 1 || output_lengths[0] != 0) {
        for (size_t i = 0; i < n_outputs; i++) {
          output_vec.push_back(std::string(outputs[i], output_lengths[i]));
        }
      }
    }

    s->set_model_multigpu(name_str, model_str, backend_str, first_gpu, num_gpus,
                         batch_size, min_batch_size, tag_str, input_vec,
                         output_vec);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}


// Retrieve the model and model length from the database
extern "C"
SRError get_model(void* c_client,
                  const char* name,
                  const size_t name_length,
                  size_t* model_length,
                  const char** model)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && model_length != NULL &&
                    model != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string_view model_str_view(s->get_model(name_str));

    *model_length = model_str_view.size();
    *model = model_str_view.data();
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Put a script in the database that is stored in a file.
extern "C"
SRError set_script_from_file(void* c_client,
                            const char* name,
                            const size_t name_length,
                            const char* device,
                            const size_t device_length,
                            const char* script_file,
                            const size_t script_file_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && device != NULL &&
                    script_file != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string device_str(device, device_length);
    std::string script_file_str(script_file, script_file_length);

    s->set_script_from_file(name_str, device_str, script_file_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Put a script in the database that is stored in a file in a multi-GPU system
extern "C"
SRError set_script_from_file_multigpu(void* c_client,
                                     const char* name,
                                     const size_t name_length,
                                     const char* script_file,
                                     const size_t script_file_length,
                                     const int first_gpu,
                                     const int num_gpus)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && script_file != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string script_file_str(script_file, script_file_length);

    s->set_script_from_file_multigpu(name_str, script_file_str, first_gpu, num_gpus);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Put a script in the database that is stored in a string.
extern "C"
SRError set_script(void* c_client,
                  const char* name,
                  const size_t name_length,
                  const char* device,
                  const size_t device_length,
                  const char* script,
                  const size_t script_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && device != NULL &&
                    script != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);

    std::string name_str(name, name_length);
    std::string device_str(device, device_length);
    std::string script_str(script, script_length);

    s->set_script(name_str, device_str, script_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Put a script in the database that is stored in a string in a multi-GPU system
extern "C"
SRError set_script_multigpu(void* c_client,
                           const char* name,
                           const size_t name_length,
                           const char* script,
                           const size_t script_length,
                           const int first_gpu,
                           const int num_gpus)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && script != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);

    std::string name_str(name, name_length);
    std::string script_str(script, script_length);

    s->set_script_multigpu(name_str, script_str, first_gpu, num_gpus);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}



// Retrieve the script stored in the database
extern "C"
SRError get_script(void* c_client,
                  const char* name,
                  const size_t name_length,
                  const char** script,
                  size_t* script_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && script != NULL &&
                    script_length != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);
    std::string_view script_str_view(s->get_script(name_str));

    (*script) = script_str_view.data();
    (*script_length) = script_str_view.size();
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

void _check_params_run_script(void* c_client,
                              const char* name,
                              const char* function,
                              const char** inputs,
                              const size_t* input_lengths,
                              const size_t n_inputs,
                              const char** outputs,
                              const size_t* output_lengths,
                              const size_t n_outputs)
{
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && function != NULL &&
                    inputs != NULL && input_lengths != NULL &&
                    outputs != NULL && output_lengths != NULL);

    // Inputs and outputs are mandatory for run_script
    for (size_t i = 0; i < n_inputs; i++){
      if (inputs[i] == NULL || input_lengths[i] == 0) {
        throw SRParameterException(
          "inputs[" + std::to_string(i) + "] is NULL or empty");
      }
    }
    for (size_t i = 0; i < n_outputs; i++) {
      if (outputs[i] == NULL || output_lengths[i] == 0) {
        throw SRParameterException(
          "outputs[" + std::to_string(i) + "] is NULL or empty");
      }
    }
}

// Run  a script function in the database
extern "C"
SRError run_script(void* c_client,
                  const char* name,
                  const size_t name_length,
                  const char* function,
                  const size_t function_length,
                  const char** inputs,
                  const size_t* input_lengths,
                  const size_t n_inputs,
                  const char** outputs,
                  const size_t* output_lengths,
                  const size_t n_outputs)
{
  SRError result = SRNoError;
  try
  {
    _check_params_run_script(c_client, name, function,
                             inputs, input_lengths, n_inputs,
                             outputs, output_lengths, n_outputs);
    std::string name_str(name, name_length);
    std::string function_str(function, function_length);

    std::vector<std::string> input_vec;
    if (n_inputs != 1 || input_lengths[0] != 0) {
      for (size_t i = 0; i < n_inputs; i++) {
        input_vec.push_back(std::string(inputs[i], input_lengths[i]));
      }
    }

    std::vector<std::string> output_vec;
    if (n_outputs != 1 || output_lengths[0] != 0) {
      for (size_t i = 0; i < n_outputs; i++) {
        output_vec.push_back(std::string(outputs[i], output_lengths[i]));
      }
    }

    Client* s = reinterpret_cast<Client*>(c_client);
    s->run_script(name_str, function_str, input_vec, output_vec);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Run  a script function in the database in a multi-GPU system
extern "C"
SRError run_script_multigpu(void* c_client,
                           const char* name,
                           const size_t name_length,
                           const char* function,
                           const size_t function_length,
                           const char** inputs,
                           const size_t* input_lengths,
                           const size_t n_inputs,
                           const char** outputs,
                           const size_t* output_lengths,
                           const size_t n_outputs,
                           const int offset,
                           const int first_gpu,
                           const int num_gpus)
{
  SRError result = SRNoError;
  try
  {
    _check_params_run_script(c_client, name, function,
                             inputs, input_lengths, n_inputs,
                             outputs, output_lengths, n_outputs);
    std::string name_str(name, name_length);
    std::string function_str(function, function_length);

    std::vector<std::string> input_vec;
    if (n_inputs != 1 || input_lengths[0] != 0) {
      for (size_t i = 0; i < n_inputs; i++) {
        input_vec.push_back(std::string(inputs[i], input_lengths[i]));
      }
    }

    std::vector<std::string> output_vec;
    if (n_outputs != 1 || output_lengths[0] != 0) {
      for (size_t i = 0; i < n_outputs; i++) {
        output_vec.push_back(std::string(outputs[i], output_lengths[i]));
      }
    }

    Client* s = reinterpret_cast<Client*>(c_client);
    s->run_script_multigpu(name_str, function_str, input_vec, output_vec,
                           offset, first_gpu, num_gpus);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

void _check_params_run_model(void* c_client,
                  const char* name,
                  const char** inputs,
                  const size_t* input_lengths,
                  const size_t n_inputs,
                  const char** outputs,
                  const size_t* output_lengths,
                  const size_t n_outputs)
{
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL &&
                    inputs != NULL && input_lengths != NULL &&
                    outputs != NULL && output_lengths != NULL);

    // Inputs and outputs are mandatory for run_script
    for (size_t i = 0; i < n_inputs; i++){
      if (inputs[i] == NULL || input_lengths[i] == 0) {
        throw SRParameterException(
          "inputs[" + std::to_string(i) + "] is NULL or empty");
      }
    }
    for (size_t i = 0; i < n_outputs; i++) {
      if (outputs[i] == NULL || output_lengths[i] == 0) {
        throw SRParameterException(
          "outputs[" + std::to_string(i) + "] is NULL or empty");
      }
    }
}

// Run a model in the database
extern "C"
SRError run_model(void* c_client,
                 const char* name,
                 const size_t name_length,
                 const char** inputs,
                 const size_t* input_lengths,
                 const size_t n_inputs,
                 const char** outputs,
                 const size_t* output_lengths,
                 const size_t n_outputs)
{
  SRError result = SRNoError;
  try
  {
    _check_params_run_model(c_client, name, inputs, input_lengths, n_inputs,
                           outputs, output_lengths, n_outputs);
    std::string name_str(name, name_length);

    std::vector<std::string> input_vec;
    if (n_inputs != 1 || input_lengths[0] != 0) {
      for (size_t i = 0; i < n_inputs; i++) {
        input_vec.push_back(std::string(inputs[i], input_lengths[i]));
      }
    }

    std::vector<std::string> output_vec;
    if (n_outputs != 1 || output_lengths[0] != 0) {
      for (size_t i = 0; i < n_outputs; i++) {
        output_vec.push_back(std::string(outputs[i], output_lengths[i]));
      }
    }

    Client* s = reinterpret_cast<Client*>(c_client);
    s->run_model(name_str, input_vec, output_vec);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Run a model in the database
extern "C"
SRError run_model_multigpu(void* c_client,
                          const char* name,
                          const size_t name_length,
                          const char** inputs,
                          const size_t* input_lengths,
                          const size_t n_inputs,
                          const char** outputs,
                          const size_t* output_lengths,
                          const size_t n_outputs,
                          const int offset,
                          const int first_gpu,
                          const int num_gpus)
{
  SRError result = SRNoError;
  try
  {
    _check_params_run_model(c_client, name, inputs, input_lengths, n_inputs,
                           outputs, output_lengths, n_outputs);
    std::string name_str(name, name_length);

    std::vector<std::string> input_vec;
    if (n_inputs != 1 || input_lengths[0] != 0) {
      for (size_t i = 0; i < n_inputs; i++) {
        input_vec.push_back(std::string(inputs[i], input_lengths[i]));
      }
    }

    std::vector<std::string> output_vec;
    if (n_outputs != 1 || output_lengths[0] != 0) {
      for (size_t i = 0; i < n_outputs; i++) {
        output_vec.push_back(std::string(outputs[i], output_lengths[i]));
      }
    }

    Client* s = reinterpret_cast<Client*>(c_client);
    s->run_model_multigpu(name_str, input_vec, output_vec, offset,
                         first_gpu, num_gpus);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Remove a model from the database
extern "C"
SRError delete_model(void* c_client,
                     const char* name,
                     const size_t name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL);

    std::string name_str(name, name_length);
    Client* s = reinterpret_cast<Client*>(c_client);
    s->delete_model(name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Remove a model from the database on a system with multiple GPUs
extern "C"
SRError delete_model_multigpu(void* c_client,
                              const char* name,
                              const size_t name_length,
                              const int first_gpu,
                              const int num_gpus)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL);

    std::string name_str(name, name_length);
    Client* s = reinterpret_cast<Client*>(c_client);
    s->delete_model_multigpu(name_str, first_gpu, num_gpus);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Remove a script from the database
extern "C"
SRError delete_script(void* c_client,
                      const char* name,
                      const size_t name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL);

    std::string name_str(name, name_length);
    Client* s = reinterpret_cast<Client*>(c_client);
    s->delete_script(name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Remove a script from the database in a system with multiple GPUs
extern "C"
SRError delete_script_multigpu(void* c_client,
                               const char* name,
                               const size_t name_length,
                               const int first_gpu,
                               const int num_gpus)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL);

    std::string name_str(name, name_length);
    Client* s = reinterpret_cast<Client*>(c_client);
    s->delete_script_multigpu(name_str, first_gpu, num_gpus);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Check whether a key exists in the database
extern "C"
SRError key_exists(void* c_client, const char* key, const size_t key_length,
                   bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && key != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string key_str(key, key_length);

    *exists = s->key_exists(key_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Check whether a model exists in the database
extern "C"
SRError model_exists(void* c_client, const char* name, const size_t name_length,
                     bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    *exists = s->model_exists(name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Check whether a tensor exists in the database
extern "C"
SRError tensor_exists(void* c_client, const char* name, const size_t name_length,
                      bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    *exists = s->tensor_exists(name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delay until a dataset exists in the database
extern "C"
SRError dataset_exists(void* c_client, const char* name, const size_t name_length,
                       bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client *>(c_client);
    std::string name_str(name, name_length);

    *exists = s->dataset_exists(name_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delay until a key exists in the database
extern "C"
SRError poll_key(void* c_client,
                 const char* key,
                 const size_t key_length,
                 const int poll_frequency_ms,
                 const int num_tries,
                 bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && key != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string key_str(key, key_length);

    *exists = s->poll_key(key_str, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delay until a model exists in the database
extern "C"
SRError poll_model(void* c_client,
                   const char* name,
                   const size_t name_length,
                   const int poll_frequency_ms,
                   const int num_tries,
                   bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    *exists = s->poll_model(name_str, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delay until a tensor exists in the database
extern "C"
SRError poll_tensor(void* c_client,
                 const char* name,
                 const size_t name_length,
                 const int poll_frequency_ms,
                 const int num_tries,
                 bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    *exists = s->poll_tensor(name_str, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delay until a dataset exists in the database
extern "C"
SRError poll_dataset(void* c_client,
                     const char* name,
                     const size_t name_length,
                     const int poll_frequency_ms,
                     const int num_tries,
                     bool* exists)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && exists != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string name_str(name, name_length);

    *exists = s->poll_dataset(name_str, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Establish a data source
extern "C"
SRError set_data_source(void* c_client,
                        const char* source_id,
                        const size_t source_id_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && source_id != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string source_id_str(source_id, source_id_length);

    s->set_data_source(source_id_str);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Control whether a model ensemble prefix is used
extern "C"
SRError use_model_ensemble_prefix(void* c_client, bool use_prefix)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    s->use_model_ensemble_prefix(use_prefix);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Control whether a tensor ensemble prefix is used
extern "C"
SRError use_tensor_ensemble_prefix(void* c_client, bool use_prefix)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    s->use_tensor_ensemble_prefix(use_prefix);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Control whether aggregation lists are prefixed
extern "C"
SRError use_list_ensemble_prefix(void* c_client, bool use_prefix)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    s->use_list_ensemble_prefix(use_prefix);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Append a dataset to the aggregation list
extern "C"
SRError append_to_list(void* c_client, const char* list_name,
                       const size_t list_name_length, const void* dataset)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && list_name != NULL && dataset != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    const DataSet* d = reinterpret_cast<const DataSet*>(dataset);
    std::string lname(list_name, list_name_length);

    s->append_to_list(lname, *d);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Delete an aggregation list
extern "C"
SRError delete_list(void* c_client, const char* list_name,
                    const size_t list_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && list_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(list_name, list_name_length);

    s->delete_list(lname);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Copy an aggregation list
extern "C"
SRError copy_list(void* c_client,
                  const char* src_name, const size_t src_name_length,
                  const char* dest_name, const size_t dest_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && src_name != NULL && dest_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string sname(src_name, src_name_length);
    std::string dname(dest_name, dest_name_length);

    s->copy_list(sname, dname);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Rename an aggregation list
extern "C"
SRError rename_list(void* c_client,
                    const char* src_name, const size_t src_name_length,
                    const char* dest_name, const size_t dest_name_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && src_name != NULL && dest_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string sname(src_name, src_name_length);
    std::string dname(dest_name, dest_name_length);

    s->rename_list(sname, dname);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Get the number of entries in the list
extern "C"
SRError get_list_length(void* c_client, const char* list_name,
                        const size_t list_name_length, int* result_length)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && list_name != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(list_name, list_name_length);

    *result_length = s->get_list_length(lname);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Poll list length until length is equal to the provided length
extern "C"
SRError poll_list_length(void* c_client, const char* name,
                         const size_t name_length, int list_length,
                         int poll_frequency_ms, int num_tries,
                         bool* poll_result)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && poll_result != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(name, name_length);

    *poll_result = s->poll_list_length(
      lname, list_length, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Poll list length until length is greater than or equal to the provided length
extern "C"
SRError poll_list_length_gte(void* c_client, const char* name,
                             const size_t name_length, int list_length,
                             int poll_frequency_ms, int num_tries,
                             bool* poll_result)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && poll_result != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(name, name_length);

    *poll_result = s->poll_list_length_gte(
      lname, list_length, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Poll list length until length is less than or equal to the provided length
extern "C"
SRError poll_list_length_lte(void* c_client, const char* name,
                             const size_t name_length, int list_length,
                             int poll_frequency_ms, int num_tries,
                             bool* poll_result)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && name != NULL && poll_result != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(name, name_length);

    *poll_result = s->poll_list_length_lte(
      lname, list_length, poll_frequency_ms, num_tries);
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Get datasets from an aggregation list
extern "C"
SRError get_datasets_from_list(void* c_client, const char* list_name,
                               const size_t list_name_length,
                               void*** datasets, size_t* num_datasets)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && list_name != NULL &&
                    datasets != NULL && num_datasets != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(list_name, list_name_length);

    std::vector<DataSet> result_datasets = s->get_datasets_from_list(lname);
    size_t ndatasets = result_datasets.size();
    *datasets = NULL;
    if (ndatasets > 0) {
      DataSet** alloc = new DataSet*[ndatasets];
      for (size_t i = 0; i < ndatasets; i++) {
        alloc[i] = new DataSet(std::move(result_datasets[i]));
      }
      *datasets = (void**)alloc;
    }
    *num_datasets = ndatasets;
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Get a range of datasets (by index) from an aggregation list
extern "C"
SRError get_dataset_list_range(void* c_client, const char* list_name,
                               const size_t list_name_length,
                               const int start_index, const int end_index,
                               void*** datasets, size_t* num_datasets)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && list_name != NULL &&
                    datasets != NULL && num_datasets != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(list_name, list_name_length);

    std::vector<DataSet> result_datasets = s->get_dataset_list_range(
      lname, start_index, end_index);
    size_t ndatasets = result_datasets.size();
    *datasets = NULL;
    if (*num_datasets > 0) {
      DataSet** alloc = new DataSet*[ndatasets];
      for (size_t i = 0; i < ndatasets; i++) {
        alloc[i] = new DataSet(std::move(result_datasets[i]));
      }
      *datasets = (void**)alloc;
    }
    *num_datasets = ndatasets;
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}
// Get a range of datasets (by index) from an aggregation list into an
// already allocated vector of datasets
extern "C"
SRError _get_dataset_list_range_allocated(void* c_client, const char* list_name,
                               const size_t list_name_length,
                               const int start_index, const int end_index,
                               void** datasets)
{
  SRError result = SRNoError;
  try
  {
    // Sanity check params
    SR_CHECK_PARAMS(c_client != NULL && list_name != NULL &&
                    datasets != NULL);

    Client* s = reinterpret_cast<Client*>(c_client);
    std::string lname(list_name, list_name_length);

    std::vector<DataSet> result_datasets = s->get_dataset_list_range(
      lname, start_index, end_index);
    size_t num_datasets = result_datasets.size();
    if ( num_datasets != (size_t) (end_index-start_index+1)) {
      SRSetLastError(SRInternalException(
        "Returned dataset list is not equal to the requested range"
      ));
    }

    if (num_datasets > 0) {
      for (size_t i = 0; i < num_datasets; i++) {
        datasets[i] = (void*)(new DataSet(std::move(result_datasets[i])));
      }
    }
  }
  catch (const Exception& e) {
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}
