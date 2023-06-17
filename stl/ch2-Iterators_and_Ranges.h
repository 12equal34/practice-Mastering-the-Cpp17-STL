#pragma once

// ch2에서 필요한 헤더파일들은 다음과 같다.
#include <vector>
#include <cassert>

//-----------------------------------------------------------------------------
// [The problem with integer indices]
//-----------------------------------------------------------------------------
namespace section1
{
// 이전 장에서 다음과 같은 컨테이너 기반 알고리즘은
// array_of_ints나 std::vector에 대해서 합리적으로 동작하지만
// list_of_ints에 대해서는 불합리적이다.
template <typename Container>
void double_each_element(Container& arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}
// 이유는 size()와 at()의 구현 때문이다.
// list_of_ints::at()은 O(n)이므로 위의 알고리즘은 O(n^2)이 된다.
// 따라서 at() with integer indices는 해당 알고리즘에 대해
// 컨테이너에 따라서 비효율적인 경우가 존재한다.
// 그렇다면 컨테이너마다 데이터를 어떻게 다루어야 효율적인 알고리즘을
// 작성할 수 있을까?
// 컴퓨터가 데이터를 다루는 핵심적인 연산인 포인터를 관점으로 살펴본다.
}

//-----------------------------------------------------------------------------
// [On beyond pointers]
//-----------------------------------------------------------------------------
namespace section2
{
// 자료구조를 생각하지 말고 배열의 한 원소, 링크드 리스트의 한 원소, 또는
// 트리의 한 원소를 어떻게 동일하게 다루는가? 가장 적합한 방법은 메모리 안의
// 원소의 주소를 가리키는 포인터를 사용하는 것이다.

// 링크드 리스트의 원소 개수를 구하는 알고리즘을
// c에서 다음과 같이 작성한다.
// for (node *p = lst.head_; p != nullptr; p = p->next) {
//      if (pred(p->data)) {
//          sum += 1;
//      }
// }
// p = p->next에 해당하는 부분은 포인터 p를 incrementing시키는 것과 같다.
// 이러한 개념을 the concept of incrementing 이라 하자.

// c++에서는 the concept of incrementing를 ++operator를 오버로드하여
// p = p->next에 대응되는 부분을 ++p로 작성한다.

struct list_node {
    int        data;
    list_node* next;
};

class list_of_ints_iterator
{
    list_node* ptr_;

    friend class list_of_ints;
    explicit list_of_ints_iterator(list_node* p)
        : ptr_(p)
    { }
public:
    int&                   operator*() const { return ptr_->data; }
    list_of_ints_iterator& operator++()
    {
        ptr_ = ptr_->next;
        return *this;
    }
    list_of_ints_iterator& operator++(int)
    {
        auto it = *this;
        ++*this;
        return it;
    }
    bool operator==(const list_of_ints_iterator& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }
    bool operator!=(const list_of_ints_iterator& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }
};

class list_of_ints
{
    list_node* head_ = nullptr;
    list_node* tail_ = nullptr;
    // ...
public:
    using iterator = list_of_ints_iterator;
    iterator begin() { return iterator {head_}; }
    iterator end() { return iterator {nullptr}; }
};

template <class Container, class Predicate>
int count_if(Container& ctr, Predicate pred)
{
    int sum = 0;
    for (auto it = ctr.begin(); it != ctr.end(); ++it) {
        if (pred(*it)) {
            sum += 1;
        }
    }
    return sum;
}
}

