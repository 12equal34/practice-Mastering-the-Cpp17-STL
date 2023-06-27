#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <memory>
#include <cassert>

//-----------------------------------------------------------------------------
// [The origins of smart pointers]
//-----------------------------------------------------------------------------
namespace section1
{
// C ���� raw pointers�� ������ ���� ����Ѵ�.
// 1. As a cheap, non-copying view of an object owned by the caller
// 2. As a way for the callee to modify an object owned by the caller
// 3. As one-half of a pointer/length pair, used for arrays
// 4. As an optional argument (either a valid pointer or null)
// 5. As a way to manage memory on the heap

// C++������,
// native references�� �����ϱ� ������ 1,2�� ��ü�ȴ�.
// std::string_view�� �����ϱ� ������ 1,3�� ��ü�ȴ�.
// std::optional<T>�� �����ϱ� ������ 4�� ��ü�ȴ�.
// ���� ���� ���� 5�ε�, �̹� �忡�� 5�� ���� �ٷ��.
//
// �� ������ C���� Heap allocation���� ������ ���� ���� �������� �ִ�.
// Memory leaks, Use-after-free,
// Heap corruption via pointer arithmetic: ���� ���� �Ҵ��� �迭�� �ּҰ� A�϶�
// A+k �ּҸ� free�ϸ� �� �ȴ�.
// �� �Ҵ翡�� malloc�� null�� ������ �� �ְ�, operator new�� std::bad_alloc��
// ������ �� �ִ�.
}

//-----------------------------------------------------------------------------
// [Smart pointers never forget]
//-----------------------------------------------------------------------------
namespace section2
{
// "smart pointer"�� ���̵��� ������ ����.
// a pointeró�� ����ϴ� Ŭ�����̴�.
// Ŭ�����̹Ƿ� ������ �����Ѵ�.
// The pointer's destructor also frees its pointee : �޸� ������ ���´�.
// Maybe the pointer cannot be copied : use-after-free�� ���´�.
// Or maybe the pointer can be copied, but it knows how many copies exist and
// won't free the pointee until the last pointer to it has been destroyed
// Or maybe the pointer can be copied, and you can free the pointee, but if
// you do, all other pointers to it magically become null
// Or maybe you're allowed to adjust the pointer's value arithmetically, but
// the arithmetic "what object is pointed-to" is managed separately from the
// identity of "what object is to be freed"

// �̷��� ����Ʈ ������ Ÿ�Ե��� std::unique_ptr<T>, std::shared_ptr<T>, and
// (not really a pointer type) std::weak_ptr<T> �� �ִ�.
}

//-----------------------------------------------------------------------------
// [Automatically managing memory with std::unique_ptr<T>]
//-----------------------------------------------------------------------------
namespace section3
{
// smart pointer�� �ݵ�� operator* �� �����ؾ� �Ѵ�.
// �׷��� �̷��� class invariants�� �����ϴ� the special member functions��
// �ݵ�� �����ε��ؾ� �Ѵ�.
// std::unique_ptr<T> �� operator*�� T*�� �����ϸ� ������ �����ؾ� �Ѵ�.
// class invariant: unique_ptr�� �����Ͽ� ���� �Ҵ�� ��ü�� ����Ű��
// unique_ptr�� �Ҹ��ڰ� ȣ��� �� �� ��ü�� free�� �ȴ�.

template <typename T>
class unique_ptr
{
    T* m_ptr = nullptr;
public:
    constexpr unique_ptr() noexcept = default;
    constexpr unique_ptr(T* p) noexcept
        : m_ptr(p)
    { }

    T* get() const noexcept { return m_ptr; }
       operator bool() const noexcept { return bool(get()); }
    T& operator*() const noexcept { return *get(); }
    T* operator->() const noexcept { return get(); }

    // ���� 2���� �ɹ� �Լ��� primitive operations�� �ȴ�.
    void reset(T* p = nullptr) noexcept
    {
        T* old_p = std::exchange(m_ptr, p);
        delete old_p;
    }
    // the original pointer�� free���� �ʰ� ȣ���ڿ��� �������� �ֵ���
    // the pointer�� �������� �����Ѵ�.
    T* release() noexcept { return std::exchange(m_ptr, nullptr); }

