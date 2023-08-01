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
// ������ ���� ���Ѵٴ� ���̴�. ��, �����Ϸ��� volatile accesses��
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
// b.store(a.load())�� �� �ٿ� �ۼ��� �� ������, �̷��� �ۼ����� ���ƾ� �Ѵ�.
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

    // a compare-exchange loop
    int expected, desired;
    do {
        expected = a.load();
        desired  = expected * 9;
    } while (!a.compare_exchange_weak(expected, desired));

    // a.compare_exchange_weak��
    // ���� a�� ���� expected�� �����ϴٸ�, �� ���� desired�� �����ϰ�
    // true�� �����Ѵ�.
    // ���� a�� ���� expected�� (�ٸ� �����尡 �����ؼ�) �ٸ���, �� ����
    // desired�� �������� �ʰ� expected�� �����ϰ� false�� �����Ѵ�.
    //
    // ���� false�� �����ϸ� �ٽ� loop �������� ���ư��Ƿ�
    // �ٽ� ���� a�� ���� expected���� load�Ͽ� �� ������ �ݺ��Ѵ�.
    // (�̹� compare_exchange_weak���� expected = a.load()��
    // ȣ��Ǿ��� ������ ���⼭�� �ߺ��ؼ� �ѹ� �� ȣ���ϰ� �ִ�.)

    // ���� ������ ��ġ��,
    // a�� ���� atomically multiplied by 9�� �ȴ�.
}

// �������� a.compare_exchange_weak�� false ���� ���Ŀ�
// expected = a.load()�� �ѹ� �� ȣ���ϰ� �ִ�.
// �̸� �ذ��ϱ� ���� a.compare_exchange_weak�� expected�� ���� �����ϹǷ�
// ������ ���� �����ϰ� �ۼ��� �� �ִ�.
void example2()
{
    std::atomic<int> a = 6;

    // a compare-exchange loop
    int expected = a.load();
    while (!a.compare_exchange_weak(expected, expected * 9))
        ; // desired ���� �ʿ����� �ʴ�.
}

// the atomic variable a�� �ٸ� �����忡 ���� ���� �������� �ʴ´ٸ�
// a compare-exchange loop�� �ϴ� �Ϳ� ���ؼ��� ����.
// �׷��� ����ϰ� �����ȴٸ� the loop�� ������ ������ �ð��� ���� �ɸ��� �ȴ�.
// ���� starvation of the looping thread�� ��Ÿ�� �� �ִ�.
// (��ƻ���: Ư�� ���μ����� �켱 ������ ���Ƽ� ���ϴ� �ڿ��� ����ؼ� ������
// ����. ���⼭�� ����ؼ� the loop�� �� ���̴�.)

// a�� ���� ���� �� compare-exchange�� false�� �����ϰ� ������ �ٽ� ���Ƿ�
// � ������� �� ���� �ڵ带 �����ϰ� �ȴ�.
// Compare-exchange loops�� �� ��ü�� ���α׷��� �ƹ��� ������ ���ϴ� ������
// livelock ���¸� �������� �ʴ´�.

// ������ �Ϲ������� ���� ���� �ൿ�� ������ �ʿ� ����.
// �ֳ��ϸ� �̴� really high contention������ ��Ȳ�̰�
// ������ �̷��� ��Ȳ���� ��� �־��� ������ ������ �ʴ´�.

// ���⼭�� how you can use a compare-exchange loop to implement complicated,
// non-built-in "atomic" operations on atomic<T> objects�� �˾ư��� �ȴ�.
// a.compare_exchange_weak(expected, desired)��
// "if a has the expected value, give it the desired value."�� ����ؾ� �Ѵ�.
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
// mutex�� "mutual exclusion"���� ���� �̸��̴�.

// [�߿�]
// A mutex acts a way to ensure that only one thread is allowed into a
// particular section of code (or set of sections of code) at once.
// �׷��� "thread A is executing this code"��
// "thread B is executiing this code"�� mutually exclusive possible�ϴ�.

// a critical section�� ������ "we take a lock on the associated mutex.",
// the critical section�� ���� "we release the lock" �� �����Ѵ�.
// The library�� �� ���� �̻��� �����尡 the same mutex�� the same time��
// holding locks�� ���� �ʵ��� �������.
// ������ A�� �̹� holding lock�ϸ�, ������ B�� A�� the critical section��
// ������ the lock�� release�� ������, �ݵ�� wait�ؾ� �Ѵ�. �׷��� A�� lock��
// ���� ���ȿ� ������ B�� ������ blocked�ȴ�. �̷��� ������ waiting or
// blocking�̶� ���Ѵ�.

