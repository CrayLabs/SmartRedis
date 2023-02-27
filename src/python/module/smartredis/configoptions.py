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

from .smartredisPy import PyConfigOptions
from .util import exception_handler, typecheck

from .error import *

_notfactory = "Method called on a ConfigOptions object not created from a factory method"

class ConfigOptions:
    def __init__(self):
        """Initialize a ConfigOptions base object
        """
        self._is_created_via_factory = False
        self._config_opts = None

    @staticmethod
    def from_pybind(configoptions):
        """Initialize a ConfigOptions object from
        a PyConfigOptions object

        :param configoptions: The pybind PyConfigOptions object
                              to use for construction
        :type dataset: PyConfigOptions
        :return: The newly constructed ConfigOptions from
                 the PyConfigOptions
        :rtype: ConfigOptions
        """
        typecheck(configoptions, "configoptions", PyConfigOptions)
        new_configoptions = ConfigOptions()
        new_configoptions.set_configoptions(configoptions)
        return new_configoptions

    @exception_handler
    def set_configoptions(self, configoptions):
        """Set the PyConfigOptions attribute

        :param configoptions: The PyConfigOptions object
        :type configoptions: PyConfigOptions
        """
        typecheck(configoptions, "configoptions", PyConfigOptions)
        self._config_opts = configoptions

    def _is_factory_object():
        """Check whether this object was created via a factory method
        """
        return self._is_created_via_factory

    @staticmethod
    @exception_handler
    def create_from_environment(db_prefix):
        """Instantiate ConfigOptions, getting selections from
        environment variables. If db_prefix is non-empty,
        then "{db_prefix}_" will be prepended to the name of
        each environment variable that is read

        :param db_prefix: Prefix to prepend to environment variables
                          or an empty string to eschew prepending
        :type db_prefix: str
        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        typecheck(db_prefix, "db_prefix", str)
        factory_object = PyConfigOptions.create_from_environment(db_prefix)
        result = ConfigOptions.from_pybind(factory_object)
        result._is_created_via_factory = True
        return result

    @staticmethod
    @exception_handler
    def create_from_file(filename):
        """Instantiate ConfigOptions, getting selections from
        a file containing JSON data

        :param filename: Path to file containing JSON data
        :type filename: str
        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        typecheck(filename, "filename", str)
        factory_object = PyConfigOptions.create_from_file(filename)
        result = ConfigOptions.from_pybind(factory_object)
        result._is_created_via_factory = True
        return result

    @staticmethod
    @exception_handler
    def create_from_string(json_blob):
        """Instantiate ConfigOptions, getting selections from
        a string containing a JSON blob

        :param json_blob: JSON data
        :type json_blob: str
        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        typecheck(json_blob, "json_blob", str)
        factory_object = PyConfigOptions.create_from_string(json_blob)
        result = ConfigOptions.from_pybind(factory_object)
        result._is_created_via_factory = True
        return result

    @staticmethod
    @exception_handler
    def create_from_default():
        """Instantiate ConfigOptions, getting selections from
        the current default

        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        factory_object = PyConfigOptions.create_from_default()
        result = ConfigOptions.from_pybind(factory_object)
        result._is_created_via_factory = True
        return result

    @staticmethod
    @exception_handler
    def set_default_from_environment(db_prefix):
        """Set environment variables with a particular prefix
        as the default configuration source

        :param db_prefix: The prefix to be prepended to environment
                          variables in the form {db_prefix}_{environment
                          variable}. If the prefix is an empty string,
                          no prepending is done.
        :type db_prefix: str
        """
        typecheck(db_prefix, "db_prefix", str)
        PyConfigOptions.set_default_from_environment(db_prefix)

    @exception_handler
    def set_default_from_file(filename):
        """Set a file containing JSON data as the default configuration source

        :param filename: The file containing JSON data
        :type filename: str
        :raises RedisRuntimeError: if the file cannot be accessed or contains
                                   data that cannot be parsed
        """
        typecheck(filename, "filename", str)
        PyConfigOptions.set_default_from_file(filename)

    @exception_handler
    def set_default_from_string(string):
        """Set a string containing a JSON blob as the default
        configuration source

        :param string: The string containing a JSON blob
        :type string: str
        :raises RedisRuntimeError: if the string cannot be parsed
        """
        typecheck(string, "string", str)
        PyConfigOptions.set_default_from_string(string)

    @exception_handler
    def get_integer_option(self, key, default_value):
        """Retrieve the value of a numeric configuration option
        from the selected source

        :param key: The name of the configuration option to retrieve
        :type key: str
        :param default_value: The baseline value of the configuration
                              option to be returned if a value was not
                              set in the selected source
        :return: The value of the selected option. Returns
                 default_value if the option was not set in the
                 selected source
        :rtype: int
        """
        typecheck(key, "key", str)
        typecheck(default_value, "default_value", int)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        return self._config_opts.get_integer_option(key, default_value)

    @exception_handler
    def get_string_option(self, key, default_value):
        """Retrieve the value of a string configuration option
        from the selected source

        :param key: The name of the configuration option to retrieve
        :type key: str
        :param default_value: The baseline value of the configuration
                              option to be returned if a value was not
                              set in the selected source
        :return: The value of the selected option. Returns
                 default_value if the option was not set in the
                 selected source
        :rtype: str
        """
        typecheck(key, "key", str)
        typecheck(default_value, "default_value", str)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        return self._config_opts.get_string_option(key, default_value)

    @exception_handler
    def get_boolean_option(self, key, default_value):
        """Retrieve the value of a boolean configuration option
        from the selected source

        :param key: The name of the configuration option to retrieve
        :type key: str
        :param default_value: The baseline value of the configuration
                              option to be returned if a value was not
                              set in the selected source
        :return: The value of the selected option. Returns
                 default_value if the option was not set in the
                 selected source
        :rtype: bool
        """
        typecheck(key, "key", str)
        typecheck(default_value, "default_value", bool)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        return self._config_opts.get_boolean_option(key, default_value)

    @exception_handler
    def is_defined(self, key):
        """Check whether a configuration option is set in the selected source

        :param key: The name of the configuration option to check
        :type key: str
        :return: True IFF the target option is defined in the selected source
                 or if it has been overridden
        :rtype: bool
        """
        typecheck(key, "key", str)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        return self._config_opts.is_defined(key)

    @exception_handler
    def override_integer_option(self, key, value):
        """Override the value of a numeric configuration option
        in the selected source

        Overrides are specific to an instance of the
        ConfigOptions class. An instance that references
        the same source will not be affected by an override to
        a different ConfigOptions instance

        :param key: The name of the configuration option to override
        :type key: str
        :param value: The value to store for the configuration option
        :type value: int
        """
        typecheck(key, "key", str)
        typecheck(value, "value", int)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        self._config_opts.override_integer_option(key, value)

    @exception_handler
    def override_string_option(self, key, value):
        """Override the value of a string configuration option
        in the selected source

        Overrides are specific to an instance of the
        ConfigOptions class. An instance that references
        the same source will not be affected by an override to
        a different ConfigOptions instance

        :param key: The name of the configuration option to override
        :type key: str
        :param value: The value to store for the configuration option
        :type value: str
        """
        typecheck(key, "key", str)
        typecheck(value, "value", str)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        self._config_opts.override_string_option(key, value)

    @exception_handler
    def override_boolean_option(self, key, value):
        """Override the value of a boolean configuration option
        in the selected source

        Overrides are specific to an instance of the
        ConfigOptions class. An instance that references
        the same source will not be affected by an override to
        a different ConfigOptions instance

        :param key: The name of the configuration option to override
        :type key: str
        :param value: The value to store for the configuration option
        :type value: bool
        """
        typecheck(key, "key", str)
        typecheck(value, "value", bool)
        if not self._is_created_via_factory:
            raise RedisRuntimeError(_notfactory) from None
        self._config_opts.override_boolean_option(key, value)

