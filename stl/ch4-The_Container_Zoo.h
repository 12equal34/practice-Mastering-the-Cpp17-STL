#pragma once

// ch4에서 필요한 헤더파일들은 다음과 같다.
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
    // assert(c_style != other); // address comparison, deprecated
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

    assert(&arr != &other); // different addresses
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
// std::array와는 다르게 gigantic array를 안전하게 생성할 수 있고
// reallocation을 통해서 동적으로 크기를 조절할 수 있다.

// std::vector에는 중요한 attribute가 있는데, 바로 capacity 이다.
// capacity는 재할당할 필요없이 가질 수 있는 최대 원소의 수를 의미한다.
// capacity는 항상 size 이상의 값을 갖는다.
// 다음 예시를 보자.
// std::vector<char> v {42, 43, 44};
// v.reserve(8); //이제부터 v는 최소한 8*sizeof(char) bytes의 메모리를 갖는다.
// => v의 ptr은 {42,43,44,?,?,?,?,?}의 첫 원소를 가리키고,
// v의 size = 3, v의 capacity = 8 이다.
// 이후에 만약 재할당이 일어난다면, v의 capacity는 k*capacity()가 된다.
// (여기서 msvc는 k=1.5이고, libc++와 libstdc++은 k=2이다.)

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
// resize(c)는 capacity >= c 이면 무시되고 c > capacity이면 재할당한다.
template <typename T>
class vector
{
    T*     ptr_      = nullptr;
    size_t size_     = 0;
    size_t capacity_ = 0;
public:
    // ...

    // resize의 구현은 reserve와 비슷하다.
    void reserve(size_t c)
    {
        if (capacity_ >= c) {
            // do nothing
            return;
        }
        // 여기서 malloc이 fail하는 경우는 무시했다.
        T* new_ptr = (T*)malloc(c * sizeof(T));

        for (size_t i = 0; i < size_; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>) {
                // 만약 원소들이 예외없이 move가 가능하다면,
                ::new (&new_ptr[i]) T(std::move(ptr_[i]));
            } else {
                // 만약 원소들이 예외를 던질 수 있다면,
                // move는 안전하지 않아서 원소들의 복제본을 만들어야 한다.
                // 그래서 move constructor는 noexcept이여야 한다.
                try {
                    ::new (&new_ptr[i]) T(ptr_[i]);
                } catch (...) {
                    destroy_n_elements(new_ptr, i);
                    free(new_ptr);
                    throw;
                }
            }
        }
        destroy_n_elements(ptr_, size_);

        // 위의 for-loop는 allocator를 생각하지 않고
        // std::uninitialized_copy(a,b,c)를 통해 구현할 수도 있다.
        /*std::conditional_t<std::is_nothrow_move_constructible_v<T>,
                           std::move_iterator<T*>, T*>
            first(ptr_);
        try {
            std::uninitialized_copy(first, first + size_, new_ptr);
        } catch (...) {
            free(new_ptr);
            throw;
        }
        std::destroy(ptr_, ptr_ + size_);*/

        free(ptr_);
        ptr_      = new_ptr;
        capacity_ = c;
    }

    ~vector()
    {
        destroy_n_elements(ptr_, size_);
        free(ptr_);
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

// vec.resize(s)는 vector의 size를 바꾸며, 크기가 작아지면 잘리는
// 원소들의 소멸자를 호출하여 잘라내고, 크기가 커지면 원소들의 디폴트 생성자를
// 호출하여 추가한다. 그래서 the vector의 size는 s가 된다.
// 만약 s > vec.capacity() 이면, the vector는 .reserve()처럼 재할당한다.
// 따라서 재할당하는 경우, vec[0]의 주소가 변경된다. 그래서 이전 포인터는
// "dangling pointers"가 되기 때문에 iterator도 invaild 된다.
// 이러한 현상을 iterator invalidation 이라고 부른다.
void cases_of_iterator_invalidation()
{
    // case 1
    std::vector<int> v    = {3, 1, 4};
    auto             iter = v.begin();
    v.reserve(6);
    // iter is invalidated!

    // case 2
    v = std::vector {3, 1, 4};
    std::copy(v.begin(), v.end(), std::back_inserter(v));
    // v = {3, 1, 4, 3, 1, 4} 가 될 것 같지만
    // 처음 insertion 다음에 v의 재할당이 일어나기 때문에,
    // 다음 insertion부터 dangling iterators이 된다.

    // case 3
    auto end = v.end(); // 매번 .end()를 호출하지 않기 위함.
    for (auto it = v.begin(); it != end; ++it) {
        if (*it == 4) {
            v.erase(it); // 전체 반복 중에 원소를 지우면 안된다.
        }
    }

    // fix the bug of case 3
    for (auto it = v.begin(); it != v.end();) {
        if (*it == 4) {
            it = v.erase(it);
        } else {
            ++it;
        }
    }
    // But it's much more efficient to use the
    // erase-remove idiom.
    v.erase(
        std::remove_if(v.begin(), v.end(), [](auto&& elt) { return elt == 4; }),
        v.end());
}
}

