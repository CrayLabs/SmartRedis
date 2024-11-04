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

#include "client.h"
#include <vector>
#include <string>

int main(int argc, char* argv[]) {

  // Initialize some byte data
  size_t n_bytes = 255;
  std::vector<char> input_bytes(n_bytes);
  for(size_t i = 0; i < n_bytes; i++) {
    input_bytes[i] = i;
  }

  SmartRedis::Client client("client_test_put_get_bytes");

  std::string key = "put_get_bytes_test";

  client.put_bytes(key, input_bytes.data(), n_bytes);

  std::vector<char> output_bytes(n_bytes, 0);

  size_t received_bytes = 0;
  client.unpack_bytes(key, output_bytes.data(), n_bytes, 
                      received_bytes);

    if (received_bytes != n_bytes) {
      std::cout<<"Output byte size "<<received_bytes<<" does not match."<<std::endl;
        throw std::exception();
    }

  for(int i = 0; i < n_bytes; i++) {
    if (output_bytes[i] != input_bytes[i]) {
      std::cout<<"Byte "<<i<<" does not match."<<std::endl;
      throw std::exception();
    }
  }

  std::cout<<"Put bytes test complete"<<std::endl;

  return 0;
}
