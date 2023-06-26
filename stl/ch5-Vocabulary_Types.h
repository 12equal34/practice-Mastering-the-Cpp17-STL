#pragma once

#include <cstdio>
#include <string>
#include <algorithm>
#include <cassert>
#include <array>
#include <type_traits>
#include <memory>
#include <vector>
#include <iterator>
#include <deque>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <thread>
#include <variant>
#include <iostream>
#include <optional>
#include <any>
#include <functional>
#include <future>

#pragma warning(disable : 4101)

//-----------------------------------------------------------------------------
// [The story of std::string]
//-----------------------------------------------------------------------------
namespace section1
{
// c에서는 string의 공용어로 char*을 다루었다.
// char* greet(const char* name)
//{
//    char buffer[100];
//    snprintf(buffer, 100, "hello %s", name);
//    return strdup(buffer);
//}

// void test()
//{
//     const char* who = "world";
//     char*       hw  = greet(who);
//     assert(strcmp(hw, "hello world") == 0);
//     free(hw);
// }
//  const가 없던 시절에는 오래된 라이브러리는 char*를 리턴하고
//  const가 있게 된 후 const char*를 리턴하는 라이브러리와 혼용된다.
//  char*는 길이를 넘겨주지 않기 때문에 포인터와 길이를 받는 함수들이 있었고
//  포인터만 받는 함수들도 있었다. char*는 '\0'을 쉽게 다루기 힘들었다.

// char*는 lifetime management 와 ownership을 놓치기 쉽다.
// C function은 호출자로부터 char*을 받고, 호출자에게 char*의 관리를 맡겼다.
// 하지만 만약 char*를 리턴하길 원한다면 호출자는 이를 free하는 것을 기억해야
// 했다.

// C++ in 1998 부터 a standard string class인 std::string이 나오면서
// char*의 모든 문제가 해결되었다.
// std::string은 문자열의 바이트들과 길이를 캡슐화하고 올바르게 '\0'을 다룰 수
// 있다. hello + world 같은 연산자도 지원하고 꼭 필요한 만큼의 메모리를
// 정확하게 할당한다. RAII 덕분에 절대로 메모리 누수가 일어나지 않고 ownership이
// 명확하다. char*로 암시적 변환을 갖고 있다.

std::string greet_str(const std::string& name) { return "hello " + name; }

void test_str()
{
    std::string who = "world";
    assert(greet_str(who) == "hello world");
}
}

//-----------------------------------------------------------------------------
// [Tagging reference types with reference_wrapper]
//-----------------------------------------------------------------------------
namespace section2
{
// 다음은 std::reference_wrapper의 구현이다.
template <typename T>
class reference_wrapper
{
    T* m_ptr;
public:
    reference_wrapper(T& t) noexcept
        : m_ptr(&t)
    { }

       operator T&() const noexcept { return *m_ptr; }
    T& get() const noexcept { return *m_ptr; }
};
template <typename T>
reference_wrapper<T> ref(T& t)
{
    return reference_wrapper<T>(t);
}

void example()
{
    int  result = 0;
    auto task   = [](int& r) {
        r = 42;
    };

    // 다음은 컴파일되지 않지만
    // std::thread t(task, result);

    // by reference로 전달하면 컴파일된다.
    std::thread t(task, std::ref(result));
}
}

//-----------------------------------------------------------------------------
// [C++11 and algebraic types]
//-----------------------------------------------------------------------------
namespace section3
{
enum class Color {
    RED   = 1,
    BLACK = 2,
};
enum class Size {
    SMALL  = 1,
    MEDIUM = 2,
    LARGE  = 3,
};
// Color와 Size의 a product type을 생성할 수 있다.
using sixtype = std::pair<Color, Size>;

// Color와 Size의 a sum type을 생성할 수 있다.
// 2+3 = 5 개의 서로다른 타입, Color이거나 아니면 Size.
using fivetype = std::variant<Color, Size>;

// std::pair<A,A>와 std::array<A,2>는 겉모양 빼고 같다.
}

