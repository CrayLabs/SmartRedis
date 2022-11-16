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
void Logger::configure_logging(const std::string& _client_id)
{
    // If we're already initialized, they can set up a client ID
    // Useful if they call a Dataset API point before setting up
    // a Client object
    if (initialized) {
        client_id = _client_id;
        return;
    }

    // Get the logfile
    get_string_from_env(logfile, "SS_LOG_FILE", "");
    if (logfile.length() == 0) {
        throw SRRuntimeException(
            "No file specified for logging");
    }

    // Make sure it can be written to
    { // Limit scope of logstream to keep file open for
      // minimal time
        std::ofstream logstream;
        logstream.open(logfile, std::ios_base::app);
        if (!logstream.good()) {
            throw SRRuntimeException(
                "Cannot write to file: " + logfile);
        }
    }

    // Get the logging level
    std::string level;
    get_string_from_env(level, "SS_LOG_LEVEL", "");
    if (level.length() == 0) {
        throw SRRuntimeException(
            "No level specified for logging");
    }
    if (level.compare("NONE") == 0)
        log_level = LLNone;
    else if (level.compare("INFO") == 0)
        log_level = LLInfo;
    else if (level.compare("DEBUG") == 0)
        log_level = LLDebug;
    else {
        throw SRRuntimeException(
            "Unrecognized logging level: " + level);
    }

    // Done
    initialized = true;
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
    if (!initialized)
        configure_logging("anonymous");

    // Silently ignore logging more verbose than requested
    if (level > log_level)
        return;

    // Get the current timestamp
    auto t = std::time(NULL);
    auto tm = *std::localtime(&t);
    auto timestamp = std::put_time(&tm, "%H-%M-%S");

    // write the log data
    std::ofstream logstream;
    logstream.open(logfile, std::ios_base::app);
    logstream << client_id << "@" << timestamp << ":" << data;
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