//-----------------------------------------------------------------------------
// [Const iterators]
//-----------------------------------------------------------------------------
namespace section3
{
// 위의 list iterator 예시에서 count_if의 매개변수 타입이
// const Container&에서 Container&로 바뀌었고,
// begin(), end()가 non-const이다. 그래서 *it가
// int&을 리턴하는데, 하지만 count_if는 값을 변경시키지 않으므로
// const int&를 리턴하는 begin(), end()가 적합하다.
// 따라서 const list_of_ints를 정의하여 사용하고 싶지만
// operator*의 구현에 의해 여전히 *it는 int&로 리턴한다.
// 그래서 operator*가 const int&를 리턴하는 버전이 있어야 하지만
// 이를 오버로드할 수 없다. 그러면 const int&으로 리턴하는 것으로
// 수정하면 이번에는 참조 전달한 값을 수정할 수 없게 된다.

// 따라서 위의 문제를 해결하기 위해 STL은 서로 다른 2개의 iterator:
// bag::iterator, bag::const_iterator를 사용한다. (여기서 bag은 임의의 이름)
// non-const bag::begin()은 iterator를 리턴하고,
// bag::begin() const는 const_iterator를 리턴한다.
//
// const_iterator와 const iterator는 서로 다르므로 주의해야 한다.
// const iterator는 ++연산이 불가능하고 단일 *연산의 리턴형이 non-const이다.
// const_iterator는 ++연산이 가능하고 단일 *연산의 리턴형이 const이다.

// 이제 const_iterator를 구현하는데, iterator와 코드중복이 있으므로
// 템플릿을 통해서 효율적으로 작성한다.

struct list_node {
    int        data;
    list_node* next;
};

template <bool Const>
class list_of_ints_iterator
{
    friend class list_of_ints;
    friend class list_of_ints_iterator<!Const>;

    using node_pointer =
        std::conditional_t<Const, const list_node*, list_node*>;
    using reference = std::conditional_t<Const, const int&, int&>;

    node_pointer ptr_;

    explicit list_of_ints_iterator(node_pointer p)
        : ptr_(p)
    { }
public:
    reference operator*() const { return ptr_->data; }
    auto&     operator++()
    {
        ptr_ = ptr_->next;
        return *this;
    }
    auto operator++(int)
    {
        auto result = *this;
        ++*this;
        return result;
    }

    // Support comparison between iterator and const_iterator types
    template <bool R>
    bool operator==(const list_of_ints_iterator<R>& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }

    template <bool R>
    bool operator!=(const list_of_ints_iterator<R>& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    // Support implicit conversion of iterator to const_iterator
    // (but not vice versa)
    operator list_of_ints_iterator<true>() const
    {
        return list_of_ints_iterator<true> {ptr_};
    }
};

class list_of_ints
{
    list_node* head_ = nullptr;
    list_node* tail_ = nullptr;
    // ...
public:
    using const_iterator = list_of_ints_iterator<true>;
    using iterator       = list_of_ints_iterator<false>;

    iterator       begin() { return iterator {head_}; }
    iterator       end() { return iterator {nullptr}; }
    const_iterator begin() const { return const_iterator {head_}; }
    const_iterator end() const { return const_iterator {nullptr}; }
};
}

//-----------------------------------------------------------------------------
// [A pair of iterators defines a range]
//-----------------------------------------------------------------------------
namespace section4
{
// 다음의 예제를 살펴보자.
template <class Iterator>
void double_each_element(Iterator begin, Iterator end)
{
    for (auto it = begin; it != end; ++it) {
        *it *= 2;
    }
}

void test()
{
    std::vector<int> v {1, 2, 3, 4, 5, 6};

    // 모든 원소를 2배로 만든다.
    double_each_element(v.begin(), v.end());
    // 여기서 std::vector::iterator를 사용했다.

    // 절반의 원소만 적용할 수도 있다.
    double_each_element(v.begin(), v.begin() + 3);
    // 여기도 std::vector::iterator를 사용했다.

    // 위와 동일한데,
    double_each_element(&v[0], &v[3]);
    // 여기는 int*를 사용했다.
    // int*가 Iterator의 필요조건들을 모두 만족하기 때문에
    // 포인터 타입마저도 동작하는 것이다.
    // 이로인해 iterator-pair model은 상당히 유연함을 알 수 있다.
    // 하지만 이렇게 raw pointers를 사용하는 방식은 피해야 한다.
    // 적절한 iterator 타입을 사용해야 올바르다.

    // a pair of iterators는 a range of data elements를 정의한다.
    // 찾기 또는 변환을 하기 위해서, 직접적으로 a container에 접근할
    // 필요가 없다. 오직 the particular range of element만 필요하다.
    // 이는 나중에 설명할 a non-owning view이란 개념을 이끈다.

    // double_each_element는 Iterator가
    // incrementability, comparability, and dereferenceability
    // 를 구현하는 어떠한 타입이든 generic하게 동작할 것이다.
}
}

//-----------------------------------------------------------------------------
// [Iterator categories]
//-----------------------------------------------------------------------------
namespace section5
{
// 이전에 count와 유사한 std::distance의 구현을 살펴보자.
template <typename Iterator>
int distance(Iterator begin, Iterator end)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        sum += 1;
    }
    return sum;
}

