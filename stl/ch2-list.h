#pragma once

#include <type_traits>
#include <iterator>
#include <iostream>
#include <algorithm>

namespace Practice
{
template <typename DataType>
struct list_node {
    DataType   data;
    list_node* next;
};

template <typename DataType>
class list;

template <bool Const, typename DataType>
class list_iterator
{
    friend class list<DataType>;
    friend class list_iterator<!Const, DataType>;

    using node_pointer = std::conditional_t<Const, const list_node<DataType>*,
                                            list_node<DataType>*>;
    node_pointer ptr_;

    explicit list_iterator(node_pointer ptr)
        : ptr_(ptr)
    { }
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = DataType;
    using pointer   = std::conditional_t<Const, const DataType*, DataType*>;
    using reference = std::conditional_t<Const, const DataType&, DataType&>;

    reference operator*() const { return ptr_->data; }
    pointer   operator->() const { return &ptr_->data; }
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

    template <bool R>
    bool operator==(const list_iterator<R, DataType>& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }
    template <bool R>
    bool operator!=(const list_iterator<R, DataType>& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    operator list_iterator<true, DataType>() const
    {
        return list_iterator<true, DataType> {ptr_};
    }
};

template <typename DataType>
class list
{
    using node_structure  = list_node<DataType>;
    using node_pointer    = node_structure*;
    using reference       = typename list_iterator<false, DataType>::reference;
    using const_reference = typename list_iterator<true, DataType>::reference;

    node_pointer head_ = nullptr;
    node_pointer tail_ = nullptr;
    int          size_ = 0;

public:
    using const_iterator = list_iterator<true, DataType>;
    using iterator       = list_iterator<false, DataType>;

    iterator       begin() { return iterator {head_}; }
    iterator       end() { return iterator {nullptr}; }
    const_iterator begin() const { return const_iterator {head_}; }
    const_iterator end() const { return const_iterator {nullptr}; }

    int size() const { return size_; }

    void push_back(DataType data)
    {
        auto new_tail = new node_structure {data, nullptr};
        if (tail_) {
            tail_->next = new_tail;
        } else {
            head_ = new_tail;
        }
        tail_ = new_tail;
        size_ += 1;
    }

    void pop_front()
    {
        if (!head_) return;
        auto old = head_;
        head_    = old->next;
        delete old;
        --size_;
    }

    reference       front() { return head_->data; }
    const_reference front() const { return head_->data; }
    reference       back() { return tail_->data; }
    const_reference back() const { return tail_->data; }

public:
    list() = default;

    list(const list& rhs)
    {
        std::for_each(rhs.begin(), rhs.end(),
                      [this](auto value) { this->push_back(value); });
    }
    list& operator=(const list& rhs) = delete;

    ~list()
    {
        for (node_pointer old = head_; head_ != nullptr; old = head_) {
            head_ = old->next;
            delete old;
        }
    }
};

// constexpr function은 non-constexpr argument를 받을 때는 runtime에 실행되는
// regular 버전을 호출한다. (non-constexpr 버전을 같이 작성할 필요가 없다.)
template <class It>
constexpr typename std::iterator_traits<It>::difference_type distance(It first,
                                                                      It last)
{
    using category = std::iterator_traits<It>::iterator_category;
    static_assert(std::is_base_of_v<std::input_iterator_tag, category>);

    if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                    category>) {
        return last - first;
    } else {
        typename std::iterator_traits<It>::difference_type result = 0;
        if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag,
                                        category>) {
        }
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}

template <class InputIt, class UnaryPredicate>
auto count_if(InputIt first, InputIt last, UnaryPredicate p)
{
    using iterator_traits = std::iterator_traits<InputIt>;
    auto ret              = typename iterator_traits::difference_type {};
    for (; first != last; ++first) {
        if (p(*first)) {
            ++ret;
        }
    }
    return ret;
}

}

int main()
{
    using namespace Practice;

    list<int> lst;
    lst.push_back(30);
    lst.push_back(20);
    lst.push_back(10);
    lst.push_back(50);

    const auto clst = lst;

    lst.push_back(70);

    std::cout << clst.front() << std::endl;
    std::cout << clst.back() << std::endl;

    std::for_each(clst.begin(), clst.end(),
                  [](auto v) { std::cout << v << ' '; });
    std::cout << std::endl;
    std::cout << distance(clst.begin(), clst.end()) << std::endl;
    std::cout << count_if(clst.begin(), clst.end(), [](auto v) {
        return v < 20;
    }) << std::endl;
}