// "Taking a lock on a mutex"�� "locking the mutex"��,
// "releasing the lock"�� "unlocking the mutex"�� �����ϰ� ���Ѵ�.

// .try_lock()��
// ���� a lock�� ������ �� ������ locking the mutex�ϰ� true�� �����Ѵ�.
// ���� already locked by some thread�̸� false�� �����Ѵ�. (��ٸ��� �ʴ´�.)

// �ڹٿ����� ��ü���� ������ mutex�� ���� �־ �ڽ��� synchronized block��
// �����Ѵ�. �ݸ� C++�� a mutex�� �� ��ü�� ��ü Ÿ���̴�.
// �׷��� mutex�� a section of code�� ��Ʈ���ϱ� ���� ����ϱ� ���Ѵٸ�,
// the lifetime semantics of the mutex object itself�� ����ؾ� �Ѵ�.
// ��, �ش� ���ؽ��� ����ϱ� ���ϴ� ��� sections of the code���� visible�ϵ���
// a single mutex object�� ��� ��ġ�� �ξ�� �ϴ°�?

// �ܼ��� one critical section that needs protection�� ���ؼ���
// ����ó�� a mutex�� a function scoped static variable�� �����Ѵ�.
namespace in_a_function_scoped_static_variable
{
    void log(const char* message)
    {
        static std::mutex m;
        m.lock(); // stdout�� �ٸ� �޽������� ���� �����־����� ��Ȳ�� ���Ѵ�.
        puts(message);
        m.unlock();
    }
    // ���⼭ static Ű����� ����� �߿��ϴ�!
    // ���� �̸� �����Ѵٸ�, the same mutex object�� �������� �ʴ´�.
    // �Ź� �����帶�� ���еǴ� mutex objects�� locking and unlocking �ϹǷ�
    // the library�� �ƹ� �ϵ� ���� �ʰ� �ȴ�.
}

// ���� �ٸ� �� �Լ��� ��ȣ ����(mutually exclusive)�ؾ� �ϴ� ���
// ����ó�� a mutex�� a global variable�μ� �� �Լ����� visible�ϰ� �Ѵ�.
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
}

// �׷��� the global variable�� �����ؾ��� ����̱� ������
// ����ó�� a mutex�� ��������� ���� a class type�� ���� �����Ѵ�.
namespace a_member_variable_of_a_class
{
    // ���� Logger ��ü�� ���� ������ ���ٵ��� ���� m_mtx�� ���� lock�� �Ѵ�.
    // ������ ���� �ٸ� Logger ��ü���� �޽������� ���� �����־����� ��Ȳ�� ����
    // �� �ִ�.
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
// ��κ��� ��� lock�� �̵���ų �ʿ䰡 ����. ��κ��� ����
// � �������� ���ۿ��� lock�ϰ�, �� �������� ������ unlock�ϴ� ����.
// �̶��� �ܼ��� std::lock_guard<M>�� ����Ѵ�.
// std::lock_guard<M>�� not movable�ϰ� .lock(), .unlock()�� ���� ������
// �Ҹ��ڴ� ���� �˻���� unlock the mutex�� �Ѵ�.

// unique_lock, lock_guard ��� the kind of mutex being locked�� ���ø�
// �Ű������� �޴´�. (��κ� std::mutex�� ����Ѵ�.)
// c++17�� class template argument deduction ���п� std::unique_lock��
// <std::mutex>�� ���̴� �� ���� ����� �� �ִ�.
// ������ ���ڿ��� mutex��� ������ �ֱ� ���� �ٿ��� �ۼ��ϴ� ���� ��õ�Ѵ�.
// �̴� ������ �����̰� �� �������� �����ؼ� �ۼ��ϴ� ���� ���ƺ��δ�.
// ������ std::lock_guard�� �����̴�.
struct Lockbox {
    std::mutex m_mtx;
    int        m_value = 0;

    void locked_increment()
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_value += 1;
    }

    void locked_decrement()
    {
        std::lock_guard lk(m_mtx); // c++17
        m_value -= 1;
    }
};
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

// B�� C������ ǥ�������� ����غ������� C�� lock�� �� �ʿ䰡 ����.
// ������ D�� lock�� �ؾ��Ѵ�.
// �ֳ��ϸ� B�� C�� ���� the single consumer thread���� ����Ǳ� ������
// ����� ���ÿ� ����� �� ����. (���α׷��� �ּ��� ������ �����ٸ� �׷�����
// ���� �۾������� ���� �Ǽ��� Ʋ���� �ȴ�.)

