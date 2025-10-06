#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookIterator = std::input_or_output_iterator<T> && std::convertible_to<std::iter_reference_t<T>, const Book &>;

template <typename S, typename I>
concept BookSentinel = BookIterator<I> && std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = std::predicate<P, const Book &>;

template <typename C>
concept BookComparator = std::relation<C, const Book &, const Book &>;

template <typename T>
concept BookContainerLike = requires(T &container, const T &const_container) {
    { std::begin(container) } -> BookIterator;
    { std::end(container) } -> BookSentinel<decltype(std::begin(container))>;
    { std::cbegin(const_container) } -> BookIterator;
    { std::cend(const_container) } -> BookSentinel<decltype(std::cbegin(const_container))>;
    { const_container.empty() } -> std::same_as<bool>;
    { const_container.size() } -> std::integral;
    { container.reserve(std::declval<typename T::size_type>()) };
    { container.push_back(std::declval<typename T::value_type>()) };
    { container.emplace_back(std::declval<typename T::value_type>()) };
};

}  // namespace bookdb