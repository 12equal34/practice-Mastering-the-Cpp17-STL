#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

// https://modoocode.com/271

//-----------------------------------------------------------------------------
// ĳ��
//-----------------------------------------------------------------------------
namespace section1
{
// CPU�� ��ǻ�� �޸��� RAM�� ���������� ������ �־
// �޸𸮷κ��� �����͸� �о� ���� ���ؼ��� �ð��� ���� �ɸ���.
// ���� ���,
// �޸𸮷κ��� �����͸� �о���µ� 42 ����Ŭ�� �ɸ���
// ���� ������ 1����Ŭ�� �ɸ���.
// �׷��� �о���� �ð��� ��ٸ��� 42�� ���������� ��ģ��.
//
// CPU Ĩ �ȿ� �ִ� ���� �޸��� ĳ�� Cache�� �����Ѵ�.
// ���������� CPU�� �پ� �ֱ� ������ �б�/���� �ӵ��� �ξ� ������.
//
// ���� ���,
// �ھ�� �������� �ʴ� L1,L2 ĳ�ð� �����Ѵ�.
// L1 ĳ�� : 32KB,			r/w �ӵ� = 4 ����Ŭ
// L2 ĳ�� : 256KB			r/w �ӵ� = 12 ����Ŭ
// �����ϴ� L3 ĳ�� : 8MB		r/w �ӵ� = 36 ����Ŭ
//
// CPU�� Ư���� �ּҿ� �ִ� �����Ϳ� �����Ѵٸ�,
// L1, L2, L3, �޸� ������ �����͸� Ȯ���Ѵ�.
// �����͸� ĳ�ÿ��� �������� Cache hit ��� �Ѵ�.
// �����͸� �޸𸮱��� ���� �������� Cache miss ��� �Ѵ�.
//
// CPU�� �޸𸮸� ������ ĳ�ÿ� �����س��´�.
// ���� ĳ�ð� �� á�ٸ� Ư���� ��Ŀ� ���� ó���Ѵ�.
// Ư���� ����� CPU ���� �ٸ���.
// ��ǥ���� ���� Least Recently Used (LRU) Cache�� ���ο� ĳ�ø� ����ϴ� �����
// �ִ�. (���� ������ ĳ�ø� ���ο� ĳ�÷� ������.)
//
// ���� ���, ĳ�ð� 1KB ũ���̰� LRU ����� ����ϴ� CPU�� ���ؼ� ������ ����.
void cache_miss()
{
    for (int i = 0; i < 10000; ++i) {
        for (int j = 0; j < 10000; ++j) {
            // s += data[j];
        }
    }
    // j = 256�� �Ǿ��� ��, data[0]�� ĳ�ÿ��� �������.
    // ���� �������� data[0]�� ĳ�ÿ� �������� �����Ƿ� �޸𸮿��� �����´�.
}

void cache_hit()
{
    for (int j = 0; j < 10000; j++) {
        for (int i = 0; i < 10000; i++) {
            // s += data[j];
        }
        // ó�� data[0]�� ������ �� �޸𸮿��� �����´�.
        // ���� data[0]�� ��� ĳ�ÿ��� �����´�.
    }
}
}

