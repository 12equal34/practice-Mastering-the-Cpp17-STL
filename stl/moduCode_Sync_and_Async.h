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
// 동기와 비동기 실행
//-----------------------------------------------------------------------------
namespace section1
{
// SSD가 아닌 하드 디스크에서 파일을 읽어오면 시간이 상당히 오래 걸린다.
// 대략 4.17 밀리초가 걸리면 램에서 데이터를 읽어오는데 50나노초에 비해
// 8만배 정도 느리다.
// string txt    = read("a.txt");              // 5ms 소요 (하드디스크 읽기)
// string result = do_something_with_txt(txt); // 5ms 소요 (cpu 연산)
// do_other_computation(); // 5ms 소요 (cpu 연산)
// 총 15ms가 소요된다.
// read 함수가 파일을 하드 디스크에서 읽어오는 동안, CPU가 기다리기 때문에
// 비효율적이다. 이렇게 순차적으로 실행하는 작업을 동기적 (synchronous)으로
// 실행한다고 한다. 이러한 경우에 read 함수가 하드 디스크에서 데이터를 읽어오는
// 동안에 CPU가 do_other_computation을 작업하는 것이 효율적일 것이다.

// C++의 쓰레드로 다음과 같이 구현할 수 있다.
//		void file_read(string* result) {
//			string txt = read("a.txt"); // 5ms소요 (하드디스크 읽기)
//          *result = do_something_with_txt(txt); // 5ms 소요 (cpu 연산)
//		}
//		int main()
//		{
//			string result;
//			thread t(file_read, &result);
//			do_other_computation();  // 5ms 소요 (cpu 연산)
//
//			t.join();
//		}

// 총 10ms가 소요된다.
// 이러한 프로그램의 실행은 여러 갈래로 나뉘어져 동시에 진행되어
// 비동기적 (asynchronous) 실행이라고 부른다.
// c++11 부터 표준 라이브러리를 통해 간단히 비동기적 실행을 할 수 있다.
}

//-----------------------------------------------------------------------------
// std::promise 와 std::future
//-----------------------------------------------------------------------------
namespace section2
{
// 비동기적 실행으로 다음과 같은 작업을 원한다.
// "쓰레드 T를 사용해서 비동기적으로 T의 작업이 완료되면 리턴값을 받는다."
// 이를 다시 말하면,
// "future에 쓰레드 T가 약속된 데이터를 돌려 주겠다는 promise을 한다."
void worker(std::promise<std::string>& p)
{
    // promise 객체가 갖고 있는 future 객체에 값을 넣는다.
    p.set_value("some data");
}
void test()
{
    std::promise<std::string> p;
    std::future<std::string>  data = p.get_future();
    std::thread               t(worker, std::ref(p));

    // promise가 future에 값을 넣기 전까지 wait 함수로 기다린다.
    data.wait();
    // wait이 리턴하면 future에 값이 넣어진 상태이다.

    // get을 통해 future에 전달된 객체를 이동시킨다.
    // 이동시키므로 절대로 두번 호출하면 안된다.
    std::cout << "받은 데이터: " << data.get() << std::endl;

    t.join();
}
// get은 굳이 wait을 안해도 promise가 future에 객체를 전달할 때까지
// 기다린 후에 리턴한다.

// 생산자-소비자 패턴에서
// promise는 producer 역할을,
// future는 consumer 역할을 수행한다.
// 그래서 promise-future 패턴을 다음과 같이 구현할 수 있다.
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
            info = "some data"; // p.set_value("some data")에 대응된다.
            done = true;
        }
        cv.notify_all();
    }
    void test()
    {
        std::thread t(worker);

        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] { return done; }); // data.wait()에 대응된다.
        lk.unlock();

        std::cout << "받은 데이터: " << info << std::endl;

        t.join();
    }
}
// 하지만 가독성이 떨어지므로 promise-future를 이용하는 것이 좋다.
// 또한 future는 exception을 던질 수 있다.
namespace exception_with_future
{
    using std::string;
    void worker(std::promise<string>& p)
    {
        try {
            throw std::runtime_error("Some Error!");
        } catch (...) {
            // 매개변수 exception_ptr을 받는다.
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
            std::cout << "예외: " << e.what() << std::endl;
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
        // 어떠한 객체도 전달하지 않게 void로 설정한다.
        // future가 set이 되었는지 플래그 역할을 한다.
        std::promise<void> p;
        std::future<void>  data = p.get_future();
        std::thread        t(worker, std::ref(p));

        while (true) {
            // promise 가 설정될 때까지 무한히 기다리지 않고,
            // 전달된 시간(1초)만큼 기다리고 리턴한다.
            std::future_status status = data.wait_for(std::chrono::seconds(1));

            // 리턴되는 값은 future_status로 총 3가지 상태를 가진다.
            // 값이 설정되었음: future_status::ready
            // 값이 설정되지않음: future_status::timeout
            // 함수가 실행되지않음: future_status::deferred
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
// 이전 절에서 future는 get을 한번만 호출할 수 있다고 하였다.
// 왜냐하면 future 내부의 객체가 이동되기 때문이다.
// 하지만 여러 개의 다른 쓰레드에서 future를 get할 필요가 있을 때,
// shared_future를 사용한다.
// future와는 다르게 복사가 가능하므로 값 전달을 해도 된다.
// 내부 객체의 복사본을 만들어서 공유한다.
namespace shared_future
{
    using std::thread;

    void runner(std::shared_future<void> start)
    {
        // set 되기까지 기다린다.
        start.get();
        std::cout << "출발!" << std::endl;
    }
    void test()
    {
        std::promise<void>       p;
        std::shared_future<void> start = p.get_future();

        thread t1(runner, start);
        thread t2(runner, start);
        thread t3(runner, start);
        thread t4(runner, start);

        std::cerr << "준비...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << "땅!" << std::endl;

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
// 비동기적으로,
// Callable 함수의 리턴값은 promise의 set_value에 전달해주고,
// Callable 함수의 예외값은 promise의 set_exception에 전달해주는 것을
// 편리하게 제공하는 packaged_task가 있다.
// get_future를 통해 해당하는 future를 리턴한다.
// 즉, 쓰레드에 굳이 promise를 전달하지 않고 함수만을 전달해서 비동기적 실행을
// 한다.
namespace package_task
{
    int some_task(int x) { return 10 + x; }

    void test()
    {
        std::packaged_task<int(int)> task(some_task);
        std::future<int>             start = task.get_future();
        // packaged_task는 복사 생성자가 없어서 이동 생성해야 한다.
        std::thread t(std::move(task), 5);

        std::cout << "결과값 : " << start.get() << std::endl;
        t.join();
    }
}

// promise나 packaged_task는 비동기적으로 실행하기 위해 쓰레드를 명시적으로
// 생성했다.
// std::async에 Callable 함수를 전달하면 쓰레드를 알아서 생성하여 비동기적으로
// 실행하고 결과값을 future에 전달한다.
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
        // std::async의 첫번째 인자로 두 가지 값이 가능하다.
        // std::launch::async : 곧바로 쓰레드를 생성하여 함수를 실행한다.
        // std::launch::deferred : 쓰레드를 생성하지 않고 future의 get이
        // 호출되었을 때 실행한다. 당장 비동기적으로 실행할 필요없을 때 사용.
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
        std::cout << "1 부터 1000 까지의 합: " << parallel_sum(v) << std::endl;
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