//-----------------------------------------------------------------------------
// [Inserting and erasing in a std::vector]
//-----------------------------------------------------------------------------
namespace section5
{
// vec.push_back(t)는 벡터의 끝에 원소 t의 복제본을 추가한다.
// 벡터의 앞에 원소를 추가하는 .push_front()를 생각할 수 있지만,
// 이를 효율적으로 구현할 수 없기 때문에 굳이 만들지 않았다.
// vec.emplace_back(args...)는 a perfect-forwarding variadic
// function template 이다. 벡터의 끝에 t의 복제본을 추가하는 것이 아니라
// T(args...)를 호출하는 것처럼 객체 T를 벡터의 끝에 생성한다.

// push_back, emplace_back 가 reallocation을 발생시키면, 단순히
// capacity를 capacity()+1로 증가하는 것이 아니라, k*capacity()로
// geometric resizing을 한다. (msvc에서 k=1.5, libc++,libstdc++에서는
// k=2이다.) 그래서 평균적으로 상수시간 알고리즘이 된다.

// vec.insert(it,t)는 it의 자리에 원소 t의 복제본을 추가한다.
// it은 추가한 원소의 반복자가 된다.
// 만약 it == vec.end()이면 push_back(t)이고,
// 만약 it == vec.begin()이면 push_front(t)라고 생각할 수 있다.
// 원소의 끝이 아닌 자리에 insert하면 자리를 만들기 위해서 각 원소들을
// 옆으로 이동시키기 때문에 비용이 비쌀 수 있다.

// .insert()의 오버로드 버전이 있는데, 다음 예시를 보자.
void insert_overloads()
{
    std::vector<int> v = {1, 2};
    std::vector<int> w = {5, 6};

    v.insert(v.begin() + 1, 3);
    assert((v == std::vector {1, 3, 2}));

    v.insert(v.end() - 1, 3, 4);
    assert((v == std::vector {1, 3, 4, 4, 4, 2}));

    v.insert(v.begin() + 3, w.begin(), w.end());
    assert((v == std::vector {1, 3, 4, 5, 6, 4, 4, 2}));

    v.insert(v.begin(), {7, 8});
    assert((v == std::vector {7, 8, 1, 3, 4, 5, 6, 4, 4, 2}));
}

// vec.emplace(it, args...)도 존재한다. 이는 emplace_back 버전의 insert 라고
// 생각할 수 있다.

// vec.erase(it)는 벡터의 중간에 있는 it의 원소를 제거한다.
// vec.erase(a,b)는 자신의 부분구간 [a,b)의 원소들을 제거한다.
// 벡터의 끝에 있는 원소를 제거하기 위해서 .erase(.end()-1)가 아니라,
// vec.pop_back()을 사용한다.
// push_back()과 pop_back()만을 a dynamically growable stack을 구현할 수 있다.
}

//-----------------------------------------------------------------------------
// [Pitfalls with vector<bool>]
//-----------------------------------------------------------------------------
namespace section6
{
// std::vector는 std::vector<bool>을 specialization으로 구현했다.
// std::vector<bool> v {
// true,false,true,false,false,false,true,false,true,true};
// // 1010 0010, 11
// v.reserve(60);
// => 메모리: 0xA2, 0xC0, ?, ?, ...
//    0xA2 = 1010(A) 0010(2)
//    0xC0 = 1100(C) 0000(0)
//    size    : 10
//    capacity: 64
// std::vector<bool>은 메모리 할당을 byte단위로 하기 때문에 capacity가 8의
// 배수가 된다.
// 또한, bit 단위의 포인터는 존재하지 않기 때문에
// vector<bool>::operator[]는 bool&가 아니라 bool도 아닌,
// bool로 암시적 변환이 가능한 std::vector<bool>::reference를 리턴한다.
// 이러한 타입을 proxy type 또는 proxy reference라고 부른다.
// 한편, vector<bool>::oerator[] const는 공식적으로 bool을 리턴해야 하지만
// 몇몇 라이브러리는 (libc++) a proxy type for operator[] const를 리턴하기
// 때문에 non-portable하다. 가능하다면 vector<bool>는 피하는게 좋다.
void example()
{
    std::vector<bool> vb = {true, false, true, false};

    // vector<bool>::reference의 유일한 맴버 함수
    vb[3].flip();
    assert(vb[3] == true);

    // 다음은 컴파일 에러가 나온다.
    // bool& oops = vb[0];

    // auto는 bool로 연역하지 않기 때문에 잘못된 코드다.
    auto ref = vb[0];
    assert((!std::is_same_v<decltype(ref), bool>));
    assert(sizeof vb[0] > sizeof(bool));

    if (sizeof std::as_const(vb)[0] == sizeof(bool)) {
        puts("Your library vendor is libstdc++ or Visual Studio");
    } else {
        puts("Your library vendor is libc++");
    }
}
}

