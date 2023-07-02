#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

// https://modoocode.com/271

//-----------------------------------------------------------------------------
// 캐시
//-----------------------------------------------------------------------------
namespace section1
{
// CPU와 컴퓨터 메모리인 RAM은 물리적으로 떨어져 있어서
// 메모리로부터 데이터를 읽어 오기 위해서는 시간이 오래 걸린다.
// 예를 들어,
// 메모리로부터 데이터를 읽어오는데 42 사이클이 걸리고
// 덧셈 연산은 1사이클이 걸린다.
// 그래서 읽어오는 시간을 기다리면 42번 덧셈연산을 놓친다.
//
// CPU 칩 안에 있는 작은 메모리인 캐시 Cache가 존재한다.
// 물리적으로 CPU와 붙어 있기 때문에 읽기/쓰기 속도가 훨씬 빠르다.
//
// 예를 들어,
// 코어마다 공유하지 않는 L1,L2 캐시가 존재한다.
// L1 캐시 : 32KB,			r/w 속도 = 4 사이클
// L2 캐시 : 256KB			r/w 속도 = 12 사이클
// 공유하는 L3 캐시 : 8MB		r/w 속도 = 36 사이클
//
// CPU가 특정한 주소에 있는 데이터에 접근한다면,
// L1, L2, L3, 메모리 순으로 데이터를 확인한다.
// 데이터를 캐시에서 가져오면 Cache hit 라고 한다.
// 데이터를 메모리까지 가서 가져오면 Cache miss 라고 한다.
//
// CPU가 메모리를 읽으면 캐시에 저장해놓는다.
// 만약 캐시가 다 찼다면 특정한 방식에 따라 처리한다.
// 특정한 방식은 CPU 마다 다르다.
// 대표적인 예로 Least Recently Used (LRU) Cache에 새로운 캐시를 기록하는 방식이
// 있다. (가장 오래된 캐시를 새로운 캐시로 덮어씌운다.)
//
// 예를 들어, 캐시가 1KB 크기이고 LRU 방식을 사용하는 CPU에 대해서 다음과 같다.
void cache_miss()
{
    for (int i = 0; i < 10000; ++i) {
        for (int j = 0; j < 10000; ++j) {
            // s += data[j];
        }
    }
    // j = 256이 되었을 때, data[0]은 캐시에서 사라진다.
    // 다음 루프에서 data[0]은 캐시에 존재하지 않으므로 메모리에서 가져온다.
}

void cache_hit()
{
    for (int j = 0; j < 10000; j++) {
        for (int i = 0; i < 10000; i++) {
            // s += data[j];
        }
        // 처음 data[0]에 접근할 때 메모리에서 가져온다.
        // 이후 data[0]은 모두 캐시에서 가져온다.
    }
}
}

