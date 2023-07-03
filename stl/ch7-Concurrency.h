#pragma once

#include <atomic>
#include <thread>
#include <cassert>
#include <condition_variable>
#include <mutex>

//-----------------------------------------------------------------------------
// [The problem with volatile]
//-----------------------------------------------------------------------------
namespace section1
{
// old-style C 또는 옛날 10년 전에 사용했던
// volatile keyword은 어떠한 문제점이 있을까?
// 만약 실제 메모리에 직접 접근하길 원한다면, volatile을 사용해도 무방하다.

// volatile의 공식적인 의미는 volatile accesses가 추상적인 머신의 규칙들을
// 엄격히 따라서 평가된다. 즉, 컴파일러가 volatile accesses에
// 대한 명령어들을 재배치하거나 하나의 명령어로 접근하지 못하게 한다.
// 그래서,
// volatile int& x = memory_mapped_register_x();
// volatile bool& y = memory_mapped_register_y();
// int stack;
// stack = x; // load
// y = true;  // store
// stack += x;// load
// 는 두개의 load 사이에서 x가 그 값을 유지한다고 가정하지 않는다.
// 만약 x가 volatile이 아니면, 컴파일러는 다음과 같이 재배치한다.
// stack = 2*x; // load
// y = true;    // store
// 왜냐하면 y를 작성하는 것이 x 값에 영향을 주지 않기 때문이다.
// 하지만 x가 volatile이면, 영향을 준다고 가정하기 때문에 이러한
// reordering optimization이 허용되지 않는다.

// volatile keyword를 사용하는 대부분의 이유는
// memory-mapped registers 같이 직접적으로 하드웨어를 건드릴 필요성 때문이다.
// 소스 코드 수준에서 load 또는 store가 더 복잡한 hardware operation에 대응된다.
// 어쩌면 x가 some hardware buffer view 이고, y가 하드웨어에 다음 4바이트의
// 데이터를 x에 load하라는 signal일지도 모른다. 마치 하드웨어에서 operator
// overloading을 하는 것과 비슷하다.

// 왜 volatile은 thread-safe하지 않을까? 너무 오래된 키워드이기 때문에
// 멀티스레딩에 대한 고려가 부족하다. 그리고 예전 컴파일러는 지금보다 단순했다.
// 그래서 problematic optimizations가 나타나지 않았었다.
// 90년대 후반, 2000년대 초에 c++은 a lack of thread-aware memory model에 대해
// 실질적으로 고려하기 시작했다. volatile을 thread-safe memory acesss하게
// 만들기에 이미 너무 늦었다. 이미 많은 코드 베이스들이 volatile을 포함하기에
// 문제가 될것이다.

// thread-safe memory accesses가 필요한 경우
namespace example
{
    int64_t x = 0;
    bool    y = false;