//-----------------------------------------------------------------------------
// [Pitfalls with non-noexcept move constructors]
//-----------------------------------------------------------------------------
namespace section7
{
// move constructor과 swap function를 정의할 때,
// 반드시 noexcept을 붙여야 한다.
// 그래야 벡터의 원소로 포함될 때, move가 copy가 되는 현상을 피할 수 있다.
// 다음 예시를 보자.
struct Bad {
    int x = 0;
    Bad() = default;
    Bad(const Bad&) { puts("copy Bad"); }
    Bad(Bad&&) { puts("move Bad"); }
};
struct Good {
    int x  = 0;
    Good() = default;
    Good(const Good&) { puts("copy Good"); }
    Good(Good&&) noexcept { puts("move Good"); }
};
class ImplicitlyGood
{
    std::string x;
    Good        y;
};
class ImplicitlyBad
{
    std::string x;
    Bad         y;
};

template <class T>
void test_resizing()
{
    std::vector<T> vec(1);
    vec.resize(vec.capacity() + 1);
}
void test()
{
    test_resizing<Good>();
    test_resizing<Bad>();
    test_resizing<ImplicitlyGood>();
    test_resizing<ImplicitlyBad>();
}
// move가 copy되는 이유는
// resize의 구현에서 throw를 던질 수 있으면
// 원소를 move하다가 예외를 던지면 안전하지 않기 때문에
// 원소의 복제본을 copy하기 때문이다.
}

//-----------------------------------------------------------------------------
// [The speedy hybrid: std::deque<T>]
//-----------------------------------------------------------------------------
namespace section8
{
// std::deque는 begin과 end에서 빠른 삽입과 삭제가 가능한
// an indexed sequence container이다. 그리고 양 끝에서 삽입과 삭제는
// pointer invalidation을 유발하지 않는다.
// 개별 할당된 fixd-size subarrays의 sequence와 인덱스 접근을 할 때 필요한
// 책갈피 역할을 하는 포인터로 구현한다. (chunkwise contiguous하다.)
//
// 다음 예시를 보자.
// std::deque<char> dq {42, 43, 44};
// start : 14
// size  : 3
// capacity: 5
// double-ended array of subarrays:
// {?, char(*)[8], char(*)[8], ?, ...}
// ptr은 첫번째 배열 포인터 이전 포인터를 가리키고 (첫번째 물음표)
// begin은 첫번째 배열 포인터를 가리키고
// end는 두번째 배열 포인터 다음 포인터를 가리킨다. (두번째 물음표)
// 개별 할당된 subarrays에 대해 각각
// 첫번째 배열 포인터 char(*)[8]는 {?,?,?,?,?,?,42,43}를 가리키고,
// 두번째 배열 포인터 char(*)[8]는 {44,?,?,?,?,?,?,?}를 가리킨다.

// 양 끝에서의 insert나 erase는 iterator invalid을 유발하지 않는다.
// 중간에서의 insert나 erase는 iterator invalid를 유발한다.
void test()
{
    std::vector<int> vec   = {1, 2, 3, 4};
    std::deque<int>  deq   = {1, 2, 3, 4};
    int*             vec_p = &vec[2];
    int*             deq_p = &deq[2];
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i);
        deq.push_back(i);
    }
    assert(vec_p != &vec[2]);
    assert(deq_p == &deq[2]);
}
// std::deque<T>는 어떠한 specialization이 없기 때문에
// 어떠한 타입 T이든지 uniform public interface를 유지할 수 있다.

// 큰 단점으로 std::deque<T>의 iterator는 increment or dereference를 할 때,
// 포인터 배열로부터 navigate 하므로 많은 비용이 든다.
// 따라서 컨테이너의 양 끝에서 빠른 삽입 또는 삭제가 필요한 경우가 아니라면,
// 여전히 std::vector를 사용해야 한다.
}