//-----------------------------------------------------------------------------
// 재배치 (reordering)
//-----------------------------------------------------------------------------
namespace section2
{
// 컴퓨터는 작성한 코드 순서를 그대로 기계어로 번역하지 않는다.
int a = 0;
int b = 0;

void foo()
{
    a = b + 1;
    b = 1;
}
// 어셈블리 코드에서 a = b + 1에 포함된 명령어들이 끝나기 전에 b = 1이 먼저
// 실행될 수 있다. 이렇게 컴파일러가 명령어를 재배치하는 이유는 CPU가 한번에 한
// 명령어씩 실행하지 않고 CPU 파이프라이닝을 거치기 때문이다.
// 1. 명령어를 읽는다. (fetch)
// 2. 명령어를 해석한다. (decode)
// 3. 명령어를 실행한다. (execute)
// 4. 실행 결과를 쓴다. (write)
// 컴파일러는 최대한 CPU의 파이프라인을 효율적으로 활용할 수 있도록 명령어를
// 재배치한다. 또한 꼭 컴파일러에서만 명령어를 재배치하는 것이 아니라 CPU에서도
// 캐시에 있는 데이터에 접근하는 것이 빠르기 때문에 a = 1; (캐시에 없음) b = 1;
// (캐시에 있음) 같은 경우에 b = 1이 먼저 실행될 수 있다.

// 이렇게 명령어 순서가 바뀌는데,
// C++의 모든 객체들은 수정 순서(modification order)를 정의할 수 있다.
// 원자적 연산을 할 경우에 모든 쓰레드에서 같은 객체에 대해서 동일한 수정 순서를
// 관찰할 수 있다.
// int a가 1 -> 2 -> 3 -> 4 순서로 값이 변한다고 하자.
// T1 에서 관찰한 수정 순서 : 1 -> 4
// T2 에서 관찰한 수정 순서 : 2 -> 3
// T3 에서 관찰한 수정 순서 : 2 -> 3 -> 4
// 어떠한 쓰레드에서도 관찰할 수 없는 수정 순서 : 4->2, 3->2, 2->1 (역행)
// 같은 시간에 서로 다른 쓰레드가 a의 값을 확인할 때 서로 다른 값일 수 있다.
// 수정 순서만 같으면 된다.
// 예를 들어, T2에서 a = 3 일때, 자신 코어의 캐시에만 기록해 놓고 다른
// 코어들에게 알리지 않는다면, T3는 동일한 시간에 a = 3 가 아닐 수 있다.
// 그렇다고 모든 캐시에 동기화 작업을 한다면 느릴 것이다.
}

//-----------------------------------------------------------------------------
// 원자성 (atomicity)
//-----------------------------------------------------------------------------
namespace section3
{
// C++에서 모든 연산들이 원자적일 때,
// 쓰레드들이 같은 객체에 대해서 동일한 수정 순서를 동의할 수 있다.
// 그래서 원자적 연산이 아닌 경우 동일한 수정 순서가 보장되지 않아서
// 프로그램이 undefined behavior가 일어난다.

// 원자적(atomic) 연산 : CPU가 명령어 1개로 처리하는 명령어이다. 중간에 다른
// 쓰레드가 끼어들 수 없다. 이 연산은 더 작은 연산으로 쪼갤 수 없어서, 이 연산은
// 했다 혹은 안했다로 판단할 수 있다.

// 원자적 연산은 뮤텍스가 필요없어서 속도가 빠르다.
// std::atomic<T>는 원자적으로 만들고 싶은 타입 T를 전달하여
// atomic에서 제공하는 함수들로 여러가지 원자적인 연산들을 수행할 수 있다.
void worker(std::atomic<int>& counter)
{
    for (int i = 0; i < 10000; ++i) {
        ++counter; // 뮤텍스로 보호하지 않아도 된다.
        // 어셈블리 코드에서 lock add DWORD PTR [rdi], 1 로 실행한다.
        // (여기서 lock은 해당 명령을 원자적으로 수행하라는 의미다.)
        // 즉 명령어 한줄로 나타난다. 하지만 이러한 명령어를 지원하지 않는 CPU는
        // atomic 객체의 연산들을 원자적으로 구현할 수 없다.
        // 이는 is_lock_free()를 통해 확인할 수 있다.
    }
}

void test()
{
    using std::ref;

    std::atomic<int> counter(0);

    std::cout << "int에 대한 원자적 연산을 지원하는 가? : " << std::boolalpha
              << counter.is_lock_free() << std::endl;
    // lock free는 뮤텍스와 같은 객체들의 lock, unlock 없이 원자적 연산할 수
    // 있다는 의미다.

    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back(worker, ref(counter));
    }

    std::for_each(workers.begin(), workers.end(), [](auto&& t) { t.join(); });

    std::cout << "Counter 최종 값: " << counter << std::endl;
}
}

