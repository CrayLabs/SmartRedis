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

#include "pipelinereply.h"
#include "srexception.h"

using namespace SmartRedis;

/*
void set_queued_reply_object(sw::redis::QueuedReplies&& queued_reply,
                             std::vector<std::vector<size_t>>&& index_map)
{
    if (queued_reply.size() != index_map.size()) {
        throw SRInternalException("Pipeline construction failed.  "
                                  "The queued_reply parameter has length " +
                                  std::to_string(queued_reply.size()) +
                                  " and the index_map parameter has length " +
                                  std::to_string(index_map.size() +
                                  ": both must have the same length.");
    }

    _replies = std::move(queued_reply);
    _cmd_to_reply_index_map = std::move(index_map);
}
*/

// Append the replies inside of the QueuedReplies object into the
// PipelineReply object
void PipelineReply::append_queued_reply(sw::redis::QueuedReplies&& queued_reply)
{
    if (queued_reply.size() > 0) {
        _queued_replies.push_back(QueuedReplies(std::move(queued_reply)));
        size_t last_index = _queued_replies.size() - 1;
        for (size_t i = 0; i < _queued_replies[last_index].size(); i++) {
            _all_replies.push_back(&(_queued_replies[last_index].get(i)));
        }
    }
}

// Return an entry in the PipelineReply
CommandReply PipelineReply::operator[](int index)
{
    if (index > _all_replies.size()) {
        throw SRInternalException("An attempt was made to access index " +
                                  std::to_string(index) +
                                  " of the PipelineReply, which is beyond the "\
                                  " PipelineReply length of " +
                                  std::to_string(_all_replies.size()));
    }

    return CommandReply::shallow_clone(_all_replies[index]);
}

// Get the number of CommandReply in the PipelineReply
size_t PipelineReply::size()
{
    return _all_replies.size();
}

// Check if any CommandReply has error
bool PipelineReply::has_error()
{
    for (size_t i = 0; i < _all_replies.size(); i++) {
        if (CommandReply::shallow_clone(_all_replies[i]).has_error() > 0)
            return true;
    }
    return false;
}


// Reorder the internal order of pipeline command replies
void PipelineReply::reorder(std::vector<size_t> indices)
{
    for (size_t i = 0; i < indices.size(); i++) {
        while(i != indices[i]) {
            size_t swap_index = indices[i];
            std::swap(_all_replies[i], _all_replies[swap_index]);
            std::swap(indices[i], indices[swap_index]);
        }
    }
}
