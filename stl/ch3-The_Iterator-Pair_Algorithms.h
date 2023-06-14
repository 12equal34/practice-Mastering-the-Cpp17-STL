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
#include <numeric>

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
// 다른 방법으로 이전 절에서 보았던 back_inserter와 비슷한 방법이 있다.
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
// 제공하는 맴버 함수들이 많은데 이를 지원할 수 없는 It의 타입들에 대해서
// 해당 맴버 함수를 코드에서 사용하면 컴파일 에러가 발생한다.
// 하지만 사용하지 않으면 인스턴스화하지 않으므로 문제 없다.

// back_inserter에서의 설명과 마찬가지로 다음 helper function이 존재한다.
// 즉, 타이핑이 길어지므로 make_pair와 make_tuple같은 존재다.
template <class InputIterator>
auto make_move_iterator(InputIterator& c)
{
    return move_iterator(c);
}

// We have two different ways of moving data from one container or
// range to another: 1.std::move algorithm, 2.std::move_iterator adaptor class.
// 다음 예제를 보자.
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
// std::copy보다 std::move를 사용하는 방법이 더 명확하다.
// STL에서 move_iterator 를 제공하는 이유는 다음 절에서 설명할
// std::copy와 관련된 알고리즘에 사용되기 때문이다.
}

//-----------------------------------------------------------------------------
// [Complicated copying with std::transform]
//-----------------------------------------------------------------------------
namespace section4
{
// std::copy의 구현에서 2개의 iterator type parameters를 받는다.
// 그래서 각각의 value_type이 서로 다를 수 있는데 이는 암시적 변환을 이용한 코드
// 작성을 가능하게 한다. 다음을 보자.
void example1()
{
    std::vector<const char*> input = {"hello", "world"};
    std::vector<std::string> output(2);

    std::copy(input.begin(), input.end(), output.begin());

    assert(output[0] == "hello");
    assert(output[1] == "world");
}
// *input.begin()의 const char * 타입에서
// *output.begin()의 std::string 타입으로 암시적 캐스팅 생성자가 호출된다.
// 이런 식으로 copy연산 과정에서 캐스팅하는데 좀더 복잡한 transformation 함수가
// 필요할 때가 있다.
// 다음 std::transform 을 보자.
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

// 가끔은 two arguments를 받는 transformation이 필요할 때가 있다.
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

// 데이터를 이동시키는 3번째 방법은 다음과 같다.
void example3()
{
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    // Third approach: use std::transform
    std::transform(input.begin(), input.end(), output.begin(),
                   std::move<std::string&>);
}
// 하지만 std::move 템플릿의 explicit specialization를 포함하게 된다.
// 언제나 어떠한 explicit specialization--those angle brackets after the
// template's name-- 을 포함한다는 것은 매우 미묘하고 취약한 코드라는 확실한
// 신호이다.
}

//-----------------------------------------------------------------------------
// [Write-only range algorithms]
//-----------------------------------------------------------------------------
namespace section5
{
// There is a family of standard algorithms that march through a range modifying
// each element without reading it.

// std::fill(a,b,v)는 [a,b)의 모든 원소를 v의 복사본으로 채운다.
// std::iota(a,b,v)는 [a,b)의 모든 원소를 첫째항 v, 공차가 1인 등차수열로
// 채운다. std::generate(a,b,g)는 [a,b)의 모든 원소를 g(void)의 리턴값으로
// 채운다. (여기서 std::iota는 <numeric>에 포함되어 있다.)
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

// 다음 예시를 보자.
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
// <memory> 헤더파일에 잘 안 알려져 있는 알고리즘으로
// std::uninitialized_copy, std::uninitialized_default_construct,
// and std::destroy 가 있다.

// 다음은 a range의 모든 원소들을 소멸하는 알고리즘이다.
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
// std::addressof(x)는 x의 주소를 리턴하는 helper function이며 x의 클래스가
// operator&를 오버로드한 매우 드문 경우가 아니면 정확히 &x와 동일한다.

// std::uninitialized_copy는
// a range의 모든 원소의 자리에 직접 copy-construct into하는 알고리즘이다.
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
// 이번 절에서 대수학의 permutation연산을 하는 algorithms에 대해 공부한다.
// std::sort(a,b)는 operator<에 대해 오름차순으로 정렬한다.
// operator<를 오버로드하여 다른 조건으로 정렬할 수 있지만
// std::sort(a,b,cmp) 버전을 사용해야 한다.
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
// 정렬되는 순서가 같은 우선순위를 가질 때, std::sort는 상대적인 위치를
// 확실하게 결정지어 주지 않고, std::stable_sort는 좀더 느리지만
// the original order를 보존하여 {1,1,3,9,4,5}로 정렬한다.

// cmp는 the comparison relation을 만족하는 함수여야 정상적으로 정렬할 수 있다.
// 예를 들어 (a % 6 < b) 를 리턴하는 함수는 5와 9에 대해서
// cmp(5,9) == cmp(9,5) == true 가 되므로 비교 함수가 될 수 없다.
}

