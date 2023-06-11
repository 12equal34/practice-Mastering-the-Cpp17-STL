#pragma once

// ch3���� �ʿ��� ������ϵ��� ������ ����.
#include <algorithm>
#include <list>
#include <forward_list>
#include <cassert>
#include <set>
#include <unordered_set>
#include <iterator>
#include <iostream>
#include <type_traits>
#include <boost/iterator/iterator_facade.hpp>

//-----------------------------------------------------------------------------
// [Read-only range algorithms]
//-----------------------------------------------------------------------------
namespace section1
{
// "between a and b"��� ���� [a,b) ��� ���̴�.
// �Ϲ������� end point of a range(���⼭�� *b)�� �������ϴ� ���� �����ϴ�.
// half-open ranges�� empty ranges�� �� ǥ�����ش�. (from x to x �� empty)
// �� ����� ������ for-loop�� range�� ����. �Ʒ� ���õ��� ����.
void example1()
{
    constexpr int N = 10;
    int           a[N];

    // A correct for-loop
    for (int i = 0; i < N; ++i) {
        // ...
    }

    // One variety of "smelly" for-loop.
    for (int i = 0; i <= N; ++i) {
        // ...
    }

    // A correct invocation of a standard algorithm.
    auto correct =
        std::count_if(std::begin(a), std::end(a), [](int) { return true; });

    // A "smelly" invocation.
    auto smelly =
        std::count_if(std::begin(a), std::end(a) - 1, [](int) { return true; });

    // A "trivial" invocation: counting a range of length zero.
    auto zero =
        std::count_if(std::begin(a), std::begin(a), [](int) { return true; });
}
// std::distance(a,b)�� ++������ a���� b���� ���� ���
// std::count_if(a,b,[](auto&&){return true;})�� �����ϴ�.
// ���� random-access iterators�� ���� �ٷ� (b-a)�� ����ϹǷ� std::distance��
// ����. �׷��� (b-a)�� �߸��� ������ �����ϸ� ������ ���´�.
void example2()
{
    int                    a[] {1, 2, 3, 4, 5};
    std::list<int>         lst {1, 2, 3, 4, 5};
    std::forward_list<int> flst {1, 2, 3, 4, 5};

    assert(std::distance(std::begin(a), std::end(a)) == 5);
    assert(std::distance(std::begin(lst), std::end(lst)) == 5);
    assert(std::distance(std::begin(flst), std::end(flst)) == 5);

    assert(std::distance(std::end(a), std::begin(a)) == -5);
}
// ������ bidirectional iterators�� �߸��� ������ �����ϸ� ������ �߻��Ѵ�.
// std::distance(b,a) == -std::distance(a,b)�� ��������� std::distance��
// �˰����� �ش� �ݺ��ڰ� �߸��� �������� �ƴ��� �𸣱� ������ segfault��
// �߻��Ѵ�. (Segmentation fault : ���α׷��� ������ ���� �޸� ������ ������
// �õ��� �� �߻��ϴ� �����̴�.)

// std::count(a,b,v)�� the number of elements e for which e == v is true��
// �����ϴµ�, �̴� std::count_if(a,b,[&v](auto&& e){return e==v;}) �� �����ϴ�.
// ������ ���� ����� �ڵ带 �����Ѵ�. ���� ���� ǥ������ 1998�⿡ �־��ٸ�
// std::count�� ���� ���̺귯���� �������� �ʾ��� ���̴�.

// std::count(a,b,v)�� [a,b)�� ��� ���Ҹ� �����ϹǷ� Ư���� ������ �迭�Ǿ�
// �ִ� ��쿡 �־ ��ȿ�����̴�. ���� ���ø� ���캸��.
void example3()
{
    std::set<int>           s {1, 2, 3, 10, 42, 99};
    std::unordered_set<int> us {1, 2, 3, 10, 42, 99};
    bool                    present;

    // O(n): compare each element with 42
    present = std::count(s.begin(), s.end(), 42);

    // O(log n): ask the container to look up 42 itself
    present = s.count(42);

    // O(1): ask the container to look up 42 itself
    present = us.count(42);
}
// the standard algorithms�� �ڽ����� �ۼ��� �ڵ��� ������ ����,
// ����� the underlying data elements�� ���踦 ���� ������ �ٲ��� �ʴ´�.
// �̷��� ���踦 ���� ȿ������ �ڵ带 �ۼ��ϴ� �� ���α׷����� �츮�� ���̴�.

// std::find(a,b,v), std::find_if(a,b,p)�� ����
// std::count(a,b,v), std::count_if(a,b,p)�� ����ϰ� �۵��Ѵ�.
// �ٸ� find, find_if�� ù��°�� ��ġ�� ���Ҹ� ã���� �ٷ� �ݺ��ڸ� �����Ѵ�.
// (find�� ������ ���� ǥ������ �־��ٸ� ���̺귯���� ���Ե��� �ʾ��� ���̴�.)
template <class InputIterator, class UnaryPredicate>
InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate p)
{
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }
    return last;
}
// (find_if�� �ٽ����� "short-circuiting"�� �����̴�.)
template <class It, class U>
It find_if_not(It first, It last, U p)
{
    return std::find_if(first, last, [&](auto&& e) { return !p(e); });
}
template <class It, class T>
It find(It first, It last, T value)
{
    return std::find_if(first, last, [&](auto&& e) { return e == value; });
}
// find algorithms�� return immediately�� �����̴�.
// �̷��� return immediately�� �������� "short-circuiting"�� ������ ���ش�.
template <class It, class UnaryPredicate>
bool all_of(It first, It last, UnaryPredicate p)
{
    return std::find_if_not(first, last, p) == last;
}
template <class It, class U>
bool any_of(It first, It last, U p)
{
    return std::find_if(first, last, p) != last;
}
template <class It, class U>
bool none_of(It first, It last, U p)
{
    return std::find_if(first, last, p) == last;
}

