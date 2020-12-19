//
// Created by ko on 19.12.2020.
//

#ifndef ALLOCATOR_MEMORYALLOCATOR_H
#define ALLOCATOR_MEMORYALLOCATOR_H

#include "FixedSizeAllocator.h"
#include "CoalesceAllocator.h"
#include "PageAllocator.h"

class MemoryAllocator : public AbstractAllocator {
    static const size_t N = 8;

    FixedSizeAllocator fsa16;
    FixedSizeAllocator fsa32;
    FixedSizeAllocator fsa64;
    FixedSizeAllocator fsa128;
    FixedSizeAllocator fsa256;
    FixedSizeAllocator fsa512;
    CoalesceAllocator ca;
    PageAllocator pa;
    AbstractAllocator **allocators;

    bool initialized = false;

public:
    MemoryAllocator() : fsa16(16), fsa32(32), fsa64(64), fsa128(128),
                        fsa256(256), fsa512(512), ca(), pa() {
        allocators = new AbstractAllocator *[N]{&fsa16, &fsa32, &fsa64, &fsa128, &fsa256, &fsa512, &ca, &pa};
    }

    ~MemoryAllocator() {
        assert(!initialized);
        if (initialized) {
            destroy();
        }
        delete[] allocators;
    }

    void init() override {
        for (int i = 0; i < 8; i++) {
            allocators[i]->init();
        }
        initialized = true;
    }

    void destroy() override final {
        for (int i = 0; i < 8; i++) {
            allocators[i]->destroy();
        }
        initialized = false;
    }

    void *alloc(size_t size) override {
        for (int i = 0; i < 8; i++) {
            if (size < allocators[i]->maxAllocSize()) {
                return allocators[i]->alloc(size);
            }
        }
        return nullptr;
    }

    void free(void *p) override {
        for (int i = 0; i < 8; i++) {
            if (allocators[i]->isInAllocRange(p)) {
                allocators[i]->free(p);
                return;
            }
        }
    }

#ifdef DEBUG

    void dumpStat() const override {

    }

    void dumpBlock() const override {

    }

#endif

    bool isInAllocRange(void *p) const override {
        return true;
    }

    size_t maxAllocSize() const override {
        return pa.maxAllocSize();
    }
};

#endif //ALLOCATOR_MEMORYALLOCATOR_H
