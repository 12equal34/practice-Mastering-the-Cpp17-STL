#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <memory>
#include <cassert>

//-----------------------------------------------------------------------------
// [The origins of smart pointers]
//-----------------------------------------------------------------------------
namespace section1
{
// C 에서 raw pointers는 다음과 같이 사용한다.
// 1. As a cheap, non-copying view of an object owned by the caller
// 2. As a way for the callee to modify an object owned by the caller
// 3. As one-half of a pointer/length pair, used for arrays
// 4. As an optional argument (either a valid pointer or null)
// 5. As a way to manage memory on the heap

// C++에서는,
// native references가 존재하기 때문에 1,2는 대체된다.
// std::string_view가 존재하기 때문에 1,3는 대체된다.
// std::optional<T>가 존재하기 때문에 4는 대체된다.
// 이제 남은 것은 5인데, 이번 장에서 5에 대해 다룬다.
//
// 그 이전에 C에서 Heap allocation에는 다음과 같은 여러 문제점이 있다.
// Memory leaks, Use-after-free,
// Heap corruption via pointer arithmetic: 만약 힙에 할당한 배열의 주소가 A일때
// A+k 주소를 free하면 안 된다.
// 힙 할당에서 malloc이 null을 리턴할 수 있고, operator new가 std::bad_alloc을
// 리턴할 수 있다.
}

//-----------------------------------------------------------------------------
// [Smart pointers never forget]
//-----------------------------------------------------------------------------
namespace section2
{
// "smart pointer"의 아이디어는 다음과 같다.
// a pointer처럼 사용하는 클래스이다.
// 클래스이므로 다음을 보장한다.
// The pointer's destructor also frees its pointee : 메모리 누수를 돕는다.
// Maybe the pointer cannot be copied : use-after-free를 막는다.
// Or maybe the pointer can be copied, but it knows how many copies exist and
// won't free the pointee until the last pointer to it has been destroyed
// Or maybe the pointer can be copied, and you can free the pointee, but if
// you do, all other pointers to it magically become null
// Or maybe you're allowed to adjust the pointer's value arithmetically, but
// the arithmetic "what object is pointed-to" is managed separately from the
// identity of "what object is to be freed"

// 이러한 스마트 포인터 타입들은 std::unique_ptr<T>, std::shared_ptr<T>, and
// (not really a pointer type) std::weak_ptr<T> 가 있다.
}

//-----------------------------------------------------------------------------
// [Automatically managing memory with std::unique_ptr<T>]
//-----------------------------------------------------------------------------
namespace section3
{
// smart pointer는 반드시 operator* 를 제공해야 한다.
// 그래서 이러한 class invariants를 보존하는 the special member functions을
// 반드시 오버로드해야 한다.
// std::unique_ptr<T> 는 operator*가 T*와 동일하며 다음이 성립해야 한다.
// class invariant: unique_ptr을 생성하여 힙에 할당된 객체를 가리키고
// unique_ptr의 소멸자가 호출될 때 그 객체는 free가 된다.

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

    // 다음 2개의 맴버 함수가 primitive operations이 된다.
    void reset(T* p = nullptr) noexcept
    {
        T* old_p = std::exchange(m_ptr, p);
        delete old_p;
    }
    // the original pointer를 free하지 않고 호출자에게 소유권을 주도록
    // the pointer의 복제본을 리턴한다.
    T* release() noexcept { return std::exchange(m_ptr, nullptr); }

    unique_ptr(unique_ptr&& rhs) noexcept { this->reset(rhs.release()); }
    unique_ptr& operator=(unique_ptr&& rhs) noexcept
    {
        reset(rhs.release());
        return *this;
    }
    ~unique_ptr() { reset(); }
};

