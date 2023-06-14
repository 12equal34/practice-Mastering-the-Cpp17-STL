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
// std::iota(a,b,v)�� [a,b)�� ��� ���Ҹ� ù°�� v, ������ 1�� ����������
// ä���. std::generate(a,b,g)�� [a,b)�� ��� ���Ҹ� g(void)�� ���ϰ�����
// ä���. (���⼭ std::iota�� <numeric>�� ���ԵǾ� �ִ�.)
template <class FwdIt, class T>
void fill(FwdIt first, FwdIt last, T value)
{
    while (first != last) {
        *first = value;
        ++first;
    }
}
template <class FwdIt, class T>
void iota(FwdIt first, FwdIt last, T value)
{
    while (first != last) {
        *first = value;
        ++value;
        ++first;
    }
}
template <class FwdIt, class G>
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

    std::generate(v.begin(), v.end(),
                  [i = 0]() mutable { return ++i % 2 ? "hello" : "world"; });
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
// <memory> ������Ͽ� �� �� �˷��� �ִ� �˰�������
// std::uninitialized_copy, std::uninitialized_default_construct,
// and std::destroy �� �ִ�.

// ������ a range�� ��� ���ҵ��� �Ҹ��ϴ� �˰����̴�.
template <class T>
void destroy_at(T* p)
{
    p->~T();
}
template <class FwdIt>
void destroy(FwdIt first, FwdIt last)
{
    for (; first != last; ++first) {
        std::destroy_at(std::addressof(*first));
    }
}
// std::addressof(x)�� x�� �ּҸ� �����ϴ� helper function�̸� x�� Ŭ������
// operator&�� �����ε��� �ſ� �幮 ��찡 �ƴϸ� ��Ȯ�� &x�� �����Ѵ�.

// std::uninitialized_copy��
// a range�� ��� ������ �ڸ��� ���� copy-construct into�ϴ� �˰����̴�.
template <class It, class FwdIt>
FwdIt uninitialized_copy(It first, It last, FwdIt out)
{
    using T       = typename std::iterator_traits<FwdIt>::value_type;
    FwdIt old_out = out;
    try {
        while (first != last) {
            ::new (static_cast<void*>(std::addressof(*out))) T(*first);
            ++first;
            ++out;
        }
        return out;
    } catch (...) {
        std::destroy(old_out, out);
        throw;
    }
}

void test()
{
    alignas(std::string) char b[5 * sizeof(std::string)];
    std::string*              sb = reinterpret_cast<std::string*>(b);

    std::vector<const char*> vec = {"quick", "brown", "fox"};

    // Construct three std::strings.
    auto end = std::uninitialized_copy(vec.begin(), vec.end(), sb);

    assert(end == sb + 3);

    // Destroy three std::strings.
    std::destroy(sb, end);
}
}

//-----------------------------------------------------------------------------
// [Our first permutative algorithm: std::sort]
//-----------------------------------------------------------------------------
namespace section7
{
// �̹� ������ ������� permutation������ �ϴ� algorithms�� ���� �����Ѵ�.
// std::sort(a,b)�� operator<�� ���� ������������ �����Ѵ�.
// operator<�� �����ε��Ͽ� �ٸ� �������� ������ �� ������
// std::sort(a,b,cmp) ������ ����ؾ� �Ѵ�.
void example1()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9};
    std::sort(v.begin(), v.end(),
              [](auto&& a, auto&& b) { return a % 7 < b % 7; });
    assert((v == std::vector {1, 1, 9, 3, 4, 5}));

    std::sort(v.begin(), v.end(),
              [](auto&& a, auto&& b) { return a % 6 < b % 6; });

    assert((v == std::vector {1, 1, 9, 3, 4, 5} ||
            v == std::vector {1, 1, 3, 9, 4, 5}));
}
// ���ĵǴ� ������ ���� �켱������ ���� ��, std::sort�� ������� ��ġ��
// Ȯ���ϰ� �������� ���� �ʰ�, std::stable_sort�� ���� ��������
// the original order�� �����Ͽ� {1,1,3,9,4,5}�� �����Ѵ�.

// cmp�� the comparison relation�� �����ϴ� �Լ����� ���������� ������ �� �ִ�.
// ���� ��� (a % 6 < b) �� �����ϴ� �Լ��� 5�� 9�� ���ؼ�
// cmp(5,9) == cmp(9,5) == true �� �ǹǷ� �� �Լ��� �� �� ����.
}

