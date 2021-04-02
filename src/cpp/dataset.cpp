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

#include "dataset.h"

using namespace SmartRedis;

DataSet::DataSet(const std::string& name)
{
    this->name = name;
}

DataSet::DataSet(const std::string& name,
                 char* buf,
                 size_t buf_size)
{
    this->name = name;
    this->_metadata.fill_from_buffer(buf, buf_size);
}

void DataSet::add_tensor(const std::string& name,
                         void* data,
                         const std::vector<size_t>& dims,
                         const TensorType type,
                         MemoryLayout mem_layout)
{
    this->_add_to_tensorpack(name, data, dims,
                             type, mem_layout);
    this->_metadata.add_string(".tensor_names", name);
    return;
}

void DataSet::add_meta_scalar(const std::string& name,
                              const void* data,
                              const MetaDataType type)
{
    this->_metadata.add_scalar(name, data, type);
    return;
}

void DataSet::add_meta_string(const std::string& name,
                              const std::string& data)
{
    this->_metadata.add_string(name, data);
    return;
}

void DataSet::get_tensor(const std::string& name,
                         void*& data,
                         std::vector<size_t>& dims,
                         TensorType& type,
                         MemoryLayout mem_layout)
{
    if(!(this->_tensorpack.tensor_exists(name)))
        throw std::runtime_error("The tensor " +
                                 std::string(name) +
                                 " does not exist in " +
                                 this->name + " dataset.");

    type = this->_tensorpack.get_tensor(name)->type();
    data = this->_tensorpack.get_tensor(name)->data_view(mem_layout);
    dims = this->_tensorpack.get_tensor(name)->dims();
    return;
}

void DataSet::get_tensor(const std::string&  name,
                         void*& data,
                         size_t*& dims,
                         size_t& n_dims,
                         TensorType& type,
                         MemoryLayout mem_layout)
{
    std::vector<size_t> dims_vec;
    this->get_tensor(name, data, dims_vec,
                     type, mem_layout);

    size_t n_bytes = sizeof(int)*dims_vec.size();
    dims = this->_dim_queries.allocate_bytes(n_bytes);
    n_dims = dims_vec.size();

    std::vector<size_t>::const_iterator it = dims_vec.cbegin();
    std::vector<size_t>::const_iterator it_end = dims_vec.cend();
    size_t i = 0;
    while(it!=it_end) {
        dims[i] = *it;
        i++;
        it++;
    }

    return;
}

void DataSet::unpack_tensor(const std::string& name,
                            void* data,
                            const std::vector<size_t>& dims,
                            const TensorType type,
                            MemoryLayout mem_layout)
{
   if(!(this->_tensorpack.tensor_exists(name)))
        throw std::runtime_error("The tensor " + std::string(name)
                                               + " does not exist in "
                                               + this->name + " dataset.");

    this->_tensorpack.get_tensor(name)->fill_mem_space(data, dims, mem_layout);
    return;
}

void DataSet::get_meta_scalars(const std::string& name,
                               void*& data,
                               size_t& length,
                               MetaDataType& type)
{
    this->_metadata.get_scalar_values(name, data,
                                      length, type);
    return;
}

void DataSet::get_meta_strings(const std::string& name,
                               char**& data,
                               size_t& n_strings,
                               size_t*& lengths)
{
    this->_metadata.get_string_values(name, data,
                                      n_strings, lengths);
    return;
}

void DataSet::clear_field(const std::string& field_name)
{
    this->_metadata.clear_field(field_name);
}

std::vector<std::string> DataSet::get_tensor_names()
{
    return this->_metadata.get_string_values(".tensor_names");
}

std::vector<std::string> DataSet::get_meta_strings(const std::string& name)
{
    return this->_metadata.get_string_values(name);
}

std::string DataSet::get_tensor_type(const std::string& name)
{
    return this->_tensorpack.get_tensor(name)->name();
}

void DataSet::_add_to_tensorpack(const std::string& name,
                                 void* data,
                                 const std::vector<size_t>& dims,
                                 const TensorType type,
                                 const MemoryLayout mem_layout)
{
    this->_tensorpack.add_tensor(name, data, dims,
                                 type, mem_layout);
    return;
}

DataSet::tensor_iterator DataSet::tensor_begin()
{
    return this->_tensorpack.tensor_begin();
}

DataSet::const_tensor_iterator DataSet::tensor_cbegin()
{
    return this->_tensorpack.tensor_cbegin();
}

DataSet::tensor_iterator DataSet::tensor_end()
{
    return this->_tensorpack.tensor_end();
}

DataSet::const_tensor_iterator DataSet::tensor_cend()
{
    return this->_tensorpack.tensor_cend();
}

std::string_view DataSet::get_metadata_buf()
{
   return this->_metadata.get_metadata_buf();
}