// a helper function을 통해 절대로 raw pointers 직접 건드리지 않는다.
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// old-style code는 다음처럼 작성하지만
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
// C++17에서는 다음으로 대체할 수 있다.
void test()
{
    auto w  = std::make_unique<WidgetImpl>(30);
    auto wh = std::make_unique<WidgetHolder>();
    wh->take_ownership_of(w.release());
    // use(*wh);
}

// 다음처럼 make_unique를 사용하지 않고 unique_ptr을 사용할 수 있지만
// 이렇게 작성하면 안된다.
void bad_use()
{
    std::unique_ptr<Widget> bad(new WidgetImpl(30));
    bad.reset(new WidgetImpl(40));

    // 반드시 std::make_unique<T>()를 통해서 초기화해야한다.
    std::unique_ptr<Widget> good = std::make_unique<WidgetImpl>(30);
    good                         = std::make_unique<WidgetImpl>(40);
}

// The only way you'll get the full benefit of unique_ptr is
// if you make sure that whenever you allocate a resource, you also
// initialize a unique_ptr to manage it.
// 자원을 할당하면 반드시 동시에 unique_ptr를 초기화하여 이를 관리하게 해야한다.
// 이를 std::make_unqiue<T>() helper 함수가 도와준다.
}

//-----------------------------------------------------------------------------
// [Why C++ doesn't have the finally keyword]
//-----------------------------------------------------------------------------
namespace section4
{
// 다음과 같은 코드는 항상 delete wh;를 입력해야 하므로
//  try {
//      use(*wh);
//  } catch (...) {
//      delete wh;
//      throw;
//  }
//  delete wh;
//
// 다른 언어에는 존재하는 finally keyword를 생각하게 된다.
//  try {
//      use(*wh);
//  } finally {
//      delete wh;
//  }
// 하지만 c++에는 없다. 이는 c++의 철학이 다음과 같기 때문이다.
//
// 만약에 어떠한 invariant를 강요하고 싶다면, 절대로 explicit code로
// 작성할 수 없다. 왜냐하면 당신은 코드를 잘못 작성하여 버그를 만드는 기회가
// 항상 있기 때문이다.
//
// 만약에 어떠한 invariant를 강요하고 싶다면, 이를 강요할 수 있는 올바른 장소는
// the type system 안이다. 그러면 당신의 새로운 타입에 대한 어떠한 사용을 하든
// 항상 invariant를 만족하도록 보장할 수 있다.
//
// 따라서 위와 같은 상황에서 finally같은 키워드를 사용할 생각하는 것이 아니라..
// "이를 위해 unique_ptr을 사용해야 할까?" 아니면
// "이를 위해 an RAII class type을 작성해야 할까?"를 고민해야 하는 것이다.
}

//-----------------------------------------------------------------------------
// [Customizing the deletion callback]
//-----------------------------------------------------------------------------
namespace section5
{
// unique_ptr에 사용자 정의 deletion callback을 설정할 수 있다.
// std::unique_ptr<T,D>의 D는 a deletion callback type이다.
// 디폴트는 std::default_delete<T>이며 operator delete를 호출한다.
struct fcloser {
    void operator()(FILE* fp) const { fclose(fp); }

    // std::make_unique<T,D> 버전은 없으므로 직접 raw pointer를 다루는 것을
    // 피하기 위해 a reusable helper function을 만들었다.
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
    // 만약 use가 throw한다면 f는 close 된다.
}
// std::unique_ptr의 소멸자는 a null pointer에 대해 the deletion callback를
// 절대로 호출하지 않기 때문에, fclose(NULL)이 호출되는 걱정을 할 필요 없다.
// fclose(NULL)은 "close all open file handles in the current process"한다.

// 직접 정의한 custom deleter는 std::unique_ptr<T,D> 객체 안에서 할당할 수 있기
// 때문에 D가 맴버 데이터를 갖는다면
// sizeof(unique_ptr<T,D>) > sizeof(unique_ptr<T>) 가 성립할 것이다.
}

