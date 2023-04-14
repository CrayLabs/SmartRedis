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


#include "pyconfigoptions.h"
#include "srexception.h"

using namespace SmartRedis;

namespace py = pybind11;

PyConfigOptions::PyConfigOptions()
{
    try {
        _configoptions = NULL;
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "during ConfigOptions construction.");
    }
}

PyConfigOptions::PyConfigOptions(ConfigOptions* configoptions)
{
    _configoptions = configoptions;
}

PyConfigOptions::~PyConfigOptions()
{
    if (_configoptions != NULL) {
        delete _configoptions;
        _configoptions = NULL;
    }
}

ConfigOptions* PyConfigOptions::get() {
    return _configoptions;
}

// Instantiate ConfigOptions from environment variables
PyConfigOptions* PyConfigOptions::create_from_environment(
    const std::string& db_prefix)
{
    try {
        auto cfgOpts = ConfigOptions::create_from_environment(db_prefix);
        ConfigOptions* pCfgOpts = cfgOpts.release();
        return new PyConfigOptions(pCfgOpts);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing create_from_environment.");
    }
}

// Configuration via JSON file or JSON blob is anticipated in the future
// but not supported yet
#ifdef FUTURE_CONFIG_SUPPORT
// Instantiate ConfigOptions from a file with JSON data
PyConfigOptions* PyConfigOptions::create_from_file(
    const std::string& filename)
{
    try {
        auto cfgOpts = ConfigOptions::create_from_file(filename);
        ConfigOptions* pCfgOpts = cfgOpts.release();
        return new PyConfigOptions(pCfgOpts);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing create_from_file.");
    }
}


// Instantiate ConfigOptions from a string containing a JSON blob
PyConfigOptions* PyConfigOptions::create_from_string(
    const std::string& json_blob)
{
    try {
        auto cfgOpts = ConfigOptions::create_from_string(json_blob);
        ConfigOptions* pCfgOpts = cfgOpts.release();
        return new PyConfigOptions(pCfgOpts);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing create_from_string.");
    }
}
#endif

// Retrieve the value of a numeric configuration option
// from the selected source
int64_t PyConfigOptions::get_integer_option(const std::string& key)
{
    try {
        if (_configoptions == NULL) {
            throw SRRuntimeException(
                "Attempted to call get_integer_option "\
                "from a non-factory constructed ConfigOptions");
        }
        return _configoptions->get_integer_option(key);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing get_integer_option.");
    }
}

// Retrieve the value of a string configuration option
// from the selected source
std::string PyConfigOptions::get_string_option(const std::string& key)
{
    try {
        if (_configoptions == NULL) {
            throw SRRuntimeException(
                "Attempted to call get_string_option "\
                "from a non-factory constructed ConfigOptions");
        }
        return _configoptions->get_string_option(key);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing get_string_option.");
    }
}

// Check whether a configuration option is set in the
// selected source
bool PyConfigOptions::is_configured(const std::string& key)
{
    try {
        if (_configoptions == NULL) {
            throw SRRuntimeException(
                "Attempted to call is_defined "\
                "from a non-factory constructed ConfigOptions");
        }
        return _configoptions->is_configured(key);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing is_defined.");
    }
}

// Override the value of a numeric configuration option
// in the selected source
void PyConfigOptions::override_integer_option(
    const std::string& key, int64_t value)
{
    try {
        if (_configoptions == NULL) {
            throw SRRuntimeException(
                "Attempted to call override_integer_option "\
                "from a non-factory constructed ConfigOptions");
        }
        return _configoptions->override_integer_option(key, value);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing override_integer_option.");
    }
}

// Override the value of a string configuration option
// in the selected source
void PyConfigOptions::override_string_option(
    const std::string& key, const std::string& value)
{
    try {
        if (_configoptions == NULL) {
            throw SRRuntimeException(
                "Attempted to call override_string_option "\
                "from a non-factory constructed ConfigOptions");
        }
        return _configoptions->override_string_option(key, value);
    }
    catch (Exception& e) {
        // exception is already prepared for caller
        throw;
    }
    catch (std::exception& e) {
        // should never happen
        throw SRInternalException(e.what());
    }
    catch (...) {
        // should never happen
        throw SRInternalException("A non-standard exception was encountered "\
                                  "while executing override_string_option.");
    }
}

// EOF

