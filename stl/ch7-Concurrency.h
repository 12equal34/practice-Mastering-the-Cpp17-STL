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
// old-style C �Ǵ� ���� 10�� ���� ����ߴ�
// volatile keyword�� ��� �������� ������?
// ���� ���� �޸𸮿� ���� �����ϱ� ���Ѵٸ�, volatile�� ����ص� �����ϴ�.

// volatile�� �������� �ǹ̴� volatile accesses�� �߻����� �ӽ��� ��Ģ����
// ������ ���� �򰡵ȴ�. ��, �����Ϸ��� volatile accesses��
// ���� ��ɾ���� ���ġ�ϰų� �ϳ��� ��ɾ�� �������� ���ϰ� �Ѵ�.
// �׷���,
// volatile int& x = memory_mapped_register_x();
// volatile bool& y = memory_mapped_register_y();
// int stack;
// stack = x; // load
// y = true;  // store
// stack += x;// load
// �� �ΰ��� load ���̿��� x�� �� ���� �����Ѵٰ� �������� �ʴ´�.
// ���� x�� volatile�� �ƴϸ�, �����Ϸ��� ������ ���� ���ġ�Ѵ�.
// stack = 2*x; // load
// y = true;    // store
// �ֳ��ϸ� y�� �ۼ��ϴ� ���� x ���� ������ ���� �ʱ� �����̴�.
// ������ x�� volatile�̸�, ������ �شٰ� �����ϱ� ������ �̷���
// reordering optimization�� ������ �ʴ´�.

// volatile keyword�� ����ϴ� ��κ��� ������
// memory-mapped registers ���� ���������� �ϵ��� �ǵ帱 �ʿ伺 �����̴�.
// �ҽ� �ڵ� ���ؿ��� load �Ǵ� store�� �� ������ hardware operation�� �����ȴ�.
// ��¼�� x�� some hardware buffer view �̰�, y�� �ϵ��� ���� 4����Ʈ��
// �����͸� x�� load�϶�� signal������ �𸥴�. ��ġ �ϵ����� operator
// overloading�� �ϴ� �Ͱ� ����ϴ�.

// �� volatile�� thread-safe���� ������? �ʹ� ������ Ű�����̱� ������
// ��Ƽ�������� ���� ����� �����ϴ�. �׸��� ���� �����Ϸ��� ���ݺ��� �ܼ��ߴ�.
// �׷��� problematic optimizations�� ��Ÿ���� �ʾҾ���.
// 90��� �Ĺ�, 2000��� �ʿ� c++�� a lack of thread-aware memory model�� ����
// ���������� ����ϱ� �����ߴ�. volatile�� thread-safe memory acesss�ϰ�
// ����⿡ �̹� �ʹ� �ʾ���. �̹� ���� �ڵ� ���̽����� volatile�� �����ϱ⿡
// ������ �ɰ��̴�.

// thread-safe memory accesses�� �ʿ��� ���
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
    // ���⼭ thread A,B,C �� ��� ���������� �ٸ� �����忡�� ���ư��ٰ� ����.
    // thread_B ���� x�� �׻� 0�̰ų� 0x42'00000042 �� ���̴�.
    // ������ 32-bit ��ǻ�Ϳ����� �����Ϸ��� x�� assignment�� upper half, lower
    // half�� a pair of assignments�� �����ϹǷ� ��� ������ x�� 0x42'00000000��
    // �� �ִ�. �̸� volatile�� ����ϰ��� �Ѵٸ� 32-bit�� volatile�� ��������
    // �ʴ´�. �׷��� �����Ϸ����� an atomic 64-bit assignment�� �ؾ� �Ѵٰ�
    // �˸��� ����� ����. ���� �Ұ����ϴٸ� ������ ������ �ְ� �Ѵ�.
    // (volatile�� atomic�� ���������� �ʴ´�.)

    // thread_A ���忡���� y�� true�̸� x�� 0x42'00000042�� �Ҵ�Ǿ��� ���̴�.
    // ������ thread_C ���忡�� y�� true������ x�� 0�� ������ ������ �� �ִ�.
    // ���� ��������� ���������� �ٸ� CPU���� ���ư��ٸ�,
    // �׷��� ���� ������ ĳ�� �����͸� ���� �־,
    // x�� y�� values�� ���� �ٸ� �ð��� ������Ʈ �� ���̴�.
    // �̴� ������ ĳ�� ���ε��� ���� ���ŵǴ����� �����Ѵ�.
    // �׷��� A ���忡�� x�� y���� ���� �Ҵ�Ǿ,
    // B ���忡���� y�� x���� ���� �Ҵ�� �� �ִ� ���̴�.
    // �츮�� �����Ϸ��� y�� load�� �� ��ü ĳ�ð� up-to-date�Ǿ�����
    // �����ش޶�� ��û�� ���Ѵ�. ������ some processor architectures�� Ư����
    // instructions, or additional memory-barrier logic�� �ʿ��ϴ�. �����Ϸ���
    // ���� ��Ÿ���� volatile accesses�� ���� �׷��� instructions�� ��������
    // �ʱ� ������ ��Ƽ�������� �ۼ��� �� volatile�� ������� �ʴ� ���̴�.
    // volatile�� �ڽ��� �����忡���� sequential order��� access������,
    // �ٸ� �����忡���� in a different order��� access�ϱ� ������
    // volatile accesses�� sequentially consistent�� �������� �ʴ´�.

    // �̷��� �����鿡 ���� �ذ�å���� C++11���� std::atomic�� �߰��Ǿ���.
}
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
// �Ϲ������� ��κ��� �÷������� �̷��� Ÿ���� ���� std::atomic objects��
// atomic ������ �� ���̴�.
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
// std::atomic<T>�� objects�� ���������� memory�� �����ϰ�
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
// �� ����� ����ȴ�.
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
    // a *= 9; // *= ������ ���ǵ��� �ʾҴ�. �׷��Ƿ� ������ ����.

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

