/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021-2023, Hewlett Packard Enterprise
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

#include "srassert.h"
#include "srexception.h"
#include "configoptions.h"

using namespace SmartRedis;

// Instantiate ConfigOptions from environment variables
extern "C"
SRError create_configoptions_from_environment(
    const char* db_prefix,
    const size_t db_prefix_length,
    void** new_configoptions)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(db_prefix != NULL && new_configoptions != NULL);

    std::string db_prefix_str(db_prefix, db_prefix_length);

    ConfigOptions* cfgOpts = ConfigOptions::create_from_environment(
        db_prefix_str);
    *new_configoptions = reinterpret_cast<void* >(cfgOpts);
  }
  catch (const std::bad_alloc& e) {
    *new_configoptions = NULL;
    SRSetLastError(SRBadAllocException("config options allocation"));
    result = SRBadAllocError;
  }
  catch (const Exception& e) {
    *new_configoptions = NULL;
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    *new_configoptions = NULL;
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Instantiate ConfigOptions, getting selections from a file with JSON data
extern "C"
SRError create_configoptions_from_file(
    const char* filename,
    const size_t filename_length,
    void** new_configoptions)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(filename != NULL && new_configoptions != NULL);

    std::string filename_str(filename, filename_length);

    ConfigOptions* cfgOpts = ConfigOptions::create_from_file(
        filename_str);
    *new_configoptions = reinterpret_cast<void* >(cfgOpts);
  }
  catch (const std::bad_alloc& e) {
    *new_configoptions = NULL;
    SRSetLastError(SRBadAllocException("config options allocation"));
    result = SRBadAllocError;
  }
  catch (const Exception& e) {
    *new_configoptions = NULL;
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    *new_configoptions = NULL;
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Instantiate ConfigOptions from a string containing a JSON blob
extern "C"
SRError create_configoptions_from_string(
    const char* json_blob,
    const size_t json_blob_length,
    void** new_configoptions)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(json_blob != NULL && new_configoptions != NULL);

    std::string json_blob_str(json_blob, json_blob_length);

    ConfigOptions* cfgOpts = ConfigOptions::create_from_string(
        json_blob_str);
    *new_configoptions = reinterpret_cast<void* >(cfgOpts);
  }
  catch (const std::bad_alloc& e) {
    *new_configoptions = NULL;
    SRSetLastError(SRBadAllocException("config options allocation"));
    result = SRBadAllocError;
  }
  catch (const Exception& e) {
    *new_configoptions = NULL;
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    *new_configoptions = NULL;
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}

// Instantiate ConfigOptions from the current default source
extern "C"
SRError create_configoptions_from_default(void** new_configoptions)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(new_configoptions != NULL);

    ConfigOptions* cfgOpts = ConfigOptions::create_from_default();
    *new_configoptions = reinterpret_cast<void* >(cfgOpts);
  }
  catch (const std::bad_alloc& e) {
    *new_configoptions = NULL;
    SRSetLastError(SRBadAllocException("config options allocation"));
    result = SRBadAllocError;
  }
  catch (const Exception& e) {
    *new_configoptions = NULL;
    SRSetLastError(e);
    result = e.to_error_code();
  }
  catch (...) {
    *new_configoptions = NULL;
    SRSetLastError(SRInternalException("Unknown exception occurred"));
    result = SRInternalError;
  }

  return result;
}