//-----------------------------------------------------------------------------
// [Managing arrays with std::unique_ptr<T[]>]
//-----------------------------------------------------------------------------
namespace section6
{
// 만약 a pointer p가 배열의 첫번째 원소를 가리킨다면,
// delete p를 하면 안된다. 반드시 delete[] p를 해야한다.
// C++14 부터 std::unique_ptr<T[]>는 올바른 방법으로 std::default_delete<T[]>로
// operator delete[]를 올바르게 호출한다.

// 배열 타입에 대한 std::make_unique의 오버로드가 존재하지만 조심해서 사용해야
// 한다. argument를 원래 버전과 다른 의미로 사용하기 때문이다. 즉,
// std::make_unique<T[]>(n)은 new T[n]()를 호출한다. 하지만 이는 모든 원소들을
// 영초기화하므로 이를 원하지 않는다면, helper function을 만들어서 직접 new를
// 호출하여 리턴 값을 std::unique_ptr<T[]>으로 wrap 해야 한다.
}

//-----------------------------------------------------------------------------
// [Reference counting with std::shared_ptr<T>]
//-----------------------------------------------------------------------------
namespace section7
{
// unclear ownership에 의해 use-after-free bugs가 일어난다.
// 어떠한 메모리 자원에 대해 shared ownership이 있다면,
// 해당 자원을 여러 사람이 서로다른 시간, 서로다른 데이터구조, 서로다른
// 스레드에서 접근하여 자원을 언제든지 해제할 가능성이 있다. 그래서
// use-after-free bugs가 일어난다.
// 따라서 언제 공유 자원을 해제할지 결정하는 것이 필요하다.
// 이를 위해 std::shared_ptr<T>가 제공된다.
// reference counting으로 알려진 방법을 통해 메모리를 관리한다.
//
// std::shared_ptr은 힙 메모리 안에서 레퍼런스를 카운트하고 언제 레퍼런스들을
// 지우는 지를 결정하는 "control block"를 갖고 있다. 이는 library에서 보이지
// 않게 다루어진다.
//
// std::shared_ptr<Super> p = std::make_shared<Super>();
// p 안에 {ptr, ctrl}이 존재한다.
// ptr은 a Super object를 가리키고, ctrl은 control_block_impt<Deleter>를
// 가리킨다. 이 안에 {vptr, use=1, weak=0, ptr, Deleter}가 존재한다.
//
// unique_ptr과 비슷하게 std::make_shared<T>(args)로 직접적으로 raw pointer를
// 건드리지 않고 객체를 힙에 할당한다.
// std::make_shared<T>는 control block과 T object를 모두 할당한다.
// shared_ptr의 복제는 control block의 use-count를 increment하고
// shared_ptr의 소멸은 control block의 use-count를 decrement한다.
// shared_ptr에 값을 대입하는 것은 old value의 use-count를 decrement하고,
// new value의 use-count를 increment한다.
// 다음 예시를 보자.
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

// 다음과 같이 2개의 instances of shared_ptr이 the same control block을
// 참조하지만 the control block이 관리하는 메모리의 서로 다른 부분을 가리킬 수
// 있다.
// std::shared_ptr<Super> p = std::make_shared<Super>();
// std::shared_ptr<int> q(&p->second, p);
// 여기서 p,q의 ctrl은 모두 같은 control_block_impl<Deleter>를 가리키지만
// 각각의 ptr은 a Super object의 서로 다른 메모리를 가리킨다.

struct Super {
    int first, second;
    Super(int a, int b)
        : first(a),
          second(b)
    { }
    ~Super() { puts("destroying Super"); }
};

// 이러한 함수를 "aliasing constructor" of shared_ptr이라 부른다.
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
// shared_ptr<T>는 double-free bugs를 방지하지만
// raw pointer arguments를 받는 shared_ptr의 생성자를 사용하면 방지하지 못한다.
void example()
{
    class X
    {
    };
    std::shared_ptr<X> pa, pb, pc;

    pa = std::make_shared<X>();
    pb = pa;

    pc = std::shared_ptr<X>(pb.get()); // WRONG!
    // 이러한 방식을 원한다면 pc = std::shared_ptr<X>(pb,pb.get()) 을 사용해야
    // 한다. 하지만 이는 단순히 pc = pb와 동일하다.
    assert(pb.use_count() == 2);
    assert(pc.use_count() == 1);

    pc = nullptr;
    // pc의 use=0이 되므로 가리키는 X객체에 대해 delete를 호출한다.
    assert(pb.use_count() == 2);
    // *pb는 자원 해제된 객체에 접근하므로 undefined behavior가 발생한다.

    // 이후 pb가 소멸할때 double-free bugs가 일어난다.
}
// shared_ptr을 explicitly하게 사용하면 잘못 작성할 확률이 크므로 조심해야 한다.
// 즉, 코드에서 shared_ptr을 직접 사용하지 말고 항상 std::make_shared를 사용해야
// 한다. 그리고 shared_ptr 타입의 변수를 선언할 때 항상 auto를 사용해야 한다.
// 다만, 직접 type을 알려줘야 하는 클래스 맴버 타입에 대해서만 shared_ptr<T>로
// 선언한다.
}

