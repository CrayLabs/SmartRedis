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

#ifndef SMARTREDIS_TENSORLIST_H
#define SMARTREDIS_TENSORLIST_H

#include <forward_list>
#include "tensor.h"
#include <iostream>

namespace SmartRedis {

/*!
*   \brief  The TensorList class stores Tensor objects
            of the same type and manages the associated
            memory.
    \tparam The Tensor type (e.g. double).
*/
template <class T>
class TensorList {

    public:

    /*!
    *   \brief TensorList default constructor
    */
    TensorList() = default;

    /*!
    *   \brief TensorList move constructor
    *   \param t_list The TensorList to move for construction
    */
    TensorList(TensorList&& t_list);

    /*!
    *   \brief TensorList copy constructor
    *   \details A deep copy is performed for all Tensors
    *            in the TensorList t_list.
    *   \param t_list The TensorList to copy for construction
    */
    TensorList(const TensorList& t_list);

    /*!
    *   \brief TensorList copy assignment operator
    *   \details A deep copy is performed for all Tensors
    *            in the TensorList t_list.
    *   \param t_list The TensorList to copy for assignment
    *   \returns The TensorList that has been assigned new values
    */
    TensorList<T>& operator=(const TensorList<T>& t_list);

    /*!
    *   \brief TensorList move assignment operator
    *   \param t_list The TensorList to move for assignment
    *   \returns The TensorList that has been assigned new values
    */
    TensorList<T>& operator=(TensorList<T>&& t_list);

    /*!
    *   \brief TensorList default destructor
    */
    ~TensorList();

    /*!
    *   \brief Add a previously allocated Tensor
    *   \param tensor The Tensor to add to the TensorList
    */
    void add_tensor(Tensor<T>* tensor);

    /*!
    *   \brief Add a previously allocated TensorBase object.
    *          The TensorBase pointer will be recast as a
    *          Tensor<T>* type before being added.
    *   \param tensor The TensorBase object to add to
    *                 the TensorList
    */
    void add_tensor(TensorBase* tensor);

    /*!
    *   \brief Allocate a new Tensor and add it to the list
    *   \param tensor The Tensor to add to the TensorList
    *   \param name The name used to reference the tensor
    *   \param data c-ptr to the source data for the tensor
    *   \param dims The dimensions of the tensor
    *   \param type The data type of the tensor
    *   \param mem_layout The memory layout of the source data
    *   \returns A pointer to the newly allocated Tensor
    */
    Tensor<T>* allocate_tensor(const std::string& name,
                               void* data,
                               const std::vector<size_t>& dims,
                               const TensorType type,
                               const MemoryLayout mem_layout);

    /*!
    *   \brief Delete all of the Tensors in the TensorList,
    *          freeing all memory
    */
    void clear();

    /*!
    *   \typedef An iterator type for iterating
    *            over all Tensors
    */
    typedef typename std::forward_list<Tensor<T>*>::iterator iterator;

    /*!
    *   \typedef A const iterator type for iterating
    *            over all Tensors
    */
    typedef typename std::forward_list<Tensor<T>*>::const_iterator const_iterator;

    /*!
    *   \brief Returns an iterator pointing to the
    *          first Tensor in the TensorList
    *   \returns TensorList iterator to the first Tensor
    */
    iterator begin();

    /*!
    *   \brief Returns a const iterator pointing to the
    *          first Tensor in the TensorList
    *   \returns Const TensorList iterator to the first Tensor
    */
    const_iterator cbegin();

    /*!
    *   \brief Returns an iterator pointing to the
    *          past-the-end Tensor in the TensorList
    *   \returns TensorList iterator to the past-the-end Tensor
    */
    iterator end();

    /*!
    *   \brief Returns a const iterator pointing to the
    *          past-the-end Tensor in the TensorList
    *   \returns Const TensorList iterator to the past-the-end
    *            Tensor
    */
    const_iterator cend();

    private:

    /*!
    *   \brief Inventory of allocated Tensor objects
    */
    std::forward_list<Tensor<T>*> _inventory;

    /*!
    *   \brief Free the memory associated
    *          with all Tensors in the TensorList
    */
    inline void _free_tensors();

};

#include "tensorlist.tcc"

} //namespace SmartRedis

#endif //SMARTREDIS_TENSORLIST_H