    void thread_A()
    {
        x = 0x42'00000042;
        y = true;
    }
    void thread_B()
    {
        if (x) {
            assert(x == 0x42'00000042);
        }
    }
    void thread_C()
    {
        if (y) {
            assert(x == 0x42'00000042);
        }
    }
    // 여기서 thread A,B,C 가 모두 동시적으로 다른 쓰레드에서 돌아간다고 하자.
    // thread_B 에서 x가 항상 0이거나 0x42'00000042 일 것이다.
    // 하지만 32-bit 컴퓨터에서는 컴파일러가 x의 assignment를 upper half, lower
    // half로 a pair of assignments로 구현하므로 어느 순간에 x는 0x42'00000000일
    // 수 있다. 이를 volatile을 사용하고자 한다면 32-bit는 volatile을 제공하지
    // 않는다. 그래서 컴파일러에게 an atomic 64-bit assignment를 해야 한다고
    // 알리는 방법이 좋다. 만약 불가능하다면 컴파일 에러를 주게 한다.
    // (volatile은 atomic을 보장해주지 않는다.)

    // thread_A 입장에서는 y가 true이면 x는 0x42'00000042로 할당되었을 것이다.
    // 하지만 thread_C 입장에서 y가 true이지만 x가 0인 순간이 존재할 수 있다.
    // 만약 쓰레드들이 물리적으로 다른 CPU에서 돌아간다면,
    // 그래서 서로 각자의 캐시 데이터를 갖고 있어서,
    // x와 y의 values가 서로 다른 시간에 업데이트 될 것이다.
    // 이는 각자의 캐시 라인들이 언제 갱신되는지에 의존한다.
    // 그래서 A 입장에서 x가 y보다 먼저 할당되어도,
    // B 입장에서는 y가 x보다 먼저 할당될 수 있는 것이다.
    // 우리는 컴파일러가 y를 load할 때 전체 캐시가 up-to-date되었는지
    // 보장해달라는 요청을 원한다. 하지만 some processor architectures는 특별한
    // instructions, or additional memory-barrier logic이 필요하다. 컴파일러는
    // 옛날 스타일의 volatile accesses에 대해 그러한 instructions을 생성하지
    // 않기 때문에 멀티쓰레딩을 작성할 때 volatile을 사용하지 않는 것이다.
    // volatile은 자신의 쓰레드에서는 sequential order대로 access하지만,
    // 다른 쓰레드에서는 in a different order대로 access하기 때문에
    // volatile accesses는 sequentially consistent를 보장하지 않는다.

    // 이러한 문제들에 대한 해결책으로 C++11부터 std::atomic이 추가되었다.
}
}

//-----------------------------------------------------------------------------
// [Using std::atomic<T> for thread-safe accesses]
//-----------------------------------------------------------------------------
namespace section2
{
// std::atomic<T>를 std::vector처럼 class template이라고 생각할 수 있지만
// std::atomic<T>는 컴파일러와 함께 동작하도록 만들어졌기 때문에
// 단순히 class template으로 생각하면 안된다.
// T는 임의의 타입이 들어가도 되지만,
// 실제로는 atomic-friendly type을 사용해야 한다.
// atomic-friendly types은 정수 타입들과 포인터 타입들이다. (레지스터 크기 이하)
// 일반적으로 대부분의 플랫폼에서 이러한 타입을 갖는 std::atomic objects은
// atomic 연산을 할 것이다.
std::atomic<int64_t> x = 0;
std::atomic<bool>    y = false;

void thread_A()
{
    x = 0x42'00000042; // atomic!!
    y = true;          // atomic!!
}

void thread_B()
{
    if (x) {
        // The assignment to x happens atomically.
        assert(x == 0x42'00000042);
    }
}

void thread_C()
{
    if (y) {
        // y의 할당 이전에 x가 할당되었고
        // 다른 쓰레드의 관점에서도 순서가 보장된다.
        assert(x == 0x42'00000042);
    }
}
// std::atomic<T>는 atomic, thread-safe assignment를 수행하도록
// (++,--,+=,-=, for integral types &=,|=,^=)를 오버로드한다.

// [중요]
// std::atomic<T>의 objects는 개념적으로 memory에 존재하고
// type T의 values는 CPU register 또는 cache에 존재한다.
//
// 그래서 std::atomic의 copy-assignment, move-assignment operator가 존재하지
// 않는다. 왜냐하면 다음과 같이 작성하였을 때,
// std::atomic<int> a,b;
//
// a = b; 는 컴파일 에러가 난다.
// b의 value을 register에 load하고, register의 value를 a로 store를
// 해야한다. 이는 two different atomic operations을 해야한다.
//
// 아니면 곧바로 b에서 a로 복제하는 것이 가능하다면
// two different memory location를 an atomic operation으로 접근해야 하는데
// 이는 대부분의 컴퓨터 하드웨어에서 불가능하다.
//
// C++ 에서는 명확하게 register에서 object로,
// 또는 object에서 register로의 연산자로 작성해야 한다.
// int shortlived = b; // atomic load
// a = shrotlived; // atomic store
//
// 또는 std::atomic<T>가 제공하는 멤버함수인 .load(), .store(v)를 사용한다.
// int shortlived = b.load(); // atomic load
// a.store(shortlived); // atomic store
// 이 방법이 권장된다.
//
// b.stort(a.load())로 한 줄에 작성할 수 있지만, 이렇게 작성하지 말아야 한다.
// 동시에 호출하는 것으로 착각할 수 있다.
}

//-----------------------------------------------------------------------------
// [Doing complicated operations atomically]
//-----------------------------------------------------------------------------
namespace section3
{
// std::atomic에서 *=, /=, %=, <<= and >>= 은 모두 하드웨어에서
// 효율적으로 계산하기 힘들어서 삭제되었다.
// 하지만 이러한 연산들을 expensive하게 구현할 수 있다.
// 우리의 하드웨어가 atomic multiply instruction을 제공하지 않는다고 하자.
// 우리는 이를 a primitive atomic operation known as "compare and swap",
// or in C++ "compare-exchange"를 사용하여 구현한다.
void example()
{
    std::atomic<int> a = 6;
    // a *= 9; // *= 연산은 정의되지 않았다. 그러므로 컴파일 에러.

    int expected, desired;
    do {
        expected = a.load();
        desired  = expected * 9;
    } while (!a.compare_exchange_weak(expected, desired));
    // 현재 값이 expected과 동일하면, 값을 desired로 설정한다.
    // (a read-modify-write operation)
    // 현재 값이 expected와 (다른 쓰레드가 접근해서) 다르면,
    // 값을 expected 으로 load한다. (a load operation)

    // 위의 루프를 마치면,
    // a의 값은 atomically multiplied by 9가 된다.
}

// 위의 연산은 expected가 reference 전달이므로 다음과 같이
// 간결하게 작성할 수 있다.
void example2()
{
    std::atomic<int> a = 6;
    // a *= 9; *= 연산은 정의되지 않았다.
    int expected = a.load();
    while (!a.compare_exchange_weak(expected, expected * 9))
        ;
}
}

//-----------------------------------------------------------------------------
// [Big atomics]
//-----------------------------------------------------------------------------
namespace section4
{
// 컴파일러는 std::atomic<T>에 대해
// T가 an integral type(including bool)이거나
// T가 포인터 타입이면 최적화된 코드를 생성한다.
// 그렇다면 T가 int[100]과 같이 a bigger type이면 어떻게 될까?
// 이러한 경우는 the runtime library의 루틴을 호출하여
// the assignment under a mutex를 수행할 것이다.
// 이러한 the assignment는 사용자 정의 타입들의 복제를 모르는 라이브러리에서
// 수행되므로 c++17에서는 오직 trivially copyable types에 대해서만 작동한다.
// 다시 말해, memcpy로 복제할 수 있는 타입들 T에 대해서만 std::atomic<T>의
// 사용을 제한한다.
// 그래서 std::atomic<std::string>을 원한다면 직접 작성해야 한다.

// a trvially copyable big types을 std::atomic에 사용할 때는 C++의
// 표준 라이브러리와 다른 장소인 c++ runtime routines에서 돌아가므로
// 어떤 플랫폼에서는 -latomic을 링커의 명령어에 포함해야 한다.
// 그러나 이러한 문제는 오직 big types에 대해서만 그렇고,
// 우리는 이러한 걱정을 할 필요가 없다.

// 이제 어떻게 atomic string class를 작성하는 지 살펴보자.
}

//-----------------------------------------------------------------------------
// [Taking turns with std::mutex]
//-----------------------------------------------------------------------------
namespace section5
{
// 우리는 (만약 존재한다면) std::atomic<std::string>처럼 행동하는 a class type을
// 작성하고자 한다. atomic, thread-safe loads and stores를 제공해야 한다.
// 그래서 두개의 쓰레드가 std::string에 동시적으로 접근하면, 절대로
// "halfway assigned" state를 관찰할 수 없게 해야 한다.
// 이를 구현하는 가장 좋은 방법은 std::mutex를 사용하는 것이다.
// mutex는 "mutal exclusion"에서 따온 이름이다.

// [중요]
// A mutex acts a way to ensure that only one thread is allowed into a
// particular section of code (or set of sections of code) at once.
// 그래서 "thread A is executing this code"와
// "thread B is executiing this code"는 mutually exclusive possible하다.

// a critical section의 시작은 we take a lock on the associated mutex.
// the critical section의 끝은 we release the lock 에 대응한다.
// The library는 두개의 쓰레드가 the same mutex를 the same time에 hold locks을
// 하지 않도록 만들었다.
// 쓰레드 A가 이미 holding lock하면, 쓰레드 B는 A가 the critical section을
// 나가서 the lock을 release할 때까지, 반드시 wait해야 한다. 그래서 A가 lock을
// 갖는 동안에 쓰레드 B의 진행이 blocked된다. 이러한 현상을 waiting or
// blocking이라 말한다.

// "Taking a lock on a mutex"는 "locking the mutex"로,
// "releasing the lock"은 "unlocking the mutex"로 간편하게 말한다.

// 가끔 a mutex가 현재 locked인지 .try_lock()을 통해 알 수 있다.
// 만약 locking the mutex하면 true를 리턴하고,
// 만약 already locked by some thread이면 false를 리턴한다. (기다리지 않는다.)

// a mutex를 통해 a section of code를 통제할 때, the lifetime semantics of the
// mutex object itself를 고려해야 한다.
// 해당 뮤텍스를 사용하길 원하는 모든 a section of code에게 visible하도록 a
// single mutex object를 어느곳에 두어야 할까?

namespace in_a_function_scoped_static_variable
{
    void log(const char* message)
    {
        static std::mutex m;
        m.lock(); // avoid interleaving messages on stdout
        puts(message);
        m.unlock();
    }
    // 여기서 static 키워드는 상당히 중요하다.
    // 만약 이를 생략한다면, the same mutex object를 보장하지 않는다.
    // 매번 쓰레드마다 구분되는 mutex objects를 locking and unlocking 하므로
    // the library는 아무 일도 하지 않게 된다.
}

namespace in_a_global_variable
{
    static std::mutex m;

    void log1(const char* message)
    {
        m.lock();
        printf("LOG1: %s\n", message);
        m.unlock();
    }
    void log2(const char* message)
    {
        m.lock();
        printf("LOG2: %s\n", message);
        m.unlock();
    }
    // log1, log2 함수를 상호 배제적으로 실행할 수 있다.
    // 같은 시간에 오직 하나의 쓰레드만 log1 또는 log2의 코드를 실행할 수 있다.

    // the global variable보다는 a class type을 만들어서 구현하는 것이 좋다.
}

namespace a_member_variable_of_a_class
{
    struct Logger {
        std::mutex m_mtx;

        void log1(const char* message)
        {
            m_mtx.lock();
            printf("LOG1: %s\n", message);
            m_mtx.unlock();
        }
        void log2(const char* message)
        {
            m_mtx.lock();
            printf("LOG1: %s\n", message);
            m_mtx.unlock();
        }
    };
}
}

//-----------------------------------------------------------------------------
// ["Taking locks" the right way]
//-----------------------------------------------------------------------------
namespace section6
{
// C 또는 old-style C++에서 raw pointer에 의해 버그가 발생했다.
// 비슷하게 raw mutex에 의해 멀티쓰레딩 프로그래밍에서 문제가 발생한다.
// 1. Lock leaks: 깜박하고 mutex를 unlock하지 않는다.
// 2. Lock leaks: 예외처리에 의해 일찍 리턴되서 unlock하지 않는다.
// 3. Use-outside-of-lock: a raw mutex는 guard하는 변수들과 결합되어 있지 않기
//    때문에 실수로 lock을 하지 않고, 해당 변수들에 접근할 수 있다.
// 4. Deadlock: thread A가 mutex 1을 lock하고, thread B가 mutex 2를 lock하면
//    서로가 상대방의 mutex를 lock하려고 시도할 때, 모두 blocked 되어
//    프로그램이 진행하지 않게 된다.
// 하지만 위의 경우들은 그렇게 힘들게 만드는 케이스는 아니다.
// 예를 들어 std::atomic<T>에서 livelock 같은 경우가 그렇다. 이러한 문제는
// 멀티스레딩 관련 서적을 참고한다.

// c++ 표준 라이브러리는 이러한 버그들을 제거할 수 있게 도와주는 도구들을
// 제공한다. memory management에서의 상황과는 다르게, 각 상황에서의 문제들을
// 100퍼센트 해결해주지 못한다. 그래서 멀티스레딩은 단일 스레딩 프로그래밍보다
// 훨씬 어렵다.

// "lock leaks"는 RAII를 의식적으로 사용하면 제거할 수 있다.
template <typename M>
class unique_lock
{
    M*   m_mtx    = nullptr;
    bool m_locked = false;
public:
    constexpr unique_lock() noexcept = default;
    constexpr unique_lock(M* p) noexcept
        : m_mtx(p)
    { }

    M*   mutex() const noexcept { return m_mtx; }
    bool owns_lock() const noexcept { return m_locked; }

    void lock()
    {
        m_mtx->lock();
        m_locked = true;
    }
    void unlock()
    {
        m_mtx->unlock();
        m_locked = false;
    }

    unique_lock(unique_lock&& rhs) noexcept
    {
        m_mtx    = std::exchange(rhs.m_mtx, nullptr);
        m_locked = std::exchange(rhs.m_locked, false);
    }

    unique_lock& operator=(unique_lock&& rhs)
    {
        if (m_locked) {
            unlock();
        }
        m_mtx    = std::exchange(rhs.m_mtx, nullptr);
        m_locked = std::exchange(rhs.m_locked, false);
        return *this;
    }

    ~unique_lock()
    {
        if (m_locked) {
            unlock();
        }
    }
};
// 이름에서 말하듯이, std::unique_lock<M>은 "unqiue ownership" RAII class를
// 의미한다. lock과 unlock 대신에 unique_lock을 사용하면 a mutex lock을
// release 하는 것을 절대로 깜빡하지 않게 된다.

// std::unique_lock<M>은 맴버 함수 .lock(), .unlock()을 제공한다.
// 물론 일반적으로 필요하지는 않다.
// 만약 코드블록의 중간에 lock의 acquire or release가 필요할 때 유용하다.

// unique_lock은 movable하므로 null or empty state를 가져야 한다.
// 대부분의 경우 lock을 이동시킬 필요가 없다. 그러한 경우는
// 어떤 스코프의 시작에서 lock하고, 그 스코프의 끝에서 unlock하는 경우다.
// 이때는 단순히 std::lock_guard<M>을 사용한다.
// std::lock_guard<M>은 not movable하고 .lock(), .unlock()이 정의되지 않았다.
}

//-----------------------------------------------------------------------------
// [Always associate a mutex with its controlled data]
//-----------------------------------------------------------------------------
namespace section7
{
// 다음 클래스에는 버그가 존재한다.
class StreamingAverage_with_bug
{
    double     m_sum          = 0;
    int        m_count        = 0;
    double     m_last_average = 0;
    std::mutex m_mtx;

public:
    // Called from the single producer thread
    void add_value(double x)
    {
        std::lock_guard lk(m_mtx);
        m_sum   += x;
        m_count += 1; // A
    }

    // Called from the single consumer thread
    double get_current_average()
    {
        std::lock_guard lk(m_mtx);
        m_last_average = m_sum / m_count; // B
        return m_last_average;
    }

    // Called from the single consumer thread
    double get_last_average() const
    {
        return m_last_average; // C
    }

    // Called from the single consumer thread
    double get_current_count() const
    {
        return m_count; // D
    }
};
// A에서 문제가 된다. A와 D에서
// writing to this->m_count in the producer thread와
// reading from ths->m_count in the consumer thread에서 race 경쟁하게 된다.
// 즉, A가 writing하고 있는 와중에 D가 reading하려고 시도할 수 있다.

// B와 C에서도 표면적으로 비슷하지만... 여기부터 다시 작성해야한다.

// A better way is via a nested struct definition:
// (to place the mutex in the tightest possible relationship to the variables
// it "guards.")
class StreamingAverage
{
    struct
    {
        double sum = 0;
        int count = 0;
        std::mutex mtx;
    } m_guarded_sc;
    double m_last_average = 0;

    // ...
};
}