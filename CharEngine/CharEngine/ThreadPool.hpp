#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>
#include <vector>
#include <algorithm>

class ThreadPool
{
public:

    ThreadPool(int threads) : shutdown_(false)
    {
        // Create the specified number of threads
        threads_n = threads;
        occupied_threads_ = new bool[threads];
        threads_.reserve(threads);

        for (int i = 0; i < threads; ++i)
        {
            threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
            occupied_threads_[i] = false;
        }
    }

    ~ThreadPool()
    {
        {
            // Unblock any threads and tell them to stop
            std::unique_lock <std::mutex> l(lock_);

            shutdown_ = true;
            condVar_.notify_all();
        }

        // Wait for all threads to stop
        std::cout << "Joining threads" << std::endl;
        for (auto& thread : threads_)
            thread.join();
    }

    void doJob(std::function <void(void)> func)
    {
        // Place a job on the queu and unblock a thread
        std::unique_lock <std::mutex> l(lock_);

        jobs_.emplace(std::move(func));
        condVar_.notify_one();
    }

    bool isFinished()
    {
        bool v = true;
        for (int i = 0; i < threads_n; i++)
        {
            if (occupied_threads_[i] == true)
            {
                v = false;
                break;
            }
        }

        return(v && jobs_.empty());
    }

protected:

    void threadEntry(int i)
    {
        std::function <void(void)> job;

        while (1)
        {
            {
                std::unique_lock <std::mutex> l(lock_);

                while (!shutdown_ && jobs_.empty())
                    condVar_.wait(l);

                //occupied_threads_[i] = true;

                if (jobs_.empty())
                {
                    std::cerr << "Thread " << i << " terminates" << std::endl;
                    return;
                }
               
                occupied_threads_[i] = true;
                job = std::move(jobs_.front());
                jobs_.pop();
                
            }

            // Do the job without holding any locks
            job();
            occupied_threads_[i] = false;
        }

    }

    std::mutex lock_;
    std::condition_variable condVar_;
    bool shutdown_;
    std::queue <std::function <void(void)>> jobs_;
    std::vector <std::thread> threads_;
    bool* occupied_threads_;
    int threads_n;
};
