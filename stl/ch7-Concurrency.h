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
// 구글에 검색해서 찾아보자.
// volatile의 여러 문제점들에 대한 해결책으로
// c++의 std::atomic이 존재한다.
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
// 이러한 타입을 갖는 std::atomic objects은 atomic 연산을 한다.
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
// std::atomic<T>의 objects는 memory에 존재하고
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
    // a *= 9; *= 연산은 정의되지 않았다.

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

void example2()
{
    std::atomic<int> a = 6;
    // a *= 9; *= 연산은 정의되지 않았다.

    // 위의 연산은 expected가 reference 전달이므로 다음과 같이
    // 간결하게 작성할 수 있다.
    int expected = a.load();
    while (!a.compare_exchange_weak(expected, expected * 9))
        ;
}
}