    unique_ptr(unique_ptr&& rhs) noexcept { this->reset(rhs.release()); }
    unique_ptr& operator=(unique_ptr&& rhs) noexcept
    {
        reset(rhs.release());
        return *this;
    }
    ~unique_ptr() { reset(); }
};

// a helper function�� ���� ����� raw pointers ���� �ǵ帮�� �ʴ´�.
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// old-style code�� ����ó�� �ۼ�������
struct Widget {
    virtual ~Widget() { }
};
struct WidgetImpl : Widget {
    WidgetImpl(int size) { }
};
struct WidgetHolder {
    void take_ownership_of(Widget*) noexcept { }
};
void test_old()
{
    Widget*       w = new WidgetImpl(30);
    WidgetHolder* wh;
    try {
        wh = new WidgetHolder();
    } catch (...) {
        delete w;
        throw;
    }
    wh->take_ownership_of(w);
    try {
        // use(*wh);
    } catch (...) {
        delete wh;
        throw;
    }
    delete wh;
}
// C++17������ �������� ��ü�� �� �ִ�.
void test()
{
    auto w  = std::make_unique<WidgetImpl>(30);
    auto wh = std::make_unique<WidgetHolder>();
    wh->take_ownership_of(w.release());
    // use(*wh);
}

// ����ó�� make_unique�� ������� �ʰ� unique_ptr�� ����� �� ������
// �̷��� �ۼ��ϸ� �ȵȴ�.
void bad_use()
{
    std::unique_ptr<Widget> bad(new WidgetImpl(30));
    bad.reset(new WidgetImpl(40));

    // �ݵ�� std::make_unique<T>()�� ���ؼ� �ʱ�ȭ�ؾ��Ѵ�.
    std::unique_ptr<Widget> good = std::make_unique<WidgetImpl>(30);
    good                         = std::make_unique<WidgetImpl>(40);
}

// The only way you'll get the full benefit of unique_ptr is
// if you make sure that whenever you allocate a resource, you also
// initialize a unique_ptr to manage it.
// �ڿ��� �Ҵ��ϸ� �ݵ�� ���ÿ� unique_ptr�� �ʱ�ȭ�Ͽ� �̸� �����ϰ� �ؾ��Ѵ�.
// �̸� std::make_unqiue<T>() helper �Լ��� �����ش�.
}

//-----------------------------------------------------------------------------
// [Why C++ doesn't have the finally keyword]
//-----------------------------------------------------------------------------
namespace section4
{
// ������ ���� �ڵ�� �׻� delete wh;�� �Է��ؾ� �ϹǷ�
//  try {
//      use(*wh);
//  } catch (...) {
//      delete wh;
//      throw;
//  }
//  delete wh;
//
// �ٸ� ���� �����ϴ� finally keyword�� �����ϰ� �ȴ�.
//  try {
//      use(*wh);
//  } finally {
//      delete wh;
//  }
// ������ c++���� ����. �̴� c++�� ö���� ������ ���� �����̴�.
//
// ���࿡ ��� invariant�� �����ϰ� �ʹٸ�, ����� explicit code��
// �ۼ��� �� ����. �ֳ��ϸ� ����� �ڵ带 �߸� �ۼ��Ͽ� ���׸� ����� ��ȸ��
// �׻� �ֱ� �����̴�.
//
// ���࿡ ��� invariant�� �����ϰ� �ʹٸ�, �̸� ������ �� �ִ� �ùٸ� ��Ҵ�
// the type system ���̴�. �׷��� ����� ���ο� Ÿ�Կ� ���� ��� ����� �ϵ�
// �׻� invariant�� �����ϵ��� ������ �� �ִ�.
//
// ���� ���� ���� ��Ȳ���� finally���� Ű���带 ����� �����ϴ� ���� �ƴ϶�..
// "�̸� ���� unique_ptr�� ����ؾ� �ұ�?" �ƴϸ�
// "�̸� ���� an RAII class type�� �ۼ��ؾ� �ұ�?"�� ����ؾ� �ϴ� ���̴�.
}

//-----------------------------------------------------------------------------
// [Customizing the deletion callback]
//-----------------------------------------------------------------------------
namespace section5
{
// unique_ptr�� ����� ���� deletion callback�� ������ �� �ִ�.
// std::unique_ptr<T,D>�� D�� a deletion callback type�̴�.
// ����Ʈ�� std::default_delete<T>�̸� operator delete�� ȣ���Ѵ�.
struct fcloser {
    void operator()(FILE* fp) const { fclose(fp); }

