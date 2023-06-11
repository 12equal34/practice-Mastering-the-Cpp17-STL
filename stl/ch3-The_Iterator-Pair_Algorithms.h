#pragma once

// ch3에서 필요한 헤더파일들은 다음과 같다.
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
// "between a and b"라는 말은 [a,b) 라는 뜻이다.
// 일반적으로 end point of a range(여기서는 *b)를 역참조하는 것은 위험하다.
// half-open ranges는 empty ranges를 잘 표현해준다. (from x to x 는 empty)
// 이 방법은 기존의 for-loop의 range와 같다. 아래 예시들을 보자.
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
// std::distance(a,b)는 ++연산을 a부터 b까지 세는 경우
// std::count_if(a,b,[](auto&&){return true;})와 동등하다.
// 만약 random-access iterators의 경우는 바로 (b-a)로 계산하므로 std::distance가
// 좋다. 그런데 (b-a)는 잘못된 순서를 전달하면 음수가 나온다.
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
// 하지만 bidirectional iterators는 잘못된 순서를 전달하면 오류가 발생한다.
// std::distance(b,a) == -std::distance(a,b)를 기대하지만 std::distance의
// 알고리즘은 해당 반복자가 잘못된 순서인지 아닌지 모르기 때문에 segfault가
// 발생한다. (Segmentation fault : 프로그램이 허용되지 않은 메모리 영역에 접근을
// 시도할 때 발생하는 오류이다.)

// std::count(a,b,v)는 the number of elements e for which e == v is true를
// 리턴하는데, 이는 std::count_if(a,b,[&v](auto&& e){return e==v;}) 와 동등하다.
// 실제로 같은 어셈블리 코드를 생성한다. 만약 람다 표현식이 1998년에 있었다면
// std::count는 굳이 라이브러리에 포함하지 않았을 것이다.

// std::count(a,b,v)는 [a,b)의 모든 원소를 조사하므로 특정한 순서로 배열되어
// 있는 경우에 있어서 비효율적이다. 다음 예시를 살펴보자.
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
// the standard algorithms은 자신한테 작성된 코드대로 동작을 하지,
// 절대로 the underlying data elements의 관계를 토대로 동작을 바꾸지 않는다.
// 이러한 관계를 토대로 효율적인 코드를 작성하는 건 프로그래머인 우리의 일이다.

// std::find(a,b,v), std::find_if(a,b,p)는 각각
// std::count(a,b,v), std::count_if(a,b,p)와 비슷하게 작동한다.
// 다만 find, find_if는 첫번째로 일치한 원소를 찾으면 바로 반복자를 리턴한다.
// (find는 예전에 람다 표현식이 있었다면 라이브러리에 포함되지 않았을 것이다.)
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
// (find_if는 핵심적인 "short-circuiting"의 역할이다.)
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
// find algorithms은 return immediately의 종류이다.
// 이러한 return immediately의 종류들은 "short-circuiting"의 역할을 해준다.
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

// find_first_of(haystack,haystack, needle, needle) 알고리즘은 2개의 range:
// the range to search in, and the set of target elements를 받는다.
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
// 여기서 "haystack" iterators은 모두 InputIterator type을 만족하고,
// "needle" iterators은 최소한 ForwardIterator type을 만족함에 주목하라.

// ForwardIterator의 중요한 점은 의미론적으로 복제가 가능하며,
// 같은 range를 반복자가 반복 통행(traverse)할 수 있다는 것이다.
// 여기서 needle은 반드시 re-traversable이고 유한한 크기여야 한다.
// 반대로 haystack은 유한한 크기일 필요가 없다.
// 예를 들어, haystack으로 잠재적으로 상한이 없는 (potentially unbounded)
// input stream의 원소들을 가져올 수 있다. 다음 예시를 보자.
void example4()
{
    std::istream_iterator<char> ii(std::cin);
    std::istream_iterator<char> iend {};
    std::string                 s = "hello";

    // std::cin으로부터 'h','e','l','o'에
    // 해당하는 문자를 찾을 때까지 계속해서 문자를 받는다.
    auto it = std::find_first_of(ii, iend, s.begin(), s.end());
}

