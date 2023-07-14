#pragma once

#include <stdlib.h>
#include <cassert>

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
