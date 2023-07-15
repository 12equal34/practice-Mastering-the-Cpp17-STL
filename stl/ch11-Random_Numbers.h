#pragma once

#include <stdlib.h>
#include <cassert>
#include <random>
#include <array>
#include <vector>
#include <algorithm>
#include <numeric>

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

//-----------------------------------------------------------------------------
// Solving problems with <random>
//-----------------------------------------------------------------------------
namespace section3
{
// <random> header는 2개의 core concepts을 제공한다.
// 1. A generator    : a class modeling the UniformRandomBitGenerator concept
// 2. A distribution : a class modeling the RandomNumberDistribution concept
//
// A generator는 the internal state of a PRNG를 a C++ object로 캡슐화하고
// a next output member function을 operator()()로 제공한다.
// A distribution은 a generator의 output을 필터링하는 종류로서,
// 특정 수학적인 분포로 규정되고 특정 범위에 제한되는 데이터 값들을 얻는다.

// <random> header는 총 7개의 generator와 20개의 distribution types을 포함한다.
// 대부분은 templates으로 많은 템플릿 매개변수를 취한다.
}

//-----------------------------------------------------------------------------
// Dealing with generators
//-----------------------------------------------------------------------------
namespace section4
{
// Given any generator object g,
// g(): the internal state를 뒤죽박죽 만들고 its next output을 리턴한다.
// g.min(): g()의 the smallest possible output을 리턴한다.
// g.max(): g()의 the largest possible output을 리턴한다.
// g.discard(n): the internal state를 n번 뒤죽박죽 만든다. g()를 n번 호출하되
//               결과값은 무시한 효과와 같다.
}

//-----------------------------------------------------------------------------
// Truly random bits with std::random_device
//-----------------------------------------------------------------------------
namespace section5
{
// std::random_device는 template이 아닌 a generator이다.
// default constructor를 통해 인스턴스를 생성하면 operator()를 통해 a uniformly
// distributed unsigned integer in [rd.min(), rd.max()]를 생성할 수 있다.
void example()
{
    std::random_device rd;
    unsigned int       seed = rd();

    // 32-bits unsigned int 의 범위와 동일하다.
    // 0u <= seed <= std::numeric_limits<unsigned int>::max()
    assert(rd.min() <= seed && seed <= rd.max());
}
// std::random_device는 완벽히 UniformRandomBitGenerator concept을 따르지는
// 않는다. 복제 및 이동이 불가능하다.
// a truly random generator인 std::random_device는 보통 short-lived instance로
// 만들어서 a seed for a long-lived PRNG를 생성할 용도로 사용한다.
}

//-----------------------------------------------------------------------------
// Pseudo-random bits with std::mt19937
//-----------------------------------------------------------------------------
namespace section6
{
// 메르센 트위스터 알고리즘은 a whole family of related PRNGs를 정의한다.
// 마치 템플릿과 비슷하다. 그중 가장 흔하게 사용되는 member of the family는
// MT19937이다. the size in bits of the Twister's internal state를 의미한다.
// 모든 가능한 state를 돌면 다시 시작부분으로 돌아가므로 주기성을 갖는다.
// the period of the MT19937 generator는 2^19937 - 1 이다.
// 첫번째 절에서 SimplePRNG는 32bits의 state를 가지므로 주기가 2^31 이다.
// 주기가 길다고 RNG의 quality를 향상시켜주진 않지만, 주기가 짧으면 문제가 된다.

// std::mersenne_twister_engine<...> 를 통해 메르센 트위스터 알고리즘을
// 사용한다. 하지만 직접적으로 템플릿을 사용하지 않고 편의상 std::mt19937을
// 사용한다.
// std::mt19937은 32-bits word length를 생성한다.
// std::mt19937_64는 64-bits word length를 생성한다.
void example1()
{
    std::mt19937 g;
    assert(g.min() == 0 && g.max() == 4294967295);
    assert(g() == 3499211612);
    assert(g() == 581869302);
    assert(g() == 3890346734);
}
// std::mt19937의 default constructor는 its internal state를 잘 알려진 표준
// 값들로 설정하며 플랫폼에 상관없이 항상 같은 the output sequence를 보장한다.

// 생성자에 a seed를 전달하여 다른 output sequence를 얻는다.
// 다음과 같이 두가지 방법이 있다.
template <class It>
struct SeedSeq { // 이렇게 직접 작성하거나 std::seed_seq를 사용해도 된다.
    It begin_;
    It end_;
public:
    SeedSeq(It begin, It end)
        : begin_(begin),
          end_(end)
    { }

