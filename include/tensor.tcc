/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021-2024, Hewlett Packard Enterprise
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

#ifndef SMARTREDIS_TENSOR_TCC
#define SMARTREDIS_TENSOR_TCC

///@file

// Tensor constructor
template <class T>
Tensor<T>::Tensor(const std::string& name,
                  const void* data,
                  const std::vector<size_t>& dims,
                  const SRTensorType type,
                  const SRMemoryLayout mem_layout) :
                  TensorBase(name, data, dims, type)
{
    _set_tensor_data(data, dims, mem_layout);
}

// Tensor copy constructor
template <class T>
Tensor<T>::Tensor(const Tensor<T>& tensor) : TensorBase(tensor)
{
    // Check for self-copy
    if (&tensor == this)
        return;

    _set_tensor_data(tensor._data, tensor._dims, SRMemLayoutContiguous);
    _mem_views = tensor._mem_views;
}

// Tensor move constructor
template <class T>
Tensor<T>::Tensor(Tensor<T>&& tensor) : TensorBase(std::move(tensor))
{
    _mem_views = std::move(tensor._mem_views);
}

// Tensor copy assignment operator
template <class T>
Tensor<T>& Tensor<T>::operator=(const Tensor<T>& tensor)
{
    // Check for self assignment
    if (this == &tensor)
        return *this;

    // Deep copy tensor data
    TensorBase::operator=(tensor);
    _set_tensor_data(tensor._data, tensor._dims, SRMemLayoutContiguous);
    _mem_views = tensor._mem_views;

    // Done
    return *this;
}

// Tensor move assignment operator
template <class T>
Tensor<T>& Tensor<T>::operator=(Tensor<T>&& tensor)
{
    // Check for self-move
    if (this == &tensor)
        return *this;

    // Move data
    TensorBase::operator=(std::move(tensor));
    _mem_views = std::move(tensor._mem_views);

    // Done
    return *this;
}

// Deep copy operator
template <class T>
TensorBase* Tensor<T>::clone()
{
    try {
        Tensor<T>* new_tensor = new Tensor<T>(*this);
        (*new_tensor) = *this;
        return new_tensor;
    }
    catch (std::bad_alloc& e) {
        throw SRBadAllocException("tensor");
    }
}

// Get a pointer to a specified memory view of the Tensor data
template <class T>
void* Tensor<T>::data_view(const SRMemoryLayout mem_layout)
{
    /* This function returns a pointer to a memory
    view of the underlying tensor data.  The
    view of the underlying tensor data depends
    on the mem_layout value.  The following values
    result in the following views:
    1) MemoryLayout::contiguous : The returned view
       pointer points to the first position in
       the tensor data array.  The caller is
       expected to only index into the view
       pointer with a single []
    2) MemoryLayout::nested : The returned view
       pointer points to a nested structure of
       pointers so that the caller can cast
       to a nested pointer structure and index
       with multiple [] operators.
    */

    void* ptr = NULL;

    switch (mem_layout) {
        case SRMemLayoutContiguous:
            ptr = _data;
            break;
        case SRMemLayoutNested:
            _build_nested_memory(&ptr,
                                 _dims.data(),
                                 _dims.size(),
                                 (T*)_data);
            break;
        default:
            throw SRRuntimeException("Unsupported MemoryLayout value in "\
                                     "Tensor<T>.data_view().");
    }
    return ptr;
}

// Fill a user provided memory space with values from tensor data
template <class T>
void Tensor<T>::fill_mem_space(void* data,
                               std::vector<size_t> dims,
                               SRMemoryLayout mem_layout)
{
    if (_data == NULL) {
        throw SRRuntimeException("The tensor does not have "\
                                 "a data array to fill with.");
    }

    if (dims.size() == 0) {
        throw SRRuntimeException("The dimensions must have nonzero size");
    }

    // Calculate size of memory buffer
    size_t n_values = 1;
    std::vector<size_t>::const_iterator it = dims.cbegin();
    for ( ; it != dims.cend(); it++) {
        if (*it <= 0) {
            throw SRRuntimeException("All dimensions must be greater than 0.");
        }
        n_values *= (*it);
    }

    // Make sure there is space for all the data
    if (n_values != num_values()) {
        throw SRRuntimeException("The provided dimensions do "\
                                 "not match the size of the "\
                                 "tensor data array");
    }

    // Copy over the data
    switch (mem_layout) {
        case SRMemLayoutContiguous:
            std::memcpy(data, _data, _n_data_bytes());
            break;
        case SRMemLayoutNested: {
            size_t starting_position = 0;
            _fill_nested_mem_with_data(data, dims.data(),
                                             dims.size(),
                                             starting_position,
                                             _data);
            }
            break;
        default:
            throw SRRuntimeException("Unsupported MemoryLayout value in "\
                                     "Tensor<T>.fill_mem_space().");
    }
}

