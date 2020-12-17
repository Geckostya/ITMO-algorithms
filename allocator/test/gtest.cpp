#include "gtest/gtest.h"
#include "FixedSizeAllocator.h"
#include <vector>

TEST(sfds, afsdf) {
    std::vector<int *> ints;
    FixedSizeAllocator a(16);
    a.init();
    int *v1;
    for (int i = 0; i < 1000; i++) {
        int *v = (int *) a.alloc(sizeof(int));
        *v = i;
        if (i == 1) {
            v1 = v;
        }
        ints.push_back(v);
    }
    for (int i = 0; i < 1000; i += 2) {
        a.free(ints[i]);
    }
    for (int i = 1; i < 1000; i += 2) {
        a.free(ints[i]);
    }
    a.destroy();
}