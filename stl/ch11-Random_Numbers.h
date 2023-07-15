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
// ������������ ����������� truly random numbers��
// ������������ ���������� �˰����� ���� ������� "random-looking"
// pseudo-random numbers�� ������ �ʿ䰡 �ִ�.
// �׷��� �˰����� a pseudo-random number generator(PRNG)��� �θ���.
// ��� PRNG�� internal state�� ����, next output�� �����ϴ� ����� ���´�.
// next output�� ������ ������, its internal state�� ���׹������� �����.
// ���׹������� ����� ����� �����ִ� �ڵ��� �۵��ϹǷ� ���������̴�.
// ���� ���, ���� SimplePRNG�� a linear congruential generator�� �����Ѵ�.
template <class T>
class SimplePRNG
{
    uint32_t state = 1; // its internal state
public:
    static constexpr T min() { return 0; }
    static constexpr T max() { return 0x7FFF; }

    T operator()()
    {
        state = state * 1103515245 + 12345; // ������ ������ state�� �����Ѵ�.
        return (state >> 16) & 0x7FFF;
    }
};
// ���� state�� 32-bit range�̰� operator()�� integers in 15-bit range��
// �����Ѵ�. �̷��� ������ ���� PRNGs�� ���ȴ�. ���� ���,
// the standard Mersenne Twister algorithm (ǥ�� �޸��� Ʈ������ �˰���)��
// ���� 20 kB ������ state�� ���´�. ���ϵǴ� ���� �̺��� �ξ� ���� �����̴�.
// �̷��� ���� ���� state�� ���´ٴ� ���� ���׹����ؾ��ϴ� bits ���� ������
// ���Ѵ�. a small fraction of the PRNG's internal state���� �������� ������
// ����ϰ� �ȴ�. ��� �Ǵ� ��ǻ�Ͱ� PRNG's next output�� �����ϱ� ��ư�
// �����.
// �׷��� �̸� a pseudo-random number generator��� �θ���.
// �ݴ��,
// ��� ������ ���� ���� �����ϸ� a non-random number generator��� �θ���.

// ���� pseudo-random�� �ƹ��� �����ϰ� ����, �����ִ� �ڵ��� �����ϹǷ�
// a PRNG's behavior�� �׻� perfectly deterministic�ϴ�.
// a PRNG�� ����ϴ� ���α׷��� �Ź� ������ ��Ȯ�� ���� sequence of
// pseudo-random numbers�� ��´�.
// ������ algorithm�� two generators with tiny variations in their initial
// states�� ����ϸ� �̵��� variation�� �ް��ϰ� Ŀ���� �Ϻ��� ���� �ٸ�
// output sequences�� �����Ѵ�.
// ���� ���α׷��� ���� ������ �Ź� �ٸ� sequence of pseudo-random numbers��
// ����ϰ� �ʹٸ�, PRNG�� �ʱⰪ�� �ٲٸ� �ȴ�.
// Setting a PRNG's initial state�� seeding the PRNG ��� �θ���.
//
// Seeding the PRNG �ϴ� ����� ��� 3���� �ִ�.
// 1. �ܺ�(ȣ����, ��������)�κ��� ���޹��� a seed�� ����Ѵ�.
// 2. ���� �ð���ϰ� ���� a predictable but variable seed�� ����Ѵ�.
// 3. �÷����� "truly random" bits �κ��� a truly random seed�� ����Ѵ�.
//
// Truly random bits are collected by the operating system based on all sorts of
// random events. �������� ������� every system call���� the hardware cycle
// counter�� low-order bits�� ������ �̵��� XOR ������ �Ͽ� �� �������
// �ü���� the entropy pool�� ���� �ִ´�. Ŀ�� ������ a PRNG�� �ֱ�������
// the entropy pool�κ��� ���� bits�� ������ reseed�� �Ѵ�.
//
// �������� the raw entropy pool�� /dev/random �� expose�Ѵ�.
// the PRNG's output sequence�� /dev/urandom �� expose�Ѵ�.
// c++ ǥ�ض��̺귯���� �̷��� ��ġ�鿡 ���� ������ �ʿ���� Ŀ�����ش�.
// ���� c++ ǥ�ؿ��� �����ϴ� <random> library �� �����غ���.
}