    // std::make_unique<T,D> ������ �����Ƿ� ���� raw pointer�� �ٷ�� ����
    // ���ϱ� ���� a reusable helper function�� �������.
    static auto open(const char* name, const char* mode)
    {
        return std::unique_ptr<FILE, fcloser>(fopen(name, mode));
    }
};

void use(FILE*) { }

void test()
{
    auto f = fcloser::open("text.txt", "r");
    use(f.get());
    // ���� use�� throw�Ѵٸ� f�� close �ȴ�.
}
// std::unique_ptr�� �Ҹ��ڴ� a null pointer�� ���� the deletion callback��
// ����� ȣ������ �ʱ� ������, fclose(NULL)�� ȣ��Ǵ� ������ �� �ʿ� ����.
// fclose(NULL)�� "close all open file handles in the current process"�Ѵ�.

// ���� ������ custom deleter�� std::unique_ptr<T,D> ��ü �ȿ��� �Ҵ��� �� �ֱ�
// ������ D�� �ɹ� �����͸� ���´ٸ�
// sizeof(unique_ptr<T,D>) > sizeof(unique_ptr<T>) �� ������ ���̴�.
}

//-----------------------------------------------------------------------------
// [Managing arrays with std::unique_ptr<T[]>]
//-----------------------------------------------------------------------------
namespace section6
{
// ���� a pointer p�� �迭�� ù��° ���Ҹ� ����Ų�ٸ�,
// delete p�� �ϸ� �ȵȴ�. �ݵ�� delete[] p�� �ؾ��Ѵ�.
// C++14 ���� std::unique_ptr<T[]>�� �ùٸ� ������� std::default_delete<T[]>��
// operator delete[]�� �ùٸ��� ȣ���Ѵ�.

// �迭 Ÿ�Կ� ���� std::make_unique�� �����ε尡 ���������� �����ؼ� ����ؾ�
// �Ѵ�. argument�� ���� ������ �ٸ� �ǹ̷� ����ϱ� �����̴�. ��,
// std::make_unique<T[]>(n)�� new T[n]()�� ȣ���Ѵ�. ������ �̴� ��� ���ҵ���
// ���ʱ�ȭ�ϹǷ� �̸� ������ �ʴ´ٸ�, helper function�� ���� ���� new��
// ȣ���Ͽ� ���� ���� std::unique_ptr<T[]>���� wrap �ؾ� �Ѵ�.
}

//-----------------------------------------------------------------------------
// [Reference counting with std::shared_ptr<T>]
//-----------------------------------------------------------------------------
namespace section7
{
// unclear ownership�� ���� use-after-free bugs�� �Ͼ��.
// ��� �޸� �ڿ��� ���� shared ownership�� �ִٸ�,
// �ش� �ڿ��� ���� ����� ���δٸ� �ð�, ���δٸ� �����ͱ���, ���δٸ�
// �����忡�� �����Ͽ� �ڿ��� �������� ������ ���ɼ��� �ִ�. �׷���
// use-after-free bugs�� �Ͼ��.
// ���� ���� ���� �ڿ��� �������� �����ϴ� ���� �ʿ��ϴ�.
// �̸� ���� std::shared_ptr<T>�� �����ȴ�.
// reference counting���� �˷��� ����� ���� �޸𸮸� �����Ѵ�.
//
// std::shared_ptr�� �� �޸� �ȿ��� ���۷����� ī��Ʈ�ϰ� ���� ���۷�������
// ����� ���� �����ϴ� "control block"�� ���� �ִ�. �̴� library���� ������
// �ʰ� �ٷ������.
//
// std::shared_ptr<Super> p = std::make_shared<Super>();
// p �ȿ� {ptr, ctrl}�� �����Ѵ�.
// ptr�� a Super object�� ����Ű��, ctrl�� control_block_impt<Deleter>��
// ����Ų��. �� �ȿ� {vptr, use=1, weak=0, ptr, Deleter}�� �����Ѵ�.
//
// unique_ptr�� ����ϰ� std::make_shared<T>(args)�� ���������� raw pointer��
// �ǵ帮�� �ʰ� ��ü�� ���� �Ҵ��Ѵ�.
// std::make_shared<T>�� control block�� T object�� ��� �Ҵ��Ѵ�.
// shared_ptr�� ������ control block�� use-count�� increment�ϰ�
// shared_ptr�� �Ҹ��� control block�� use-count�� decrement�Ѵ�.
// shared_ptr�� ���� �����ϴ� ���� old value�� use-count�� decrement�ϰ�,
// new value�� use-count�� increment�Ѵ�.
// ���� ���ø� ����.
void example()
{
    class X
    {
    };
    std::shared_ptr<X> pa, pb, pc;

    pa = std::make_shared<X>();
    assert(pa.use_count() == 1);
    pb = pa; // use=2

    pc = std::move(pa);
    assert(pa == nullptr);
    assert(pc.use_count() == 2);

    pb = nullptr;
    assert(pc.use_count() == 1);
}

// ������ ���� 2���� instances of shared_ptr�� the same control block��
// ���������� the control block�� �����ϴ� �޸��� ���� �ٸ� �κ��� ����ų ��
// �ִ�.
// std::shared_ptr<Super> p = std::make_shared<Super>();
// std::shared_ptr<int> q(&p->second, p);
// ���⼭ p,q�� ctrl�� ��� ���� control_block_impl<Deleter>�� ����Ű����
// ������ ptr�� a Super object�� ���� �ٸ� �޸𸮸� ����Ų��.

struct Super {
    int first, second;
    Super(int a, int b)
        : first(a),
          second(b)
    { }
    ~Super() { puts("destroying Super"); }
};

// �̷��� �Լ��� "aliasing constructor" of shared_ptr�̶� �θ���.
auto get_second()
{
    auto p = std::make_shared<Super>(4, 2);
    return std::shared_ptr<int>(p, &p->second);
}

void test()
{
    std::shared_ptr<int> q = get_second();
    puts("accessing Super::second");
    assert(*q == 2);
}
}

