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

import numpy as np

from smartredis import Client, Dataset

def test_prefixing(context, monkeypatch):
    # configure prefix variables
    monkeypatch.setenv("SSKEYOUT", "prefix_test")
    monkeypatch.setenv("SSKEYIN", "prefix_test")

    # Set up client
    c = Client(address=None, logger_name=context)
    c.use_dataset_ensemble_prefix(True)
    c.use_tensor_ensemble_prefix(True)
    d = Dataset("test_dataset")
    data = np.uint16([1, 2, 3, 4])
    d.add_tensor("dataset_tensor", data)
    c.put_dataset(d)
    c.put_tensor("test_tensor", data)

    # Validate keys to see whether prefixing was applied properly
    assert c.dataset_exists("test_dataset")
    assert c.key_exists("prefix_test.{test_dataset}.meta")
    assert not c.key_exists("test_dataset")
    assert c.tensor_exists("test_tensor")
    assert c.key_exists("prefix_test.test_tensor")
    assert not c.key_exists("test_tensor")
