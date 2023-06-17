#pragma once

// ch4에서 필요한 헤더파일들은 다음과 같다.
#include <string>
#include <algorithm>
#include <cassert>
#include <array>
//-----------------------------------------------------------------------------
// [The notion of ownership]
//-----------------------------------------------------------------------------
namespace section1
{
// "Object A owns object B" 는 객체 A가 객체 B의 lifetime을 관리한다는 뜻이다.
// 그럴려면 A가 B의 construction, copying, moving, destruction을
// 관리해야 한다. 그래서 A를 사용하는 유저는 B에 대한 관리를 생각할 필요가 없다.
// 가장 간단한 방법은 B를 A의 member variable로 두는 것이다.
// 다음 예시를 보자.
void simple_way()
{
    struct B {
    };
    struct owning_A {
        B b_;
    };
    struct non_owning_A {
        B& b_;
    };
    B b;
    // a1 takes ownership of [a copy of] b.
    owning_A a1 {b};
    // a2 merely hods a reference to b.
    // a2 doesn't own b.
    non_owning_A a2 {b};
}

// 또다른 방법은
// A가 B의 포인터를 갖고, ~A()와 copy and move operations를 잘 정의하는 것이다.
void another_way()
{
    struct B {
    };

    struct owning_A {
        B* b_;
        explicit owning_A(B* b)
            : b_(b)
        { }
        owning_A(owning_A&& other) noexcept
            : b_(other.b_)
        {
            other.b_ = nullptr;
        }
        owning_A& operator=(owning_A&& other) noexcept
        {
            delete b_;
            b_       = other.b_;
            other.b_ = nullptr;
            return *this;
        }

        ~owning_A() { delete b_; }
    };

    struct non_owning_A {
        B* b_;
    };

    B* b = new B;

    // a1 takes ownership of *b
    owning_A a1 {b};

    // a2 merely holds a pointer to *b.
    // a2 doesn't own *b.
    non_owning_A a2 {b};
}
// ownership 이란 개념은 RAII(Resource Allocation Is Initialization)와
// 밀접하다. RAII는 Resource Freeing Is Destruction로 기억하는 것이 좋다.

// the standard container classes의 목표는
// a container가 특정한 데이터들의 소유권을 갖고 사용자에게는
// 접근성을 제공하는 것이다.
// 한편, iterators는 절대로 데이터 원소들의 소유권을 가질 수 없다.
}

//-----------------------------------------------------------------------------
// [The simplest container: std::array<T,N>]
//-----------------------------------------------------------------------------
namespace section2
{
// 가장 단순한 컨테이너는 std::array<T,N> 이다.
// T는 원소의 타입이고, N은 원소의 개수이다.
// std::array<char, 3> arr {{42,43,44}};

// C-style array는 operator[] 말고는,
// 어떠한 built-in operations을 제공하지 않는다.
// 따라서 다른 C-style array를 다른 array에 할당하거나 비교하는 작업 등을
// 할때, 이전 장의 iterator-based-algorithms을 이용해서 코드를 작성해야 한다.
// 예를 들어 다음과 같다.
void example_c_style()
{
    std::string c_style[4] = {"the", "quick", "brown", "fox"};
    assert(c_style[2] == "brown");
    assert(std::size(c_style) == 4);
    assert((std::distance(std::begin(c_style), std::end(c_style)) == 4));

    // 복제 연산자 operator=를 제공하지 않으므로
    std::string other[4];
    std::copy(std::begin(c_style), std::end(c_style), std::begin(other));

    // Swapping은 제공된다.
    using std::swap;
    swap(c_style, other);

    // 비교 연산자를 제공하지 않으므로
    assert(c_style != other); // address comparison
    assert(std::equal(c_style, c_style + 4, other, other + 4));
    assert(
        !std::lexicographical_compare(c_style, c_style + 4, other, other + 4));
}

// std::array는 중괄호를 두번 넣어야 하는데, 외부 괄호는 std::array<T,N>을
// 위한 것이고, 내부 괄호는 inner data member인 T[N]을 위한 것이다.
void example_std_array()
{
    std::array<std::string, 4> arr = {
        {"the", "quick", "brown", "fox"}
    };
    assert(arr[2] == "brown");

    assert(arr.size() == 4);
    assert((std::distance(arr.begin(), arr.end()) == 4));

    std::array<std::string, 4> other;
    other = arr;

    using std::swap;
    swap(arr, other);

    assert(&arr != &arr); // different addresses
    assert(arr == other); // lexicographically equal
    assert(arr >= other);
}

// 또다른 장점으로 std::array는 function으로부터 리턴될 수 있다.
// c-style array는 리턴될 수 없다.
// auto cross_product(const int (&a)[3], const int (&b)[3]) -> int[3];
std::array<int, 3> cross_product(const std::array<int, 3>& a,
                                 const std::array<int, 3>& b)
{
    return {
        {
         a[1] * b[2] - a[2] * b[1],
         a[2] * b[0] - a[0] * b[2],
         a[0] * b[1] - a[1] * b[0],
         }
    };
}
// 또한, std::vector<int[3]> 은 불가능하지만
// std::vector<std::array<int,3>>은 가능하다.
}