//-----------------------------------------------------------------------------
// The problem with rand()
//-----------------------------------------------------------------------------
namespace section2
{
// ���� C������� rand()�� ����ߴ�.
// rand()�� a uniformly distributed integer in the [0, RAND_MAX] range��
// �����Ѵ�. the internal state�� srand(seed_value)�� ���� seeding �Ѵ�.
namespace classic_code_to_generate_a_random_number
{
    // #include <stdlib.h>

    // [0,x) ������ ������ ������ �����Ѵ�.
    int randint0(int x) { return rand() % x; }
    // ù��° ������, rand()�� [0,32767] �������� �����ϰ� ���� �����ϹǷ�
    // randint0(10)�� [0,7] ������ ������ 8 �Ǵ� 9����, 3276���� �ѹ� �÷� ��
    // ������ �ȴ�.

    // �ι�° ������, rand()�� global state�� �����ϹǷ�
    // ��� �����尡 ���� RNG�� �����ϹǷ� �����忡 �������� �ʴ�. c++11����
    // global mutex lock�� ����Ѵٸ� �����忡 ���������� ���ɿ� ������ �ȴ�.

    // ����° ������, rand()�� global-statefulness�ϹǷ� ��� ��ġ�� ���
    // �Լ����� rand()�� ȣ���Ͽ� the state�� ������ �� �ִ�. �׷��� a
    // unit-test-driven environment���� ����� �� ����.
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
    // �߰��� �ٸ� �����忡�� rand()�� ȣ���Ѵٸ�
    // result�� ������� ����Ǿ� test�� �����ϰ� �ȴ�.
    // ���� DEBUG_LOG �� rand()�� ȣ���� �߰��ϰų� �����ϸ� ����ȴ�.
}
// the C library�� �������� a truly uniform distribution�� �������� ���ϰ�
// �ٺ������� global variables�� �����Ѵٴ� ���̴�.
// <random> ������ ��� �̷��� �������� �ذ��ϴ� �� ���캸��.
}

//-----------------------------------------------------------------------------
// Solving problems with <random>
//-----------------------------------------------------------------------------
namespace section3
{
// <random> header�� 2���� core concepts�� �����Ѵ�.
// 1. A generator    : a class modeling the UniformRandomBitGenerator concept
// 2. A distribution : a class modeling the RandomNumberDistribution concept
//
// A generator�� the internal state of a PRNG�� a C++ object�� ĸ��ȭ�ϰ�
// a next output member function�� operator()()�� �����Ѵ�.
// A distribution�� a generator�� output�� ���͸��ϴ� �����μ�,
// Ư�� �������� ������ �����ǰ� Ư�� ������ ���ѵǴ� ������ ������ ��´�.

// <random> header�� �� 7���� generator�� 20���� distribution types�� �����Ѵ�.
// ��κ��� templates���� ���� ���ø� �Ű������� ���Ѵ�.
}

//-----------------------------------------------------------------------------
// Dealing with generators
//-----------------------------------------------------------------------------
namespace section4
{
// Given any generator object g,
// g(): the internal state�� ���׹��� ����� its next output�� �����Ѵ�.
// g.min(): g()�� the smallest possible output�� �����Ѵ�.
// g.max(): g()�� the largest possible output�� �����Ѵ�.
// g.discard(n): the internal state�� n�� ���׹��� �����. g()�� n�� ȣ���ϵ�
//               ������� ������ ȿ���� ����.
}

