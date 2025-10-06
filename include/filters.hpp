#pragma once

#include <algorithm>
#include <stdexcept>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

template <BookPredicate... Predicates>
auto all_of(Predicates... preds) {
    auto preds_tuple = std::make_tuple(std::forward<Predicates>(preds)...);
    return [p = std::move(preds_tuple)](const auto &value) {
        return std::apply([&](auto &&...pred) { return (pred(value) && ...); }, p);
    };
}

template <BookPredicate... Predicates>
auto any_of(Predicates... preds) {
    auto preds_tuple = std::make_tuple(std::forward<Predicates>(preds)...);
    return [p = std::move(preds_tuple)](const auto &value) {
        return std::apply([&](auto &&...pred) { return (pred(value) || ...); }, p);
    };
}

constexpr auto YearBetween(int startYear, int endYear) {
    if (startYear > endYear)
        throw std::range_error(std::format("startYear({}) must be less or equal endYear({})", startYear, endYear));

    return [startYear, endYear](const Book &book) { return book.year > startYear && book.year <= endYear; };
}

constexpr auto RatingAbove(double rating) {
    return [rating](const Book &book) { return book.rating > rating; };
}

constexpr auto GenreIs(const Genre genre) {
    return [genre](const Book &book) { return book.genre == genre; };
}

template <BookIterator T, BookPredicate Predicate>
auto filterBooks(T first, T last, Predicate &&f) {
    booksVector results;
    std::copy_if(first, last, std::back_inserter(results), std::forward<Predicate>(f));
    return results;
}

}  // namespace bookdb