//-----------------------------------------------------------------------------
// [Swapping, reversing, and partitioning]
//-----------------------------------------------------------------------------
namespace section8
{
// STL�� std::sort ���� ���� permutative algorithms�� �ִ�.
// �� �˰������ ��κ� �ٸ� ���� �˰����� building blocks ������ �Ѵ�.
// �� �� std::swap(a,b)�� ���� �⺻���� building block�̴�.
// ���� �⺻���� �˰����� primitive operation�� ������ �ϱ⿡ Ư���ϴ�.
// std::vector���� standard library types�� ������ swap �ɹ� �Լ��� �����ϰ�
// std::swap �����ε� �Լ��� �̸� ȣ���ϰ� �Ѵ�.
// ���� ���, std::swap(a,b)�� ȣ���ϸ� a.swap(b)�� ȣ���ϰ� �ȴ�.
// �̷��� ������ ������ ����.
namespace my
{
    class obj
    {
        int v;
    public:
        obj(int value)
            : v(value)
        { }
        void swap(obj& other)
        {
            using std::swap;
            swap(this->v, other.v);
        }
    };
    void swap(obj& a, obj& b) { a.swap(b); }
}
void test()
{
    int              i1 = 1, i2 = 2;
    std::vector<int> v1 = {1}, v2 = {2};
    my::obj          m1 = 1, m2 = 2;
    using std::swap;
    swap(i1, i2); // calls std::swap<int>(int&, int&)
    swap(v1, v2); // calss std::swap(vector&, vector&)
    swap(m1, m2); // calss my::swap(obj&, obj&)
}

// ���� swap�� bidirectional iterators�� ������ ���� ������ ������ �����
// �����ϴ� std::reverse(a,b)�� ���� �� �ִ�.
void reverse_words_in_place(std::string& s)
{
    // First, reverse the whole string.
    std::reverse(s.begin(), s.end());

    // Next, un-reverse each individual word.
    for (auto it = s.begin(); true; ++it) {
        auto next = std::find(it, s.end(), ' ');
        // Reverse the order of letters in this word.
        std::reverse(it, next);
        if (next == s.end()) {
            break;
        }
        it = next;
    }
}
void test2()
{
    std::string s = "the quick brown fox jumps over the lazy dog";
    reverse_words_in_place(s);
    assert(s == "dog lazy the over jumps fox brown quick the");
}

// std::reverse�� ������ ������ ����
template <class BidirIt>
void reverse(BidirIt first, BidirIt last)
{
    while (first != last) {
        --last;
        if (first == last) break;
        using std::swap;
        swap(*first, *last);
        ++first;
    }
}
// std::reverse�� sort �� std::partition�� building block ������ �Ѵ�.
//
// std::partition(a,b,p)�� ���� [a,b)�� ���ҵ鿡 ����
// p�� ���� ���Ҵ� ������, p�� ������ ���Ҵ� �ڷ� �����Ѵ�.
// ó������ p(*first) == false�� pivot�� iter�� �����Ѵ�.
// ��, ���� ��Ƽ���� end point, ���� ��Ƽ���� start point�̴�.
template <class BidirIt, class Unary>
auto partition(BidirIt first, BidirIt last, Unary p)
{
    // std::find_if_not �� �����ϴ�.
    while (first != last && p(*first)) {
        ++first;
    }

    while (first != last) {
        // std::find_if�� ���� backward�� �����Ѵٸ� �����ϴ�.
        // �̸� Standard library���� std::rfind_if��� �̸����� ��Ʈ�� �� ������
        // std::reverse_iterator adaptor�� ����ϸ� �ȴ�.
        do {
            --last;
        } while (last != first && !p(*last));

        if (first == last) break;
        using std::swap;
        swap(*first, *last);

        // std::find_if_not �� �����ϴ�.
        do {
            ++first;
        } while (first != last && p(*first));
    }
    return first;
}
void test3()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    // ¦���� ������, Ȧ���� �ڷ� ������.
    auto it =
        std::partition(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    assert(it == v.begin() + 3);
    assert((v == std::vector {6, 2, 4, 1, 5, 9, 1, 3, 5}));
}
// ������ �ٽ� std::partition�� std::reverse�� ������� �ۼ��� �ڵ��.
template <class It>
auto rev(It it)
{
    return std::reverse_iterator(it);
}
template <class InnerIt>
auto unrev(std::reverse_iterator<InnerIt> it)
{
    return it.base();
}
template <class BidirIt, class Unary>
auto partition(BidirIt first, BidirIt last, Unary p)
{
    first = std::find_if_not(first, last, p);

    while (first != last) {
        last = unrev(std::find(rev(last), rev(first), p));

        if (first == last) break;
        using std::swap;
        swap(*first, *last);

        first = std::find_if_not(first, last, p);
    }
    return first;
}
// std::partition ���� ���� ���ҵ��� ������ �����ϴ�
// std::stable_partition(a,b,p) ������ �ִ�.
// ������ �ٷ�� non-permutative algorithms���� �ִµ�,
// std::is_partitioned(a,b,p)�� predicate p�� �������� �̹� ���ҵǾ� ������
// true�� �����ϰ� �׷��� ������ false�� �����Ѵ�.
// std::partition_point(a,b,p)�� binary search�� ����Ͽ� �̹� ���ҵ� ������
// ���� p�� ������ ù��° ���Ҹ� �����Ѵ�.(pivot�� �����Ѵ�.)
// std::partition_copy(a,b,ot,of,p)�� [a,b) ������ ���Ҹ� ���� p�� ���̸�
// ot��, p�� �����̸� of�� �����Ѵ�.
// std::copy_if(a,b,ot,p) �Ǵ� std::remove_copy_if(a,b,of,p)�� ����ϸ�
// ���� p�� ���� ����, p�� ������ ������ �����Ѵ�.
}