//-----------------------------------------------------------------------------
// [A particular set of skills: std::list<T>]
//-----------------------------------------------------------------------------
namespace section9
{
// std::list<T>는 a doubly linked list in memory를 나타낸다.
// 다음 예시를 보자.
// std::list<char> lst {42,43,44};
// lst에는 {tail, head, size=3}을 갖고 있고
// tail은 첫번째 노드를, head는 마지막 노드를 가리킨다.
// node1: {prev=nullptr, next=&node2, value=42}
// node2: {prev=&node1, next=&node3, value=43}
// node3: {prev=&node2, next=nullptr, value=44}

// std::list<T>::iterator는 bidirectional iterator이다.
// 즉, n번째 원소에 접근할 때 O(n)시간이 걸린다.

// std::list는 vector나 deque처럼 contiguous data structure가 아니기 때문에
// 보다 비효율적이다. 왜냐하면 랜덤하게 할당된 메모리에 접근하기 때문에
// 연속적인 메모리 블록보다 캐시하기 어렵기 때문이다.
// 따라서 일반적으로 std::list는 undesirable 해야 한다.
}

//-----------------------------------------------------------------------------
// [What are the special skills of std::list?]
//-----------------------------------------------------------------------------
namespace section10
{
// std::list는 iterator invalidation이 발생하지 않는다.
// push_back(v), push_front(v)는 항상 상수시간이 걸린다.
// 데이터의 "resize" or "move"가 전혀 필요없다.

// 연속적인 메모리 구조를 벗어나는 게 필요한 연산들은 vector보다 훨씬 유리하다.
// 즉, 데이터들을 insert, splice, merge 하는데 상수시간이 든다.

// lst.splice(it, otherlst)는 리스트 otherlst의 모든 원소를 lst의 it 자리에
// 이어 붙인다. 이는 lst.insert(it++,other_elt)를 반복 호출한 결과와 같다.
// 이후 otherlst.size() == 0 이 된다.

// lst.merge(otherlst)는 오직 pointer swaps만을 사용하여
// otherlst를 lst에 merge sort 한다.
void example1()
{
    std::list<int> a = {3, 6};
    std::list<int> b = {1, 2, 3, 5, 6};

    a.merge(b);
    assert(b.empty());
    assert((a == std::list {1, 2, 3, 3, 5, 6, 6}));
}

// std::remove_if와 std::unique는 연속 컨테이너에서 사용된다.
// 이 알고리즘들은 삭제할 원소들을 컨테이너의 뒤로 shuffle한다.
// std::list는 곧바로 원소들을 삭제할 수 있으므로 다음 맴버함수를 제공한다.
// lst.remove(v)는 v와 동일한 원소들을 모두 삭제(remove-erase)한다.
// lst.remove_if(p)는 p(e)를 만족하는 원소들을 모두 삭제한다.
// lst.unique()는 연속적으로 배열한 동일한 원소들 중 첫번째 원소를 제외하고
// 모두 삭제한다.
// lst.sort()는 lst를 정렬한다. std::sort(a,b)는 random iterators를 받으므로
// bidirectional iterator인 std::list::iterator는 사용하지 못한다.
// (참고로 std::sort는 intro sort를 한다.
// intro sort는 퀵 정렬, 힙 정렬, 삽입 정렬로 이루어져 있다.
// 최악과 최선의 시간 복잡도에 대해 O(nlogn)이 성립한다.)
// lst.sort()은 항상 컨테이너의 모든 원소에 대해 정렬하므로,
// 특정 구간에 대한 정렬은 pointer swaps을 통해 실행할 수 있다.
void example2()
{
    std::list<int> lst   = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    auto           begin = std::next(lst.begin(), 2);
    auto           end   = std::next(lst.end(), -2);

    // 부분구간 {4,1,5,9,2}를 lst로부터 잘라내어 정렬한다.
    std::list<int> sub;
    sub.splice(sub.begin(), lst, begin, end);
    sub.sort();

    // 정렬된 부분구간 {1,2,4,5,9}를 다시 lst에 이어 붙인다.
    lst.splice(end, sub);
    assert(sub.empty());

    assert((lst == std::list {3, 1, 1, 2, 4, 5, 9, 6, 5}));
}
}

//-----------------------------------------------------------------------------
// [Roughing it with std::forward_list<T>]
//-----------------------------------------------------------------------------
namespace section11
{
// std::forward_list<T>는 a linked list 이다.
// 다음 예시를 보자.
// std::forward_list<char> flst {42, 43, 44};
// flst는 {head=&node1, size=3}을 갖는다.
// node1: {next=&node2, value=42}
// node2: {next=&node3, value=43}
// node3: {next=nullptr, value=44}

// flst.before_begin()은 첫번째 원소 이전의 iter를 리턴한다.
// flst.erase_after(it)은 it 다음의 원소 하나를 삭제한다.
// flst.insert_after(it, v)는 it 다음으로 원소 v를 추가한다.
// flst.splice_after(it, otherflst)는 it 다음으로 otherflst의 모든 원소를
// 이어 붙인다.
}

