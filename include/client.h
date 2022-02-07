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

#ifndef SMARTREDIS_CPP_CLIENT_H
#define SMARTREDIS_CPP_CLIENT_H
#ifdef __cplusplus
#include "string.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include "redisserver.h"
#include "rediscluster.h"
#include "redis.h"
#include "dataset.h"
#include "sharedmemorylist.h"
#include "command.h"
#include "commandlist.h"
#include "commandreply.h"
#include "tensorbase.h"
#include "tensor.h"
#include "sr_enums.h"

///@file

namespace SmartRedis {

class Client;

typedef redisReply ReplyElem;

///@file
/*!
*   \brief The Client class is the primary user-facing
*          class for executing server commands.
*/
class Client
{

    public:

        /*!
        *   \brief Client constructor
        *   \param cluster Flag for if a database cluster is being used
        *   \throw SmartRedis::Exception if client connection or
        *          object initialization fails
        */
        Client(bool cluster);

        /*!
        *   \brief Client copy constructor is not available
        */
        Client(const Client& client) = delete;

        /*!
        *   \brief Client move constructor
        */
        Client(Client&& client) = default;

        /*!
        *   \brief Client copy assignment operator
        *          is not available.
        */
        Client& operator=(const Client& client) = delete;

        /*!
        *   \brief Client move assignment operator
        */
        Client& operator=(Client&& client) = default;

        /*!
        *   \brief Client destructor
        */
        ~Client();

        /*!
        *   \brief Send a DataSet object to the database
        *   \details The final dataset key under which the dataset is stored
        *            is generated from the name that was supplied when the
        *            dataset was created and may be prefixed. See
        *            set_data_source() and use_tensor_ensemble_prefix()
        *            for more details.
        *   \param dataset The DataSet object to send to the database
        *   \throw SmartRedis::Exception if put dataset command fails
        */
        void put_dataset(DataSet& dataset);

        /*!
        *   \brief Get a DataSet object from the database
        *   \details The dataset key used to locate the dataset
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The name of the dataset to retrieve
        *   \returns DataSet object retrieved from the database
        *   \throw SmartRedis::Exception if get dataset command fails
        */
        DataSet get_dataset(const std::string& name);

        /*!
        *   \brief Move a dataset to a new dataset key.  All tensors
        *          and metdata in the dataset will be moved with it.
        *   \details The dataset key used to locate the dataset to be
        *            renamed may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The original dataset key for the dataset
        *   \param new_name The new dataset key for the dataset
        *   \throw SmartRedis::Exception if dataset rename command fails
        */
        void rename_dataset(const std::string& name,
                            const std::string& new_name);

        /*!
        *   \brief Copy a dataset to a new dataset key in the database.
        *          All tensors and metadata in the DataSet will
        *          be copied as well.
        *   \details The dataset key used to locate the dataset to be
        *            copied may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param src_name The source dataset key
        *   \param dest_name The destination dataset key
        *   \throw SmartRedis::Exception if copy dataset command fails
        */
        void copy_dataset(const std::string& src_name,
                          const std::string& dest_name);


        /*!
        *   \brief Delete a dataset from the database.  All
        *          tensors and metdata in the dataset will be
        *          deleted.
        *   \details The dataset key used to locate the dataset to be
        *            deleted may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The dataset key for the dataset to be deleted.
        *   \throw SmartRedis::Exception if delete dataset command fails
        */
        void delete_dataset(const std::string& name);

        /*!
        *   \brief Put a tensor into the database
        *   \details The final tensor key under which the tensor is stored
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param key The tensor key for this tensor in the database
        *   \param data The data for this tensor
        *   \param dims The number of elements for each dimension
        *          of the tensor
        *   \param type The data type for the tensor
        *   \param mem_layout The memory layout of the provided tensor data
        *   \throw SmartRedis::Exception if put tensor command fails
        */
        void put_tensor(const std::string& key,
                        void* data,
                        const std::vector<size_t>& dims,
                        const SRTensorType type,
                        const SRMemoryLayout mem_layout);