//-----------------------------------------------------------------------------
// [Rotation and permutation]
//-----------------------------------------------------------------------------
namespace section9
{
// std::rotate(a,mid,b)�� [a,b)�� ���Ҹ� cyclic rotate���Ѽ� mid�� �ش��ϴ�
// ���Ҹ� �� ������ ������.
template <class FwdIt>
FwdIt rotate(FwdIt a, FwdIt mid, FwdIt b)
{
    // �־��� ������ {L, m, R}�� ���ٰ� ����.
    auto result = a + (b - mid); // = a + |R| + 1

    // First, reverse the whole range.
    std::reverse(a, b); // {R',m,L'}
    // Next, un-reverse each individual segment.
    std::reverse(a, result); // {m,R,L'}
    std::reverse(result, b); // {m,R,L}

    return result;
}

void test()
{
    std::vector<int> v    = {1, 2, 3, 4, 5, 6};
    auto             five = std::find(v.begin(), v.end(), 5);
    auto             one  = std::rotate(v.begin(), five, v.end());
    assert((v == std::vector {5, 6, 1, 2, 3, 4}));
    assert(*one == 1);
}

// std::next_permutation(a,b)�� ȣ���Ͽ� n�� ���ҿ� ���� ��� permutations��
// ���ϴ� �������� �ۼ��� �� �ִ�.
void test2()
{
    std::vector<int>              p = {10, 20, 30};
    std::vector<std::vector<int>> results;

    // Collect the permutations of these three elements.
    for (int i = 0; i < 6; ++i) {
        results.push_back(p);
        std::next_permutation(p.begin(), p.end());
    }

    assert((results == std::vector<std::vector<int>> {
                           {10, 20, 30},
                           {10, 30, 20},
                           {20, 10, 30},
                           {20, 30, 10},
                           {30, 10, 20},
                           {30, 20, 10},
    }));
}
// �̴� ���� ������� permutate�Ѵ�.
// std::next_permutaion(a,b,cmp) ������ cmp�� ���� permutate�Ѵ�.
// �̿� �ݴ�������� permutate�ϴ� std::prev_permutaion(a,b)�� �ִ�.
// ���� ������� ���ϴ� std::lexicographical_compare(a,b,c,d)�� �ִ�.
}

