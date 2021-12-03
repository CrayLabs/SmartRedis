/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021, Hewlett Packard Enterprise
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

#if !defined(SMARTREDIS_SMART_ERROR_H)
#define SMARTREDIS_SMART_ERROR_H

typedef enum {
    sr_ok        = 0, // No error
    sr_badalloc  = 1, // Memory allocation error
    sr_dberr     = 2, // Backend database error
    sr_internal  = 3, // Internal SmartRedis error
    sr_runtime   = 4, // Runtime error executing an operation
    sr_parameter = 5, // Bad parameter error
    sr_timeout   = 6, // Timeout error
    sr_invalid   = 7  // Uninitialized error variable
} SRError;


// Retrieve the last error encountered
#ifdef __cplusplus
extern "C"
#endif
const char* sr_get_last_error();

#ifdef __cplusplus

#include <string>

// Smart error: custom error class for the SmartRedis library
class smart_error: public std::runtime_error
{
	// Inherit all the standard constructors
	using std::runtime_error::runtime_error;


	public:
	smart_error(const char* what_arg, const char* file, int line)
	  : std::runtime_error(std::string(what_arg) + "\n" + file + ":" + std::to_string(line))
	{
	    // NOP
	}

	smart_error(const std::string& what_arg, const char* file, int line)
	  : std::runtime_error(std::string(what_arg) + "\n" + file + ":" + std::to_string(line))
	{
	    // NOP
	}

	smart_error(const smart_error& other) noexcept
	  : std::runtime_error(other)
	{
		// NOP
	}

	virtual SRError to_error_code() const noexcept {
		return sr_invalid;
	}
};

// Memory allocation error
class _smart_bad_alloc: public smart_error
{
	using smart_error::smart_error;

	virtual SRError to_error_code() const noexcept {
		return sr_badalloc;
	}
};

#define smart_bad_alloc(txt) _smart_bad_alloc(txt, __FILE__, __LINE__)

//  Back-end database error
class _smart_database_error: public smart_error
{
	using smart_error::smart_error;

	virtual SRError to_error_code() const noexcept {
		return sr_dberr;
	}
};

#define smart_database_error(txt) _smart_database_error(txt, __FILE__, __LINE__)

// Runtime error
class _smart_runtime_error: public smart_error
{
	using smart_error::smart_error;

	virtual SRError to_error_code() const noexcept {
		return sr_runtime;
	}
};

#define smart_runtime_error(txt) _smart_runtime_error(txt, __FILE__, __LINE__)

// Parameter error
class _smart_parameter_error: public smart_error
{
	using smart_error::smart_error;

	virtual SRError to_error_code() const noexcept {
		return sr_parameter;
	}
};

#define smart_parameter_error(txt) _smart_parameter_error(txt, __FILE__, __LINE__)

#define smart_runtime_error(txt) _smart_runtime_error(txt, __FILE__, __LINE__)

// Timeout error
class _smart_timeout_error: public smart_error
{
	using smart_error::smart_error;

	virtual SRError to_error_code() const noexcept {
		return sr_timeout;
	}
};

#define smart_timeout_error(txt) _smart_timeout_error(txt, __FILE__, __LINE__)

// Internal error
class _smart_internal_error: public smart_error
{
	using smart_error::smart_error;

	virtual SRError to_error_code() const noexcept {
		return sr_internal;
	}
};

#define smart_internal_error(txt) _smart_internal_error(txt, __FILE__, __LINE__)

// Store the last error encountered
extern "C"
void sr_set_last_error(const smart_error& last_error);

#endif // __cplusplus
#endif // SMARTREDIS_SMART_ERROR_H