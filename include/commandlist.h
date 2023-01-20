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

#ifndef SMARTREDIS_COMMANDLIST_H
#define SMARTREDIS_COMMANDLIST_H

#include <stdlib.h>
#include <vector>
#include "command.h"
#include "srexception.h"

///@file

namespace SmartRedis {

class CommandList;

/*!
*   \brief The CommandList class constructs multiple Client
*          Command.
*   \details CommandList handles the dynamic allocation of a new
*            message in the list and provides iterators
*            for iterating over Command.
*/
class CommandList
{
    public:

        /*!
        *   \brief Default CommandList constructor
        */
        CommandList() = default;

        /*!
        *   \brief CommandList copy constructor
        *   \param cmd_lst The CommandList to copy for construction
        */
       CommandList(const CommandList& cmd_lst);

        /*!
        *   \brief CommandList default move constructor
        */
       CommandList(CommandList&& cmd_lst) = default;

        /*!
        *   \brief CommandList copy assignment operator
        *   \param cmd_lst The CommandList to copy for assignment
        */
        CommandList& operator=(const CommandList& cmd_lst);

        /*!
        *   \brief CommandList move assignment operator
        */
        CommandList& operator=(CommandList&& cmd_lst) = default;

        /*!
        *   \brief Default CommandList destructor
        */
        ~CommandList();

        /*!
        *   \brief Add new Commmand of type C to the CommandList
        *   \tparam C Any type of Command
        */
        template <class T>
        T* add_command();

        /*!
        *   \brief Return a reference to a Command in the
        *          CommandList
        *   \param index The index of the Command in the CommandList
        *   \return A reference to the Command at the specified index
        *   \throw Internal exception if index is not valid
        */
        Command& operator[](size_t index);

        /*!
        *   \brief An iterator type for iterating
        *            over all Commands
        */
        typedef std::vector<Command*>::iterator iterator;

        /*!
        *   \brief A const iterator type for iterating
        *            over all Commands
        */
        typedef std::vector<Command*>::const_iterator const_iterator;

        /*!
        *   \brief Returns an iterator pointing to the
        *          first Command
        *   \returns CommandList iterator to the first Command
        */
        iterator begin();

        /*!
        *   \brief Returns a const iterator pointing to the
        *          first Command
        *   \returns Const CommandList iterator to the first Command
        */
        const_iterator cbegin();

        /*!
        *   \brief Returns an iterator pointing to the
        *          past-the-end Command
        *   \returns CommandList iterator to the past-the-end Command
        */
        iterator end();

        /*!
        *   \brief Returns a const iterator pointing to the
        *          past-the-end Command
        *   \returns Const CommandList iterator to the past-the-end Command
        */
        const_iterator cend();

        /*!
        *   \brief Return the number of Command in the CommandList
        *   \returns The number of Command in CommandList
        */
        size_t size();

    private:

        /*!
        *   \brief A vector container a pointer to all Command
        */
        std::vector<Command*> _commands;
};

#include "commandlist.tcc"

} // namespace SmartRedis

#endif // SMARTREDIS_COMMANDLIST_H