//-----------------------------------------------------------------------------
// [Don't double-manage!]
//-----------------------------------------------------------------------------
namespace section8
{
// shared_ptr<T>�� double-free bugs�� ����������
// raw pointer arguments�� �޴� shared_ptr�� �����ڸ� ����ϸ� �������� ���Ѵ�.
void example()
{
    class X
    {
    };
    std::shared_ptr<X> pa, pb, pc;

    pa = std::make_shared<X>();
    pb = pa;

    pc = std::shared_ptr<X>(pb.get()); // WRONG!
    // �̷��� ����� ���Ѵٸ� pc = std::shared_ptr<X>(pb,pb.get()) �� ����ؾ�
    // �Ѵ�. ������ �̴� �ܼ��� pc = pb�� �����ϴ�.
    assert(pb.use_count() == 2);
    assert(pc.use_count() == 1);

    pc = nullptr;
    // pc�� use=0�� �ǹǷ� ����Ű�� X��ü�� ���� delete�� ȣ���Ѵ�.
    assert(pb.use_count() == 2);
    // *pb�� �ڿ� ������ ��ü�� �����ϹǷ� undefined behavior�� �߻��Ѵ�.

    // ���� pb�� �Ҹ��Ҷ� double-free bugs�� �Ͼ��.
}
// shared_ptr�� explicitly�ϰ� ����ϸ� �߸� �ۼ��� Ȯ���� ũ�Ƿ� �����ؾ� �Ѵ�.
// ��, �ڵ忡�� shared_ptr�� ���� ������� ���� �׻� std::make_shared�� ����ؾ�
// �Ѵ�. �׸��� shared_ptr Ÿ���� ������ ������ �� �׻� auto�� ����ؾ� �Ѵ�.
// �ٸ�, ���� type�� �˷���� �ϴ� Ŭ���� �ɹ� Ÿ�Կ� ���ؼ��� shared_ptr<T>��
// �����Ѵ�.
}

//-----------------------------------------------------------------------------
// [Holding nullable handles with weak_ptr]
//-----------------------------------------------------------------------------
namespace section9
{
// ���� ������ the control block�� weak count�� ���ؼ� �������� �ʾҴ�.
// ���⼭ weak_ptr�� ���ؼ� �����Ѵ�.

// shared_ptr�� �����ϴ� �����ϴ� ��ü�� ���ؼ� �������� ���� �ʰ� �� ��ü��
// ���� �����͸� �ٷ�� ���� ��찡 �ִ�.
// �׷��� ����ó�� ������ �غ���.
struct DangerousWatcher {
    int* m_ptr = nullptr;

    void watch(const std::shared_ptr<int>& p) { m_ptr = p.get(); }
    int  current_value() const
    {
        // ���⼭ *m_ptr�� �̹� �ڿ� �����Ǿ��ٸ� �����ϴ�.
        return *m_ptr;
    }
};

struct NotReallyAWatcher {
    std::shared_ptr<int> m_ptr;