    template <class It2>
    void generate(It2 b, It2 e)
    {
        assert((e - b) <= (end_ - begin_));
        std::copy(begin_, begin_ + (e - b), b);
    }
};
void method1()
{
    std::random_device rd;
    uint32_t           numbers[624];
    std::generate(numbers, std::end(numbers), std::ref(rd));

    SeedSeq      sseq(numbers, std::end(numbers));
    std::mt19937 g(sseq);
}

void method2()
{
    std::random_device rd;
    std::mt19937       g(rd());
    // rd가 생성하는 truly random 32-bits integer로 충분하다.
    // 32는 19937보다 훨씬 작고,
    // 대략 2^32 = 4*10^9 개의 seeds 마다 다른 output sequences를 만들 수 있다.
    // 만약 the level of predictability가 중요하다면,
    // 메르센 트위스터 알고리즘은 암호학적으로 안전하지 않음에 유의해야 한다.
    // 리버스 엔지니어링을 통해 seed에 대한 19937 bits 를 파악하여 다음의 모든
    // output을 예측할 수 있다.
    // 만약 a cryptographically secure pseudo-random number generator(CSPRNG)가
    // 필요하다면, AES-CTR or ISAAC 같은 c++표준에서 제공하지 않는 것들을
    // 사용해야 한다. CSPRNG의 구현을 UniformRandomBitGenerator Model로 wrap해야
    // 한다.
}
}

