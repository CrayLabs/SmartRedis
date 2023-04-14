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

#include <string_view>
#include "configoptions.h"
#include "srexception.h"
#include "logger.h"
#include "utility.h"

using namespace SmartRedis;

// ConfigOptions constructor
ConfigOptions::ConfigOptions(
    cfgSrc source,
    const std::string& string)
    : _source(source), _string(string), _lazy(source == cs_envt),
      _log_context("")
{
    // Read in options if needed
    if (!_lazy) {
        _populate_options();
    }
}

// Instantiate ConfigOptions, getting selections from environment variables
std::unique_ptr<ConfigOptions> ConfigOptions::create_from_environment(
    const std::string& db_prefix)
{
    // NOTE: We can't use std::make_unique<> here because our constructor
    // is private
    return std::unique_ptr<ConfigOptions>(
        new ConfigOptions(cs_envt, db_prefix));
}

// Configuration via JSON file or JSON blob is anticipated in the future
// but not supported yet
#ifdef FUTURE_CONFIG_SUPPORT
// Instantiate ConfigOptions, getting selections from a file with JSON data
std::unique_ptr<ConfigOptions> ConfigOptions::create_from_file(
    const std::string& filename)
{
    // NOTE: We can't use std::make_unique<> here because our constructor
    // is private
    return std::unique_ptr<ConfigOptions>(
        new ConfigOptions(cs_file, filename));
}

// Instantiate ConfigOptions, getting selections from a JSON blob
std::unique_ptr<ConfigOptions> ConfigOptions::create_from_string(
    const std::string& json_blob)
{
    // NOTE: We can't use std::make_unique<> here because our constructor
    // is private
    return std::unique_ptr<ConfigOptions>(
        new ConfigOptions(cs_blob, json_blob));
}
#endif

// Retrieve the value of a numeric configuration option
int64_t ConfigOptions::get_integer_option(const std::string& key)
{
    // If we already have the value, return it
    auto search = _int_options.find(key);
    if (search != _int_options.end())
        return _int_options[key];

    // If we're doing lazy evaluation of keys, fetch the value
    int64_t default_value = -1;
    int64_t result = default_value;
    if (_lazy) {
        int temp = 0;
        get_config_integer(
            temp, _prefixed(key), default_value, flag_no_default);
        result = (int64_t)temp;
    }

    // Store the final value before we exit
    _int_options.insert({key, result});
    return result;
}

// Retrieve the value of a string configuration option
std::string ConfigOptions::get_string_option(const std::string& key)
{
    // If we already have the value, return it
    auto search = _string_options.find(key);
    if (search != _string_options.end())
        return _string_options[key];

    // If we're doing lazy evaluation of keys, fetch the value
    std::string default_value("");
    std::string result(default_value);
    if (_lazy) {
        get_config_string(
            result, _prefixed(key), default_value, flag_no_default);
    }

    // Store the final value before we exit
    _string_options.insert({key, result});
    return result;
}

// Resolve the value of a numeric configuration option
int64_t ConfigOptions::_resolve_integer_option(
    const std::string& key, int64_t default_value)
{
    // If we already have the value, return it
    auto search = _int_options.find(key);
    if (search != _int_options.end())
        return _int_options[key];

    // If we're doing lazy evaluation of keys, fetch the value
    int64_t result = default_value;
    if (_lazy) {
        int temp = 0;
        get_config_integer(temp, _prefixed(key), default_value);
        result = (int64_t)temp;
    }

    // Store the final value before we exit
    _int_options.insert({key, result});
    return result;
}

// Resolve the value of a string configuration option
std::string ConfigOptions::_resolve_string_option(
    const std::string& key, const std::string& default_value)
{
    // If we already have the value, return it
    auto search = _string_options.find(key);
    if (search != _string_options.end())
        return _string_options[key];

    // If we're doing lazy evaluation of keys, fetch the value
    std::string result(default_value);
    if (_lazy) {
        get_config_string(result, _prefixed(key), default_value);
    }

    // Store the final value before we exit
    _string_options.insert({key, result});
    return result;
}

// Check whether a configuration option is set in the selected source
bool ConfigOptions::is_configured(const std::string& key)
{
    // Check each map in turn
    if (_int_options.find(key) != _int_options.end())
        return true;
    if (_string_options.find(key) != _string_options.end())
        return true;

    // Check to see if the value is available and we just haven't
    // seen it yet
    if (_lazy) {
        std::string prefixed = _prefixed(key);
        char* environment_string = std::getenv(prefixed.c_str());
        return NULL != environment_string;
    }

    // Not found
    return false;
}

// Override the value of a numeric configuration option
void ConfigOptions::override_integer_option(
    const std::string& key, int64_t value)
{
    _int_options.insert_or_assign(key, value);
}

// Override the value of a string configuration option
void ConfigOptions::override_string_option(
    const std::string& key, const std::string& value)
{
    _string_options.insert_or_assign(key, value);
}

// Process option data from a fixed source
void ConfigOptions::_populate_options()
{
    throw SRRuntimeException(
        "Sources other than environment variables "
        "are not currently supported"
    );
}

// Apply a prefix to a key if the source is environment
// variables and the prefix is nonempty
std::string ConfigOptions::_prefixed(const std::string& key)
{
    std::string result(key);
    if (_source == cs_envt && _string != "")
        result = _string + + "_" + key;
    return result;
}