    void watch(const std::shared_ptr<int>& p) { m_ptr = p; }
    int  current_value() const
    {
        // ���⼭ *m_ptr�� ����� �ڿ� �������� �ʱ� ������ �����ϴ�.
        // ������ �̴� �������� �����ϹǷ� ���ϴ� �ٰ� �ƴϴ�.
        return *m_ptr;
    }
};

// shared_ptr�� �޸� ���� �ý��ۿ� ���� ���� ������ the control
// block���κ��� ��ü�� ������ �� �ִ��� ��� �� �ִ� a non-owning reference
// �� ����� �� �ִ�. ������ �̷��� a non-owning reference�� ��ü�� �����ϰ���
// �ϴ� ������ �ٸ� �����忡�� �� ��ü�� deallocate�� ���� �ֱ� ������ �����ؼ�
// �ٸ� ����� ����ؾ� �Ѵ�.
//
// ���� �츮�� �ʿ��� the primitive operation�� ������ ����.
// "atomically get an owning reference (a shared_ptr) to the referenced object
// if it exists, or otherwise indicate failure."
//
// �׷��� �츮�� a non-owning reference�� �ƴ϶�...
// We need a ticket that we can exchange at some future date for an owning
// reference. �̰��� �ٷ� std::weak_ptr<T>�̴�.
struct CorrectWatcher {
    std::weak_ptr<int> m_ptr;

    void watch(const std::shared_ptr<int>& p) { m_ptr = std::weak_ptr<int>(p); }
    int  current_value() const
    {
        // �����ϰ� *m_ptr�� �Ҵ�Ǿ� �ִ� �� Ȯ���� �� �ִ�.
        if (auto p = m_ptr.lock()) {
            return *p;
        } else {
            throw "It has no value; it's been deallocated!";
        }
    }
};
// weak_ptr�� ���� ���� two operations�� �˸� �ȴ�.
// 1. a shared_ptr<T>�κ��� a weak_ptr<T>�� ������ �� �ִ�.
// 2. a weak_ptr<T>�κ��� wptr.lock()�� ȣ���Ͽ�
//                       a shared_ptr<T>�� ������ �õ��� �� �ִ�.
// wptr.lock()�� ���� ���� weak_ptr�� ����Ǿ��ٸ� (�ڿ��� �����Ǿ��ٸ�), a null
// shared_ptr�� �����Ѵ�.
// wptr.expired()�� ���� Ȯ���� �� ������ ���� ���� false�� ���������� ��
// ����ũ���� �Ŀ� true�� ������ �� �־ useless �ϴ�.

// weak_ptr�� ������ the control block�� weak-count�� increment�Ѵ�.
// weak_ptr�� �Ҹ��� the control block�� weak-count�� decrement�Ѵ�.
// ���� use-count�� 0�� �Ǿ��� �� weak-count�� 0�� �ƴ϶��,
// the control block�� �Ҹ���� �ʴ´�.
// ���̻� weak_ptr objects�� ��ü�� ����Ű�� �ʾ��� ��, (weak-count�� 0�� �Ǿ�)
// the control block�� deallocate �ȴ�.
//
// ���� use-count�� 0�� �Ǿ��� �� weak-count�� 0�� �ƴ� ���,
// the control_block_impl<Deleter>�� ptr�� ���̻� �ش� ��ü�� ����Ű�� �ʾƼ�
// weak_ptr�� ptr�� ����� �� ���� �ȴ�.
}