//-----------------------------------------------------------------------------
// Truly random bits with std::random_device
//-----------------------------------------------------------------------------
namespace section5
{
// std::random_device�� template�� �ƴ� a generator�̴�.
// default constructor�� ���� �ν��Ͻ��� �����ϸ� operator()�� ���� a uniformly
// distributed unsigned integer in [rd.min(), rd.max()]�� ������ �� �ִ�.
void example()
{
    std::random_device rd;
    unsigned int       seed = rd();

    // 32-bits unsigned int �� ������ �����ϴ�.
    // 0u <= seed <= std::numeric_limits<unsigned int>::max()
    assert(rd.min() <= seed && seed <= rd.max());
}
// std::random_device�� �Ϻ��� UniformRandomBitGenerator concept�� ��������
// �ʴ´�. ���� �� �̵��� �Ұ����ϴ�.
// a truly random generator�� std::random_device�� ���� short-lived instance��
// ���� a seed for a long-lived PRNG�� ������ �뵵�� ����Ѵ�.
}

//-----------------------------------------------------------------------------
// Pseudo-random bits with std::mt19937
//-----------------------------------------------------------------------------
namespace section6
{
// �޸��� Ʈ������ �˰����� a whole family of related PRNGs�� �����Ѵ�.
// ��ġ ���ø��� ����ϴ�. ���� ���� ���ϰ� ���Ǵ� member of the family��
// MT19937�̴�. the size in bits of the Twister's internal state�� �ǹ��Ѵ�.
// ��� ������ state�� ���� �ٽ� ���ۺκ����� ���ư��Ƿ� �ֱ⼺�� ���´�.
// the period of the MT19937 generator�� 2^19937 - 1 �̴�.
// ù��° ������ SimplePRNG�� 32bits�� state�� �����Ƿ� �ֱⰡ 2^31 �̴�.
// �ֱⰡ ��ٰ� RNG�� quality�� ���������� ������, �ֱⰡ ª���� ������ �ȴ�.

// std::mersenne_twister_engine<...> �� ���� �޸��� Ʈ������ �˰�����
// ����Ѵ�. ������ ���������� ���ø��� ������� �ʰ� ���ǻ� std::mt19937��
// ����Ѵ�.
// std::mt19937�� 32-bits word length�� �����Ѵ�.
// std::mt19937_64�� 64-bits word length�� �����Ѵ�.
void example1()
{
    std::mt19937 g;
    assert(g.min() == 0 && g.max() == 4294967295);
    assert(g() == 3499211612);
    assert(g() == 581869302);
    assert(g() == 3890346734);
}
// std::mt19937�� default constructor�� its internal state�� �� �˷��� ǥ��
// ����� �����ϸ� �÷����� ������� �׻� ���� the output sequence�� �����Ѵ�.

// �����ڿ� a seed�� �����Ͽ� �ٸ� output sequence�� ��´�.
// ������ ���� �ΰ��� ����� �ִ�.
template <class It>
struct SeedSeq { // �̷��� ���� �ۼ��ϰų� std::seed_seq�� ����ص� �ȴ�.
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
    // rd�� �����ϴ� truly random 32-bits integer�� ����ϴ�.
    // 32�� 19937���� �ξ� �۰�,
    // �뷫 2^32 = 4*10^9 ���� seeds ���� �ٸ� output sequences�� ���� �� �ִ�.
    // ���� the level of predictability�� �߿��ϴٸ�,
    // �޸��� Ʈ������ �˰����� ��ȣ�������� �������� ������ �����ؾ� �Ѵ�.
    // ������ �����Ͼ�� ���� seed�� ���� 19937 bits �� �ľ��Ͽ� ������ ���
    // output�� ������ �� �ִ�.
    // ���� a cryptographically secure pseudo-random number generator(CSPRNG)��
    // �ʿ��ϴٸ�, AES-CTR or ISAAC ���� c++ǥ�ؿ��� �������� �ʴ� �͵���
    // ����ؾ� �Ѵ�. CSPRNG�� ������ UniformRandomBitGenerator Model�� wrap�ؾ�
    // �Ѵ�.
}
}

