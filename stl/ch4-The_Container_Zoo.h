#pragma once

// ch4���� �ʿ��� ������ϵ��� ������ ����.
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
// "Object A owns object B" �� ��ü A�� ��ü B�� lifetime�� �����Ѵٴ� ���̴�.
// �׷����� A�� B�� construction, copying, moving, destruction��
// �����ؾ� �Ѵ�. �׷��� A�� ����ϴ� ������ B�� ���� ������ ������ �ʿ䰡 ����.
// ���� ������ ����� B�� A�� member variable�� �δ� ���̴�.
// ���� ���ø� ����.
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

// �Ǵٸ� �����
// A�� B�� �����͸� ����, ~A()�� copy and move operations�� �� �����ϴ� ���̴�.
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
// ownership �̶� ������ RAII(Resource Allocation Is Initialization)��
// �����ϴ�. RAII�� Resource Freeing Is Destruction�� ����ϴ� ���� ����.

// the standard container classes�� ��ǥ��
// a container�� Ư���� �����͵��� �������� ���� ����ڿ��Դ�
// ���ټ��� �����ϴ� ���̴�.
// ����, iterators�� ����� ������ ���ҵ��� �������� ���� �� ����.
}

//-----------------------------------------------------------------------------
// [The simplest container: std::array<T,N>]
//-----------------------------------------------------------------------------
namespace section2
{
// ���� �ܼ��� �����̳ʴ� std::array<T,N> �̴�.
// T�� ������ Ÿ���̰�, N�� ������ �����̴�.
// std::array<char, 3> arr {{42,43,44}};

// C-style array�� operator[] �����,
// ��� built-in operations�� �������� �ʴ´�.
// ���� �ٸ� C-style array�� �ٸ� array�� �Ҵ��ϰų� ���ϴ� �۾� ����
// �Ҷ�, ���� ���� iterator-based-algorithms�� �̿��ؼ� �ڵ带 �ۼ��ؾ� �Ѵ�.
// ���� ��� ������ ����.
void example_c_style()
{
    std::string c_style[4] = {"the", "quick", "brown", "fox"};
    assert(c_style[2] == "brown");
    assert(std::size(c_style) == 4);
    assert((std::distance(std::begin(c_style), std::end(c_style)) == 4));

    // ���� ������ operator=�� �������� �����Ƿ�
    std::string other[4];
    std::copy(std::begin(c_style), std::end(c_style), std::begin(other));

    // Swapping�� �����ȴ�.
    using std::swap;
    swap(c_style, other);

    // �� �����ڸ� �������� �����Ƿ�
    // assert(c_style != other); // address comparison, deprecated
    assert(std::equal(c_style, c_style + 4, other, other + 4));
    assert(
        !std::lexicographical_compare(c_style, c_style + 4, other, other + 4));
}

// std::array�� �߰�ȣ�� �ι� �־�� �ϴµ�, �ܺ� ��ȣ�� std::array<T,N>��
// ���� ���̰�, ���� ��ȣ�� inner data member�� T[N]�� ���� ���̴�.
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

// �Ǵٸ� �������� std::array�� function���κ��� ���ϵ� �� �ִ�.
// c-style array�� ���ϵ� �� ����.
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
// ���� �����̳��� ���ҷ� ������� �� �ִ�.
// ��, std::vector<int[3]> �� �Ұ���������
// std::vector<std::array<int,3>>�� �����ϴ�.

// ���� array�� �����ϰų� �����ؾ� �Ѵٸ�
// array�� wrap�ϴ� class�� ����ؾ� ������ ���̴�.
// array of three integers�� encapsulate�ϴ� class�� ��������.
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
// Vec3 class�� �ʱ�ȭ�� �������� �ǹ̰� �� �´� comparison
// operator�� ���ǵ��� �ʾƼ� �Ǽ����� �ʴ´�.

// sizeof (std::array<int,100>) == sizeof (int[100])
// == 100 * sizeof (int) �� �����Ѵ�.
// ���������μ� ���ÿ� ũ�Ⱑ ū array�� ����� �Ǽ��� �ϸ� �ȵȴ�.
void dont_do_this()
{
    // 4 megabytes ũ���� ���� ������ �Ҵ��Ѵ�.
    // segfault�� �߻��� �� �ִ�.
    // int arr[1'000'000];
}
void dont_do_this_either()
{
    // ���� ������ �ذ������� �ʴ´�.
    // std::array<int, 1'000'000> arr;
}

// �̷��� gigantic arrays�� std::vector�� ����ؾ� �Ѵ�.
}

