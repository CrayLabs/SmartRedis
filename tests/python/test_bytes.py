# BSD 2-Clause License
#
# Copyright (c) 2021-2024, Hewlett Packard Enterprise
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

import io

import numpy as np
from smartredis import Client

# ----- Tests -----------------------------------------------------------


def test_bytes_exist(mock_data, context):
    """Test that raw bytes exist in the database
    after a put operation"""

    # Create a client
    client = Client(None, logger_name=context)

    # Initialize test data
    data = np.random.rand(2, 8, 4, 2, 30)
    bytes = io.BytesIO(data.tobytes())

    # Put data as raw bytes
    client.put_bytes("python_bytes_exist", bytes)

    # Assert that the bytes are present in the database
    assert client.bytes_exists("python_bytes_exist")


def test_put_get_bytes(mock_data, context):
    """Test put/get_bytes for byte array"""

    # Create a client
    client = Client(None, logger_name=context)

    # Initialize test data
    data = np.random.rand(2, 8, 4, 2, 30)
    bytes = io.BytesIO(data.tobytes())

    # Put data as raw bytes
    client.put_bytes("python_put_get_bytes", bytes)

    # Retrieve the bytes
    retrieved_bytes = client.get_bytes("python_put_get_bytes")

    # Assert equal values
    assert bytes.getvalue() == retrieved_bytes.getvalue()


def test_poll_bytes(mock_data, context):
    """Test that raw bytes are polled correctly"""

    # Create a client
    client = Client(None, logger_name=context)

    # Assert that the data does not exist before being put
    assert client.poll_bytes("python_bytes_poll", 1, 5) == False

    # Inititalize test data
    data = np.random.rand(2, 8, 4, 2, 30)
    bytes = io.BytesIO(data.tobytes())

    # Put data as raw bytes
    client.put_bytes("python_bytes_poll", bytes)

    # Assert polling returns true after put
    assert client.poll_bytes("python_bytes_poll", 1, 5) == True


def test_delete_bytes(mock_data, context):
    """Test that raw bytes can be removed from
    the database after a put operation"""

    # Create a client
    client = Client(None, logger_name=context)

    # Inititalize test data
    data = np.random.rand(2, 8, 4, 2, 30)
    bytes = io.BytesIO(data.tobytes())

    # Put data as raw bytes
    client.put_bytes("python_bytes_deleted", bytes)

    # Assert that the bytes exist after put
    assert client.bytes_exists("python_bytes_deleted")

    # Delete the bytes from the database
    client.delete_bytes("python_bytes_deleted")

    # Assert the bytes no longer exist
    assert not client.bytes_exists("python_bytes_deleted")
