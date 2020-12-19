#ifndef ALLOCATOR_FIXEDSIZEALLOCATOR_H
#define ALLOCATOR_FIXEDSIZEALLOCATOR_H

#include "AbstractAllocator.h"
#include "NativePageAllocator.h"

#ifdef WINDOWS
#include <algorithm> // std::min for windows
#endif

class FixedSizeAllocator : public AbstractAllocator {
private:
    struct Block {
#ifdef DEBUG
        size_t value = 0x5afe;
#endif
        Block *next;
    };

    struct MemPage {
        MemPage *nextPage;
    };

    static const size_t DATA_SIZE = 1 << 23;
    static const size_t PAGE_SIZE = DATA_SIZE + sizeof(MemPage);

private:
    const size_t blockSize;
    MemPage *mem = nullptr;
    Block *freeBlocksHead = nullptr;
    int initializedBlocks = 0;

private:
    inline Block *getBlock(int i) {
        return reinterpret_cast<Block *>(toByte(mem + 1) + i * blockSize);
    }

    inline Block *firstBlock() {
        return reinterpret_cast<Block *>(mem + 1);
    }

    inline void newPage() {
        MemPage *oldPage = mem;
        mem = allocPage<MemPage>(PAGE_SIZE);
        mem->nextPage = oldPage;
        freeBlocksHead = firstBlock();
        initializedBlocks = 0;
    }

    inline Block *takeBlock() {
        Block *block = freeBlocksHead;
        freeBlocksHead = freeBlocksHead->next;
        return block;
    }

public:
    FixedSizeAllocator(size_t blockSize) : blockSize(blockSize) {
        assert(blockSize >= sizeof(Block));
        assert(blockSize && !(blockSize & (blockSize - 1))); // check power of two
    }

    ~FixedSizeAllocator() {
        assert(mem == nullptr);
        if (mem != nullptr) {
            destroy();
        }
    }

    void init() override {
        assert(mem == nullptr);
        newPage();
    }

    void destroy() override final {
        assert(mem != nullptr);
        MemPage *tmp;
        do {
#ifdef DEBUG
            byte *end = toByte(getBlock(initializedBlocks));
            for (Block *block = firstBlock(); toByte(block) < end; block = reinterpret_cast<Block *>(toByte(block) + blockSize)) {
                assert(block->value == 0x5afe);
            }
            initializedBlocks = DATA_SIZE / blockSize;
#endif
            tmp = mem->nextPage;
            freePage(mem, PAGE_SIZE);
            mem = tmp;
        } while (mem != nullptr);
    }

    void *alloc(size_t size) override {
        assert(mem != nullptr);
        assert(size <= blockSize);
        if (freeBlocksHead == nullptr) {
            newPage();
        }
        size_t allBlocks = DATA_SIZE / blockSize;
        if (initializedBlocks < allBlocks) {
            Block *block = getBlock(initializedBlocks);
            block->next = ++initializedBlocks < allBlocks ? getBlock(initializedBlocks) : nullptr;
#ifdef DEBUG
            block->value = 0x5afe;
#endif
        }

        Block *block = takeBlock();
#ifdef DEBUG
        assert(block->value == 0x5afe);
#endif
        return block;
    }

    void free(void *p) override {
#ifdef DEBUG
        assert(isInAllocRange(p));
#endif
        assert(mem != nullptr);
        Block *block = reinterpret_cast<Block *>(p);
        block->next = freeBlocksHead;
        freeBlocksHead = block;
#ifdef DEBUG
        block->value = 0x5afe;
#endif
    }

#ifdef DEBUG

    void dumpStat() const override {
        // TODO
    }

    void dumpBlock() const override {
        // TODO
    }

#endif


    bool isInAllocRange(void *p) const override {
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            bool inPage = toByte(page) < toByte(p) && toByte(p) < toByte(page) + PAGE_SIZE;
            if (inPage) {
                bool correctPadding = (toByte(p) - toByte(page + 1)) % blockSize == 0;
                return  correctPadding;
            }
        }
        return false;
    }

    size_t maxAllocSize() const override {
        return blockSize;
    }
};

#endif //ALLOCATOR_FIXEDSIZEALLOCATOR_H
