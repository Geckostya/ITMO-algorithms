//
// Created by ko on 19.12.2020.
//

#ifndef ALLOCATOR_PAGEALLOCATOR_H
#define ALLOCATOR_PAGEALLOCATOR_H

class PageAllocator : public AbstractAllocator {
public:
    void init() override {}

    void destroy() override {}

    void *alloc(size_t size) override {
        size = size + sizeof(size_t);
        size_t *p = allocPage<size_t>(size);
        p[0] = size;
        return p + 1;
    }

    void free(void *p) override {
        size_t *sp = reinterpret_cast<size_t *>(p) - 1;
        freePage(sp, *sp);
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
        return (size_t)(1) << (sizeof(size_t) * 8 - 1);
    }
};

#endif //ALLOCATOR_PAGEALLOCATOR_H
