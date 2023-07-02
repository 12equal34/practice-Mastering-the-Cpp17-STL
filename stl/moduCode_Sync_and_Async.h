#pragma once

#include <string>
#include <future>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <execution>

//-----------------------------------------------------------------------------
// ����� �񵿱� ����
//-----------------------------------------------------------------------------
namespace section1
{
// SSD�� �ƴ� �ϵ� ��ũ���� ������ �о���� �ð��� ����� ���� �ɸ���.
// �뷫 4.17 �и��ʰ� �ɸ��� ������ �����͸� �о���µ� 50�����ʿ� ����
// 8���� ���� ������.
// string txt    = read("a.txt");              // 5ms �ҿ� (�ϵ��ũ �б�)
// string result = do_something_with_txt(txt); // 5ms �ҿ� (cpu ����)
// do_other_computation(); // 5ms �ҿ� (cpu ����)
// �� 15ms�� �ҿ�ȴ�.
// read �Լ��� ������ �ϵ� ��ũ���� �о���� ����, CPU�� ��ٸ��� ������
// ��ȿ�����̴�. �̷��� ���������� �����ϴ� �۾��� ������ (synchronous)����
// �����Ѵٰ� �Ѵ�. �̷��� ��쿡 read �Լ��� �ϵ� ��ũ���� �����͸� �о����
// ���ȿ� CPU�� do_other_computation�� �۾��ϴ� ���� ȿ������ ���̴�.

// C++�� ������� ������ ���� ������ �� �ִ�.
//		void file_read(string* result) {
//			string txt = read("a.txt"); // 5ms�ҿ� (�ϵ��ũ �б�)
//          *result = do_something_with_txt(txt); // 5ms �ҿ� (cpu ����)
//		}
//		int main()
//		{
//			string result;
//			thread t(file_read, &result);
//			do_other_computation();  // 5ms �ҿ� (cpu ����)
//
//			t.join();
//		}

// �� 10ms�� �ҿ�ȴ�.
// �̷��� ���α׷��� ������ ���� ������ �������� ���ÿ� ����Ǿ�
// �񵿱��� (asynchronous) �����̶�� �θ���.
// c++11 ���� ǥ�� ���̺귯���� ���� ������ �񵿱��� ������ �� �� �ִ�.
}

//-----------------------------------------------------------------------------
// std::promise �� std::future
//-----------------------------------------------------------------------------
namespace section2
{
// �񵿱��� �������� ������ ���� �۾��� ���Ѵ�.
// "������ T�� ����ؼ� �񵿱������� T�� �۾��� �Ϸ�Ǹ� ���ϰ��� �޴´�."
// �̸� �ٽ� ���ϸ�,
// "future�� ������ T�� ��ӵ� �����͸� ���� �ְڴٴ� promise�� �Ѵ�."
void worker(std::promise<std::string>& p)
{
    // promise ��ü�� ���� �ִ� future ��ü�� ���� �ִ´�.
    p.set_value("some data");
}
void test()
{
    std::promise<std::string> p;
    std::future<std::string>  data = p.get_future();
    std::thread               t(worker, std::ref(p));

    // promise�� future�� ���� �ֱ� ������ wait �Լ��� ��ٸ���.
    data.wait();
    // wait�� �����ϸ� future�� ���� �־��� �����̴�.

    // get�� ���� future�� ���޵� ��ü�� �̵���Ų��.
    // �̵���Ű�Ƿ� ����� �ι� ȣ���ϸ� �ȵȴ�.
    std::cout << "���� ������: " << data.get() << std::endl;

    t.join();
}
// get�� ���� wait�� ���ص� promise�� future�� ��ü�� ������ ������
// ��ٸ� �Ŀ� �����Ѵ�.

// ������-�Һ��� ���Ͽ���
// promise�� producer ������,
// future�� consumer ������ �����Ѵ�.
// �׷��� promise-future ������ ������ ���� ������ �� �ִ�.
namespace promise_future_implementation
{
    std::condition_variable cv;
    std::mutex              m;
    bool                    done = false;
    std::string             info;

    void worker()
    {
        {
            std::lock_guard<std::mutex> lk(m);
            info = "some data"; // p.set_value("some data")�� �����ȴ�.
            done = true;
        }
        cv.notify_all();
    }
    void test()
    {
        std::thread t(worker);

        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] { return done; }); // data.wait()�� �����ȴ�.
        lk.unlock();

        std::cout << "���� ������: " << info << std::endl;

        t.join();
    }
}
// ������ �������� �������Ƿ� promise-future�� �̿��ϴ� ���� ����.
// ���� future�� exception�� ���� �� �ִ�.
namespace exception_with_future
{
    using std::string;
    void worker(std::promise<string>& p)
    {
        try {
            throw std::runtime_error("Some Error!");
        } catch (...) {
            // �Ű����� exception_ptr�� �޴´�.
            p.set_exception(std::current_exception());
        }
    }
    void test()
    {
        std::promise<string> p;
        std::future<string>  data = p.get_future();
        std::thread          t(worker, std::ref(p));

        data.wait();

        try {
            data.get();
        } catch (const std::exception& e) {
            std::cout << "����: " << e.what() << std::endl;
        }
        t.join();
    }
}

