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
// c������ string�� ������ char*�� �ٷ����.
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
//  const�� ���� �������� ������ ���̺귯���� char*�� �����ϰ�
//  const�� �ְ� �� �� const char*�� �����ϴ� ���̺귯���� ȥ��ȴ�.
//  char*�� ���̸� �Ѱ����� �ʱ� ������ �����Ϳ� ���̸� �޴� �Լ����� �־���
//  �����͸� �޴� �Լ��鵵 �־���. char*�� '\0'�� ���� �ٷ�� �������.

// char*�� lifetime management �� ownership�� ��ġ�� ����.
// C function�� ȣ���ڷκ��� char*�� �ް�, ȣ���ڿ��� char*�� ������ �ð��.
// ������ ���� char*�� �����ϱ� ���Ѵٸ� ȣ���ڴ� �̸� free�ϴ� ���� ����ؾ�
// �ߴ�.

// C++ in 1998 ���� a standard string class�� std::string�� �����鼭
// char*�� ��� ������ �ذ�Ǿ���.
// std::string�� ���ڿ��� ����Ʈ��� ���̸� ĸ��ȭ�ϰ� �ùٸ��� '\0'�� �ٷ� ��
// �ִ�. hello + world ���� �����ڵ� �����ϰ� �� �ʿ��� ��ŭ�� �޸𸮸�
// ��Ȯ�ϰ� �Ҵ��Ѵ�. RAII ���п� ����� �޸� ������ �Ͼ�� �ʰ� ownership��
// ��Ȯ�ϴ�. char*�� �Ͻ��� ��ȯ�� ���� �ִ�.

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
// ������ std::reference_wrapper�� �����̴�.
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

    // ������ �����ϵ��� ������
    // std::thread t(task, result);

    // by reference�� �����ϸ� �����ϵȴ�.
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
// Color�� Size�� a product type�� ������ �� �ִ�.
using sixtype = std::pair<Color, Size>;

// Color�� Size�� a sum type�� ������ �� �ִ�.
// 2+3 = 5 ���� ���δٸ� Ÿ��, Color�̰ų� �ƴϸ� Size.
using fivetype = std::variant<Color, Size>;

// std::pair<A,A>�� std::array<A,2>�� �Ѹ�� ���� ����.
}

