#pragma once

// ch4���� �ʿ��� ������ϵ��� ������ ����.
#include <string>
#include <algorithm>
#include <cassert>
#include <array>
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
    assert(c_style != other); // address comparison
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

    assert(&arr != &arr); // different addresses
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
// std::array�ʹ� �ٸ��� gigantic array�� �����ϰ� ������ �� �ְ� ��������
// ũ�⸦ ������ �� �ִ�.
// std::vector���� �߿��� attribute�� �ִµ�, �ٷ� capacity �̴�.
// capacity�� �׻� size �̻��� ���� ���´�. ���� vector�� �� ��ŭ��
// ���Ҹ� ���� �� �ִ��� ��Ÿ����.
// std::vector<char> v {42, 43, 44};
// v.reserve(8);
// => v�� ptr�� {42,43,44,?,?,?,?,?}�� ù ���Ҹ� ����Ű��,
// v�� size = 3, v�� capacity = 8 �̴�.

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
                // ���� ���ҵ��� ���ܾ��� move�� �����ϴٸ�,
                ::new (&new_ptr[i]) T(std::move(ptr_[i]));
            } else {
                // ���� ���ҵ��� ���ܸ� ���� �� �ִٸ�,
                // move�� �������� �ʾƼ� ���ҵ��� �������� ������ �Ѵ�.
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
}
