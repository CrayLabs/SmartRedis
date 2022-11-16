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
#include "utility.h"

using namespace SmartRedis;

/*!
*   \brief Initialize an integer from an environment variable
*   \param value Receives the value of the environment variable
*   \param env_var The name of the environment variable to query
*   \param default_value Default if the environment variable is not set
*/
void get_integer_from_env(int& value,
                         const std::string& env_var,
                         int default_value)
{
    value = default_value;

    char* env_val = std::getenv(env_var.c_str());

    if (env_val != NULL && strlen(env_val) > 0) {
        // Enforce that all characters are digits because std::stoi
        // will truncate a string like "10xy" to 10.
        // We want to guard users from input errors they might have.
        for (char* c = env_val; *c != '\0'; c++) {
            if (!isdigit(*c) && !(*c == '-' && c == env_val)) {
                throw SRParameterException("The value of " + env_var +
                                           " must be a valid number.");
            }
        }

        try {
            value = std::stoi(env_val);
        }
        catch (std::invalid_argument& e) {
            throw SRParameterException("The value of " + env_var + " could "\
                                       "not be converted to type integer.");
        }
        catch (std::out_of_range& e) {
            throw SRParameterException("The value of " + env_var + " is too "\
                                       "large to be stored as an integer "\
                                       "value.");
        }
        catch (std::exception& e) {
            throw SRInternalException("An unexpected error occurred  "\
                                      "while attempting to convert the "\
                                      "environment variable " + env_var +
                                      " to an integer.");
        }
    }
}

/*!
*   \brief Initialize a string from an environment variable
*   \param value Receives the value of the environment variable
*   \param env_var The name of the environment variable to query
*   \param default_value Default if the environment variable is not set
*/
void get_string_from_env(std::string& value,
                         const std::string& env_var,
                         const std::string& default_value)
{
    value = default_value;

    char* env_val = std::getenv(env_var.c_str());

    if (env_val != NULL && strlen(env_val) > 0)
        value = env_val;
}