//-----------------------------------------------------------------------------
// [Working with std::tuple]
//-----------------------------------------------------------------------------
namespace section4
{
// C++11�� variadic templates���� 2�� �̻��� ���ø� �Ű������� ���� �� �ְ�
// �Ǿ���. �̿� ���� std::tuple<Ts..>�� ������ heterogeneous array �̴�.
// std::tuple<int, double>�� std::pair<int, double>�� ���̰� ����.
// ���� ���, std::tuple<int, int, char, std::string>��
// �ɹ��� a int, another int, a char, and finally a std::string�� ����
// a struct �� �����ϴ�.
// ù��°�� �ι�° ������ Ÿ���� �ٸ��� ������ ��Ÿ�� �ð��� �ε����ϴ�
// operator[](size_t)�� ����� �� ����. �׷��� ������ �ð��� �ش� ���ҿ�
// ������ ���� �����Ϸ����� �˷��ִ� ������� ������ �� �ִ�.
// std::get<0>(t), std::get<1>(t), ...�� ����Ѵ�.
// �̷��� ����� std::tuple�� �ٷ�� ������ �Ǵµ�,
// the homogeneous container�� member functions�� ����ϰ�,
// the heterogeneous algebraic type�� free function templates��
// ����Ѵ�.

// ������ �Ϲ������� tuples�� ������ ���� ���� ���� ���̴�.
// template metaprogramming�� �� �� �ַ� ����Ѵ�.
// �װ� �ƴ϶�� �ַ� �����͵��� �Ѳ����� ��� ó���ϴ� ����� ������ �����
// std::tie �� ����Ѵ�.
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
// std::tie�� arguments�� value�� �ƴ� references�� tuple�� ����� ������ �����
// �����ϴ�. �׷��� ����ó�� "multiple assignment"�� �����ϴ�.
void example2()
{
    std::string s;
    int         i;

    // ���� �󿡼� ������ ���Թ����� ���� ���� ������ �� �ִ�.
    std::tie(s, i) = std::make_tuple("hello", 42);
}
// �ݸ鿡, std::make_tuple(a,b,c...)�� arguments�� references�� �ƴ϶�
// values�� ���纻�� �����Ѵ�.

// c++17�� ������ ���ø� �Ű����� ������ �ܼ��� std::tuple(a,b,c...)�� ����� ��
// �ְ� �ϴµ�, ������ ����ó��
// std::tuple�� std::reference_wrapper arguments�� �����ϰ�
// std::make_tuple�� std::reference_wrapper arguments�� native c++ references��
// decay�Ѵ�.
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
// ������ ��κ��� �Լ��� ���ø��� template metaprogramming���� �����ϴ�.
// std::get<I>(t) : t�� I��° ������ reference�� retrieve�Ѵ�.
// std::tuple_size_v<decltype(t)>: �־��� tuple�� size�� �����Ѵ�.
// std::tuple_element_t<I, decltpye(t)>: �־��� tuple�� I��° ������ Ÿ����
// �����Ѵ�. �Ϲ������� decltype(std::get<I>(t))�� ����Ѵ�.
// std::tuple_cat(t1,t2,t3...): �־��� tuples�� concatenate�Ѵ�.
// std::forward_as_tuple(a,b,c...): �Ϻ� ���޵� ���ҵ��� references�� ����
// a tuple�� �����Ѵ�.
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
// STL class template�� ����ϸ� �� �׽�Ʈ�� STL components�� �����Ͽ�
// ���� �ð��� �ٿ��ش�. ������ �츮�� �ڵ带 less readable or give your types
// too much functionality �ϰ� �� �� �ִ�.
// ���� ���, ���� �忡�� std::array<double, 3>�� Vec3�� �����ϱ� ����
// ����ߴµ� ���ʿ��� operator< �� �߰��Ǿ���.
// the algebraic types (tuple, pair, optional, or variant)�� ��ٷ� API��
// ����ϸ� �ȵȴ�. your own "domain-specific vocabulary" types�� ����
// named classes�� �ۼ��Ѵٸ�, �츮�� �ڵ带 �а� �����ϰ� �����ϴµ� �� ����
// ���� ���̴�.
//

//-----------------------------------------------------------------------------
// [Expressing alternatives with std::variant]
//-----------------------------------------------------------------------------
namespace section6
{
// std::tuple<A,B,C>�� a product type�̰�,
// std::variant<A,B,C>�� a sum type�̴�.
// ���� ���ø� ����.
void example1()
{
    std::variant<int, double> v1;

    v1 = 1; // v1�� ���� int
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 1);
    assert(std::get<int>(v1) == 1);

    v1 = 3.14; // v1�� ���� double
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 3.14);
    assert(std::get<double>(v1) == 3.14);

    assert(std::holds_alternative<int>(v1) == false);
    assert(std::holds_alternative<double>(v1) == true);

    assert(std::get_if<int>(&v1) == nullptr); // ���� v1�� double�̴�.
    assert(*std::get_if<double>(&v1) == 3.14);

