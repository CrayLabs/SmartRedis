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

from .smartredisPy import PyDataset
from .util import Dtypes, exception_handler, typecheck

from .error import *

class ConfigOptions:
    def __init__(self):
        """Initialize a ConfigOptions base object
        """
        self._is_created_via_factory = False

    def is_factory_object():
        """Check whether this object was created via a factory method
        """
        return self._is_created_via_factory

    @staticmethod
    def create_from_environment(db_prefix):
        """Instantiate ConfigOptions, getting selections from
        environment variables. If \p db_prefix is non-empty,
        then "{db_prefix}_" will be prepended to the name of
        each environment variable that is read

        :param db_prefix: Prefix to prepend to environment variables
                          or an empty string to eschew prepending
        :type db_prefix: str
        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        typecheck(db_prefix, "db_prefix", str)
        fco = _FactoryConfigOptions()
        return fco.create_from_environment(db_prefix)

    @staticmethod
    def create_from_file(filename):
        """Instantiate ConfigOptions, getting selections from
        a file containing JSON data

        :param filename: Path to file containing JSON data
        :type filename: str
        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        typecheck(filename, "filename", str)
        fco = _FactoryConfigOptions()
        return fco.create_from_file(filename)

    @staticmethod
    def create_from_string(json_blob):
        """Instantiate ConfigOptions, getting selections from
        a string containing a JSON blob

        :param json_blob: JSON data
        :type json_blob: str
        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        typecheck(json_blob, "json_blob", str)
        fco = _FactoryConfigOptions()
        return fco.create_from_string(json_blob)

    @staticmethod
    def create_from_default():
        """Instantiate ConfigOptions, getting selections from
        the current default

        :return: An instantiated ConfigOptions object
        :rtype: ConfigOptions
        """
        fco = _FactoryConfigOptions()
        return fco.create_from_default()

class _FactoryConfigOptions(ConfigOptions):
    def __init__(self):
        self._is_created_via_factory = True
        self._config_opts = PyConfigOptions()

