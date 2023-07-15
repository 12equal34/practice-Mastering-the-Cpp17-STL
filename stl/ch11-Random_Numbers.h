#pragma once

#include <stdlib.h>
#include <cassert>
#include <random>

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

// std::mersenne_twister_engine<...> �� ���� �޸��� Ʈ������ �˰�����
// ����Ѵ�. ������ ���������� ���ø��� ������� �ʰ� ���ǻ� std::mt19937��
// ����Ѵ�.
void example1()
{
    std::mt19937 g; // default constructor��
    assert(g.min() == 0 && g.max() == 4294967295);
    assert(g() == 3499211612);
    assert(g() == 581869302);
    assert(g() == 3890346734);
}
// std::mt19937�� default constructor�� its internal state�� �� �˷��� ǥ��
// ����� �����ϸ� �÷����� ������� �׻� ���� the output sequence�� �����Ѵ�.
}