//-----------------------------------------------------------------------------
// [Working with std::tuple]
//-----------------------------------------------------------------------------
namespace section4
{
// C++11의 variadic templates으로 2개 이상의 템플릿 매개변수를 받을 수 있게
// 되었다. 이에 따라 std::tuple<Ts..>는 완전한 heterogeneous array 이다.
// std::tuple<int, double>은 std::pair<int, double>과 차이가 없다.
// 예를 들어, std::tuple<int, int, char, std::string>은
// 맴버로 a int, another int, a char, and finally a std::string을 갖는
// a struct 와 유사하다.
// 첫번째와 두번째 원소의 타입이 다르기 때문에 런타임 시간에 인덱싱하는
// operator[](size_t)는 사용할 수 없다. 그래서 컴파일 시간에 해당 원소에
// 접근할 것을 컴파일러에게 알려주는 방식으로 접근할 수 있다.
// std::get<0>(t), std::get<1>(t), ...을 사용한다.
// 이러한 방식은 std::tuple을 다루는 패턴이 되는데,
// the homogeneous container는 member functions를 사용하고,
// the heterogeneous algebraic type은 free function templates를
// 사용한다.

// 하지만 일반적으로 tuples를 조작할 일은 별로 없을 것이다.
// template metaprogramming을 할 때 주로 사용한다.
// 그게 아니라면 주로 데이터들을 한꺼번에 묶어서 처리하는 비용이 저렴한 방법인
// std::tie 를 사용한다.
void example1()
{
    using Author                = std::pair<std::string, std::string>;
    std::vector<Author> authors = {
        {"Fyodor",   "Dostroevsky"},
        {"Sylvia",   "Plath"      },
        {"Vladimir", "Nabokov"    },
        {"Douglas",  "Hofstadter" },
    };

    std::sort(authors.begin(), authors.end(), [](auto&& a, auto&& b) {
        return std::tie(a.first, a.second) < std::tie(b.first, b.second);
    });
    assert((authors[0] == Author("Douglas", "Hofstadter")));

    std::sort(authors.begin(), authors.end(), [](auto&& a, auto&& b) {
        return std::tie(a.second, a.first) < std::tie(b.second, b.first);
    });
    assert((authors[0] == Author("Fyodor", "Dostroevsky")));
}
// std::tie는 arguments의 value가 아닌 references의 tuple을 만들기 때문에 비용이
// 저렴하다. 그래서 다음처럼 "multiple assignment"가 가능하다.
void example2()
{
    std::string s;
    int         i;

    // 문맥 상에서 한줄의 대입문으로 여러 값을 대입할 수 있다.
    std::tie(s, i) = std::make_tuple("hello", 42);
}
// 반면에, std::make_tuple(a,b,c...)는 arguments의 references가 아니라
// values의 복사본을 생성한다.

// c++17의 생성자 템플릿 매개변수 연역은 단순히 std::tuple(a,b,c...)로 사용할 수
// 있게 하는데, 다음의 예제처럼
// std::tuple은 std::reference_wrapper arguments를 보존하고
// std::make_tuple은 std::reference_wrapper arguments를 native c++ references로
// decay한다.
void example3()
{
    auto [i, j, k] = std::tuple {1, 2, 3};

    auto t1 = std::make_tuple(i, std::ref(j), k);
    static_assert(std::is_same_v<decltype(t1), std::tuple<int, int&, int>>);

    auto t2 = std::tuple(i, std::ref(j), k);
    static_assert(
        std::is_same_v<decltype(t2),
                       std::tuple<int, std::reference_wrapper<int>, int>>);
}
}

//-----------------------------------------------------------------------------
// [Manipulating tuple values]
//-----------------------------------------------------------------------------
namespace section5
{
// 여기의 대부분의 함수와 템플릿은 template metaprogramming에만 유용하다.
// std::get<I>(t) : t의 I번째 원소의 reference를 retrieve한다.
// std::tuple_size_v<decltype(t)>: 주어진 tuple의 size를 리턴한다.
// std::tuple_element_t<I, decltpye(t)>: 주어진 tuple의 I번째 원소의 타입을
// 리턴한다. 일반적으로 decltype(std::get<I>(t))를 사용한다.
// std::tuple_cat(t1,t2,t3...): 주어진 tuples를 concatenate한다.
// std::forward_as_tuple(a,b,c...): 완벽 전달된 원소들의 references를 갖는
// a tuple를 생성한다.
template <typename F>
void run_zeroarg(const F& f)
{
    //...
}

template <typename F, typename... Args>
void run_multiarg(const F& f, Args&&... args)
{
    auto fwd_args = std::forward_as_tuple(std::forward<Args>(args)...);
    auto lambda   = [&f, fwd_args]() {
        std::apply(f, fwd_args);
    };
    run_zeroarg(f);
}
}

