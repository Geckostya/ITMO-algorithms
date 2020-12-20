#ifndef ALLOCATOR_FIXEDSIZEALLOCATOR_H
#define ALLOCATOR_FIXEDSIZEALLOCATOR_H

#include "AbstractAllocator.h"
#include "NativePageAllocator.h"

#ifdef WINDOWS
#include <algorithm> // std::min for windows
#endif

#ifdef DEBUG
#include <set>
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
    size_t initializedBlocks = 0;

private:
    inline Block *getBlock(int i, MemPage *page) const {
        return reinterpret_cast<Block *>(toByte(page + 1) + i * blockSize);
    }

    inline Block *getBlock(int i) const {
        return getBlock(i, mem);
    }

    inline Block *firstBlock() const {
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
        int freeCount = 0;
        for (Block *block = freeBlocksHead; block != nullptr; block = block->next) {
            freeCount++;
        }
        int pageCount = 0, blocksCount = -freeCount;
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            pageCount++;
            blocksCount += page == mem ? std::min(initializedBlocks, DATA_SIZE / blockSize) : DATA_SIZE / blockSize;
        }
        std::cout << "Consumed blocks: " << blocksCount << "; Free blocks: " << freeCount << std::endl;

        std::cout << "Memory consumed: " << blocksCount * blockSize << " / " << pageCount * DATA_SIZE << std::endl;
        std::cout << "Consumed OS blocks: " << pageCount << std::endl;
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            std::cout << "page " << (void *) page << ' ' << PAGE_SIZE << std::endl;
        }
    }

    void dumpBlock() const override {
        std::set<Block *> allBlocks;
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            size_t size = page == mem ? std::min(initializedBlocks, DATA_SIZE / blockSize) : DATA_SIZE / blockSize;
            for (int i = 0; i < size; i++) {
                Block *block = getBlock(i, page);
                allBlocks.insert(block);
            }
        }
        for (Block *block = freeBlocksHead; block != nullptr; block = block->next) {
            allBlocks.erase(block);
        }
        for (Block *block : allBlocks) {
            std::cout << (void *) block << ' ' << blockSize << std::endl;
        }

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
