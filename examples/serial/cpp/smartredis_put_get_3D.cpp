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

#include "client.h"
#include <vector>
#include <string>
int main(int argc, char* argv[]) {

    // Initialize tensor dimensions
    size_t dim1 = 400;
    size_t dim2 = 300;
    size_t dim3 = 50;
    //x direction 3 elements - y 2 - z 5
    std::vector<size_t> dims = {400, 300, 50};

    // Initialize a tensor to random values.  Note that a dynamically
    // allocated tensor via malloc is also useable with the client
    // API.  The std::vector is used here for brevity.

    //how many elements will be stored in the tensor
    size_t n_values = dim1 * dim2 * dim3;
    std::string key = "3d_tensor";
    bool cluster_mode = false; // Set to false if not using a clustered database
    SmartRedis::Client client(cluster_mode, __FILE__);
    double*** input_tensor = (double***)malloc(dims[0]*sizeof(double**));

    for(size_t i=0; i<dims[0]; i++) {
        input_tensor[i] = (double**)malloc(dims[1]*sizeof(double*));
        for(size_t j=0; j<dims[1]; j++) {
            input_tensor[i][j] = (double*)malloc(dims[2]*sizeof(double));
        }
    }

    for(size_t i=0; i<dims[0]; i++)
        for(size_t j=0; j<dims[1]; j++)
            for(size_t k=0; k<dims[2]; k++)
                input_tensor[i][j][k] = ((double)rand())/(double)RAND_MAX;

    // Initialize a SmartRedis client
    // Put the tensor in the database
    for(size_t j=0; j<100; j++) {
        client.put_tensor(key, (void*)input_tensor, dims,
                        SRTensorTypeDouble, SRMemLayoutNested);
    }
    // Retrieve the tensor from the database using the unpack feature.
    // vector<vector<vector<size_t>>> vector_3d = 
    // std::vector<size_t> u_result = allocate_3D_array<size_t>(dims[0], dims[1], dims[2]);
    //std::vector<size_t> u_result[dim1][dim2][dim3];
    double*** u_result = (double***)malloc(dim1*sizeof(double**));
    for (int i=0; i<dim1; i++) {
        u_result[i] = (double**)malloc(dim2*sizeof(double*));
        for(int j=0; j<dim2; j++){
            u_result[i][j] = (double*)malloc(dim3 * sizeof(double));
        }
    }
    
    client.unpack_tensor(key, u_result, dims,
                        SRTensorTypeDouble, SRMemLayoutNested);

    // Retrieve the tensor from the database using the get feature.
    SRTensorType get_type;
    std::vector<size_t> get_dims;
    void* get_tensor;
    client.get_tensor(key, get_tensor, get_dims, get_type, SRMemLayoutNested);
    return 0;
}