//-----------------------------------------------------------------------------
// [Talking about oneself with std::enable_shared_from_this]
//-----------------------------------------------------------------------------
namespace section10
{
// shared_ptr�� double-managing a pointer by creating multiple control blocks
// �� ���� �����ϴٰ� ����ߴ�.
//
// A::foo()�� �ܺ� �Լ� bar()�� �����Ѵٰ� ����.
// bar()�� ��ü A�� pointer�� �ʿ��ϴٰ� ����.
// ���� lifetime management�� ������ �ʿ䰡 ���ٸ�, �ܼ��� bar(this)�� �ϸ�
// �ȴ�.
//
// A�� shared_ptr�� ���� �޸� �����ǰ� �ִٰ� ����.
// bar()�� �ݹ��� ���� this�� ���纻�� ���� ��򰡿� �����Ѵٰ� ����.
// ���� A::foo()�� �������� ���� ���ÿ� �����ϴ� ���ο� �����带 �������� ��,
// bar()�� �����ϴ� ���߿��� A�� ��� �־�� �Ѵ�.
//
// �׷��� bar()�� std::shared_ptr<A>�� �Ű������� ������ �Ѵ�.
// A::foo()�� �ɹ� ���� std::shared_ptr<A>�� �����ٸ� �̸� �������� ��
// ������ A�� �ڱ��ڽ��� ���� �־ ����� �Ҹ���� �ʰ� �ȴ�.
//
// ��ſ� �ɹ� ������ std::weak_ptr<A>�� �����ٸ� bar(this->m_wptr.lock())��
// ȣ���ϸ� �ذ�ȴ�. ������ �̴� ������ syntatic overhead�� �ְ� m_wptr��
// �ʱ�ȭ�Ǿ� �ִ��� ������� �ʱ� ������ c++������ ������ ����
// std::enable_shared_from_this�� �����ߴ�.

// enable_shared_from_this�� ��ü�� �� �κ��� location in memory�� ����ϱ�
// ������ �Ʒ��� ���� �ۼ��Ѵ�.
template <class T>
class enable_shared_from_this
{
    std::weak_ptr<T> m_weak;
public:
    enable_shared_from_this(const enable_shared_from_this&) { }
    enable_shared_from_this& operator=(const enable_shared_from_this&) { }
    std::shared_ptr<T>       shared_from_this() const
    {
        return std::shared_ptr<T>(m_weak);
    }
};
// shared_ptr�� �����ڴ� T�� enable_shared_from_this<T>�κ���
// publicly inherit �ߴ����� �Ǵ��Ͽ� m_weak�� �����Ѵ�.
// �׷��� �ݵ�� public and unambiguous �ϰ� ����ؾ� �Ѵ�.

struct Widget : std::enable_shared_from_this<Widget> {
    template <class F>
    void call_on_me(const F& f)
    {
        f(this->shared_from_this());
    }
};

void test()
{
    auto sa = std::make_shared<Widget>();

    assert(sa.use_count() == 1);
    sa->call_on_me([](auto sb) { assert(sb.use_count() == 2); });

    Widget w;
    try {
        w.call_on_me([](auto) {});
    } catch (const std::bad_weak_ptr&) {
        puts("Caught!");
    }
}
}

//-----------------------------------------------------------------------------
// [The Curiously Recurring Template Pattern]
//-----------------------------------------------------------------------------
namespace section11
{
// "X inherits from A<X>"�� the Curiously Recurring Template Pattern, or
// CRTP��� �θ���.
template <class Derived>
class addable
{
public:
    auto operator+(const Derived& rhs) const
    {
        Derived lhs = static_cast<const Derived&>(*this);
        lhs         += rhs;
        return lhs;
    }
};
// boost::addable�� ����ϰ� ������ ���� �����Ѵ�.
// operator+�� a friend free function ���� �д�. �̷��� ����� "Barton-Nackman
// trick"�̶� �θ���.
template <class Derived>
class addable2
{
public:
    friend auto operator+(Derived lhs, const Derived& rhs)
    {
        lhs += rhs;
        return lhs;
    }
};
// ��� derived-class behavior�� a method of base class�� inject��Ű�� ������
// CRTP ������ ����Ѵ�.
}

//-----------------------------------------------------------------------------
// [A final warning]
//-----------------------------------------------------------------------------
// shared_ptr, weak_ptr, and enable_shared_from_this�� �ý�����
// ������ �÷��͸�ŭ�� �������� �ְ�, c++�� Ư¡�� ���������� �Ҹ�� �ӵ���
// �����Ѵ�.
// shared_ptr�� �����ϴ� �Ϳ� �����ؾ� �Ѵ�. ��κ��� �ڵ忡�� shared_ptr��
// ����Ͽ� �� ��ü�� ���� �������� �����ϸ� �ȵȴ�.
// ù��°�� �׻� �� �Ҵ��� (�� �ǹ̷��� ����Ͽ�) ������ ���ؾ� �Ѵ�.
// �ι�°�� �� �Ҵ� ��ü�� a unique owner�� ������ �ؾ� �Ѵ�.
// �� ������ �Ұ����� ��, shared ownership�� std::shared_ptr<T>�� ����Ѵ�.

