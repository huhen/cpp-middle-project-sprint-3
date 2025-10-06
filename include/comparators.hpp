#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book &a, const Book &b) const { return a.author < b.author; }
};

struct LessByTitle {
    bool operator()(const Book &a, const Book &b) const { return a.title < b.title; }
};

struct LessByYear {
    bool operator()(const Book &a, const Book &b) const { return a.year < b.year; }
};

struct LessByGenre {
    using is_transparent = void;

    bool operator()(const Book &a, const Book &b) const { return a.genre < b.genre; }

    bool operator()(const Book &a, Genre b) const { return a.genre < b; }

    bool operator()(Genre a, const Book &b) const { return a < b.genre; }

    bool operator()(Genre a, Genre b) const { return a < b; }
};

struct LessByRating {
    bool operator()(const Book &a, const Book &b) const { return a.rating < b.rating; }
};

struct GreatByRating {
    bool operator()(const Book &a, const Book &b) const { return a.rating > b.rating; }
};

struct LessByPopularity {
    bool operator()(const Book &a, const Book &b) const { return a.read_count < b.read_count; }
};

}  // namespace bookdb::comp