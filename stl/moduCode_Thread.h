#pragma once
// 모두의 코드
// https://modoocode.com/269
// 에서 공부한 내용들을 정리한다.
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
        std::cout << "쓰레드 1 작동중 \n";
    }
}

void func2()
{
    for (int i = 0; i < 10; ++i) {
        std::cout << "쓰레드 2 작동중 \n";
    }
}

void func3()
{
    for (int i = 0; i < 10; ++i) {
        std::cout << "쓰레드 3 작동중 \n";
    }
}

void how_to_thread()
{
    // 쓰레드 t1을 함수 func1을 받아 생성하고, func1을 쓰레드 t1에서 실행한다.
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);

    // 쓰레드 t1의 실행이 종료되면 리턴한다.
    t1.join();
    t2.join();
    t3.join();
    // 여기서 joins을 하지 않으면 쓰레드의 작업들이 끝나기 전에 main함수가
    // 종료되어서 쓰레드 객체들 t1,t2,t3의 소멸자가 호출된다. join되거나
    // detach되지 않은 쓰레드의 소멸자가 호출되므로 예외를 던지게 된다.
}

void what_is_detach()
{
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);

    // main함수가 끝나기 전에 해당 쓰레드가 끝날 필요가 없음을 명시한다.
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
    printf("쓰레드 %x 에서 %d 부터 %d 까지 계산한 결과 : %d \n", this_id,
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
    std::cout << "전체 합: " << total << std::endl;
}
}

//-----------------------------------------------------------------------------
// 메모리를 같이 접근한다면?
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

    std::cout << "Counter 최종 값: " << counter << std::endl;
}
}

