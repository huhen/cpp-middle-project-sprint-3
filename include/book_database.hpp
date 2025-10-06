#pragma once

#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases
    using value_type = typename BookContainer::value_type;
    using reference = typename BookContainer::reference;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;
    using size_type = typename BookContainer::size_type;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<value_type> _il) {

        books_.reserve(_il.size());
        std::for_each(_il.begin(), _il.end(), [this](const Book &_book) { PushBack(_book); });
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    constexpr void PushBack(const value_type &book) {
        books_.push_back(book);
        UpdateAuthorReference(books_.back());
    }

    constexpr void PushBack(value_type &&book) {
        books_.push_back(std::move(book));
        UpdateAuthorReference(books_.back());
    }

    template <typename... Args>
    constexpr reference EmplaceBack(Args &&...args) {
        reference ref = books_.emplace_back(std::forward<Args>(args)...);
        UpdateAuthorReference(ref);
        return ref;
    }

    // Standard container interface methods
    bool empty() const noexcept { return books_.empty(); }

    size_type size() const noexcept { return books_.size(); }

    iterator begin() noexcept { return books_.begin(); }

    iterator end() noexcept { return books_.end(); }

    const_iterator begin() const noexcept { return books_.begin(); }

    const_iterator end() const noexcept { return books_.end(); }

    const_iterator cbegin() const noexcept { return books_.cbegin(); }

    const_iterator cend() const noexcept { return books_.cend(); }

    const BookContainer &GetBooks() const noexcept { return books_; }

    const AuthorContainer &GetAuthors() const noexcept { return authors_; }

    operator std::span<const Book>() const { return books_; }

    operator std::span<Book>() { return books_; }

private:
    BookContainer books_;
    AuthorContainer authors_;

    constexpr void UpdateAuthorReference(Book &book) {
        auto it = authors_.find(book.author);
        if (it == authors_.end()) {
            it = authors_.emplace(std::string{book.author}).first;
        }
        book.author = *it;
    }
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