//-----------------------------------------------------------------------------
// [A note about named classes]
//-----------------------------------------------------------------------------
// STL class template을 사용하면 잘 테스트된 STL components를 재사용하여
// 개발 시간을 줄여준다. 하지만 우리의 코드를 less readable or give your types
// too much functionality 하게 할 수 있다.
// 예를 들어, 이전 장에서 std::array<double, 3>을 Vec3를 구현하기 위해
// 사용했는데 불필요한 operator< 가 추가되었다.
// the algebraic types (tuple, pair, optional, or variant)를 곧바로 API로
// 사용하면 안된다. your own "domain-specific vocabulary" types을 위한
// named classes를 작성한다면, 우리의 코드를 읽고 이해하고 유지하는데 더 쉽게
// 해줄 것이다.
//

//-----------------------------------------------------------------------------
// [Expressing alternatives with std::variant]
//-----------------------------------------------------------------------------
namespace section6
{
// std::tuple<A,B,C>는 a product type이고,
// std::variant<A,B,C>는 a sum type이다.
// 다음 예시를 보자.
void example1()
{
    std::variant<int, double> v1;

    v1 = 1; // v1은 현재 int
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 1);
    assert(std::get<int>(v1) == 1);

    v1 = 3.14; // v1은 현재 double
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 3.14);
    assert(std::get<double>(v1) == 3.14);

    assert(std::holds_alternative<int>(v1) == false);
    assert(std::holds_alternative<double>(v1) == true);

    assert(std::get_if<int>(&v1) == nullptr); // 현재 v1은 double이다.
    assert(*std::get_if<double>(&v1) == 3.14);

    // std::get은 exception을 던질 수 있다.
    // std::get_if는 실패하면 nullptr을 리턴한다.

    // Worst...
    try {
        std::cout << std::get<int>(v1) << std::endl;
    } catch (const std::bad_variant_access&) {
    }

    // Still bad...
    if (v1.index() == 0) {
        std::cout << std::get<int>(v1) << std::endl;
    }

    // Slightly better...
    if (std::holds_alternative<int>(v1)) {
        std::cout << std::get<int>(v1) << std::endl;
    }

    // ...Best.
    if (int* p = std::get_if<int>(&v1)) {
        std::cout << *p << std::endl;
    }
}
}

//-----------------------------------------------------------------------------
// [Visiting variants]
//-----------------------------------------------------------------------------
namespace section7
{
// 다음 예시를 보자.
struct Visitor {
    double operator()(double d) { return d; }
    double operator()(int i) { return double(i); }
    double operator()(const std::string&) { return -1; }
};

using Var = std::variant<int, double, std::string>;

void show(Var v) { std::cout << std::visit(Visitor {}, v) << std::endl; }

void test()
{
    show(3.14);
    show(1);
    show("hello world");
}

// 다음처럼 작성할 수 있다.
void show1(Var v)
{
    std::visit(
        [](const auto& alt) {
            if constexpr (std::is_same_v<decltype(alt), const std::string&>) {
                std::cout << double(-1) << std::endl;
            } else {
                std::cout << double(alt) << std::endl;
            }
        },
        v);
}

// 다음처럼 multiple dispatch 버전으로 만들 수 있다.
struct MultiVisitor {
    template <class T, class U, class V>
    void operator()(T, U, V) const
    {
        puts("wrong");
    }
    void operator()(char, int, double) const { puts("right!"); }
};

void test2()
{
    std::variant<int, double, char> v1 = 'x';
    std::variant<char, int, double> v2 = 1;
    std::variant<double, char, int> v3 = 3.14;
    std::visit(MultiVisitor {}, v1, v2, v3); // prints "right!"
}
}

