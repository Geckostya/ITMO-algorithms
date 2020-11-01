#include "gtest/gtest.h"
#include "Dictionary.h"
#include <random>
#include <chrono>
#include <functional>


TEST(avl, complexTest) {
    myalg::Dictionary<int, int> l{};
    l.put(1, 1);
    l.put(5, 5);
    l.put(2, 2);
    l.put(3, 3);
    l.put(8, 88);
    l.put(9, 0);
    assert(l.find(1)->v == 1);
    l.remove(1);
    assert(!l.find(1));
    assert(l.find(5)->v == 5);
    assert(l.find(2)->v == 2);
    assert(l.find(3)->v == 3);
    assert(l.find(8)->v == 88);
    assert(l.find(9)->v == 0);
    l.put(1, 10);
    assert(l.find(1)->v == 10);
}

TEST(avl, operatorGet) {
    myalg::Dictionary<int, int> l{};
    l.put(1, 1);
    l.put(3, 2);
    const auto& cl = l;
    ASSERT_EQ(1, cl.at(1));
    ASSERT_EQ(2, cl.at(3));

    ASSERT_EQ(1, l[1]);
    ASSERT_EQ(2, l[3]);
    ASSERT_EQ(0, l[5]);
    ASSERT_TRUE(l.contains(5));
}

TEST(avl, operatorSet) {
    myalg::Dictionary<int, int> l{};
    const auto& cl = l;
    l[1] = 5;
    l[2] = 10;
    l[3] = 15;
    ASSERT_EQ(5, l[1]);
    ASSERT_EQ(10, l[2]);
    ASSERT_EQ(15, l[3]);
    ASSERT_EQ(15, cl.at(3));
}

TEST(avl, delRoot1) {
    myalg::Dictionary<int, int> l{};
    l[1] = 5;
    ASSERT_EQ(5, l[1]);
    l.remove(1);
    ASSERT_EQ(0, l.size());
    ASSERT_EQ(0, l[1]);
    ASSERT_EQ(1, l.size());
}

TEST(avl, delRoot2) {
    myalg::Dictionary<int, int> l{};
    l[1] = 5;
    l[2] = 12;
    ASSERT_EQ(5, l[1]);
    l.remove(1);
    ASSERT_EQ(1, l.size());
    ASSERT_EQ(12, l[2]);
    ASSERT_EQ(0, l[1]);
    ASSERT_EQ(2, l.size());
}

TEST(avl, delChild) {
    myalg::Dictionary<int, int> l{};
    l[1] = 5;
    l[2] = 12;
    ASSERT_EQ(5, l[1]);
    l.remove(2);
    ASSERT_EQ(1, l.size());
    ASSERT_EQ(5, l[1]);
    ASSERT_EQ(0, l[2]);
    ASSERT_EQ(2, l.size());
}

TEST(avl, rotateRight) {
    myalg::Dictionary<int, int> l{};
    l[2] = 2;
    l[1] = 1;
    l[5] = 5;
    l[4] = 4;
    l[3] = 3;
    ASSERT_EQ(5, l.size());
    ASSERT_EQ(5, l[5]);
    ASSERT_EQ(4, l[4]);
    ASSERT_EQ(3, l[3]);
    ASSERT_EQ(2, l[2]);
    ASSERT_EQ(1, l[1]);
}

TEST(avl, rotateBigRight) {
    myalg::Dictionary<int, int> l{};
    l[2] = 2;
    l[1] = 1;
    l[5] = 5;
    l[3] = 3;
    l[4] = 4;
    ASSERT_EQ(5, l.size());
    ASSERT_EQ(5, l[5]);
    ASSERT_EQ(4, l[4]);
    ASSERT_EQ(3, l[3]);
    ASSERT_EQ(2, l[2]);
    ASSERT_EQ(1, l[1]);
}

TEST(avl, rotateLeft) {
    myalg::Dictionary<int, int> l{};
    l[2] = 2;
    l[1] = 1;
    l[3] = 3;
    l[4] = 4;
    l[5] = 5;
    ASSERT_EQ(5, l.size());
    ASSERT_EQ(1, l[1]);
    ASSERT_EQ(2, l[2]);
    ASSERT_EQ(3, l[3]);
    ASSERT_EQ(4, l[4]);
    ASSERT_EQ(5, l[5]);
}

TEST(avl, rotateBigLeft) {
    myalg::Dictionary<int, int> l{};
    l[2] = 2;
    l[1] = 1;
    l[3] = 3;
    l[5] = 5;
    l[4] = 4;
    ASSERT_EQ(5, l.size());
    ASSERT_EQ(1, l[1]);
    ASSERT_EQ(2, l[2]);
    ASSERT_EQ(3, l[3]);
    ASSERT_EQ(4, l[4]);
    ASSERT_EQ(5, l[5]);
}


TEST(avl, stressTest) {
    bool verbose = false;
    bool printAll = false;
    int N = 100, M = 10000;
    long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::cout << "Stress test seed: " << seed << std::endl;
    std::mt19937 rand(seed);
    const std::uniform_int_distribution<int> distribution(0, 100);
    auto int_rand = std::bind(distribution, rand);

    for (int iter = 0; iter < M; iter++) {
        myalg::Dictionary<int, int> dict;
        std::map<int, int> mapp;
        if (verbose) {
            std::cout << std::endl << std::endl;
        }
        for (int ttt = 0; ttt < N; ttt++) {
            ASSERT_EQ(dict.size(), mapp.size());
            int limit = dict.size() > 0 ? 0 : 1;
            int action = int_rand() % (2 - limit) + limit;
            int key = int_rand();
            int value = int_rand();
            ASSERT_EQ(dict.contains(key), mapp.count(key) > 0);
            switch (action) {
                case 0:
                    dict.remove(key);
                    mapp.erase(key);
                    ASSERT_EQ(dict.contains(key), mapp.count(key) > 0);
                    break;
                case 1:
                    dict[key] = value;
                    mapp[key] = value;
                    ASSERT_EQ(dict.at(key), mapp.at(key));
                    break;
                default:
                    break;
            }
            ASSERT_EQ(dict.size(), mapp.size());
            if (printAll) {
                bool equal = dict.size() == mapp.size();
                auto t0 = dict.iterator();
                for (auto t1 = mapp.begin(); equal && !t0.isEnd() && t1 != mapp.end(); t0.next(), t1++) {
                    if (t0.key() != t1->first || *t0 != t1->second) {
                        std::cout << '(' << t0.key() << ' ' << *t0 << ") (" << t1->first << ' ' << t1->second << ")\n";
                        equal = false;
                    }
                }
                if (!equal) {
                    std::cout << dict.size() << ':';
                    for (auto t = dict.iterator(); !t.isEnd(); t.next()) {
                        std::cout << t.key() << ' ' << *t << ' ';
                    }
                    std::cout << std::endl;
                    std::cout << mapp.size() << ':';
                    for (auto &t : mapp) {
                        std::cout << t.first << ' ' << t.second << ' ';
                    }
                    std::cout << std::endl;
                }
            }
        }
    }
}