//-----------------------------------------------------------------------------
// Filtering generator outputs with adaptors
//-----------------------------------------------------------------------------
namespace section7
{
// the raw output of a generator�� a distribution�� ���� usable data values��
// convert�Ѵ�.
// ����, a generator adapter�� ���� the raw bits�� �پ��� ������� reformat�� ��
// �ִ�. ǥ�ض��̺귯���� 3���� adaptors�� �����Ѵ�.
// 1. std::discard_block_engine
// 2. std::shuffle_order_engine
// 3. std::independent_bits_engine
// �̷��� adaptors�� the container adaptors(s.a. std::stack)�� ����ϴ�.
// std::discard_block_engine<Gen, p, r>�� an underlying generator�� �����ϰ�
// operator()�� �����ϰ� ��� ������� delgate�Ѵ�.
// operator()�� the first r of every p outputs from the underlying generator��
// �����Ѵ�.
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
// std::shuffle_order_engine<Gen, k>��
// operator()�� Y = buffer[Y % k]; buffer[Y] = base()(); �� ����ϴ�.
// underlying generator�� �̹� pseudo-random�̱� ������ the randomness��
// ������ ���� �ʴ´�.

// std::independent_bits_engine<Gen, w, T>�� T�� unsinged int�̰�
// w-bits ���� �۾ƾ� �Ѵ�.
// operator()�� ��� w random bits�� ����� ������ base()()�� ȣ���ϰ�
// ȣ�Ⱚ���� ��Ȯ�� w-bits ��ŭ �̾� �پ ������� �ִ´�.
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
// the underlying generator�� bias�� �����Ѵ�.
// ¦���� �� ������ ������ �ִ� WeightCoin�� ���ؼ�
// independent_bits_engine<WeightCoin, w,T>�� ���� ������ �����Ѵ�.
}

//-----------------------------------------------------------------------------
// Dealing with distributions
//-----------------------------------------------------------------------------
namespace section8
{
// ���� raw random bits�� distribution�� ���� usable numberic values��
// convert�ϴ� ����� �����.
// 1. generate raw bits
// 2. format them into data values
// �־��� any distribution ��ü dist�� ���ؼ�,
// dist(g) : Ȯ�������� ���� next output�� �����Ѵ�. g()�� ȣ���ϰų� its
// internal state�� ���� g()�� ȣ������ ���� �� �ִ�.
// dist.reset() : the internal state�� clear�Ѵ�.
// dist.min(), dist.max() : ������ outputs �� �ּ�, �ִ밪�� �˷��ش�.
}

//-----------------------------------------------------------------------------
// Rolling dice with uniform_int_distribution
//-----------------------------------------------------------------------------
namespace section9
{
// ���� ���� [a,b]�� ����,
// ���� Ȯ�� P(i|a,b) = 1/(b-a+1)�� ���� ������ ���� i�� ���Ѵ�.
// std::uniform_int_distribution�� RandomNumberDistribution�� �����Ѵ�.

// std::uniform_int_distribution�� ������ ������ ����ϴ�.
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
        assert(g.max() - g.min() >= range); // ���������� assert���� �ʴ´�.

        // ���������� ceil(log2(range)) random bits�� ����� ���̰�
        // ������ while���� �ݺ�Ƚ���� �ּ�ȭ�� ���̴�.
        while (true) {
            UInt r = g() - g.min();
            if (r <= range) {
                return Int(m_min + r);
            }
        }
    }
};

// an empty range�� ������ �ݺ��ڿ� ���� �������� �ٸ���
// an integral numberic range�� ���� ������ �ǹ��Ѵ�.
// �ֳ��ϸ� [LLONG_MIN, LLONG_MAX +1)�� ǥ���ϸ� LLONG_MAX + 1�� �����÷ο찡
// �߻��ϱ� ������ [LLONG_MIN, LLONG_MAX]�� ǥ���ؾ� �Ѵ�.
// ���� ���, uniform_int_distribution<int>(0,6)�� [0,6]�� �ǹ��Ѵ�.
// uniform_int_distribution<int>(42,42)�� [42,42] = 42�� �ǹ��Ѵ�.
// �ݸ鿡,
// std::uniform_real_distribution<double>(a,b)�� [a,b)�� �ǹ��Ѵ�.
// �ֳ��ϸ� [0, INFINITY) �̾�� �����÷ο찡 �߻����� �ʱ� �����̴�.
// std::uniform_real_distribution<float>(0,1)(g)�� �Ҽ��� �ø��� ����
// float(1.0)�� ������ �� �ִ�.
}