// 그리고 std::count_if의 구현을 살펴보자.
template <typename Iterator, typename Predicate>
int count_if(Iterator begin, Iterator end, Predicate pred)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            sum += 1;
        }
    }
    return sum;
}

void test()
{
    std::vector<int> v {3, 1, 4, 1, 5, 9, 2, 6};

    int number_above =
        count_if(v.begin(), v.end(), [](int e) { return e > 5; });
    int number_below =
        count_if(v.begin(), v.end(), [](int e) { return e < 5; });

    // 위의 distance의 구현은 vector에 대해서 비효율적으로 계산한다.
    int total = section5::distance(v.begin(), v.end());
    // O(1)이 가능한 알고리즘을 O(n)으로 계산해버린다.

    assert(number_above == 2);
    assert(number_below == 5);
    assert(total == 8);
}

// 따라서, std::distance의 구현은
// a template specialization을 포함한다.
template <>
int distance(int* begin, int* end)
{
    return static_cast<int>(end - begin);
}

// 그러나, 우리는 int*나 std::vector::iterator에 대해서만
// specilaization를 하는 것이 아니라
// 특정 연산을 지원하는 모든 반복자 타입들에 대해
// 효율적으로 구현하고자 한다.
// 그래서 반복자의 종류들을 구별한다.
// STL에서 구분하는 the concepts of Iterator은 다음과 같다.
// 1. RandomAccessIterator
// 2. BidirectionalIterator (weaker than RandomAccessIterator)
// 3. ForwardIterator       (weaker than BidirectionalIterator)
// 4. InputIterator         (weaker than ForwardIterator)
// 5. OutputIterator        (weaker than ForwardIterator)
}

