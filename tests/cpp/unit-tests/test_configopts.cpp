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

#include "../../../third-party/catch/single_include/catch2/catch.hpp"
#include "../client_test_utils.h"
#include "logger.h"
#include "configoptions.h"

unsigned long get_time_offset();

using namespace SmartRedis;

SCENARIO("Testing for ConfigOptions", "[CfgOpts]")
{
    std::cout << std::to_string(get_time_offset()) << ": Testing for ConfigOptions" << std::endl;
    std::string context("test_configopts");
    log_data(context, LLDebug, "***Beginning ConfigOptions testing***");

    GIVEN("A ConfigOptions object")
    {
        // Make sure keys aren't set before we start
        const char* keys[] = {
            "test_integer_key_that_is_not_really_present",
            "test_string_key_that_is_not_really_present",
            "test_boolean_key_that_is_not_really_present",
            "test_integer_key",
            "test_string_key",
            "test_boolean_key",
            "test_boolean_key_f0",
            "test_boolean_key_f1",
            "test_boolean_key_f2",
            "test_boolean_key_t0",
            "test_boolean_key_t1",
            "test_boolean_key_t2",
            "test_boolean_key_t3",
            "test_setting_a_string"
        };
        INFO("Reserved keys must not be set before running this test.");
        for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
            REQUIRE(std::getenv(keys[i]) == NULL);
        }

        // Set up keys for testing
        setenv("test_integer_key", "42", true);
        setenv("test_string_key", "charizard", true);
        setenv("test_boolean_key", "False", true);
        setenv("test_boolean_key_f0", "false", true);
        setenv("test_boolean_key_f1", "FALSE", true);
        setenv("test_boolean_key_f2", "0", true);
        setenv("test_boolean_key_t0", "fAlse", true);
        setenv("test_boolean_key_t1", "pikachu", true);
        setenv("test_boolean_key_t2", "1", true);
        setenv("test_boolean_key_t3", "fail", true);

        ConfigOptions* co = ConfigOptions::create_from_environment("");

        THEN("Options should be configurable")
        {
            // Unimplemented bits
            CHECK_THROWS_AS(
                ConfigOptions::create_from_file("some_file.json"),
                RuntimeException);
            CHECK_THROWS_AS(
                ConfigOptions::create_from_string("{ \"key\" = \"value\" }"),
                RuntimeException);
            ConfigOptions::set_default_from_file("some_file.json");
            CHECK_THROWS_AS(
                ConfigOptions::create_from_default(),
                RuntimeException);
#if 0
            ConfigOptions::set_default_from_string("{ \"key\" = \"value\" }");
            CHECK_THROWS_AS(
                ConfigOptions::create_from_default(),
                RuntimeException);
#endif

#if 0
            // integer option tests
            CHECK(co->get_integer_option("test_integer_key", 0) == 42);
            CHECK_FALSE(co->is_defined("test_integer_key_that_is_not_really_present"));
            CHECK(co->get_integer_option(
                "test_integer_key_that_is_not_really_present", 0) == 0);
            CHECK(co->is_defined("test_integer_key_that_is_not_really_present"));
            co->override_integer_option(
                "test_integer_key_that_is_not_really_present", 42
            );
            CHECK(co->get_integer_option(
                "test_integer_key_that_is_not_really_present", 0) == 42);

            // string option tests
            CHECK(co->get_string_option("test_string_key", "missing") == "charizard");
            CHECK_FALSE(co->is_defined("test_string_key_that_is_not_really_present"));
            CHECK(co->get_string_option(
                "test_string_key_that_is_not_really_present", "missing"
            ) == "missing");
            co->override_string_option(
                "test_string_key_that_is_not_really_present", "meowth"
            );
            CHECK(co->get_string_option(
                "test_string_key_that_is_not_really_present", "missing"
            ) == "meowth");
            CHECK(co->is_defined("test_string_key_that_is_not_really_present"));

            // boolean option tests
            CHECK(co->get_boolean_option("test_boolean_key", true) == false);
            CHECK_FALSE(co->is_defined("test_boolean_key_that_is_not_really_present"));
            CHECK(co->get_boolean_option(
                "test_boolean_key_that_is_not_really_present", true
            ) == true);
            co->override_boolean_option(
                "test_boolean_key_that_is_not_really_present", true
            );
            CHECK(co->get_boolean_option(
                "test_boolean_key_that_is_not_really_present", true
            ) == true);
            CHECK(co->is_defined("test_boolean_key_that_is_not_really_present"));
            for (size_t i = 0; i < 3; i++) {
                std::string test_key("test_boolean_key_f");
                test_key += std::to_string(i);
                CHECK(co->get_boolean_option(test_key, true) == false);
            }
            for (size_t i = 0; i < 4; i++) {
                std::string test_key("test_boolean_key_t");
                test_key += std::to_string(i);
                CHECK(co->get_boolean_option(test_key, false) == true);
            }
#endif
        }
    }

    // Clean up test keys
    unsetenv("test_integer_key");
    unsetenv("test_string_key");
    unsetenv("test_boolean_key");
    unsetenv("test_boolean_key_f0");
    unsetenv("test_boolean_key_f1");
    unsetenv("test_boolean_key_f2");
    unsetenv("test_boolean_key_t0");
    unsetenv("test_boolean_key_t1");
    unsetenv("test_boolean_key_t2");
    unsetenv("test_boolean_key_t3");

    log_data(context, LLDebug, "***End ConfigOptions testing***");
}

#ifdef SKIPPING_TESTS

