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

#include "commandreply.h"
#include "srexception.h"

using namespace SmartRedis;

CommandReply::CommandReply(const CommandReply& reply)
{
    if (this != &reply) {
        _uptr_reply = RedisReplyUPtr(deep_clone_reply(reply._reply), sw::redis::ReplyDeleter());
        _reply = _uptr_reply.get();
    }
}

CommandReply::CommandReply(const redisReply* reply)
{
    _uptr_reply = RedisReplyUPtr(deep_clone_reply(reply),
                                 sw::redis::ReplyDeleter());
    _reply = _uptr_reply.get();
}

// Move constructor with RedisReplyUPtr as input
CommandReply::CommandReply(RedisReplyUPtr&& reply)
{
    _uptr_reply = std::move(reply);
    _reply = _uptr_reply.get();
}

// Move constructor with redisReply as input
CommandReply::CommandReply(redisReply*&& reply)
{
    _uptr_reply = NULL;
    _reply = std::move(reply);
}

// Move constructor with CommandReply as input
CommandReply::CommandReply(CommandReply&& reply)
{
    if (this != &reply) {
        _uptr_reply = std::move(reply._uptr_reply);
        _reply = _uptr_reply.get();
    }
}

CommandReply& CommandReply::operator=(const CommandReply& reply)
{
    if (this != &reply) {
        _uptr_reply.reset(NULL);
        _uptr_reply = RedisReplyUPtr(deep_clone_reply(reply._reply), sw::redis::ReplyDeleter());
        _reply = _uptr_reply.get();
    }
    return *this;
}

CommandReply& CommandReply::operator=(const redisReply* reply)
{
    _uptr_reply.reset(NULL);
    _uptr_reply = RedisReplyUPtr(deep_clone_reply(reply),
                                 sw::redis::ReplyDeleter());
    _reply = _uptr_reply.get();
    return *this;
}

// Move assignment operator with RedisReplyUPtr as input
CommandReply& CommandReply::operator=(RedisReplyUPtr&& reply)
{
    _uptr_reply = std::move(reply);
    _reply = _uptr_reply.get();
    return *this;
}

// Move assignment operator with redisReply as input.
CommandReply& CommandReply::operator=(redisReply*&& reply)
{
    _uptr_reply = NULL;
    _reply = std::move(reply);
    return *this;
}

// Move assignment operator with CommandReply as input.
CommandReply& CommandReply::operator=(CommandReply&& reply)
{
    if (this !=& reply) {
        _uptr_reply = std::move(reply._uptr_reply);
        _reply = _uptr_reply.get();
    }
    return *this;
}

CommandReply CommandReply::shallow_clone(redisReply* reply)
{
    CommandReply r;
    r._uptr_reply = NULL;
    r._reply = reply;
    return r;
}

// Get the string field of the reply
char* CommandReply::str()
{
    if (_reply->type != REDIS_REPLY_STRING)
        throw SRRuntimeException("A pointer to the reply str "\
                                 "cannot be returned because "\
                                 "the reply type is " +
                                 redis_reply_type());
    return _reply->str;
}

// Get string field of REDIS_REPLY_STATUS
std::string CommandReply::status_str()
{
  if (_reply->type != REDIS_REPLY_STATUS)
    throw SRRuntimeException("A pointer to the reply str "\
                             "cannot be returned because "\
                             "the reply type is " +
                             redis_reply_type());
  return std::string(_reply->str, _reply->len);
}

// Get string field of REDIS_REPLY_DOUBLE
std::string CommandReply::dbl_str()
{
  if (_reply->type != REDIS_REPLY_DOUBLE)
    throw SRRuntimeException("A pointer to the reply str "\
                              "cannot be returned because "\
                              "the reply type is " +
                              redis_reply_type());
  return std::string(_reply->str, _reply->len);
}

// Get string field of REDIS_REPLY_BIGNUM
std::string CommandReply::bignum_str()
{
  if (_reply->type != REDIS_REPLY_BIGNUM)
    throw SRRuntimeException("A pointer to the reply str "\
                              "cannot be returned because "\
                              "the reply type is " +
                              redis_reply_type());
  return std::string(_reply->str, _reply->len);
}

