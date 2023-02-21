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

#ifndef SMARTREDIS_C_CONFIGOPTIONS_H
#define SMARTREDIS_C_CONFIGOPTIONS_H

#include "sr_enums.h"
#include "srexception.h"

///@file
///\brief C-wrappers for the C++ DataSet class

#ifdef __cplusplus
extern "C" {
#endif

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          environment variables. If \p db_prefix is non-empty,
*          then "{db_prefix}_" will be prepended to the name of
*          each environment variable that is read.
*   \param db_prefix The prefix to use with environment variables,
*                    or an empty string to disable prefixing
*   \param db_prefix_length The length of the db_prefix string,
*                           excluding null terminating character
*   \param log_context The context to associate with logging for an
*                      object using this ConfigOptions
*   \param log_context_length The length of the log_context string,
*                             excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_environment(
    const char* db_prefix,
    const size_t db_prefix_length,
    const char* log_context,
    const size_t log_context_length,
    void** new_configoptions);

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          a file with JSON data.
*   \param filename A UTF-8 file with JSON data containing the
*                   configuration data
*   \param filename_length The length of the filename string,
*                          excluding null terminating character
*   \param log_context The context to associate with logging for an
*                      object using this ConfigOptions
*   \param log_context_length The length of the log_context string,
*                             excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_file(
    const char* filename,
    const size_t filename_length,
    const char* log_context,
    const size_t log_context_length,
    void** new_configoptions);

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          a string containing a JSON blob
*   \param json_blob A JSON blob containing the configuration data
*   \param json_blob_length The length of the json_blob string,
*                          excluding null terminating character
*   \param log_context The context to associate with logging for an
*                      object using this ConfigOptions
*   \param log_context_length The length of the log_context string,
*                             excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_string(
    const char* json_blob,
    const size_t json_blob_length,
    const char* log_context,
    const size_t log_context_length,
    void** new_configoptions);

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          the current default source
*   \param log_context The context to associate with logging for an
*                      object using this ConfigOptions
*   \param log_context_length The length of the log_context string,
*                             excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_default(
    const char* log_context,
    const size_t log_context_length,
    void** new_configoptions);

} // extern "C"

#endif
#endif // SMARTREDIS_C_CONFIGOPTIONS_H
