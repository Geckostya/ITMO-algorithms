#include "gtest/gtest.h"
#include "sort.h"
#include <random>
#include <chrono>
#include <functional>

#define INT_SORT_TEST(testName) template<typename Compare> void testName(void (*sort_function)(int*, int*, Compare), Compare comp)

#define LESS [](int a, int b) { return a < b; }
#define GREATER [](int a, int b) { return a > b; }


template<typename T, typename Compare>
void test_sorted( T *first, T *last, Compare comp) {
    for (T *i = first; first < last - 1; first++) {
        EXPECT_FALSE(comp(*(i + 1), *i));
    }
}

template<typename T, typename Compare>
void test_sorting(void(*sort_function)(T*, T*, Compare), T *first, T *last, Compare comp) {
    sort_function(first, last, comp);
    test_sorted(first, last, comp);
}

void test_have_all_once(const int *arr, int *expected, int n, int unique_value) {
    for (int i = 0; i < n; i++) {
        bool found = false;
        for (int j = 0; j < n; j++) {
            if (arr[i] == expected[j]) {
                found = true;
                expected[j] = unique_value;
                break;
            }
        }
        if (!found) {
            GTEST_FAIL();
        }
    }
}

// Empty corner case
INT_SORT_TEST(test_empty) {
    int a[] = {};
    test_sorting(sort_function, a, a, comp);
}

// One element corner case
INT_SORT_TEST(test_sort_one) {
    int a[] = {1};
    test_sorting(sort_function, a, a + 1, comp);
    EXPECT_EQ(a[0], 1);
}

// Smallest valuable test
INT_SORT_TEST(test_sort_two) {
    int a[] = {1, 2};
    test_sorting(sort_function, a, a + 2, comp);

    int values[] = {1, 2};
    test_have_all_once(a, values, 2, 0);
}

// Small normal test
INT_SORT_TEST(test_sort_six) {
    int a[] = {3, 4, 2, 1, 6, 5};
    test_sorting(sort_function, a, a + 6, comp);

    int values[] = {1, 2, 3, 4, 5, 6};
    test_have_all_once(a, values, 6, 0);
}

// Small normal test with duplicates
INT_SORT_TEST(test_sort_six_with_duplicates) {
    int a[] = {4, 4, 2, 1, 2, 5};
    test_sorting(sort_function, a, a + 6, comp);

    int values[] = {1, 2, 2, 4, 4, 5};
    test_have_all_once(a, values, 6, 0);
}

TEST(insertion_sort, test_empty) {
    test_empty(myalg::insertion_sort, LESS);
}

TEST(insertion_sort, test_sort_one) {
    test_sort_one(myalg::insertion_sort, LESS);
    test_sort_one(myalg::insertion_sort, GREATER);
}

TEST(insertion_sort, test_sort_two) {
    test_sort_two(myalg::insertion_sort, LESS);
    test_sort_two(myalg::insertion_sort, GREATER);
}

TEST(insertion_sort, test_sort_six) {
    test_sort_six(myalg::insertion_sort, LESS);
    test_sort_six(myalg::insertion_sort, GREATER);
}

TEST(insertion_sort, test_sort_six_with_duplicates) {
    test_sort_six_with_duplicates(myalg::insertion_sort, LESS);
    test_sort_six_with_duplicates(myalg::insertion_sort, GREATER);
}



TEST(quick_sort, test_empty) {
    myalg::use_insertion_sort = false;
    test_empty(myalg::sort, LESS);
    myalg::use_insertion_sort = true;
}

TEST(quick_sort, tst_sort_one) {
    myalg::use_insertion_sort = false;
    test_sort_one(myalg::sort, LESS);
    test_sort_one(myalg::sort, GREATER);
    myalg::use_insertion_sort = true;
}

TEST(quick_sort, test_sort_two) {
    myalg::use_insertion_sort = false;
    test_sort_two(myalg::sort, LESS);
    test_sort_two(myalg::sort, GREATER);
    myalg::use_insertion_sort = true;
}

TEST(quick_sort, test_sort_six) {
    myalg::use_insertion_sort = false;
    test_sort_six(myalg::sort, LESS);
    test_sort_six(myalg::sort, GREATER);
    myalg::use_insertion_sort = true;
}

TEST(quick_sort, test_sort_six_with_duplicates) {
    myalg::use_insertion_sort = false;
    test_sort_six_with_duplicates(myalg::sort, LESS);
    test_sort_six_with_duplicates(myalg::sort, GREATER);
    myalg::use_insertion_sort = true;
}

TEST(quick_sort, random_test) {
    int N = 10000;
    std::mt19937 rand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const std::uniform_int_distribution<int> distribution(0, 10000);
    auto int_rand = std::bind(distribution, rand);

    int *a = new int[N];
    int *b = new int[N];

    for (int i = 0; i < N; i++) {
        a[i] = int_rand();
        b[i] = a[i];
    }
    test_sorting(myalg::sort, a, a + N, LESS);
    test_have_all_once(a, b, N, -1);

    delete[] a;
    delete[] b;
}

TEST(quick_sort, random_performance_test) {
    int N = 1000, M = 10000;
    std::mt19937 rand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const std::uniform_int_distribution<int> distribution(0, 10000);
    auto int_rand = std::bind(distribution, rand);

    int *a = new int[N];
    long long time = 0;
    for (int k = 0; k < M; k++) {
        for (int i = 0; i < N; i++) {
            a[i] = int_rand();
        }
        auto start = std::chrono::high_resolution_clock::now();
        myalg::sort(a, a + N, LESS);
        auto end = std::chrono::high_resolution_clock::now();
        time += (end - start).count();
    }
    std::cout << "crono: " << time / M << std::endl;

    delete[] a;
}