//-----------------------------------------------------------------------------
// [Heaps and heapsort]
//-----------------------------------------------------------------------------
namespace section10
{
// the max-heap property: ���� a�� ��� ���ҿ� ����
// a[i] > a[2i+1] && a[i] > a[2i+2]�� �����Ѵ�.
// ���� ������ ������Ű�� ������ heapsort�̶� �Ѵ�.
// std::make_heap(a,b)�� ���ĵ��� ���� [a,b)�� �������Ѵ�.
// �̴� std::push_heap(a,++b)�� �ݺ� ȣ���Ͽ� ������ �� �ִ�.
template <class RandomIt>
void make_heap(RandomIt a, RandomIt b)
{
    for (auto it = a; it != b;) {
        std::push_heap(a, ++it);
    }
}

// std::push_heap(a,b)�� [a,b-1)�� �̹� max-heap�̶�� ���� �Ͽ�
// ���Ҹ� ���� b[-1]�� �ְ� the max-heap property�� ������ ������
// �θ� ���ҵ��� ���ϸ� swap�Ѵ�.
template <class RandomIt>
void push_heap(RandomIt a, RandomIt b)
{
    auto child = ((b - 1) - a);
    while (child != 0) {
        auto parent = (child - 1) / 2;
        if (a[child] < a[parent]) {
            return;
        }
        std::iter_swap(a + child, a + parent);
        child = parent;
    }
}

// std::pop_heap(a,b)�� [a,b)�� �̹� max-heap�̶�� ���� �Ͽ�
// a[0]�� b[-1]�� swap�Ͽ� ������ ���� ū ���Ҹ� �� �ڷ� ������.
// �׷� ����, �ڽ� ���� �� �� ū ���Ұ� �����ϸ� �����ϴ� ������
// [a,b-1)���� the max-heap property�� ������ ������ �ݺ��Ѵ�.
// ����, b[-1]�� ���� ū ���Ұ� �ǰ� [a,b-1)�� max-heap�̴�.
template <class RandomIt>
void pop_heap(RandomIt a, RandomIt b)
{
    using DistanceT = decltype(b - a);

    std::iter_swap(a, b - 1);

    DistanceT parent        = 0;
    DistanceT new_heap_size = ((b - 1) - a);
    while (true) {
        auto leftchild  = 2 * parent + 1;
        auto rightchild = 2 * parent + 2;
        if (leftchild >= new_heap_size) {
            return;
        }
        auto biggerchild = leftchild;
        if (rightchild < new_heap_size && a[leftchild] < a[rightchild]) {
            biggerchild = rightchild;
        }
        if (a[biggerchild] < a[parent]) {
            return;
        }
        std::iter_swap(a + parent, b + biggerchild);
        parent = biggerchild;
    }
}

// std::sort_heap(a,b)�� [a,b)�� �̹� max-heap�̶�� ���� �Ͽ�
// std::pop_heap(a,b--)�� �ݺ� ȣ���ؼ� ������������ �����Ѵ�.
// ��, ù��° ȣ�⿡�� b[-1]�� ���� ū ���Ұ� �ǰ� �ι�° ȣ�⿡��
// b[-2]�� �ι�°�� ū ���Ұ� �ǰ�... �̸� �ݺ��Ͽ� ������������ ���ĵȴ�.
template <class RandomIt>
void sort_heap(RandomIt a, RandomIt b)
{
    for (auto it = b; it != a; --it) {
        pop_heap(a, it);
    }
}

template <class RandomIt>
void sort(RandomIt a, RandomIt b)
{
    make_heap(a, b);
    sort_heap(a, b);
}
}

//-----------------------------------------------------------------------------
// [Merges and mergesort]
//-----------------------------------------------------------------------------
namespace section11
{
// std::inplace_merge(a,mid,b)�� [a,b)�� �κ� ���� [a,mid), [mid,b)��
// �̹� ���ĵǾ� �ִٴ� ���� �Ͽ� ���ս��Ѽ� ���Ľ�Ų��. �̴� ������ ����������
// building block ������ �Ѵ�.
template <class RandomIt>
void sort(RandomIt a, RandomIt b)
{
    auto n = std::distance(a, b);
    if (n >= 2) {
        auto mid = a + n / 2;
        std::sort(a, mid);
        std::sort(mid, b);
        std::inplace_merge(a, mid, b);
    }
}
// ���⼭ inplace_merge��� ���� �߰����� ���۾��� ���յǴ� �ǹ̸� ������
// ���������� ���� �ӽ� ���۸� �����Ͽ� �����Ѵ�. �� �Ҵ翡 ������ �ִ� ����
// �� �˰����� ���ؾ� �Ѵ�. �̷��� ���� �ӽ� ���۸� �����ϴ� �˰�������
// std::stable_sort, std::stable_partition�� �ִ�.
// �ݸ鿡 std::merge(a,b,c,d,o)�� the non-allocating merge algorithm�̴�.
// [a,b),[c,d)�� the output range defined by o �� �����Ѵ�.
}

