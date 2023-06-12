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
#include <numeric>

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
// �ٸ� ������� ���� ������ ���Ҵ� back_inserter�� ����� ����� �ִ�.
// 2. std::move_iterator adaptor class
template <class It>
class move_iterator
{
    using OriginalRefType = typename std::iterator_traits<It>::reference;
    It iter;
public:
    using iterator_category =
        typename std::iterator_traits<It>::iterator_category;
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using value_type      = typename std::iterator_traits<It>::value_type;
    using pointer         = It;
    using reference =
        std::conditional_t<std::is_reference_v<OriginalRefType>,
                           std::remove_reference_t<OriginalRefType>&&,
                           OriginalRefType>;

    move_iterator() = default;
    explicit move_iterator(It it)
        : iter(std::move(it))
    { }

    // Allow constructing or assigning from any kind of move-iterator.
    // These templates also serve as our own type's copy constructor
    // and assignment operator, respectively.
    template <class U>
    move_iterator(const move_iterator<U>& m)
        : iter(m.base())
    { }
    template <class U>
    auto& operator=(const move_iterator<U>& m)
    {
        iter = m.base();
        return *this;
    }

    It             base() const { return iter; }
    It             operator->() { return iter; }
    reference      operator*() { return static_cast<reference>(*iter); }
    decltype(auto) operator[](difference_type n) const
    {
        return *std::move(iter[n]);
    }
    auto& operator++()
    {
        ++iter;
        return *this;
    }
    auto& operator++(int)
    {
        auto result = *this;
        ++*this;
        return result;
    }
    auto& operator+=(difference_type n) const
    {
        iter += n;
        return *this;
    }
    auto& operator-=(difference_type n) const
    {
        iter -= n;
        return *this;
    }
};
// �����ϴ� �ɹ� �Լ����� ������ �̸� ������ �� ���� It�� Ÿ�Ե鿡 ���ؼ�
// �ش� �ɹ� �Լ��� �ڵ忡�� ����ϸ� ������ ������ �߻��Ѵ�.
// ������ ������� ������ �ν��Ͻ�ȭ���� �����Ƿ� ���� ����.

// back_inserter������ ����� ���������� ���� helper function�� �����Ѵ�.
// ��, Ÿ������ ������Ƿ� make_pair�� make_tuple���� �����.
template <class InputIterator>
auto make_move_iterator(InputIterator& c)
{
    return move_iterator(c);
}

// We have two different ways of moving data from one container or
// range to another: 1.std::move algorithm, 2.std::move_iterator adaptor class.
// ���� ������ ����.
void example()
{
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    // First approach: use the std::move algorithm
    std::move(input.begin(), input.end(), output.begin());

    // Second approach: use move_iterator
    std::copy(std::move_iterator(input.begin()),
              std::move_iterator(input.end()), output.begin());
}
// std::copy���� std::move�� ����ϴ� ����� �� ��Ȯ�ϴ�.
// STL���� move_iterator �� �����ϴ� ������ ���� ������ ������
// std::copy�� ���õ� �˰��� ���Ǳ� �����̴�.
}

