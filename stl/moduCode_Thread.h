#pragma once
// ����� �ڵ�
// https://modoocode.com/269
// ���� ������ ������� �����Ѵ�.
#pragma warning(disable : 4477)

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <functional>
#include <cassert>
#include <string>
#include <format>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>

namespace section1
{
void func1()
{
    for (int i = 0; i < 10; ++i) {
        std::cout << "������ 1 �۵��� \n";
    }
}

void func2()
{
    for (int i = 0; i < 10; ++i) {
        std::cout << "������ 2 �۵��� \n";
    }
}

void func3()
{
    for (int i = 0; i < 10; ++i) {
        std::cout << "������ 3 �۵��� \n";
    }
}

void how_to_thread()
{
    // ������ t1�� �Լ� func1�� �޾� �����ϰ�, func1�� ������ t1���� �����Ѵ�.
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);

    // ������ t1�� ������ ����Ǹ� �����Ѵ�.
    t1.join();
    t2.join();
    t3.join();
    // ���⼭ joins�� ���� ������ �������� �۾����� ������ ���� main�Լ���
    // ����Ǿ ������ ��ü�� t1,t2,t3�� �Ҹ��ڰ� ȣ��ȴ�. join�ǰų�
    // detach���� ���� �������� �Ҹ��ڰ� ȣ��ǹǷ� ���ܸ� ������ �ȴ�.
}

void what_is_detach()
{
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);

    // main�Լ��� ������ ���� �ش� �����尡 ���� �ʿ䰡 ������ ����Ѵ�.
    t1.detach();
    t2.detach();
    t3.detach();

    std::cout << "main function end";
}
}

namespace section2
{
template <class BidirIt>
void worker(BidirIt first, BidirIt last, int& result)
{
    int sum = 0;
    for (auto it = first; it != last; ++it) {
        sum += *it;
    }
    result = sum;

    auto this_id = std::this_thread::get_id();
    printf("������ %x ���� %d ���� %d ���� ����� ��� : %d \n", this_id,
           *first, *--last, sum);
}

void test()
{
    constexpr int    data_size = 10000;
    std::vector<int> data(data_size);
    for (int i = 0; i < data_size; i++) {
        data[i] = i;
    }

    std::vector<int> partial_sums(4);

    std::vector<std::thread> workers;
    workers.reserve(4);
    auto begin = data.begin();
    for (int i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back(worker<decltype(begin)>, begin + i * 2500,
                             begin + (i + 1) * 2500, std::ref(partial_sums[i]));
    }

    for (int i = 0; i < 4; ++i) {
        workers[i].join();
    }

    int total = 0;
    for (int i = 0; i < 4; ++i) {
        total += partial_sums[i];
    }
    std::cout << "��ü ��: " << total << std::endl;
}
}

//-----------------------------------------------------------------------------
// �޸𸮸� ���� �����Ѵٸ�?
//-----------------------------------------------------------------------------
namespace section3
{
void worker(int& counter)
{
    for (int i = 0; i < 10000; ++i) {
        counter += 1;
    }
}
void test()
{
    int                      counter = 0;
    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back(worker, std::ref(counter));
    }

    for (int i = 0; i < 4; ++i) {
        workers[i].join();
    }

    std::cout << "Counter ���� ��: " << counter << std::endl;
}
}

//-----------------------------------------------------------------------------
// std::mutex : �� �����忡���� �ڵ带 �����ų �� �ְ� ����Ѵ�.
//-----------------------------------------------------------------------------
namespace section4
{
void worker(int& result, std::mutex& m)
{
    for (int i = 0; i < 10000; ++i) {
        // ���ؽ� m�� �������� ���� �����尡 ���� �� �ִ�.
        // m.lock()�� ���ؽ� m�� �������� �����´�.
        // �׷��� m�� ������ �����尡 m.unlock()�� �Ҷ����� ������ ��ٸ���.
        // m.unlock()�� ���ؽ� m�� �������� ��ȯ�Ѵ�.

        m.lock();
        // �� ������ ������ �ϳ��� �����忡���� �����ϰ� ������ �� �ִ� �ڵ�
        // �����̴�. �̸� �Ӱ� ���� (critical section) �̶�� �θ���.

        result += 1;

        //
        m.unlock();

        // �Ǽ��� m.unlock()�� ���� ������ �� ���α׷��� m�� �������� ����
        // ��������� .join() ȣ�⿡�� ������ ��ٸ� ���̶�� ������ �� �ִ�.
        // ������ �� ������ m�� ���� �ִ� �����尡 �ݺ������� �ٽ� m.lock()��
        // ȣ���ϹǷ� �ڱ� �ڽŵ� �ڽ��� ���ؽ��� ��ȯ�ϱ⸦ ��ٸ��� �ȴ�. �ᱹ
        // �ƹ� �����嵵 �������� �ʰ� �Ǵµ�, �̸� ����� (deadlock)�̶��
        // �θ���.
    }
}
void test()
{
    int        counter = 0;
    std::mutex m;

    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back(worker, std::ref(counter), std::ref(m));
    }

    for (int i = 0; i < 4; ++i) {
        workers[i].join();
    }

