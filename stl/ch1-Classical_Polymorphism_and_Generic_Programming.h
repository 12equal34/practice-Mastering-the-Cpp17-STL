#pragma once

// ch1���� �ʿ��� ������ϵ��� ������ ����.
#include <stdexcept>
#include <vector>
#include <cassert>

// �������� ���θ� �����ϴµ�, ������� �����鼭 ������ �� �ֵ��� �ۼ��Ͽ���.
// �� ����� Mastering the C++17 STL �̸�, ���ۿ� �˻��ϱ� �ٶ���.
// ���ӽ����̽��� ��(section)���� ��ȣ(1,2...)�� �Űܼ� �ۼ��Ͽ���.

// ���� �� �׽�Ʈ �ڵ���� main.cpp ���Ͽ��� �ش� é���� ���������
// �����Ͽ� test()�� ȣ���ϸ� �ȴ�.

//-----------------------------------------------------------------------------
// [Concrete monomorphic functions]
//-----------------------------------------------------------------------------
namespace section1
{
// �Ʒ��� ���� Ÿ���� ���� ��,
class array_of_ints
{
    int data[10] = {};
public:
    int  size() const { return 10; }
    int& at(int i) { return data[i]; }
};

// ������ ���� �Լ��� ��������.
void double_each_element_concrete(array_of_ints& arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}

// double_each_element_concrete�Լ��� ���� array_of_ints Ÿ�Կ� ���ؼ���
// �۵��Ѵ�. �ٸ� Ÿ���� ������ ������ ������ ��Ÿ����. �̷��� �Լ����� ������
// concrete or monomorphic functions�� �Ѵ�. ���� �̷��� �Լ����� abstract��
// �ϱ⿡ �����ؼ� concrete��� �θ���. ���� c++ stl�� concrete sort��
// �����ߴٸ� Ư���� Ÿ�Ե鿡 ���ؼ��� �۵��� ���̴�.
}

//-----------------------------------------------------------------------------
// [Classically polymorphic functions]
//-----------------------------------------------------------------------------
namespace section2
{
// OO programming�� ���� �߻�ȭ ���α׷����� �� �� �ִ�.
// �̴� c++���� abstract base class��
// public virtual member functions�� ���� �����Ѵ�.

// abstract base
class container_of_ints
{
public:
    virtual int  size() const = 0;
    virtual int& at(int)      = 0;
};

// derived
class array_of_ints : public container_of_ints
{
    int data[10] = {};
public:
    int  size() const override { return 10; }
    int& at(int i) override { return data[i]; }
};

// derived
class list_of_ints : public container_of_ints
{
    struct node {
        int   data;
        node* next;
    };
    node* head_ = nullptr;
    int   size_ = 0;
public:
    int  size() const override { return size_; }
    int& at(int i) override
    {
        if (i >= size_) throw std::out_of_range("at");
        node* p = head_;
        for (int j = 0; j < i; ++j) {
            p = p->next;
        }
        return p->data;
    }
};

void double_each_element_polymorhic(container_of_ints& arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}

void test()
{
    array_of_ints arr;
    double_each_element_polymorhic(arr);

    list_of_ints lst;
    double_each_element_polymorhic(lst);
}

// container_of_ints�� ��ӹ޴� dynamic type���� ������
// the virtual table of function pointers�� ���� �Լ� �����͸�
// ���ؼ� �ش� �Լ��� ȣ���Ѵ�.
// �̸� ���� double_each_element_polymorhic�Լ���
// �پ��� dynamic types�� �Ű������� �ѱ� �� �ִ�.
// �̷��� �Լ��� polymorphic�ϴٰ� �Ѵ�.

// ������ �������� ����ϴ� �Լ��� �ҽ��ڵ带 �� �ϳ���
// ����ڵ带 �����ϱ� ������ �޸𸮸� ������ �� �ִ�.
// ����, double_each_element_polymorhic�� �Լ� �����Ϳ�
// ������ �� �ִ�.
// ������, base class�� ����ؾ� �ϹǷ� ������ �Ϲݼ��� ���� �� ����.
}

