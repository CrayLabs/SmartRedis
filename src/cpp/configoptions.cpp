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

cfgSrc ConfigOptions::_default_source = cs_envt;
std::string ConfigOptions::_default_cfg_string = std::string("");

// ConfigOptions constructor
ConfigOptions::ConfigOptions(
    cfgSrc source,
    const std::string& string,
    const std::string& log_context)
    : _source(source), _string(string), _lazy(source == cs_envt),
      _log_context(log_context)
{
    // Handle the default case
    if (_source == cs_default) {
        _source = _default_source;
        _string = _default_cfg_string;
    }

    // Decide how lazy to be
    _lazy = _source == cs_envt;

    // If we need to read in options, do so now
    if (!_lazy) {
        _populate_options();
    }
}

// Instantiate ConfigOptions, getting selections from environment variables
ConfigOptions* ConfigOptions::create_from_environment(
    const std::string& db_prefix, const std::string& log_context)
{
    return new ConfigOptions(cs_envt, db_prefix, log_context);
}

// Instantiate ConfigOptions, getting selections from a file with JSON data
ConfigOptions* ConfigOptions::create_from_file(
    const std::string& filename, const std::string& log_context)
{
    return new ConfigOptions(cs_file, filename, log_context);
}

// Instantiate ConfigOptions, getting selections from a JSON blob
ConfigOptions* ConfigOptions::create_from_string(
    const std::string& json_blob, const std::string& log_context)
{
    return new ConfigOptions(cs_blob, json_blob, log_context);
}

// Instantiate ConfigOptions, getting selections from the default source
ConfigOptions* ConfigOptions::create_from_default(
    const std::string& log_context)
{
    return new ConfigOptions(
        _default_source, _default_cfg_string, log_context);
}

// Set environment variables with a particular prefix as the default source
void ConfigOptions::set_default_from_environment(const std::string& db_prefix)
{
    _default_source = cs_envt;
    _default_cfg_string = db_prefix;
}

// Set a UTF-8 file containing JSON data as the default configuration source
void ConfigOptions::set_default_from_file(const std::string& filename)
{
    _default_source = cs_file;
    _default_cfg_string = filename;
}

// Set a string containing a JSON blob as the default configuration source
void ConfigOptions::set_default_from_string(const std::string& json_blob)
{
    _default_source = cs_blob;
    _default_cfg_string = json_blob;
}

// Retrieve the value of a numeric configuration option
int64_t ConfigOptions::get_integer_option(
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
        get_config_integer(temp, key, default_value);
        result = (int64_t)temp;
    }

    // Store the final value before we exit
    _int_options.insert({key, result});
    return result;
}

// Retrieve the value of a string configuration option
std::string ConfigOptions::get_string_option(
    const std::string& key, const std::string& default_value)
{
    // If we already have the value, return it
    auto search = _string_options.find(key);
    if (search != _string_options.end())
        return _string_options[key];

    // If we're doing lazy evaluation of keys, fetch the value
    std::string result(default_value);
    if (_lazy) {
        get_config_string(result, key, default_value);
    }

    // Store the final value before we exit
    _string_options.insert({key, result});
    return result;
}

// Retrieve the value of a boolean configuration option
bool ConfigOptions::get_boolean_option(
    const std::string& key, bool default_value)
{
    // If we already have the value, return it
    auto search = _bool_options.find(key);
    if (search != _bool_options.end())
        return _bool_options[key];

    // If we're doing lazy evaluation of keys, fetch the value
    bool result = default_value;
    if (_lazy) {
        get_config_bool(result, key, default_value);
    }

    // Store the final value before we exit
    _bool_options.insert({key, result});
    return result;
}

// Check whether a configuration option is set in the selected source
bool ConfigOptions::is_defined(const std::string& key)
{
    // Check each map in turn
    if (_int_options.find(key) != _int_options.end())
        return true;
    if (_string_options.find(key) != _string_options.end())
        return true;
    if (_bool_options.find(key) != _bool_options.end())
        return true;

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

// Override the value of a boolean configuration option
void ConfigOptions::override_boolean_option(
    const std::string& key, bool value)
{
    _bool_options.insert_or_assign(key, value);
}

// Process option data from a fixed source
void ConfigOptions::_populate_options()
{
    throw SRRuntimeException(
        "Sources other than environment variables "
        "are not currently supported"
    );
}
