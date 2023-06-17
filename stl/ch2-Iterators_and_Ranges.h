#pragma once

// ch2���� �ʿ��� ������ϵ��� ������ ����.
#include <vector>
#include <cassert>

//-----------------------------------------------------------------------------
// [The problem with integer indices]
//-----------------------------------------------------------------------------
namespace section1
{
// ���� �忡�� ������ ���� �����̳� ��� �˰�����
// array_of_ints�� std::vector�� ���ؼ� �ո������� ����������
// list_of_ints�� ���ؼ��� ���ո����̴�.
template <typename Container>
void double_each_element(Container& arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}
// ������ size()�� at()�� ���� �����̴�.
// list_of_ints::at()�� O(n)�̹Ƿ� ���� �˰����� O(n^2)�� �ȴ�.
// ���� at() with integer indices�� �ش� �˰��� ����
// �����̳ʿ� ���� ��ȿ������ ��찡 �����Ѵ�.
// �׷��ٸ� �����̳ʸ��� �����͸� ��� �ٷ��� ȿ������ �˰�����
// �ۼ��� �� ������?
// ��ǻ�Ͱ� �����͸� �ٷ�� �ٽ����� ������ �����͸� �������� ���캻��.
}

//-----------------------------------------------------------------------------
// [On beyond pointers]
//-----------------------------------------------------------------------------
namespace section2
{
// �ڷᱸ���� �������� ���� �迭�� �� ����, ��ũ�� ����Ʈ�� �� ����, �Ǵ�
// Ʈ���� �� ���Ҹ� ��� �����ϰ� �ٷ�°�? ���� ������ ����� �޸� ����
// ������ �ּҸ� ����Ű�� �����͸� ����ϴ� ���̴�.

// ��ũ�� ����Ʈ�� ���� ������ ���ϴ� �˰�����
// c���� ������ ���� �ۼ��Ѵ�.
// for (node *p = lst.head_; p != nullptr; p = p->next) {
//      if (pred(p->data)) {
//          sum += 1;
//      }
// }
// p = p->next�� �ش��ϴ� �κ��� ������ p�� incrementing��Ű�� �Ͱ� ����.
// �̷��� ������ the concept of incrementing �̶� ����.

// c++������ the concept of incrementing�� ++operator�� �����ε��Ͽ�
// p = p->next�� �����Ǵ� �κ��� ++p�� �ۼ��Ѵ�.

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
// ���� list iterator ���ÿ��� count_if�� �Ű����� Ÿ����
// const Container&���� Container&�� �ٲ����,
// begin(), end()�� non-const�̴�. �׷��� *it��
// int&�� �����ϴµ�, ������ count_if�� ���� �����Ű�� �����Ƿ�
// const int&�� �����ϴ� begin(), end()�� �����ϴ�.
// ���� const list_of_ints�� �����Ͽ� ����ϰ� ������
// operator*�� ������ ���� ������ *it�� int&�� �����Ѵ�.
// �׷��� operator*�� const int&�� �����ϴ� ������ �־�� ������
// �̸� �����ε��� �� ����. �׷��� const int&���� �����ϴ� ������
// �����ϸ� �̹����� ���� ������ ���� ������ �� ���� �ȴ�.

// ���� ���� ������ �ذ��ϱ� ���� STL�� ���� �ٸ� 2���� iterator:
// bag::iterator, bag::const_iterator�� ����Ѵ�. (���⼭ bag�� ������ �̸�)
// non-const bag::begin()�� iterator�� �����ϰ�,
// bag::begin() const�� const_iterator�� �����Ѵ�.
//
// const_iterator�� const iterator�� ���� �ٸ��Ƿ� �����ؾ� �Ѵ�.
// const iterator�� ++������ �Ұ����ϰ� ���� *������ �������� non-const�̴�.
// const_iterator�� ++������ �����ϰ� ���� *������ �������� const�̴�.

// ���� const_iterator�� �����ϴµ�, iterator�� �ڵ��ߺ��� �����Ƿ�
// ���ø��� ���ؼ� ȿ�������� �ۼ��Ѵ�.

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
// ������ ������ ���캸��.
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

    // ��� ���Ҹ� 2��� �����.
    double_each_element(v.begin(), v.end());
    // ���⼭ std::vector::iterator�� ����ߴ�.

    // ������ ���Ҹ� ������ ���� �ִ�.
    double_each_element(v.begin(), v.begin() + 3);
    // ���⵵ std::vector::iterator�� ����ߴ�.

