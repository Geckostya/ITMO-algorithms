#include "gtest/gtest.h"
#include "list.h"
#include <random>
#include <functional>
#include <chrono>
#include <deque>

using namespace myalg;

TEST(list, testInit) {
    List<int> list;
}

TEST(list, insertHead) {
    List<int> list;
    ASSERT_EQ(list.size(), 0);
    list.insertHead(123);
    ASSERT_EQ(123, list.head());
    ASSERT_EQ(list.size(), 1);
    list.insertHead(312);
    ASSERT_EQ(312, list.head());
    ASSERT_EQ(list.size(), 2);
    list.insertHead(231);
    ASSERT_EQ(231, list.head());
    ASSERT_EQ(list.size(), 3);
}

TEST(list, removeHead) {
    List<int> list;
    list.insertHead(123);
    list.insertHead(312);
    list.insertHead(231);
    ASSERT_EQ(3, list.size());
    list.removeHead();
    ASSERT_EQ(312, list.head());
    ASSERT_EQ(2, list.size());
    list.removeHead();
    ASSERT_EQ(123, list.head());
    ASSERT_EQ(1, list.size());
    list.removeHead();
    ASSERT_EQ(0, list.size());
}

TEST(list, insertTail) {
    List<int> list;
    ASSERT_EQ(0, list.size());
    list.insertTail(123);
    ASSERT_EQ(123, list.tail());
    ASSERT_EQ(1, list.size());
    list.insertTail(312);
    ASSERT_EQ(312, list.tail());
    ASSERT_EQ(2, list.size());
    list.insertTail(231);
    ASSERT_EQ(231, list.tail());
    ASSERT_EQ(3, list.size());
}

TEST(list, removeTail) {
    List<int> list;
    list.insertTail(123);
    list.insertTail(312);
    list.insertTail(231);
    ASSERT_EQ(list.size(), 3);
    list.removeTail();
    ASSERT_EQ(312, list.tail());
    ASSERT_EQ(list.size(), 2);
    list.removeTail();
    ASSERT_EQ(123, list.tail());
    ASSERT_EQ(list.size(), 1);
    list.removeTail();
    ASSERT_EQ(list.size(), 0);
}


TEST(list, testRemove) { //TODO
    List<int> list;
    list.insertTail(14);
    list.insertHead(84);
    list.insertHead(40);
    list.insertTail(95);
    list.insertTail(10);
    list.removeTail();
}

TEST(list, stressTest) {
    bool verbose = false;
    bool printAll = false;
    int N = 100, M = 10000;
    long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::cout << "Stress test seed: " << seed << std::endl;
    std::mt19937 rand(seed);
    const std::uniform_int_distribution<int> distribution(0, 100);
    auto int_rand = std::bind(distribution, rand);
    
    for (int iter = 0; iter < M; iter++) {
        List<int> list;
        std::deque<int> deque;
        if (verbose) {
            std::cout << std::endl << std::endl;
        }
        for (int ttt = 0; ttt < N; ttt++) {
            ASSERT_EQ(list.size(), deque.size());
            int limit = list.size() > 0 ? 0 : 2;
            int action = int_rand() % (4 - limit) + limit;
            int value = int_rand();
            if (verbose) {
                std::cout << (action == 0 ? "removeHead()" : action == 1 ? "removeTail()" : action == 2 ? "insertHead("
                                                                                                        : "insertTail(");
                if (action > 1) {
                    std::cout << value << ")";
                }
                std::cout << std::endl;
            }
            switch (action) {
                case 0:
                    list.removeHead();
                    deque.pop_front();
                    break;
                case 1:
                    list.removeTail();
                    deque.pop_back();
                    break;
                case 2:
                    list.insertHead(value);
                    deque.push_front(value);
                    break;
                case 3:
                    list.insertTail(value);
                    deque.push_back(value);
                    break;
                default:
                    break;
            }
            ASSERT_EQ(list.size(), deque.size());
            if (printAll) {
                std::cout << list.size() << ':';
                for (auto t = list.iterator(); !t.isOutOfRange(); t.next()) {
                    std::cout << t.get() << ' ';
                }
                std::cout << std::endl;
                std::cout << deque.size() << ':';
                for (auto t = deque.begin(); t != deque.end(); t++) {
                    std::cout << *t << ' ';
                }
                std::cout << std::endl;
            }
            if (list.size() > 0) {
                ASSERT_EQ(list.head(), deque.front());
                ASSERT_EQ(list.tail(), deque.back());
            }
        }
    }
}

TEST(list, iterNext) {
    List<int> list;
    for (int i = 0; i < 10; i++) {
        list.insertTail(i * 3 + 2);
    }
    int i = 0;
    for (auto it = list.iterator(); it.hasNext(); it.next(), i++) {
        ASSERT_EQ(it.get(), i * 3 + 2);
    }
    ASSERT_EQ(9, i);

    i = 0;
    for (auto it = list.iterator(); !it.isOutOfRange(); it.next(), i++) {
        ASSERT_EQ(it.get(), i * 3 + 2);
    }
    ASSERT_EQ(10, i);
}

TEST(list, iterInsert) {
    List<int> list;
    auto it = list.iterator();
    it.insert(4);
    it.insert(1);
    it.insert(0);
    it.next();
    it.next();
    it.insert(3);
    it.insert(2);
    ASSERT_EQ(list.size(), 5);
    int i = 0;
    for (auto it2 = list.iterator(); !it2.isOutOfRange(); it2.next(), i++) {
        ASSERT_EQ(it2.get(), i);
    }
}

TEST(list, iterRemove) {
    List<int> list;
    for (int i = 0; i < 5; i++) {
        list.insertTail(i);
    }
    auto it = list.iterator();
    it.remove();
    ASSERT_EQ(4, list.size());
    int i = 0;
    for (auto it2 = list.iterator(); !it2.isOutOfRange(); it2.next(), i++) {
        ASSERT_EQ(it2.get(), i + 1);
    }
    it.next();
    it.remove();
    ASSERT_EQ(1, list.head());
    i = 1;
    auto it2 = list.iterator();
    it2.next();
    for (; !it2.isOutOfRange(); it2.next(), i++) {
        ASSERT_EQ(it2.get(), i + 2);
    }
}

TEST(list, iterRemoveEnd) {
    List<int> list;
    for (int i = 0; i < 6; i++) {
        list.insertTail(i);
    }
    auto it = list.iterator();
    for (; it.hasNext(); it.next()) {}
    ASSERT_EQ(6, list.size());
    ASSERT_EQ(5, list.tail());
    ASSERT_EQ(5, it.get());
    it.remove();
    ASSERT_EQ(5, list.size());
    ASSERT_EQ(4, list.tail());
    ASSERT_EQ(4, it.get());
    it.remove();
    ASSERT_EQ(4, list.size());
    ASSERT_EQ(3, list.tail());
    ASSERT_EQ(3, it.get());
    it.remove();
    ASSERT_EQ(3, list.size());
    ASSERT_EQ(2, list.tail());
    ASSERT_EQ(2, it.get());
}