//-----------------------------------------------------------------------------
// memory_order
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace section4
{
// atomic 객체들은 원자적 연산을 할 때, 메모리에 접근하는 방식을 지정할 수 있다.

// 1. memory_order_relexed
// 메모리 접근 순서가 바뀌는 것을 허용한다.
void t1(std::atomic<int>& a, std::atomic<int>& b)
{
    b.store(1, std::memory_order_relaxed);
    int x = a.load(std::memory_order_relaxed);

    printf("x : %d \n", x);
}
void t2(std::atomic<int>& a, std::atomic<int>& b)
{
    a.store(1, std::memory_order_relaxed);
    int y = b.load(std::memory_order_relaxed);

    printf("y : %d \n", y);
}
void test1()
{
    using std::ref;

    std::vector<std::thread> threads;

    std::atomic<int> a(0);
    std::atomic<int> b(0);

    threads.emplace_back(t1, ref(a), ref(b));
    threads.emplace_back(t2, ref(a), ref(b));

    std::for_each(threads.begin(), threads.end(), [](auto&& t) { t.join(); });
    // 결과로 x:0 y:0 또는 x:1 y:0 또는 x:0 y:1 또는 x:1 y:1 이 나온다.
    // CPU가 순서를 재배치할 수 있어서 매우 빠른 속도로 실행된다.
}

// 이전 절에서 ++counter; 부분은 memory_order_relaxed 방식으로 사용할 수 있다.
// counter.fetch_add(1, memory_order_relaxed);로 사용할 수있다.

// 2. memory_order_acquire, memory_order_release

// producer-consumer 관계에서는 memory_order_relaxed를 사용하면 안 된다.
namespace wrong
{
    void producer(std::atomic<bool>& is_ready, int& data)
    {
        data = 10;
        is_ready.store(true, std::memory_order_relaxed);
        // 여기서 data = 10이 대입되기 전에 is_ready가 true가 되면,
    }
    void consumer(std::atomic<bool>& is_ready, int& data)
    {
        // is_ready_load가 true가 되어 data가 준비되었다고 착각하여,
        while (!is_ready.load(std::memory_order_relaxed)) {
        }
        // data = 0인 채로 데이터를 읽게 된다.
        std::cout << "Data : " << data << std::endl;
    }
    void test()
    {
        using std::ref;

        std::vector<std::thread> threads;
        std::atomic<bool>        is_ready(false);
        int                      data = 0;

        threads.emplace_back(producer, ref(is_ready), ref(data));
        threads.emplace_back(consumer, ref(is_ready), ref(data));

        std::for_each(threads.begin(), threads.end(),
                      [](auto&& t) { t.join(); });
    }
}
// producer-consumer 관계에서는 memory_order_acquire, memory_order_release를
// 사용한다.
namespace right
{
    void producer(std::atomic<bool>& is_ready, int& data)
    {
        data = 10;
        is_ready.store(true, std::memory_order_release);
        // memory_order_release는 이전의 모든 메모리 명령들이
        // 이 다음으로 재배치되는 것을 금지한다.
    }
    void consumer(std::atomic<bool>& is_ready, int& data)
    {
        // memory_order_acquire는 이후의 모든 메모리 명령들이
        // 이 이전으로 재배치되는 것을 금지한다.
        while (!is_ready.load(std::memory_order_acquire)) {
        }
        std::cout << "Data : " << data << std::endl;
    }
    void test()
    {
        using std::ref;

        std::vector<std::thread> threads;
        std::atomic<bool>        is_ready(false);
        int                      data = 0;

        threads.emplace_back(producer, ref(is_ready), ref(data));
        threads.emplace_back(consumer, ref(is_ready), ref(data));

        std::for_each(threads.begin(), threads.end(),
                      [](auto&& t) { t.join(); });
    }
}
// 두 개의 쓰레드가 같은 변수의 memory_order_acquire, memory_order_release를
// 통해서 동기화를 수행하고 있다.
namespace another_example
{
    using std::memory_order_acquire;
    using std::memory_order_relaxed;
    using std::memory_order_release;
    using std::ref;

    std::atomic<bool> is_ready;
    std::atomic<int>  data[3];

    void producer()
    {
        data[0].store(1, memory_order_relaxed);
        data[1].store(2, memory_order_relaxed);
        data[2].store(3, memory_order_relaxed);
        is_ready.store(true, memory_order_release);
    }

    void consumer()
    {
        while (!is_ready.load(memory_order_acquire)) {
        }

        std::cout << "data[0]: " << data[0].load(memory_order_relaxed)
                  << std::endl;
        std::cout << "data[1]: " << data[1].load(memory_order_relaxed)
                  << std::endl;
        std::cout << "data[2]: " << data[2].load(memory_order_relaxed)
                  << std::endl;
    }

    void test()
    {
        std::vector<std::thread> threads;

        threads.emplace_back(producer);
        threads.emplace_back(consumer);

        std::for_each(threads.begin(), threads.end(),
                      [](auto&& t) { t.join(); });
    }
    // 항상 다음과 같이 출력한다.
    //      data[0] : 1
    //      data[1] : 2
    //      data[2] : 3
}

// memory_order_acq_rel는 acquire 와 release를 모두 수행한다.

// memory_order_seq_cst는 메모리 명령의 순차적 일관성 (sequential consistency)
// 를 보장한다.
namespace no_seq_cst
{
    std::atomic<bool> x(false);
    std::atomic<bool> y(false);
    std::atomic<int>  z(0);

    void write_x() { x.store(true, std::memory_order_release); }
    void write_y() { y.store(true, std::memory_order_release); }
    void read_x_then_y()
    {
        while (!x.load(std::memory_order_acquire)) {
            // write_x와 read_x_then_y 간의 동기화를 한다.
        }
        // 현재 쓰레드 c 입장에서 x.store가 y.store보다 먼저 발생하고,
        if (y.load(std::memory_order_acquire)) {
            ++z;
        }
    }
    void read_y_then_x()
    {
        while (!y.load(std::memory_order_acquire)) {
            // write_y와 read_y_then_x 간의 동기화를 한다.
        }
        // 현재 쓰레드 d 입장에서 y.store가 x.store보다 먼저 발생할 수 있다.
        if (x.load(std::memory_order_acquire)) {
            ++z;
        }
    }
    void test()
    {
        std::thread a(write_x);
        std::thread b(write_y);
        std::thread c(read_x_then_y);
        std::thread d(read_y_then_x);
        a.join();
        b.join();
        c.join();
        d.join();
        std::cout << "z: " << z << std::endl;
        // z: 0 을 출력할 수 있다.
    }
}
// memory_order_seq_cst는 CPU의 동기화를 통해 모든 쓰레드에서 동일한
// 연산 순서를 관찰할 수 있도록 보장한다.
namespace with_seq_cst
{
    std::atomic<bool> x(false);
    std::atomic<bool> y(false);
    std::atomic<int>  z(0);

    void write_x() { x.store(true, std::memory_order_seq_cst); }
    void write_y() { y.store(true, std::memory_order_seq_cst); }
    void read_x_then_y()
    {
        while (!x.load(std::memory_order_seq_cst)) {
        }
        if (y.load(std::memory_order_seq_cst)) {
            ++z;
        }
    }
    void read_y_then_x()
    {
        while (!y.load(std::memory_order_seq_cst)) {
        }
        if (x.load(std::memory_order_seq_cst)) {
            ++z;
        }
    }
    void test()
    {
        std::thread a(write_x);
        std::thread b(write_y);
        std::thread c(read_x_then_y);
        std::thread d(read_y_then_x);
        a.join();
        b.join();
        c.join();
        d.join();
        std::cout << "z: " << z << std::endl;
        // z: 0 을 출력할 수 없다.
    }

}
// atomic 객체의 default memory_order는 memory_order_seq_cst 이다.
// ++counter는 counter.fetch_add(1,memory_order_seq_cst)가 된다.
// 멀티 코어 시스템에서 memory_order_seq_cst 는 비싼 연산이고
// 특히 ARM 계열의 CPU에서 동기화 비용이 크다.
// 인텔 혹은 AMD의 x86(-64) CPU는 거의 순차적 일관성이 보장되기 때문에
// memory_order_seq_cst를 강제하더라도 그 차이가 크지 않다.
// (그래서 z: 0 이 되거나 위에서 x:0 y:0 을 출력하는 경우가 드물다.)
}