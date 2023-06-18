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
    return /* clang-format off */
    {{  a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    }}; /* clang-format on */
}
// 또한 컨테이너의 원소로 집어넣을 수 있다.
// 즉, std::vector<int[3]> 은 불가능하지만
// std::vector<std::array<int,3>>은 가능하다.

// 자주 array를 저장하거나 리턴해야 한다면
// array를 wrap하는 class를 고려해야 적합할 것이다.
// array of three integers를 encapsulate하는 class를 생각하자.
struct Vec3 {
    int x, y, z;
    Vec3(int x, int y, int z)
        : x(x),
          y(y),
          z(z)
    { }
};
bool operator==(const Vec3& a, const Vec3& b)
{
    return std::tie(a.x, a.y, a.z) == std::tie(b.x, b.y, b.z);
}
bool operator!=(const Vec3& a, const Vec3& b) { return !(a == b); }
Vec3 cross_product(const Vec3& a, const Vec3& b)
{
    return /* clang-format off */
    {   a.y *b.z - a.z *b.y,
        a.z *b.x - a.x *b.z,
        a.x *b.y - a.y *b.x,
    }; /* clang-format on */
}
// Vec3 class의 초기화가 쉬워지고 의미가 안 맞는 comparison
// operator가 정의되지 않아서 실수하지 않는다.

// sizeof (std::array<int,100>) == sizeof (int[100])
// == 100 * sizeof (int) 가 성립한다.
// 지역변수로서 스택에 크기가 큰 array를 만드는 실수를 하면 안된다.
void dont_do_this()
{
    // 4 megabytes 크기의 스택 공간을 할당한다.
    // segfault가 발생할 수 있다.
    // int arr[1'000'000];
}
void dont_do_this_either()
{
    // 위의 문제를 해결해주지 않는다.
    // std::array<int, 1'000'000> arr;
}

// 이렇게 gigantic arrays는 std::vector를 사용해야 한다.
}

//-----------------------------------------------------------------------------
// [The workhorse: std::vector<T>]
//-----------------------------------------------------------------------------
namespace section3
{
// std::vector는 스택이 아닌 힙 메모리의 연속된 데이터 원소들의 배열을 나타낸다.
// std::array와는 다르게 gigantic array를 안전하게 생성할 수 있고 동적으로
// 크기를 조절할 수 있다.
// std::vector에는 중요한 attribute가 있는데, 바로 capacity 이다.
// capacity는 항상 size 이상의 값을 갖는다. 현재 vector가 얼마 만큼의
// 원소를 가질 수 있는지 나타낸다.
// std::vector<char> v {42, 43, 44};
// v.reserve(8);
// => v의 ptr은 {42,43,44,?,?,?,?,?}의 첫 원소를 가리키고,
// v의 size = 3, v의 capacity = 8 이다.

// resizeability를 제외하고 vector는 array와 유사하게
// copyable, comparable (by T::operator<) 하다.

// 일반적으로 말하자면, std::vector는 전체 standard library에서
// 가장 흔하게 사용되는 container이다. 많은 원소를 저장하거나
// 얼만큼의 원소를 저장해야하는지 모를 때, 첫번째 생각으로 resizeable,
// efficiency of a contiguous array한 vector를 사용해야 한다.

// contiguous arrays는 cache-friendliness 한 locality를 가졌기 때문에
// 하드웨어 상에서 가장 효율적인 data structures 이다.
// vector를 .begin()부터 .end()까지 통행하는 것은
// 메모리를 순서대로 통행하는 것과 같다. 그래서 하드웨어는 다음의 메모리가
// 무엇인지 예측할 수 있어서 CPU는 필요한 데이터들을 미리 준비할 수 있다.
// 반면에 linked list는 .begin()부터 .end()까지 통행하는 것은
// 메모리를 임의로 통행하는 것과 같아서 CPU's cahe 안에 미리 데이터를 준비할
// 수가 없다.
}

//-----------------------------------------------------------------------------
// [Resizing a std::vector]
//-----------------------------------------------------------------------------
namespace section4
{
// vector의 two primitive operations: .resize() and .reserve().
template <typename T>
inline void destroy_n_elements(T* p, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        p[i].~T();
    }
}
template <typename T>
class vector
{
    T*     ptr_      = nullptr;
    size_t size_     = 0;
    size_t capacity_ = 0;
public:
    // ...
    void reserve(size_t c)
    {
        if (capacity_ >= c) {
            // do nothing
            return;
        }

        T* new_ptr = (T*) malloc(c * sizeof(T));

        for (size_t i = 0; i< size_; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>) {
                // 만약 원소들이 예외없이 move가 가능하다면,
                ::new (&new_ptr[i]) T(std::move(ptr_[i]));
            } else {
                // 만약 원소들이 예외를 던질 수 있다면,
                // move는 안전하지 않아서 원소들의 복제본을 만들어야 한다.
                try {
                    ::new (&new_ptr[i]) T(ptr_[i]);
                } catch (...) {
                    destroy_n_elements(new_ptr, i);
                    free(new_ptr);
                    throw;
                }
            }
        }
    }
};
// 여기서 malloc과 new의 차이점에 대해 알아보자.
// malloc은 힙에 메모리를 할당만 하고, new는 여기에 객체도 초기화한다.
// malloc은 해당 타입(T*)으로 캐스팅되야 하는 void* 를 리턴하고,
// new는 해당 타입(T)의 T*를 리턴한다.
// malloc으로 할당하면 free로 할당 해제를 해야하고,
// new로 할당하면 delete로 할당 해제해야 한다.
// free는 할당된 메모리를 할당 해제하는 c/c++의 표준 라이브러리의 함수이고,
// delete는 new에 의해 할당된 메모리를 할당 해제하는 c++의 연산자이다.
// free는 메모리 할당만 해제하고, delete는 메모리 할당을 해제하기 전에 개체의
// 소멸자를 호출한다.
// 저수준 시스템 프로그래밍에는 수동 메모리 관리하는 free/malloc을 사용하고
// 자동 메모리 관리에 사용하는 new/delete는 사용이 편하지만 오버헤드가 크다.
}