    // std::get�� exception�� ���� �� �ִ�.
    // std::get_if�� �����ϸ� nullptr�� �����Ѵ�.

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
// ���� ���ø� ����.
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

// ����ó�� �ۼ��� �� �ִ�.
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

// ����ó�� multiple dispatch �������� ���� �� �ִ�.
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
// std::make_variant<A,B,C>(a) �� �������� �ʴ´�.
// std::make_pair, std::make_tuple��
// std::reference_wrapper<T>�� T&���� decay�Ѵ�.
// ��, std::make_pair(std::ref(a), std::cref(b))��
// std::pair<A&, const B&>�� �����Ѵ�.
// �׷��� std::make_variant<A,B,C>(a)�� �ִٸ� �̷���
// reference-decaying ability �� �־�� ������,
// std::optional, std::any, std::variant�� ���۷��� Ÿ���� ���ҵ���
// ���� ���ϰ� ǥ�صǾ� �ִ�.
// ������ std::variant<std::reference_wrapper<T>,...>�� �����ѵ�,
// ��ó�� wrapper�Ǿ� �ִ� Ÿ���� ���۷��� Ÿ���� �ƴϹǷ� �չ����̴�.

// std::variant<A,B,C>�� �׻� A,B,C �� ��Ȯ�� �ϳ��� Ÿ�� ���� ������
// "valueless by exception"�� �߻��ϸ� ��� ���� ���� ���� �� �ִ�.
struct A {
    A() { throw "haha"; }
};
struct B {
    operator int() { throw "haha"; }
};
struct C {
    C()               = default;
    C& operator=(C&&) = default;
    C(C&&) { throw "haha"; } // move-constructor�� �ݵ�� noexpect�̾�� �Ѵ�.
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

    // emplace ��ſ� ���Թ��� ����ϸ� �̷��� ��Ȳ�� ���� �� �ִ�.
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

    // ������ move-constructor�� ���� �� ����.
    try {
        v1 = C();
    } catch (...) {
    }
    assert(v1.valueless_by_exception());

    assert(v1.index() == size_t(-1));

    // valueless�� v1�� std::visit�� �ϸ� std::bad_variant_access�� ������.
}
}

//-----------------------------------------------------------------------------
// [Delaying initialization with std::optional]
//-----------------------------------------------------------------------------
namespace section9
{
// std::variant�� ������ ���� ����� �� �ִ�.
// "Maybe I have an object, and maybe I don't."
// "maybe I don't"�� state�� the standard tag type�� std::monostate�� ��Ÿ����.
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

// �̷��� ������ variant�� std::optional<T>�� �����Ѵ�.
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

// free (non-member) functions�� ���� �ִ� std::tuple�� std::variant�ʹ�
// �ٸ��� std::optional<T>�� ���� member functions�� ���� �ִ�.
// ���� T�κ��� �񱳿����ڵ��� ����� �� �ִ�. std::nullopt ���´� �׻�
// less than any real value of T �� �����Ѵ�.
//
// bool(o) == o.has_value(), !o == !o.has_value()�� �׻� �����Ѵ�.
// std::optional<bool> o�� ���� o == false�� !o�� �ǹ̰� ���� �ٸ���.
//
// o.value()�� o�� ���� �ִ� ���� reference�� �����ϰ�,
// ���� ���� ���ٸ� std::bad_optional_access�� ������.
// �ݸ鿡 *o�� o�� ���� �ִ� ���� reference�� �����ϰ�, ���� ���� ���ٸ�
// undefined behavior�� ����ų �� �ִ� �ǹ̾��� reference�� �����Ѵ�.
//
// o.value_or(x)�� o�� ���� �ִ� ���� ���纻�� �����ϰų�, ���� ���ٸ�
// a copy of x converted to type T�� �����Ѵ�.
// ���� �׽�Ʈ���� use(limit.value_or(some_default))�� ����� �� �ִ�.

// std::optional<T>�� "maybe a T" �Ӹ� �ƴ϶� "not yet a T"�� ��Ȳ����
// ����� �� �ִ�.
auto make_lambda(int arg)
{
    return [arg](int x) {
        return x + arg;
    };
}
using L = decltype(make_lambda(0));

// Ÿ�� L�� ����Ʈ �����ڰ� ����, �̵� ���� �����ڰ� ����.
static_assert(!std::is_default_constructible_v<L>);
static_assert(!std::is_move_assignable_v<L>);

// �׷��� Ŭ������ �ɹ��� �Ǹ� ����Ʈ �����ڰ� ���� ������
// "not yet a T" ��Ȳ�� �ȴ�.
class ProblematicAdder
{
    L fn_;
};
static_assert(!std::is_default_constructible_v<ProblematicAdder>);

// �̷��� ��Ȳ���� std::optional<L>�� ����Ѵ�.
class Adder
{
    std::optional<L> fn_;
public:
    void setup(int first_arg) { fn_.emplace(make_lambda(first_arg)); }
    int  call(int second_arg)
    {
        // �ѹ��̶� setup()�� ȣ������ ������ ���ܸ� ������.
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
// The variant data type�� recursive data types�� �ƴϱ� ������
// ������ ������ ������ ����.
//     using JSONValue = std::variant<
//         std::nullptr_t, bool, double, std::vector<JSONValue>,
//         std::map<std::string, JSONValue>>;
//
// ������ boost::variant�� ����ϸ� the marker type boost::recursive_variant_��
// ���� ����ó�� ������� Ÿ���� ���� �� �ִ�.
//     using JSONValue = boost::variant<
//         std::nullptr_t,
//         bool,
//         double,
//         std::string,
//         std::vector<boost::recursive_variant_>,
//         std::map<std::string, boost::recursive_variant_>
//     >;
//
// algebraic type ��ſ� forward references to class types�� ����� �� �ִ�.
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
// ��� Ÿ���� ���� �� �ִ� Ÿ������ std::any�� �ִ�.
// std::any Ÿ������ �� �� �ִ� ������� ������ ����.
// ���� ��ü�� ���� �ִ� �� �����.
// ���ο� ��ü�� ���� �ְ�, ���� ��ü�� �Ҹ��Ų��.
// ���� ��ü�� Ÿ���� �����.
// ���� ��ü�� �����Ѵ�.
void example1()
{
    // ����� an empty container�̴�.
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
        // ���ܸ� ������ �ʰ� null pointer�� �����Ѵ�.
    }

    try {
        std::string& s = std::any_cast<std::string&>(a);
        // use(s);
    } catch (const std::bad_any_cast&) {
        // ���� std::bad_any_cast�� ������.
    }
}
// std::any ������ std::visit�� �������� �ʴ´�.
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