//-----------------------------------------------------------------------------
// [What about make_variant? and a note on value semantics]
//-----------------------------------------------------------------------------
namespace section8
{
// std::make_variant<A,B,C>(a) 는 존재하지 않는다.
// std::make_pair, std::make_tuple은
// std::reference_wrapper<T>를 T&으로 decay한다.
// 즉, std::make_pair(std::ref(a), std::cref(b))는
// std::pair<A&, const B&>를 생성한다.
// 그래서 std::make_variant<A,B,C>(a)가 있다면 이렇게
// reference-decaying ability 가 있어야 하지만,
// std::optional, std::any, std::variant는 레퍼런스 타입의 원소들을
// 갖지 못하게 표준되어 있다.
// 하지만 std::variant<std::reference_wrapper<T>,...>는 가능한데,
// 이처럼 wrapper되어 있는 타입은 레퍼런스 타입이 아니므로 합법적이다.

// std::variant<A,B,C>는 항상 A,B,C 중 정확히 하나의 타입 값만 갖지만
// "valueless by exception"가 발생하면 어느 값도 갖지 않을 수 있다.
struct A {
    A() { throw "haha"; }
};
struct B {
    operator int() { throw "haha"; }
};
struct C {
    C()               = default;
    C& operator=(C&&) = default;
    C(C&&) { throw "haha"; } // move-constructor는 반드시 noexpect이어야 한다.
};

void test()
{
    std::variant<int, A, C> v1 = 42;

    try {
        v1.emplace<A>();
    } catch (const char* haha) {
    }
    assert(v1.valueless_by_exception());

    try {
        v1.emplace<int>(B());
    } catch (const char* haha) {
    }
    assert(v1.valueless_by_exception());

    // emplace 대신에 대입문을 사용하면 이러한 상황을 피할 수 있다.
    v1 = 42;
    try {
        v1 = A();
    } catch (...) {
    }
    assert(std::get<int>(v1) == 42);

    try {
        v1 = B();
    } catch (...) {
    }
    assert(std::get<int>(v1) == 42);

    // 하지만 move-constructor는 피할 수 없다.
    try {
        v1 = C();
    } catch (...) {
    }
    assert(v1.valueless_by_exception());

    assert(v1.index() == size_t(-1));

    // valueless인 v1을 std::visit를 하면 std::bad_variant_access를 던진다.
}
}

//-----------------------------------------------------------------------------
// [Delaying initialization with std::optional]
//-----------------------------------------------------------------------------
namespace section9
{
// std::variant를 다음과 같이 사용할 수 있다.
// "Maybe I have an object, and maybe I don't."
// "maybe I don't"의 state를 the standard tag type인 std::monostate로 나타낸다.
std::map<std::string, int> g_limits = {
    {"memory", 655360},
};

std::variant<std::monostate, int> get_resource_limit(const std::string& key)
{
    if (auto it = g_limits.find(key); it != g_limits.end()) {
        return it->second;
    }
    return std::monostate {};
}

void test()
{
    auto limit = get_resource_limit("memory");
    if (std::holds_alternative<int>(limit)) {
        // use(std::get<int>(limit));
    } else {
        // use(some_default);
    }
}

// 이러한 종류의 variant로 std::optional<T>가 존재한다.
std::optional<int> get_resource_limit_op(const std::string& key)
{
    if (auto it = g_limits.find(key); it != g_limits.end()) {
        return it->second;
    }
    return std::nullopt;
}

void test_op()
{
    auto limit = get_resource_limit_op("memory");
    if (limit.has_value()) {
        // use(*limit);
    } else {
        // use(some_default);
    }
}

// free (non-member) functions을 갖고 있는 std::tuple과 std::variant와는
// 다르게 std::optional<T>는 편리한 member functions을 갖고 있다.
// 또한 T로부터 비교연산자들을 사용할 수 있다. std::nullopt 상태는 항상
// less than any real value of T 가 성립한다.
//
// bool(o) == o.has_value(), !o == !o.has_value()가 항상 성립한다.
// std::optional<bool> o에 대해 o == false와 !o의 의미가 서로 다르다.
//
// o.value()는 o가 갖고 있는 값의 reference를 리턴하고,
// 만약 값이 없다면 std::bad_optional_access를 던진다.
// 반면에 *o는 o가 갖고 있는 값의 reference를 리턴하고, 만약 값이 없다면
// undefined behavior를 일으킬 수 있는 의미없는 reference를 리턴한다.
//
// o.value_or(x)는 o가 갖고 있는 값의 복사본을 리턴하거나, 값이 없다면
// a copy of x converted to type T을 리턴한다.
// 위의 테스트에서 use(limit.value_or(some_default))를 사용할 수 있다.

// std::optional<T>는 "maybe a T" 뿐만 아니라 "not yet a T"의 상황에서
// 사용할 수 있다.
auto make_lambda(int arg)
{
    return [arg](int x) {
        return x + arg;
    };
}
using L = decltype(make_lambda(0));

// 타입 L은 디폴트 생성자가 없고, 이동 대입 연산자가 없다.
static_assert(!std::is_default_constructible_v<L>);
static_assert(!std::is_move_assignable_v<L>);

// 그래서 클래스의 맴버가 되면 디폴트 생성자가 없기 때문에
// "not yet a T" 상황이 된다.
class ProblematicAdder
{
    L fn_;
};
static_assert(!std::is_default_constructible_v<ProblematicAdder>);

// 이러한 상황에서 std::optional<L>을 사용한다.
class Adder
{
    std::optional<L> fn_;
public:
    void setup(int first_arg) { fn_.emplace(make_lambda(first_arg)); }
    int  call(int second_arg)
    {
        // 한번이라도 setup()을 호출하지 않으면 예외를 던진다.
        return fn_.value()(second_arg);
    }
};
static_assert(std::is_default_constructible_v<Adder>);

void example()
{
    Adder adder;
    adder.setup(4);
    int result = adder.call(5);
    assert(result == 9);
}
}

