#pragma once

#include <format>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr std::string_view GenreTotring(Genre g) {
    // clang-format off
    using bookdb::Genre;
    switch (g) {
        case Genre::Fiction:    return "Fiction";
        case Genre::Mystery:    return "Mystery";
        case Genre::NonFiction: return "NonFiction";
        case Genre::SciFi:      return "SciFi";
        case Genre::Biography:  return "Biography";
        case Genre::Unknown:    return "Unknown";
        default:
            throw std::logic_error{"Unsupported bookdb::Genre"};
    }
    // clang-format on
}

constexpr Genre GenreFromString(std::string_view s) {
    if (s == "Fiction")
        return Genre::Fiction;
    if (s == "NonFiction")
        return Genre::NonFiction;
    if (s == "SciFi")
        return Genre::SciFi;
    if (s == "Biography")
        return Genre::Biography;
    if (s == "Mystery")
        return Genre::Mystery;
    return Genre::Unknown;
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string title, std::string_view author, int year, Genre genre, double rating, int read_count)
        : author(author), title(std::move(title)), year(year), genre(genre), rating(rating), read_count(read_count) {}

    constexpr Book(std::string title, std::string_view author, int year, std::string_view genre_str, double rating,
                   int read_count)
        : author(author), title(std::move(title)), year(year), genre(GenreFromString(genre_str)), rating(rating),
          read_count(read_count) {}

    auto operator<=>(const Book &) const = default;
};

using booksVector = std::vector<std::reference_wrapper<const Book>>;

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        const std::string_view genre_str = GenreTotring(g);

        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book b, FormatContext &fc) const {
        // clang-format on
        return format_to(fc.out(), "\"{}\" - {}({}) genre: {} read {} times rating: {}", b.title, b.author, b.year,
                         b.genre, b.read_count, b.rating);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

}  // namespace std