//-----------------------------------------------------------------------------
// [Abstracting with std::stack<T> and std::queue<T>]
//-----------------------------------------------------------------------------
namespace section12
{
// std::stack<T,Ctr>은 T 타입의 원소들을 갖는 스택이고,
// 원소들의 컨테이너는 Ctr이 관리한다.
// 예를 들어, stack<T,vector<T>>는 vector가 원소들을 관리하고,
// stack<T,list<T>>는 list가 원소를 관리한다.
// Ctr의 디폴트는 std::deque<T>이다.
// 왜냐하면 deque는 컨테이너의 양 끝에서 원소를 추가하거나 삭제할 때
// 재할당이 일어나지 않기 때문이다.

// push는 컨테이너의 push_back에 대응된다.
// pop는 컨테이너의 pop_back에 대응된다.
// top은 컨테이너의 back에 대응된다.
// accessor로 size와 empty가 있다.
void example()
{
    std::stack<int> stk;
    stk.push(3);
    stk.push(1);
    stk.push(4);
    assert(stk.top() == 4);
    stk.pop();
    assert(stk.top() == 1);
    stk.pop();
    assert(stk.top() == 3);

    std::stack<int> a, b;
    a.push(3);
    a.push(1);
    a.push(4);
    b.push(2);
    b.push(7);
    assert(a != b);
    assert(a.top() < b.top()); // 4 < 7
    assert(a > b);             // 3 > 2
}

// 비슷하게 std::queue<T,Ctr>이 제공된다.
// push_back은 컨테이너의 push_back에 대응되고,
// pop_front는 컨테이너의 pop_front에 대응된다.
// accessor로 front, back, size, empty가 있다.
// 컨테이너의 디폴트로 std::deque<T>를 사용한다.

// deque<T>로 queue를 구현할 때, front에서 pop을 할지 back에서 pop을 할지
// 정해야 하는데, 이는 실제 생활에서 기다리는 줄에 설때, 앞에 있는 사람이 나가고
// 뒤로 줄 서는 것처럼, front에서 pop하고 back에서 push한다.
}

//-----------------------------------------------------------------------------
// [The useful adaptor: std::priority_queue<T>]
//-----------------------------------------------------------------------------
namespace section13
{
// [참고]
// the max-heap property: 구간 a의 모든 원소에 대해
// a[i] > a[2i+1] && a[i] > a[2i+2]를 만족한다.
// a[0]가 maximum이 된다.

// 이전 장에서 heap algorithms: make_heap, push_heap, pop_heap 에 대해서
// 공부를 하였다. 이러한 알고리즘들로 the max-heap property를 갖는 구간을
// 만들 수 있다.
//
// the max-heap property를 불변성으로 갖는 데이터 구조를
// priority queue 라고 한다.
//
// 보통 우선순위 큐는 binary tree를 통해 묘사가 되는데,
// 반드시 pointer-based tree structure 이어야 the max-heap property를
// 구현할 수 있는 것은 아니다. 이전 장에서 배열 구조에서 구현할 수 있음을
// 보였다. 따라서 더 효율적인 컨테이너로 vector<T>를 디폴트로 사용한다.

// std::priority_queue<T, Ctr, Cmp> 에서 디폴트로 컨테이너 vector<T>를
// 사용하는데, 원소를 추가 및 삭제할 때마다 the max-heap property를 유지시키기
// 위해서 매번 원소들을 이동해야 하기 때문에 queue<T>를 사용하지 않는 것이다.
// 또한, Cmp의 디폴트로 std::less<T>를 사용한다.

// 개념적으로 컨테이너의 front 원소는 heap의 top 원소를 의미하기 때문에
// heap의 top은 가장 큰 원소가 된다.
// .push(v)는 std::push_heap(v)인 것처럼 v의 복제본을 삽입한다.
// .top()은 top 원소의 reference를 리턴한다. 이는 ctr.front()에 대응된다.
// .pop()은 top 원소를 std::pop_heap()한다.

void example()
{
    std::priority_queue<int>                                   max_heap;
    std::priority_queue<int, std::vector<int>, std::greater<>> min_heap;

    for (int v : {3, 1, 4, 1, 5, 9}) {
        max_heap.push(v);
        min_heap.push(v);
    }

    assert(max_heap.top() == 9);
    assert(min_heap.top() == 1);
}
}