//-----------------------------------------------------------------------------
// [Revisiting variant]
//-----------------------------------------------------------------------------
namespace section10
{
// The variant data type은 recursive data types가 아니기 때문에
// 다음은 컴파일 에러가 난다.
//     using JSONValue = std::variant<
//         std::nullptr_t, bool, double, std::vector<JSONValue>,
//         std::map<std::string, JSONValue>>;
//
// 하지만 boost::variant를 사용하면 the marker type boost::recursive_variant_를
// 통해 다음처럼 재귀적인 타입을 만들 수 있다.
//     using JSONValue = boost::variant<
//         std::nullptr_t,
//         bool,
//         double,
//         std::string,
//         std::vector<boost::recursive_variant_>,
//         std::map<std::string, boost::recursive_variant_>
//     >;
//
// algebraic type 대신에 forward references to class types을 사용할 수 있다.
struct JSONValue {
    std::variant<std::nullptr_t, bool, double, std::string,
                 std::vector<JSONValue>, std::map<std::string, JSONValue>>
        value_;
};
}

//-----------------------------------------------------------------------------
// [Infinite alternatives with std::any]
//-----------------------------------------------------------------------------
namespace section11
{
// 모든 타입을 담을 수 있는 타입으로 std::any가 있다.
// std::any 타입으로 할 수 있는 연산들은 다음과 같다.
// 현재 객체를 갖고 있는 지 물어본다.
// 새로운 객체를 집어 넣고, 기존 객체는 소멸시킨다.
// 현재 객체의 타입을 물어본다.
// 현재 객체를 리턴한다.
void example1()
{
    // 현재는 an empty container이다.
    std::any a;
    assert(!a.has_value());

    a = std::string("hello");
    assert(a.has_value());
    assert(a.type() == typeid(std::string));

    a = 42;
    assert(a.has_value());
    assert(a.type() == typeid(int));

    if (std::string* p = std::any_cast<std::string>(&a)) {
        // use(*p);
    } else {
        // 예외를 던지지 않고 null pointer를 리턴한다.
    }

    try {
        std::string& s = std::any_cast<std::string&>(a);
        // use(s);
    } catch (const std::bad_any_cast&) {
        // 예외 std::bad_any_cast를 던진다.
    }
}
// std::any 버전의 std::visit은 존재하지 않는다.
}