//-----------------------------------------------------------------------------
// ���ġ (reordering)
//-----------------------------------------------------------------------------
namespace section2
{
// ��ǻ�ʹ� �ۼ��� �ڵ� ������ �״�� ����� �������� �ʴ´�.
int a = 0;
int b = 0;

void foo()
{
    a = b + 1;
    b = 1;
}
// ����� �ڵ忡�� a = b + 1�� ���Ե� ��ɾ���� ������ ���� b = 1�� ����
// ����� �� �ִ�. �̷��� �����Ϸ��� ��ɾ ���ġ�ϴ� ������ CPU�� �ѹ��� ��
// ��ɾ �������� �ʰ� CPU ���������̴��� ��ġ�� �����̴�.
// 1. ��ɾ �д´�. (fetch)
// 2. ��ɾ �ؼ��Ѵ�. (decode)
// 3. ��ɾ �����Ѵ�. (execute)
// 4. ���� ����� ����. (write)
// �����Ϸ��� �ִ��� CPU�� ������������ ȿ�������� Ȱ���� �� �ֵ��� ��ɾ
// ���ġ�Ѵ�. ���� �� �����Ϸ������� ��ɾ ���ġ�ϴ� ���� �ƴ϶� CPU������
// ĳ�ÿ� �ִ� �����Ϳ� �����ϴ� ���� ������ ������ a = 1; (ĳ�ÿ� ����) b = 1;
// (ĳ�ÿ� ����) ���� ��쿡 b = 1�� ���� ����� �� �ִ�.

// �̷��� ��ɾ� ������ �ٲ�µ�,
// C++�� ��� ��ü���� ���� ����(modification order)�� ������ �� �ִ�.
// ������ ������ �� ��쿡 ��� �����忡�� ���� ��ü�� ���ؼ� ������ ���� ������
// ������ �� �ִ�.
// int a�� 1 -> 2 -> 3 -> 4 ������ ���� ���Ѵٰ� ����.
// T1 ���� ������ ���� ���� : 1 -> 4
// T2 ���� ������ ���� ���� : 2 -> 3
// T3 ���� ������ ���� ���� : 2 -> 3 -> 4
// ��� �����忡���� ������ �� ���� ���� ���� : 4->2, 3->2, 2->1 (����)
// ���� �ð��� ���� �ٸ� �����尡 a�� ���� Ȯ���� �� ���� �ٸ� ���� �� �ִ�.
// ���� ������ ������ �ȴ�.
// ���� ���, T2���� a = 3 �϶�, �ڽ� �ھ��� ĳ�ÿ��� ����� ���� �ٸ�
// �ھ�鿡�� �˸��� �ʴ´ٸ�, T3�� ������ �ð��� a = 3 �� �ƴ� �� �ִ�.
// �׷��ٰ� ��� ĳ�ÿ� ����ȭ �۾��� �Ѵٸ� ���� ���̴�.
}

//-----------------------------------------------------------------------------
// ���ڼ� (atomicity)
//-----------------------------------------------------------------------------
namespace section3
{
// C++���� ��� ������� �������� ��,
// ��������� ���� ��ü�� ���ؼ� ������ ���� ������ ������ �� �ִ�.
// �׷��� ������ ������ �ƴ� ��� ������ ���� ������ ������� �ʾƼ�
// ���α׷��� undefined behavior�� �Ͼ��.

// ������(atomic) ���� : CPU�� ��ɾ� 1���� ó���ϴ� ��ɾ��̴�. �߰��� �ٸ�
// �����尡 ����� �� ����. �� ������ �� ���� �������� �ɰ� �� ���, �� ������
// �ߴ� Ȥ�� ���ߴٷ� �Ǵ��� �� �ִ�.

// ������ ������ ���ؽ��� �ʿ��� �ӵ��� ������.
// std::atomic<T>�� ���������� ����� ���� Ÿ�� T�� �����Ͽ�
// atomic���� �����ϴ� �Լ���� �������� �������� ������� ������ �� �ִ�.
void worker(std::atomic<int>& counter)
{
    for (int i = 0; i < 10000; ++i) {
        ++counter; // ���ؽ��� ��ȣ���� �ʾƵ� �ȴ�.
        // ����� �ڵ忡�� lock add DWORD PTR [rdi], 1 �� �����Ѵ�.
        // (���⼭ lock�� �ش� ����� ���������� �����϶�� �ǹ̴�.)
        // �� ��ɾ� ���ٷ� ��Ÿ����. ������ �̷��� ��ɾ �������� �ʴ� CPU��
        // atomic ��ü�� ������� ���������� ������ �� ����.
        // �̴� is_lock_free()�� ���� Ȯ���� �� �ִ�.
    }
}

void test()
{
    using std::ref;

    std::atomic<int> counter(0);

    std::cout << "int�� ���� ������ ������ �����ϴ� ��? : " << std::boolalpha
              << counter.is_lock_free() << std::endl;
    // lock free�� ���ؽ��� ���� ��ü���� lock, unlock ���� ������ ������ ��
    // �ִٴ� �ǹ̴�.

    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back(worker, ref(counter));
    }

    std::for_each(workers.begin(), workers.end(), [](auto&& t) { t.join(); });

    std::cout << "Counter ���� ��: " << counter << std::endl;
}
}