//-----------------------------------------------------------------------------
// [Holding nullable handles with weak_ptr]
//-----------------------------------------------------------------------------
namespace section9
{
// 이전 절에서 the control block의 weak count에 대해서 설명하지 않았다.
// 여기서 weak_ptr에 대해서 설명한다.

// shared_ptr로 관리하는 공유하는 객체에 대해서 소유권을 갖지 않고 이 객체에
// 대한 포인터를 다루고 싶은 경우가 있다.
// 그래서 다음처럼 구현을 해보자.
struct DangerousWatcher {
    int* m_ptr = nullptr;

    void watch(const std::shared_ptr<int>& p) { m_ptr = p.get(); }
    int  current_value() const
    {
        // 여기서 *m_ptr가 이미 자원 해제되었다면 위험하다.
        return *m_ptr;
    }
};

struct NotReallyAWatcher {
    std::shared_ptr<int> m_ptr;

    void watch(const std::shared_ptr<int>& p) { m_ptr = p; }
    int  current_value() const
    {
        // 여기서 *m_ptr은 절대로 자원 해제되지 않기 때문에 안전하다.
        // 하지만 이는 소유권을 공유하므로 원하던 바가 아니다.
        return *m_ptr;
    }
};

// shared_ptr의 메모리 관리 시스템에 대해 전혀 모르지만 the control
// block으로부터 객체를 참조할 수 있는지 물어볼 수 있는 a non-owning reference
// 를 고려할 수 있다. 하지만 이러한 a non-owning reference는 객체에 접근하고자
// 하는 순간에 다른 스레드에서 이 객체를 deallocate할 수도 있기 때문에 위험해서
// 다른 방법을 고려해야 한다.
//
// 따라서 우리가 필요한 the primitive operation은 다음과 같다.
// "atomically get an owning reference (a shared_ptr) to the referenced object
// if it exists, or otherwise indicate failure."
//
// 그래서 우리는 a non-owning reference가 아니라...
// We need a ticket that we can exchange at some future date for an owning
// reference. 이것이 바로 std::weak_ptr<T>이다.
struct CorrectWatcher {
    std::weak_ptr<int> m_ptr;

