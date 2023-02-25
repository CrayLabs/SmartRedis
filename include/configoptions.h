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

#ifndef SMARTREDIS_CONFIGOPTIONS_H
#define SMARTREDIS_CONFIGOPTIONS_H

#ifndef __cplusplus
#error C users should include c_configoptions.h, not configoptions.h
#endif

#ifdef __cplusplus
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "srobject.h"
#include "sr_enums.h"

///@file

namespace SmartRedis {

/*!
*   \brief Configuration source enumeration
*/
enum cfgSrc {
    cs_file,    // Configuration data is coming from a JSON file
    cs_envt,    // Configuration data is coming from environment variables
    cs_blob,    // Configuration data is coming from a JSON blob string
    cs_default  // Configuration data is coming from the default source
} ;


/*!
*   \brief The ConfigOptions class bottlenecks all configuration
*          options used in SmartRedis and allows them to be accessed
*          from environment variables, from a JSON file, or from a
*          JSON blob supplied as a text string.
*/
class ConfigOptions
{
    private:
        /*!
        *   \brief ConfigOptions constructor. Do not use! To instantiate
        *          a ConfigOptions object, use one of the factory methods
        *          below.
        *   \param source The selected source for config data
        *   \param string The string associated with the source
        */
        ConfigOptions(cfgSrc source, const std::string& string);

    public:

        /*!
        *   \brief ConfigOptions copy constructor
        *   \param cfgopts The ConfigOptions to copy
        */
        ConfigOptions(const ConfigOptions& cfgopts) = default;

        /*!
        *   \brief ConfigOptions copy assignment operator
        *   \param cfgopts The ConfigOptions to copy and assign
        */
        ConfigOptions& operator=(const ConfigOptions& cfgopts) = default;

        /*!
        *   \brief ConfigOptions move constructor
        *   \param cfgopts The ConfigOptions to move
        */
        ConfigOptions(ConfigOptions&& cfgopts) = default;

        /*!
        *   \brief ConfigOptions move assignment operator
        *   \param cfgopts The ConfigOptions to move and assign
        */
        ConfigOptions& operator=(ConfigOptions&& cfgopts) = default;

        /*!
        *   \brief ConfigOptions destructor
        */
        virtual ~ConfigOptions() = default;

        /////////////////////////////////////////////////////////////
        // Factory construction methods

        /*!
        *   \brief Instantiate ConfigOptions, getting selections from
        *          environment variables. If \p db_prefix is non-empty,
        *          then "{db_prefix}_" will be prepended to the name of
        *          each environment variable that is read.
        *   \param db_prefix The prefix to use with environment variables,
        *                    or an empty string to disable prefixing
        *   \returns The constructed ConfigOptions object
        *   \throw SmartRedis::Exception if db_prefix contains invalid
        *          characters
        */
        static ConfigOptions* create_from_environment(
            const std::string& db_prefix);

        /*!
        *   \brief Instantiate ConfigOptions, getting selections from
        *          a file with JSON data.
        *   \param filename A UTF-8 file with JSON data containing the
        *                   configuration data
        *   \returns The constructed ConfigOptions object
        *   \throw SmartRedis::Exception if the file cannot be found or
        *          the data within it cannot be parsed
        */
        static ConfigOptions* create_from_file(
            const std::string& filename);

        /*!
        *   \brief Instantiate ConfigOptions, getting selections from
        *          a string containing a JSON blob
        *   \param json_blob A JSON blob containing the configuration data
        *   \returns The constructed ConfigOptions object
        *   \throw SmartRedis::Exception if the JSON blob cannot be parsed
        */
        static ConfigOptions* create_from_string(
            const std::string& json_blob);

        /*!
        *   \brief Instantiate ConfigOptions, getting selections from
        *          the current default source
        *   \returns The constructed ConfigOptions object
        */
        static ConfigOptions* create_from_default();

        /////////////////////////////////////////////////////////////
        // Default configuration selectors

        /*!
        *   \brief Set environment variables with a particular prefix
        *          as the default configuration source
        *   \param db_prefix The prefix to be prepended to environment
        *                    variables in the form {db_prefix}_{environment
        *                    variable}. If the prefix is an empty string,
        *                    no prepending is done.
        *   \throw SmartRedis::Exception if the prefix contains invalid
        *          characters
        */
        static void set_default_from_environment(
            const std::string& db_prefix);

        /*!
        *   \brief Set a UTF-8 file containing JSON data as the default
        *          configuration source
        *   \param filename A UTF-8 file with JSON data containing the
        *                   configuration data
        *   \throw SmartRedis::Exception if the file cannot be found or
        *          the data within it cannot be parsed
        */
        static void set_default_from_file(const std::string& filename);

