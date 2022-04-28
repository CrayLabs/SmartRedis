#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>

#ifndef SMARTREDIS_THREADPOOL_H
namespace SmartRedis {

class ThreadPool
{
  public:
    // C-tor
    ThreadPool(unsigned int num_threads=0);

    // D-tor
    ~ThreadPool();

    // Shut down the thread pool
    void shutdown();

    // Worker thread main loop to acquire and perform jobs
    void perform_jobs();

    // Submit a job to threadpool for execution
    void submit_job(std::function<void()> job);

  protected:
    // The threads in our worker pool
    std::vector<std::thread> threads;

    // The current task queue of jobs waiting to be performed
    std::queue<std::function<void()>> jobs;

    // Lock, protecting the job queue
    std::mutex queue_mutex;

    // Condition variable for signalling worker threads
    std::condition_variable cv;

    // Flag for if the thread pool is shutting down
    volatile bool shutting_down;

    // Flag for if the thread pool shut down has completed
    volatile bool shutdown_complete;
};

} // namespace SmartRedis
#endif // SMARTREDIS_THREADPOOL_H