namespace wait_for
{
    void worker(std::promise<void>& p)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        p.set_value();
    }
    void test()
    {
        // ��� ��ü�� �������� �ʰ� void�� �����Ѵ�.
        // future�� set�� �Ǿ����� �÷��� ������ �Ѵ�.
        std::promise<void> p;
        std::future<void>  data = p.get_future();
        std::thread        t(worker, std::ref(p));

        while (true) {
            // promise �� ������ ������ ������ ��ٸ��� �ʰ�,
            // ���޵� �ð�(1��)��ŭ ��ٸ��� �����Ѵ�.
            std::future_status status = data.wait_for(std::chrono::seconds(1));

            // ���ϵǴ� ���� future_status�� �� 3���� ���¸� ������.
            // ���� �����Ǿ���: future_status::ready
            // ���� ������������: future_status::timeout
            // �Լ��� �����������: future_status::deferred
            if (status == std::future_status::timeout) {
                std::cerr << ">";
            } else if (status == std::future_status::ready) {
                break;
            }
        }
        t.join();
    }
}
}

//-----------------------------------------------------------------------------
// shared_future
//-----------------------------------------------------------------------------
namespace section3
{
// ���� ������ future�� get�� �ѹ��� ȣ���� �� �ִٰ� �Ͽ���.
// �ֳ��ϸ� future ������ ��ü�� �̵��Ǳ� �����̴�.
// ������ ���� ���� �ٸ� �����忡�� future�� get�� �ʿ䰡 ���� ��,
// shared_future�� ����Ѵ�.
// future�ʹ� �ٸ��� ���簡 �����ϹǷ� �� ������ �ص� �ȴ�.
// ���� ��ü�� ���纻�� ���� �����Ѵ�.
namespace shared_future
{
    using std::thread;

    void runner(std::shared_future<void> start)
    {
        // set �Ǳ���� ��ٸ���.
        start.get();
        std::cout << "���!" << std::endl;
    }
    void test()
    {
        std::promise<void>       p;
        std::shared_future<void> start = p.get_future();

        thread t1(runner, start);
        thread t2(runner, start);
        thread t3(runner, start);
        thread t4(runner, start);

        std::cerr << "�غ�...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << "��!" << std::endl;

        p.set_value();

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}
}

namespace section4
{
// �񵿱�������,
// Callable �Լ��� ���ϰ��� promise�� set_value�� �������ְ�,
// Callable �Լ��� ���ܰ��� promise�� set_exception�� �������ִ� ����
// ���ϰ� �����ϴ� packaged_task�� �ִ�.
// get_future�� ���� �ش��ϴ� future�� �����Ѵ�.
// ��, �����忡 ���� promise�� �������� �ʰ� �Լ����� �����ؼ� �񵿱��� ������
// �Ѵ�.
namespace package_task
{
    int some_task(int x) { return 10 + x; }

    void test()
    {
        std::packaged_task<int(int)> task(some_task);
        std::future<int>             start = task.get_future();
        // packaged_task�� ���� �����ڰ� ��� �̵� �����ؾ� �Ѵ�.
        std::thread t(std::move(task), 5);

        std::cout << "����� : " << start.get() << std::endl;
        t.join();
    }
}

// promise�� packaged_task�� �񵿱������� �����ϱ� ���� �����带 ���������
// �����ߴ�.
// std::async�� Callable �Լ��� �����ϸ� �����带 �˾Ƽ� �����Ͽ� �񵿱�������
// �����ϰ� ������� future�� �����Ѵ�.
namespace std_async
{
    int sum(const std::vector<int>& v, size_t start, size_t end)
    {
        int total = 0;
        for (size_t i = start; i < end; ++i) {
            total += v[i];
        }
        return total;
    }

    int parallel_sum(const std::vector<int>& v)
    {
        // std::async�� ù��° ���ڷ� �� ���� ���� �����ϴ�.
        // std::launch::async : ��ٷ� �����带 �����Ͽ� �Լ��� �����Ѵ�.
        // std::launch::deferred : �����带 �������� �ʰ� future�� get��
        // ȣ��Ǿ��� �� �����Ѵ�. ���� �񵿱������� ������ �ʿ���� �� ���.
        std::future<int> lower_half_future =
            std::async(std::launch::async, sum, cref(v), 0, v.size() / 2);

        int upper_half = sum(v, v.size() / 2, v.size());

        return lower_half_future.get() + upper_half;
    }
    void test()
    {
        std::vector<int> v;
        v.reserve(1000);
        for (int i = 0; i < v.capacity(); ++i) {
            v.push_back(i + 1);
        }
        std::cout << "1 ���� 1000 ������ ��: " << parallel_sum(v) << std::endl;
    }
}

namespace example
{
    int do_work(int x)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return x;
    }
    void do_work_parallel()
    {
        auto f1 = std::async([] { do_work(3); });
        auto f2 = std::async([] { do_work(3); });
        do_work(3);

        f1.get();
        f2.get();
    }
    void do_work_sequential()
    {
        do_work(3);
        do_work(3);
        do_work(3);
    }

    void test()
    {
        auto clock = std::chrono::system_clock();

        auto start = clock.now();
        do_work_parallel();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            clock.now() - start);
        std::cout << "parallel time: " << duration << std::endl;

        start = clock.now();
        do_work_sequential();
        duration = std::chrono::duration_cast<std::chrono::seconds>(
            clock.now() - start);
        std::cout << "sequential time: " << duration << std::endl;
    }
}
}