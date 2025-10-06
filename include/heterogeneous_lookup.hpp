#pragma once

#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    template <typename T, typename U>
    bool operator()(const T &lhs, const U &rhs) const {
        return std::string_view(lhs) < std::string_view(rhs);
    }
};

struct TransparentStringEqual {
    using is_transparent = void;

    template <typename T, typename U>
    bool operator()(const T &lhs, const U &rhs) const {
        return std::string_view(lhs) == std::string_view(rhs);
    }
};

struct TransparentStringHash {
    using is_transparent = void;

    template <typename T>
    size_t operator()(const T &str) const {
        return std::hash<std::string_view>{}(str);
    }
};

}  // namespace bookdb
