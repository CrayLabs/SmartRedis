# BSD 2-Clause License
#
# Copyright (c) 2021-2022, Hewlett Packard Enterprise
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

from .smartredisPy import PySRObject
from .util import exception_handler, typecheck

from .error import *

class SRObject:
    def __init__(self, context):
        """Initialize a SRObject object

        :param context: logging context
        :type name: str
        """
        typecheck(context, "context", str)
        self._name = context
        self._data = PySRObject(context)


    @staticmethod
    def from_pybind(srobject):
        """Initialize a SRObject object from
        a PySRObject object

        :param srobject: The pybind PySRObject object
                           to use for construction
        :type srobject: PySRObject
        :return: The newly constructor SRObject from
                 the PySRObject
        :rtype: SRObject
        """
        typecheck(srobject, "srobject", PySRObject)
        new_srobject = SRObject(srobject._name)
        new_srobject.set_data(srobject)
        return new_srobject

    @exception_handler
    def get_data(self):
        """Return the PySRObject attribute

        :return: The PySRObject attribute containing
                 the srobject information
        :rtype: PySRObject
        """
        return self._data

    @exception_handler
    def set_data(self, srobject):
        """Set the PySRObject attribute

        :param srobject: The PySRObject object
        :type srobject: PySRObject
        """
        typecheck(srobject, "srobject", PySRObject)
        self._data = srobject
