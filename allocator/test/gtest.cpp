#include "gtest/gtest.h"
#include "FixedSizeAllocator.h"
#include "CoalesceAllocator.h"
#include "MemoryAllocator.h"
#include "PageAllocator.h"
#include <vector>
#include <algorithm>
#include <random>

#ifdef DEBUF
//#define COUT
#endif

void testInitDestroy(AbstractAllocator &a) {
    a.init();
    a.destroy();
    ASSERT_TRUE(true);
}

void testAllocInt(AbstractAllocator &a) {
    a.init();
    int *v = static_cast<int *>(a.alloc(sizeof(int)));
    *v = 16;
    assert(*v == 16);
    int *u = static_cast<int *>(a.alloc(sizeof(int)));
    *u = -32;
    assert(*v == 16);
    assert(*u == -32);
    a.free(v);
    a.free(u);

    a.destroy();
}

void testAllocAfterFree(AbstractAllocator &a) {
    a.init();
    int *z = static_cast<int *>(a.alloc(16));
    int *x = static_cast<int *>(a.alloc(16));
    int *c = static_cast<int *>(a.alloc(16));
    *z = 7;
    *x = 15;
    *c = 21;

    a.free(x);
    ASSERT_EQ(*z, 7);
    ASSERT_EQ(*c, 21);
    x = static_cast<int *>(a.alloc(16)); // same size
    *x = 14;
    a.free(x);
    ASSERT_EQ(*z, 7);
    ASSERT_EQ(*c, 21);
    x = static_cast<int *>(a.alloc(8)); // less size
    *x = 13;
    a.free(x);
    ASSERT_EQ(*z, 7);
    ASSERT_EQ(*c, 21);
    x = static_cast<int *>(a.alloc(32)); // more size
    *x = 12;
    a.free(x);
    ASSERT_EQ(*z, 7);
    ASSERT_EQ(*c, 21);
    a.free(z);
    a.free(c);

    a.destroy();
}

void testAllocLotsInt(AbstractAllocator &a) {
    std::vector<int *> ints;
    a.init();
    for (int i = 0; i < 1000; i++) {
        int *v = static_cast<int *>(a.alloc(sizeof(int)));
        *v = i;
        ints.push_back(v);
    }
    for (int i = 0; i < 1000; i += 2) {
        ASSERT_EQ(*ints[i], i);
    }
    for (int i = 0; i < 1000; i += 2) {
        a.free(ints[i]);
    }
    for (int i = 1; i < 1000; i += 2) {
        a.free(ints[i]);
    }
    a.destroy();
}

void testLotsOfMemory(AbstractAllocator &a) {
    std::vector<int *> ints;
    a.init();
    int iter = 1 << 31 / (256);
    for (int i = 0; i < iter; i++) {
        int *v = static_cast<int *>(a.alloc(256));
        *v = i;
        ints.push_back(v);
    }
    for (int i = 0; i < iter; i ++) {
        ASSERT_EQ(*ints[i], i);
        a.free(ints[i]);
    }
    a.destroy();
}

void generateSeq(byte *&v, int size, int i) {
    int seed = i * i;
    for (int j = 0; j < size; j++, seed++) {
        v[j] = static_cast<byte>(seed);
    }
}

void checkSeq(byte *&v, int size, int i) {
    int seed = i * i;
    for (int j = 0; j < size; j++, seed++) {
        ASSERT_EQ(v[j], static_cast<byte>(seed));
    }
}

void testRandomAllocations(AbstractAllocator &a, int iter) {
    std::vector<byte *> objs;
    std::vector<int> sizes;
    std::vector<int> inds;
    a.init();
    size_t maxSize = std::min((size_t)1024, a.maxAllocSize());

    // fill memory with 2 iter objects
    for (int i = 0; i < 2 * iter; i++) {
        int size = rand() % maxSize;
        byte *v = static_cast<byte *>(a.alloc(size));
        generateSeq(v, size, i);
        objs.push_back(v);
        sizes.push_back(size);
        inds.push_back(i);
    }

#ifdef COUT
    a.dumpBlock();
    a.dumpStat();
#endif

    std::mt19937 g(0x241251);

    std::shuffle(inds.begin(), inds.end(), g);

    // free randomly 1 iter objects
    for (int i = 0; i < iter; i++) {
        int id = inds[i];
        checkSeq(objs[id], sizes[id], id);
        a.free(objs[id]);
    }

#ifdef COUT
    a.dumpBlock();
    a.dumpStat();
#endif

    // add another 1 iter objects
    for (int i = 2 * iter; i < 3 * iter; i++) {
        int size = rand() % maxSize;
        byte *v = static_cast<byte *>(a.alloc(size));
        generateSeq(v, size, i);
        objs.push_back(v);
        sizes.push_back(size);
        inds.push_back(i);
    }

#ifdef COUT
    a.dumpBlock();
    a.dumpStat();
#endif

    // free all
    for (int i = iter; i < 3 * iter; i++) {
        int id = inds[i];
        checkSeq(objs[id], sizes[id], id);
        a.free(objs[id]);
    }

    a.destroy();
}

void testAll(AbstractAllocator &a) {
    testInitDestroy(a);
    testInitDestroy(a); // initialization after destroy
    testAllocInt(a);
    testAllocAfterFree(a);
    testAllocLotsInt(a);
    testLotsOfMemory(a);
    testRandomAllocations(a, 20000);
}

TEST(common_allocators_tests, test_fixed_sized) {
    FixedSizeAllocator a(256);
    testAll(a);
}

TEST(common_allocators_tests, test_coalesce) {
    CoalesceAllocator a;
    testAll(a);
}

TEST(common_allocators_tests, test_page) {
    PageAllocator a;
    testAll(a);
}

TEST(common_allocators_tests, test_mem_alloc) {
    MemoryAllocator a;
    testAll(a);
}

TEST(print_test, dump_coalesce) {
    CoalesceAllocator a;
    testRandomAllocations(a, 10);
}