    std::cout << "Counter ���� ��: " << counter << std::endl;
}

// �Ǽ��� m.unlock()�� ���߸��� ������ ������� �߻��ϴµ�
// �̸� �����ϱ� ���� unique_ptró�� �Ҹ��ڿ��� m.unlock�� �ϵ��� ó���Ѵ�.
void worker_with_lock_guard(int& result, std::mutex& m)
{
    for (int i = 0; i < 10000; ++i) {
        // �����ڰ� m.lock()�� ȣ���Ѵ�.
        std::lock_guard<std::mutex> lock(m);

        // critical section
        result += 1;

        //

        // �Ҹ��ڰ� m.unlock()�� ȣ���Ѵ�.
    }
}
void test_with_lock_guard()
{
    int        counter = 0;
    std::mutex m;

    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back(worker_with_lock_guard, std::ref(counter),
                             std::ref(m));
    }

    for (int i = 0; i < 4; ++i) {
        workers[i].join();
    }

    std::cout << "Counter ���� ��: " << counter << std::endl;
}

// ������ ������� �ٸ� ������ε� �߻��� �� �ִ�.
void worker1(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10000; ++i) {
        // ���� t1�� ���� ���ε�,
        std::lock_guard<std::mutex> lock1(m1); // t2�� m1�� ���� �ִٰ� ����.
        // t2�� m1�� ��ȯ�� ������ ������ ��ٸ���.

        std::lock_guard<std::mutex> lock2(m2);
        // �� ���� �Ӱ迵�� m1,m2�� �����ִ�.
    }
}
void worker2(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10000; ++i) {
        // ���� t2�� ���� ���ε�,
        std::lock_guard<std::mutex> lock2(m2); // t1�� m2�� ���� �ִٰ� ����.
        // t1�� m2�� ��ȯ�� ������ ������ ��ٸ���.
        // ������ t1�� m2�� ��ȯ�Ϸ��� t2�� m1�� ��ȯ�ؾ� �ϰ�
        // t2�� m1�� ��ȯ�Ϸ��� t1�� m2�� ��ȯ�ؾ� �Ѵ�. (����� �߻�!)

        std::lock_guard<std::mutex> lock1(m1);
        // �� ���� �Ӱ迵�� m1,m2�� �����ִ�.
    }
}
void test_deadlock()
{
    std::mutex m1, m2;

    std::thread t1(worker1, std::ref(m1), std::ref(m2));
    std::thread t2(worker2, std::ref(m1), std::ref(m2));

    t1.join();
    t2.join();

    std::cout << "finish" << std::endl;
}

// ���� ����� ������ �ذ��غ���.
void worker1_no_deadlock(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10; ++i) {
        // �켱���� ���� worker1�� �ڵ带 �ٲ� �ʿ䰡 ����.
        m1.lock();
        m2.lock();
        // ���� t1�� m1,m2�� �������� ������.

        std::cout << "Worker1 Hi! " << i << std::endl;

        m2.unlock();
        m1.unlock();
    }
}
void worker2_no_deadlock(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10; ++i) {
        while (true) {
            m2.lock();
            // ���� t2�� m2�� �������� ������.

            // m1.try_lock()�� m1�� lock�� �� �� �ִٸ� lock�� �ϰ� true��
            // �����Ѵ�. lock�� �� �� ���ٸ� ��ٸ��� �ʰ� false�� �����Ѵ�.

            // ���� t2�� m1�� �������� ������ �Ѵ�.
            // ���� �ٷ� m1�� �������� ������ �� ���ٸ�,
            if (!m1.try_lock()) {
                // ���� ���� �ִ� m2�� �������� ��ȯ�Ѵ�.
                m2.unlock();
                continue;
            }
            // ���� t2�� m1,m2�� �������� ������.

            std::cout << "Worker2 Hi! " << i << std::endl;

            m1.unlock();
            m2.unlock();
            break;
        }
    }
}
void test_no_deadlock()
{
    std::mutex m1, m2;

    std::thread t1(worker1_no_deadlock, std::ref(m1), std::ref(m2));
    std::thread t2(worker2_no_deadlock, std::ref(m1), std::ref(m2));

    t1.join();
    t2.join();

    std::cout << "finish" << std::endl;
}
}

//-----------------------------------------------------------------------------
// �����ڿ� �Һ��� ����
//-----------------------------------------------------------------------------
namespace section5
{
// Producer�� ó���ؾ� �� ���� �������� �������̴�.
// Consumer�� �����ڷκ��� ���� ���� ó���ϴ� �������̴�.

// ���� ���, ���ͳݿ��� �������� �����ͼ� �м��ϴ� ���α׷��� �ۼ��ϴ� ��찡
// �ִ�.
void producer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int thread_index)
{
    for (int i = 0; i < 5; ++i) {
        // ������Ʈ�� �ٿ��ϴµ� ������ ���� �ð��� �ɸ��ٰ� ����.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 * thread_index));
        // ó���ؾ� �� ���� �����´�.
        std::string content = "������Ʈ: " + std::to_string(i) +
                              " from thread(" + std::to_string(thread_index) +
                              ")\n";

        // downloaded_pages�� ���� �����尡 �����ϹǷ� �Ӱ� ������ �ִ´�.
        m.lock();
        // ó���ؾ� �� ���� �ٸ� ������� �����ϱ� ���� �����Ѵ�.
        downloaded_pages.push(content);
        m.unlock();
    }
}

