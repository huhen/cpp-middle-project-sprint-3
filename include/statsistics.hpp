#pragma once

#include <algorithm>
#include <cstddef>
#include <flat_map>
#include <format>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

#include "book_database.hpp"
#include "comparators.hpp"

#include <print>
#include <unordered_map>

namespace bookdb {

template <typename Comparator>
using authorHistogramMap = std::flat_map<std::string_view, size_t, Comparator>;

template <typename Hasher>
using authorHistogramHashMap = std::unordered_map<std::string_view, size_t, Hasher>;

template <typename Comparator>
using genreRatingMap = std::flat_map<bookdb::Genre, double, Comparator>;

template <BookContainerLike T, typename Hasher = TransparentStringHash>
auto buildAuthorHistogram(const BookDatabase<T> &cont, Hasher hasher = {}) {
    authorHistogramHashMap<Hasher> histogram;

    for (const auto &book : cont) {
        ++histogram[book.author];
    }

    return histogram;
}

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    // были мысли сначала инициализировать histogram через GetAuthors, в теории должно быть быстрее
    // но пулучалось както громоздко
    authorHistogramMap<Comparator> histogram(comp);

    for (const auto &book : cont) {
        auto [it, _] = histogram.try_emplace(book.author, 0);
        ++it->second;
    }

    return histogram;
}

template <BookIterator T, typename Comparator = comp::LessByGenre>
auto calculateGenreRatings(T first, T last, Comparator comp = {}) {
    std::flat_map<bookdb::Genre, std::pair<double, int>> genreStats;

    std::for_each(first, last, [&genreStats](const auto &book) {
        auto it = genreStats.try_emplace(book.genre, 0.0, 0).first;
        auto &[total_rating, count] = it->second;
        total_rating += book.rating;
        ++count;
    });

    genreRatingMap<Comparator> genreRatings(comp);
    std::transform(genreStats.cbegin(), genreStats.cend(), std::inserter(genreRatings, genreRatings.end()),
                   [](const auto &pair) { return std::make_pair(pair.first, pair.second.first / pair.second.second); });

    return genreRatings;
}

template <BookContainerLike T>
auto calculateAverageRating(const BookDatabase<T> &cont) {
    const double sum = std::transform_reduce(cont.begin(), cont.end(), 0.0, std::plus<>(),
                                             [](const auto &item) { return item.rating; });
    return cont.size() != 0 ? sum / cont.size() : 0.0;
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &cont, size_t N) {
    if (cont.size() < N)
        throw std::length_error(std::format(
            "Number of requested books({}) must be less or equal number of books in the database({})", N, cont.size()));

    booksVector randomBooks;

    randomBooks.reserve(N);
    std::sample(cont.begin(), cont.end(), std::back_inserter(randomBooks), N, std::mt19937{std::random_device{}()});
    return randomBooks;
}

template <BookComparator Comparator = comp::LessByRating>
auto getTopNBy(std::span<Book> books, size_t N, Comparator comp = {}) {
    if (books.size() < N)
        throw std::length_error(
            std::format("Number of requested books({}) must be less or equal number of books in the database({})", N,
                        books.size()));

    std::partial_sort(books.begin(), books.begin() + N, books.end(), comp);

    booksVector topBooks;
    topBooks.reserve(N);
    std::copy_n(books.begin(), N, std::back_inserter(topBooks));

    return topBooks;
}

}  // namespace bookdb

namespace std {
template <typename Comparator>
struct formatter<bookdb::authorHistogramMap<Comparator>, char> {
    template <typename FormatContext>
    auto format(const bookdb::authorHistogramMap<Comparator> histogram, FormatContext &fc) const {
        format_to(fc.out(), "\n");

        for (const auto &authorStat : histogram)
            format_to(fc.out(), "{} - {}\n", authorStat.first, authorStat.second);

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <typename Hasher>
struct formatter<bookdb::authorHistogramHashMap<Hasher>, char> {
    template <typename FormatContext>
    auto format(const bookdb::authorHistogramHashMap<Hasher> histogram, FormatContext &fc) const {
        format_to(fc.out(), "\n");

        for (const auto &authorStat : histogram)
            format_to(fc.out(), "{} - {}\n", authorStat.first, authorStat.second);

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <typename Comparator>
struct formatter<bookdb::genreRatingMap<Comparator>, char> {
    template <typename FormatContext>
    auto format(const bookdb::genreRatingMap<Comparator> genreRatings, FormatContext &fc) const {
        format_to(fc.out(), "\n");

        for (const auto &authorStat : genreRatings)
            format_to(fc.out(), "{} avg: {}\n", authorStat.first, authorStat.second);

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

}  // namespace std