// find_first_of(haystack,haystack, needle, needle) �˰����� 2���� range:
// the range to search in, and the set of target elements�� �޴´�.
template <class It, class FwdIt>
It find_first_of(It first, It last, FwdIt targetfirst, FwdIt targetlast)
{
    return std::find_if(first, last, [&](auto&& e) {
        return std::any_of(targetfirst, targetlast,
                           [&](auto&& t) { return e == t; });
    });
}
template <class It, class FwdIt, class BinaryPredicate>
It find_first_of(It first, It last, FwdIt targetfirst, FwdIt targetlast,
                 BinaryPredicate p)
{
    return std::find_if(first, last, [&](auto&& e) {
        return std::any_of(targetfirst, targetlast,
                           [&](auto&& t) { return p(e, t); });
    });
}
// ���⼭ "haystack" iterators�� ��� InputIterator type�� �����ϰ�,
// "needle" iterators�� �ּ��� ForwardIterator type�� �����Կ� �ָ��϶�.

// ForwardIterator�� �߿��� ���� �ǹ̷������� ������ �����ϸ�,
// ���� range�� �ݺ��ڰ� �ݺ� ����(traverse)�� �� �ִٴ� ���̴�.
// ���⼭ needle�� �ݵ�� re-traversable�̰� ������ ũ�⿩�� �Ѵ�.
// �ݴ�� haystack�� ������ ũ���� �ʿ䰡 ����.
// ���� ���, haystack���� ���������� ������ ���� (potentially unbounded)
// input stream�� ���ҵ��� ������ �� �ִ�. ���� ���ø� ����.
void example4()
{
    std::istream_iterator<char> ii(std::cin);
    std::istream_iterator<char> iend {};
    std::string                 s = "hello";

    // std::cin���κ��� 'h','e','l','o'��
    // �ش��ϴ� ���ڸ� ã�� ������ ����ؼ� ���ڸ� �޴´�.
    auto it = std::find_first_of(ii, iend, s.begin(), s.end());
}