// locking m_mtx�� m_sum, m_count�� ������ �� �ʿ��ϰ� m_last_average��
// ������ �� �ʿ����� �ʴ�.
// �� Ŭ������ ���������� �� ���� mutexes�� ���� �� �ִ�.
// (���� �̴� the Single Responsibility Principle�� �����ϰ�, �� ����
// components�� refactoring�ϴ°� �̵��̴�.)
// ���ؽ����� �ٷ� �� ���� ���� practice�� �ش� ���ؽ���, �̰��� �����ϰ� �ִ�
// the variables�� ���� ���� Ÿ��Ʈ�� ���踦 ������ ��ġ��Ű�� ���̴�.
//
// �Ѱ��� ����� ���ɽ����� �̸��� ���� ���̴�.
namespace via_careful_naming
{
    class StreamingAverage
    {
        double m_sum          = 0;
        int    m_count        = 0;
        double m_last_average = 0;
        std::mutex m_sum_count_mtx; // �̸����� ������ �����ϴ��� �� �� �ִ�.

        // ...
    };
}
// �� ���� ����� a nested struct�� �����ϴ� ���̴�.
namespace via_a_nested_struct_definition
{
    class StreamingAverage
    {
        struct {
            double     sum   = 0;
            int        count = 0;
            std::mutex mtx;
        } m_guarded_sc;
        double m_last_average = 0;

        // ...
    };
    // �ڵ��� ������ ���α׷��Ӱ� this->m_guarded_sc.sum�� �ۼ��ϰ� �Ͽ�
    // �� ����� this->m_guarded_sc.mtx�� lock�ؾ����� ����Ű�� ���̴�.
    // "anonymous struct members"�� ����Ͽ� m_guarded_sc�� Ÿ�����ϴ� ����
    // ���� �� ������ ���ϴ� �ٰ� �ƴϴ�.
}

// an RAII handle�� �����Ͽ� private members�� ���� ���ؽ��� lock�Ǿ��� ����
// ������ �� �ְ� �Ѵ�.
namespace more_bulletproof_but_somewhat_inflexible
{
    template <class Data>
    class Guarded
    {
        std::mutex m_mtx;
        Data       m_data;

        class Handle
        {
            std::unique_lock<std::mutex> m_lk;
            Data*                        m_ptr;
        public:
            Handle(std::unique_lock<std::mutex> lk, Data* p)
                : m_lk(std::move(lk), m_ptr(p))
            { }
            auto operator->() const { return m_ptr; }
        };
    public:
        Handle lock()
        {
            std::unique_lock lk(m_mtx);
            return Handle {std::move(lk), &m_data};
        }
    };

    class StreamingAverage
    {
        struct Guts {
            double m_sum   = 0;
            int    m_count = 0;
        };
        Guarded<Guts> m_sc;
        double        m_last_average = 0;

        // ...

        double get_current_average()
        {
            auto h         = m_sc.lock();
            m_last_average = h->m_sum / h->m_count;
            return m_last_average;
        }

        // ��� �Լ����� locking m_mtx���� �ʰ�� m_sum�� ������ �� ����.
        // �׸��� m_sum�� �����Ϸ��� ���� the RAII Handle type�� ���ؼ���
        // �����ϴ�.
    };
}

// ��������� ���� �ܼ��� "forget to lock the mutex" cases�� �ٷ����.
// ���� �ذ����� ���� �پ��� lock�� ���õ� ���׵��� �����Ѵ�.
// ���� get_current_average�� ����ó�� �ۼ��غ���.
namespace rewriting_get_current_average
{
    class StreamingAverage
    {
        template <class Data>
        using Guarded = more_bulletproof_but_somewhat_inflexible::Guarded<Data>;
        struct Guts {
            double m_sum   = 0;
            int    m_count = 0;
        };
        Guarded<Guts> m_sc;
        double        m_last_average = 0;

        // ...

        double get_sum() { return m_sc.lock()->m_sum; }

        int get_count() { return m_sc.lock()->m_count; }

        double get_current_average()
        {
            // a gap between the read of m_sum and the read of m_count��
            // �����. �׸��� ���� �� ���׸� ��ġ�� ���� �Ʒ��� �ּ��� �߰��ϸ�
            // auto h = m_sc.lock();
            // ���� ���ؽ��� �� ��° lock�� �� deadlock ���°� �ȴ�.
            // (���ؽ��� lock�� �� �� �̹� locked�Ǿ� ������ block�ϰ�
            /// �� ���ؽ��� unlock�ϱ⸦ ��ٸ���.)
            return get_sum() / get_count();
        }
    };
    // ����� �̹� �����ִ� lock�� �ѹ� �� lock�ϸ� �ȵȴ�.
    // �ڵ��� �������� �̷��� ����� ���� �� ���ٸ�,
    // recursive_mutex�� ����Ѵ�.
}
}