// Get string field of REDIS_REPLY_VERB
std::string CommandReply::verb_str()
{
  if (_reply->type != REDIS_REPLY_VERB)
    throw SRRuntimeException("A pointer to the reply str "\
                              "cannot be returned because "\
                              "the reply type is " +
                              redis_reply_type());
  return std::string(_reply->str, _reply->len);
}

// Get the integer field of the reply
long long CommandReply::integer()
{
    if (_reply->type != REDIS_REPLY_INTEGER)
        throw SRRuntimeException("The reply integer "\
                                 "cannot be returned because "\
                                 "the reply type is " +
                                 redis_reply_type());
    return _reply->integer;
}

// Get the double field of the reply
double CommandReply::dbl()
{
    if (_reply->type != REDIS_REPLY_DOUBLE)
        throw SRRuntimeException("The reply double "\
                                 "cannot be returned because "\
                                 "the reply type is " +
                                 redis_reply_type());
    return _reply->dval;
}

// Index operator for CommandReply that will return the indexed element of
// the CommandReply if there are multiple elements
CommandReply CommandReply::operator[](int index)
{
    if (_reply->type != REDIS_REPLY_ARRAY)
        throw SRRuntimeException("The reply cannot be indexed "\
                                 "because the reply type is " +
                                 redis_reply_type());
    return shallow_clone(_reply->element[index]);
}

// Get the length of the CommandReply string field
size_t CommandReply::str_len()
{
    if (_reply->type != REDIS_REPLY_STRING)
        throw SRRuntimeException("The length of the reply str "\
                                 "cannot be returned because "\
                                 "the reply type is " +
                                 redis_reply_type());
    return _reply->len;
}

// Get the number of elements in the CommandReply
size_t CommandReply::n_elements()
{
    if (_reply->type != REDIS_REPLY_ARRAY)
        throw SRRuntimeException("The number of elements "\
                                 "cannot be returned because "\
                                 "the reply type is " +
                                 redis_reply_type());
    return _reply->elements;
}

// Return the number of errors in the CommandReply and any nested CommandReply
int CommandReply::has_error()
{
    int num_errors = 0;
    if (_reply->type == REDIS_REPLY_ERROR)
        num_errors++;
    else if (_reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < _reply->elements; i++) {
            CommandReply tmp = (*this)[i];
            num_errors += tmp.has_error();
        }
    }
    return num_errors;
}

// This will print any errors in the CommandReply or nested CommandReply.
void CommandReply::print_reply_error()
{
    if (_reply->type == REDIS_REPLY_ERROR) {
        std::string_view error(_reply->str,
                               _reply->len);
        std::cout<<error<<std::endl;
    }
    else if (_reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < _reply->elements; i++) {
            CommandReply tmp = (*this)[i];
            tmp.print_reply_error();
        }
    }
}

// This will return any errors in the CommandReply or nested CommandReply.
std::vector<std::string> CommandReply::get_reply_errors()
{
    std::vector<std::string> errors;
    std::queue<redisReply*> q;
    q.push(_reply);
    while (q.size() > 0) {
        redisReply* reply = q.front();
        q.pop();
        if (reply == NULL)
            continue;
        if (reply->type == REDIS_REPLY_ERROR)
            errors.push_back(std::string(reply->str, reply->len));
        else if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; i++)
                q.push(reply->element[i]);
        }
    }
    return errors;
}