//-----------------------------------------------------------------------------
// [The trees: std::set<T> and std::map<K,V>]
//-----------------------------------------------------------------------------
namespace section14
{
// std::set<T,Cmp>는 a unique set for any T that implements operator<의
// 인터페이스를 제공한다. set의 구현은 a red-black tree를 통해 구현한다.
// 이는 balanced binary search trees 를 의미한다. 평균적으로 insert와 find은
// 항상 O(log n) 시간이 걸린다.
// 다음 예시를 보자.
// std::set<char> s {42,43,44};
// s는 {begin=&node2, root=&node1, size=3}을 갖는다.
// node1: {left=&node2,right=&node3,parent=s.root,rb,43}
// node2: {left=nullptr,right=nullptr,parent=node1.left,rb,43}
// node3: {left=nullptr,right=nullptr,parent=node1.right,rb,44}

// 이진 검색 트리는 정렬된 순서로 원소들을 저장하기 때문에
// push_front or push_back은 의미가 없다. 따라서 원소 v를 추가할 때는
// s.insert(v)를 사용한다.
void example1()
{
    std::set<int> s;
    for (int i : {3, 1, 4, 1, 5}) {
        s.insert(i);
    }
    // 정렬된 순서로 저장된다.
    assert((s == std::set {1, 3, 4, 5}));
    auto it = s.begin();
    assert(*it == 1);

    s.erase(4);
    s.erase(it);
    assert((s == std::set {3, 5}));
}

// s.insert(i)는 std::pair<set<T>::iterator,bool> ret를 리턴하는데
// ret.first는 방금 추가했거나 원래 있던 v 원소를 가리키는 반복자이고,
// ret.second는 true이면 방금 추가한 것이고, false면 v가 원래 있었음을 의미한다.
void example2()
{
    std::set<int> s;
    auto [it1, b1] = s.insert(1);
    assert(*it1 == 1 && b1 == true);

    auto [it2, b2] = s.insert(2);
    assert(*it2 == 2 && b2 == true);

    auto [it3, b3] = s.insert(1);
    assert(*it3 == 1 && b3 == false);
}

// set의 원소들은 오름차순으로 저장되었기 때문에
// for-loop로 s.begin()부터 std::prev(s.end())까지 오름차순되어 있다.
// 그래서 The tree-based structure of a set은
// std::find와 std::lower_bound를 사용할 수 있다.
// 하지만 비효율적이기 때문에 트리구조의 루트 노드에 바로 접근 가능한
// std::set의 맴버 함수를 사용해야 한다.
// For std::find(s.begin(), s.end(), v), use s.find(v).
// For std::lower_bound(s.begin(), s.end(), v), use s.lower_bound(v).
// For std::upper_bound(s.begin(), s.end(), v), use s.upper_bound(v).
// For std::count(s.begin(), s.end(), v), use s.count(v).
// For std::equal_range(s.begin(), s.end(), v), use s.equal_range(v).
// 여기서 set은 deduplicated이기 때문에 s.count(v)는 0 또는 1를 리턴할 것이다.

// std::map<K,V>는 원소 key-value pair를 갖는 set이며,
// std::set< pair<K,V>,K::operator< >라고 생각할 수 있다.
// std::map은 operator[]를 제공하는데, 다음 예시를 보자.
void indexing_with_operator()
{
    std::map<std::string, std::string> m;

    // m[key] = value 를 통해서 원소 key-value pair를 map에 insert한다.
    m["hello"] = "world";
    m["quick"] = "brown";
    m["hello"] = "dolly";
    assert(m.size() == 2);

    // 다음처럼 m[key]를 호출하기만 해도
    // key-default value pair을 map에 삽입해버리기 때문에 조심해야 한다.
    assert(m["literally"] == "");
    assert(m.size() == 3);
    // 여기서 만약 value 타입 V가 not default-constructible이면
    // 컴파일 에러가 나타난다.

    // 또한 operator[] const 버전이 없기 때문에 const map은 사용할 수 없다.

    // 만약 value 타입 V가 not default-constructible이라면 아래와 같이
    // 코드를 작성해야 한다.
    using Pair = decltype(m)::value_type;
    if (m.find("hello") == m.end()) {
        // 원소 k가 존재하는지 판별하기 위해 m.find(k)를 사용한다.
        // 원소를 삽입하기 위해 m.insert(kv) or m.emplace(k,v)를 사용한다.
        m.insert(Pair {"hello", "dolly"});
        // or equivalently
        m.emplace("hello", "dolly");
    }
}
// std::map과 std::set은 based on trees of pointers 이다.
// 그래서 cache-unfriendly하기 때문에 이러한 데이터 구조를 피하고
// std::unordered_map과 std::unordered_set을 선호해야 한다.
}

