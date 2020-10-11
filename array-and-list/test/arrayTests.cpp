#include "gtest/gtest.h"
#include "array.h"

using namespace myalg;

TEST(array, testInit) {
    Array<int> arr;
    Array<int> arr2(1123);
}

TEST(array, insertSimpleTest) {
    Array<int> arr;
    ASSERT_EQ(0, arr.size());

    arr.insert(12);
    ASSERT_EQ(1, arr.size());
    ASSERT_EQ(12, arr[0]);

    arr.insert(-123);
    ASSERT_EQ(2, arr.size());
    ASSERT_EQ(-123, arr[1]);
    ASSERT_EQ(12, arr[0]);
}

TEST(array, insertInnerTest) {
    Array<int> arr;
    arr.insert(1);
    arr.insert(2);
    arr.insert(3);
    ASSERT_EQ(3, arr.size());
    ASSERT_EQ(1, arr[0]);
    ASSERT_EQ(2, arr[1]);
    ASSERT_EQ(3, arr[2]);

    arr.insert(3, 4);
    ASSERT_EQ(4, arr[3]);

    arr.insert(0, 5);
    ASSERT_EQ(5, arr[0]);
    for (int i = 1; i < 5; i++) {
        ASSERT_EQ(i, arr[i]);
    }

    arr.insert(3, 6);
    ASSERT_EQ(2, arr[2]);
    ASSERT_EQ(6, arr[3]);
    ASSERT_EQ(3, arr[4]);

    arr.insert(100, 100);
    ASSERT_EQ(101, arr.size());
    ASSERT_EQ(100, arr[100]);
}

TEST(array, remove) {
    Array<int> arr;
    for (int i = 0; i < 10; i++) {
        arr.insert(i);
    }
    ASSERT_EQ(10, arr.size());

    arr.remove(10);
    ASSERT_EQ(9, arr.size());
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(i, arr[i]);
    }

    arr.remove(0);
    ASSERT_EQ(8, arr.size());
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(i + 1, arr[i]);
    }

    arr.remove(4);
    ASSERT_EQ(7, arr.size());
    for (int i = 0; i < 7; i++) {
        ASSERT_EQ( i + (i < 4 ? 1 : 2), arr[i]);
    }
}

TEST(array, iterNext) {
    Array<int> arr;
    for (int i = 0; i < 10; i++) {
        arr.insert(i * 3 + 2);
    }
    int i = 0;
    for (auto it = arr.iterator(); it.hasNext(); it.next(), i++) {
        ASSERT_EQ(it.get(), i * 3 + 2);
    }
}

TEST(array, iterInsert) {
    Array<int> arr;
    auto it = arr.iterator();
    it.insert(4);
    it.insert(1);
    it.insert(0);
    it.next();
    it.next();
    it.insert(3);
    it.insert(2);
    ASSERT_EQ(arr.size(), 5);
    for (int i = 0; i < arr.size(); i++) {
        ASSERT_EQ(i, arr[i]);
    }
}

TEST(array, iterRemove) {
    Array<int> arr;
    for (int i = 0; i < 5; i++) {
        arr.insert(i);
    }
    auto it = arr.iterator();
    it.remove();
    ASSERT_EQ(4, arr.size());
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(i + 1, arr[i]);
    }
    it.next();
    it.remove();
    ASSERT_EQ(1, arr[0]);
    for (int i = 1; i < 3; i++) {
        ASSERT_EQ(i + 2, arr[i]);
    }
}