    // ���� �����ѵ�,
    double_each_element(&v[0], &v[3]);
    // ����� int*�� ����ߴ�.
    // int*�� Iterator�� �ʿ����ǵ��� ��� �����ϱ� ������
    // ������ Ÿ�Ը����� �����ϴ� ���̴�.
    // �̷����� iterator-pair model�� ����� �������� �� �� �ִ�.
    // ������ �̷��� raw pointers�� ����ϴ� ����� ���ؾ� �Ѵ�.
    // ������ iterator Ÿ���� ����ؾ� �ùٸ���.

    // a pair of iterators�� a range of data elements�� �����Ѵ�.
    // ã�� �Ǵ� ��ȯ�� �ϱ� ���ؼ�, ���������� a container�� ������
    // �ʿ䰡 ����. ���� the particular range of element�� �ʿ��ϴ�.
    // �̴� ���߿� ������ a non-owning view�̶� ������ �̲���.

    // double_each_element�� Iterator��
    // incrementability, comparability, and dereferenceability
    // �� �����ϴ� ��� Ÿ���̵� generic�ϰ� ������ ���̴�.
}
}

//-----------------------------------------------------------------------------
// [Iterator categories]
//-----------------------------------------------------------------------------
namespace section5
{
// ������ count�� ������ std::distance�� ������ ���캸��.
template <typename Iterator>
int distance(Iterator begin, Iterator end)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        sum += 1;
    }
    return sum;
}

// �׸��� std::count_if�� ������ ���캸��.
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

    // ���� distance�� ������ vector�� ���ؼ� ��ȿ�������� ����Ѵ�.
    int total = section5::distance(v.begin(), v.end());
    // O(1)�� ������ �˰����� O(n)���� ����ع�����.

    assert(number_above == 2);
    assert(number_below == 5);
    assert(total == 8);
}

// ����, std::distance�� ������
// a template specialization�� �����Ѵ�.
template <>
int distance(int* begin, int* end)
{
    return static_cast<int>(end - begin);
}

// �׷���, �츮�� int*�� std::vector::iterator�� ���ؼ���
// specilaization�� �ϴ� ���� �ƴ϶�
// Ư�� ������ �����ϴ� ��� �ݺ��� Ÿ�Ե鿡 ����
// ȿ�������� �����ϰ��� �Ѵ�.
// �׷��� �ݺ����� �������� �����Ѵ�.
// STL���� �����ϴ� the concepts of Iterator�� ������ ����.
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
// a weaker concept than ForwardIterator�� ������?
// ���� ���, ForwardIterator�� �ڽ��� �����ϰų�
// �����͵��� 2���ؼ� ������ �� ���� ���̴�.
// iterartor�� �ٷ�� �͸����� �� ������ ���ϴ� �����͵���
// ���� ������ ���� �ʱ� ������ ������ ���� iterator��
// ����� �� �ִ�. �̰��� �ݺ��ڰ� ����Ű�� �����Ͱ� ����
// ��ſ� data member�� ���� ������, �ݺ����� ������ ���ǹ��ϴ�.
// not meaningfully copyable, do not point to data elements
// in any meaningful sense�� iterator�� InputIterator ��� �Ѵ�.
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
// std::istream_iterator�� ���� �����ϴ�.

// ���� �����Ǵ� iterator�� ����� �� �ִ�.
// not meaningfully copyable, writeable-into but not readable-out-of
// �� iterator�� OutputIterator ��� �Ѵ�.
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

// C++�� ��� iterator Ÿ���� ��� ������ five categories �߿� �ϳ��� ���Ѵ�.
// 1. InputIterator
// 2. OutputIterator
// 3. ForwardIterator
// 4. BidirectionalIterator
// 5. RandomAccessIterator
//
// ������ �ð��� Ư�� iterator type�� BidirectionalIterator �Ǵ�
// RandomAccessIterator���� �˾Ƴ��� ����. �׷���,
// InputIterator, OutputIterator, �Ǵ� ForwardIterator����
// �˾Ƴ���� �Ұ����ϴ�.
//
// �ֳ��ϸ�, �������
// getc_iterator, putc_iterator, list_of_ints::iterator ���
// ��Ȯ�� ���� syntactic operations(dereferencing with* it,
// incrementing with ++it, and comparison with it != it)��
// �����ϱ� ������ ���� semantic level������ ���еǱ� �����̴�.
//
// �׷��ٸ� STL�� ��� �̵��� �����ұ�?
// iterator class���� iterator_category�� a member typedef�� �����Ѵ�.
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
// ���⼭ <iterator> ������Ͽ�
// ������ ���� five standard tag types�� ���ǵǾ���.
// struct input_iterator_tag {};
// struct output_iterator_tag {};
// struct forward_iterator_tag : public input_iterator_tag {};
// struct bidirectional_iterator_tag : public forward_iterator_tag {};
// struct random_access_iterator_tag : public bidirectional_iterator_tag {};
//
// ������ ���� ��ǿ� �ָ��ض�.
// The conceptual hierarchy of iterator kinds is reflected in the
// the class hierarchy of iterator_category tag classes.
// �̴� ���ø� ��Ÿ���α׷��ֿ��� tag�� dispatch�� �� �����ϴ�.
// �������, random_access_iterator_tag Ÿ���� ���� �Լ��� �ѱ� �� �ִ�.
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