// ���� ������ expected�� reference �����̹Ƿ� ������ ����
// �����ϰ� �ۼ��� �� �ִ�.
void example2()
{
    std::atomic<int> a = 6;
    // a *= 9; *= ������ ���ǵ��� �ʾҴ�.
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
// �����Ϸ��� std::atomic<T>�� ����
// T�� an integral type(including bool)�̰ų�
// T�� ������ Ÿ���̸� ����ȭ�� �ڵ带 �����Ѵ�.
// �׷��ٸ� T�� int[100]�� ���� a bigger type�̸� ��� �ɱ�?
// �̷��� ���� the runtime library�� ��ƾ�� ȣ���Ͽ�
// the assignment under a mutex�� ������ ���̴�.
// �̷��� the assignment�� ����� ���� Ÿ�Ե��� ������ �𸣴� ���̺귯������
// ����ǹǷ� c++17������ ���� trivially copyable types�� ���ؼ��� �۵��Ѵ�.
// �ٽ� ����, memcpy�� ������ �� �ִ� Ÿ�Ե� T�� ���ؼ��� std::atomic<T>��
// ����� �����Ѵ�.
// �׷��� std::atomic<std::string>�� ���Ѵٸ� ���� �ۼ��ؾ� �Ѵ�.

// a trvially copyable big types�� std::atomic�� ����� ���� C++��
// ǥ�� ���̺귯���� �ٸ� ����� c++ runtime routines���� ���ư��Ƿ�
// � �÷��������� -latomic�� ��Ŀ�� ��ɾ �����ؾ� �Ѵ�.
// �׷��� �̷��� ������ ���� big types�� ���ؼ��� �׷���,
// �츮�� �̷��� ������ �� �ʿ䰡 ����.

// ���� ��� atomic string class�� �ۼ��ϴ� �� ���캸��.
}

//-----------------------------------------------------------------------------
// [Taking turns with std::mutex]
//-----------------------------------------------------------------------------
namespace section5
{
// �츮�� (���� �����Ѵٸ�) std::atomic<std::string>ó�� �ൿ�ϴ� a class type��
// �ۼ��ϰ��� �Ѵ�. atomic, thread-safe loads and stores�� �����ؾ� �Ѵ�.
// �׷��� �ΰ��� �����尡 std::string�� ���������� �����ϸ�, �����
// "halfway assigned" state�� ������ �� ���� �ؾ� �Ѵ�.
// �̸� �����ϴ� ���� ���� ����� std::mutex�� ����ϴ� ���̴�.
// mutex�� "mutal exclusion"���� ���� �̸��̴�.

// [�߿�]
// A mutex acts a way to ensure that only one thread is allowed into a
// particular section of code (or set of sections of code) at once.
// �׷��� "thread A is executing this code"��
// "thread B is executiing this code"�� mutually exclusive possible�ϴ�.

// a critical section�� ������ we take a lock on the associated mutex.
// the critical section�� ���� we release the lock �� �����Ѵ�.
// The library�� �ΰ��� �����尡 the same mutex�� the same time�� hold locks��
// ���� �ʵ��� �������.
// ������ A�� �̹� holding lock�ϸ�, ������ B�� A�� the critical section��
// ������ the lock�� release�� ������, �ݵ�� wait�ؾ� �Ѵ�. �׷��� A�� lock��
// ���� ���ȿ� ������ B�� ������ blocked�ȴ�. �̷��� ������ waiting or
// blocking�̶� ���Ѵ�.

// "Taking a lock on a mutex"�� "locking the mutex"��,
// "releasing the lock"�� "unlocking the mutex"�� �����ϰ� ���Ѵ�.

// ���� a mutex�� ���� locked���� .try_lock()�� ���� �� �� �ִ�.
// ���� locking the mutex�ϸ� true�� �����ϰ�,
// ���� already locked by some thread�̸� false�� �����Ѵ�. (��ٸ��� �ʴ´�.)

// a mutex�� ���� a section of code�� ������ ��, the lifetime semantics of the
// mutex object itself�� ����ؾ� �Ѵ�.
// �ش� ���ؽ��� ����ϱ� ���ϴ� ��� a section of code���� visible�ϵ��� a
// single mutex object�� ������� �ξ�� �ұ�?

namespace in_a_function_scoped_static_variable
{
    void log(const char* message)
    {
        static std::mutex m;
        m.lock(); // avoid interleaving messages on stdout
        puts(message);
        m.unlock();
    }
    // ���⼭ static Ű����� ����� �߿��ϴ�.
    // ���� �̸� �����Ѵٸ�, the same mutex object�� �������� �ʴ´�.
    // �Ź� �����帶�� ���еǴ� mutex objects�� locking and unlocking �ϹǷ�
    // the library�� �ƹ� �ϵ� ���� �ʰ� �ȴ�.
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
    // log1, log2 �Լ��� ��ȣ ���������� ������ �� �ִ�.
    // ���� �ð��� ���� �ϳ��� �����常 log1 �Ǵ� log2�� �ڵ带 ������ �� �ִ�.