        /*!
        *   \brief Retrieve the tensor data, dimensions, and type for the
        *          provided tensor key. This function will allocate and retain
        *          management of the memory for the tensor data.
        *   \details The tensor key under used to locate the tensor
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *
        *            The memory of the data pointer is valid
        *            until the Client is destroyed. This method
        *            is meant to be used when the dimensions and
        *            type of the tensor are unknown or the user does
        *            not want to manage memory.  However, given that
        *            the memory associated with the return data is
        *            valid until Client destruction, this method
        *            should not be used repeatedly for large tensor
        *            data.  Instead  it is recommended that the user
        *            use unpack_tensor() for large tensor data and
        *            to limit memory use by the Client.
        *   \param key  The tensor key for the tensor
        *   \param data Receives tensor data
        *   \param dims Receives the number of elements in each dimension
        *               of the tensor data
        *   \param type Receives the type for the tensor data
        *   \param mem_layout The memory layout into which tensor
        *                     data should be written
        *   \throw SmartRedis::Exception if get tensor command fails
        */
        void get_tensor(const std::string& key,
                        void*& data,
                        std::vector<size_t>& dims,
                        SRTensorType& type,
                        const SRMemoryLayout mem_layout);


        /*!
        *   \brief Retrieve the tensor data, dimensions, and type for the
        *          provided tensor key. This function will allocate and retain
        *          management of the memory for the tensor data. This is a
        *          c-style interface for the tensor dimensions.  Another
        *          function exists for std::vector dimensions.
        *   \details The tensor key under used to locate the tensor
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *
        *            The memory of the data pointer is valid
        *            until the Client is destroyed. This method
        *            is meant to be used when the dimensions and
        *            type of the tensor are unknown or the user does
        *            not want to manage memory.  However, given that
        *            the memory associated with the return data is
        *            valid until Client destruction, this method
        *            should not be used repeatedly for large tensor
        *            data.  Instead  it is recommended that the user
        *            use unpack_tensor() for large tensor data and
        *            to limit memory use by the Client.
        *   \param key  The tensor key for the tensor
        *   \param data Receives tensor data
        *   \param dims Receives the number of elements in each dimension
        *               of the tensor data
        *   \param n_dims Receives the number tensor dimensions
        *   \param type Receives the type for the tensor data
        *   \param mem_layout The memory layout into which tensor
        *                     data should be written
        *   \throw SmartRedis::Exception if get tensor command fails
        */
        void get_tensor(const std::string& key,
                        void*& data,
                        size_t*& dims,
                        size_t& n_dims,
                        SRTensorType& type,
                        const SRMemoryLayout mem_layout);

        /*!
        *   \brief Retrieve a tensor from the database into memory provided
        *          by the caller
        *   \details The tensor key under used to locate the tensor
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param key  The tensor key for the tensor
        *   \param data A buffer into which to place tensor data
        *   \param dims The dimensions for the provided data buffer
        *   \param type The tensor type for the provided data buffer
        *   \param mem_layout The memory layout for the provided data buffer
        *   \throw SmartRedis::Exception if unpack tensor command fails
        */
        void unpack_tensor(const std::string& key,
                           void* data,
                           const std::vector<size_t>& dims,
                           const SRTensorType type,
                           const SRMemoryLayout mem_layout);

        /*!
        *   \brief Move a tensor to a new tensor key
        *   \details The tensor key under used to locate the tensor to be
        *            renamed may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param key The original tensor key
        *   \param new_key The new tensor key
        *   \throw SmartRedis::Exception if rename tensor command fails
        */
        void rename_tensor(const std::string& key,
                           const std::string& new_key);

        /*!
        *   \brief Delete a tensor from the database
        *   \details The tensor key under used to locate the tensor to be
        *            deleted may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param key The tensor key for the tensor to delete
        *   \throw SmartRedis::Exception if delete tensor command fails
        */
        void delete_tensor(const std::string& key);

        /*!
        *   \brief Copy a tensor to a destination tensor key
        *   \details The tensor key under used to locate the tensor to be
        *            copied may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param src_key The source tensor key
        *   \param dest_key The destination tensor key
        *   \throw SmartRedis::Exception if copy tensor command fails
        */
        void copy_tensor(const std::string& src_key,
                         const std::string& dest_key);

