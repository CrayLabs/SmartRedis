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

#ifndef SMARTREDIS_LOGGER_H
#define SMARTREDIS_LOGGER_H

#ifdef __cplusplus // Skip C++ headers for C users
#include <string>
#include "utility.h"
#endif // __cplusplus
#include <stdlib.h>
#include "sr_enums.h"
#include "srobject.h"

///@file

/*
* Redirect logging functions to exception-free variants for C and Fortran
*/
#ifndef __cplusplus
#define log_data log_data_noexcept
#define log_warning log_warning_noexcept
#define log_error log_error_noexcept
#endif // __cplusplus

#ifdef __cplusplus
namespace SmartRedis {

/*!
*   \brief The Logger class implements a logging facility for SmartRedis
*/
class Logger {

    public:

        /*!
        *   \brief Retrieve the unique Logger instance
        *   \returns The actual logger instance
        */
       static Logger& get_instance()
       {
           static Logger __instance; // instantiated on first acccess
           return __instance;
       }

        /*!
        *   \brief Set up logging for the current client
        *   \param logger_name ID to use for the current client
        */
       void configure_logging(const std::string& logger_name);

        /*!
        *   \brief Rename the current client
        *   \param new_name new ID to use for the current client
        */
        void rename_client(const std::string& new_name);

    private:

        /*!
        *   \brief Logger constructor
        */
        Logger() { _initialized = false; }

        /*!
        *   \brief Logger copy constructor unavailable
        *   \param logger The Logger to copy for construction
        */
        Logger(const Logger& logger) = delete;

        /*!
        *   \brief Logger assignment operator unavailable
        *   \param logger The Logger to assign
        */
        void operator=(const Logger& logger) = delete;

        /*!
        *   \brief Logger move assignment operator unavailable
        *   \param logger The Logger to move
        */
        void operator=(Logger&& logger) = delete;

        /*!
        *   \brief Default Logger destructor
        */
        ~Logger() = default;

    public:

        /*!
        *   \brief Conditionally log data if the logging level is high enough
        *   \param level Minimum logging level for data to be logged
        *   \param data Text of data to be logged
        */
        void log_data(SRLoggingLevel level, const char* data);

        /*!
        *   \brief Conditionally log data if the logging level is high enough
        *   \param level Minimum logging level for data to be logged
        *   \param data Text of data to be logged
        */
        void log_data(SRLoggingLevel level, const std::string& data);

        /*!
        *   \brief Conditionally log data if the logging level is high enough
        *   \param level Minimum logging level for data to be logged
        *   \param data Text of data to be logged
        */
        void log_data(SRLoggingLevel level, const std::string_view& data);

        /*!
        *   \brief Conditionally log warning data if the logging level is
        *          high enough
        *   \param level Minimum logging level for data to be logged
        *   \param data Text of data to be logged
        */
        void log_warning(SRLoggingLevel level, const std::string& data)
        {
            log_data(level, "WARNING: " + data);
        }

        /*!
        *   \brief Conditionally log error data if the logging level is
        *          high enough
        *   \param level Minimum logging level for data to be logged
        *   \param data Text of data to be logged
        */
        void log_error(SRLoggingLevel level, const std::string& data)
        {
            log_data(level, "ERROR: " + data);
        }

    private:

        /*!
        *   \brief Track whether logging is initialized
        */
        bool _initialized;

        /*!
        *   \brief The client ID for this client
        */
        std::string _logger_name;

        /*!
        *   \brief The file to which to write log data
        */
        std::string _logfile;

        /*!
        *   \brief The current logging level
        */
        SRLoggingLevel _log_level;
};

/*!
*   \brief Conditionally log data if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*/
inline void log_data(SRLoggingLevel level, const std::string& data)
{
    Logger::get_instance().log_data(level, data);
}

/*!
*   \brief Conditionally log a warning if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*/
inline void log_warning(SRLoggingLevel level, const std::string& data)
{
    Logger::get_instance().log_warning(level, data);
}

/*!
*   \brief Conditionally log an error if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*/
inline void log_error(SRLoggingLevel level, const std::string& data)
{
    Logger::get_instance().log_error(level, data);
}


/////////////////////////////////////////////////////////
// Stack-based function logger

/*!
*   \brief The FunctionLogger class logs entry and exit of an API function.
*          The intended use is to create an instance of this class on the stack
*          inside each API point via the LOG_API_FUNCTION macro, below.
*/
class FunctionLogger {
    public:
        /*!
        *   \brief Logger constructor
        *   \param function_name The name of the function to track
        */
        FunctionLogger(const char* function_name)
            : _name(function_name)
        {
            log_data(LLDebug, "API Function " + _name + " called");
        }

        /*!
        *   \brief Logger destructor
        */
        ~FunctionLogger()
        {
            log_data(LLDebug, "API Function " + _name + " exited");
        }
    private:
        /*!
        *   \brief The name of the current function
        */
        std::string _name;
};

/*!
*   \brief Instantiate a FunctionLogger for the enclosing API function
*/
#define LOG_API_FUNCTION() FunctionLogger ___function_logger___(__func__)


} //namespace SmartRedis
#endif // __cplusplus


/////////////////////////////////////////////////////////
// Prototypes for C and Fortran logging methods

/*!
*   \brief Macro to establish C linkage in both C and C++
*/
#ifdef __cplusplus
#define C_API extern "C"
#else
#define C_API
#endif

/*!
*   \brief Conditionally log data if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*   \param data_len Length in characters of data to be logged
*/
C_API void log_data_noexcept(
    SRLoggingLevel level, const char* data, size_t data_len);

/*!
*   \brief Conditionally log a warning if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*   \param data_len Length in characters of data to be logged
*/
C_API void log_warning_noexcept(
    SRLoggingLevel level, const char* data, size_t data_len);

/*!
*   \brief Conditionally log an error if the logging level is high enough
*   \param level Minimum logging level for data to be logged
*   \param data Text of data to be logged
*   \param data_len Length in characters of data to be logged
*/
C_API void log_error_noexcept(
    SRLoggingLevel level, const char* data, size_t data_len);

#endif //SMARTREDIS_LOGGER_H
