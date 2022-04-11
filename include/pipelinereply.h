/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021-2022, Hewlett Packard Enterprise
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

#ifndef SMARTREDIS_PIPELINEREPLY_H
#define SMARTREDIS_PIPELINEREPLY_H

#include "stdlib.h"
#include <sw/redis++/redis++.h>
#include <iostream>
#include <vector>
#include <queue>

#include "commandreply.h"

namespace SmartRedis {

class PipelineReply;

/*!
*   \brief Redis++ pipeline command type
*/
typedef sw::redis::QueuedReplies QueuedReplies;

///@file
/*!
*   \brief The PipelineReply class stores and processes replies
*          from pipelined commands
*   \details The PipelineReply is built around processing
*            redis-plus-plus QueuedReplies.  The redis-plus-plus
*            generic pipeline execution returns a QueuedReplies
*            object.  The destructor of the QueuedReplies object
*            will call the destructor for each unique ptr inside
*            of the QueuedReplies object.  The QueuedReplies
*            object does not allow for copy semantics, and only
*            moved semantics, so we are restricted to using
*            move semantics.
*/
class PipelineReply {

    public:

        /*!
        *   \brief Default PipelineReply constructor
        */
        PipelineReply() = default;

        /*!
        *   \brief Default PipelineReply destructor
        */
        ~PipelineReply() = default;

        /*!
        *   \brief PipelineReply move constructor from QueuedReplies object
        *   \param queued_reply The QueuedReplies object to move for construction
        */
        PipelineReply(QueuedReplies&& queued_reply);


        /*!
        *   \brief PipelineReply copy constructor
        *   \param reply The PipelineReply to copy
        */
        PipelineReply(const PipelineReply& reply) = delete;

        /*!
        *   \brief PipelineReply copy assignment operator
        *   \param reply The PipelineReply to copy
        *   \returns PipelineReply reference
        */
        PipelineReply& operator=(PipelineReply& reply) = delete;

        /*!
        *   \brief PipelineReply move constructor
        *   \param reply The PipelineReply for construction
        */
        PipelineReply(PipelineReply&& reply) = default;

        /*!
        *   \brief PipelineReply move assignment operator
        *   \param reply The PipelineReply for assignment
        *   \returns PipelineReply reference
        */
        PipelineReply& operator=(PipelineReply&& reply) = default;

        /*!
        *   \brief PipelineReply move assignment operator from
        *          QueuedReplies object
        *   \param queued_reply The QueuedReplies object to move for
        *                       assignment
        *   \returns PipelineReply reference
        */
        PipelineReply& operator=(QueuedReplies&& queued_reply);

        /*!
        *   \brief Index operator for PipelineReply
        *          that will return the indexed element
        *          of the PipelineReply
        *   \param index Index to retrieve
        *   \returns The indexed CommandReply
        */
        CommandReply operator[](int index);

        /*!
        *   \brief Get the number of CommandReply in the PipelineReply
        *   \returns The number of CommandReply in the PipelineReply
        */
        size_t size();

        /*!
        *   \brief Return if any of the CommandReply in the PipelineReply
        *          has an error
        *   \returns True if any CommandReply has an error, otherwise false
        */
        bool has_error();

    private:

        /*!
        *   \brief The original redis-plus-plus object containing the
        *          pipeline response
        */
        sw::redis::QueuedReplies _replies;

};

} //namespace SmartRedis

#endif //SMARTREDIS_PIPELINEREPLY_H