// ���������� std::equal �� std::mismatch�� ���� �����ϰڴ�.
// std::equal(a,b,c,d)�� [a,b)�� [c,d)�� ��� ���ҵ��� ���� ���� ��
// true�� �����ϰ� �׷��� ������ false�� �����Ѵ�.
//
// std::mismatch(a,b,c,d)�� find�� �����̸� ������ ����.
template <class It1, class It2, class B>
auto mismatch(It1 first1, It1 last1, It2 first2, It2 last2, B p)
{
    while (first1 != last1 && first2 != last2 && p(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return std::make_pair(first1, first2);
}

template <class It1, class It2>
auto mismatch(It1 first1, It1 last1, It2 first2, It2 last2)
{
    return std::mismatch(first1, last1, first2, last2, std::equal_to<> {});
}

template <class T>
constexpr bool is_random_access_iterator_v =
    std::is_base_of_v<std::random_access_iterator_tag,
                      typename std::iterator_traits<T>::iterator_category>;

template <class It1, class It2, class B>
bool equal(It1 first1, It1 last1, It2 first2, It2 last2, B p)
{
    if constexpr (is_random_access_iterator_v<It1> &&
                  is_random_access_iterator_v<It2>) {
        // Ranges of different lengths can never be equal.
        if ((last2 - first2) != (last1 - first1)) {
            return false;
        }
    }
    return std::mismatch(first1, last1, first2, last2, p) ==
           std::make_pair(last1, last2);
}

template <class It1, class It2>
bool equal(It1 first1, It1 last1, It2 first2, It2 last2)
{
    return std::equal(first1, last1, first2, last2, std::equal_to<> {});
}
// ���⼭ std::equal_to<> {}�� ����ߴµ�, [](auto a, auto b){return a==b;}��
// ������ ������ �ϰ� more perfect forwarding(������ ����)�� �����Ѵ�.

// ����������, ���� two-range algorithms�� one-and-a-half-range algorithms��
// �����Ѵ�. std::mistmatch(a,b,c,d)�� std::mismatch(a,b,c)�� �ۼ��Ѵٸ�
// �ι�° range�� end point�� c + std::distance(a,b)�� �ǹ��Ѵ�.
// ������ ������ ��� �����͸� ������ ���ɼ��� �����Ƿ� c++17��
// the one-and-a-half-range algorithms�� ���� safe two-range variants
// �� �߰��Ͽ���.
}

//-----------------------------------------------------------------------------
// [Shunting data with std::copy]
//-----------------------------------------------------------------------------
// (shunt: to move (someone or something) to a different and usually less
// important or noticeable place or position.)
namespace section2
{
// std::copy�� ������ ������ ����.
template <class InIt, class OutIt>
OutIt copy(InIt first1, InIt last1, OutIt destination)
{
    while (first1 != last1) {
        *destination = *first1;
        ++first1;
        ++destination;
    }
    return destination;
}
// std::copy�� two-range version�� �������� �ʴ´�.
// �׷��� ���� ������ ũ�� üũ�� �̹� �ߴٰ� �����Ѵ�.
// ������ Ʋ���� ���� �����÷ο찡 �߻��Ѵ�.

// destination�� output iterator��� �ߴµ�
// std::copy�� �����͸� �ٸ����� �����ϴ� �뵵�� �ƴ�, �����͸� ������ �Լ���
// �����ϴ� �뵵�� ���� �� �ִ�. ���� ��� ������ ����.
class putc_iterator
    : public boost::iterator_facade<putc_iterator, const putc_iterator,
                                    std::output_iterator_tag>
{
    friend class boost::iterator_core_access;

    auto& dereference() const { return *this; }
    void  increment() { }
    bool  equal(const putc_iterator&) const { return false; }
public:
    // This iterator is its own proxy object!
    void operator=(char ch) const { putc(ch, stdout); }
};
void test1()
{
    std::string s = "hello";
    std::copy(s.begin(), s.end(), putc_iterator {});
}
// �̷��� destination�� ������ ���п� ���� �����÷ο� ������ �ذ��� �� �ִ�.
// ���� ������ �迭�� �ƴ� �������� std::vector�� ���ٰ� ��������.
// �׷��� writing an element�� pushing an element back�� �����Ѵ�.
// output iterator�� putc_iterator�� ����ϰ� �ۼ��ϰ�
// putc ��ſ� push_back�� ����Ѵٸ� �����÷ο찡 �߻����� ���� ���̴�.
// �̷��� iterator�� STL���� �����Ѵ�. <iterator> ������� �ȿ���,
template <class Container>
class back_insert_iterator
{
    using CtrValueType = typename Container::value_type;
    Container* c;
public:
    using iterator_category = std::output_iterator_tag;
    using difference_type   = void;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;

    explicit back_insert_iterator(Container& ctr)
        : c(&ctr)
    { }

    auto& operator*() { return *this; }
    auto& operator++() { return *this; }
    auto& operator++(int) { return *this; }

    auto& operator=(const CtrValueType& v)
    {
        c->push_back(v);
        return *this;
    }
    auto& operator=(CtrValueType&& v)
    {
        c->push_back(std::move(v));
        return *this;
    }
};
template <class Container>
auto back_inserter(Container& c)
{
    return back_insert_iterator<Container>(c);
}
void test2()
{
    std::string       s = "hello";
    std::vector<char> dest;
    std::copy(s.begin(), s.end(), section2::back_inserter(dest));
    assert(dest.size() == 5);
}
// ���⼭ back_inserter_iterator(dest) ��ſ� back_inserter(dest)���
// �ۼ��ߴµ�, �̴� c++14 ������ �̷��� �ۼ��ؾ� �߰� c++17����
// �����ڿ� ���� ���ø� Ÿ���� ���� ��� ���п� ��ٷ�
// back_inserter_iterator(dest) �� �ۼ��ص� �ȴ�.
// ������, c++17 ���ķ�
// std::make_pair ��ſ� std::pair��,
// std::make_tuple ��ſ� std::tuple�� ��ȣ�ϴ� ��ó��
// �� ���� Ÿ�����ϴ� ����� std::back_inserter(dest)�� ��ȣ�ؾ� �Ѵ�.
}

//-----------------------------------------------------------------------------
// [Variations on a theme - std::move and std::move_iterator]
//-----------------------------------------------------------------------------
namespace section3
{
// ���� ������ std::copy�� input range�� ���ҵ��� output���� �����Ѵ�.
// ���⼭ ������ �ƴ� �̵� ������ ����ϴ� ���� �����ұ�?
// �̷��� ������ ���� STL�� 2������ ���ٹ��� �����Ѵ�.
// 
// 1. std::move algorithm defined in <algorithm>
template <class InIt, class OutIt>
OutIt move(InIt first1, InIt last1, OutIt destination)
{
    while (first1 != last1) {
        *destination = std::move(*first1);
        ++first1;
        ++destination;
    }
    return destination;
}
// �̴� std::copy �˰���� std::move �� �߰��� �� ����� ��Ȯ�� ��ġ�Ѵ�.
// ����� ���⼭ std::move ��ƿ��Ƽ �Լ��� <utility>�� ���Ե� �Ű����� 3����
// �ƴ� 1���� �޴� �����ε� �Լ��� ���� std::move �˰���� ���� �ٸ���.
// �̷��� ���� ���ڰ� �̸��� �����ϴ� ��찡 std::remove ���� �����Ѵ�.
//
// �ٸ� ������� ���� ������ ���Ҵ� back_inserter�� �����̸�
// 
// 2. back_inserter
template<class It>
class move_iterator
{

};
}
