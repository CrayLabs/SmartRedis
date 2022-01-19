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

#include "limits.h"

#include "../../../third-party/catch/catch.hpp"
#include "../client_test_utils.h"

#include "redisserver.h"
#include "rediscluster.h"
#include "redis.h"
#include "srexception.h"

using namespace SmartRedis;

/*  Derived objects for Redis and RedisCluster are defined to access
    protected members. There is slight code duplication, but
    currently SmartRedis is not coded for diamond inheritance
    (i.e. virtual base classes) of Redis and RedisCluster, and a
    change like that does not seem warranted for testing purposes
    only.  If virtual base classes are implemented, RedisTest and
    RedisClusterTest can be merged.
*/

class RedisTest : public Redis
{
    public:
        virtual int get_connection_timeout() {return _connection_timeout;}
        virtual int get_connection_interval() {return _connection_interval;}
        virtual int get_command_timeout() {return _command_timeout;}
        virtual int get_command_interval() {return _command_interval;}
        virtual int get_connection_attempts() {return _connection_attempts;}
        virtual int get_command_attempts() {return _command_attempts;}
        virtual int get_default_conn_timeout() {return _DEFAULT_CONN_TIMEOUT;}
        virtual int get_default_conn_interval() {return _DEFAULT_CONN_INTERVAL;}
        virtual int get_default_cmd_timeout() {return _DEFAULT_CMD_TIMEOUT;}
        virtual int get_default_cmd_interval() {return _DEFAULT_CMD_INTERVAL;}
};

class RedisClusterTest : public RedisCluster
{
    public:
        virtual int get_connection_timeout() {return _connection_timeout;}
        virtual int get_connection_interval() {return _connection_interval;}
        virtual int get_command_timeout() {return _command_timeout;}
        virtual int get_command_interval() {return _command_interval;}
        virtual int get_connection_attempts() {return _connection_attempts;}
        virtual int get_command_attempts() {return _command_attempts;}
        virtual int get_default_conn_timeout() {return _DEFAULT_CONN_TIMEOUT;}
        virtual int get_default_conn_interval() {return _DEFAULT_CONN_INTERVAL;}
        virtual int get_default_cmd_timeout() {return _DEFAULT_CMD_TIMEOUT;}
        virtual int get_default_cmd_interval() {return _DEFAULT_CMD_INTERVAL;}
};

// Helper method to invoke the constructor when we expect an
// error to be thrown
void invoke_constructor()
{
    if (use_cluster()) {
        RedisClusterTest cluster_obj;
    }
    else {
        RedisTest non_cluster_obj;
    }
}

// Helper function to unset all env so that there isn't any
// cross-test contamination
void unset_all_env_vars()
{
        unsetenv("SR_CONN_TIMEOUT");
        unsetenv("SR_CONN_INTERVAL");
        unsetenv("SR_CMD_TIMEOUT");
        unsetenv("SR_CMD_INTERVAL");
}

// Helper function to check that all default values being used
template <class T>
void check_all_defaults(T& server)
{
    CHECK(server.get_connection_timeout() ==
          server.get_default_conn_timeout());

    CHECK(server.get_connection_interval() ==
          server.get_default_conn_interval());

    CHECK(server.get_command_timeout() ==
          server.get_default_cmd_timeout());

    CHECK(server.get_command_interval() ==
          server.get_default_cmd_interval());
}