//-----------------------------------------------------------------------------
// [A note about transparent comparators]
//-----------------------------------------------------------------------------
namespace section15
{
// 이전 절에서 m.find("hello")라고 했는데,
// "hello"는 const char[6] type이기 때문에 decltype(m)::key_type인
// std::string으로 캐스팅 시켜서 decltype(m)::key_compare인
// std::less<std::string> 으로 호출하게 된다.
// 즉, m.find("hello")는 첫번째 parameter가 std::string이기 때문에
// std::string("hello")로 암시적 생성을 하여 find의 argument로 넘어간다.
// 하지만 이러한 과정은 비효율적이다.
// 따라서 다음과 같은 typedef is_transparent가 정의된
// heterogeneous operator()를 정의하게 된다.
// std::less<>는 다음과 같이 구현한다.
struct MagicLess {
    using is_transparent = std::true_type;

    template <class T, class U>
    bool operator()(T&& t, U&& u) const
    {
        return std::forward<T>(t) < std::forward<U>(u);
    }
};

void test()
{
    std::map<std::string, std::string, MagicLess>   m;
    std::map<std::string, std::string, std::less<>> m2;

    // find는 std::string을 생성하지 않고,
    // const std::string& 과 const char* 를 매개변수로 갖는
    // string의 비교 연산자 operator< 로 계산된다.
    auto it = m2.find("hello");
}
// 이러한 magic은 std::map의 구현 전체에서 일어나는데,
// find는 특별하게 is_transparent를 체크하여 이에 따라 행동을 바꾼다.
// 비슷하게 count, lower_bound, upper_bound, equal_range도 그렇다.
// 그러나 이상하게 erase는 m.erase(it)과 m.erase(v)를 구분하기 어렵기
// 때문에 그렇게 구현하지 않았다.
// 그래서 heterogeneous comparison를 사용하는 삭제는 다음과 같이 한다.
// auto [begin, end] = m.equal_range("hello");
// m.erase(begin,end);
}

//-----------------------------------------------------------------------------
// [Oddballs: std::multiset<T> and std::multimap<K,V>]
//-----------------------------------------------------------------------------
namespace section16
{
// std::multiset<T,Cmp>는 std::set<T,Cmp>와 동일하며 duplicate elements를
// 허용한다. std::multimap<K,V,Cmp>도 마찬가지이다.
// 다음 예시를 보자.
void example()
{
    std::multimap<std::string, std::string> mm;
    mm.emplace("hello", "world");
    mm.emplace("quick", "brown");
    mm.emplace("hello", "dolly");
    assert(mm.size() == 3);

    auto it    = mm.begin();
    using Pair = decltype(mm)::value_type;
    assert((*(it++) == Pair("hello", "world")));
    assert((*(it++) == Pair("hello", "dolly")));
    assert((*(it++) == Pair("quick", "brown")));

    std::multimap<std::string, std::string> mm2 = {
        {"hello", "world"},
        {"quick", "brown"},
        {"hello", "dolly"},
    };
    assert(mm2.count("hello") == 2);
    mm2.erase("hello"); // "hello"를 키로 갖는 모든 원소를 삭제한다.
    assert(mm2.count("hello") == 0);
}
// multi 버전은 operator[]가 존재하지 않는다.
}

//-----------------------------------------------------------------------------
// [Moving elements without moving them]
//-----------------------------------------------------------------------------
namespace section17
{
// list가 데이터들을 자르고 이어 붙이는 알고리즘이 있는 것처럼
// the tree-based containers 도 비슷한 알고리즘을 갖고 있다.
void example1()
{
    std::map<std::string, std::string> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
    std::map<std::string, std::string> otherm = {
        {"hello", "dolly"},
        {"sad",   "clown"},
    };

    // 중복된 키를 갖는 원소는 transfer 되지 않는다.
    m.merge(otherm);
    // 이러한 원소는 원래 컨테이너에 여전히 남게 된다.
    assert((otherm == decltype(m) {
                          {"hello", "dolly"},
    }));

    assert((m == decltype(m) {
                     {"hello", "world"},
                     {"quick", "brown"},
                     {"sad",   "clown"},
    }));
}

// m.update(otherm)은 m.insert(otherm.begin(), otherm.end())와 동등하다.
void example2()
{
    std::map<std::string, std::string> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
    std::map<std::string, std::string> otherm = {
        {"hello", "dolly"},
        {"sad",   "clown"},
    };

    using Pair = decltype(m)::value_type;

    // 개별 원소들을 추출해서 삽입하는 알고리즘은 다음과 같다.
    auto nh1 = otherm.extract("sad");
    assert(nh1.key() == "sad" && nh1.mapped() == "clown");
    // 기존 원소에 해당하는 키가 없으므로 삽입이 성공한다.
    auto [it2, inserted2, nh2] = m.insert(std::move(nh1));
    assert(*it2 == Pair("sad", "clown") && inserted2 == true && nh2.empty());

    auto nh3 = otherm.extract("hello");
    assert(nh3.key() == "hello" && nh3.mapped() == "dolly");
    // 기존 원소에 해당하는 키가 있으므로 삽입이 실패한다.
    auto [it4, inserted4, nh4] = m.insert(std::move(nh3));
    assert(*it4 == Pair("hello", "world") && inserted4 == false &&
           !nh4.empty());

    // 기존 원소에 해당하는 키가 있어서 삽입이 실패했을 때,
    // 기존 원소를 새로운 원소로 덮어씌울 수 있다.
    m.insert_or_assign(nh4.key(), nh4.mapped());

    // 보다 쉬운 방법은 기존의 원소를 삭제한 뒤, 삽입하는 것이다.
    m.erase(it4);
    m.insert(std::move(nh4));
}

// extract이 리턴하는 타입은 "node handle"이다.
// 이는 nh.key()와 nh.mapped() 접근자를 제공하는데
// 이로부터 실제 데이터를 복제하거나 이동하지 않고 그대로 추출하거나
// 조작하여 재삽입시킬 수 있는 것이다.
// 다음과 같이 std::transform으로 데이터를 manipulation을 할 수 있다.
void example3()
{
    std::map<std::string, std::string> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
    assert(m.begin()->first == "hello");
    assert(std::next(m.begin())->first == "quick");

    auto nh = m.extract("quick");
    std::transform(nh.key().begin(), nh.key().end(), nh.key().begin(),
                   ::toupper);
    m.insert(std::move(nh));

    assert(m.begin()->first == "QUICK");
    assert(std::next(m.begin())->first == "hello");
}
}

