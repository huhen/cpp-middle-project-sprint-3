#pragma once

#include <format>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr std::array<std::pair<Genre, std::string_view>, 6> genre_names = {{{Genre::Fiction, "Fiction"},
                                                                            {Genre::Mystery, "Mystery"},
                                                                            {Genre::NonFiction, "NonFiction"},
                                                                            {Genre::SciFi, "SciFi"},
                                                                            {Genre::Biography, "Biography"},
                                                                            {Genre::Unknown, "Unknown"}}};

constexpr std::string_view GenreToString(Genre g) {
    const auto &it =
        std::find_if(genre_names.begin(), genre_names.end(), [g](const auto &pair) { return pair.first == g; });
    if (it != genre_names.end()) {
        return it->second;
    }
    throw std::logic_error{"Unsupported bookdb::Genre"};
}

constexpr Genre GenreFromString(std::string_view s) {
    const auto &it =
        std::find_if(genre_names.begin(), genre_names.end(), [s](const auto &pair) { return pair.second == s; });
    if (it != genre_names.end()) {
        return it->first;
    }
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

using BooksVector = std::vector<std::reference_wrapper<const Book>>;

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        const std::string_view genre_str = GenreToString(g);

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
