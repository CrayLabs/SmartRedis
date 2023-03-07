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

/////////////////////////////////////////////////////////////
// Factory construction methods

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          environment variables. If \p db_prefix is non-empty,
*          then "{db_prefix}_" will be prepended to the name of
*          each environment variable that is read.
*   \param db_prefix The prefix to use with environment variables,
*                    or an empty string to disable prefixing
*   \param db_prefix_length The length of the db_prefix string,
*                           excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_environment(
    const char* db_prefix,
    const size_t db_prefix_length,
    void** new_configoptions);

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          a file with JSON data.
*   \param filename A UTF-8 file with JSON data containing the
*                   configuration data
*   \param filename_length The length of the filename string,
*                          excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_file(
    const char* filename,
    const size_t filename_length,
    void** new_configoptions);

/*!
*   \brief Instantiate ConfigOptions, getting selections from
*          a string containing a JSON blob
*   \param json_blob A JSON blob containing the configuration data
*   \param json_blob_length The length of the json_blob string,
*                          excluding null terminating character
*   \param new_configoptions Receives the new configoptions object
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError create_configoptions_from_string(
    const char* json_blob,
    const size_t json_blob_length,
    void** new_configoptions);


/////////////////////////////////////////////////////////////
// Option access

/*!
*   \brief Retrieve the value of a numeric configuration option
*          from the selected source
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to retrieve
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param default_value The baseline value of the configuration
*          option to be returned if a value was not set in the
*          selected source
*   \param result Receives the selected integer option result. Returns
*            \p default_value if the option was not set in the
*            selected source
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError get_integer_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    int64_t default_value,
    int64_t* result);

/*!
*   \brief Retrieve the value of a string configuration option
*          from the selected source
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to retrieve
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param default_value The baseline value of the configuration
*          option to be returned if a value was not set in the
*          selected source
*   \param default_value_len The length of the default_value string,
*                            excluding null terminating character
*   \param result Receives the selected string option result. Returns
*            \p default_value if the option was not set in the
*            selected source
*   \param result_len Receives the length of the result string,
*                     excluding null terminating character
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError get_string_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    const char* default_value,
    size_t default_value_len,
    char** result,
    size_t* result_len);

/*!
*   \brief Retrieve the value of a boolean configuration option
*          from the selected source
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to retrieve
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param default_value The baseline value of the configuration
*          option to be returned if a value was not set in the
*          selected source
*   \param result Receives the selected boolean option result. Returns
*            \p default_value if the option was not set in the
*            selected source
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError get_boolean_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    bool default_value,
    bool* result);

/*!
*   \brief Check whether a configuration option is set in the
*          selected source
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to check
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param result Receives true IFF the key was defined or has been
*                 overridden; false otherwise
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError is_defined(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    bool* result);

/////////////////////////////////////////////////////////////
// Option overrides

/*!
*   \brief Override the value of a numeric configuration option
*          in the selected source
*   \details Overrides are specific to an instance of the
*            ConfigOptions class. An instance that references
*            the same source will not be affected by an override to
*            a different ConfigOptions instance
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to override
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param value The value to store for the configuration option
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError override_integer_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    int64_t value);

/*!
*   \brief Override the value of a string configuration option
*          in the selected source
*   \details Overrides are specific to an instance of the
*            ConfigOptions class. An instance that references
*            the same source will not be affected by an override to
*            a different ConfigOptions instance
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to override
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param value The value to store for the configuration option
*   \param value_len The length of the value string,
*                    excluding null terminating character
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError override_string_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    const char* value,
    size_t value_len);

/*!
*   \brief Override the value of a boolean configuration option
*          in the selected source
*   \details Overrides are specific to an instance of the
*            ConfigOptions class. An instance that references
*            the same source will not be affected by an override to
*            a different ConfigOptions instance
*   \param c_cfgopts The ConfigOptions object to use for communication
*   \param key The name of the configuration option to override
*   \param key_len The length of the key string,
*                  excluding null terminating character
*   \param value The value to store for the configuration option
*   \returns Returns SRNoError on success or an error code on failure
*/
SRError override_boolean_option(
    void* c_cfgopts,
    const char* key,
    size_t key_len,
    bool value);


} // extern "C"

#endif
#endif // SMARTREDIS_C_CONFIGOPTIONS_H
