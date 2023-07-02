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
// ���ۿ� �˻��ؼ� ã�ƺ���.
// volatile�� ���� �������鿡 ���� �ذ�å����
// c++�� std::atomic�� �����Ѵ�.
}

//-----------------------------------------------------------------------------
// [Using std::atomic<T> for thread-safe accesses]
//-----------------------------------------------------------------------------
namespace section2
{
// std::atomic<T>�� std::vectoró�� class template�̶�� ������ �� ������
// std::atomic<T>�� �����Ϸ��� �Բ� �����ϵ��� ��������� ������
// �ܼ��� class template���� �����ϸ� �ȵȴ�.
// T�� ������ Ÿ���� ���� ������,
// �����δ� atomic-friendly type�� ����ؾ� �Ѵ�.
// atomic-friendly types�� ���� Ÿ�Ե�� ������ Ÿ�Ե��̴�. (�������� ũ�� ����)
// �̷��� Ÿ���� ���� std::atomic objects�� atomic ������ �Ѵ�.
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
        // y�� �Ҵ� ������ x�� �Ҵ�Ǿ���
        // �ٸ� �������� ���������� ������ ����ȴ�.
        assert(x == 0x42'00000042);
    }
}
// std::atomic<T>�� atomic, thread-safe assignment�� �����ϵ���
// (++,--,+=,-=, for integral types &=,|=,^=)�� �����ε��Ѵ�.

// [�߿�]
// std::atomic<T>�� objects�� memory�� �����ϰ�
// type T�� values�� CPU register �Ǵ� cache�� �����Ѵ�.
//
// �׷��� std::atomic�� copy-assignment, move-assignment operator�� ��������
// �ʴ´�. �ֳ��ϸ� ������ ���� �ۼ��Ͽ��� ��,
// std::atomic<int> a,b;
//
// a = b; �� ������ ������ ����.
// b�� value�� register�� load�ϰ�, register�� value�� a�� store��
// �ؾ��Ѵ�. �̴� two different atomic operations�� �ؾ��Ѵ�.
//
// �ƴϸ� ��ٷ� b���� a�� �����ϴ� ���� �����ϴٸ�
// two different memory location�� an atomic operation���� �����ؾ� �ϴµ�
// �̴� ��κ��� ��ǻ�� �ϵ����� �Ұ����ϴ�.
//
// C++ ������ ��Ȯ�ϰ� register���� object��,
// �Ǵ� object���� register���� �����ڷ� �ۼ��ؾ� �Ѵ�.
// int shortlived = b; // atomic load
// a = shrotlived; // atomic store
//
// �Ǵ� std::atomic<T>�� �����ϴ� ����Լ��� .load(), .store(v)�� ����Ѵ�.
// int shortlived = b.load(); // atomic load
// a.store(shortlived); // atomic store
//
// b.stort(a.load())�� �� �ٿ� �ۼ��� �� ������, �̷��� �ۼ����� ���ƾ� �Ѵ�.
// ���ÿ� ȣ���ϴ� ������ ������ �� �ִ�.
}

//-----------------------------------------------------------------------------
// [Doing complicated operations atomically]
//-----------------------------------------------------------------------------
namespace section3
{
// std::atomic���� *=, /=, %=, <<= and >>= �� ��� �ϵ�����
// ȿ�������� ����ϱ� ���� �����Ǿ���.
// ������ �̷��� ������� expensive�ϰ� ������ �� �ִ�.
// �츮�� �ϵ��� atomic multiply instruction�� �������� �ʴ´ٰ� ����.
// �츮�� �̸� a primitive atomic operation known as "compare and swap",
// or in C++ "compare-exchange"�� ����Ͽ� �����Ѵ�.
void example()
{
    std::atomic<int> a = 6;
    // a *= 9; *= ������ ���ǵ��� �ʾҴ�.

    int expected, desired;
    do {
        expected = a.load();
        desired  = expected * 9;
    } while (!a.compare_exchange_weak(expected, desired));
    // ���� ���� expected�� �����ϸ�, ���� desired�� �����Ѵ�.
    // (a read-modify-write operation)
    // ���� ���� expected�� (�ٸ� �����尡 �����ؼ�) �ٸ���,
    // ���� expected ���� load�Ѵ�. (a load operation)

    // ���� ������ ��ġ��,
    // a�� ���� atomically multiplied by 9�� �ȴ�.
}

void example2()
{
    std::atomic<int> a = 6;
    // a *= 9; *= ������ ���ǵ��� �ʾҴ�.

    // ���� ������ expected�� reference �����̹Ƿ� ������ ����
    // �����ϰ� �ۼ��� �� �ִ�.
    int expected = a.load();
    while (!a.compare_exchange_weak(expected, expected * 9))
        ;
}
}