//-----------------------------------------------------------------------------
// [The workhorse: std::vector<T>]
//-----------------------------------------------------------------------------
namespace section3
{
// std::vector�� ������ �ƴ� �� �޸��� ���ӵ� ������ ���ҵ��� �迭�� ��Ÿ����.
// std::array�ʹ� �ٸ��� gigantic array�� �����ϰ� ������ �� �ְ�
// reallocation�� ���ؼ� �������� ũ�⸦ ������ �� �ִ�.

// std::vector���� �߿��� attribute�� �ִµ�, �ٷ� capacity �̴�.
// capacity�� ���Ҵ��� �ʿ���� ���� �� �ִ� �ִ� ������ ���� �ǹ��Ѵ�.
// capacity�� �׻� size �̻��� ���� ���´�.
// ���� ���ø� ����.
// std::vector<char> v {42, 43, 44};
// v.reserve(8); //�������� v�� �ּ��� 8*sizeof(char) bytes�� �޸𸮸� ���´�.
// => v�� ptr�� {42,43,44,?,?,?,?,?}�� ù ���Ҹ� ����Ű��,
// v�� size = 3, v�� capacity = 8 �̴�.
// ���Ŀ� ���� ���Ҵ��� �Ͼ�ٸ�, v�� capacity�� k*capacity()�� �ȴ�.
// (���⼭ msvc�� k=1.5�̰�, libc++�� libstdc++�� k=2�̴�.)

// resizeability�� �����ϰ� vector�� array�� �����ϰ�
// copyable, comparable (by T::operator<) �ϴ�.

// �Ϲ������� �����ڸ�, std::vector�� ��ü standard library����
// ���� ���ϰ� ���Ǵ� container�̴�. ���� ���Ҹ� �����ϰų�
// ��ŭ�� ���Ҹ� �����ؾ��ϴ��� �� ��, ù��° �������� resizeable,
// efficiency of a contiguous array�� vector�� ����ؾ� �Ѵ�.

// contiguous arrays�� cache-friendliness �� locality�� ������ ������
// �ϵ���� �󿡼� ���� ȿ������ data structures �̴�.
// vector�� .begin()���� .end()���� �����ϴ� ����
// �޸𸮸� ������� �����ϴ� �Ͱ� ����. �׷��� �ϵ����� ������ �޸𸮰�
// �������� ������ �� �־ CPU�� �ʿ��� �����͵��� �̸� �غ��� �� �ִ�.
// �ݸ鿡 linked list�� .begin()���� .end()���� �����ϴ� ����
// �޸𸮸� ���Ƿ� �����ϴ� �Ͱ� ���Ƽ� CPU's cahe �ȿ� �̸� �����͸� �غ���
// ���� ����.
}

//-----------------------------------------------------------------------------
// [Resizing a std::vector]
//-----------------------------------------------------------------------------
namespace section4
{
// vector�� two primitive operations: .resize() and .reserve().
template <typename T>
inline void destroy_n_elements(T* p, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        p[i].~T();
    }
}
// resize(c)�� capacity >= c �̸� ���õǰ� c > capacity�̸� ���Ҵ��Ѵ�.
template <typename T>
class vector
{
    T*     ptr_      = nullptr;
    size_t size_     = 0;
    size_t capacity_ = 0;
public:
    // ...

