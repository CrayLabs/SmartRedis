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

#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "utility.h"
#include "logger.h"
#include "srexception.h"

using namespace SmartRedis;

/*!
*   \brief Set up logging for the current client
*   \param _client_id ID to use for the current client
*/
void Logger::configure_logging(const std::string& client_id)
{
    // If we're already initialized, they can set up a client ID
    // Useful if they call a Dataset API point before setting up
    // a Client object
    _client_id = client_id;
    if (_initialized) {
        return;
    }
    _initialized = true;

    // Get the logfile
    get_config_string(_logfile, "SR_LOG_FILE", "", true);
    std::string requestedLogfile(_logfile);
    bool missingLogFile = _logfile.length() == 0;

    // Make sure it can be written to
    bool badLogFile = false;
    if (_logfile.length() > 0) {
        std::ofstream logstream;
        logstream.open(_logfile, std::ios_base::app);
        badLogFile = !logstream.good();

        // Switch to STDOUT if we can't write to the specified file
        if (badLogFile)
            _logfile = "";
    }

    // Get the logging level
    std::string level;
    get_config_string(level, "SR_LOG_LEVEL", "", true);
    bool missingLogLevel = level.length() == 0;
    bool badLogLevel = false;
    if (level.length() > 0) {
        if (level.compare("QUIET") == 0)
            _log_level = LLQuiet;
        else if (level.compare("INFO") == 0)
            _log_level = LLInfo;
        else if (level.compare("DEBUG") == 0)
            _log_level = LLDebug;
        else if (level.compare("DEVELOPER") == 0)
            _log_level = LLDeveloper;
        else {
            // Don't recognize the requested level; use default
            badLogLevel = true;
            _log_level = LLInfo;
        }
    }
    else {
        _log_level = LLInfo;
    }

    // Now that everything is configured, issue warning and
    // error messages
    if (missingLogFile) {
        log_warning(
            LLInfo,
            "Environment variable SS_LOG_FILE is not set. "
            "Defaulting to stdout"
        );
    }
    if (missingLogLevel) {
        log_warning(
            LLInfo,
            "Environment variable SS_LOG_LEVEL is not set. "
            "Defaulting to INFO"
        );
    }
    if (badLogFile) {
        throw SRRuntimeException(
            "Cannot write to file: " + requestedLogfile);
    }
    if (badLogLevel) {
        throw SRRuntimeException(
            "Unrecognized logging level: " + level);
    }

    // Done
    _initialized = true;
}

/*!
*   \brief Conditionally log data if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*/
void Logger::log_data(SRLoggingLevel level, const std::string& data)
{
    // If we're not initialized, configure logging as an anonymous
    // client. This can happen if a caller invokes a Dataset API point
    // without initializing a Client object
    if (!_initialized)
        configure_logging("anonymous");

    // Silently ignore logging more verbose than requested
    if (level > _log_level)
        return;

    // Get the current timestamp
    auto t = std::time(NULL);
    auto tm = *std::localtime(&t);
    auto timestamp = std::put_time(&tm, "%H-%M-%S");

    // write the log data
    // (There must be a cleaner way to write this!)
    if (_logfile.length() > 0) {
        std::ofstream logstream;
        logstream.open(_logfile, std::ios_base::app);
        logstream << _client_id << "@" << timestamp << ":" << data
                  << std::endl;
    } else {
        std::cout << _client_id << "@" << timestamp << ":" << data
                  << std::endl;
    }
}

/*!
*   \brief Conditionally log data if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*/
void Logger::log_data(SRLoggingLevel level, const char* data)
{
    const std::string _data(data);
    log_data(level, _data);
}

/*!
*   \brief Conditionally log data if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*/
void Logger::log_data(SRLoggingLevel level, const std::string_view& data)
{
    const std::string _data(data);
    log_data(level, _data);
}

/*!
*   \brief Conditionally log data if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*   \param data_len Length in characters of data to be logged
*/
extern "C" void log_data_noexcept(
    SRLoggingLevel level, const char* data, size_t data_len)
{
    auto &logger = Logger::get_instance();
    try {
        std::string strData(data, data_len);
        logger.log_data(level, strData);
    }
    catch (Exception& e) {
        std::cout << "Logging failure: " << e.where()
                  << ": " << e.what() << std::endl;
    }
}

/*!
*   \brief Conditionally log a warning if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*   \param data_len Length in characters of data to be logged
*/
extern "C" void log_warning_noexcept(
    SRLoggingLevel level, const char* data, size_t data_len)
{
    auto &logger = Logger::get_instance();
    try {
        std::string strData(data, data_len);
        logger.log_warning(level, strData);
    }
    catch (Exception& e) {
        std::cout << "Logging failure: " << e.where()
                  << ": " << e.what() << std::endl;
    }
}

/*!
*   \brief Conditionally log an error if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*   \param data_len Length in characters of data to be logged
*/
extern "C" void log_error_noexcept(
    SRLoggingLevel level, const char* data, size_t data_len)
{
    auto &logger = Logger::get_instance();
    try {
        std::string strData(data, data_len);
        logger.log_error(level, strData);
    }
    catch (Exception& e) {
        std::cout << "Logging failure: " << e.where()
                  << ": " << e.what() << std::endl;
    }
}
