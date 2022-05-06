#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "threadpool.h"
#include "srexception.h"

using namespace SmartRedis;

#define TIME_THREADPOOL

// C-tor
ThreadPool::ThreadPool(unsigned int num_threads)
{
    // By default, we'll make one thread for each hardware context
    if (num_threads == 0)
        num_threads = std::thread::hardware_concurrency();

    // Create worker threads
	if (num_threads < 1) num_threads = 1; // Force a minimum of 1 thread
    for (unsigned int i = 0; i < num_threads; i++) {
        threads.push_back(std::thread(&ThreadPool::perform_jobs, this, i));
    }

    // Announce that we're open for business
    shutting_down = false;
    shutdown_complete = false;
}

// D-tor
ThreadPool::~ThreadPool()
{
    // Shut down the threadpool if it hasn't already been stopped
    if (!shutdown_complete) {
        shutdown();
    }
}

// Shut down the thread pool, blocking any further jobs from submission
void ThreadPool::shutdown()
{
    // We're closed for business
    shutting_down = true;

    // Wait for worker threads to finish up
    cv.notify_all(); // Wake them up
    for (std::thread& thr : threads) {
        thr.join(); // Blocks until the thread finishes execution
    }

    // Done
    shutdown_complete = true;
}

// Worker thread main loop to acquire and perform jobs
volatile bool dummy = false;
void ThreadPool::perform_jobs(unsigned int tid)
{
    const int spin_count = 1000;
    #ifdef TIME_THREADPOOL
    int jobid = 0;
    std::cout << "Thread " << std::to_string(tid) << " reporting for duty" << std::endl;
    #endif

    // Loop foorever processing jobs until we get killed
    std::function<void()> job;
    while (!shutting_down)
    {
        #ifdef TIME_THREADPOOL
        auto start = std::chrono::steady_clock::now();
        #endif

        // Spin for a bit to see if a job appears
        for (int i = 0; i < spin_count && !!shutting_down; i++) {
            if (!jobs.empty()) // Benign race condition; risk of false positive
                break;
        }

        // Get a job, blocking until one appears if none immediately available
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (jobs.empty()) {
                cv.wait(lock, [this](){
                    return !jobs.empty() || shutting_down;
                });
            }

            // There is a job there for us
            if (!shutting_down) {
                job = jobs.front();
                jobs.pop();
            }
        } // End scope and release lock

        #ifdef TIME_THREADPOOL
        auto have_job = std::chrono::steady_clock::now();
        #endif
        // Perform the job
        if (!shutting_down) {
            job();
            #ifdef TIME_THREADPOOL
            auto job_done = std::chrono::steady_clock::now();
            std::chrono::duration<double> get_job = have_job - start;
            std::chrono::duration<double> execute_job = job_done - have_job;
            std::cout << "Thread " << std::to_string(tid) << " "
                      << "time to get job " << std::to_string(jobid++) << ": " << get_job.count() << " s; "
                      << "time to execute job: " << execute_job.count() << " s" << std::endl;
            #endif
        }
    }
    #ifdef TIME_THREADPOOL
    std::cout << "Thread " << std::to_string(tid) << " shutting down" << std::endl;
    #endif
}

// Submit a job to threadpool for execution
void ThreadPool::submit_job(std::function<void()> job)
{
    // If the threadpool is shutting down, reject the job
    if (shutting_down) {
        throw SRInternalException(
            "Attempt made to add job to shutdown threadpool");
    }

    // Add the job to our queue
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    } // End scope and release lock

    // Wake up a sleeping worker to process it
    cv.notify_one();
}