        /*!
        *   \brief Set a model (from file) in the database for future
        *          execution
        *   \details The final model key that is used to store the model
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The model key for this model
        *   \param model_file The source file for the model
        *   \param backend The name of the backend (TF, TFLITE, TORCH, ONNX)
        *   \param device The name of the device for execution. May be either
        *                 CPU or GPU. If multiple GPUs are present, a specific
        *                 GPU can be targeted by appending its zero-based
        *                 index, i.e. "GPU:1"
        *   \param batch_size The batch size for model execution
        *   \param min_batch_size The minimum batch size for model
        *                         execution
        *   \param tag A tag to attach to the model for information purposes
        *   \param inputs One or more names of model input nodes
        *                 (TF models only). For other models, provide an
        *                 empty vector
        *   \param outputs One or more names of model output nodes
        *                 (TF models only). For other models, provide an
        *                 empty vector
        *   \throw SmartRedis::Exception if set model from file fails
        */
        void set_model_from_file(const std::string& key,
                                 const std::string& model_file,
                                 const std::string& backend,
                                 const std::string& device,
                                 int batch_size = 0,
                                 int min_batch_size = 0,
                                 const std::string& tag = "",
                                 const std::vector<std::string>& inputs
                                     = std::vector<std::string>(),
                                 const std::vector<std::string>& outputs
                                     = std::vector<std::string>());

        /*!
        *   \brief Set a model (from a buffer) in the
        *          database for future execution
        *   \details The final model key that is used to store the model
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The model key to associate with the model
        *   \param model The model as a continuous buffer
        *   \param backend The name of the backend (TF, TFLITE, TORCH, ONNX)
        *   \param device The name of the device for execution. May be either
        *                 CPU or GPU. If multiple GPUs are present, a specific
        *                 GPU can be targeted by appending its zero-based
        *                 index, i.e. "GPU:1"
        *   \param batch_size The batch size for model execution
        *   \param min_batch_size The minimum batch size for model
        *                         execution
        *   \param tag A tag to attach to the model for information purposes
        *   \param inputs One or more names of model input nodes
        *                 (TF models only). For other models, provide an
        *                 empty vector
        *   \param outputs One or more names of model output nodes
        *                 (TF models only). For other models, provide an
        *                 empty vector
        *   \throw SmartRedis::Exception if set model command fails
        */
        void set_model(const std::string& key,
                       const std::string_view& model,
                       const std::string& backend,
                       const std::string& device,
                       int batch_size = 0,
                       int min_batch_size = 0,
                       const std::string& tag = "",
                       const std::vector<std::string>& inputs
                           = std::vector<std::string>(),
                       const std::vector<std::string>& outputs
                           = std::vector<std::string>());

        /*!
        *   \brief Retrieve a model from the database
        *   \details The model key that is used to locate the model
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The model key associated with the model
        *   \returns A string buffer containing the model.
        *            The memory associated with the model
        *            is managed by this Client object and is valid
        *            until the Client's destruction
        *   \throw SmartRedis::Exception if get model command fails
        */
        std::string_view get_model(const std::string& key);

        /*!
        *   \brief Set a script (from file) in the
        *          database for future execution
        *   \details The final script key that is used to store the script
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The script key to associate with the script
        *   \param device The name of the device for execution. May be either
        *                 CPU or GPU. If multiple GPUs are present, a specific
        *                 GPU can be targeted by appending its zero-based
        *                 index, i.e. "GPU:1"
        *   \param script_file The source file for the script
        *   \throw SmartRedis::Exception if set script command fails
        */
        void set_script_from_file(const std::string& key,
                                  const std::string& device,
                                  const std::string& script_file);

        /*!
        *   \brief Set a script (from buffer) in the
        *          database for future execution
        *   \details The final script key that is used to store the script
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The script key to associate with the script
        *   \param device The name of the device for execution. May be either
        *                 CPU or GPU. If multiple GPUs are present, a specific
        *                 GPU can be targeted by appending its zero-based
        *                 index, i.e. "GPU:1"
        *   \param script The script source in a string buffer
        *   \throw SmartRedis::Exception if set script command fails
        */
        void set_script(const std::string& key,
                        const std::string& device,
                        const std::string_view& script);

        /*!
        *   \brief Retrieve a script from the database
        *   \details The script key that is used to locate the script
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The key associated with the script
        *   \returns A string buffer containing the script.
        *            The memory associated with the model
        *            is managed by this Client object and is valid
        *            until the Client's destruction
        *   \throw SmartRedis::Exception if get script command fails
        */
        std::string_view get_script(const std::string& key);