//-----------------------------------------------------------------------------
// [Complicated copying with std::transform]
//-----------------------------------------------------------------------------
namespace section4
{
// std::copy�� �������� 2���� iterator type parameters�� �޴´�.
// �׷��� ������ value_type�� ���� �ٸ� �� �ִµ� �̴� �Ͻ��� ��ȯ�� �̿��� �ڵ�
// �ۼ��� �����ϰ� �Ѵ�. ������ ����.
void example1()
{
    std::vector<const char*> input = {"hello", "world"};
    std::vector<std::string> output(2);

    std::copy(input.begin(), input.end(), output.begin());

    assert(output[0] == "hello");
    assert(output[1] == "world");
}
// *input.begin()�� const char * Ÿ�Կ���
// *output.begin()�� std::string Ÿ������ �Ͻ��� ĳ���� �����ڰ� ȣ��ȴ�.
// �̷� ������ copy���� �������� ĳ�����ϴµ� ���� ������ transformation �Լ���
// �ʿ��� ���� �ִ�.
// ���� std::transform �� ����.
template <class InIt, class OutIt, class Unary>
OutIt transform(InIt first1, InIt last1, OutIt destination, Unary op)
{
    while (first1 != last1) {
        *destination = op(*first1);
        ++first1;
        ++destination;
    }
    return destination;
}

void example2()
{
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    std::transform(input.begin(), input.end(), output.begin(),
                   [](std::string s) {
                       std::transform(s.begin(), s.end(), s.begin(), ::toupper);
                       return s;
                   });

    assert(input[0] == "hello");
    assert(output[0] == "HELLO");
}

// ������ two arguments�� �޴� transformation�� �ʿ��� ���� �ִ�.
template <class InIt1, class InIt2, class OutIt, class Binary>
OutIt transform(InIt1 first1, InIt1 last1, InIt2 first2, InIt2 last2,
                OutIt destination, Binary op)
{
    while (first1 != last1) {
        *destination = op(*first1, *first2);
        ++first1;
        ++first2;
        ++destination;
    }
    return destination;
}

// �����͸� �̵���Ű�� 3��° ����� ������ ����.
void example3()
{
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    // Third approach: use std::transform
    std::transform(input.begin(), input.end(), output.begin(),
                   std::move<std::string&>);
}
// ������ std::move ���ø��� explicit specialization�� �����ϰ� �ȴ�.
// ������ ��� explicit specialization--those angle brackets after the
// template's name-- �� �����Ѵٴ� ���� �ſ� �̹��ϰ� ����� �ڵ��� Ȯ����
// ��ȣ�̴�.
}

//-----------------------------------------------------------------------------
// [Write-only range algorithms]
//-----------------------------------------------------------------------------
namespace section5
{
// There is a family of standard algorithms that march through a range modifying
// each element without reading it.

// std::fill(a,b,v)�� [a,b)�� ��� ���Ҹ� v�� ���纻���� ä���.
// std::iota(a,b,v)�� [a,b)�� ��� ���Ҹ� ù°�� v, ������ 1�� ���������� ä���.
// std::generate(a,b,g)�� [a,b)�� ��� ���Ҹ� g(void)�� ���ϰ����� ä���.
// (���⼭ std::iota�� <numeric>�� ���ԵǾ� �ִ�.)
template<class FwdIt, class T>
void fill(FwdIt first, FwdIt last, T value)
{
    while (first != last) {
        *first = value;
        ++first;
    }
}
template<class FwdIt, class T>
void iota(FwdIt first, FwdIt last, T value)
{
    while (first != last) {
        *first = value;
        ++value;
        ++first;
    }
}
template<class FwdIt, class G>
void generate(FwdIt first, FwdIt last, G generator)
{
    while (first != last) {
        *first = generator();
        ++first;
    }
}

// ���� ���ø� ����.
void example()
{
    std::vector<std::string> v(4);

    std::fill(v.begin(), v.end(), "hello");
    assert(v[0] == "hello");
    assert(v[1] == "hello");
    assert(v[2] == "hello");
    assert(v[3] == "hello");

    std::iota(v.begin(), v.end(), "hello");
    assert(v[0] == "hello");
    assert(v[1] == "ello");
    assert(v[2] == "llo");
    assert(v[3] == "lo");

    std::generate(v.begin(), v.end(), [i = 0]() mutable {
        return ++i % 2 ? "hello" : "world";
    });
    assert(v[0] == "hello");
    assert(v[1] == "world");
    assert(v[2] == "hello");
    assert(v[3] == "world");
}
}


//-----------------------------------------------------------------------------
// [Algorithms that affect object lifetime]
//-----------------------------------------------------------------------------
namespace section6
{

}

