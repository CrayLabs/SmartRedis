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

// Move assignment constructor from QueuedReplies object
PipelineReply::PipelineReply(QueuedReplies&& queued_reply)
{
    // Move the QueuedReplies
    _replies = std::move(queued_reply);
}

// Copy assignment operator from QueuedReplies object
PipelineReply& PipelineReply::operator=(QueuedReplies&& queued_reply)
{
    // Move the QueuedReplies
    _replies = std::move(queued_reply);
    return *this;
}

// Return an entry in the PipelineReply
CommandReply PipelineReply::operator[](int index)
{
    if (index > _replies.size()) {
        throw SRInternalException("An attempt was made to access index " +
                                  std::to_string(index) +
                                  " of the PipelineReply, which is beyond the "\
                                  " PipelineReply length of " +
                                  std::to_string(_replies.size()));
    }

    return CommandReply(&_replies.get(index));
}

// Get the number of CommandReply in the PipelineReply
size_t PipelineReply::size()
{
    return _replies.size();
}

// Check if any CommandReply has error
bool PipelineReply::has_error()
{
    for (size_t i = 0; i < _replies.size(); i++) {
        if (CommandReply(&_replies.get(i)).has_error() > 0)
            return true;
    }
    return false;
}
