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
