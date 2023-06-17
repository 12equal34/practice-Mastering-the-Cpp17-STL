#pragma once
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

namespace Practice
{
template <class InputIt>
void print_all(InputIt first, InputIt last)
{
    for (; first != last; ++first) {
        std::cout << *first << ' ';
    }
    std::cout << std::endl;
}

template <class InputIt, class UnaryPredicate>
void print_if(InputIt first, InputIt last, UnaryPredicate p)
{
    for (; first != last; ++first) {
        if (p(*first)) {
            std::cout << *first << ' ';
        }
    }
    std::cout << std::endl;
}

template <class InputIt, class UnaryPredicate>
InputIt find_if(InputIt first, InputIt last, UnaryPredicate p)
{
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }
    return last;
}

template <class InputIt, class UnaryPredicate>
InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate p)
{
    return Practice::find_if(first, last, [&](auto&& e) { return !p(e); });
}

template <class InputIt, class UnaryPredicate>
bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return Practice::find_if_not(first, last, p) == last;
}

template <class InputIt, class UnaryPredicate>
bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return Practice::find_if(first, last, p) != last;
}

template <class InputIt, class UnaryPredicate>
bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return Practice::find_if(first, last, p) == last;
}

template <class InputIt, class ForwardIt>
InputIt find_first_of(InputIt first, InputIt last, ForwardIt targetfirst,
                      ForwardIt targetlast)
{
    return Practice::find_if(first, last, [&](auto&& e) {
        return Practice::any_of(targetfirst, targetlast,
                                [&](auto&& t) { return e == t; });
    });
}

// std::istream_iterator와 유사한 클래스
template <class T>
class getc_iterator
{
    T ch;
public:
    getc_iterator()
        : ch(getc(stdin))
    { }
    T     operator*() const { return ch; }
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

class putc_iterator
{
    // 책에서는 non-static으로 구현했는데 여기서는 일부로
    // 임시객체를 만들지 않기 위해 static으로 구현했다.
    struct proxy {
        void operator=(char ch) const { putc(ch, stdout); }
    };
    static constexpr proxy proxy_ {};
public:
    auto& operator*() const { return proxy_; }
    auto& operator++() { return *this; }
    auto& operator++(int) { return *this; }
    bool  operator==(const putc_iterator&) const { return false; }
    bool  operator!=(const putc_iterator&) const { return true; }
};

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
    return back_insert_iterator<Container> {c};
}

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

    template <class U>
    bool operator==(const move_iterator<U>& rhs)
    {
        return iter == rhs.iter;
    }
    template <class U>
    bool operator!=(const move_iterator<U>& rhs)
    {
        return iter != rhs.iter;
    }
};

template <class InIt, class OutIt, class Unary>
OutIt transform(InIt first1, InIt last1, OutIt dest, Unary op)
{
    while (first1 != last1) {
        *dest = op(*first1);
        ++first1;
        ++dest;
    }
    return dest;
}

auto toupper(char c)
{
    c += 'A' - 'a';
    return c;
}

template <class InIt1, class InIt2, class OutIt, class Binary>
OutIt transform(InIt1 first1, InIt1 last1, InIt2 first2, InIt2 last2,
                OutIt dest, Binary op)
{
    while (first1 != last1 && first2 != last2) {
        *dest = op(*first1, *first2);
        ++first1;
        ++first2;
        ++dest;
    }
    return dest;
}


}

int main()
{
    auto        c = std::vector {1.0f, 2.0f, 3.0f};
    decltype(c) x {2.0f, 4.0f, 16.0f};
    decltype(c) res(x.size());

    Practice::transform(c.begin(), c.end(), x.begin(), x.end(), res.begin(),
                        [](auto e1, auto e2) { return e1 * e2; });

    Practice::print_all(c.begin(), c.end());
    Practice::print_all(x.begin(), x.end());
    Practice::print_all(res.begin(), res.end());

    std::reverse(res.begin(), res.end());
    Practice::print_all(res.begin(), res.end());

}