        /*!
        *   \brief Run a model in the database using the
        *   \details The model key that is used to locate the model to be run
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *          specificed input and output tensors
        *   \param key The model key associated with the model
        *   \param inputs The tensor keys for inputs tensors to use
        *                 in the model
        *   \param outputs The tensor keys of output tensors to
        *                 use to capture model results
        *   \throw SmartRedis::Exception if run model command fails
        */
        void run_model(const std::string& key,
                       std::vector<std::string> inputs,
                       std::vector<std::string> outputs);

        /*!
        *   \brief Run a script function in the database using the
        *          specificed input and output tensors
        *   \details The script key that is used to locate the script to be
        *            run may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param key The script key associated with the script
        *   \param function The name of the function in the script to run
        *   \param inputs The tensor keys of inputs tensors to use
        *                 in the script
        *   \param outputs The tensor keys of output tensors to use to
        *                  capture script results
        *   \throw SmartRedis::Exception if run script command fails
        */
        void run_script(const std::string& key,
                        const std::string& function,
                        std::vector<std::string> inputs,
                        std::vector<std::string> outputs);

        /*!
        *   \brief Check if a key exists in the database
        *   \param key The key that will be checked in the database.
        *              No prefix will be added to \p key.
        *   \returns Returns true if the key exists in the database
        *   \throw SmartRedis::Exception if key exists command fails
        */
        bool key_exists(const std::string& key);

        /*!
        *   \brief Check if a model (or script) key exists in the database
        *   \details The model or script key that is used to check for existence
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param name The model/script key to be checked in the database
        *   \returns Returns true if the key exists in the database
        *   \throw SmartRedis::Exception if model exists command fails
        */
        bool model_exists(const std::string& name);

        /*!
        *   \brief Check if a tensor key exists in the database
        *   \details The tensor key that is used to check for existence
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The tensor key to be checked in the database
        *   \returns Returns true if the tensor key exists in the database
        *   \throw SmartRedis::Exception if tensor exists command fails
        */
        bool tensor_exists(const std::string& name);

        /*!
        *   \brief Check if a dataset exists in the database
        *   \details The dataset key that is used to check for existence
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The dataset key to be checked in the database
        *   \returns Returns true if the dataset key exists in the database
        *   \throw SmartRedis::Exception if dataset exists command fails
        */
        bool dataset_exists(const std::string& name);

        /*!
        *   \brief Check if a key exists in the database, repeating the check
*       *          the check at a specified polling interval
        *   \param key The key to be checked in the database
        *   \param poll_frequency_ms The time delay between checks,
        *                            in milliseconds
        *   \param num_tries The total number of times to check for the key
        *   \returns Returns true if the key is found within the
        *            specified number of tries, otherwise false.
        *   \throw SmartRedis::Exception if poll key command fails
        */
        bool poll_key(const std::string& key,
                      int poll_frequency_ms,
                      int num_tries);

        /*!
        *   \brief Check if a tensor key exists in the database, repeating
*       *          the check at a specified polling interval
        *   \details The tensor key that is used to check for existence
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The tensor key to be checked in the database
        *   \param poll_frequency_ms The time delay between checks,
        *                            in milliseconds
        *   \param num_tries The total number of times to check for the key
        *   \returns Returns true if the key is found within the
        *            specified number of tries, otherwise false.
        *   \throw SmartRedis::Exception if poll tensor command fails
        */
        bool poll_tensor(const std::string& name,
                         int poll_frequency_ms,
                         int num_tries);

        /*!
        *   \brief Check if a dataset key exists in the database, repeating
*       *          the check at a specified polling interval
        *   \details The dataset key that is used to check for existence
        *            may be prefixed. See set_data_source()
        *            and use_tensor_ensemble_prefix() for more details.
        *   \param name The dataset key to be checked in the database
        *   \param poll_frequency_ms The time delay between checks,
        *                            in milliseconds
        *   \param num_tries The total number of times to check for the key
        *   \returns Returns true if the key is found within the
        *            specified number of tries, otherwise false.
        *   \throw SmartRedis::Exception if poll dataset command fails
        */
        bool poll_dataset(const std::string& name,
                          int poll_frequency_ms,
                          int num_tries);