    // the global variable���ٴ� a class type�� ���� �����ϴ� ���� ����.
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
// C �Ǵ� old-style C++���� raw pointer�� ���� ���װ� �߻��ߴ�.
// ����ϰ� raw mutex�� ���� ��Ƽ������ ���α׷��ֿ��� ������ �߻��Ѵ�.
// 1. Lock leaks: �����ϰ� mutex�� unlock���� �ʴ´�.
// 2. Lock leaks: ����ó���� ���� ���� ���ϵǼ� unlock���� �ʴ´�.
// 3. Use-outside-of-lock: a raw mutex�� guard�ϴ� ������� ���յǾ� ���� �ʱ�
//    ������ �Ǽ��� lock�� ���� �ʰ�, �ش� �����鿡 ������ �� �ִ�.
// 4. Deadlock: thread A�� mutex 1�� lock�ϰ�, thread B�� mutex 2�� lock�ϸ�
//    ���ΰ� ������ mutex�� lock�Ϸ��� �õ��� ��, ��� blocked �Ǿ�
//    ���α׷��� �������� �ʰ� �ȴ�.
// ������ ���� ������ �׷��� ����� ����� ���̽��� �ƴϴ�.
// ���� ��� std::atomic<T>���� livelock ���� ��찡 �׷���. �̷��� ������
// ��Ƽ������ ���� ������ �����Ѵ�.

// c++ ǥ�� ���̺귯���� �̷��� ���׵��� ������ �� �ְ� �����ִ� ��������
// �����Ѵ�. memory management������ ��Ȳ���� �ٸ���, �� ��Ȳ������ ��������
// 100�ۼ�Ʈ �ذ������� ���Ѵ�. �׷��� ��Ƽ�������� ���� ������ ���α׷��ֺ���
// �ξ� ��ƴ�.

// "lock leaks"�� RAII�� �ǽ������� ����ϸ� ������ �� �ִ�.
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
// �̸����� ���ϵ���, std::unique_lock<M>�� "unqiue ownership" RAII class��
// �ǹ��Ѵ�. lock�� unlock ��ſ� unique_lock�� ����ϸ� a mutex lock��
// release �ϴ� ���� ����� �������� �ʰ� �ȴ�.

// std::unique_lock<M>�� �ɹ� �Լ� .lock(), .unlock()�� �����Ѵ�.
// ���� �Ϲ������� �ʿ������� �ʴ�.
// ���� �ڵ����� �߰��� lock�� acquire or release�� �ʿ��� �� �����ϴ�.

// unique_lock�� movable�ϹǷ� null or empty state�� ������ �Ѵ�.
// ��κ��� ��� lock�� �̵���ų �ʿ䰡 ����. �׷��� ����
// � �������� ���ۿ��� lock�ϰ�, �� �������� ������ unlock�ϴ� ����.
// �̶��� �ܼ��� std::lock_guard<M>�� ����Ѵ�.
// std::lock_guard<M>�� not movable�ϰ� .lock(), .unlock()�� ���ǵ��� �ʾҴ�.
}

//-----------------------------------------------------------------------------
// [Always associate a mutex with its controlled data]
//-----------------------------------------------------------------------------
namespace section7
{
// ���� Ŭ�������� ���װ� �����Ѵ�.
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
// A���� ������ �ȴ�. A�� D����
// writing to this->m_count in the producer thread��
// reading from ths->m_count in the consumer thread���� race �����ϰ� �ȴ�.
// ��, A�� writing�ϰ� �ִ� ���߿� D�� reading�Ϸ��� �õ��� �� �ִ�.

// B�� C������ ǥ�������� ���������... ������� �ٽ� �ۼ��ؾ��Ѵ�.

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