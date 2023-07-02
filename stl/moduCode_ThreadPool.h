#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <type_traits>
#include <algorithm>

namespace ThreadPool
{
class ThreadPool
{
public:
    ThreadPool(size_t num_workers);
    ~ThreadPool();

    template <class F, class... Args>
    std::future<std::invoke_result_t<F, Args...>> EnqueueJob(F&& f,
                                                             Args&&... args);

private:
    size_t                            m_num_workers;
    std::vector<std::thread>          m_workers;
    std::queue<std::function<void()>> m_job_queue;
    std::condition_variable           m_cv_job_queue;
    std::mutex                        m_mutex_job_queue;
    bool                              m_stop_all;

    void WorkerThread();
};

ThreadPool::ThreadPool(size_t num_workers)
    : m_num_workers(num_workers),
      m_stop_all(false)
{
    m_workers.reserve(num_workers);
    for (int i = 0; i < num_workers; ++i) {
        m_workers.emplace_back([this] { this->WorkerThread(); });
    }
}

void ThreadPool::WorkerThread()
{
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex_job_queue);

        m_cv_job_queue.wait(
            lock, [this] { return !this->m_job_queue.empty() || m_stop_all; });
        if (m_job_queue.empty() && m_stop_all) {
            return;
        }

        auto job {std::move(m_job_queue.front())};
        m_job_queue.pop();
        lock.unlock();

        job();
    }
}

ThreadPool::~ThreadPool()
{
    m_stop_all = true;
    m_cv_job_queue.notify_all();

    for (auto& t : m_workers) {
        t.join();
    }
}

template <class F, class... Args>
std::future<std::invoke_result_t<F, Args...>>
ThreadPool::EnqueueJob(F&& f, Args&&... args)
{
    if (m_stop_all) {
        throw std::runtime_error("You can't enqueue a job for a thread pool is "
                                 "already stopped to use.");
    }

    using return_type = std::invoke_result_t<F, Args...>;

    auto job = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto job_result = job->get_future();

    {
        std::lock_guard<std::mutex> lock(m_mutex_job_queue);
        m_job_queue.push([job] { (*job)(); });
    }
    m_cv_job_queue.notify_one();

    return job_result;
}

namespace example
{
    int work(int t, int id)
    {
        printf("%d번째 job 이 start! \n", id);
        std::this_thread::sleep_for(std::chrono::seconds(t));
        printf("%d번째 job이 end! (소요시간: %ds) \n", id, t);
        return t + id;
    }
    void test()
    {
        ThreadPool pool(3);

        std::vector<std::future<int>> futures;
        for (int i = 0; i < 10; ++i) {
            futures.emplace_back(pool.EnqueueJob(work, i % 3 + 1, i));
        }
        for (auto& f : futures) {
            printf("result: %d \n", f.get());
        }
    }
}

}