//-----------------------------------------------------------------------------
// [Input and output iterators]
//-----------------------------------------------------------------------------
namespace section6
{
// a weaker concept than ForwardIterator이 있을까?
// 예를 들어, ForwardIterator로 자신을 복제하거나
// 데이터들을 2배해서 저장할 수 있을 것이다.
// iterartor를 다루는 것만으로 그 범위에 속하는 데이터들을
// 전혀 영향을 주지 않기 때문에 다음과 같은 iterator를
// 고려할 수 있다. 이것은 반복자가 가리키는 데이터가 없고
// 대신에 data member를 갖고 있으며, 반복자의 복제가 무의미하다.
// not meaningfully copyable, do not point to data elements
// in any meaningful sense한 iterator를 InputIterator 라고 한다.
class getc_iterator
{
    char ch;
public:
    getc_iterator()
        : ch(getc(stdin))
    { }
    char  operator*() const { return ch; }
    auto& operator++()
    {
        ch = getc(stdin);
        return *this;
    }
    auto operator++(int)
    {
        auto result(*this);
        ++*this;
        return result;
    }
    bool operator==(const getc_iterator&) const { return false; }
    bool operator!=(const getc_iterator&) const { return true; }
};
// std::istream_iterator는 위와 유사하다.

// 위와 대응되는 iterator를 고려할 수 있다.
// not meaningfully copyable, writeable-into but not readable-out-of
// 한 iterator를 OutputIterator 라고 한다.
class putc_iterator
{
    struct proxy {
        void operator=(char ch) { putc(ch, stdout); }
    };
public:
    proxy operator*() const { return proxy {}; }
    auto& operator++() { return *this; }
    auto& operator++(int) { return *this; }
    bool  operator==(const putc_iterator&) const { return false; }
    bool  operator!=(const putc_iterator&) const { return true; }
};

void test()
{
    putc_iterator it;
    for (char ch : {'h', 'e', 'l', 'l', 'o', '\n'}) {
        *it++ = ch;
    }
}

// C++의 모든 iterator 타입은 적어도 다음의 five categories 중에 하나에 속한다.
// 1. InputIterator
// 2. OutputIterator
// 3. ForwardIterator
// 4. BidirectionalIterator
// 5. RandomAccessIterator
//
// 컴파일 시간에 특정 iterator type이 BidirectionalIterator 또는
// RandomAccessIterator인지 알아내기 쉽다. 그러나,
// InputIterator, OutputIterator, 또는 ForwardIterator인지
// 알아내기란 불가능하다.
//
// 왜냐하면, 예를들어
// getc_iterator, putc_iterator, list_of_ints::iterator 모두
// 정확히 같은 syntactic operations(dereferencing with* it,
// incrementing with ++it, and comparison with it != it)를
// 지원하기 때문에 오직 semantic level에서만 구분되기 때문이다.
//
// 그렇다면 STL은 어떻게 이들을 구별할까?
// iterator class마다 iterator_category란 a member typedef을 정의한다.
//
//  class getc_iterator {
//      using iterator_category = std::input_iterator_tag;
//      // ...
//  };
//  class putc_iterator {
//      using iterator_category = std::output_iterator_tag;
//      // ...
//  };
//  template <bool Const> class list_of_ints_iterator {
//      using iterator_category = std::forward_iterator_tag;
//      // ...
//  };
//
// 여기서 <iterator> 헤더파일에
// 다음과 같은 five standard tag types가 정의되었다.
// struct input_iterator_tag {};
// struct output_iterator_tag {};
// struct forward_iterator_tag : public input_iterator_tag {};
// struct bidirectional_iterator_tag : public forward_iterator_tag {};
// struct random_access_iterator_tag : public bidirectional_iterator_tag {};
//
// 다음과 같은 사실에 주목해라.
// The conceptual hierarchy of iterator kinds is reflected in the
// the class hierarchy of iterator_category tag classes.
// 이는 템플릿 메타프로그래밍에서 tag를 dispatch할 때 유용하다.
// 예를들어, random_access_iterator_tag 타입은 다음 함수에 넘길 수 있다.
void foo(std::bidirectional_iterator_tag t [[maybe_unused]])
{
    puts("std::vector's iterators are indeed bidirectional...");
}
void bar(std::random_access_iterator_tag)
{
    puts("...and random-access, too!");
}
void bar(std::forward_iterator_tag)
{
    puts("forward_iterator_tag is not as good a match");
}
void test2()
{
    using It = std::vector<int>::iterator;
    foo(It::iterator_category {});
    bar(It::iterator_category {});
}

// 그런데 int*과 같은 class type이 아닌
// primitive scalar type은 class가 아니기 때문에
// member 자체를 만들 수가 없다.
// 이러한 문제는 간접층을 추가해서 해결한다.
// 곧바로 T::iterator_category로 구현하는 것이 아닌,
// 항상 std::iterator_traits<T>::iterator_category를 통해서
// 구현한다.
// 클래스 템플릿 std::iterator_traits<T>는 T가 포인터 타입일때
// 적절하게 specialization한다.
// std::iterator_traits<T>는 다음과 같은 five member typedefs을 제공한다.
// 1. iterator_category
// 2. difference_type
// 3. value_type
// 4. pointer
// 5. reference
}

//-----------------------------------------------------------------------------
// [Putting it all together]
//-----------------------------------------------------------------------------
namespace section7
{
struct list_node {
    int        data;
    list_node* next;
};

template <bool Const>
class list_of_ints_iterator
{
    friend class list_of_ints;
    friend class list_of_ints_iterator<!Const>;