    void watch(const std::shared_ptr<int>& p) { m_ptr = std::weak_ptr<int>(p); }
    int  current_value() const
    {
        // 안전하게 *m_ptr이 할당되어 있는 지 확인할 수 있다.
        if (auto p = m_ptr.lock()) {
            return *p;
        } else {
            throw "It has no value; it's been deallocated!";
        }
    }
};
// weak_ptr에 대해 오직 two operations만 알면 된다.
// 1. a shared_ptr<T>로부터 a weak_ptr<T>를 생성할 수 있다.
// 2. a weak_ptr<T>로부터 wptr.lock()을 호출하여
//                       a shared_ptr<T>의 생성을 시도할 수 있다.
// wptr.lock()에 대해 만약 weak_ptr이 만료되었다면 (자원이 해제되었다면), a null
// shared_ptr을 리턴한다.
// wptr.expired()를 통해 확인할 수 있지만 지금 당장 false를 리턴하지만 몇
// 마이크로초 후에 true를 리턴할 수 있어서 useless 하다.

// weak_ptr의 복제는 the control block의 weak-count를 increment한다.
// weak_ptr의 소멸은 the control block의 weak-count를 decrement한다.
// 만약 use-count가 0이 되었을 때 weak-count가 0이 아니라면,
// the control block는 소멸되지 않는다.
// 더이상 weak_ptr objects가 객체를 가리키지 않았을 때, (weak-count가 0이 되어)
// the control block은 deallocate 된다.
//
// 만약 use-count가 0이 되었을 때 weak-count가 0이 아닌 경우,
// the control_block_impl<Deleter>의 ptr이 더이상 해당 객체를 가리키지 않아서
// weak_ptr의 ptr은 사용할 수 없게 된다.
}

//-----------------------------------------------------------------------------
// [Talking about oneself with std::enable_shared_from_this]
//-----------------------------------------------------------------------------
namespace section10
{
// shared_ptr의 double-managing a pointer by creating multiple control blocks
// 에 대해 위험하다고 언급했다.
//
// A::foo()가 외부 함수 bar()를 수반한다고 하자.
// bar()는 객체 A의 pointer가 필요하다고 하자.
// 만약 lifetime management를 걱정할 필요가 없다면, 단순히 bar(this)를 하면
// 된다.
//
// A가 shared_ptr에 의해 메모리 관리되고 있다고 하자.
// bar()는 콜백을 위해 this의 복사본을 내부 어딘가에 저장한다고 하자.
// 이후 A::foo()가 끝나가는 동안 동시에 실행하는 새로운 스레드를 생성했을 때,
// bar()가 실행하는 도중에는 A가 살아 있어야 한다.
//
// 그래서 bar()는 std::shared_ptr<A>의 매개변수를 가져야 한다.
// A::foo()가 맴버 변수 std::shared_ptr<A>를 가진다면 이를 전달해줄 수
// 있지만 A는 자기자신을 갖고 있어서 절대로 소멸되지 않게 된다.
//
// 대신에 맴버 변수로 std::weak_ptr<A>를 가진다면 bar(this->m_wptr.lock())을
// 호출하면 해결된다. 하지만 이는 조금의 syntatic overhead가 있고 m_wptr이
// 초기화되어 있는지 명료하지 않기 때문에 c++에서는 다음과 같은
// std::enable_shared_from_this를 구현했다.

// enable_shared_from_this는 객체의 한 부분인 location in memory를 사용하기
// 때문에 아래와 같이 작성한다.
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
// shared_ptr의 생성자는 T가 enable_shared_from_this<T>로부터
// publicly inherit 했는지를 판단하여 m_weak를 설정한다.
// 그래서 반드시 public and unambiguous 하게 상속해야 한다.

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
// "X inherits from A<X>"를 the Curiously Recurring Template Pattern, or
// CRTP라고 부른다.
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
// boost::addable은 비슷하게 다음과 같이 제공한다.
// operator+를 a friend free function 으로 둔다. 이러한 방식을 "Barton-Nackman
// trick"이라 부른다.
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
// 어떠한 derived-class behavior를 a method of base class로 inject시키고 싶을때
// CRTP 패턴을 사용한다.
}

