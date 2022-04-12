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

        /*
        *   \brief Set the QueuedReplies of the PipelineReply object
        *   \details Because sw::redis::QueuedReplies only has move
        *            semantics, only rvalue interface is provided.
        *            The queued_reply and index_map parameters
        *            must be of the same length.
        *   \param queued_reply The sw::redis::QueuedReplies object
        *   \param index_map Mapping between pipeline provided command index
        *          and executed (i.e. pipeline response) index.
        *   \throw InternalException if the queue_reply and index_map
        *          parameters do not have same length.
       void set_queued_reply_object(sw::redis::QueuedReplies&& queued_reply,
                                    std::vector<std::vector<size_t>>&& index_map);
         */

        /*!
        *   \brief Append the replies in a QueuedReplies object to the
        *          PipelineReply object
        *   \param queued_reply The sw::redis::QueuedReplies object
        */
        void append_queued_reply(sw::redis::QueuedReplies&& queued_reply);

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

        /*!
        *   \brief Reorder the stored pipeline command reply messages based
        *          on the provided index vector.
        *   \details This function allows a user to reorder the stored command
        *            replies in the cases where the operator [] should align
        *            to some other sequence not created by the append() function.
        *   \param indices A std::vector<size_t> of indices where each
        *                  entry should be moved
        *   \throw SmartRedis::InternalError if indices does not match
        *          the length of the internal container for replies
        */
        void reorder(std::vector<size_t> indices);

    private:

        /*!
        *   \brief The original redis-plus-plus object containing the
        *          pipeline response.  This is held here just
        *          for memory management.
        */
        std::vector<sw::redis::QueuedReplies> _queued_replies;

        // TODO We could use std::reference_wrapper here instead of pointers
        /*!
        *   \brief This is an aggregate vector of all of the
        *          redisReply references from _queued_replies.
        *          This will make for easier random access in the
        *          future.
        */
        std::vector<redisReply*> _all_replies;


        /*!
        *   \brief Mapping between pipeline provided command index
        *          and executed (i.e. pipeline response) index.
        *          This is necessary because we don't have the ability
        *          to reorder the redis++ vector because it is a
        *          private member variable.
        */
        std::vector<std::vector<size_t>> _cmd_to_reply_index_map;

};

} //namespace SmartRedis

#endif //SMARTREDIS_PIPELINEREPLY_H