        /*!
        *   \brief Check if a model (or script) key exists in the database,
        *          repeating the check at a specified polling interval
        *   \details The model or script key that is used to check for existence
        *            may be prefixed. See set_data_source()
        *            and use_model_ensemble_prefix() for more details.
        *   \param name The model/script key to be checked in the database
        *   \param poll_frequency_ms The time delay between checks,
        *                            in milliseconds
        *   \param num_tries The total number of times to check for the key
        *   \returns Returns true if the key is found within the
        *            specified number of tries, otherwise false.
        *   \throw SmartRedis::Exception if poll model command fails
        */
        bool poll_model(const std::string& name,
                        int poll_frequency_ms,
                        int num_tries);

        /*!
        *   \brief Set the data source, a key prefix for future operations.
        *   \details The selected prefix must have previously been set via
        *            one of the environment variables SSKEYOUT and SSKEYIN.
        *   \param source_id The prefix for retrieval commands
        *   \throw SmartRedis::Exception for failed setting of data source
        */
        void set_data_source(std::string source_id);

        /*!
        *   \brief Control whether names of tensor and dataset keys are
        *          prefixed (e.g. in an ensemble) when forming database keys.
        *   \details Prefixes will only be used if they were previously set
        *            through the environment variables SSKEYOUT and SSKEYIN.
        *            Keys of entities created before this function is called
        *            will not be retroactively prefixed.
        *            By default, the client prefixes tensor and dataset keys
        *            with the first prefix specified with the SSKEYIN
        *            and SSKEYOUT environment variables.
        *
        *  \param use_prefix If set to true, all future operations
        *                    on tensors and datasets will use
        *                    a prefix, if available.
        *  \throw SmartRedis::Exception for failed activation of tensor prefixing
        */
        void use_tensor_ensemble_prefix(bool use_prefix);

        /*!
        *   \brief Control whether model and script keys are
        *          prefixed (e.g. in an ensemble) when forming database keys.
        *   \details Prefixes will only be used if they were previously set
        *            through the environment variables SSKEYOUT and SSKEYIN.
        *            Keys of entities created before this function is called
        *            will not be retroactively prefixed.
        *            By default, the client prefixes tensor and dataset keys
        *            with the first prefix specified with the SSKEYIN
        *            and SSKEYOUT environment variables.
        *  \param use_prefix If set to true, all future operations
        *                    on models and scripts will use
        *                    a prefix, if available.
        *  \throw SmartRedis::Exception for failed activation of model prefixing
        */
        void use_model_ensemble_prefix(bool use_prefix);

        /*!
        *   \brief Returns information about the given database node
        *   \param address The address of the database node (host:port)
        *   \returns parsed_reply_nested_map containing the database node
        *            information
        *   \throw SmartRedis::Exception if the command fails or if the
        *          address is not addressable by this client.
        *          When using a cluster of database nodes,
        *          it is best practice to bind each node in the cluster
        *          to a specific address to avoid inconsistencies in
        *          addresses retrieved with the CLUSTER SLOTS command.
        *          Inconsistencies in node addresses across
        *          CLUSTER SLOTS commands will lead to SmartRedis::Exception
        *          being thrown.
        */
        parsed_reply_nested_map get_db_node_info(std::string address);

        /*!
        *   \brief Returns the response from a CLUSTER INFO command
        *          addressed to a single cluster node.
        *   \param address The address of the database node (host:port)
        *   \returns parsed_reply_map containing the database cluster information.
        *   \throw SmartRedis::Exception if the command fails or if the
        *          address is not addressable by this client.
        *          When using a cluster of database nodes,
        *          it is best practice to bind each node in the cluster
        *          to a specific address to avoid inconsistencies in
        *          addresses retrieved with the CLUSTER SLOTS command.
        *          Inconsistencies in node addresses across
        *          CLUSTER SLOTS commands will lead to SmartRedis::Exception
        *          being thrown.
        */
        parsed_reply_map get_db_cluster_info(std::string address);