// 마지막으로 std::equal 과 std::mismatch에 대해 설명하겠다.
// std::equal(a,b,c,d)는 [a,b)와 [c,d)의 모든 원소들이 각각 같을 때
// true를 리턴하고 그렇지 않으면 false를 리턴한다.
//
// std::mismatch(a,b,c,d)는 find의 유형이며 다음과 같다.
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
// 여기서 std::equal_to<> {}를 사용했는데, [](auto a, auto b){return a==b;}와
// 동일한 동작을 하고 more perfect forwarding(완전한 전송)을 수반한다.

// 마지막으로, 많은 two-range algorithms은 one-and-a-half-range algorithms을
// 제공한다. std::mistmatch(a,b,c,d)는 std::mismatch(a,b,c)로 작성한다면
// 두번째 range의 end point는 c + std::distance(a,b)를 의미한다.
// 하지만 범위를 벗어난 포인터를 참조할 가능성이 있으므로 c++17은
// the one-and-a-half-range algorithms에 대한 safe two-range variants
// 를 추가하였다.
}

//-----------------------------------------------------------------------------
// [Shunting data with std::copy]
//-----------------------------------------------------------------------------
// (shunt: to move (someone or something) to a different and usually less
// important or noticeable place or position.)
namespace section2
{
// std::copy의 구현은 다음과 같다.
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
// std::copy는 two-range version을 제공하지 않는다.
// 그래서 쓰기 버퍼의 크기 체크는 이미 했다고 가정한다.
// 가정이 틀리면 버퍼 오버플로우가 발생한다.

// destination을 output iterator라고 했는데
// std::copy는 데이터를 다른곳에 저장하는 용도가 아닌, 데이터를 임의의 함수에
// 공급하는 용도로 사용될 수 있다. 예를 들어 다음과 같다.
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
// 이러한 destination의 유연함 덕분에 버퍼 오버플로우 문제를 해결할 수 있다.
// 먼저 고정된 배열이 아닌 가변형의 std::vector를 쓴다고 가정하자.
// 그러면 writing an element는 pushing an element back과 상응한다.
// output iterator를 putc_iterator와 비슷하게 작성하고
// putc 대신에 push_back을 사용한다면 오버플로우가 발생하지 않을 것이다.
// 이러한 iterator는 STL에서 제공한다. <iterator> 헤더파일 안에서,
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
// 여기서 back_inserter_iterator(dest) 대신에 back_inserter(dest)라고
// 작성했는데, 이는 c++14 까지는 이렇게 작성해야 했고 c++17부터
// 생성자에 대한 템플릿 타입의 연역 기능 덕분에 곧바로
// back_inserter_iterator(dest) 을 작성해도 된다.
// 하지만, c++17 이후로
// std::make_pair 대신에 std::pair를,
// std::make_tuple 대신에 std::tuple를 선호하는 것처럼
// 더 적게 타이핑하는 방법인 std::back_inserter(dest)를 선호해야 한다.
}

//-----------------------------------------------------------------------------
// [Variations on a theme - std::move and std::move_iterator]
//-----------------------------------------------------------------------------
namespace section3
{
// 이전 절에서 std::copy은 input range의 원소들을 output으로 복제한다.
// 여기서 복제가 아닌 이동 연산을 사용하는 것이 가능할까?
// 이러한 문제를 위해 STL은 2가지의 접근법을 제공한다.
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
// 이는 std::copy 알고리즘과 std::move 를 추가한 것 빼고는 정확히 일치한다.
// 참고로 여기서 std::move 유틸리티 함수는 <utility>에 포함된 매개변수 3개가
// 아닌 1개를 받는 오버로드 함수로 위의 std::move 알고리즘과 전혀 다르다.
// 이렇게 운이 나쁘게 이름을 공유하는 경우가 std::remove 에도 존재한다.
//
// 다른 방법으로 이전 절에서 보았던 back_inserter의 변형이며
// 
// 2. back_inserter
template<class It>
class move_iterator
{

};
}