//-----------------------------------------------------------------------------
// [A final warning]
//-----------------------------------------------------------------------------
// shared_ptr, weak_ptr, and enable_shared_from_this의 시스템은
// 가비지 컬렉터만큼의 안전함을 주고, c++의 특징인 결정론적인 소멸과 속도를
// 보존한다.
// shared_ptr을 남용하는 것에 주의해야 한다. 대부분의 코드에서 shared_ptr을
// 사용하여 힙 객체에 대한 소유권을 공유하면 안된다.
// 첫번째로 항상 힙 할당을 (값 의미론을 사용하여) 완전히 피해야 한다.
// 두번째로 힙 할당 객체는 a unique owner를 갖도록 해야 한다.
// 두 가지가 불가능할 때, shared ownership과 std::shared_ptr<T>을 고려한다.

//-----------------------------------------------------------------------------
// [Denoting un-special-ness with observer_ptr<T>]
//-----------------------------------------------------------------------------
namespace section12
{
class Widget;
// 다음과 같은 a function signature를 생각하자.
void remusnoc(std::unique_ptr<Widget> p);
// 는 다루고 있는 객체의 소유권을 remusnoc에 넘기는 것을 의미한다.
// 이 함수를 호출할 때, 우리는 Widget 객체에 대해 unique ownership을 갖고 있어야
// 한다. 그리고 함수를 호출한 후, 우리는 더이상 그 객체에 대해 접근할 수 없다.
// 우리는 remusnoc가 Widget을 소멸할 지, 유지할 지, 또는 다른 객체나 스레드에
// 붙일 지는 모른다. 더 이상 우리가 신경써야할 대상이 아니게 된다.
//
// 위와 반대로,
std::unique_ptr<Widget> recudorp();
// 를 호출할 때, 리턴받는 Widget 객체가 어떠한 것이든 이 객체에 대한 unique
// ownership 을 우리가 갖게 된다. 이는 다른 Widget 객체의 레퍼런스가 아니다.
// 또한 어떠한 static data의 포인터도 아니다. 명시적으로 힙 할당된 Widget
// 객체이고 고유한 소유권을 호출하는 우리가 갖게 된다.
//
// 하지만 다음과 같은 c++ function의 의미는 어떻게 될까?
void suougibma(Widget* p);
// 이 함수는 넘겨받는 포인터의 소유권을 가질 수도 있고 아닐 수도 있어서
// 모호하다. suougibma의 문서를 찾아 보아야 할 수도 있고 전체 코드에 대한
// stylistic conventions ("로우 포인터는 절대로 소유권을 나타내지
// 않는다.") 으로부터 의미를 파악할 수 있다. 하지만 이는 함수 시그니처 자체가
// 말해주지 않는다.
//
// unique_ptr<T>는 ownership transfer를 표현하는 a vocabulary type이다.
// 반면에 *T는 전혀 vocabulary가 아니다.
// 만약 non-owning pointers를 넘기는 코드가 많다면,
// (물론 포인터가 아니라 레퍼런스로 넘길 수 있다면 반드시 레퍼런스로 넘겨야 하고
// 그럴 수 없을 때 non-owning pointer를 넘기는 경우에 해당한다.)
// 이러한 non-owning pointer를 표현하는 a vocabulary type을 생각할 수 있다.
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
// observer_ptr은 포인터를 복제할 수 없고 포인터가 가리키는 객체에 대한
// lifetime에 전혀 영향을 끼치지 않는다. 만약 the lifetime에 영향을 주고 싶다면
// 소유권을 받는 unique_ptr 이나 shared_ptr을 사용해야 한다.
//
// 하지만 c++ standard library에는 이러한 a vocabulary type이 없다.
// 주된 이유로 많은 숙련자들이 다음과 같이 생각하기 때문이다.
// 반드시 *T는 the vocabulary type for "non-owning pointer"로 해석해야
// 한다. 소유권을 이전하기 위해 *T를 사용하는 오래된 코드는 반드시 재작성되거나
// 최소한 owner<T*>처럼 re-annotated 해야한다.
// 
// 다시한번 강조하자면,
// Never use raw pointers for ownership transfer!
}