    using node_pointer =
        std::conditional_t<Const, const list_node*, list_node*>;
    node_pointer ptr_;

    explicit list_of_ints_iterator(node_pointer p)
        : ptr_(p)
    { }
public:
    // Member typedefs required by std::iterator_traits
    using difference_type   = std::ptrdiff_t;
    using value_type        = int;
    using pointer           = std::conditional_t<Const, const int*, int*>;
    using reference         = std::conditional_t<Const, const int&, int&>;
    using iterator_category = std::forward_iterator_tag;

    reference operator*() const { return ptr_->data; }
    auto&     operator++()
    {
        ptr_ = ptr_->next;
        return *this;
    }
    auto operator++(int)
    {
        auto result = *this;
        ++*this;
        return result;
    }

    // Support comparison between iterator and const_iterator types
    template <bool R>
    bool operator==(const list_of_ints_iterator<R>& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }
    template <bool R>
    bool operator!=(const list_of_ints_iterator<R>& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    // Support implicit conversion of iterator to const_iterator
    // (but not vice versa)
    operator list_of_ints_iterator<true>() const
    {
        return list_of_ints_iterator<true> {ptr_};
    }
};

class list_of_ints
{
    list_node* head_ = nullptr;
    list_node* tail_ = nullptr;
    int        size_ = 0;
public:
    using const_iterator = list_of_ints_iterator<true>;
    using iterator       = list_of_ints_iterator<false>;

    // Begin and end member functions
    iterator       begin() { return iterator {head_}; }
    iterator       end() { return iterator {nullptr}; }
    const_iterator begin() const { return const_iterator {head_}; }
    const_iterator end() const { return const_iterator {nullptr}; }

    // Other member operations
    int  size() const { return size_; }
    void push_back(int value)
    {
        list_node* new_tail = new list_node {value, nullptr};
        if (tail_) {
            tail_->next = new_tail;
        } else {
            head_ = new_tail;
        }
        tail_ = new_tail;
        size_ += 1;
    }
    ~list_of_ints()
    {
        for (list_node *next, *p = head_; p != nullptr; p = next) {
            next = p->next;
            delete p;
        }
    }
};

template <typename Iterator>
auto distance(Iterator begin, Iterator end)
{
    using Traits = std::iterator_traits<Iterator>;
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                    typename Traits::iterator_category>) {
        return end - begin;
    } else {
        auto result = typename Traits::difference_type {};
        for (auto it = begin; it != end; ++it) {
            ++result;
        }
        return result;
    }
}

template <typename Iterator, typename Predicate>
auto count_if(Iterator begin, Iterator end, Predicate pred)
{
    using Traits = std::iterator_traits<Iterator>;
    auto sum     = typename Traits::difference_type {};
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            ++sum;
        }
    }
    return sum;
}

void test()
{
    list_of_ints lst;
    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);
    auto s = count_if(lst.begin(), lst.end(), [](int i) { return i >= 2; });
    assert(s == 2);
    auto d = distance(lst.begin(), lst.end());
    assert(d == 3);
}
// 다음 장부터는 직접 function templates을 구현하지 않고
// STL에서 제공하는 것들을 사용할 것이다.
}

