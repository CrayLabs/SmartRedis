/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021-2023, Hewlett Packard Enterprise
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

#ifndef SMARTREDIS_SHAREDMEMORYLIST_TCC
#define SMARTREDIS_SHAREDMEMORYLIST_TCC

// Record a memory allocation
template <class T>
void SharedMemoryList<T>::add_allocation(size_t bytes, T* ptr)
{
    std::shared_ptr<T> s_ptr(ptr);
    _inventory.push_front(s_ptr);
}

// Allocate memory and record the allocation
template <class T>
T* SharedMemoryList<T>::allocate_bytes(size_t bytes)
{
    try {
        T* ptr = (T*)new unsigned char[bytes];
        add_allocation(bytes, ptr);
        return ptr;
    }
    catch (std::bad_alloc& e) {
        throw SRBadAllocException("shared memory buffer");
    }
}

// Perform type-specific memory allocation
template <class T>
T* SharedMemoryList<T>::allocate(size_t n_values)
{
    size_t bytes = n_values * sizeof(T);
    return allocate_bytes(bytes);
}

#endif // SMARTREDIS_SHAREDMEMORYLIST_TCC