//-----------------------------------------------------------------------------
// memory_order
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace section4
{
// atomic ��ü���� ������ ������ �� ��, �޸𸮿� �����ϴ� ����� ������ �� �ִ�.

// 1. memory_order_relexed
// �޸� ���� ������ �ٲ�� ���� ����Ѵ�.
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
    // ����� x:0 y:0 �Ǵ� x:1 y:0 �Ǵ� x:0 y:1 �Ǵ� x:1 y:1 �� ���´�.
    // CPU�� ������ ���ġ�� �� �־ �ſ� ���� �ӵ��� ����ȴ�.
}

// ���� ������ ++counter; �κ��� memory_order_relaxed ������� ����� �� �ִ�.
// counter.fetch_add(1, memory_order_relaxed);�� ����� ���ִ�.

// 2. memory_order_acquire, memory_order_release

// producer-consumer ���迡���� memory_order_relaxed�� ����ϸ� �� �ȴ�.
namespace wrong
{
    void producer(std::atomic<bool>& is_ready, int& data)
    {
        data = 10;
        is_ready.store(true, std::memory_order_relaxed);
        // ���⼭ data = 10�� ���ԵǱ� ���� is_ready�� true�� �Ǹ�,
    }
    void consumer(std::atomic<bool>& is_ready, int& data)
    {
        // is_ready_load�� true�� �Ǿ� data�� �غ�Ǿ��ٰ� �����Ͽ�,
        while (!is_ready.load(std::memory_order_relaxed)) {
        }
        // data = 0�� ä�� �����͸� �а� �ȴ�.
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
// producer-consumer ���迡���� memory_order_acquire, memory_order_release��
// ����Ѵ�.
namespace right
{
    void producer(std::atomic<bool>& is_ready, int& data)
    {
        data = 10;
        is_ready.store(true, std::memory_order_release);
        // memory_order_release�� ������ ��� �޸� ��ɵ���
        // �� �������� ���ġ�Ǵ� ���� �����Ѵ�.
    }
    void consumer(std::atomic<bool>& is_ready, int& data)
    {
        // memory_order_acquire�� ������ ��� �޸� ��ɵ���
        // �� �������� ���ġ�Ǵ� ���� �����Ѵ�.
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
// �� ���� �����尡 ���� ������ memory_order_acquire, memory_order_release��
// ���ؼ� ����ȭ�� �����ϰ� �ִ�.
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
    // �׻� ������ ���� ����Ѵ�.
    //      data[0] : 1
    //      data[1] : 2
    //      data[2] : 3
}

// memory_order_acq_rel�� acquire �� release�� ��� �����Ѵ�.

// memory_order_seq_cst�� �޸� ����� ������ �ϰ��� (sequential consistency)
// �� �����Ѵ�.
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
            // write_x�� read_x_then_y ���� ����ȭ�� �Ѵ�.
        }
        // ���� ������ c ���忡�� x.store�� y.store���� ���� �߻��ϰ�,
        if (y.load(std::memory_order_acquire)) {
            ++z;
        }
    }
    void read_y_then_x()
    {
        while (!y.load(std::memory_order_acquire)) {
            // write_y�� read_y_then_x ���� ����ȭ�� �Ѵ�.
        }
        // ���� ������ d ���忡�� y.store�� x.store���� ���� �߻��� �� �ִ�.
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
        // z: 0 �� ����� �� �ִ�.
    }
}
// memory_order_seq_cst�� CPU�� ����ȭ�� ���� ��� �����忡�� ������
// ���� ������ ������ �� �ֵ��� �����Ѵ�.
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
        // z: 0 �� ����� �� ����.
    }

}
// atomic ��ü�� default memory_order�� memory_order_seq_cst �̴�.
// ++counter�� counter.fetch_add(1,memory_order_seq_cst)�� �ȴ�.
// ��Ƽ �ھ� �ý��ۿ��� memory_order_seq_cst �� ��� �����̰�
// Ư�� ARM �迭�� CPU���� ����ȭ ����� ũ��.
// ���� Ȥ�� AMD�� x86(-64) CPU�� ���� ������ �ϰ����� ����Ǳ� ������
// memory_order_seq_cst�� �����ϴ��� �� ���̰� ũ�� �ʴ�.
// (�׷��� z: 0 �� �ǰų� ������ x:0 y:0 �� ����ϴ� ��찡 �幰��.)
}