    // ���� ��� �ִ� Cat Ŭ���������� �翬�� �۵�������
    assert(a.type() == typeid(Cat));
    assert(std::any_cast<Cat>(&a) != nullptr);

    // Cat�� base Ŭ������ ���ؼ��� �۵����� �ʴ´�.
    assert(a.type() != typeid(Animal));
    assert(std::any_cast<Animal>(&a) == nullptr);

    // ���� void*���� ĳ���ÿ� ���ؼ��� �۵����� �ʴ´�.
    assert(std::any_cast<void*>(&a) == nullptr);
}
}

//-----------------------------------------------------------------------------
// [Type erasure in a nutshell]
//-----------------------------------------------------------------------------
namespace section13
{
// std::any�� ��� �����ұ�? �ٽ��� type erasure�̴�.
// ��� Ÿ�� T�� ���� Ư�� operations�� ���Ͻ��ϰ�, �ٸ� ��� operations��
// �����. ���Ͻ��ϴ� Ư�� operations�� ������ ����. ���� �ִ� ��ü�� ���纻��
// �����Ѵ�. ���� �ִ� ��ü�� ���纻�� �̵� �����Ѵ�. ���� �ִ� ��ü�� typeid��
// ��´�.
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
//        // T�� copy-constructible�̾�� �Ѵ�.
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

// ���⼭ �̵� ���� �����ڴ� �����ߴ�.
// ���� �̷��� ������ noexcept���� swapping pointer�� ������
// "small object optimization"�� ��쿡
// nothrow-move-constructible T�� heap allocation�� ���ϰ�
// not swapping pointer�� �ϸ鼭 T�� stack�� �����Ѵ�.
//
}

//-----------------------------------------------------------------------------
// [std::any and copyability]
//-----------------------------------------------------------------------------
namespace section14
{
// AnyImpl<T>::copy_to�� T�� copy-constructible�̾�� ���ǵǱ� ������
// std::any�� move-only type�� ������ �� ����.
// ���� move-only type�� �����ϰ� ������ �̸� ���δ�
// copy-constructible wrapper�� ����ؾ� �Ѵ�. ���� ���, "Shim":
using Ptr = std::unique_ptr<int>;

template <class T>
struct Shim {
    T get() { return std::move(*t_); }

