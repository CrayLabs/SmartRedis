# BSD 2-Clause License
#
# Copyright (c) 2021-2023, Hewlett Packard Enterprise
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os

import pytest
from smartredis import *
from smartredis.error import *

#####
# Test attempts to use API functions from non-factory object

def test_non_factory_configobject():
    co = ConfigOptions()
    with pytest.raises(RedisRuntimeError):
        _ = co.get_integer_option("key", 42)
    with pytest.raises(RedisRuntimeError):
        _ = co.get_string_option("key", "default")
    with pytest.raises(RedisRuntimeError):
        _ = co.get_boolean_option("key", False)
    with pytest.raises(RedisRuntimeError):
        _ = co.is_defined("key")
    with pytest.raises(RedisRuntimeError):
        _ = co.override_integer_option("key", 42)
    with pytest.raises(RedisRuntimeError):
        _ = co.override_string_option("key", "value")
    with pytest.raises(RedisRuntimeError):
        _ = co.override_boolean_option("key", False)

def test_unimplemented():
    with pytest.raises(RedisRuntimeError):
        _ = ConfigOptions.create_from_file("some_file.json")
    with pytest.raises(RedisRuntimeError):
        _ = ConfigOptions.create_from_string("{ \"key\" = \"value\" }")

def test_options(monkeypatch):
    monkeypatch.setenv("test_integer_key", "42")
    monkeypatch.setenv("test_string_key", "charizard")
    monkeypatch.setenv("test_boolean_key", "False")
    monkeypatch.setenv("test_boolean_key_f0", "false")
    monkeypatch.setenv("test_boolean_key_f1", "FALSE")
    monkeypatch.setenv("test_boolean_key_f2", "0")
    monkeypatch.setenv("test_boolean_key_f3", "fAlse")
    monkeypatch.setenv("test_boolean_key_t0", "pikachu")
    monkeypatch.setenv("test_boolean_key_t1", "1")
    monkeypatch.setenv("test_boolean_key_t2", "fail")
    co = ConfigOptions.create_from_environment("")

    # integer option tests
    assert co.get_integer_option("test_integer_key", 0) == 42
    assert not co.is_defined("test_integer_key_that_is_not_really_present")
    assert co.get_integer_option(
        "test_integer_key_that_is_not_really_present", 0) == 0
    assert co.is_defined("test_integer_key_that_is_not_really_present")
    co.override_integer_option(
        "test_integer_key_that_is_not_really_present", 42
    )
    assert co.get_integer_option(
        "test_integer_key_that_is_not_really_present", 0) == 42

    # string option tests
    assert co.get_string_option("test_string_key", "missing") == "charizard"
    assert not co.is_defined("test_string_key_that_is_not_really_present")
    assert co.get_string_option(
        "test_string_key_that_is_not_really_present", "missing"
    ) == "missing"
    co.override_string_option(
        "test_string_key_that_is_not_really_present", "meowth"
    )
    assert co.get_string_option(
        "test_string_key_that_is_not_really_present", "missing"
    ) == "meowth"
    assert co.is_defined("test_string_key_that_is_not_really_present")

    # boolean option tests
    assert co.get_boolean_option("test_boolean_key", True) == False
    assert not co.is_defined("test_boolean_key_that_is_not_really_present")
    assert co.get_boolean_option(
        "test_boolean_key_that_is_not_really_present", True
    ) == True
    co.override_boolean_option(
        "test_boolean_key_that_is_not_really_present", True
    )
    assert co.get_boolean_option(
        "test_boolean_key_that_is_not_really_present", True
    ) == True
    assert co.is_defined("test_boolean_key_that_is_not_really_present")
    for i in range(4):
        key = f"test_boolean_key_f{i}"
        assert co.get_boolean_option(key, True) == False
    for i in range(3):
        key = f"test_boolean_key_t{i}"
        assert co.get_boolean_option(key, False) == True

def test_options_with_prefix(monkeypatch):
    monkeypatch.setenv("prefixtest_integer_key", "42")
    monkeypatch.setenv("prefixtest_string_key", "charizard")
    monkeypatch.setenv("prefixtest_boolean_key", "False")
    monkeypatch.setenv("prefixtest_boolean_key_f0", "false")
    monkeypatch.setenv("prefixtest_boolean_key_f1", "FALSE")
    monkeypatch.setenv("prefixtest_boolean_key_f2", "0")
    monkeypatch.setenv("prefixtest_boolean_key_f3", "fAlse")
    monkeypatch.setenv("prefixtest_boolean_key_t0", "pikachu")
    monkeypatch.setenv("prefixtest_boolean_key_t1", "1")
    monkeypatch.setenv("prefixtest_boolean_key_t2", "fail")
    co = ConfigOptions.create_from_environment("prefixtest")

    # integer option tests
    assert co.get_integer_option("integer_key", 0) == 42
    assert not co.is_defined("integer_key_that_is_not_really_present")
    assert co.get_integer_option(
        "integer_key_that_is_not_really_present", 0) == 0
    assert co.is_defined("integer_key_that_is_not_really_present")
    co.override_integer_option(
        "integer_key_that_is_not_really_present", 42
    )
    assert co.get_integer_option(
        "integer_key_that_is_not_really_present", 0) == 42

    # string option tests
    assert co.get_string_option("string_key", "missing") == "charizard"
    assert not co.is_defined("string_key_that_is_not_really_present")
    assert co.get_string_option(
        "string_key_that_is_not_really_present", "missing"
    ) == "missing"
    co.override_string_option(
        "string_key_that_is_not_really_present", "meowth"
    )
    assert co.get_string_option(
        "string_key_that_is_not_really_present", "missing"
    ) == "meowth"
    assert co.is_defined("string_key_that_is_not_really_present")

    # boolean option tests
    assert co.get_boolean_option("boolean_key", True) == False
    assert not co.is_defined("boolean_key_that_is_not_really_present")
    assert co.get_boolean_option(
        "boolean_key_that_is_not_really_present", True
    ) == True
    co.override_boolean_option(
        "boolean_key_that_is_not_really_present", True
    )
    assert co.get_boolean_option(
        "boolean_key_that_is_not_really_present", True
    ) == True
    assert co.is_defined("boolean_key_that_is_not_really_present")
    for i in range(4):
        key = f"boolean_key_f{i}"
        assert co.get_boolean_option(key, True) == False
    for i in range(3):
        key = f"boolean_key_t{i}"
        assert co.get_boolean_option(key, False) == True