SCENARIO("Prefix Testing for ConfigOptions", "[CfgOpts]")
{
    std::cout << std::to_string(get_time_offset()) << ": Prefix Testing for ConfigOptions" << std::endl;
    std::string context("test_configopts");
    log_data(context, LLDebug, "***Beginning ConfigOptions prefix testing***");

    GIVEN("A ConfigOptions object")
    {
        // Make sure keys aren't set before we start
        const char* keys[] = {
            "prefixtest_integer_key_that_is_not_really_present",
            "prefixtest_string_key_that_is_not_really_present",
            "prefixtest_boolean_key_that_is_not_really_present",
            "prefixtest_integer_key",
            "prefixtest_string_key",
            "prefixtest_boolean_key",
            "prefixtest_boolean_key_f0",
            "prefixtest_boolean_key_f1",
            "prefixtest_boolean_key_f2",
            "prefixtest_boolean_key_t0",
            "prefixtest_boolean_key_t1",
            "prefixtest_boolean_key_t2",
            "prefixtest_boolean_key_t3",
            "prefixtest_setting_a_string"
        };
        INFO("Reserved keys must not be set before running this test.");
        for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
            REQUIRE(std::getenv(keys[i]) == NULL);
        }

        // Set up keys for testing
        setenv("prefixtest_integer_key", "42", true);
        setenv("prefixtest_string_key", "charizard", true);
        setenv("prefixtest_boolean_key", "False", true);
        setenv("prefixtest_boolean_key_f0", "false", true);
        setenv("prefixtest_boolean_key_f1", "FALSE", true);
        setenv("prefixtest_boolean_key_f2", "0", true);
        setenv("prefixtest_boolean_key_t0", "fAlse", true);
        setenv("prefixtest_boolean_key_t1", "pikachu", true);
        setenv("prefixtest_boolean_key_t2", "1", true);
        setenv("prefixtest_boolean_key_t3", "fail", true);

        ConfigOptions* co = ConfigOptions::create_from_environment("prefixtest");

        THEN("Prefixed options should be configurable")
        {
            // Unimplemented bits
            CHECK_THROWS_AS(
                ConfigOptions::create_from_file("some_file.json"),
                RuntimeException);
            CHECK_THROWS_AS(
                ConfigOptions::create_from_string("{ \"key\" = \"value\" }"),
                RuntimeException);
            ConfigOptions::set_default_from_file("some_file.json");
            CHECK_THROWS_AS(
                ConfigOptions::create_from_default(),
                RuntimeException);
            ConfigOptions::set_default_from_string("{ \"key\" = \"value\" }");
            CHECK_THROWS_AS(
                ConfigOptions::create_from_default(),
                RuntimeException);

            // integer option tests
            CHECK(co->get_integer_option("integer_key", 0) == 42);
            CHECK_FALSE(co->is_defined("integer_key_that_is_not_really_present"));
            CHECK(co->get_integer_option(
                "integer_key_that_is_not_really_present", 0) == 0);
            CHECK(co->is_defined("integer_key_that_is_not_really_present"));
            co->override_integer_option(
                "integer_key_that_is_not_really_present", 42
            );
            CHECK(co->get_integer_option(
                "integer_key_that_is_not_really_present", 0) == 42);

            // string option tests
            CHECK(co->get_string_option("string_key", "missing") == "charizard");
            CHECK_FALSE(co->is_defined("string_key_that_is_not_really_present"));
            CHECK(co->get_string_option(
                "string_key_that_is_not_really_present", "missing"
            ) == "missing");
            co->override_string_option(
                "string_key_that_is_not_really_present", "meowth"
            );
            CHECK(co->get_string_option(
                "string_key_that_is_not_really_present", "missing"
            ) == "meowth");
            CHECK(co->is_defined("string_key_that_is_not_really_present"));

            // boolean option tests
            CHECK(co->get_boolean_option("boolean_key", true) == false);
            CHECK_FALSE(co->is_defined("boolean_key_that_is_not_really_present"));
            CHECK(co->get_boolean_option(
                "boolean_key_that_is_not_really_present", true
            ) == true);
            co->override_boolean_option(
                "boolean_key_that_is_not_really_present", true
            );
            CHECK(co->get_boolean_option(
                "boolean_key_that_is_not_really_present", true
            ) == true);
            CHECK(co->is_defined("boolean_key_that_is_not_really_present"));
            for (size_t i = 0; i < 3; i++) {
                std::string test_key("boolean_key_f");
                test_key += std::to_string(i);
                CHECK(co->get_boolean_option(test_key, true) == false);
            }
            for (size_t i = 0; i < 4; i++) {
                std::string test_key("boolean_key_t");
                test_key += std::to_string(i);
                CHECK(co->get_boolean_option(test_key, false) == true);
            }
        }
    }

    // Clean up test keys
    unsetenv("prefixtest_integer_key");
    unsetenv("prefixtest_string_key");
    unsetenv("prefixtest_boolean_key");
    unsetenv("prefixtest_boolean_key_f0");
    unsetenv("prefixtest_boolean_key_f1");
    unsetenv("prefixtest_boolean_key_f2");
    unsetenv("prefixtest_boolean_key_t0");
    unsetenv("prefixtest_boolean_key_t1");
    unsetenv("prefixtest_boolean_key_t2");
    unsetenv("prefixtest_boolean_key_t3");

    log_data(context, LLDebug, "***End ConfigOptions prefix testing***");
}

#endif // SKIPPING_TESTS