//-----------------------------------------------------------------------------
// [The hashes: std::unordered_set<T> and std::unordered_map<K,V>]
//-----------------------------------------------------------------------------
namespace section18
{
// [참고]
// 임의의 크기를 가진 데이터를 고정된 크기의 값으로 대응시키는 함수를
// hash function이라고 정의한다.
// hash function이 리턴하는 값을 hash value 또는 단순히 hashes 라고 한다.
// hash values을 인덱스로 사용하는 a fixed-size table을 hash table이라 부른다.
// hash table에 색인하기 위해 hash function을 사용하는 것을 hashing or
// scatter storage addressing 이라고 부른다.
// 이렇게 데이터를 저장하는 방식은 검색 및 저장이 상수시간에 가능하다.
// 하지만 이는 키와 hash function 간의 통계적인 속성에 의존하기 때문에
// 최악의 경우 O(n)이 되고, 다른 원소임에도 불구하고 같은 해시값을 가지는
// hash collision이 일어날 수 있다.

// std::unordered_set은 a chained hash table을 나타낸다.
// 이는 buckets의 고정 크기의 배열을 갖고,
// 각 bucket은 데이터 원소들의 a singly linked list이다.
// 새로운 원소가 컨테이너에 추가될 때, 새로운 원소 값에 대한 "hash" 값과 연관된
// the linked list (bucket)으로 집어 넣는다.
// 내부적으로 데이터를 linked lists에 보관하므로 forward iterator를 제공한다.
// 또한 데이터를 정렬되지 않은 순서로 보관한다.
// insert, erase, find 는 최소 O(1)과 최악으로 O(n) 수행시간을 갖기 때문에
// 최적화가 필요한 경우에 해시 함수를 잘 정의해서 unordered_set을 사용한다.
// std::unordered_map<K,V>는 std::unordered_set<pair<K,V>>와 비슷하다.

// std::unordered_set, std::unordered_map은 Hash, KeyEqual 템플릿 매개변수를
// 받는다. 디폴트로 std::hash<K>, std::equal_to<K>를 전달한다.

}

//[Load factor and bucket lists]
// 다음은 buckets을 관리하는 알고리즘들은 다음과 같다.
// s.bucket_count() returns the current number of buckets in the array.
// 
// s.bucket(v) returns the index i of the bucket in which you'd find the
// element v, if it existed in this unordered_set.
// 
// s.bucket_size(i) returns the number of elements in the ith bucket.Observe
// that invariably s.count(v) <= s.bucket_size(s.bucket(v)).
// 
// s.load_factor() returns s.size() / s.bucket_count() as a float value.
// 
// s.rehash(n) increases(or decreases) the size of the bucket array to exactly
// n.

// [Where does the memory come from?]
//
// 여기까지 각각의 컨테이너를 설명했다.
// 그렇다면 컨테이너는 어디로부터 메모리를 할당받는 것일까?
// std::array를 제외한 모든 컨테이너는 optional 템플릿 매개변수로 allocator를
// 받는다. 이는 컨테이너의 재할당이 일어나거나, 새로운 노드를 링크드 리스트에
// 추가하는 등 메모리가 어디로부터 할당되는 지를 가리킨다.
// 디폴트로 std::allocator<T>를 사용한다.
// allocator와 관련해서 ch8에서 공부한다.