//-----------------------------------------------------------------------------
// Filtering generator outputs with adaptors
//-----------------------------------------------------------------------------
namespace section7
{
// the raw output of a generator는 a distribution을 통해 usable data values로
// convert한다.
// 또한, a generator adapter를 통해 the raw bits를 다양한 방법으로 reformat할 수
// 있다. 표준라이브러리는 3개의 adaptors를 제공한다.
// 1. std::discard_block_engine
// 2. std::shuffle_order_engine
// 3. std::independent_bits_engine
// 이러한 adaptors는 the container adaptors(s.a. std::stack)과 비슷하다.
// std::discard_block_engine<Gen, p, r>은 an underlying generator를 유지하고
// operator()를 제외하고 모든 연산들을 delgate한다.
// operator()는 the first r of every p outputs from the underlying generator를
// 리턴한다.
void example()
{
    std::vector<uint32_t> raw(10), filtered(10);

    std::discard_block_engine<std::mt19937, 3, 2> g2;
    std::mt19937                                  g1 = g2.base();

    std::generate(raw.begin(), raw.end(), g1);
    std::generate(filtered.begin(), filtered.end(), g2);

    assert(raw[0] == filtered[0]);
    assert(raw[1] == filtered[1]);
    assert(raw[3] == filtered[2]);
    assert(raw[4] == filtered[3]);
}
// std::shuffle_order_engine<Gen, k>는
// operator()는 Y = buffer[Y % k]; buffer[Y] = base()(); 와 비슷하다.
// underlying generator가 이미 pseudo-random이기 때문에 the randomness에
// 영향을 주지 않는다.

// std::independent_bits_engine<Gen, w, T>는 T가 unsinged int이고
// w-bits 보다 작아야 한다.
// operator()는 적어도 w random bits를 계산할 때까지 base()()를 호출하고
// 호출값들을 정확히 w-bits 만큼 이어 붙어서 결과값에 넣는다.
void example2()
{
    std::independent_bits_engine<std::mt19937, 40, uint64_t> g2;
    std::mt19937                                             g1 = g2.base();

    assert(g1() == 0xd09'1bb5c);   // Take "1bb5c"... (20bits)
    assert(g1() == 0x22a'e9ef6);   // and "e9ef6"...  (20bits)
    assert(g2() == 0x1bb5c'e9ef6); // Paste and serve!(40bits)
}

void example3()
{
    using coinflipper = std::independent_bits_engine<std::mt19937, 1, uint16_t>;

    coinflipper         onecoin;
    std::array<int, 64> results;
    std::generate(results.begin(), results.end(), onecoin);
    assert((results == std::array<int, 64> {
                           {
                            0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0,
                            1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1,
                            1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0,
                            1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
                            }
    }));

    std::independent_bits_engine<coinflipper, 32, uint32_t> manycoins;
    assert(manycoins() == 0x1772af15);
    assert(manycoins() == 0x9e2a904c);
}
// the underlying generator의 bias를 유지한다.
// 짝수가 더 나오는 경향이 있는 WeightCoin에 대해서
// independent_bits_engine<WeightCoin, w,T>은 같은 경향을 유지한다.
}

//-----------------------------------------------------------------------------
// Dealing with distributions
//-----------------------------------------------------------------------------
namespace section8
{
// 이제 raw random bits를 distribution을 통해 usable numberic values로
// convert하는 방법을 배우자.
// 1. generate raw bits
// 2. format them into data values
// 주어진 any distribution 객체 dist에 대해서,
// dist(g) : 확률분포에 따라 next output을 생성한다. g()을 호출하거나 its
// internal state에 따라서 g()을 호출하지 않을 수 있다.
// dist.reset() : the internal state를 clear한다.
// dist.min(), dist.max() : 가능한 outputs 중 최소, 최대값을 알려준다.
}

//-----------------------------------------------------------------------------
// Rolling dice with uniform_int_distribution
//-----------------------------------------------------------------------------
namespace section9
{
// 닫힌 구간 [a,b]에 대해,
// 이항 확률 P(i|a,b) = 1/(b-a+1)를 갖는 무작위 정수 i를 구한다.
// std::uniform_int_distribution은 RandomNumberDistribution을 만족한다.

// std::uniform_int_distribution의 구현은 다음과 비슷하다.
template <class Int>
class uniform_int_distribution
{
    using UInt = std::make_unsigned_t<Int>;
    UInt m_min, m_max;
public:
    uniform_int_distribution(Int a, Int b)
        : m_min(a),
          m_max(b)
    { }

    template <class Gen>
    Int operator()(Gen& g)
    {
        UInt range = (m_max - m_min);
        assert(g.max() - g.min() >= range); // 실제구현은 assert하지 않는다.

        // 실제구현은 ceil(log2(range)) random bits를 사용할 것이고
        // 다음의 while문의 반복횟수를 최소화할 것이다.
        while (true) {
            UInt r = g() - g.min();
            if (r <= range) {
                return Int(m_min + r);
            }
        }
    }
};

// an empty range에 유용한 반복자에 대한 구간과는 다르게
// an integral numberic range는 닫힌 구간을 의미한다.
// 왜냐하면 [LLONG_MIN, LLONG_MAX +1)로 표현하면 LLONG_MAX + 1은 오버플로우가
// 발생하기 때문에 [LLONG_MIN, LLONG_MAX]로 표현해야 한다.
// 예를 들어, uniform_int_distribution<int>(0,6)은 [0,6]을 의미한다.
// uniform_int_distribution<int>(42,42)는 [42,42] = 42을 의미한다.
// 반면에,
// std::uniform_real_distribution<double>(a,b)는 [a,b)를 의미한다.
// 왜냐하면 [0, INFINITY) 이어야 오버플로우가 발생하지 않기 때문이다.
// std::uniform_real_distribution<float>(0,1)(g)는 소수점 올림에 의해
// float(1.0)을 리턴할 수 있다.
}

//-----------------------------------------------------------------------------
// Generating populations with normal_distribution
//-----------------------------------------------------------------------------
namespace section10
{
// 정규분포를 나타낸다.
// std::normal_distribution<double>(m, sd)는 N(m,sd^2)를 생성한다.
// 디폴트 생성자는 표준 정규분포 N(0,1)을 생성한다.
// 예를 들어, 인구수 10,000에 대해 키에 대한 정규분포로 생각하면
void example()
{
    double                           mean   = 161.8;
    double                           stddev = 6.8;
    std::normal_distribution<double> dist(mean, stddev);

    std::mt19937 g(std::random_device {}());

    std::vector<double> v;
    for (int i = 0; i < 10000; ++i) {
        v.push_back(dist(g));
    }
    std::sort(v.begin(), v.end());

    auto square = [](auto x) {
        return x * x;
    };
    double mean_of_values = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    double mean_of_squares =
        std::inner_product(v.begin(), v.end(), v.begin(), 0.0) / v.size();
    double actual_stddev = std::sqrt(mean_of_squares - square(mean_of_values));
    printf("Expected mean and stddev: %g, %g\n", mean, stddev);
    printf("Actual mean and stddev: %g, %g\n", mean_of_values, actual_stddev);
}

// std::normal_distribution은 stateful이기 때문에,
// 매번 값을 생성하기 위해 새로운 instance를 만드는 것은 비효율적이다.
// next values 간은 independent하고 항상 단일 값만 생성하므로
// 새로운 instance를 만들 필요가 없다.
}

//-----------------------------------------------------------------------------
// Making weighted choices with discrete_distribution
//-----------------------------------------------------------------------------
namespace section11
{
// std::discrete_distribution<int>(wbegin, wend)는
// 이항분포로 [0, wend - wbegin) 구간에 속하는 무작위 정수를
// 가중치 컨테이너의 구간 [wbegin, wend)대로 생성한다.
template <class Values, class Weights, class Gen>
auto weighted_choice(const Values& v, const Weights& w, Gen& g)
{
    auto dist  = std::discrete_distribution<int>(std::begin(w), std::end(w));
    int  index = dist(g);
    return v[index];
}
void test()
{
    auto                     g       = std::mt19937(std::random_device {}());
    std::vector<std::string> choices = {"quick", "brown", "fox"};
    std::vector<int>         weights = {1, 7, 2};
    std::string              word    = weighted_choice(choices, weights, g);

    if (word == "brown") {
        printf("확률 7/10 로 word == \"brown\"이 성립하였습니다.");
    }

    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    // a private member variable of type std::vector<double>을 갖는다.
    std::vector<double> probs = dist.probabilities();
    assert((probs == std::vector {0.1, 0.7, 0.2}));
}

// 직접 discrete_distribution을 사용하지 않고 함수 weighted_choice처럼
// 캡슐화하는 것이 좋다.

// 힙 할당 또는 부동소수점 계산을 피해야 한다면 다음과 같이 구현할 수 있다.
template <class Values, class Gen>
auto weighted_choice_non_allocating(const Values& v, const std::vector<int>& w,
                                    Gen& g)
{
    int  sum    = std::accumulate(w.begin(), w.end(), 0);
    int  cutoff = std::uniform_int_distribution<int>(0, sum - 1)(g);
    auto vi     = v.begin();
    auto wi     = w.begin();
    while (cutoff > *wi) {
        cutoff -= *wi++;
        ++vi;
    }
    return *vi;
}
// 하지만 부동소수점 계산을 하면 정수에 대한 오버플로우를 걱정할 필요가 없다.
// 여기서 sum이 오버플로우가 발생할 수 있다.
}

//-----------------------------------------------------------------------------
// Shuffling cards with std::shuffle
//-----------------------------------------------------------------------------
namespace section12
{
// std::shuffle(a,b,g)는 [a,b) 원소들을 무작위로 shuffle 한다.
// 예전의 std::random_shuffle(a,b)는 the global C library's rand()를 사용하기에
// 사용하면 안된다.
void example()
{
    std::vector<int> deck(52);
    std::iota(deck.begin(), deck.end(), 1);
    std::mt19937 g(std::random_device {}());

    std::shuffle(deck.begin(), deck.end(), g);
}

// 모든 플랫폼에서 고정된 값을 시드로 갖는 std::mt19937가 생성하는 the outputs는
// 동일하지만, distributions, shuffle algorithm 은 그렇지 않다.

// 위의 예제에서 메르센 트위스터가 생성하는 seed는 4*10^9 경우의 섞는 방법을
// 생성한다. 하지만 52개의 카드에 대해 52! = 8*10^67 경우의 섞는 방법이
// 존재하므로 실제 카지노 게임을 구현한다면, 성능에 대해 걱정하지 않는다면,
// 다음과 같이 구현한다.
void example2()
{
    std::vector<int> deck(52);
    std::iota(deck.begin(), deck.end(), 1);
    std::random_device rd;
    std::shuffle(deck.begin(), deck.end(), rd);
    // The deck is now TRULY randomly shuffled.
}
}