        /*!
        *   \brief Returns the response from an AI.INFO command sent to
        *          the database shard at the provided address
        *   \param address The address of the database node (host:port)
        *   \param key The model or script name
        *   \param reset_stat Boolean indicating if the counters associated
        *                     with the model or script should be reset.
        *   \returns parsed_reply_map containing the AI.INFO information.
        *   \throw SmartRedis::Exception or derivative error object if
        *          command execution or reply parsing fails.
        *          When using a cluster of database nodes,
        *          it is best practice to bind each node in the cluster
        *          to a specific address to avoid inconsistencies in
        *          addresses retrieved with the CLUSTER SLOTS command.
        *          Inconsistencies in node addresses across
        *          CLUSTER SLOTS commands will lead to SmartRedis::Exception
        *          being thrown.
        */
        parsed_reply_map get_ai_info(const std::string& address,
                                     const std::string& key,
                                     const bool reset_stat);

        /*!
        *   \brief Flush the database shard at the provided address
        *   \param address The address of the database node (host:port)
        *   \throw SmartRedis::Exception if the command fails or if the
        *          address is not addressable by this client.
        */
        void flush_db(std::string address);

        /*!
        *   \brief Read the configuration parameters of a running server.
        *   \param expression Parameter used in the configuration or a
        *                     glob pattern (Using '*' retrieves all
        *                     configuration parameters, though this is
        *                     expensive)
        *   \param address The address of the database node (host:port)
        *   \returns An unordered map from configuration parameters
        *            to their values.
        *            If no configuration parameters correspond to the
        *            requested expression, the map is empty.
        *   \throw SmartRedis::Exception if the command fails or if the
        *          address is not addressable by this client.
        *          When using a cluster of database nodes,
        *          it is best practice to bind each node in the cluster
        *          to a specific address to avoid inconsistencies in
        *          addresses retrieved with the CLUSTER SLOTS command.
        *          Inconsistencies in node addresses across
        *          CLUSTER SLOTS commands will lead to SmartRedis::Exception
        *          being thrown.
        */
        std::unordered_map<std::string,std::string> config_get(std::string expression,
                                                               std::string address);

        /*!
        *   \brief Reconfigure the server. It can change both trivial
        *          parameters or switch from one to another persistence option.
        *          All configuration parameters set using this command are
        *          immediately loaded by Redis and will take effect starting with
        *          the next command executed.
        *   \param config_param A configuration parameter to set
        *   \param value The value to assign to the configuration parameter
        *   \param address The address of the database node (host:port)
        *   \throw SmartRedis::Exception if the command fails or if the
        *          address is not addressable by this client.
        *          When using a cluster of database nodes,
        *          it is best practice to bind each node in the cluster
        *          to a specific address to avoid inconsistencies in
        *          addresses retrieved with the CLUSTER SLOTS command.
        *          Inconsistencies in node addresses across
        *          CLUSTER SLOTS commands will lead to SmartRedis::Exception
        *          being thrown.
        */
        void config_set(std::string config_param, std::string value, std::string address);

        /*!
        *   \brief Performs a synchronous save of the database shard,
        *          capturing a snapshot of all the data inside the Redis
        *          instance  in the form of an RDB file.
        *   \param address The address of the database node (host:port)
        *   \throw SmartRedis::Exception if the command fails or if the
        *          address is not addressable by this client.
        *          When using a cluster of database nodes,
        *          it is best practice to bind each node in the cluster
        *          to a specific address to avoid inconsistencies in
        *          addresses retrieved with the CLUSTER SLOTS command.
        *          Inconsistencies in node addresses across
        *          CLUSTER SLOTS commands will lead to SmartRedis::Exception
        *          being thrown.
        */
        void save(std::string address);

    protected:

        /*!
        *  \brief Abstract base class used to generalized
        *         running with cluster or non-cluster
        */
        RedisServer* _redis_server;

        /*!
        *  \brief Dynamically allocated RedisCluster object if the Client is
        *         being run in cluster mode. This
        *         object will be destroyed with the Client.
        */
        RedisCluster* _redis_cluster;

        /*!
        *  \brief Dynamically allocated Redis object if the Client is not
        *         being run in cluster mode. This
        *         object will be destroyed with the Client.
        */
        Redis* _redis;

        /*!
        *   \brief Execute an AddressAtCommand
        *   \param cmd The AddresseAtCommand to execute
        *   \returns The CommandReply after execution
        */
        inline CommandReply _run(AddressAtCommand& cmd)
        {
            return this->_redis_server->run(cmd);
        }

        /*!
        *   \brief Execute an AddressAnyCommand
        *   \param cmd The AddressAnyCommand to execute
        *   \returns The CommandReply after execution
        */
        inline CommandReply _run(AddressAnyCommand& cmd)
        {
            return this->_redis_server->run(cmd);
        }