//-----------------------------------------------------------------------------
// Generating populations with normal_distribution
//-----------------------------------------------------------------------------
namespace section10
{
// ���Ժ����� ��Ÿ����.
// std::normal_distribution<double>(m, sd)�� N(m,sd^2)�� �����Ѵ�.
// ����Ʈ �����ڴ� ǥ�� ���Ժ��� N(0,1)�� �����Ѵ�.
// ���� ���, �α��� 10,000�� ���� Ű�� ���� ���Ժ����� �����ϸ�
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

// std::normal_distribution�� stateful�̱� ������,
// �Ź� ���� �����ϱ� ���� ���ο� instance�� ����� ���� ��ȿ�����̴�.
// next values ���� independent�ϰ� �׻� ���� ���� �����ϹǷ�
// ���ο� instance�� ���� �ʿ䰡 ����.
}

//-----------------------------------------------------------------------------
// Making weighted choices with discrete_distribution
//-----------------------------------------------------------------------------
namespace section11
{
// std::discrete_distribution<int>(wbegin, wend)��
// ���׺����� [0, wend - wbegin) ������ ���ϴ� ������ ������
// ����ġ �����̳��� ���� [wbegin, wend)��� �����Ѵ�.
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
        printf("Ȯ�� 7/10 �� word == \"brown\"�� �����Ͽ����ϴ�.");
    }

    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    // a private member variable of type std::vector<double>�� ���´�.
    std::vector<double> probs = dist.probabilities();
    assert((probs == std::vector {0.1, 0.7, 0.2}));
}

// ���� discrete_distribution�� ������� �ʰ� �Լ� weighted_choiceó��
// ĸ��ȭ�ϴ� ���� ����.

// �� �Ҵ� �Ǵ� �ε��Ҽ��� ����� ���ؾ� �Ѵٸ� ������ ���� ������ �� �ִ�.
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
// ������ �ε��Ҽ��� ����� �ϸ� ������ ���� �����÷ο츦 ������ �ʿ䰡 ����.
// ���⼭ sum�� �����÷ο찡 �߻��� �� �ִ�.
}

//-----------------------------------------------------------------------------
// Shuffling cards with std::shuffle
//-----------------------------------------------------------------------------
namespace section12
{
// std::shuffle(a,b,g)�� [a,b) ���ҵ��� �������� shuffle �Ѵ�.
// ������ std::random_shuffle(a,b)�� the global C library's rand()�� ����ϱ⿡
// ����ϸ� �ȵȴ�.
void example()
{
    std::vector<int> deck(52);
    std::iota(deck.begin(), deck.end(), 1);
    std::mt19937 g(std::random_device {}());

    std::shuffle(deck.begin(), deck.end(), g);
}

// ��� �÷������� ������ ���� �õ�� ���� std::mt19937�� �����ϴ� the outputs��
// ����������, distributions, shuffle algorithm �� �׷��� �ʴ�.

// ���� �������� �޸��� Ʈ�����Ͱ� �����ϴ� seed�� 4*10^9 ����� ���� �����
// �����Ѵ�. ������ 52���� ī�忡 ���� 52! = 8*10^67 ����� ���� �����
// �����ϹǷ� ���� ī���� ������ �����Ѵٸ�, ���ɿ� ���� �������� �ʴ´ٸ�,
// ������ ���� �����Ѵ�.
void example2()
{
    std::vector<int> deck(52);
    std::iota(deck.begin(), deck.end(), 1);
    std::random_device rd;
    std::shuffle(deck.begin(), deck.end(), rd);
    // The deck is now TRULY randomly shuffled.
}
}