//-----------------------------------------------------------------------------
// [The deprecated std::iterator]
//-----------------------------------------------------------------------------
namespace section8
{
// 모든 iterator class는 five member typedefs를 제공해야하므로
// 이를 도와주는 a helper class template를 생각할 수 있다.
// 그것이 바로 std::iterator인데
// 생각만큼 사용하기에 단순하지 않아서 c++17부터 deprecated 되었다.
//      namespace std
//      {
//      template <class Category, class T,
//                class Distance = std::ptrdiff_t,
//                class Pointer = T*, class Reference = T&>
//      struct iterator {
//          using iterator_category = Category;
//          using value_type        = T;
//          using difference_type   = Distance;
//          using pointer           = Pointer;
//          using reference         = Reference;
//      };
//      }
//      class list_of_ints_iterator
//          : public std::iterator<std::forward_iterator_tag, int>
//      {
//          // ...
//      };

// const iterators에 대해 const-correctness가 성립해야 하므로
// 다음과 같이 작성해야한다.
// template <bool Const,
//          class Base =
//              std::iterator<std::forward_iterator_tag, int, std::ptrdiff_t,
//                            std::conditional_t<Const, const int*, int*>,
//                            std::conditional_t<Const, const int&, int&>>>
// class list_of_ints_iterator : public Base
//{
//    using typename Base::reference; // Awkward!
//    using node_pointer =
//        std::conditional_t<Const, const list_node*, list_node*>;
//    node_pointer ptr_;
// public:
//    reference operator*() const { return ptr_->data; }
//    // ...
//};

// 위의 코드는 읽거나 작성하기에 쉽지 않다.
// 게다가 std::iterator를 사용하는 방식이 public inheritance이기 때문에
// classical oo class hierarchy 같아 보인다.
// 그래서 처음 접하는 사람은 다음과 같이 class hierarchy를 사용하는 것으로
// 의도할 지 모른다.
//  template <typename... Ts, typename Predicate>
//  int count_if(const std::iterator<Ts...>& begin,
//               const std::iterator<Ts...>& end,
//               Predicate pred);
// 이렇게 작성하면 polymorphic programming을 우연히 유발해서
// 올바르지 않고 잘못된 방법이다. 왜냐면 base class를 참조하게 되서
// derived class에 대해 전혀 다른 결과를 만든다.
//
// 따라서 이러한 이유들로 std::iterator는 더이상 사용되지 않는다.

// Boost의 boost::iterator_facade 베이스 클래스는 std::iterator와
// 다르게 작성하기 귀찮은 default functionality를 제공한다.
// 예를 들어 operator++(int), operator!=를 제공한다.
// 이들은 primitive member functions (dereference, increment, and
// equal)을 토대로 정의된다.

// boost::iterator_facade를 사용하기 위해서, 이를 상속하고
// primitive private member functions을 정의하고
// friend class boost::iterator_core_access;를 넣어서 접근가능하게 한다.
// 즉, 다음과 같이 작성한다.
//    #include <boost/iterator/iterator_facade.hpp>
//    template <bool Const> class list_of_ints_iterator
//       : public boost::iterator_facade
//                       <list_of_ints_iterator<Const>,
//                        std::conditional_t<Const, const int, int>,
//                        std::forward_iterator_tag>
//    {
//        friend class boost::iterator_core_access;
//        friend class list_of_ints;
//        friend class list_of_ints_iterator<!Const>;
//
//        using node_pointer =
//            std::conditional_t<Const, const list_node*, list_node*>;
//        node_pointer ptr_;
//
//        explicit list_of_ints_iterator(node_pointer p)
//            : ptr_(p)
//        { }
//
//        auto& dereference() const { return ptr_->data; }
//        void  increment() { ptr_ = ptr_->next; }
//
//        // Support comparison between iterator and const_iterator types
//        template <bool R>
//        bool equal(const list_of_ints_iterator<R>& rhs) const
//        {
//            return ptr_ == rhs.ptr_;
//        }
//    public:
//        // Support implicit conversion of iterator to const_iterator
//        // (but not vice versa)
//        operator list_of_ints_iterator<true>() const
//        {
//            return list_of_ints_iterator<true> {ptr_};
//        }
//    };
//
// boost::iterator_facade의 첫번째 템플릿 매개변수는
// 항상 정의하고자 하려는 클래스이다. 이러한 패턴을
// the Curiously Recurring Template Pattern이라고 부른다.
//
// 위의 예시에서 ForwardIterator에 대해 2개의 관계 연산자를 대신 구현해주었다.
// 만약 RandomAccessIterator이었다면 상당히 많은 연산자들을 대신 구현해준다.
// 이는 the single primitive member function: distance_to를 토대로 구현된다.
}