// �׷��� int*�� ���� class type�� �ƴ�
// primitive scalar type�� class�� �ƴϱ� ������
// member ��ü�� ���� ���� ����.
// �̷��� ������ �������� �߰��ؼ� �ذ��Ѵ�.
// ��ٷ� T::iterator_category�� �����ϴ� ���� �ƴ�,
// �׻� std::iterator_traits<T>::iterator_category�� ���ؼ�
// �����Ѵ�.
// Ŭ���� ���ø� std::iterator_traits<T>�� T�� ������ Ÿ���϶�
// �����ϰ� specialization�Ѵ�.
// std::iterator_traits<T>�� ������ ���� five member typedefs�� �����Ѵ�.
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
// ���� ����ʹ� ���� function templates�� �������� �ʰ�
// STL���� �����ϴ� �͵��� ����� ���̴�.
}

//-----------------------------------------------------------------------------
// [The deprecated std::iterator]
//-----------------------------------------------------------------------------
namespace section8
{
// ��� iterator class�� five member typedefs�� �����ؾ��ϹǷ�
// �̸� �����ִ� a helper class template�� ������ �� �ִ�.
// �װ��� �ٷ� std::iterator�ε�
// ������ŭ ����ϱ⿡ �ܼ����� �ʾƼ� c++17���� deprecated �Ǿ���.
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

// const iterators�� ���� const-correctness�� �����ؾ� �ϹǷ�
// ������ ���� �ۼ��ؾ��Ѵ�.
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

// ���� �ڵ�� �аų� �ۼ��ϱ⿡ ���� �ʴ�.
// �Դٰ� std::iterator�� ����ϴ� ����� public inheritance�̱� ������
// classical oo class hierarchy ���� ���δ�.
// �׷��� ó�� ���ϴ� ����� ������ ���� class hierarchy�� ����ϴ� ������
// �ǵ��� �� �𸥴�.
//  template <typename... Ts, typename Predicate>
//  int count_if(const std::iterator<Ts...>& begin,
//               const std::iterator<Ts...>& end,
//               Predicate pred);
// �̷��� �ۼ��ϸ� polymorphic programming�� �쿬�� �����ؼ�
// �ùٸ��� �ʰ� �߸��� ����̴�. �ֳĸ� base class�� �����ϰ� �Ǽ�
// derived class�� ���� ���� �ٸ� ����� �����.
//
// ���� �̷��� ������� std::iterator�� ���̻� ������ �ʴ´�.

// Boost�� boost::iterator_facade ���̽� Ŭ������ std::iterator��
// �ٸ��� �ۼ��ϱ� ������ default functionality�� �����Ѵ�.
// ���� ��� operator++(int), operator!=�� �����Ѵ�.
// �̵��� primitive member functions (dereference, increment, and
// equal)�� ���� ���ǵȴ�.

// boost::iterator_facade�� ����ϱ� ���ؼ�, �̸� ����ϰ�
// primitive private member functions�� �����ϰ�
// friend class boost::iterator_core_access;�� �־ ���ٰ����ϰ� �Ѵ�.
// ��, ������ ���� �ۼ��Ѵ�.
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
// boost::iterator_facade�� ù��° ���ø� �Ű�������
// �׻� �����ϰ��� �Ϸ��� Ŭ�����̴�. �̷��� ������
// the Curiously Recurring Template Pattern�̶�� �θ���.
//
// ���� ���ÿ��� ForwardIterator�� ���� 2���� ���� �����ڸ� ��� �������־���.
// ���� RandomAccessIterator�̾��ٸ� ����� ���� �����ڵ��� ��� �������ش�.
// �̴� the single primitive member function: distance_to�� ���� �����ȴ�.
}