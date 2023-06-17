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
    return {
        {
         a[1] * b[2] - a[2] * b[1],
         a[2] * b[0] - a[0] * b[2],
         a[0] * b[1] - a[1] * b[0],
         }
    };
}
// ����, std::vector<int[3]> �� �Ұ���������
// std::vector<std::array<int,3>>�� �����ϴ�.
}