//-----------------------------------------------------------------------------
// std::mutex : 한 쓰레드에서만 코드를 실행시킬 수 있게 허용한다.
//-----------------------------------------------------------------------------
namespace section4
{
void worker(int& result, std::mutex& m)
{
    for (int i = 0; i < 10000; ++i) {
        // 뮤텍스 m의 사용권한은 단일 쓰레드가 가질 수 있다.
        // m.lock()은 뮤텍스 m의 사용권한을 가져온다.
        // 그래서 m을 소유한 쓰레드가 m.unlock()을 할때까지 무한히 기다린다.
        // m.unlock()은 뮤텍스 m의 사용권한을 반환한다.

        m.lock();
        // 이 사이의 영역은 하나의 쓰레드에서만 유일하게 실행할 수 있는 코드
        // 영역이다. 이를 임계 영역 (critical section) 이라고 부른다.

        result += 1;

        //
        m.unlock();

        // 실수로 m.unlock()을 하지 않으면 이 프로그램은 m을 갖고있지 않은
        // 쓰레드들의 .join() 호출에서 무한히 기다릴 것이라고 생각할 수 있다.
        // 하지만 그 이전에 m을 갖고 있는 쓰레드가 반복문에서 다시 m.lock()을
        // 호출하므로 자기 자신도 자신의 뮤텍스를 반환하기를 기다리게 된다. 결국
        // 아무 쓰레드도 진행하지 않게 되는데, 이를 데드락 (deadlock)이라고
        // 부른다.
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

    std::cout << "Counter 최종 값: " << counter << std::endl;
}

// 실수로 m.unlock()을 빠뜨리는 문제로 데드락이 발생하는데
// 이를 방지하기 위해 unique_ptr처럼 소멸자에서 m.unlock을 하도록 처리한다.
void worker_with_lock_guard(int& result, std::mutex& m)
{
    for (int i = 0; i < 10000; ++i) {
        // 생성자가 m.lock()을 호출한다.
        std::lock_guard<std::mutex> lock(m);

        // critical section
        result += 1;

        //

        // 소멸자가 m.unlock()을 호출한다.
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

    std::cout << "Counter 최종 값: " << counter << std::endl;
}

// 하지만 데드락은 다른 방식으로도 발생할 수 있다.
void worker1(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10000; ++i) {
        // 현재 t1이 실행 중인데,
        std::lock_guard<std::mutex> lock1(m1); // t2가 m1을 갖고 있다고 하자.
        // t2가 m1을 반환할 때까지 무한히 기다린다.

        std::lock_guard<std::mutex> lock2(m2);
        // 두 개의 임계영역 m1,m2가 겹쳐있다.
    }
}
void worker2(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10000; ++i) {
        // 현재 t2가 실행 중인데,
        std::lock_guard<std::mutex> lock2(m2); // t1은 m2를 갖고 있다고 하자.
        // t1이 m2를 반환할 때까지 무한히 기다린다.
        // 하지만 t1이 m2를 반환하려면 t2가 m1을 반환해야 하고
        // t2가 m1을 반환하려면 t1이 m2를 반환해야 한다. (데드락 발생!)

        std::lock_guard<std::mutex> lock1(m1);
        // 두 개의 임계영역 m1,m2가 겹쳐있다.
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

// 위의 데드락 문제를 해결해보자.
void worker1_no_deadlock(std::mutex& m1, std::mutex& m2)
{
    for (int i = 0; i < 10; ++i) {
        // 우선권을 갖는 worker1은 코드를 바꿀 필요가 없다.
        m1.lock();
        m2.lock();
        // 현재 t1는 m1,m2의 사용권한을 가졌다.

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
            // 현재 t2는 m2의 사용권한을 가졌다.

            // m1.try_lock()은 m1이 lock을 할 수 있다면 lock을 하고 true를
            // 리턴한다. lock을 할 수 없다면 기다리지 않고 false를 리턴한다.

            // 현재 t2는 m1의 사용권한을 갖고자 한다.
            // 지금 바로 m1의 사용권한을 가져올 수 없다면,
            if (!m1.try_lock()) {
                // 현재 갖고 있는 m2의 사용권한을 반환한다.
                m2.unlock();
                continue;
            }
            // 현재 t2는 m1,m2의 사용권한을 가졌다.

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
// 생산자와 소비자 패턴
//-----------------------------------------------------------------------------
namespace section5
{
// Producer는 처리해야 할 일을 가져오는 쓰레드이다.
// Consumer는 생산자로부터 받은 일을 처리하는 쓰레드이다.

// 예를 들어, 인터넷에서 페이지를 가져와서 분석하는 프로그램을 작성하는 경우가
// 있다.
void producer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int thread_index)
{
    for (int i = 0; i < 5; ++i) {
        // 웹사이트를 다운하는데 다음과 같이 시간이 걸린다고 하자.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 * thread_index));
        // 처리해야 할 일을 가져온다.
        std::string content = "웹사이트: " + std::to_string(i) +
                              " from thread(" + std::to_string(thread_index) +
                              ")\n";

        // downloaded_pages는 여러 쓰레드가 공유하므로 임계 영역에 넣는다.
        m.lock();
        // 처리해야 할 일을 다른 쓰레드와 공유하기 위해 저장한다.
        downloaded_pages.push(content);
        m.unlock();
    }
}

void consumer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int& num_processed)
{
    // 전체 처리하는 페이지 개수 : 5*5 = 25개
    while (num_processed < 25) {
        m.lock();

        if (downloaded_pages.empty()) {
            m.unlock();
            // 웹사이트 페이지 정보가 큐에 추가되는 속도가 매우 느리므로
            // sleep 하지 않으면 계속 empty()를 호출하므로 CPU 자원 낭비가
            // 발생한다.
            // (하지만, 매번 언제 오는지 모르는 데이터를 확인하기 위해
            // 이렇게 해야 할까? 이 방식보다는 producer 에서 데이터가 오면
            // consumer를 깨우는 방식이 낫다. 다음 절에서 설명한다.)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            continue;
        }

        auto content {downloaded_pages.front()};
        downloaded_pages.pop();
        ++num_processed;

        m.unlock();

        // 생산자로부터 받은 페이지를 처리하는데 최소 80밀리초가 걸린다고 하자.
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
        // 처리해야 할 일을 가져온다.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 * thread_index));
        std::string content = "웹사이트: " + std::to_string(i) +
                              " from thread(" + std::to_string(thread_index) +
                              ")\n";

        m.lock();
        // 처리해야 할 일을 저장한다.
        downloaded_pages.push(content);
        m.unlock();

        // sleeping 스레드 중 하나를 깨워서 조건을 다시 검사하게 한다.
        // (만약 자고 있는 쓰레드가 없다면 아무 일도 안 일어난다.)
        cv.notify_one();
    }
}

void consumer(std::queue<std::string>& downloaded_pages, std::mutex& m,
              int& num_processed, std::condition_variable& cv)
{
    while (num_processed < 25) {
        // lock_guard는 생성자에서만 lock을 한다.
        // 반면에 unique_lock은 unlock 후에 다시 lock 할 수 있다.
        // 여기서는 cv.wait의 인자로 사용한다.
        std::unique_lock<std::mutex> lk(m);

        // 전달한 predicate가 false이면, lk를 unlock 하고 무한히 sleep한다.
        // true이면, 리턴한다.
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

        // 처리해야 할 일을 처리한다.
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

    // sleep 중인 모든 쓰레드들을 깨워서 모두 join시킨다.
    cv.notify_all();
    std::for_each(consumers.begin(), consumers.end(),
                  [](auto&& t) { t.join(); });
}
}