    // resize�� ������ reserve�� ����ϴ�.
    void reserve(size_t c)
    {
        if (capacity_ >= c) {
            // do nothing
            return;
        }
        // ���⼭ malloc�� fail�ϴ� ���� �����ߴ�.
        T* new_ptr = (T*)malloc(c * sizeof(T));

        for (size_t i = 0; i < size_; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>) {
                // ���� ���ҵ��� ���ܾ��� move�� �����ϴٸ�,
                ::new (&new_ptr[i]) T(std::move(ptr_[i]));
            } else {
                // ���� ���ҵ��� ���ܸ� ���� �� �ִٸ�,
                // move�� �������� �ʾƼ� ���ҵ��� �������� ������ �Ѵ�.
                // �׷��� move constructor�� noexcept�̿��� �Ѵ�.
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

        // ���� for-loop�� allocator�� �������� �ʰ�
        // std::uninitialized_copy(a,b,c)�� ���� ������ ���� �ִ�.
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
// ���⼭ malloc�� new�� �������� ���� �˾ƺ���.
// malloc�� ���� �޸𸮸� �Ҵ縸 �ϰ�, new�� ���⿡ ��ü�� �ʱ�ȭ�Ѵ�.
// malloc�� �ش� Ÿ��(T*)���� ĳ���õǾ� �ϴ� void* �� �����ϰ�,
// new�� �ش� Ÿ��(T)�� T*�� �����Ѵ�.
// malloc���� �Ҵ��ϸ� free�� �Ҵ� ������ �ؾ��ϰ�,
// new�� �Ҵ��ϸ� delete�� �Ҵ� �����ؾ� �Ѵ�.
// free�� �Ҵ�� �޸𸮸� �Ҵ� �����ϴ� c/c++�� ǥ�� ���̺귯���� �Լ��̰�,
// delete�� new�� ���� �Ҵ�� �޸𸮸� �Ҵ� �����ϴ� c++�� �������̴�.
// free�� �޸� �Ҵ縸 �����ϰ�, delete�� �޸� �Ҵ��� �����ϱ� ���� ��ü��
// �Ҹ��ڸ� ȣ���Ѵ�.
// ������ �ý��� ���α׷��ֿ��� ���� �޸� �����ϴ� free/malloc�� ����ϰ�
// �ڵ� �޸� ������ ����ϴ� new/delete�� ����� �������� ������尡 ũ��.

// vec.resize(s)�� vector�� size�� �ٲٸ�, ũ�Ⱑ �۾����� �߸���
// ���ҵ��� �Ҹ��ڸ� ȣ���Ͽ� �߶󳻰�, ũ�Ⱑ Ŀ���� ���ҵ��� ����Ʈ �����ڸ�
// ȣ���Ͽ� �߰��Ѵ�. �׷��� the vector�� size�� s�� �ȴ�.
// ���� s > vec.capacity() �̸�, the vector�� .reserve()ó�� ���Ҵ��Ѵ�.
// ���� ���Ҵ��ϴ� ���, vec[0]�� �ּҰ� ����ȴ�. �׷��� ���� �����ʹ�
// "dangling pointers"�� �Ǳ� ������ iterator�� invaild �ȴ�.
// �̷��� ������ iterator invalidation �̶�� �θ���.
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
    // v = {3, 1, 4, 3, 1, 4} �� �� �� ������
    // ó�� insertion ������ v�� ���Ҵ��� �Ͼ�� ������,
    // ���� insertion���� dangling iterators�� �ȴ�.

    // case 3
    auto end = v.end(); // �Ź� .end()�� ȣ������ �ʱ� ����.
    for (auto it = v.begin(); it != end; ++it) {
        if (*it == 4) {
            v.erase(it); // ��ü �ݺ� �߿� ���Ҹ� ����� �ȵȴ�.
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
// vec.push_back(t)�� ������ ���� ���� t�� �������� �߰��Ѵ�.
// ������ �տ� ���Ҹ� �߰��ϴ� .push_front()�� ������ �� ������,
// �̸� ȿ�������� ������ �� ���� ������ ���� ������ �ʾҴ�.
// vec.emplace_back(args...)�� a perfect-forwarding variadic
// function template �̴�. ������ ���� t�� �������� �߰��ϴ� ���� �ƴ϶�
// T(args...)�� ȣ���ϴ� ��ó�� ��ü T�� ������ ���� �����Ѵ�.

// push_back, emplace_back �� reallocation�� �߻���Ű��, �ܼ���
// capacity�� capacity()+1�� �����ϴ� ���� �ƴ϶�, k*capacity()��
// geometric resizing�� �Ѵ�. (msvc���� k=1.5, libc++,libstdc++������
// k=2�̴�.) �׷��� ��������� ����ð� �˰����� �ȴ�.

// vec.insert(it,t)�� it�� �ڸ��� ���� t�� �������� �߰��Ѵ�.
// it�� �߰��� ������ �ݺ��ڰ� �ȴ�.
// ���� it == vec.end()�̸� push_back(t)�̰�,
// ���� it == vec.begin()�̸� push_front(t)��� ������ �� �ִ�.
// ������ ���� �ƴ� �ڸ��� insert�ϸ� �ڸ��� ����� ���ؼ� �� ���ҵ���
// ������ �̵���Ű�� ������ ����� ��� �� �ִ�.

// .insert()�� �����ε� ������ �ִµ�, ���� ���ø� ����.
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

// vec.emplace(it, args...)�� �����Ѵ�. �̴� emplace_back ������ insert ���
// ������ �� �ִ�.

// vec.erase(it)�� ������ �߰��� �ִ� it�� ���Ҹ� �����Ѵ�.
// vec.erase(a,b)�� �ڽ��� �κб��� [a,b)�� ���ҵ��� �����Ѵ�.
// ������ ���� �ִ� ���Ҹ� �����ϱ� ���ؼ� .erase(.end()-1)�� �ƴ϶�,
// vec.pop_back()�� ����Ѵ�.
// push_back()�� pop_back()���� a dynamically growable stack�� ������ �� �ִ�.
}

//-----------------------------------------------------------------------------
// [Pitfalls with vector<bool>]
//-----------------------------------------------------------------------------
namespace section6
{
// std::vector�� std::vector<bool>�� specialization���� �����ߴ�.
// std::vector<bool> v {
// true,false,true,false,false,false,true,false,true,true};
// // 1010 0010, 11
// v.reserve(60);
// => �޸�: 0xA2, 0xC0, ?, ?, ...
//    0xA2 = 1010(A) 0010(2)
//    0xC0 = 1100(C) 0000(0)
//    size    : 10
//    capacity: 64
// std::vector<bool>�� �޸� �Ҵ��� byte������ �ϱ� ������ capacity�� 8��
// ����� �ȴ�.
// ����, bit ������ �����ʹ� �������� �ʱ� ������
// vector<bool>::operator[]�� bool&�� �ƴ϶� bool�� �ƴ�,
// bool�� �Ͻ��� ��ȯ�� ������ std::vector<bool>::reference�� �����Ѵ�.
// �̷��� Ÿ���� proxy type �Ǵ� proxy reference��� �θ���.
// ����, vector<bool>::oerator[] const�� ���������� bool�� �����ؾ� ������
// ��� ���̺귯���� (libc++) a proxy type for operator[] const�� �����ϱ�
// ������ non-portable�ϴ�. �����ϴٸ� vector<bool>�� ���ϴ°� ����.
void example()
{
    std::vector<bool> vb = {true, false, true, false};

    // vector<bool>::reference�� ������ �ɹ� �Լ�
    vb[3].flip();
    assert(vb[3] == true);

    // ������ ������ ������ ���´�.
    // bool& oops = vb[0];

    // auto�� bool�� �������� �ʱ� ������ �߸��� �ڵ��.
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
// move constructor�� swap function�� ������ ��,
// �ݵ�� noexcept�� �ٿ��� �Ѵ�.
// �׷��� ������ ���ҷ� ���Ե� ��, move�� copy�� �Ǵ� ������ ���� �� �ִ�.
// ���� ���ø� ����.
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
// move�� copy�Ǵ� ������
// resize�� �������� throw�� ���� �� ������
// ���Ҹ� move�ϴٰ� ���ܸ� ������ �������� �ʱ� ������
// ������ �������� copy�ϱ� �����̴�.
}

//-----------------------------------------------------------------------------
// [The speedy hybrid: std::deque<T>]
//-----------------------------------------------------------------------------
namespace section8
{
// std::deque�� begin�� end���� ���� ���԰� ������ ������
// an indexed sequence container�̴�. �׸��� �� ������ ���԰� ������
// pointer invalidation�� �������� �ʴ´�.
// ���� �Ҵ�� fixd-size subarrays�� sequence�� �ε��� ������ �� �� �ʿ���
// å���� ������ �ϴ� �����ͷ� �����Ѵ�. (chunkwise contiguous�ϴ�.)
//
// ���� ���ø� ����.
// std::deque<char> dq {42, 43, 44};
// start : 14
// size  : 3
// capacity: 5
// double-ended array of subarrays:
// {?, char(*)[8], char(*)[8], ?, ...}
// ptr�� ù��° �迭 ������ ���� �����͸� ����Ű�� (ù��° ����ǥ)
// begin�� ù��° �迭 �����͸� ����Ű��
// end�� �ι�° �迭 ������ ���� �����͸� ����Ų��. (�ι�° ����ǥ)
// ���� �Ҵ�� subarrays�� ���� ����
// ù��° �迭 ������ char(*)[8]�� {?,?,?,?,?,?,42,43}�� ����Ű��,
// �ι�° �迭 ������ char(*)[8]�� {44,?,?,?,?,?,?,?}�� ����Ų��.

// �� �������� insert�� erase�� iterator invalid�� �������� �ʴ´�.
// �߰������� insert�� erase�� iterator invalid�� �����Ѵ�.
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
// std::deque<T>�� ��� specialization�� ���� ������
// ��� Ÿ�� T�̵��� uniform public interface�� ������ �� �ִ�.

// ū �������� std::deque<T>�� iterator�� increment or dereference�� �� ��,
// ������ �迭�κ��� navigate �ϹǷ� ���� ����� ���.
// ���� �����̳��� �� ������ ���� ���� �Ǵ� ������ �ʿ��� ��찡 �ƴ϶��,
// ������ std::vector�� ����ؾ� �Ѵ�.
}

//-----------------------------------------------------------------------------
// [A particular set of skills: std::list<T>]
//-----------------------------------------------------------------------------
namespace section9
{
// std::list<T>�� a doubly linked list in memory�� ��Ÿ����.
// ���� ���ø� ����.
// std::list<char> lst {42,43,44};
// lst���� {tail, head, size=3}�� ���� �ְ�
// tail�� ù��° ��带, head�� ������ ��带 ����Ų��.
// node1: {prev=nullptr, next=&node2, value=42}
// node2: {prev=&node1, next=&node3, value=43}
// node3: {prev=&node2, next=nullptr, value=44}

// std::list<T>::iterator�� bidirectional iterator�̴�.
// ��, n��° ���ҿ� ������ �� O(n)�ð��� �ɸ���.

// std::list�� vector�� dequeó�� contiguous data structure�� �ƴϱ� ������
// ���� ��ȿ�����̴�. �ֳ��ϸ� �����ϰ� �Ҵ�� �޸𸮿� �����ϱ� ������
// �������� �޸� ��Ϻ��� ĳ���ϱ� ��Ʊ� �����̴�.
// ���� �Ϲ������� std::list�� undesirable �ؾ� �Ѵ�.
}

//-----------------------------------------------------------------------------
// [What are the special skills of std::list?]
//-----------------------------------------------------------------------------
namespace section10
{
// std::list�� iterator invalidation�� �߻����� �ʴ´�.
// push_back(v), push_front(v)�� �׻� ����ð��� �ɸ���.
// �������� "resize" or "move"�� ���� �ʿ����.

// �������� �޸� ������ ����� �� �ʿ��� ������� vector���� �ξ� �����ϴ�.
// ��, �����͵��� insert, splice, merge �ϴµ� ����ð��� ���.

// lst.splice(it, otherlst)�� ����Ʈ otherlst�� ��� ���Ҹ� lst�� it �ڸ���
// �̾� ���δ�. �̴� lst.insert(it++,other_elt)�� �ݺ� ȣ���� ����� ����.
// ���� otherlst.size() == 0 �� �ȴ�.

// lst.merge(otherlst)�� ���� pointer swaps���� ����Ͽ�
// otherlst�� lst�� merge sort �Ѵ�.
void example1()
{
    std::list<int> a = {3, 6};
    std::list<int> b = {1, 2, 3, 5, 6};

    a.merge(b);
    assert(b.empty());
    assert((a == std::list {1, 2, 3, 3, 5, 6, 6}));
}

// std::remove_if�� std::unique�� ���� �����̳ʿ��� ���ȴ�.
// �� �˰������ ������ ���ҵ��� �����̳��� �ڷ� shuffle�Ѵ�.
// std::list�� ��ٷ� ���ҵ��� ������ �� �����Ƿ� ���� �ɹ��Լ��� �����Ѵ�.
// lst.remove(v)�� v�� ������ ���ҵ��� ��� ����(remove-erase)�Ѵ�.
// lst.remove_if(p)�� p(e)�� �����ϴ� ���ҵ��� ��� �����Ѵ�.
// lst.unique()�� ���������� �迭�� ������ ���ҵ� �� ù��° ���Ҹ� �����ϰ�
// ��� �����Ѵ�.
// lst.sort()�� lst�� �����Ѵ�. std::sort(a,b)�� random iterators�� �����Ƿ�
// bidirectional iterator�� std::list::iterator�� ������� ���Ѵ�.
// (����� std::sort�� intro sort�� �Ѵ�.
// intro sort�� �� ����, �� ����, ���� ���ķ� �̷���� �ִ�.
// �־ǰ� �ּ��� �ð� ���⵵�� ���� O(nlogn)�� �����Ѵ�.)
// lst.sort()�� �׻� �����̳��� ��� ���ҿ� ���� �����ϹǷ�,
// Ư�� ������ ���� ������ pointer swaps�� ���� ������ �� �ִ�.
void example2()
{
    std::list<int> lst   = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    auto           begin = std::next(lst.begin(), 2);
    auto           end   = std::next(lst.end(), -2);

    // �κб��� {4,1,5,9,2}�� lst�κ��� �߶󳻾� �����Ѵ�.
    std::list<int> sub;
    sub.splice(sub.begin(), lst, begin, end);
    sub.sort();

    // ���ĵ� �κб��� {1,2,4,5,9}�� �ٽ� lst�� �̾� ���δ�.
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
// std::forward_list<T>�� a linked list �̴�.
// ���� ���ø� ����.
// std::forward_list<char> flst {42, 43, 44};
// flst�� {head=&node1, size=3}�� ���´�.
// node1: {next=&node2, value=42}
// node2: {next=&node3, value=43}
// node3: {next=nullptr, value=44}

// flst.before_begin()�� ù��° ���� ������ iter�� �����Ѵ�.
// flst.erase_after(it)�� it ������ ���� �ϳ��� �����Ѵ�.
// flst.insert_after(it, v)�� it �������� ���� v�� �߰��Ѵ�.
// flst.splice_after(it, otherflst)�� it �������� otherflst�� ��� ���Ҹ�
// �̾� ���δ�.
}

//-----------------------------------------------------------------------------
// [Abstracting with std::stack<T> and std::queue<T>]
//-----------------------------------------------------------------------------
namespace section12
{
// std::stack<T,Ctr>�� T Ÿ���� ���ҵ��� ���� �����̰�,
// ���ҵ��� �����̳ʴ� Ctr�� �����Ѵ�.
// ���� ���, stack<T,vector<T>>�� vector�� ���ҵ��� �����ϰ�,
// stack<T,list<T>>�� list�� ���Ҹ� �����Ѵ�.
// Ctr�� ����Ʈ�� std::deque<T>�̴�.
// �ֳ��ϸ� deque�� �����̳��� �� ������ ���Ҹ� �߰��ϰų� ������ ��
// ���Ҵ��� �Ͼ�� �ʱ� �����̴�.

// push�� �����̳��� push_back�� �����ȴ�.
// pop�� �����̳��� pop_back�� �����ȴ�.
// top�� �����̳��� back�� �����ȴ�.
// accessor�� size�� empty�� �ִ�.
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

// ����ϰ� std::queue<T,Ctr>�� �����ȴ�.
// push_back�� �����̳��� push_back�� �����ǰ�,
// pop_front�� �����̳��� pop_front�� �����ȴ�.
// accessor�� front, back, size, empty�� �ִ�.
// �����̳��� ����Ʈ�� std::deque<T>�� ����Ѵ�.

// deque<T>�� queue�� ������ ��, front���� pop�� ���� back���� pop�� ����
// ���ؾ� �ϴµ�, �̴� ���� ��Ȱ���� ��ٸ��� �ٿ� ����, �տ� �ִ� ����� ������
// �ڷ� �� ���� ��ó��, front���� pop�ϰ� back���� push�Ѵ�.
}

//-----------------------------------------------------------------------------
// [The useful adaptor: std::priority_queue<T>]
//-----------------------------------------------------------------------------
namespace section13
{
// [����]
// the max-heap property: ���� a�� ��� ���ҿ� ����
// a[i] > a[2i+1] && a[i] > a[2i+2]�� �����Ѵ�.
// a[0]�� maximum�� �ȴ�.

// ���� �忡�� heap algorithms: make_heap, push_heap, pop_heap �� ���ؼ�
// ���θ� �Ͽ���. �̷��� �˰����� the max-heap property�� ���� ������
// ���� �� �ִ�.
//
// the max-heap property�� �Һ������� ���� ������ ������
// priority queue ��� �Ѵ�.
//
// ���� �켱���� ť�� binary tree�� ���� ���簡 �Ǵµ�,
// �ݵ�� pointer-based tree structure �̾�� the max-heap property��
// ������ �� �ִ� ���� �ƴϴ�. ���� �忡�� �迭 �������� ������ �� ������
// ������. ���� �� ȿ������ �����̳ʷ� vector<T>�� ����Ʈ�� ����Ѵ�.

// std::priority_queue<T, Ctr, Cmp> ���� ����Ʈ�� �����̳� vector<T>��
// ����ϴµ�, ���Ҹ� �߰� �� ������ ������ the max-heap property�� ������Ű��
// ���ؼ� �Ź� ���ҵ��� �̵��ؾ� �ϱ� ������ queue<T>�� ������� �ʴ� ���̴�.
// ����, Cmp�� ����Ʈ�� std::less<T>�� ����Ѵ�.

// ���������� �����̳��� front ���Ҵ� heap�� top ���Ҹ� �ǹ��ϱ� ������
// heap�� top�� ���� ū ���Ұ� �ȴ�.
// .push(v)�� std::push_heap(v)�� ��ó�� v�� �������� �����Ѵ�.
// .top()�� top ������ reference�� �����Ѵ�. �̴� ctr.front()�� �����ȴ�.
// .pop()�� top ���Ҹ� std::pop_heap()�Ѵ�.

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
// std::set<T,Cmp>�� a unique set for any T that implements operator<��
// �������̽��� �����Ѵ�. set�� ������ a red-black tree�� ���� �����Ѵ�.
// �̴� balanced binary search trees �� �ǹ��Ѵ�. ��������� insert�� find��
// �׻� O(log n) �ð��� �ɸ���.
// ���� ���ø� ����.
// std::set<char> s {42,43,44};
// s�� {begin=&node2, root=&node1, size=3}�� ���´�.
// node1: {left=&node2,right=&node3,parent=s.root,rb,43}
// node2: {left=nullptr,right=nullptr,parent=node1.left,rb,43}
// node3: {left=nullptr,right=nullptr,parent=node1.right,rb,44}

// ���� �˻� Ʈ���� ���ĵ� ������ ���ҵ��� �����ϱ� ������
// push_front or push_back�� �ǹ̰� ����. ���� ���� v�� �߰��� ����
// s.insert(v)�� ����Ѵ�.
void example1()
{
    std::set<int> s;
    for (int i : {3, 1, 4, 1, 5}) {
        s.insert(i);
    }
    // ���ĵ� ������ ����ȴ�.
    assert((s == std::set {1, 3, 4, 5}));
    auto it = s.begin();
    assert(*it == 1);

    s.erase(4);
    s.erase(it);
    assert((s == std::set {3, 5}));
}

// s.insert(i)�� std::pair<set<T>::iterator,bool> ret�� �����ϴµ�
// ret.first�� ��� �߰��߰ų� ���� �ִ� v ���Ҹ� ����Ű�� �ݺ����̰�,
// ret.second�� true�̸� ��� �߰��� ���̰�, false�� v�� ���� �־����� �ǹ��Ѵ�.
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

// set�� ���ҵ��� ������������ ����Ǿ��� ������
// for-loop�� s.begin()���� std::prev(s.end())���� ���������Ǿ� �ִ�.
// �׷��� The tree-based structure of a set��
// std::find�� std::lower_bound�� ����� �� �ִ�.
// ������ ��ȿ�����̱� ������ Ʈ�������� ��Ʈ ��忡 �ٷ� ���� ������
// std::set�� �ɹ� �Լ��� ����ؾ� �Ѵ�.
// For std::find(s.begin(), s.end(), v), use s.find(v).
// For std::lower_bound(s.begin(), s.end(), v), use s.lower_bound(v).
// For std::upper_bound(s.begin(), s.end(), v), use s.upper_bound(v).
// For std::count(s.begin(), s.end(), v), use s.count(v).
// For std::equal_range(s.begin(), s.end(), v), use s.equal_range(v).
// ���⼭ set�� deduplicated�̱� ������ s.count(v)�� 0 �Ǵ� 1�� ������ ���̴�.

// std::map<K,V>�� ���� key-value pair�� ���� set�̸�,
// std::set< pair<K,V>,K::operator< >��� ������ �� �ִ�.
// std::map�� operator[]�� �����ϴµ�, ���� ���ø� ����.
void indexing_with_operator()
{
    std::map<std::string, std::string> m;

    // m[key] = value �� ���ؼ� ���� key-value pair�� map�� insert�Ѵ�.
    m["hello"] = "world";
    m["quick"] = "brown";
    m["hello"] = "dolly";
    assert(m.size() == 2);

    // ����ó�� m[key]�� ȣ���ϱ⸸ �ص�
    // key-default value pair�� map�� �����ع����� ������ �����ؾ� �Ѵ�.
    assert(m["literally"] == "");
    assert(m.size() == 3);
    // ���⼭ ���� value Ÿ�� V�� not default-constructible�̸�
    // ������ ������ ��Ÿ����.

    // ���� operator[] const ������ ���� ������ const map�� ����� �� ����.

    // ���� value Ÿ�� V�� not default-constructible�̶�� �Ʒ��� ����
    // �ڵ带 �ۼ��ؾ� �Ѵ�.
    using Pair = decltype(m)::value_type;
    if (m.find("hello") == m.end()) {
        // ���� k�� �����ϴ��� �Ǻ��ϱ� ���� m.find(k)�� ����Ѵ�.
        // ���Ҹ� �����ϱ� ���� m.insert(kv) or m.emplace(k,v)�� ����Ѵ�.
        m.insert(Pair {"hello", "dolly"});
        // or equivalently
        m.emplace("hello", "dolly");
    }
}
// std::map�� std::set�� based on trees of pointers �̴�.
// �׷��� cache-unfriendly�ϱ� ������ �̷��� ������ ������ ���ϰ�
// std::unordered_map�� std::unordered_set�� ��ȣ�ؾ� �Ѵ�.
}

//-----------------------------------------------------------------------------
// [A note about transparent comparators]
//-----------------------------------------------------------------------------
namespace section15
{
// ���� ������ m.find("hello")��� �ߴµ�,
// "hello"�� const char[6] type�̱� ������ decltype(m)::key_type��
// std::string���� ĳ���� ���Ѽ� decltype(m)::key_compare��
// std::less<std::string> ���� ȣ���ϰ� �ȴ�.
// ��, m.find("hello")�� ù��° parameter�� std::string�̱� ������
// std::string("hello")�� �Ͻ��� ������ �Ͽ� find�� argument�� �Ѿ��.
// ������ �̷��� ������ ��ȿ�����̴�.
// ���� ������ ���� typedef is_transparent�� ���ǵ�
// heterogeneous operator()�� �����ϰ� �ȴ�.
// std::less<>�� ������ ���� �����Ѵ�.
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

    // find�� std::string�� �������� �ʰ�,
    // const std::string& �� const char* �� �Ű������� ����
    // string�� �� ������ operator< �� ���ȴ�.
    auto it = m2.find("hello");
}
// �̷��� magic�� std::map�� ���� ��ü���� �Ͼ�µ�,
// find�� Ư���ϰ� is_transparent�� üũ�Ͽ� �̿� ���� �ൿ�� �ٲ۴�.
// ����ϰ� count, lower_bound, upper_bound, equal_range�� �׷���.
// �׷��� �̻��ϰ� erase�� m.erase(it)�� m.erase(v)�� �����ϱ� ��Ʊ�
// ������ �׷��� �������� �ʾҴ�.
// �׷��� heterogeneous comparison�� ����ϴ� ������ ������ ���� �Ѵ�.
// auto [begin, end] = m.equal_range("hello");
// m.erase(begin,end);
}

//-----------------------------------------------------------------------------
// [Oddballs: std::multiset<T> and std::multimap<K,V>]
//-----------------------------------------------------------------------------
namespace section16
{
// std::multiset<T,Cmp>�� std::set<T,Cmp>�� �����ϸ� duplicate elements��
// ����Ѵ�. std::multimap<K,V,Cmp>�� ���������̴�.
// ���� ���ø� ����.
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
    mm2.erase("hello"); // "hello"�� Ű�� ���� ��� ���Ҹ� �����Ѵ�.
    assert(mm2.count("hello") == 0);
}
// multi ������ operator[]�� �������� �ʴ´�.
}

//-----------------------------------------------------------------------------
// [Moving elements without moving them]
//-----------------------------------------------------------------------------
namespace section17
{
// list�� �����͵��� �ڸ��� �̾� ���̴� �˰����� �ִ� ��ó��
// the tree-based containers �� ����� �˰����� ���� �ִ�.
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

    // �ߺ��� Ű�� ���� ���Ҵ� transfer ���� �ʴ´�.
    m.merge(otherm);
    // �̷��� ���Ҵ� ���� �����̳ʿ� ������ ���� �ȴ�.
    assert((otherm == decltype(m) {
                          {"hello", "dolly"},
    }));

    assert((m == decltype(m) {
                     {"hello", "world"},
                     {"quick", "brown"},
                     {"sad",   "clown"},
    }));
}

// m.update(otherm)�� m.insert(otherm.begin(), otherm.end())�� �����ϴ�.
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

    // ���� ���ҵ��� �����ؼ� �����ϴ� �˰����� ������ ����.
    auto nh1 = otherm.extract("sad");
    assert(nh1.key() == "sad" && nh1.mapped() == "clown");
    // ���� ���ҿ� �ش��ϴ� Ű�� �����Ƿ� ������ �����Ѵ�.
    auto [it2, inserted2, nh2] = m.insert(std::move(nh1));
    assert(*it2 == Pair("sad", "clown") && inserted2 == true && nh2.empty());

    auto nh3 = otherm.extract("hello");
    assert(nh3.key() == "hello" && nh3.mapped() == "dolly");
    // ���� ���ҿ� �ش��ϴ� Ű�� �����Ƿ� ������ �����Ѵ�.
    auto [it4, inserted4, nh4] = m.insert(std::move(nh3));
    assert(*it4 == Pair("hello", "world") && inserted4 == false &&
           !nh4.empty());

    // ���� ���ҿ� �ش��ϴ� Ű�� �־ ������ �������� ��,
    // ���� ���Ҹ� ���ο� ���ҷ� ����� �� �ִ�.
    m.insert_or_assign(nh4.key(), nh4.mapped());

    // ���� ���� ����� ������ ���Ҹ� ������ ��, �����ϴ� ���̴�.
    m.erase(it4);
    m.insert(std::move(nh4));
}

// extract�� �����ϴ� Ÿ���� "node handle"�̴�.
// �̴� nh.key()�� nh.mapped() �����ڸ� �����ϴµ�
// �̷κ��� ���� �����͸� �����ϰų� �̵����� �ʰ� �״�� �����ϰų�
// �����Ͽ� ����Խ�ų �� �ִ� ���̴�.
// ������ ���� std::transform���� �����͸� manipulation�� �� �� �ִ�.
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
// [����]
// ������ ũ�⸦ ���� �����͸� ������ ũ���� ������ ������Ű�� �Լ���
// hash function�̶�� �����Ѵ�.
// hash function�� �����ϴ� ���� hash value �Ǵ� �ܼ��� hashes ��� �Ѵ�.
// hash values�� �ε����� ����ϴ� a fixed-size table�� hash table�̶� �θ���.
// hash table�� �����ϱ� ���� hash function�� ����ϴ� ���� hashing or
// scatter storage addressing �̶�� �θ���.
// �̷��� �����͸� �����ϴ� ����� �˻� �� ������ ����ð��� �����ϴ�.
// ������ �̴� Ű�� hash function ���� ������� �Ӽ��� �����ϱ� ������
// �־��� ��� O(n)�� �ǰ�, �ٸ� �����ӿ��� �ұ��ϰ� ���� �ؽð��� ������
// hash collision�� �Ͼ �� �ִ�.

// std::unordered_set�� a chained hash table�� ��Ÿ����.
// �̴� buckets�� ���� ũ���� �迭�� ����,
// �� bucket�� ������ ���ҵ��� a singly linked list�̴�.
// ���ο� ���Ұ� �����̳ʿ� �߰��� ��, ���ο� ���� ���� ���� "hash" ���� ������
// the linked list (bucket)���� ���� �ִ´�.
// ���������� �����͸� linked lists�� �����ϹǷ� forward iterator�� �����Ѵ�.
// ���� �����͸� ���ĵ��� ���� ������ �����Ѵ�.
// insert, erase, find �� �ּ� O(1)�� �־����� O(n) ����ð��� ���� ������
// ����ȭ�� �ʿ��� ��쿡 �ؽ� �Լ��� �� �����ؼ� unordered_set�� ����Ѵ�.
// std::unordered_map<K,V>�� std::unordered_set<pair<K,V>>�� ����ϴ�.

// std::unordered_set, std::unordered_map�� Hash, KeyEqual ���ø� �Ű�������
// �޴´�. ����Ʈ�� std::hash<K>, std::equal_to<K>�� �����Ѵ�.

}

//[Load factor and bucket lists]
// ������ buckets�� �����ϴ� �˰������ ������ ����.
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
// ������� ������ �����̳ʸ� �����ߴ�.
// �׷��ٸ� �����̳ʴ� ���κ��� �޸𸮸� �Ҵ�޴� ���ϱ�?
// std::array�� ������ ��� �����̳ʴ� optional ���ø� �Ű������� allocator��
// �޴´�. �̴� �����̳��� ���Ҵ��� �Ͼ�ų�, ���ο� ��带 ��ũ�� ����Ʈ��
// �߰��ϴ� �� �޸𸮰� ���κ��� �Ҵ�Ǵ� ���� ����Ų��.
// ����Ʈ�� std::allocator<T>�� ����Ѵ�.
// allocator�� �����ؼ� ch8���� �����Ѵ�.