//-----------------------------------------------------------------------------
// [Swapping, reversing, and partitioning]
//-----------------------------------------------------------------------------
namespace section8
{
// STL은 std::sort 말고도 많은 permutative algorithms이 있다.
// 이 알고리즘들은 대부분 다른 정렬 알고리즘의 building blocks 역할을 한다.
// 그 중 std::swap(a,b)는 가장 기본적인 building block이다.
// 또한 기본적인 알고리즘의 primitive operation의 역할을 하기에 특별하다.
// std::vector같은 standard library types은 각자의 swap 맴버 함수를 정의하고
// std::swap 오버로드 함수가 이를 호출하게 한다.
// 예를 들어, std::swap(a,b)를 호출하면 a.swap(b)를 호출하게 된다.
// 이러한 구현은 다음과 같다.
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

// 이제 swap과 bidirectional iterators를 가지고 기존 원소의 순서를 뒤집어서
// 스왑하는 std::reverse(a,b)를 만들 수 있다.
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

// std::reverse의 구현은 다음과 같고
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
// std::reverse는 sort 중 std::partition의 building block 역할을 한다.
//
// std::partition(a,b,p)은 구간 [a,b)의 원소들에 대해
// p가 참인 원소는 앞으로, p가 거짓인 원소는 뒤로 정렬한다.
// 처음으로 p(*first) == false인 pivot의 iter를 리턴한다.
// 즉, 앞의 파티션의 end point, 뒤의 파티션의 start point이다.
template <class BidirIt, class Unary>
auto partition(BidirIt first, BidirIt last, Unary p)
{
    // std::find_if_not 과 동일하다.
    while (first != last && p(*first)) {
        ++first;
    }

    while (first != last) {
        // std::find_if가 만약 backward로 진행한다면 동일하다.
        // 이를 Standard library에서 std::rfind_if라는 이름으로 빠트린 것 같지만
        // std::reverse_iterator adaptor를 사용하면 된다.
        do {
            --last;
        } while (last != first && !p(*last));

        if (first == last) break;
        using std::swap;
        swap(*first, *last);

        // std::find_if_not 과 동일하다.
        do {
            ++first;
        } while (first != last && p(*first));
    }
    return first;
}
void test3()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    // 짝수를 앞으로, 홀수를 뒤로 보낸다.
    auto it =
        std::partition(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    assert(it == v.begin() + 3);
    assert((v == std::vector {6, 2, 4, 1, 5, 9, 1, 3, 5}));
}
// 다음은 다시 std::partition을 std::reverse를 기반으로 작성한 코드다.
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
// std::partition 또한 원래 원소들의 순서를 보존하는
// std::stable_partition(a,b,p) 버전이 있다.
// 분할을 다루는 non-permutative algorithms들이 있는데,
// std::is_partitioned(a,b,p)는 predicate p를 기준으로 이미 분할되어 있으면
// true를 리턴하고 그렇지 않으면 false를 리턴한다.
// std::partition_point(a,b,p)는 binary search를 사용하여 이미 분할된 구간에
// 대해 p가 거짓인 첫번째 원소를 리턴한다.(pivot를 리턴한다.)
// std::partition_copy(a,b,ot,of,p)는 [a,b) 구간의 원소를 각각 p가 참이면
// ot에, p가 거짓이면 of에 복제한다.
// std::copy_if(a,b,ot,p) 또는 std::remove_copy_if(a,b,of,p)를 사용하면
// 각각 p가 참인 수열, p가 거짓인 수열을 복제한다.
}