//-----------------------------------------------------------------------------
// [Denoting un-special-ness with observer_ptr<T>]
//-----------------------------------------------------------------------------
namespace section12
{
class Widget;
// ������ ���� a function signature�� ��������.
void remusnoc(std::unique_ptr<Widget> p);
// �� �ٷ�� �ִ� ��ü�� �������� remusnoc�� �ѱ�� ���� �ǹ��Ѵ�.
// �� �Լ��� ȣ���� ��, �츮�� Widget ��ü�� ���� unique ownership�� ���� �־��
// �Ѵ�. �׸��� �Լ��� ȣ���� ��, �츮�� ���̻� �� ��ü�� ���� ������ �� ����.
// �츮�� remusnoc�� Widget�� �Ҹ��� ��, ������ ��, �Ǵ� �ٸ� ��ü�� �����忡
// ���� ���� �𸥴�. �� �̻� �츮�� �Ű����� ����� �ƴϰ� �ȴ�.
//
// ���� �ݴ��,
std::unique_ptr<Widget> recudorp();
// �� ȣ���� ��, ���Ϲ޴� Widget ��ü�� ��� ���̵� �� ��ü�� ���� unique
// ownership �� �츮�� ���� �ȴ�. �̴� �ٸ� Widget ��ü�� ���۷����� �ƴϴ�.
// ���� ��� static data�� �����͵� �ƴϴ�. ��������� �� �Ҵ�� Widget
// ��ü�̰� ������ �������� ȣ���ϴ� �츮�� ���� �ȴ�.
//
// ������ ������ ���� c++ function�� �ǹ̴� ��� �ɱ�?
void suougibma(Widget* p);
// �� �Լ��� �Ѱܹ޴� �������� �������� ���� ���� �ְ� �ƴ� ���� �־
// ��ȣ�ϴ�. suougibma�� ������ ã�� ���ƾ� �� ���� �ְ� ��ü �ڵ忡 ����
// stylistic conventions ("�ο� �����ʹ� ����� �������� ��Ÿ����
// �ʴ´�.") ���κ��� �ǹ̸� �ľ��� �� �ִ�. ������ �̴� �Լ� �ñ״�ó ��ü��
// �������� �ʴ´�.
//
// unique_ptr<T>�� ownership transfer�� ǥ���ϴ� a vocabulary type�̴�.
// �ݸ鿡 *T�� ���� vocabulary�� �ƴϴ�.
// ���� non-owning pointers�� �ѱ�� �ڵ尡 ���ٸ�,
// (���� �����Ͱ� �ƴ϶� ���۷����� �ѱ� �� �ִٸ� �ݵ�� ���۷����� �Ѱܾ� �ϰ�
// �׷� �� ���� �� non-owning pointer�� �ѱ�� ��쿡 �ش��Ѵ�.)
// �̷��� non-owning pointer�� ǥ���ϴ� a vocabulary type�� ������ �� �ִ�.
template <typename T>
class observer_ptr
{
    T* m_ptr = nullptr;
public:
    constexpr observer_ptr() noexcept = default;
    constexpr observer_ptr(T* p) noexcept
        : m_ptr(p)
    { }
    T* get() const noexcept { return m_ptr; }
       operator bool() const noexcept { return bool(get()); }
    T& opeartor*() const noexcept { return *get(); }
    T* operator->() const noexcept { return get(); }
};
void revresbo(observer_ptr<Widget> p);
// observer_ptr�� �����͸� ������ �� ���� �����Ͱ� ����Ű�� ��ü�� ����
// lifetime�� ���� ������ ��ġ�� �ʴ´�. ���� the lifetime�� ������ �ְ� �ʹٸ�
// �������� �޴� unique_ptr �̳� shared_ptr�� ����ؾ� �Ѵ�.
//
// ������ c++ standard library���� �̷��� a vocabulary type�� ����.
// �ֵ� ������ ���� �����ڵ��� ������ ���� �����ϱ� �����̴�.
// �ݵ�� *T�� the vocabulary type for "non-owning pointer"�� �ؼ��ؾ�
// �Ѵ�. �������� �����ϱ� ���� *T�� ����ϴ� ������ �ڵ�� �ݵ�� ���ۼ��ǰų�
// �ּ��� owner<T*>ó�� re-annotated �ؾ��Ѵ�.
// 
// �ٽ��ѹ� �������ڸ�,
// Never use raw pointers for ownership transfer!
}
