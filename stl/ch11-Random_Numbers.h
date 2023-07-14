#pragma once

#include <stdlib.h>
#include <cassert>

//-----------------------------------------------------------------------------
// Random numbers versus pseoudo-random numbers
//-----------------------------------------------------------------------------
namespace section1
{
// 물리학적으로 비결정론적인 truly random numbers와
// 물리학적으로 결정론적인 알고리즘을 통해 만들어진 "random-looking"
// pseudo-random numbers를 구분할 필요가 있다.
// 그러한 알고리즘을 a pseudo-random number generator(PRNG)라고 부른다.
// 모든 PRNG는 internal state를 갖고, next output을 제공하는 방법을 갖는다.
// next output을 제공할 때마다, its internal state를 뒤죽박죽으로 만든다.
// 뒤죽박죽으로 만드는 방법은 적혀있는 코드대로 작동하므로 결정론적이다.
// 예를 들어, 다음 SimplePRNG는 a linear congruential generator를 구현한다.
template <class T>
class SimplePRNG
{
    uint32_t state = 1; // its internal state
public:
    static constexpr T min() { return 0; }
    static constexpr T max() { return 0x7FFF; }

    T operator()()
    {
        state = state * 1103515245 + 12345; // 정해진 방법대로 state를 수정한다.
        return (state >> 16) & 0x7FFF;
    }
};
// 내부 state는 32-bit range이고 operator()는 integers in 15-bit range를
// 생성한다. 이러한 패턴이 실제 PRNGs에 사용된다. 예를 들어,
// the standard Mersenne Twister algorithm (표준 메르센 트위스터 알고리즘)은
// 거의 20 kB 정도의 state를 갖는다. 리턴되는 값은 이보다 훨씬 적은 범위이다.
// 이렇게 많은 내부 state를 갖는다는 것은 뒤죽박죽해야하는 bits 수가 많음을
// 뜻한다. a small fraction of the PRNG's internal state만이 무작위수 생성에
// 사용하게 된다. 사람 또는 컴퓨터가 PRNG's next output을 예측하기 어렵게
// 만든다.
// 그래서 이를 a pseudo-random number generator라고 부른다.
// 반대로,
// 어떠한 패턴을 갖고 예측 가능하면 a non-random number generator라고 부른다.

// 물론 pseudo-random을 아무리 정교하게 만들어도, 적혀있는 코드대로 동작하므로
// a PRNG's behavior는 항상 perfectly deterministic하다.
// a PRNG를 사용하는 프로그램을 매번 돌려도 정확히 같은 sequence of
// pseudo-random numbers를 얻는다.
// 동일한 algorithm에 two generators with tiny variations in their initial
// states를 사용하면 이들의 variation이 급격하게 커져서 완벽히 서로 다른
// output sequences를 생성한다.
// 만약 프로그램을 돌릴 때마다 매번 다른 sequence of pseudo-random numbers를
// 사용하고 싶다면, PRNG의 초기값만 바꾸면 된다.
// Setting a PRNG's initial state를 seeding the PRNG 라고 부른다.
//
// Seeding the PRNG 하는 방법은 적어도 3개가 있다.
// 1. 외부(호출자, 엔드유저)로부터 공급받은 a seed를 사용한다.
// 2. 현재 시간기록과 같은 a predictable but variable seed를 사용한다.
// 3. 플랫폼의 "truly random" bits 로부터 a truly random seed를 사용한다.
//
// Truly random bits are collected by the operating system based on all sorts of
// random events. 전형적인 방법으로 every system call마다 the hardware cycle
// counter의 low-order bits를 모으고 이들을 XOR 연산을 하여 그 결과값을
// 운영체제의 the entropy pool에 집어 넣는다. 커널 내부의 a PRNG는 주기적으로
// the entropy pool로부터 얻은 bits를 가지고 reseed를 한다.
//
// 리눅스는 the raw entropy pool을 /dev/random 에 expose한다.
// the PRNG's output sequence는 /dev/urandom 에 expose한다.
// c++ 표준라이브러리는 이러한 장치들에 직접 접근할 필요없게 커버해준다.
// 이제 c++ 표준에서 제공하는 <random> library 를 공부해보자.
}

//-----------------------------------------------------------------------------
// The problem with rand()
//-----------------------------------------------------------------------------
namespace section2
{
// 옛날 C방법으로 rand()를 사용했다.
// rand()는 a uniformly distributed integer in the [0, RAND_MAX] range를
// 리턴한다. the internal state는 srand(seed_value)를 통해 seeding 한다.
namespace classic_code_to_generate_a_random_number
{
    // #include <stdlib.h>

    // [0,x) 범위의 무작위 정수를 생성한다.
    int randint0(int x) { return rand() % x; }
    // 첫번째 문제로, rand()는 [0,32767] 범위에서 균일하게 값을 리턴하므로
    // randint0(10)은 [0,7] 범위의 수들이 8 또는 9보다, 3276번당 한번 꼴로 더
    // 나오게 된다.

    // 두번째 문제로, rand()는 global state에 접근하므로
    // 모든 쓰레드가 같은 RNG를 공유하므로 쓰레드에 안전하지 않다. c++11에서
    // global mutex lock을 사용한다면 쓰레드에 안전하지만 성능에 문제가 된다.

    // 세번째 문제로, rand()는 global-statefulness하므로 어떠한 위치의 어떠한
    // 함수든지 rand()를 호출하여 the state를 변경할 수 있다. 그래서 a
    // unit-test-driven environment에서 사용할 수 없다.
    int heads(int n)
    {
        // DEBUG_LOG("heads");
        int result = 0;
        for (int i = 0; i < n; ++i) {
            result += (rand() % 2);
        }
        return result;
    }
    void test_heads()
    {
        srand(17);
        int result = heads(42);
        assert(result == 27);
    }
    // 중간에 다른 스레드에서 rand()를 호출한다면
    // result의 결과값이 변경되어 test를 방해하게 된다.
    // 또한 DEBUG_LOG 에 rand()의 호출을 추가하거나 제거하면 변경된다.
}
// the C library의 문제점은 a truly uniform distribution를 제공하지 못하고
// 근본적으로 global variables에 의존한다는 점이다.
// <random> 에서는 어떻게 이러한 문제들을 해결하는 지 살펴보자.
}