        /*!
        *   \brief Set a string containing a JSON blob as the default
        *          configuration source
        *   \param json_blob A JSON blob containing the configuration data
        *   \throw SmartRedis::Exception if the JSON blob cannot be parsed
        */
        static void set_default_from_string(const std::string& json_blob);

        /////////////////////////////////////////////////////////////
        // Option access

        /*!
        *   \brief Retrieve the value of a numeric configuration option
        *          from the selected source
        *   \param key The name of the configuration option to retrieve
        *   \param default_value The baseline value of the configuration
        *          option to be returned if a value was not set in the
        *          selected source
        *   \returns The value of the selected option. Returns
        *            \p default_value if the option was not set in the
        *            selected source
        */
        int64_t get_integer_option(const std::string& key, int64_t default_value);

        /*!
        *   \brief Retrieve the value of a string configuration option
        *          from the selected source
        *   \param key The name of the configuration option to retrieve
        *   \param default_value The baseline value of the configuration
        *          option to be returned if a value was not set in the
        *          selected source
        *   \returns The value of the selected option. Returns
        *            \p default_value if the option was not set in the
        *            selected source
        */
        std::string get_string_option(const std::string& key, const std::string& default_value);

        /*!
        *   \brief Retrieve the value of a boolean configuration option
        *          from the selected source
        *   \param key The name of the configuration option to retrieve
        *   \param default_value The baseline value of the configuration
        *          option to be returned if a value was not set in the
        *          selected source
        *   \returns The value of the selected option. Returns
        *            \p default_value if the option was not set in the
        *            selected source
        */
        bool get_boolean_option(const std::string& key, bool default_value);

        /*!
        *   \brief Check whether a configuration option is set in the
        *          selected source
        *   \param key The name of the configuration option to check
        *   \returns True IFF the target option is defined in the selected
        *            source
        */
        bool is_defined(const std::string& key);

        /*!
        *   \brief Retrieve the logging context
        *   \returns The log context associated with this object
        */
        std::string get_log_context() { return _log_context; }

        /*!
        *   \brief Store the logging context
        *   \param log_context The context to associate with logging
        */
        void set_log_context(const std::string& log_context) {
            _log_context = log_context;
        }

        /////////////////////////////////////////////////////////////
        // Option overrides

        /*!
        *   \brief Override the value of a numeric configuration option
        *          in the selected source
        *   \details Overrides are specific to an instance of the
        *            ConfigOptions class. An instance that references
        *            the same source will not be affected by an override to
        *            a different ConfigOptions instance
        *   \param key The name of the configuration option to override
        *   \param value The value to store for the configuration option
        */
        void override_integer_option(const std::string& key, int64_t value);

        /*!
        *   \brief Override the value of a string configuration option
        *          in the selected source
        *   \details Overrides are specific to an instance of the
        *            ConfigOptions class. An instance that references
        *            the same source will not be affected by an override to
        *            a different ConfigOptions instance
        *   \param key The name of the configuration option to override
        *   \param value The value to store for the configuration option
        */
        void override_string_option(const std::string& key, const std::string& value);

        /*!
        *   \brief Override the value of a boolean configuration option
        *          in the selected source
        *   \details Overrides are specific to an instance of the
        *            ConfigOptions class. An instance that references
        *            the same source will not be affected by an override to
        *            a different ConfigOptions instance
        *   \param key The name of the configuration option to override
        *   \param value The value to store for the configuration option
        */
        void override_boolean_option(const std::string& key, bool value);

    private:

        /*!
        *   \brief Process option data from a fixed source
        *   \throw SmartRedis::Exception: sources other than environment
        *          variables are not implemented yet
        */
        void _populate_options();

        /*!
        *  \brief Boolean option map
        */
        std::unordered_map<std::string, bool> _bool_options;

        /*!
        *  \brief Integer option map
        */
        std::unordered_map<std::string, int64_t> _int_options;

        /*!
        *  \brief String option map
        */
        std::unordered_map<std::string, std::string> _string_options;

        /*!
        *  \brief Configuration source
        */
        cfgSrc _source;

        /*!
        *  \brief Configuration string. Meaning is specific to the source
        */
       std::string _string;

        /*!
        *  \brief Lazy evaluation (do we read in all options at once or only
        *         on demand)
        */
        bool _lazy;

        /*!
        *  \brief Logging context
        */
        std::string _log_context;

        /*!
        *  \brief Default configuration source
        */
        static cfgSrc _default_source;

        /*!
        *  \brief Default configuration string -- meaning is specific to the
        *         default config source. For from environment variables,
        *         the string is the key prefix. For from file, the string is
        *         the filename. For from string, it's the JSON blob
        */
       static std::string _default_cfg_string;
};

} // namespace SmartRedis

#endif
#endif // SMARTREDIS_CONFIGOPTIONS_H