//-----------------------------------------------------------------------------
// [Rotation and permutation]
//-----------------------------------------------------------------------------
namespace section9
{
// std::rotate(a,mid,b)는 [a,b)의 원소를 cyclic rotate시켜서 mid에 해당하는
// 원소를 맨 앞으로 보낸다.
template <class FwdIt>
FwdIt rotate(FwdIt a, FwdIt mid, FwdIt b)
{
    // 주어진 수열이 {L, m, R}과 같다고 하자.
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

// std::next_permutation(a,b)을 호출하여 n개 원소에 대한 모든 permutations을
// 구하는 루프문을 작성할 수 있다.
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
// 이는 사전 순서대로 permutate한다.
// std::next_permutaion(a,b,cmp) 버전은 cmp를 토대로 permutate한다.
// 이와 반대방향으로 permutate하는 std::prev_permutaion(a,b)가 있다.
// 사전 순서대로 비교하는 std::lexicographical_compare(a,b,c,d)가 있다.
}

//-----------------------------------------------------------------------------
// [Heaps and heapsort]
//-----------------------------------------------------------------------------
namespace section10
{
// the max-heap property: 구간 a의 모든 원소에 대해
// a[i] > a[2i+1] && a[i] > a[2i+2]를 만족한다.
// 위의 성질을 만족시키는 정렬을 heapsort이라 한다.
// std::make_heap(a,b)는 정렬되지 않은 [a,b)를 힙정렬한다.
// 이는 std::push_heap(a,++b)를 반복 호출하여 구현할 수 있다.
template <class RandomIt>
void make_heap(RandomIt a, RandomIt b)
{
    for (auto it = a; it != b;) {
        std::push_heap(a, ++it);
    }
}

// std::push_heap(a,b)는 [a,b-1)가 이미 max-heap이라는 가정 하에
// 원소를 현재 b[-1]에 넣고 the max-heap property를 만족할 때까지
// 부모 원소들을 비교하며 swap한다.
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

// std::pop_heap(a,b)는 [a,b)가 이미 max-heap이라는 가정 하에
// a[0]과 b[-1]을 swap하여 기존의 가장 큰 원소를 맨 뒤로 보낸다.
// 그런 다음, 자식 원소 중 더 큰 원소가 존재하면 스왑하는 과정을
// [a,b-1)에서 the max-heap property가 성립할 때까지 반복한다.
// 이후, b[-1]은 가장 큰 원소가 되고 [a,b-1)은 max-heap이다.
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

// std::sort_heap(a,b)는 [a,b)가 이미 max-heap이라는 가정 하에
// std::pop_heap(a,b--)를 반복 호출해서 오름차순으로 정렬한다.
// 즉, 첫번째 호출에서 b[-1]이 가장 큰 원소가 되고 두번째 호출에서
// b[-2]가 두번째로 큰 원소가 되고... 이를 반복하여 오름차순으로 정렬된다.
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
// std::inplace_merge(a,mid,b)는 [a,b)의 부분 구간 [a,mid), [mid,b)이
// 이미 정렬되어 있다는 가정 하에 병합시켜서 정렬시킨다. 이는 다음의 병합정렬의
// building block 역할을 한다.
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
// 여기서 inplace_merge라는 말이 추가적인 버퍼없이 병합되는 의미를 갖지만
// 내부적으로 힙에 임시 버퍼를 생성하여 구현한다. 힙 할당에 문제가 있는 경우는
// 이 알고리즘을 피해야 한다. 이렇게 힙에 임시 버퍼를 생성하는 알고리즘으로
// std::stable_sort, std::stable_partition이 있다.
// 반면에 std::merge(a,b,c,d,o)는 the non-allocating merge algorithm이다.
// [a,b),[c,d)를 the output range defined by o 에 병합한다.
}

//-----------------------------------------------------------------------------
// [Searching and inserting in a sorted array with std::lower_bound]
//-----------------------------------------------------------------------------
namespace section12
{
// 정렬된 구간에 대해 binary search를 이용하면 linear search 보다 빠르다.
// std::lower_bound(a,b,v)은 오름차순으로 정렬된 [a,b)에서
// Search for first element x st. v ≤ x. (not x < v 를 의미)
// 만약 그러한 x가 없다면, v는 [a,b)의 모든 원소보다
// 큰 값이 된다. 이때 b를 리턴하게 되는데, 만약 v를 b에 넣는다면 [a,b]는
// 오름차순으로 정렬된다.
// 반면에, std::upper_bound(a,b,v)는 오름차순으로 정렬된 [a,b)에서
// Search first element x st. v < x.
// 만약 그러한 x가 없다면, 위와 동일하게 b를 리턴한다.
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

