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

#include "c_client.h"
#include "c_dataset.h"
#include "c_client_test_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include "srexception.h"
#include "logger.h"

bool cluster = true;

#define TEST_LOG(logtype, loglevel, logmessage) \
log_##logtype(loglevel, logmessage, strlen(logmessage))

int main(int argc, char* argv[])
{
  int result = 0;
  void *client = NULL;
  const char* client_id = "client_test_logging";
  size_t cid_len = strlen(client_id);

  // Initialize client
  if (SRNoError != SmartRedisCClient(use_cluster(), client_id, cid_len, &client) || NULL == client)
    return -1;

  // Log stuff
  TEST_LOG(data, LLQuiet, "This is data logged at the Quiet level");
  TEST_LOG(data, LLInfo, "This is data logged at the Info level");
  TEST_LOG(data, LLDebug, "This is data logged at the Debug level");
  TEST_LOG(data, LLDeveloper, "This is data logged at the Developer level");

  TEST_LOG(warning, LLQuiet, "This is a warning logged at the Quiet level");
  TEST_LOG(warning, LLInfo, "This is a warning logged at the Info level");
  TEST_LOG(warning, LLDebug, "This is a warning logged at the Debug level");
  TEST_LOG(warning, LLDeveloper, "This is a warning logged at the Developer level");

  TEST_LOG(error, LLQuiet, "This is an error logged at the Quiet level");
  TEST_LOG(error, LLInfo, "This is an error logged at the Info level");
  TEST_LOG(error, LLDebug, "This is an error logged at the Debug level");
  TEST_LOG(error, LLDeveloper, "This is an error logged at the Developer level");

  // Done
  printf("Test passed: %s\n", result == 0 ? "YES" : "NO");
  return result;
}
