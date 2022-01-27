# BSD 2-Clause License
#
# Copyright (c) 2021, Hewlett Packard Enterprise
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

import inspect
import os
import os.path as osp
import functools

import numpy as np

from .dataset import Dataset
from .smartredisPy import PyClient
from .util import Dtypes, init_default, exception_handler

from .error import *
from .smartredisPy import RedisReplyError as RRE

class Client(PyClient):
    def __init__(self, address=None, cluster=False):
        """Initialize a RedisAI client

        For clusters, the address can be a single tcp/ip address and port
        of a database node. The rest of the cluster will be discovered
        by the client itself. (e.g. address="127.0.0.1:6379")

        If an address is not set, the client will look for the environment
        variable ``SSDB`` (e.g. SSDB="127.0.0.1:6379;")

        :param address: Address of the database
        :param cluster: True if connecting to a redis cluster, defaults to False
        :type cluster: bool, optional
        :raises RedisConnectionError: if connection initialization fails
        """
        if address:
            self.__set_address(address)
        if "SSDB" not in os.environ:
            raise RedisConnectionError("Could not connect to database. $SSDB not set")
        try:
            super().__init__(cluster)
        except RRE as e:
            raise RedisConnectionError(str(e)) from None
        except RuntimeError as e:
            raise RedisConnectionError(str(e)) from None

    @exception_handler
    def put_tensor(self, key, data):
        """Put a tensor to a Redis database

        :param key: key for tensor for be stored at
        :type key: str
        :param data: numpy array
        :type data: np.array
        :raises RedisReplyError: if put fails
        """
        if not isinstance(data, np.ndarray):
            raise TypeError("Argument provided was not a numpy array")
        dtype = Dtypes.tensor_from_numpy(data)
        super().put_tensor(key, dtype, data)

    @exception_handler
    def get_tensor(self, key):
        """Get a tensor from the database

        :param key: key to get tensor from
        :type key: str
        :raises RedisReplyError: if get fails
        :return: numpy array
        :rtype: np.array
        """
        return super().get_tensor(key)

    @exception_handler
    def delete_tensor(self, key):
        """Delete a tensor within the database

        :param key: key tensor is stored at
        :type key: str
        :raises RedisReplyError: if deletion fails
        """
        super().delete_tensor(key)

    @exception_handler
    def copy_tensor(self, key, dest_key):
        """Copy a tensor at one key to another key

        :param key: key of tensor to be copied
        :type key: str
        :param dest_key: key to store new copy at
        :type dest_key: str
        :raises RedisReplyError: if copy operation fails
        """
        super().copy_tensor(key, dest_key)

    @exception_handler
    def rename_tensor(self, key, new_key):
        """Rename a tensor in the database

        :param key: key of tensor to be renamed
        :type key: str
        :param new_key: new name for the tensor
        :type new_key: str
        :raises RedisReplyError: if rename operation fails
        """
        super().rename_tensor(key, new_key)

    @exception_handler
    def put_dataset(self, dataset):
        """Put a Dataset instance into the database

        All associated tensors and metadata within the Dataset
        instance will also be stored.

        :param dataset: a Dataset instance
        :type dataset: Dataset
        :raises TypeError: if argument is not a Dataset
        :raises RedisReplyError: if connection fails
        """
        if not isinstance(dataset, Dataset):
            raise TypeError("Argument to put_dataset was not of type Dataset")
        pybind_dataset = dataset.get_data()
        super().put_dataset(pybind_dataset)

    @exception_handler
    def get_dataset(self, key):
        """Get a dataset from the database

        :param key: key the dataset is stored under
        :type key: str
        :raises RedisConnectionError: if connection fails
        :return: Dataset instance
        :rtype: Dataset
        """
        dataset = super().get_dataset(key)
        python_dataset = Dataset.from_pybind(dataset)
        return python_dataset

    @exception_handler
    def delete_dataset(self, key):
        """Delete a dataset within the database

        :param key: name of the dataset
        :type key: str
        :raises RedisReplyError: if deletion fails
        """
        super().delete_dataset(key)

    @exception_handler
    def copy_dataset(self, key, dest_key):
        """Copy a dataset from one key to another

        :param key: name of dataset to be copied
        :type key: str
        :param dest_key: new name of dataset
        :type dest_key: str
        :raises RedisReplyError: if copy operation fails
        """
        super().copy_dataset(key, dest_key)

    @exception_handler
    def rename_dataset(self, key, new_key):
        """Rename a dataset in the database

        :param key: name of the dataset to be renamed
        :type key: str
        :param new_key: new name for the dataset
        :type new_key: str
        :raises RedisReplyError: if rename operation fails
        """
        super().rename_dataset(key, new_key)

    @exception_handler
    def set_function(self, key, function, device="CPU"):
        """Set a callable function into the database

        Function must be a callable TorchScript function and have at least
        one input and one output. Call the function with the Client.run_script
        method.
        Device selection is either "GPU" or "CPU". If many devices are
        present, a number can be passed for specification e.g. "GPU:1".

        :param key: key to store function at
        :type key: str
        :param function: callable function
        :type function: callable
        :param device: device to run function on, defaults to "CPU"
        :type device: str, optional
        :raises TypeError: if argument was not a callable function
        :raises RedisReplyError: if function failed to set
        """
        device = self.__check_device(device)
        if not callable(function):
            raise TypeError("Argument provided was not a callable function")
        fn_src = inspect.getsource(function)
        super().set_script(key, device, fn_src)

    @exception_handler
    def set_script(self, key, script, device="CPU"):
        """Store a TorchScript at key in the database

        Device selection is either "GPU" or "CPU". If many devices are
        present, a number can be passed for specification e.g. "GPU:1".

        :param key: key to store script under
        :type key: str
        :param script: TorchScript code
        :type script: str
        :param device: device for script execution, defaults to "CPU"
        :type device: str, optional
        :raises RedisReplyError: if script fails to set
        """
        device = self.__check_device(device)
        super().set_script(key, device, script)

    @exception_handler
    def set_script_from_file(self, key, file, device="CPU"):
        """Same as Client.set_script but from file

        :param key: key to store script under
        :type key: str
        :param file: path to TorchScript code
        :type file: str
        :param device: device for script execution, defaults to "CPU"
        :type device: str, optional
        :raises RedisReplyError: if script fails to set
        """
        device = self.__check_device(device)
        file_path = self.__check_file(file)
        super().set_script_from_file(key, device, file_path)

    @exception_handler
    def get_script(self, key):
        """Get a Torchscript stored in the database

        :param key: key at which script is stored
        :type key: str
        :raises RedisReplyError: if script doesn't exist
        :return: TorchScript stored at key
        :rtype: str
        """
        script = super().get_script(key)
        return script

    @exception_handler
    def run_script(self, key, fn_name, inputs, outputs):
        """Execute TorchScript stored inside the database remotely

        :param key: key script is stored under
        :type key: str
        :param fn_name: name of the function within the script to execute
        :type fn_name: str
        :param inputs: list of input tensors stored in database
        :type inputs: list[str]
        :param outputs: list of output tensor names to store results under
        :type outputs: list[str]
        :raises RedisReplyError: if script execution fails
        """
        inputs, outputs = self.__check_tensor_args(inputs, outputs)
        super().run_script(key, fn_name, inputs, outputs)

    @exception_handler
    def get_model(self, key):
        """Get a stored model

        :param key: key of stored model
        :type key: str
        :raises RedisReplyError: if get fails or model doesnt exist
        :return: model
        :rtype: bytes
        """
        model = super().get_model(key)
        return model

    @exception_handler
    def set_model(
        self,
        key,
        model,
        backend,
        device="CPU",
        batch_size=0,
        min_batch_size=0,
        tag="",
        inputs=None,
        outputs=None,
    ):
        """Put a TF, TF-lite, PT, or ONNX model in the database

        :param key: key to store model under
        :type key: str
        :param model: serialized model
        :type model: bytes
        :param backend: name of the backend (TORCH, TF, TFLITE, ONNX)
        :type backend: str
        :param device: name of device for execution, defaults to "CPU"
        :type device: str, optional
        :param batch_size: batch size for execution, defaults to 0
        :type batch_size: int, optional
        :param min_batch_size: minimum batch size for model execution, defaults to 0
        :type min_batch_size: int, optional
        :param tag: additional tag for model information, defaults to ""
        :type tag: str, optional
        :param inputs: model inputs (TF only), defaults to None
        :type inputs: list[str], optional
        :param outputs: model outupts (TF only), defaults to None
        :type outputs: list[str], optional
        :raises RedisReplyError: if model fails to set
        """
        device = self.__check_device(device)
        backend = self.__check_backend(backend)
        inputs, outputs = self.__check_tensor_args(inputs, outputs)
        super().set_model(
            key,
            model,
            backend,
            device,
            batch_size,
            min_batch_size,
            tag,
            inputs,
            outputs,
        )

    @exception_handler
    def set_model_from_file(
        self,
        key,
        model_file,
        backend,
        device="CPU",
        batch_size=0,
        min_batch_size=0,
        tag="",
        inputs=None,
        outputs=None,
    ):
        """Put a TF, TF-lite, PT, or ONNX model from file in the database

        :param key: key to store model under
        :type key: str
        :param model_file: serialized model
        :type model_file: file path to model
        :param backend: name of the backend (TORCH, TF, TFLITE, ONNX)
        :type backend: str
        :param device: name of device for execution, defaults to "CPU"
        :type device: str, optional
        :param batch_size: batch size for execution, defaults to 0
        :type batch_size: int, optional
        :param min_batch_size: minimum batch size for model execution, defaults to 0
        :type min_batch_size: int, optional
        :param tag: additional tag for model information, defaults to ""
        :type tag: str, optional
        :param inputs: model inputs (TF only), defaults to None
        :type inputs: list[str], optional
        :param outputs: model outupts (TF only), defaults to None
        :type outputs: list[str], optional
        :raises RedisReplyError: if model fails to set
        """
        device = self.__check_device(device)
        backend = self.__check_backend(backend)
        m_file = self.__check_file(model_file)
        inputs, outputs = self.__check_tensor_args(inputs, outputs)
        super().set_model_from_file(
            key,
            m_file,
            backend,
            device,
            batch_size,
            min_batch_size,
            tag,
            inputs,
            outputs,
        )

    @exception_handler
    def run_model(self, key, inputs=None, outputs=None):
        """Execute a stored model

        :param key: key for stored model
        :type key: str
        :param inputs: keys of stored inputs to provide model, defaults to None
        :type inputs: list[str], optional
        :param outputs: keys to store outputs under, defaults to None
        :type outputs: list[str], optional
        :raises RedisReplyError: if model execution fails
        """
        inputs, outputs = self.__check_tensor_args(inputs, outputs)
        super().run_model(key, inputs, outputs)

    @exception_handler
    def tensor_exists(self, name):
        """Check if a tensor exists in the database

        The key associated to the entity will be
        computed internally based on the current prefix behavior.

        :param key: The tensor name that will be checked in the database
        :type key: str
        :returns: Returns true if the tensor exists in the database
        :rtype: bool
        :raises RedisReplyError: if `tensor_exists` fails (i.e. causes an error)
        """
        return super().tensor_exists(name)

    @exception_handler
    def dataset_exists(self, name):
        """Check if a dataset exists in the database

        The key associated to the entity will be
        computed internally based on the current prefix behavior.

        :param key: The dataset name that will be checked in the database
        :type key: str
        :returns: Returns true if the dataset exists in the database
        :rtype: bool
        :raises RedisReplyError: if `dataset_exists` fails (i.e. causes an error)
        """
        return super().dataset_exists(name)

    @exception_handler
    def model_exists(self, name):
        """Check if a model or script exists in the database

        The key associated to the entity will be
        computed internally based on the current prefix behavior.

        :param key: The model or script name that will be checked in the database
        :type key: str
        :returns: Returns true if the model exists in the database
        :rtype: bool
        :raises RedisReplyError: if `model_exists` fails (i.e. causes an error)
        """
        return super().model_exists(name)

    @exception_handler
    def key_exists(self, key):
        """Check if the key exists in the database

        :param key: The key that will be checked in the database
        :type key: str
        :returns: Returns true if the key exists in the database
        :rtype: bool
        :raises RedisReplyError: if `key_exists` fails
        """
        return super().key_exists(key)

    @exception_handler
    def poll_key(self, key, poll_frequency_ms, num_tries):
        """Check if the key exists in the database

        The check is performed repeatedly at a
        specified frequency for a specified number
        of times.

        :param key: The key that will be checked in the database
        :type key: int
        :param poll_frequency_ms: The frequency of checks for the
                                  key in milliseconds
        :type poll_frequency_ms: int
        :param num_tries: The total number of times to check for
                          the specified number of keys.
        :type num_tries: int
        :returns: Returns true if the key is found within the
                  specified number of tries, otherwise false.
        :rtype: bool
        :raises RedisReplyError: if key poll fails
        """
        return super().poll_key(key, poll_frequency_ms, num_tries)

    @exception_handler
    def poll_tensor(self, name, poll_frequency_ms, num_tries):
        """Check if a tensor exists in the database

        The check will be performed at a
        specified frequency for a specified number
        of times. The key associated to the entity will be
        computed internally based on the current prefix behavior.

        :param key: The key that will be checked in the database
        :type key: int
        :param poll_frequency_ms: The frequency of checks for the
                                  key in milliseconds
        :type poll_frequency_ms: int
        :param num_tries: The total number of times to check for
                          the specified number of keys.
        :type num_tries: int
        :returns: Returns true if the key is found within the
                  specified number of tries, otherwise false.
        :rtype: bool
        :raises RedisReplyError: if `poll_tensor` fails
        """
        return super().poll_tensor(name, poll_frequency_ms, num_tries)

    @exception_handler
    def poll_dataset(self, name, poll_frequency_ms, num_tries):
        """Check if a dataset exists in the database

        The check will be performed at a
        specified frequency for a specified number
        of times. The key associated to the entity will be
        computed internally based on the current prefix behavior.

        :param key: The key that will be checked in the database
        :type key: int
        :param poll_frequency_ms: The frequency of checks for the
                                  key in milliseconds
        :type poll_frequency_ms: int
        :param num_tries: The total number of times to check for
                          the specified number of keys.
        :type num_tries: int
        :returns: Returns true if the key is found within the
                  specified number of tries, otherwise false.
        :rtype: bool
        :raises RedisReplyError: if `poll_dataset` fails
        """
        return super().poll_dataset(name, poll_frequency_ms, num_tries)

    @exception_handler
    def poll_model(self, name, poll_frequency_ms, num_tries):
        """Check if a model or script exists in the database

        The check will be performed at a
        specified frequency for a specified number
        of times. The key associated to the entity will be
        computed internally based on the current prefix behavior.

        :param key: The key that will be checked in the database
        :type key: int
        :param poll_frequency_ms: The frequency of checks for the
                                  key in milliseconds
        :type poll_frequency_ms: int
        :param num_tries: The total number of times to check for
                          the specified number of keys.
        :type num_tries: int
        :returns: Returns true if the key is found within the
                  specified number of tries, otherwise false.
        :rtype: bool
        :raises RedisReplyError: if `poll_model` fails
        """
        return super().poll_model(name, poll_frequency_ms, num_tries)

    @exception_handler
    def set_data_source(self, source_id):
        """Set the data source (i.e. key prefix for get functions)

        :param source_id: The prefix for retrieval commands
        :type source_id: str
        :raises RedisReplyError: if set data
        """
        return super().set_data_source(source_id)

    @exception_handler
    def use_model_ensemble_prefix(self, use_prefix):
        """Set whether model and script keys should be prefixed

        This function can be used to avoid key collisions in an ensemble.
        Prefixes will only be used if they were previously set through
        environment variables SSKEYIN and SSKEYOUT.
        By default, the client does not prefix model and script
        keys.

        :param use_prefix: If set to true, all future operations
                           on models and scripts will use a prefix, if
                           available.
        :type use_prefix: bool
        """
        return super().use_model_ensemble_prefix(use_prefix)

    @exception_handler
    def use_tensor_ensemble_prefix(self, use_prefix):
        """Set whether tensor and dataset keys should be prefixed

        This function can be used to avoid key collisions in an ensemble.
        Prefixes will only be used if they were previously set through
        environment variables SSKEYIN and SSKEYOUT.
        By default, the client prefixes tensor and dataset
        keys when a prefix is available.

        :param use_prefix: If set to true, all future operations
                           on tensors and datasets will use a prefix, if
                           available.
        :type use_prefix: bool
        """
        return super().use_tensor_ensemble_prefix(use_prefix)

    @exception_handler
    def get_db_node_info(self, addresses):
        """Returns information about given database nodes

        :param addresses: The addresses of the database nodes
        :type address: list[str]
        :returns: A list of dictionaries with each entry in the
                  list corresponding to an address reply
        :rtype: list[dict]
        :raises RedisReplyError: if there is an error
                in command execution or the address
                is not reachable by the client.
                In the case of using a cluster of database nodes,
                it is best practice to bind each node in the cluster
                to a specific address to avoid inconsistencies in
                addresses retrieved with the CLUSTER SLOTS command.
                Inconsistencies in node addresses across
                CLUSTER SLOTS commands will lead to RedisReplyError
                being thrown.
        """
        return super().get_db_node_info(addresses)

    @exception_handler
    def get_db_cluster_info(self, addresses):
        """Returns cluster information from a specified db node.
        If the address does not correspond to a cluster node,
        an empty dictionary is returned.

        :param addresses: The addresses of the database nodes
        :type address: list[str]
        :returns: A list of dictionaries with each entry in the
                  list corresponding to an address reply
        :rtype: list[dict]
        :raises RedisReplyError: if there is an error
                in command execution or the address
                is not reachable by the client or if on a
                non-cluster environment.
                In the case of using a cluster of database nodes,
                it is best practice to bind each node in the cluster
                to a specific address to avoid inconsistencies in
                addresses retrieved with the CLUSTER SLOTS command.
                Inconsistencies in node addresses across
                CLUSTER SLOTS commands will lead to RedisReplyError
                being thrown.
        """
        return super().get_db_cluster_info(addresses)

    @exception_handler
    def flush_db(self, addresses):
        """Removes all keys from a specified db node.

        :param addresses: The addresses of the database nodes
        :type address: list[str]
        :raises RedisReplyError: if there is an error
                in command execution or the address
                is not reachable by the client.
                In the case of using a cluster of database nodes,
                it is best practice to bind each node in the cluster
                to a specific address to avoid inconsistencies in
                addresses retrieved with the CLUSTER SLOTS command.
                Inconsistencies in node addresses across
                CLUSTER SLOTS commands will lead to RedisReplyError
                being thrown.
        """
        super().flush_db(addresses)

    @exception_handler
    def config_get(self, expression, address):
        """Read the configuration parameters of a running server.
        If the address does not correspond to a cluster node,
        an empty dictionary is returned.

        :param expression: Parameter used in the configuration or a
                           glob pattern (Use '*' to retrieve all
                           configuration parameters)
        :type expression: str
        :param address: The address of the database node
        :type address: str
        :returns: A dictionary that maps configuration parameters to
                  their values. If the provided expression does not
                  exist, then an empty dictionary is returned.
        :rtype: dict
        :raises RedisReplyError: if there is an error
                in command execution or the address
                is not reachable by the client.
                In the case of using a cluster of database nodes,
                it is best practice to bind each node in the cluster
                to a specific address to avoid inconsistencies in
                addresses retrieved with the CLUSTER SLOTS command.
                Inconsistencies in node addresses across
                CLUSTER SLOTS commands will lead to RedisReplyError
                being thrown.
        """
        return super().config_get(expression, address)

    @exception_handler
    def config_set(self, config_param, value, address):
        """Reconfigure the server. It can change both trivial
        parameters or switch from one to another persistence option.
        All the configuration parameters set using this command are
        immediately loaded by Redis and will take effect starting with
        the next command executed.
        If the address does not correspond to a cluster node,
        an empty dictionary is returned.

        :param config_param: A configuration parameter to set
        :type config_param: str
        :param value: The value to assign to the configuration parameter
        :type value: str
        :param address: The address of the database node
        :type address: str
        :raises RedisReplyError: if there is an error
                in command execution or the address
                is not reachable by the client or if the config_param
                is unsupported. In the case of using a cluster of
                database nodes, it is best practice to bind each node
                in the cluster to a specific address to avoid inconsistencies
                in addresses retrieved with the CLUSTER SLOTS command.
                Inconsistencies in node addresses across
                CLUSTER SLOTS commands will lead to RedisReplyError
                being thrown.
        """
        super().config_set(config_param, value, address)

    @exception_handler
    def save(self, addresses):
        """Performs a synchronous save of the database shard
        producinga point in time snapshot of all the data
        inside the Redis instance, in the form of an RBD file.

        :param addresses: The addresses of the database nodes
        :type addresses: list[str]
        :raises RedisReplyError: if there is an error
                in command execution or the address
                is not reachable by the client.
                In the case of using a cluster of database nodes,
                it is best practice to bind each node in the cluster
                to a specific address to avoid inconsistencies in
                addresses retrieved with the CLUSTER SLOTS command.
                Inconsistencies in node addresses across
                CLUSTER SLOTS commands will lead to RedisReplyError
                being thrown.
        """
        super().save(addresses)

    # ---- helpers --------------------------------------------------------

    @staticmethod
    def __check_tensor_args(inputs, outputs):
        inputs = init_default([], inputs, (list, str))
        outputs = init_default([], outputs, (list, str))
        if isinstance(inputs, str):
            inputs = [inputs]
        if isinstance(outputs, str):
            outputs = [outputs]
        return inputs, outputs

    @staticmethod
    def __check_backend(backend):
        backend = backend.upper()
        if backend in ["TF", "TFLITE", "TORCH", "ONNX"]:
            return backend
        else:
            raise TypeError(f"Backend type {backend} unsupported")

    @staticmethod
    def __check_file(file):
        file_path = osp.abspath(file)
        if not osp.isfile(file_path):
            raise FileNotFoundError(file_path)
        return file_path

    @staticmethod
    def __check_device(device):
        device = device.upper()
        if not device.startswith("CPU") and not device.startswith("GPU"):
            raise TypeError("Device argument must start with either CPU or GPU")
        return device

    @staticmethod
    def __set_address(address):
        if "SSDB" in os.environ:
            del os.environ["SSDB"]
        os.environ["SSDB"] = address