// copy values from nested memory structure to contiguous memory structure
template <class T>
void* Tensor<T>::_copy_nested_to_contiguous(const void* src_data,
                                            const size_t* dims,
                                            const size_t n_dims,
                                            void* dest_data)
{
    if (n_dims > 1) {
        T** current = (T**)src_data;
        for (size_t i = 0; i < dims[0]; i++) {
          dest_data =
            _copy_nested_to_contiguous(*current, &dims[1],
                                       n_dims - 1, dest_data);
          current++;
        }
    }
    else {
        std::memcpy(dest_data, src_data, sizeof(T) * dims[0]);
        return &((T*)dest_data)[dims[0]];
    }
    return dest_data;
}

// copy from a flat, contiguous memory structure to a provided nested structure
template <class T>
void Tensor<T>::_fill_nested_mem_with_data(void* data,
                                           size_t* dims,
                                           size_t n_dims,
                                           size_t& data_position,
                                           void* tensor_data)
{
    if (n_dims > 1) {
        T** current = (T**)data;
        for (size_t i = 0; i < dims[0]; i++, current++) {
            _fill_nested_mem_with_data(
                    *current, &dims[1], n_dims-1,
                    data_position, tensor_data);
        }
    }
    else {
        T* data_to_copy = &(((T*)tensor_data)[data_position]);
        std::memcpy(data, data_to_copy, dims[0] * sizeof(T));
        data_position += dims[0];
    }
}

// Builds nested array structure to point to the provided flat, contiguous
// memory space.  The space is returned via the data input pointer.
template <class T>
T* Tensor<T>::_build_nested_memory(void** data,
                                   size_t* dims,
                                   size_t n_dims,
                                   T* contiguous_mem)
{
    if (dims == NULL) {
        throw SRRuntimeException("Missing dims in call to "\
                                 "_build_nested_memory");
    }
    if (n_dims > 1) {
        T** new_data = _mem_views.allocate(dims[0]);
        if (new_data == NULL)
            throw SRBadAllocException("nested memory for tensor");
        (*data) = reinterpret_cast<void*>(new_data);
        for (size_t i = 0; i < dims[0]; i++)
            contiguous_mem = _build_nested_memory(
                reinterpret_cast<void**>(&new_data[i]), &dims[1],
                n_dims - 1, contiguous_mem);
    }
    else {
        (*data) = (T*)contiguous_mem;
        contiguous_mem += dims[0];
    }
    return contiguous_mem;
}

// Set the tensor data from a src memory location.
template <class T>
void Tensor<T>::_set_tensor_data(const void* src_data,
                                 const std::vector<size_t>& dims,
                                 const SRMemoryLayout mem_layout)
{
    size_t n_values = num_values();
    size_t n_bytes = n_values * sizeof(T);
    try {
        _data = new unsigned char[n_bytes];
    }
    catch (std::bad_alloc& e) {
        throw SRBadAllocException("tensor data");
    }

    switch (mem_layout) {
        case SRMemLayoutContiguous:
            std::memcpy(_data, src_data, n_bytes);
            break;
        case SRMemLayoutNested:
            _copy_nested_to_contiguous(
                src_data, dims.data(), dims.size(), _data);
            break;
        default:
            throw SRRuntimeException("Invalid memory layout in call "\
                                     "to _set_tensor_data");
    }
}

// Get the total number of bytes of the data
template <class T>
size_t Tensor<T>::_n_data_bytes()
{
    return num_values() * sizeof(T);
}

#endif // SMARTREDIS_TENSOR_TCC
