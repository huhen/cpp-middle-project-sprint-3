#include <chrono>
#include <gtest/gtest.h>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

using namespace bookdb;

auto InitDataBase() {
    BookDatabase<std::vector<Book>> db;

    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4., 190);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120);
    db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156);
    db.EmplaceBack("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178);
    db.EmplaceBack("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98);
    db.EmplaceBack("Jane Eyre", "Charlotte Brontë", 1847, Genre::Fiction, 4.6, 110);
    db.EmplaceBack("The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203);
    db.EmplaceBack("Lord of the Flies", "William Golding", 1954, Genre::Fiction, 4.2, 89);
    return db;
}

auto InitLargeDataBase() {
    constexpr size_t size = 1000000;

    BookDatabase<std::vector<Book>> db;
    for (size_t i = 0; i < size; ++i) {
        db.EmplaceBack("Book " + std::to_string(i), "Author " + std::to_string(i % 100), 1900 + i % 120, Genre::Fiction,
                       3.0 + (i % 20) / 10.0, i % 200);
    }
    return db;
}

TEST(TestDataBase, EmplaceBackTest) {
    auto db = InitDataBase();
    EXPECT_EQ(db.size(), 10);
    EXPECT_FALSE(db.empty());
}

TEST(TestDataBase, EmptyDB) {
    BookDatabase<std::vector<Book>> db;
    EXPECT_EQ(db.size(), 0);
    EXPECT_TRUE(db.empty());
}

TEST(TestDataBase, ClearDB) {
    auto db = InitDataBase();
    ASSERT_FALSE(db.empty());

    db.Clear();
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
}

TEST(TestDataBase, PushBack) {
    BookDatabase<std::vector<Book>> db;
    db.PushBack({"New Book", "New Author", 2024, Genre::Fiction, 5.0, 1});
    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks().front().title, "New Book");

    Book book{"Second Book", "Second Author", 2023, Genre::SciFi, 4.0, 10};
    db.PushBack(book);
    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db.GetBooks().back().title, "Second Book");
}

TEST(TestDataBase, TypeAliases) {
    static_assert(std::is_same_v<BookDatabase<>::value_type, Book>);
    static_assert(std::is_same_v<BookDatabase<>::reference, Book &>);
    static_assert(std::is_same_v<BookDatabase<>::iterator, std::vector<Book>::iterator>);
    static_assert(std::is_same_v<BookDatabase<>::const_iterator, std::vector<Book>::const_iterator>);
    static_assert(std::is_same_v<BookDatabase<>::size_type, std::vector<Book>::size_type>);
}

TEST(TestDataBase, InitializerListConstructor) {
    BookDatabase db = {{"Book 1", "Author 1", 2001, Genre::Fiction, 4.0, 100},
                       {"Book 2", "Author 2", 2002, Genre::SciFi, 4.5, 200}};
    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db.GetBooks()[0].title, "Book 1");
    EXPECT_EQ(db.GetBooks()[1].author, "Author 2");
}

TEST(TestFilters, YearBetween) {
    auto db = InitDataBase();
    auto filtered = filterBooks(db.begin(), db.end(), YearBetween(1900, 1950));
    EXPECT_EQ(filtered.size(), 5);

    EXPECT_THROW(YearBetween(2000, 1900), std::range_error);
}

TEST(TestFilters, RatingAbove) {
    auto db = InitDataBase();
    auto filtered = filterBooks(db.begin(), db.end(), RatingAbove(4.5));
    EXPECT_EQ(filtered.size(), 4);
}

TEST(TestFilters, GenreIs) {
    auto db = InitDataBase();
    auto filtered = filterBooks(db.begin(), db.end(), GenreIs(Genre::SciFi));
    EXPECT_EQ(filtered.size(), 2);
}

TEST(TestFilters, AllOf) {
    auto db = InitDataBase();
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 2000), RatingAbove(4.5)));
    EXPECT_EQ(filtered.size(), 2);
    EXPECT_EQ(filtered[0].get().title, "To Kill a Mockingbird");
    EXPECT_EQ(filtered[1].get().title, "The Hobbit");
}

TEST(TestFilters, AnyOf) {
    auto db = InitDataBase();
    auto filtered = filterBooks(db.begin(), db.end(), any_of(GenreIs(Genre::SciFi), RatingAbove(4.8)));
    // SciFi: 1984, Brave New World
    // Rating > 4.8: The Hobbit
    EXPECT_EQ(filtered.size(), 3);
}

TEST(TestFilters, EmptyResult) {
    auto db = InitDataBase();
    auto filtered = filterBooks(db.begin(), db.end(), GenreIs(Genre::Biography));
    EXPECT_TRUE(filtered.empty());
}

TEST(TestStatistics, CalculateGenreRatings) {
    auto db = InitDataBase();
    auto genreRatings = calculateGenreRatings(db.begin(), db.end());

    EXPECT_EQ(genreRatings.size(), 2);
    EXPECT_NEAR(genreRatings[Genre::Fiction], 4.55, 0.01);
    EXPECT_NEAR(genreRatings[Genre::SciFi], 4.25, 0.01);
}

TEST(TestStatistics, CalculateAverageRating) {
    auto db = InitDataBase();
    double average = calculateAverageRating(db);
    EXPECT_NEAR(average, 4.49, 0.01);

    BookDatabase<> empty_db;
    average = calculateAverageRating(empty_db);
    EXPECT_EQ(average, 0.0);
}

TEST(TestStatistics, GetTopNBy) {
    auto db = InitDataBase();
    auto top3 = getTopNBy(db, 3, comp::GreatByRating{});

    EXPECT_EQ(top3.size(), 3);
    EXPECT_EQ(top3[0].get().title, "The Hobbit");
    EXPECT_EQ(top3[1].get().title, "To Kill a Mockingbird");
    EXPECT_EQ(top3[2].get().title, "Pride and Prejudice");

    EXPECT_THROW(getTopNBy(db, 11, comp::GreatByRating{}), std::length_error);
}

TEST(TestStatistics, SampleRandomBooks) {
    auto db = InitDataBase();
    auto sampled = sampleRandomBooks(db, 5);
    EXPECT_EQ(sampled.size(), 5);

    EXPECT_THROW(sampleRandomBooks(db, 11), std::length_error);
}

TEST(TestStatistics, BuildAuthorHistogram) {
    auto db = InitDataBase();
    auto histogram = buildAuthorHistogramFlat(db);

    EXPECT_EQ(histogram.size(), 9);
    EXPECT_EQ(histogram["George Orwell"], 2);
    EXPECT_EQ(histogram["J.R.R. Tolkien"], 1);
}

TEST(TestPerformance, BuildAuthorHistogram) {
    auto db = InitLargeDataBase();

    auto start = std::chrono::high_resolution_clock::now();
    auto histogram = buildAuthorHistogram(db);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "buildAuthorHistogram execution time: " << diff.count() << " s\n";

    EXPECT_EQ(histogram.size(), 100);
}

TEST(TestPerformance, BuildAuthorHistogramFlat) {
    auto db = InitLargeDataBase();

    auto start = std::chrono::high_resolution_clock::now();
    auto histogram = buildAuthorHistogramFlat(db);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "buildAuthorHistogramFlat execution time: " << diff.count() << " s\n";

    EXPECT_EQ(histogram.size(), 100);
}