SCENARIO("Test runtime settings are initialized correctly", "[RedisServer]")
{
    GIVEN("A Redis derived object created with all environment variables unset")
    {
        unset_all_env_vars();

        if (use_cluster()) {
            RedisClusterTest redis_server;
            THEN("Default member variable values are used")
            {
                check_all_defaults(redis_server);
            }
        }
        else {
            RedisTest redis_server;
            THEN("Default member variable values are used")
            {
                check_all_defaults(redis_server);
            }
        }
    }
    GIVEN("A Redis derived object with empty environment variables set")
    {
        unset_all_env_vars();
        setenv("SR_CONN_TIMEOUT", "", true);
        setenv("SR_CONN_INTERVAL", "", true);
        setenv("SR_CMD_TIMEOUT", "", true);
        setenv("SR_CMD_INTERVAL", "", true);

        if (use_cluster()) {
            RedisClusterTest redis_server;
            THEN("Default member variable values are used")
            {
                check_all_defaults(redis_server);
            }
        }
        else {
            RedisTest redis_server;
            THEN("Default member variable values are used")
            {
                check_all_defaults(redis_server);
            }
        }
    }
    GIVEN("A Redis derived object with valid environment variables set")
    {
        int conn_timeout = 5; //seconds
        int conn_interval = 500; //milliseconds
        int cmd_timeout = 2; //seconds
        int cmd_interval = 250; //milliseconds
        int expected_conn_attempts = 11;
        int expected_cmd_attempts = 9;

        unset_all_env_vars();
        setenv("SR_CONN_TIMEOUT", std::to_string(conn_timeout).c_str(), true);
        setenv("SR_CONN_INTERVAL", std::to_string(conn_interval).c_str(), true);
        setenv("SR_CMD_TIMEOUT", std::to_string(cmd_timeout).c_str(), true);
        setenv("SR_CMD_INTERVAL", std::to_string(cmd_interval).c_str(), true);

        if (use_cluster()) {
            RedisClusterTest redis_server;
            THEN("Environment variables are used for member variables")
            {
                CHECK(redis_server.get_connection_timeout() ==
                      conn_timeout);
                CHECK(redis_server.get_connection_interval() ==
                      conn_interval);
                CHECK(redis_server.get_connection_attempts() ==
                      expected_conn_attempts);

                CHECK(redis_server.get_command_timeout() ==
                      cmd_timeout);
                CHECK(redis_server.get_command_interval() ==
                      cmd_interval);
                CHECK(redis_server.get_command_attempts() ==
                      expected_cmd_attempts);
            }
        }
        else {
            RedisTest redis_server;
            THEN("Environment variables are used for member variables")
            {
                CHECK(redis_server.get_connection_timeout() ==
                      conn_timeout);
                CHECK(redis_server.get_connection_interval() ==
                      conn_interval);
                CHECK(redis_server.get_connection_attempts() ==
                      expected_conn_attempts);

                CHECK(redis_server.get_command_timeout() ==
                      cmd_timeout);
                CHECK(redis_server.get_command_interval() ==
                      cmd_interval);
                CHECK(redis_server.get_command_attempts() ==
                      expected_cmd_attempts);
            }
        }
    }
    GIVEN("A negative value of SR_CONN_TIMEOUT")
    {
        unset_all_env_vars();
        setenv("SR_CONN_TIMEOUT", "-1", true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
    GIVEN("A negative value of SR_CONN_INTERVAL")
    {
        unset_all_env_vars();
        setenv("SR_CONN_INTERVAL", "-2", true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
    GIVEN("A negative value of SR_CMD_TIMEOUT")
    {
        unset_all_env_vars();
        setenv("SR_CMD_TIMEOUT", "-3", true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }

    GIVEN("A negative value of SR_CMD_INTERVAL")
    {
        unset_all_env_vars();
        setenv("SR_CMD_INTERVAL", "-4", true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
    GIVEN("An environment variable that includes non-digits")
    {
        unset_all_env_vars();
        setenv("SR_CMD_INTERVAL", "425xkdfa4kd", true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
    GIVEN("An environment variable that is larger than integer storage size")
    {
        std::string env_var_str = std::to_string(INT_MAX) + "0";
        unset_all_env_vars();
        setenv("SR_CMD_INTERVAL", env_var_str.c_str(), true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
    GIVEN("An environment variable value of SR_CONN_TIMEOUT "\
          "that is too large for conversion to number of attempts")
    {
        std::string env_var_str = std::to_string(INT_MAX/1000+1);
        unset_all_env_vars();
        setenv("SR_CONN_TIMEOUT", env_var_str.c_str(), true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
    GIVEN("An environment variable value of SR_CMD_TIMEOUT "\
          "that is too large for conversion to number of attempts")
    {
        std::string env_var_str = std::to_string(INT_MAX/1000+1);
        unset_all_env_vars();
        setenv("SR_CMD_TIMEOUT", env_var_str.c_str(), true);
        THEN("Constructor throws an exception")
        {
            CHECK_THROWS_AS(invoke_constructor(), Exception);
        }
    }
}