// Return the type of the CommandReply in the form of a string.
std::string CommandReply::redis_reply_type()
{
    switch (_reply->type) {
        case REDIS_REPLY_STRING:
            return "REDIS_REPLY_STRING";
        case REDIS_REPLY_ARRAY:
            return "REDIS_REPLY_ARRAY";
        case REDIS_REPLY_INTEGER:
            return "REDIS_REPLY_INTEGER";
        case REDIS_REPLY_NIL:
            return "REDIS_REPLY_NIL";
        case REDIS_REPLY_STATUS:
            return "REDIS_REPLY_STATUS";
        case REDIS_REPLY_ERROR:
            return "REDIS_REPLY_ERROR";
        case REDIS_REPLY_DOUBLE:
            return "REDIS_REPLY_DOUBLE";
        case REDIS_REPLY_BOOL:
            return "REDIS_REPLY_BOOL";
        case REDIS_REPLY_MAP:
            return "REDIS_REPLY_MAP";
        case REDIS_REPLY_SET:
            return "REDIS_REPLY_SET";
        case REDIS_REPLY_ATTR:
            return "REDIS_REPLY_ATTR";
        case REDIS_REPLY_PUSH:
            return "REDIS_REPLY_PUSH";
        case REDIS_REPLY_BIGNUM:
            return "REDIS_REPLY_BIGNUM";
        case REDIS_REPLY_VERB:
            return "REDIS_REPLY_VERB";
        default:
            throw SRRuntimeException("Invalid Redis reply type");
  }
}

// Print the reply structure of the CommandReply
void CommandReply::print_reply_structure(std::string index_tracker)
{
    //TODO these recursive functions can't use 'this' unless
    //we have a constructor that takes redisReply*
    std::cout << index_tracker + " type: "
              << redis_reply_type()<<std::endl;
    switch (_reply->type) {
        case REDIS_REPLY_STRING:
            std::cout << index_tracker + " value: "
                      << std::string(str(), str_len())
                      << std::endl;
            break;
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < n_elements(); i++) {
              std::string r_prefix = index_tracker + "[" +
                                     std::to_string(i) + "]";
              CommandReply tmp = (*this)[i];
              tmp.print_reply_structure(r_prefix);
            }
            break;
        case REDIS_REPLY_INTEGER:
            std::cout << index_tracker + " value: "
                      << _reply->integer << std::endl;
            break;
        case REDIS_REPLY_DOUBLE:
            std::cout << index_tracker + " value: "
                      << _reply->dval<<std::endl;
            break;
        case REDIS_REPLY_ERROR:
            std::cout << index_tracker + " value: "
                      << std::string(str(), str_len())
                      << std::endl;
            break;
        case REDIS_REPLY_BOOL:
            std::cout << index_tracker + " value: "
                      << _reply->integer << std::endl;
            break;
        default:
            std::cout << index_tracker
                      <<"  value type not supported." << std::endl;
    }
}

redisReply* CommandReply::deep_clone_reply(const redisReply* reply)
{
    // GIGO: if the input is NULL, cloning it is easy
    if (reply == NULL)
        return NULL;
    redisReply* redis_reply = NULL;
    try {
        redis_reply = new redisReply;
    }
    catch (std::bad_alloc& e) {
        throw SRBadAllocException("Redis reply deep clone");
    }
    *redis_reply = *reply;
    redis_reply->str = NULL;
    redis_reply->element = NULL;
    switch (redis_reply->type) {
        case REDIS_REPLY_ARRAY:
        case REDIS_REPLY_MAP:
        case REDIS_REPLY_SET:
            // allocate memory for element and do deep copy
            if (redis_reply->elements > 0) {
                try {
                    redis_reply->element = new redisReply*[redis_reply->elements]{NULL};
                }
                catch (std::bad_alloc& e) {
                    throw SRBadAllocException("redis reply element");
                }
                if (reply->element != NULL) {
                    for(size_t i = 0; i < reply->elements; i++)
                        redis_reply->element[i] = deep_clone_reply(reply->element[i]);
                }
                else {
                    throw SRRuntimeException("The expected number of elements," +
                                             std::to_string(redis_reply->elements) +
                                             ", in the input redisReply is inconsistent "\
                                             "with the actual number of elements in the "\
                                             "input redisReply.");
                }
            }
            break;
        case REDIS_REPLY_ERROR:
        case REDIS_REPLY_STATUS:
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_DOUBLE:
            // allocate memory for str and do deep copy
            if (redis_reply->len > 0) {
                try {
                    redis_reply->str = new char[redis_reply->len];
                }
                catch (std::bad_alloc& e) {
                    throw SRBadAllocException("redis reply buffer");
                }
                std::memcpy(redis_reply->str, reply->str, redis_reply->len);
            }
            break;
        default:
            break;
    }
    return redis_reply;
}
