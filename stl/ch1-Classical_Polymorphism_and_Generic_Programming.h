#pragma once

// ch1에서 필요한 헤더파일들은 다음과 같다.
#include <stdexcept>
#include <vector>
#include <cassert>

// 이제부터 공부를 시작하는데, 순서대로 읽으면서 공부할 수 있도록 작성하였다.
// 본 교재는 Mastering the C++17 STL 이며, 구글에 검색하길 바란다.
// 네임스페이스로 절(section)마다 번호(1,2...)를 매겨서 작성하였다.

// 예제 및 테스트 코드들은 main.cpp 파일에서 해당 챕터의 헤더파일을
// 포함하여 test()를 호출하면 된다.

//-----------------------------------------------------------------------------
// [Concrete monomorphic functions]
//-----------------------------------------------------------------------------
namespace section1
{
// 아래와 같은 타입이 있을 때,
class array_of_ints
{
    int data[10] = {};
public:
    int  size() const { return 10; }
    int& at(int i) { return data[i]; }
};

// 다음과 같은 함수를 생각하자.
void double_each_element_concrete(array_of_ints& arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}

// double_each_element_concrete함수는 오직 array_of_ints 타입에 대해서만
// 작동한다. 다른 타입을 넣으면 컴파일 에러가 나타난다. 이러한 함수들의 종류를
// concrete or monomorphic functions라 한다. 또한 이러한 함수들은 abstract라
// 하기에 부족해서 concrete라고 부른다. 만약 c++ stl이 concrete sort만
// 제공했다면 특정한 타입들에 대해서만 작동할 것이다.
}

//-----------------------------------------------------------------------------
// [Classically polymorphic functions]
//-----------------------------------------------------------------------------
namespace section2
{
// OO programming을 통해 추상화 프로그래밍을 할 수 있다.
// 이는 c++에서 abstract base class의
// public virtual member functions을 통해 구현한다.

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

// container_of_ints를 상속받는 dynamic type들은 각자의
// the virtual table of function pointers에 적힌 함수 포인터를
// 통해서 해당 함수를 호출한다.
// 이를 통해 double_each_element_polymorhic함수에
// 다양한 dynamic types를 매개변수로 넘길 수 있다.
// 이러한 함수를 polymorphic하다고 한다.

// 장점은 다형성을 사용하는 함수의 소스코드를 딱 하나만
// 기계코드를 생성하기 때문에 메모리를 절약할 수 있다.
// 또한, double_each_element_polymorhic의 함수 포인터에
// 접근할 수 있다.
// 하지만, base class를 상속해야 하므로 진정한 일반성을 가질 순 없다.
}

//-----------------------------------------------------------------------------
// [Generic programming with templates]
//-----------------------------------------------------------------------------
namespace section3
{
// c++에서는 templates를 통해 generic algorithms을 구현한다.
// template이란 compiler에게 arr의 타입이 무엇일지 모르겠지만
// 어떠한 타입이든지 새로운 함수를 만들어 달라는 요청문이다.
// 여기서 arr의 타입은 parameter type이라 부르고,
// 새로운 함수를 만드는 행위는 a template instantiation이라 한다.

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

// ContainerModel이 반드시 지원해야하는 operators가 무엇인지
// 명확하게 표현할 수 있다면 좋은 프로그램 디자인일 것이다.
// 그러한 set of operations을 c++에서는 concept이라 한다.

// ContainerModel의 concept은 다음과 같다.
// "1.맴버 함수 size를 가져야 한다. size는 컨테이너의 크기를 int형 또는
//    int와 비교 가능한 타입을 리턴해야 한다.
//  2.맴버 함수 at를 가져야 한다. at은 int index 또는
//    int로 암시적 변환하는 타입를 받아서 이를 인덱스로 갖는
//    컨테이너의 원소를 non-const reference로 리턴해야 한다."

// 임의의 class가 위와 같은 Container concept에 필요한
// set of operations를 갖는다면,
// (예를 들어, at의 리턴 타입이 operator*=(int)를 가져야 한다.)
// double_each_element를 사용할 수 있다.
// 이러한 class를 a model of the Container concept 이라고 부른다.
// 이러한 이유로 ContainerModel이라고 temlate type을 앞서 명명했다.
// 앞으로는 관습적으로 Container라는 이름으로 작성할 것이다.

// 우리가 템플릿을 통해 추상 알고리즘을 구현할 때,
// 적합한 concepts을 갖는 models를 template types에 전달하여
// 컴파일 시간에 abstract algorithm의 동작을 결정하는데,
// 이 과정을 generic programming이라고 부른다.

// Container concept의 설명에서 컨테이너가 갖는 원소의 타입에 대한
// 언급이 없다는 것에 유의하길 바란다.
// 예를 들어, 위의 test()에서는 컨테이너 모델이 int형 원소를 포함하지만
// double형 원소를 포함해도 상관없다.

void can_double_not_int()
{
    std::vector<double> vecd = {1.0, 2.0, 3.0};
    double_each_element(vecd);
}

// 이러한 이유로 classical polymorphism보다 일반화되었다.
// classical polymorphism는 a stable interface signature안에서만
// 다양한 동작을 결정할 수 있다. 즉, at(i)는 항상 int&를 리턴해야한다.
// 그래서 한번이라도 signatures을 엉터리로 정의하면 좋지 않은 결과를 만든다.

// 또한 generic programming의 장점은 다음과 같다.
// 인라인 호출이 가능하기 때문에 속도향상의 기회가 있다.
// classical polymorphism은 반드시 virtual table에 반복적으로 query를
// 하여 virtual method를 호출하기 때문에 컴파일 시간에 작동하지 않는다.
// 반면에, template은 해당 method를 즉시 호출하거나 인라인 호출할 수 있다.

namespace Examples_Provided_by_the_STL
{
    // 컨테이너가 포함하는 원소들의 개수를 리턴한다.
    template <class Container>
    int count(const Container& container)
    {
        int sum = 0;
        for (auto&& elt : container) {
            sum += 1;
        }
        return sum;
    }

    // 컨테이너가 포함하는 원소들 중에서, 유저가 전달한 술어(predicate)를 갖는
    // 명제(proposition)가 참인 원소들의 개수를 리턴한다.
    // ("A는 B이다" or "A는 B가 아니다"라는 명제에서 'B'를 술어라고 한다.)
    // (여기서는 A가 elt이고, B는 pred라고 생각하면 된다.)
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
    // the Container는 the rangerd for-loop syntax를 지원해야 하므로
    // begin(), end()를 포함하고 알맞는 타입을 가진 iterator를 리턴해야 한다.
    
    // 중요한 사실로 generic programming에서 pred의 매개변수 elt가
    // 어떠한 타입, 그리고 값 또는 레퍼런스인지를 절대로 특정하지 않는다.

    // 여태까지 container라는 표현을 사용하고 count, count_if을
    // 자연스레 container-based algorithms으로 작성하였다.
    // 이는 c++20 or c++23으로 오면서 현대적인 C++ 스타일이다.
    // c++11 이전까지는 move semantics와 move construction이 없었기 때문에
    // 직접적으로 container를 다루는 방법은 비효율적이었다.
    // 따라서, STL은 더 가벼운 iterator를 다루는 방법으로 디자인되었다.
    // 이 방법으로 작성된 알고리즘을 range-based algorithms이라 한다.
    // 이는 다음 장의 주제이다.
}
}