//
// Created by ko on 17.12.2020.
//

#ifndef ALLOCATOR_ABSTRACTALLOCATOR_H
#define ALLOCATOR_ABSTRACTALLOCATOR_H

#include <cstddef>

typedef unsigned char byte;

inline byte *toByte(void *p) {
    return reinterpret_cast<byte *>(p);
}

class AbstractAllocator {
public:
    virtual ~AbstractAllocator() {}

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual void *alloc(size_t size) = 0;

    virtual void free(void *p) = 0;

#ifdef DEBUG

    virtual void dumpStat() const = 0;

    virtual void dumpBlock() const = 0;

#endif

    virtual bool isInAllocRange(void *p) const = 0;

    virtual size_t maxAllocSize() const = 0;
};

#endif //ALLOCATOR_ABSTRACTALLOCATOR_H
