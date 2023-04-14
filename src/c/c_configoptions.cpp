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

#include <iostream>
#include <cstring>
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

    auto cfgOpts = ConfigOptions::create_from_environment(db_prefix_str);
    ConfigOptions* pCfgOpts = cfgOpts.release();
    *new_configoptions = reinterpret_cast<void* >(pCfgOpts);
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

// Configuration via JSON file or JSON blob is anticipated in the future
// but not supported yet
#ifdef FUTURE_CONFIG_SUPPORT
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

    auto cfgOpts = ConfigOptions::create_from_file(filename_str);
    ConfigOptions* pCfgOpts = cfgOpts.release();
    *new_configoptions = reinterpret_cast<void* >(pCfgOpts);
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

    auto cfgOpts = ConfigOptions::create_from_string(json_blob_str);
    ConfigOptions* pCfgOpts = cfgOpts.release();
    *new_configoptions = reinterpret_cast<void* >(pCfgOpts);
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
#endif

// Retrieve the value of a numeric configuration option
extern "C"
SRError get_integer_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    int64_t* option_result)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_cfgopts != NULL && key != NULL &&
      option_result != NULL);

    std::string key_str(key, key_len);
    ConfigOptions* co = reinterpret_cast<ConfigOptions*>(c_cfgopts);

    *option_result = co->get_integer_option(key_str);
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

// Retrieve the value of a string configuration option
extern "C"
SRError get_string_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    char** option_result,
    size_t* option_result_len)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_cfgopts != NULL && key != NULL &&
      option_result != NULL && option_result_len != NULL);

    std::string key_str(key, key_len);
    ConfigOptions* co = reinterpret_cast<ConfigOptions*>(c_cfgopts);

    std::string option_result_str = co->get_string_option(key_str);

    // TBD FINDME: we're leaking memory here since there will
    // be no way to reclaim it
    *option_result_len = option_result_str.length();
    *option_result = new char[*option_result_len + 1];
    strncpy(*option_result, option_result_str.c_str(), *option_result_len);
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

// Check whether a configuration option is set
extern "C"
SRError is_configured(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    bool* cfg_result)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_cfgopts != NULL && key != NULL && cfg_result != NULL);

    std::string key_str(key, key_len);
    ConfigOptions* co = reinterpret_cast<ConfigOptions*>(c_cfgopts);

    *cfg_result = co->is_configured(key_str);
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

// Override the value of a numeric configuration option
extern "C"
SRError override_integer_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    int64_t value)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_cfgopts != NULL && key != NULL);

    std::string key_str(key, key_len);
    ConfigOptions* co = reinterpret_cast<ConfigOptions*>(c_cfgopts);

    co->override_integer_option(key_str, value);
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

// Override the value of a string configuration option
extern "C"
SRError override_string_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    const char* value,
    size_t value_len)
{
  SRError result = SRNoError;
  try {
    // Sanity check params
    SR_CHECK_PARAMS(c_cfgopts != NULL && key != NULL && value != NULL);

    std::string key_str(key, key_len);
    std::string value_str(value, value_len);
    ConfigOptions* co = reinterpret_cast<ConfigOptions*>(c_cfgopts);

    co->override_string_option(key_str, value_str);
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