        /*!
        *   \brief Execute a SingleKeyCommand Command
        *   \param cmd The SingleKeyCommand to execute
        *   \returns The CommandReply after execution
        */
        inline CommandReply _run(SingleKeyCommand& cmd)
        {
            return this->_redis_server->run(cmd);
        }

        /*!
        *   \brief Execute a MultiKeyCommand Command
        *   \param cmd The MultiKeyCommand to execute
        *   \returns The CommandReply after execution
        */
        inline CommandReply _run(MultiKeyCommand& cmd)
        {
            return this->_redis_server->run(cmd);
        }

        /*!
        *   \brief Execute a CompoundCommand
        *   \param cmd The CompoundCommand to execute
        *   \returns The CommandReply after execution
        */
        inline CommandReply _run(CompoundCommand& cmd)
        {
            return this->_redis_server->run(cmd);
        }

        /*!
        *   \brief Execute a list of commands
        *   \param cmd_list The CommandList to execute
        *   \returns The CommandReply from the last command execution
        */
        inline std::vector<CommandReply> _run(CommandList& cmd_list)
        {
            return this->_redis_server->run(cmd_list);
        }

        /*!
        *  \brief Set the prefixes that are used for set and get methods
        *         using SSKEYIN and SSKEYOUT environment variables.
        */
        void _set_prefixes_from_env();

        /*!
        *  \brief Get the key prefix for placement methods
        *  \returns std::string container the placement prefix
        */
        inline std::string _put_prefix();

        /*!
        *  \brief Get the key prefix for retrieval methods
        *  \returns std::string container the retrieval prefix
        */
        inline std::string _get_prefix();

        /*!
        *  \brief Append a vector of keys with the retrieval prefix
        *  \param keys The vector of keys to prefix for retrieval
        */
        inline void _append_with_get_prefix(std::vector<std::string>& keys);

        /*!
        *  \brief Append a vector of keys with the placement prefix
        *  \param keys The vector of keys to prefix for placement
        */
        inline void _append_with_put_prefix(std::vector<std::string>& keys);

        /*!
        *  \brief Execute the command to retrieve the DataSet metadata portion
        *         of the DataSet.
        *   \param name The name of the DataSet, not prefixed.
        *   \returns The CommandReply for the command to retrieve the DataSet
        *             metadata
        */
        inline CommandReply _get_dataset_metadata(const std::string& name);

        /*!
        *   \brief Retrieve the tensor from the DataSet and return
        *          a TensorBase object that can be used to return
        *          tensor information to the user. The returned
        *          TensorBase object has been dynamically allocated,
        *          but not yet tracked for memory management in
        *          any object.
        *   \details The TensorBase object returned will always
        *            have a MemoryLayout::contiguous layout.
        *   \param name  The name used to reference the tensor
        *   \returns A TensorBase object.
        */
        TensorBase* _get_tensorbase_obj(const std::string& name);

        /*!
        *   \brief The name of the hash field used to confirm that the
        *          DataSet placement operation was successfully completed.
        */
        inline static const std::string _DATASET_ACK_FIELD = ".COMPLETE";

        friend class PyClient;

    private:

        /*!
        *  \brief SharedMemoryList to manage memory associated
        *         with model retrieval requests
        */
        SharedMemoryList<char> _model_queries;

        /*!
        *  \brief SharedMemoryList to manage memory associated
        *         with tensor dimensions from tensor retrieval
        */
        SharedMemoryList<size_t> _dim_queries;

        /*!
        *  \brief The _tensor_pack memory is not for querying
        *         by name, but is used to manage memory.
        */
        TensorPack _tensor_memory;

        /*!
        *  \brief The prefix for keys during placement
        */
        std::string _put_key_prefix;

        /*!
        *  \brief The prefix for keys during retrieval
        */
        std::string _get_key_prefix;

        /*!
        *  \brief Vector of all potential retrieval prefixes
        */
        std::vector<std::string> _get_key_prefixes;

        /*!
        * \brief Flag determining whether prefixes should be used
        *        for tensor keys.
        */
        bool _use_tensor_prefix;

        /*!
        * \brief Flag determining whether prefixes should be used
        *        for model and script keys.
        */
        bool _use_model_prefix;

