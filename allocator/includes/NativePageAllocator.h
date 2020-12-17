//
// Created by ko on 17.12.2020.
//

#ifndef ALLOCATOR_NATIVEPAGEALLOCATOR_H
#define ALLOCATOR_NATIVEPAGEALLOCATOR_H

#include <cstddef>

#ifdef WINDOWS
#include <windows.h>
#include <memoryapi.h>
#endif

void *allocPage(size_t size) {
#ifdef WINDOWS
    return VirtualAlloc(
            NULL,
            size,
            MEM_COMMIT,
            PAGE_READWRITE
        );
#else
    return malloc(size);
#endif
}

template<typename T>
inline T *allocPage(size_t size) {
    return reinterpret_cast<T *>(allocPage(size));
}

void freePage(void *p, size_t size) {
#ifdef WINDOWS
    VirtualFree(
            p,
            size,
            MEM_RELEASE
        );
#else
    free(p);
#endif
}

#endif //ALLOCATOR_NATIVEPAGEALLOCATOR_H