//-----------------------------------------------------------------------------
// [std::any versus polymorphic class types]
//-----------------------------------------------------------------------------
namespace section12
{
struct Animal {
    virtual ~Animal() = default;
};

struct Cat : Animal {
};

void test()
{
    std::any a = Cat {};

    // 현재 담고 있는 Cat 클래스에서는 당연히 작동하지만
    assert(a.type() == typeid(Cat));
    assert(std::any_cast<Cat>(&a) != nullptr);

    // Cat의 base 클래스에 대해서는 작동하지 않는다.
    assert(a.type() != typeid(Animal));
    assert(std::any_cast<Animal>(&a) == nullptr);

    // 또한 void*로의 캐스팅에 대해서도 작동하지 않는다.
    assert(std::any_cast<void*>(&a) == nullptr);
}
}

//-----------------------------------------------------------------------------
// [Type erasure in a nutshell]
//-----------------------------------------------------------------------------
namespace section13
{
// std::any는 어떻게 구현할까? 핵심은 type erasure이다.
// 모든 타입 T에 대해 특정 operations을 동일시하고, 다른 모든 operations을
// 지운다. 동일시하는 특정 operations은 다음과 같다. 갖고 있는 객체의 복사본을
// 생성한다. 갖고 있는 객체의 복사본을 이동 생성한다. 갖고 있는 객체의 typeid를
// 얻는다.
// class Any;
// struct AnyBase
//{
//    virtual const std::type_info& type() = 0;
//    virtual void                  copy_to(Any&) = 0;
//    virtual void                  move_to(Any&) = 0;
//    virtual ~AnyBase() = default;
//};
//
// template <typename T>
// struct AnyImpl : AnyBase {
//    T                     t_;
//    const std::type_info& type() { return typeid(T); }
//    void                  copy_to(Any& rhs) override
//    {
//        // T가 copy-constructible이어야 한다.
//        rhs.emplace<T>(t_);
//    }
//    void move_to(Any& rhs) override { rhs.emplace<T>(std::move(t_)); }
//};
//
// class Any
//{
//    std::unique_ptr<AnyBase> p_ = nullptr;
// public:
//    template <typename T, typename... Args>
//    std::decay_t<T>& emplace(Args&&... args)
//    {
//        p_ = std::make_unique<AnyImpl<T>>(std::forward<Args>(args)...);
//    }
//
//    bool has_value() const noexcept { return (p_ != nullptr); }
//
//    void reset() noexcept { p_ = nullptr; }
//
//    const std::type_info& type() const
//    {
//        return p_ ? p_->type() : typeid(void);
//    }
//
//    Any(const Any& rhs) { *this = rhs; }
//
//    Any& operator=(const Any& rhs)
//    {
//        if (rhs.has_value()) {
//            rhs.p_->copy_to(*this);
//        }
//        return *this;
//    }
//};

// 여기서 이동 대입 연산자는 생략했다.
// 보통 이러한 연산은 noexcept으로 swapping pointer를 하지만
// "small object optimization"의 경우에
// nothrow-move-constructible T의 heap allocation을 피하고
// not swapping pointer를 하면서 T를 stack에 복사한다.
//
}

//-----------------------------------------------------------------------------
// [std::any and copyability]
//-----------------------------------------------------------------------------
namespace section14
{
// AnyImpl<T>::copy_to는 T가 copy-constructible이어야 정의되기 때문에
// std::any는 move-only type을 저장할 수 없다.
// 따라서 move-only type을 저장하고 싶으면 이를 감싸는
// copy-constructible wrapper를 사용해야 한다. 예를 들어, "Shim":
using Ptr = std::unique_ptr<int>;

template <class T>
struct Shim {
    T get() { return std::move(*t_); }

    template <class... Args>
    Shim(Args&&... args)
        : t_(std::in_place, std::forward<Args>(args)...)
    { }