//-----------------------------------------------------------------------------
// [Searching and inserting in a sorted array with std::lower_bound]
//-----------------------------------------------------------------------------
namespace section12
{
// ���ĵ� ������ ���� binary search�� �̿��ϸ� linear search ���� ������.
// std::lower_bound(a,b,v)�� ������������ ���ĵ� [a,b)����
// Search for first element x st. v �� x. (not x < v �� �ǹ�)
// ���� �׷��� x�� ���ٸ�, v�� [a,b)�� ��� ���Һ���
// ū ���� �ȴ�. �̶� b�� �����ϰ� �Ǵµ�, ���� v�� b�� �ִ´ٸ� [a,b]��
// ������������ ���ĵȴ�.
// �ݸ鿡, std::upper_bound(a,b,v)�� ������������ ���ĵ� [a,b)����
// Search first element x st. v < x.
// ���� �׷��� x�� ���ٸ�, ���� �����ϰ� b�� �����Ѵ�.
template <class FwdIt, class T, class C>
FwdIt lower_bound(FwdIt first, FwdIt last, const T& value, C lessthan)
{
    using DiffT = typename std::iterator_traits<FwdIt>::difference_type;
    FwdIt it;
    DiffT count = std::distance(first, last);

    while (count > 0) {
        DiffT step = count / 2;
        it         = first;
        std::advance(it, step);
        if (lessthan(*it, value)) {
            ++it;
            first = it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}
template <class FwdIt, class T>
FwdIt lower_bound(FwdIt first, FwdIt last, const T& value)
{
    return std::lower_bound(first, last, value, std::less<> {});
}

void example1()
{
    std::vector<int> vec = {3, 7};
    for (int value : {1, 5, 9}) {
        // Find the appropriate insertion point...
        auto it = std::lower_bound(vec.begin(), vec.end(), value);
        // ...and insert our value there.
        vec.insert(it, value);
    }
    assert((vec == std::vector {1, 3, 5, 7, 9}));
}

void example2()
{
    std::vector<int> vec   = {2, 3, 3, 3, 4};
    auto             lower = std::lower_bound(vec.begin(), vec.end(), 3);

    // ù��° ���
    auto upper = std::upper_bound(vec.begin(), vec.end(), 3);

    // �ι�° ���: ù��° ���ó�� ��ü ������ �˻��� �ʿ䰡 ����.
    auto upper2 = std::upper_bound(lower, vec.end(), 3);
    assert(upper2 == upper);

    // ����° ���: lower bound ���� �˻��ϸ� �����˻��� �� ���� ���� �ִ�.
    auto upper3 = std::find_if(lower, vec.end(), [](int v) { return v != 3; });
    assert(upper3 == upper);

    // ��� ����̵� ������ ����.
    assert(*lower >= 3);
    assert(*upper > 3);
    assert(std::all_of(lower, upper, [](int v) { return v == 3; }));
}
}

//-----------------------------------------------------------------------------
// [Deleting from a sorted array with std::remove_if]
//-----------------------------------------------------------------------------
namespace section13
{
// �������� �����ϴ� STL�� �˰������ �����̳ʿ� ��� ���ҵ��� ���� ����
// �� ����. ��ſ� STL�� �˰������ ������ ���ҵ��� ��迭�Ͽ�
// ������ �׸���� ���������� ��ġ�� ��ġ�Ͽ� ���Ŀ� �����̳ʰ� �̵���
// �����ϱ� ���� ����� ������ �Ѵ�.
void example1()
{
    std::vector<int> vec = {1, 3, 3, 4, 6, 8};

    // ���� 3���� �ڷ� ������.
    auto first_3 = std::stable_partition(vec.begin(), vec.end(),
                                         [](int v) { return v != 3; });

    // STL �˰����� 3���� �������� ���ϴ� ��� ��迭�Ѵ�.
    assert((vec == std::vector {1, 4, 6, 8, 3, 3}));

    // ����, ������ �׸��(���� 3��)�� �����̳��� �˰������� �����.
    vec.erase(first_3, vec.end());

    // �����̳� vector���� ���ŵǾ���.
    assert((vec == std::vector {1, 4, 6, 8}));
}

// ���� ����� ���ʿ��� ���� �� �ϸ� stable_partition�� ���� �ӽ� ���۸�
// �����ϱ� ������ ������ ���� �˰����� ����ؾ� �Ѵ�.
template <class FwdIt, class T>
FwdIt remove(FwdIt first, FwdIt last, const T& value)
{
    auto out = std::find(first, last, value);
    if (out != last) {
        auto in = out;
        while (++in != last) {
            if (*in == value) {
                // don't bother with this item
            } else {
                *out++ = std::move(*in);
            }
        }
    }
    return out;
}
void example2()
{
    std::vector<int> vec = {1, 3, 3, 4, 6, 8};

    // ���� 3���� 3�� �ƴ� ����� ������.
    auto new_end = std::remove(vec.begin(), vec.end(), 3);
    assert((vec == std::vector {1, 4, 6, 8, 6, 8}));

    // ���� �����̳��� �˰������� �����Ѵ�.
    vec.erase(new_end, vec.end());
    assert((vec == std::vector {1, 4, 6, 8}));

    // "erase-remove idiom"
    // �� �ܰ踦 �� �ٷ� �ۼ��Ѵ�.
    vec.erase(std::remove(vec.begin(), vec.end(), 3), vec.end());

    // ���� �־��� ������ �ſ� ��� ���ĵǾ� �ִٸ�, �����˻��� ����.
    auto first = std::lower_bound(vec.begin(), vec.end(), 3);
    auto last  = std::upper_bound(first, vec.end(), 3);
    // �����̳��� �˰������� ���� ���ҵ��� �����Ѵ�.
    vec.erase(first, last);
}
// std::remove(a,b,v)�� ������ �ȵǾ �Ǵ� [a,b)���� v�� ������ ���ҵ���
// �ڸ��� �� �ڿ� ������ v�� �ƴ� ���ҵ�� ������ �����Ͽ� ���� �����.
// (v���� �� ���� ���� ���ҵ��� �״�� �������� �о� ���δ�.)
// �� �� ù��°�� �����ؾ��� ���Ҹ� ����Ű�� �ݺ��ڸ� �����Ѵ�.
// �����ؾ��� ���ҵ��� �����Ϸ��� �����̳��� �˰����� ����ؾ� �Ѵ�.

// std::unique(a,b)�� ������ �ȵǾ �Ǵ� [a,b)���� ���ӵ� ������ ���ҵ���
// �ϳ��� ����� ��� �����Ѵ�. �׸��� �����ؾ� �� �׸��� ���Ҹ� ����Ű��
// ù��° �ݺ��ڸ� �����Ѵ�.
void example3()
{
    std::vector<int> vec = {1, 2, 2, 3, 3, 3, 1, 3, 3};

    // "erase-remove idiom"
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    assert((vec == std::vector {1, 2, 3, 1, 3}));
}

// �Ϲ����� std::remove���� �����̳ʰ� �����ϴ� erase member function��
// ����ϴ� ���� ��κ� ����.
// �����ϴ� ������ �߿����� �ʴ� ���, std::remove���� std::unstable_remove��
// ����ϴ� ���� ����. (������ STL�� ������� �ʾ����� ���߿� ǥ��ȭ�� ���̶��
// å�� ���� �ִµ�, �� �������� ������� ���� �� ����.)
namespace my {
template<class BidirIt, class T>
BidirIt unstable_remove(BidirIt first, BidirIt last, const T& value)
{
    while (true) {
        // Find the first instance of "value"...
        first = std::find(first, last, value);
        // ...and the last instance of "not value"...
        do {
            if (first == last) {
                return last;
            }
            --last;
        } while (*last == value);
        // ...and move the latter over top of the former.
        *first = std::move(*last);
        // Rinse and repeat.
        ++first;
    }
}
} // namespace my
void test()
{
    std::vector<int> vec = { 4, 1, 3, 6, 3, 8 };
    vec.erase(
        my::unstable_remove(vec.begin(), vec.end(), 3),
        vec.end()
    );
    assert((vec == std::vector { 4, 1, 8, 6 }));
}
}