    // 첫번째 방법
    auto upper = std::upper_bound(vec.begin(), vec.end(), 3);

    // 두번째 방법: 첫번째 방법처럼 전체 구간을 검색할 필요가 없다.
    auto upper2 = std::upper_bound(lower, vec.end(), 3);
    assert(upper2 == upper);

    // 세번째 방법: lower bound 부터 검색하면 선형검색이 더 빠를 수도 있다.
    auto upper3 = std::find_if(lower, vec.end(), [](int v) { return v != 3; });
    assert(upper3 == upper);

    // 어떠한 방법이든 다음과 같다.
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
// 구간으로 정의하는 STL의 알고리즘들은 컨테이너에 담긴 원소들을 직접 지울
// 수 없다. 대신에 STL의 알고리즘들은 구간의 원소들을 재배열하여
// 제거할 항목들을 예측가능한 위치로 배치하여 추후에 컨테이너가 이들을
// 제거하기 쉽게 만드는 역할을 한다.
void example1()
{
    std::vector<int> vec = {1, 3, 3, 4, 6, 8};

    // 숫자 3들을 뒤로 보낸다.
    auto first_3 = std::stable_partition(vec.begin(), vec.end(),
                                         [](int v) { return v != 3; });

    // STL 알고리즘은 3들을 제거하지 못하는 대신 재배열한다.
    assert((vec == std::vector {1, 4, 6, 8, 3, 3}));

    // 이제, 제거할 항목들(숫자 3들)을 컨테이너의 알고리즘으로 지운다.
    vec.erase(first_3, vec.end());

    // 컨테이너 vector에서 제거되었다.
    assert((vec == std::vector {1, 4, 6, 8}));
}

// 위의 방법은 불필요한 일을 더 하며 stable_partition가 힙에 임시 버퍼를
// 생성하기 때문에 다음과 같은 알고리즘을 사용해야 한다.
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

    // 숫자 3들을 3이 아닌 수들로 덮어쒸운다.
    auto new_end = std::remove(vec.begin(), vec.end(), 3);
    assert((vec == std::vector {1, 4, 6, 8, 6, 8}));

    // 이제 컨테이너의 알고리즘으로 제거한다.
    vec.erase(new_end, vec.end());
    assert((vec == std::vector {1, 4, 6, 8}));

    // "erase-remove idiom"
    // 두 단계를 한 줄로 작성한다.
    vec.erase(std::remove(vec.begin(), vec.end(), 3), vec.end());

    // 만약 주어진 수열이 매우 길고 정렬되어 있다면, 이진검색이 좋다.
    auto first = std::lower_bound(vec.begin(), vec.end(), 3);
    auto last  = std::upper_bound(first, vec.end(), 3);
    // 컨테이너의 알고리즘으로 직접 원소들을 제거한다.
    vec.erase(first, last);
}
// std::remove(a,b,v)는 정렬이 안되어도 되는 [a,b)에서 v와 동일한 원소들의
// 자리에 그 뒤에 나오는 v가 아닌 원소들로 순서를 보존하여 덮어 쒸운다.
// (v들을 쏙 빼고 남은 원소들을 그대로 왼쪽으로 밀어 붙인다.)
// 그 후 첫번째로 제거해야할 원소를 가리키는 반복자를 리턴한다.
// 제거해야할 원소들을 제거하려면 컨테이너의 알고리즘을 사용해야 한다.

// std::unique(a,b)는 정렬이 안되어도 되는 [a,b)에서 연속된 동일한 원소들을
// 하나만 남기고 모두 제거한다. 그리고 제거해야 할 항목의 원소를 가리키는
// 첫번째 반복자를 리턴한다.
void example3()
{
    std::vector<int> vec = {1, 2, 2, 3, 3, 3, 1, 3, 3};

    // "erase-remove idiom"
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    assert((vec == std::vector {1, 2, 3, 1, 3}));
}

// 일반적인 std::remove보다 컨테이너가 제공하는 erase member function을
// 사용하는 것이 대부분 좋다.
// 제거하는 순서가 중요하지 않는 경우, std::remove보다 std::unstable_remove를
// 사용하는 것이 좋다. (아직은 STL에 적용되지 않았지만 나중에 표준화될 것이라고
// 책에 적혀 있는데, 현 시점에도 적용되지 않은 것 같다.)
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