    // Shim의 move-only object만을 생각하겠다.
    Shim(Shim&&)            = default;
    Shim& operator=(Shim&&) = default;
    Shim(const Shim&) { throw "복제 생성자"; }
    Shim& operator=(const Shim&) { throw "복제 대입 연산자"; }
private:
    std::optional<T> t_;
};

void test()
{
    Ptr p = std::make_unique<int>(42);

    // Ptr은 move-only이기 때문에 std::any에 저장할 수 없다.
    // std::any a = std::move(p);

    // 하지만 Shim<Ptr>은 가능하다.
    std::any a = Shim<Ptr>(std::move(p));
    assert(a.type() == typeid(Shim<Ptr>));

    std::any b = std::move(a);

    try {
        std::any c = b;
    } catch (...) {
    }

    Ptr r = std::any_cast<Shim<Ptr>&>(b).get();
    assert(*r == 42);
}
}

//-----------------------------------------------------------------------------
// [Again with the type erasure: std::function]
//-----------------------------------------------------------------------------
namespace section15
{
// std::any에서 모든 타입이 동일하게 작동하는 operations을 생각했다.
// 이에 따라 인수 타입들 A...를 R 타입으로 변환하는 operation을 생각할 수 있다.
// 이에 대응하는 std::function<R(A...)>가 있다.
int  my_abs(int x) { return x < 0 ? -x : x; }
long unusual(long x, int y = 3) { return x + y; }

void test()
{
    std::function<int(int)> f;
    assert(!f);

    f = my_abs;
    assert(f(-42) == 42);

    f = [](long x) {
        return unusual(x);
    };
    assert(f(-42) == -39);

    f = [i = 0](int) mutable {
        return ++i;
    };
    assert(f(-42) == 1);
    assert(f(-42) == 2);

    // Copying std::function은 항상 갖고있는 객체의 복사본을 만든다.
    // 그러므로 갖고있는 객체의 상태를 보존한다.
    auto g = f;
    assert(f(-42) == 3);
    assert(f(-42) == 4);
    assert(g(-42) == 3);
    assert(g(-42) == 4);

    // std::any처럼 타입에 대한 정보를 알 수 있다.
    // f.target<T>()는 a.type()과 동등하다.
    // f.target<T>()는 std::any_cast<T*>(&a)와 동등하다.
    // 하지만 거의 사용하지 않는다.
    if (f.target_type() == typeid(int (*)(int))) {
        int (*p)(int) = *f.target<int (*)(int)>();
        // use(p);
    } else {
        // ...
    }
}

// 가장 중요한 use-case는 vocabulary type으로서 모듈 간에 passing
// "behaviors"하는 역할이다. 이러한 경우에 템플릿으로는 불가능할 수 있다.
// 예를 들어, 외부 라이브러리에 콜백함수를 넘기거나, 직접 라이브러리를 제작할 때
// 호출자의 콜백함수가 필요한 코드를 작성하는 경우가 있다.
//
// 템플릿은 호출되는 지점에서 템플릿의 코드가 반드시 보여야 한다.
// 외부에는 해당 템플릿이 보이지 않기 때문에 외부에서 콜백함수를 받을 수 없다.
template <class F>
void templated_for_each(std::vector<int>& v, F f)
{
    for (int& i : v) {
        f(i);
    }
}
// 반면에 type-erasure인 std::function은 받을 수 있다.
// type_erased_for_each has a stable ABI and a fixed address.
// It can be called with only its declaration in scope.
extern void type_erased_for_each(std::vector<int>&, std::function<void(int)>);
}

//-----------------------------------------------------------------------------
// [std::funciton, copyability, and allocation]
//-----------------------------------------------------------------------------
namespace section16
{
// std::any, std::function은 모두 copy-constructible 객체만 저장할 수 있다.
// 즉, move-only types은 직접적으로 저장할 수 없다. wrapper를 통해 간접적으로는
// 가능하다.
// std::future<T>, std::unique_ptr<T>같은 move-only types를 캡쳐하는 lamdas를
// 사용하는 경우에 lamdas는 모두 move-only이다.
// 그래서 move-only types를 캡쳐할 때 shared_ptr로 캡쳐하게 한다.
void example()
{
    auto capture = [](auto& p) {
        using T = std::decay_t<decltype(p)>;
        return std::make_shared<T>(std::move(p));
    };

    std::promise<int> p;

    std::function<void()> f = [sp = capture(p)]() {
        sp->set_value(42);
    };
}
// std::any와 std::function 모두 customization allocation을 허용하지 않는다.
}