        /*!
        * \brief Build full formatted key of a tensor, based on
        *        current prefix settings.
        * \param key Tensor key
        * \param on_db Indicates whether the key refers to an entity
        *              which is already in the database.
        */
        inline std::string _build_tensor_key(const std::string& key,
                                             const bool on_db);

        /*!
        * \brief Build full formatted key of a model or a script,
        *        based on current prefix settings.
        * \param key Model or script key
        * \param on_db Indicates whether the key refers to an entity
        *              which is already in the database.
        */
        inline std::string _build_model_key(const std::string& key,
                                            const bool on_db);

        /*!
        *  \brief Build full formatted key of a dataset, based
        *         on current prefix settings.
        *  \param dataset_name Name of dataset
        *  \param on_db Indicates whether the key refers to an entity
        *               which is already in the database.
        *  \returns Formatted key.
        */
        inline std::string _build_dataset_key(const std::string& dataset_name,
                                              const bool on_db);

        /*!
        *  \brief Create the key for putting or getting a DataSet tensor
        *         in the database
        *  \param dataset_name The name of the dataset
        *  \param tensor_name The name of the tensor
        *  \param on_db Indicates whether the key refers to an entity
        *               which is already in the database.
        *  \returns A string of the key for the tensor
        */
        inline std::string _build_dataset_tensor_key(const std::string& dataset_name,
                                                     const std::string& tensor_name,
                                                     const bool on_db);

        /*!
        *  \brief Create keys for putting or getting a DataSet tensors
        *         in the database
        *  \param dataset_name The name of the dataset
        *  \param tensor_name A std::vector of tensor names
        *  \param on_db Indicates whether the keys refer to an entity which
        *               is already in the database.
        *  \returns A std::vector<std::string> of the keys for the tensors
        */
        inline std::vector<std::string>
        _build_dataset_tensor_keys(const std::string& dataset_name,
                                   const std::vector<std::string>& tensor_names,
                                   const bool on_db);

        /*!
        *  \brief Create the key for putting or getting DataSet metadata
        *         in the database
        *  \param dataset_name The name of the dataset
        *  \param on_db Indicates whether the key refers to an entity which
        *               is already in the database.
        *  \returns A string of the key for the metadata
        */
        inline std::string _build_dataset_meta_key(const std::string& dataset_name,
                                                   const bool on_db);

        /*!
        *  \brief Create the key to place an indicator in the database
        *         that the dataset has been successfully stored.
        *  \param dataset_name The name of the dataset
        *  \param on_db Indicates whether the key refers to an entity which
        *               is already in the database.
        *  \returns A string of the key for the ack key
        */
        inline std::string _build_dataset_ack_key(const std::string& dataset_name,
                                                  const bool on_db);

        /*!
        *   \brief Append the Command associated with
        *          placing DataSet metadata in the database
        *          to a CommandList
        *   \param cmd_list The CommandList to append DataSet
        *                   metadata commands
        *   \param dataset The dataset used for the Command
        *                  construction
        */
        void _append_dataset_metadata_commands(CommandList& cmd_list,
                                               DataSet& dataset);

        /*!
        *   \brief Append the Command associated with
        *          placing DataSet tensors in the database
        *          to a CommandList
        *   \param cmd_list The CommandList to append DataSet
        *                   tensor commands
        *   \param dataset The dataset used for the Command
        *                  construction
        */
        void _append_dataset_tensor_commands(CommandList& cmd_list,
                                             DataSet& dataset);

        /*!
        *   \brief Append the Command associated with
        *          acknowledging that the DataSet is complete
        *          (all put commands processed) to the CommandList
        *   \param cmd_list The CommandList to append DataSet
        *                   ack command
        *   \param dataset The dataset used for the Command
        *                  construction
        */
        void _append_dataset_ack_command(CommandList& cmd_list,
                                         DataSet& dataset);

        /*!
        *   \brief Put the metadata fields embedded in a
        *          CommandReply into the DataSet
        *   \param dataset The DataSet that will have
        *                  metadata place in it.
        *   \param reply The CommandReply containing the
        *                metadata fields.
        */
        void _unpack_dataset_metadata(DataSet& dataset,
                                      CommandReply& reply);
};

} //namespace SmartRedis

#endif //__cplusplus
#endif //SMARTREDIS_CPP_CLIENT_H