void consumer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int& num_processed)
{
    // ��ü ó���ϴ� ������ ���� : 5*5 = 25��
    while (num_processed < 25) {
        m.lock();

        if (downloaded_pages.empty()) {
            m.unlock();
            // ������Ʈ ������ ������ ť�� �߰��Ǵ� �ӵ��� �ſ� �����Ƿ�
            // sleep ���� ������ ��� empty()�� ȣ���ϹǷ� CPU �ڿ� ����
            // �߻��Ѵ�.
            // (������, �Ź� ���� ������ �𸣴� �����͸� Ȯ���ϱ� ����
            // �̷��� �ؾ� �ұ�? �� ��ĺ��ٴ� producer ���� �����Ͱ� ����
            // consumer�� ����� ����� ����. ���� ������ �����Ѵ�.)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            continue;
        }

        auto content {downloaded_pages.front()};
        downloaded_pages.pop();
        ++num_processed;

        m.unlock();

        // �����ڷκ��� ���� �������� ó���ϴµ� �ּ� 80�и��ʰ� �ɸ��ٰ� ����.
        std::cout << content;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

void test()
{
    std::queue<std::string>  downloaded_pages;
    std::mutex               m;
    std::vector<std::thread> producers;
    producers.reserve(5);
    for (int i = 0; i < producers.capacity(); ++i) {
        producers.emplace_back(producer, std::ref(downloaded_pages),
                               std::ref(m), i + 1);
    }

    int                      num_processed = 0;
    std::vector<std::thread> consumers;
    consumers.reserve(3);
    for (int i = 0; i < consumers.capacity(); ++i) {
        consumers.emplace_back(consumer, std::ref(downloaded_pages),
                               std::ref(m), std::ref(num_processed));
    }

    std::for_each(producers.begin(), producers.end(),
                  [](auto&& t) { t.join(); });
    std::for_each(consumers.begin(), consumers.end(),
                  [](auto&& t) { t.join(); });
}
}

//-----------------------------------------------------------------------------
// condition_variable
//-----------------------------------------------------------------------------
namespace section6
{
void producer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int thread_index, std::condition_variable& cv)
{
    for (int i = 0; i < 5; ++i) {
        // ó���ؾ� �� ���� �����´�.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 * thread_index));
        std::string content = "������Ʈ: " + std::to_string(i) +
                              " from thread(" + std::to_string(thread_index) +
                              ")\n";

        m.lock();
        // ó���ؾ� �� ���� �����Ѵ�.
        downloaded_pages.push(content);
        m.unlock();

        // sleeping ������ �� �ϳ��� ������ ������ �ٽ� �˻��ϰ� �Ѵ�.
        // (���� �ڰ� �ִ� �����尡 ���ٸ� �ƹ� �ϵ� �� �Ͼ��.)
        cv.notify_one();
    }
}

void consumer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int& num_processed, std::condition_variable& cv)
{
    while (num_processed < 25) {
        // lock_guard�� �����ڿ����� lock�� �Ѵ�.
        // �ݸ鿡 unique_lock�� unlock �Ŀ� �ٽ� lock �� �� �ִ�.
        // ���⼭�� cv.wait�� ���ڷ� ����Ѵ�.
        std::unique_lock<std::mutex> lk(m);

        // ������ predicate�� false�̸�, lk�� unlock �ϰ� ������ sleep�Ѵ�.
        // true�̸�, �����Ѵ�.
        cv.wait(lk, [&downloaded_pages, &num_processed] {
            return !downloaded_pages.empty() || num_processed == 25;
        });
        
        if (num_processed == 25) {
            lk.unlock();
            return;
        }

        auto content {downloaded_pages.front()};
        downloaded_pages.pop();
        ++num_processed;

        lk.unlock();

        // ó���ؾ� �� ���� ó���Ѵ�.
        std::cout << content;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

void test()
{
    using std::ref;

    std::queue<std::string> downloaded_pages;
    std::mutex              m;
    std::condition_variable cv;

    std::vector<std::thread> producers;
    producers.reserve(5);
    for (int i = 0; i < producers.capacity(); ++i) {
        producers.emplace_back(producer, ref(downloaded_pages), ref(m), i + 1,
                               ref(cv));
    }

    int                      num_processed = 0;
    std::vector<std::thread> consumers;
    consumers.reserve(3);
    for (int i = 0; i < consumers.capacity(); ++i) {
        consumers.emplace_back(consumer, ref(downloaded_pages), ref(m),
                               ref(num_processed), ref(cv));
    }

    std::for_each(producers.begin(), producers.end(),
                  [](auto&& t) { t.join(); });

    // sleep ���� ��� ��������� ������ ��� join��Ų��.
    cv.notify_all();
    std::for_each(consumers.begin(), consumers.end(),
                  [](auto&& t) { t.join(); });
}
}