    template <class... Args>
    Shim(Args&&... args)
        : t_(std::in_place, std::forward<Args>(args)...)
    { }

    // Shim�� move-only object���� �����ϰڴ�.
    Shim(Shim&&)            = default;
    Shim& operator=(Shim&&) = default;
    Shim(const Shim&) { throw "���� ������"; }
    Shim& operator=(const Shim&) { throw "���� ���� ������"; }
private:
    std::optional<T> t_;
};

void test()
{
    Ptr p = std::make_unique<int>(42);

    // Ptr�� move-only�̱� ������ std::any�� ������ �� ����.
    // std::any a = std::move(p);

    // ������ Shim<Ptr>�� �����ϴ�.
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
// std::any���� ��� Ÿ���� �����ϰ� �۵��ϴ� operations�� �����ߴ�.
// �̿� ���� �μ� Ÿ�Ե� A...�� R Ÿ������ ��ȯ�ϴ� operation�� ������ �� �ִ�.
// �̿� �����ϴ� std::function<R(A...)>�� �ִ�.
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

    // Copying std::function�� �׻� �����ִ� ��ü�� ���纻�� �����.
    // �׷��Ƿ� �����ִ� ��ü�� ���¸� �����Ѵ�.
    auto g = f;
    assert(f(-42) == 3);
    assert(f(-42) == 4);
    assert(g(-42) == 3);
    assert(g(-42) == 4);

    // std::anyó�� Ÿ�Կ� ���� ������ �� �� �ִ�.
    // f.target<T>()�� a.type()�� �����ϴ�.
    // f.target<T>()�� std::any_cast<T*>(&a)�� �����ϴ�.
    // ������ ���� ������� �ʴ´�.
    if (f.target_type() == typeid(int (*)(int))) {
        int (*p)(int) = *f.target<int (*)(int)>();
        // use(p);
    } else {
        // ...
    }
}

// ���� �߿��� use-case�� vocabulary type���μ� ��� ���� passing
// "behaviors"�ϴ� �����̴�. �̷��� ��쿡 ���ø����δ� �Ұ����� �� �ִ�.
// ���� ���, �ܺ� ���̺귯���� �ݹ��Լ��� �ѱ�ų�, ���� ���̺귯���� ������ ��
// ȣ������ �ݹ��Լ��� �ʿ��� �ڵ带 �ۼ��ϴ� ��찡 �ִ�.
//
// ���ø��� ȣ��Ǵ� �������� ���ø��� �ڵ尡 �ݵ�� ������ �Ѵ�.
// �ܺο��� �ش� ���ø��� ������ �ʱ� ������ �ܺο��� �ݹ��Լ��� ���� �� ����.
template <class F>
void templated_for_each(std::vector<int>& v, F f)
{
    for (int& i : v) {
        f(i);
    }
}
// �ݸ鿡 type-erasure�� std::function�� ���� �� �ִ�.
// type_erased_for_each has a stable ABI and a fixed address.
// It can be called with only its declaration in scope.
extern void type_erased_for_each(std::vector<int>&, std::function<void(int)>);
}

//-----------------------------------------------------------------------------
// [std::funciton, copyability, and allocation]
//-----------------------------------------------------------------------------
namespace section16
{
// std::any, std::function�� ��� copy-constructible ��ü�� ������ �� �ִ�.
// ��, move-only types�� ���������� ������ �� ����. wrapper�� ���� ���������δ�
// �����ϴ�.
// std::future<T>, std::unique_ptr<T>���� move-only types�� ĸ���ϴ� lamdas��
// ����ϴ� ��쿡 lamdas�� ��� move-only�̴�.
// �׷��� move-only types�� ĸ���� �� shared_ptr�� ĸ���ϰ� �Ѵ�.
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
// std::any�� std::function ��� customization allocation�� ������� �ʴ´�.
}