#include "client.h"
#include <mpi.h>

void run_mnist(const std::string& model_name,
               const std::string& script_name)
{
    // Get the MPI rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize a vector that will hold input image tensor
    size_t n_values = 1*1*28*28;
    std::vector<float> img(n_values, 0);

    // Load the mnist image from a file using MPI rank 0
    if(rank==0) {
        std::string image_file = "../../../common/mnist_data/one.raw";
        std::ifstream fin(image_file, std::ios::binary);
        std::ostringstream ostream;
        ostream << fin.rdbuf();
        fin.close();

        const std::string tmp = ostream.str();
        std::memcpy(img.data(), tmp.data(), img.size()*sizeof(float));
    }

    // Broadcast the image to all MPI ranks.  This is more efficient
    // thank all ranks loading the same file.  This is specific
    // to this example.
    MPI_Bcast(img.data(), 28*28, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0)
        std::cout<<"All ranks have MNIST image"<<std::endl;

    // Declare keys that we will use in forthcoming client commands
    std::string in_key = "mnist_input_rank_" + std::to_string(rank);
    std::string script_out_key = "mnist_processed_input_rank_" +
                                 std::to_string(rank);
    std::string out_key = "mnist_output_rank_" + std::to_string(rank);

    // Initialize a Client object
    SmartRedis::Client client(false);

    // Put the image tensor on the database
    client.put_tensor(in_key, img.data(), {1,1,28,28},
                      SmartRedis::TensorType::flt,
                      SmartRedis::MemoryLayout::contiguous);

    // Run the preprocessing script
    client.run_script(script_name, "pre_process",
                      {in_key}, {script_out_key});

    // Run the model
    client.run_model(model_name, {script_out_key}, {out_key});

    // Get the result of the model
    std::vector<float> result(1*10);
    client.unpack_tensor(out_key, result.data(), {10},
                         SmartRedis::TensorType::flt,
                         SmartRedis::MemoryLayout::contiguous);

    // Print out the results of the model for Rank 0
    if(rank==0)
        for(size_t i=0; i<result.size(); i++)
            std::cout<<"Rank 0: Result["<<i<<"] = "<<result[i]<<std::endl;

    return;
}

int main(int argc, char* argv[]) {

    // Initialize the MPI comm world
    MPI_Init(&argc, &argv);

    // Retrieve the MPI rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Set the model and script that will be used by all ranks
    // from MPI rank 0.
    if(rank==0) {
        SmartRedis::Client client(false);

        // Build model key, file name, and then set model
        // from file using client API
        std::string model_key = "mnist_model";
        std::string model_file = "../../../"\
                                 "common/mnist_data/mnist_cnn.pt";
        client.set_model_from_file(model_key, model_file,
                                "TORCH", "CPU", 20);

        // Build script key, file name, and then set script
        // from file using client API
        std::string script_key = "mnist_script";
        std::string script_file = "../../../common/mnist_data/"
                                "data_processing_script.txt";
        client.set_script_from_file(script_key, "CPU", script_file);

        // Get model and script to illustrate client API
        // functionality, but this is not necessary for this example.
        std::string_view model = client.get_model(model_key);
        std::string_view script = client.get_script(script_key);
    }

    // Run the MNIST model
    MPI_Barrier(MPI_COMM_WORLD);
    run_mnist("mnist_model", "mnist_script");

    if(rank==0)
        std::cout<<"Finished SmartRedis MNIST example."<<std::endl;

    // Finalize MPI Comm World
    MPI_Finalize();

    return 0;
}