//-----------------------------------------------------------------------------
// [Generic programming with templates]
//-----------------------------------------------------------------------------
namespace section3
{
// c++������ templates�� ���� generic algorithms�� �����Ѵ�.
// template�̶� compiler���� arr�� Ÿ���� �������� �𸣰�����
// ��� Ÿ���̵��� ���ο� �Լ��� ����� �޶�� ��û���̴�.
// ���⼭ arr�� Ÿ���� parameter type�̶� �θ���,
// ���ο� �Լ��� ����� ������ a template instantiation�̶� �Ѵ�.

template <class ContainerModel>
void double_each_element(ContainerModel& arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}

void test()
{
    section2::array_of_ints arr;
    double_each_element(arr);

    section2::list_of_ints lst;
    double_each_element(lst);

    std::vector<int> vec = {1, 2, 3};
    double_each_element(lst);
}

// ContainerModel�� �ݵ�� �����ؾ��ϴ� operators�� ��������
// ��Ȯ�ϰ� ǥ���� �� �ִٸ� ���� ���α׷� �������� ���̴�.
// �׷��� set of operations�� c++������ concept�̶� �Ѵ�.

// ContainerModel�� concept�� ������ ����.
// "1.�ɹ� �Լ� size�� ������ �Ѵ�. size�� �����̳��� ũ�⸦ int�� �Ǵ�
//    int�� �� ������ Ÿ���� �����ؾ� �Ѵ�.
//  2.�ɹ� �Լ� at�� ������ �Ѵ�. at�� int index �Ǵ�
//    int�� �Ͻ��� ��ȯ�ϴ� Ÿ�Ը� �޾Ƽ� �̸� �ε����� ����
//    �����̳��� ���Ҹ� non-const reference�� �����ؾ� �Ѵ�."

// ������ class�� ���� ���� Container concept�� �ʿ���
// set of operations�� ���´ٸ�,
// (���� ���, at�� ���� Ÿ���� operator*=(int)�� ������ �Ѵ�.)
// double_each_element�� ����� �� �ִ�.
// �̷��� class�� a model of the Container concept �̶�� �θ���.
// �̷��� ������ ContainerModel�̶�� temlate type�� �ռ� ����ߴ�.
// �����δ� ���������� Container��� �̸����� �ۼ��� ���̴�.

// �츮�� ���ø��� ���� �߻� �˰����� ������ ��,
// ������ concepts�� ���� models�� template types�� �����Ͽ�
// ������ �ð��� abstract algorithm�� ������ �����ϴµ�,
// �� ������ generic programming�̶�� �θ���.

// Container concept�� ������ �����̳ʰ� ���� ������ Ÿ�Կ� ����
// ����� ���ٴ� �Ϳ� �����ϱ� �ٶ���.
// ���� ���, ���� test()������ �����̳� ���� int�� ���Ҹ� ����������
// double�� ���Ҹ� �����ص� �������.

void can_double_not_int()
{
    std::vector<double> vecd = {1.0, 2.0, 3.0};
    double_each_element(vecd);
}

// �̷��� ������ classical polymorphism���� �Ϲ�ȭ�Ǿ���.
// classical polymorphism�� a stable interface signature�ȿ�����
// �پ��� ������ ������ �� �ִ�. ��, at(i)�� �׻� int&�� �����ؾ��Ѵ�.
// �׷��� �ѹ��̶� signatures�� ���͸��� �����ϸ� ���� ���� ����� �����.

// ���� generic programming�� ������ ������ ����.
// �ζ��� ȣ���� �����ϱ� ������ �ӵ������ ��ȸ�� �ִ�.
// classical polymorphism�� �ݵ�� virtual table�� �ݺ������� query��
// �Ͽ� virtual method�� ȣ���ϱ� ������ ������ �ð��� �۵����� �ʴ´�.
// �ݸ鿡, template�� �ش� method�� ��� ȣ���ϰų� �ζ��� ȣ���� �� �ִ�.

namespace Examples_Provided_by_the_STL
{
    // �����̳ʰ� �����ϴ� ���ҵ��� ������ �����Ѵ�.
    template <class Container>
    int count(const Container& container)
    {
        int sum = 0;
        for (auto&& elt : container) {
            sum += 1;
        }
        return sum;
    }

    // �����̳ʰ� �����ϴ� ���ҵ� �߿���, ������ ������ ����(predicate)�� ����
    // ����(proposition)�� ���� ���ҵ��� ������ �����Ѵ�.
    // ("A�� B�̴�" or "A�� B�� �ƴϴ�"��� �������� 'B'�� ������ �Ѵ�.)
    // (���⼭�� A�� elt�̰�, B�� pred��� �����ϸ� �ȴ�.)
    template <class Container, class Predicate>
    int count_if(const Container& container, Predicate pred)
    {
        int sum = 0;
        for (auto&& elt : container) {
            if (pred(elt)) {
                sum += 1;
            }
        }
        return sum;
    }

    void test()
    {
        std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
        assert(count(v) == 8);

        int number_above = count_if(v, [](int e) { return e > 5; });
        int number_below = count_if(v, [](int e) { return e < 5; });

        assert(number_above == 2);
        assert(number_below == 5);
    }
    // the Container�� the rangerd for-loop syntax�� �����ؾ� �ϹǷ�
    // begin(), end()�� �����ϰ� �˸´� Ÿ���� ���� iterator�� �����ؾ� �Ѵ�.
    
    // �߿��� ��Ƿ� generic programming���� pred�� �Ű����� elt��
    // ��� Ÿ��, �׸��� �� �Ǵ� ���۷��������� ����� Ư������ �ʴ´�.

    // ���±��� container��� ǥ���� ����ϰ� count, count_if��
    // �ڿ����� container-based algorithms���� �ۼ��Ͽ���.
    // �̴� c++20 or c++23���� ���鼭 �������� C++ ��Ÿ���̴�.
    // c++11 ���������� move semantics�� move construction�� ������ ������
    // ���������� container�� �ٷ�� ����� ��ȿ�����̾���.
    // ����, STL�� �� ������ iterator�� �ٷ�� ������� �����εǾ���.
    // �� ������� �ۼ��� �˰����� range-based algorithms�̶� �Ѵ�.
    // �̴� ���� ���� �����̴�.
}
}