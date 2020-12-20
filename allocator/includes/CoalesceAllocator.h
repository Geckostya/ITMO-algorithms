//
// Created by ko on 18.12.2020.
//

#ifndef ALLOCATOR_COALESCEALLOCATOR_H
#define ALLOCATOR_COALESCEALLOCATOR_H

#ifdef DEBUG
#include <iostream>
#endif

class CoalesceAllocator : public AbstractAllocator {
private:
    static const size_t LEFT_CONSUMED = 1 << 30;
    static const size_t IS_CONSUMED = 1 << 29;
    static const size_t ALL_FLAGS = LEFT_CONSUMED | IS_CONSUMED;

    static size_t getValue(size_t v) {
        return v & (~ALL_FLAGS);
    }

    static size_t setFlag(size_t v, size_t flag) {
        return v | flag;
    }

    static bool hasFlag(size_t v, size_t flag) {
        return (v & flag) == 0 ? 0 : 1;
    }

    static size_t getFlag(size_t v, size_t flag) {
        return v & flag;
    }

    struct Block {
        size_t flaggedSize;

        byte *userData() {
            return toByte(this + 1);
        }
    };

    struct FreeBlock {
        size_t flaggedSize;
#ifdef DEBUG
        size_t value = 0x5afe;
#endif
        FreeBlock *prev;
        FreeBlock *next;

        size_t *shiftRight(size_t shift) { return reinterpret_cast<size_t *>(toByte(this) + shift); }

        FreeBlock *rightBlock() {
            FreeBlock *block = reinterpret_cast<FreeBlock *>(shiftRight(getValue(flaggedSize)));
            bool notFree = hasFlag(block->flaggedSize, IS_CONSUMED) || block->flaggedSize == 0;
#ifdef DEBUG
            if (!notFree) assert(block->value == 0x5afe);
#endif
            return notFree ? nullptr : block;
        }

        FreeBlock *leftBlock() {
            size_t leftSize = *reinterpret_cast<size_t *>(toByte(this) - sizeof(size_t));
            bool notFree = hasFlag(flaggedSize, LEFT_CONSUMED) || leftSize == 0;
#ifdef DEBUG
            if (!notFree) assert((reinterpret_cast<FreeBlock *>(toByte(this) - leftSize))->value == 0x5afe);
#endif
            return notFree ? nullptr : reinterpret_cast<FreeBlock *>(toByte(this) - leftSize);
        }

        void setSize(size_t newSize, size_t flag) {
            this->flaggedSize = newSize | flag;
            size_t *endSize = reinterpret_cast<size_t *>(toByte(this) + newSize - sizeof(size_t));
            *endSize = newSize;
        }

        void removeFromList(FreeBlock **head) {
            if (prev) prev->next = next;
            if (next) next->prev = prev;
            if (*head == this) {
                *head = next;
            }
        }

        Block *takeBlock(size_t blockSize, FreeBlock **head) {
            size_t restSize = getValue(flaggedSize) - blockSize;
            if (restSize >= MIN_SIZE) {
                FreeBlock *newBlock = reinterpret_cast<FreeBlock *>(toByte(this) + blockSize);
                newBlock->next = next;
                if (next) next->prev = newBlock;
                newBlock->setSize(restSize, 0);
#ifdef DEBUG
                newBlock->value = 0x5afe;
#endif
                next = newBlock;
            } else {
                blockSize = getValue(flaggedSize);
            }
            size_t *right_size = shiftRight(blockSize);
            if (*right_size != 0) {
                *right_size |= LEFT_CONSUMED;
            }
            removeFromList(head);
            Block *block = reinterpret_cast<Block *>(this);
            block->flaggedSize = setFlag(blockSize, getFlag(flaggedSize, LEFT_CONSUMED) | IS_CONSUMED);
            return block;
        }

        FreeBlock *joinRight(FreeBlock **head) {
            FreeBlock *right = rightBlock();
            if (right == nullptr) return this;
            setSize(getValue(flaggedSize) + getValue(right->flaggedSize), getFlag(flaggedSize, LEFT_CONSUMED));
            right->removeFromList(head);
            return this;
        }
    };

    struct MemPage {
        MemPage *nextPage;
        size_t freeBlockEndSize;
    };

    static const size_t PAGE_SIZE = 1 << 24;
    static const size_t DATA_SIZE = PAGE_SIZE - sizeof(MemPage) - sizeof(size_t);
    static const size_t MIN_SIZE = sizeof(FreeBlock) + sizeof(size_t);

private:
    MemPage *mem = nullptr;
    FreeBlock *freeBlocksHead = nullptr;

#ifdef DEBUG
    size_t blocksCount = 0;
    size_t consumed_memory = 0;
#endif

private:
    inline FreeBlock *firstBlock() {
        return reinterpret_cast<FreeBlock *>(mem + 1);
    }

    inline void newPage() {
        MemPage *oldPage = mem;
        mem = allocPage<MemPage>(PAGE_SIZE);
        mem->nextPage = oldPage;
        mem->freeBlockEndSize = 0;
        *reinterpret_cast<size_t *>(toByte(mem) + PAGE_SIZE - sizeof(size_t)) = -1;
        freeBlocksHead = firstBlock();
        freeBlocksHead->flaggedSize = DATA_SIZE;
        freeBlocksHead->next = nullptr;
        freeBlocksHead->prev = nullptr;
#ifdef DEBUG
        freeBlocksHead->value = 0x5afe;
#endif
    }

    FreeBlock *findFreeBlock(size_t size) {
        for (FreeBlock *block = freeBlocksHead; block != nullptr; block = block->next) {
            if (getValue(block->flaggedSize) >= size) {
                return block;
            }
        }
        return nullptr;
    }

public:
    ~CoalesceAllocator() {
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
#ifdef DEBUG
        blocksCount = 0;
        consumed_memory = 0;
#endif
        MemPage *tmp;
        do {
#ifdef DEBUG
            assert(firstBlock()->value == 0x5afe);
            assert(firstBlock()->flaggedSize == DATA_SIZE);
#endif
            tmp = mem->nextPage;
            freePage(mem, PAGE_SIZE);
            mem = tmp;
        } while (mem != nullptr);
    }


    void *alloc(size_t size) override {
        assert(mem != nullptr);
        size += sizeof(Block);
        if (size < MIN_SIZE) {
            size = MIN_SIZE;
        }
        assert(size <= DATA_SIZE);
#ifdef DEBUG
        blocksCount++;
        consumed_memory += size;
#endif
        FreeBlock *freeBlock = findFreeBlock(size);
        if (freeBlock == nullptr) {
            newPage();
            freeBlock = freeBlocksHead;
        }

        return freeBlock->takeBlock(size, &freeBlocksHead)->userData();
    }


    void free(void *p) override {
        assert(mem != nullptr);
#ifdef DEBUG
        assert(isInAllocRange(p));
#endif
        Block *block = reinterpret_cast<Block *>(toByte(p) - sizeof(size_t));
#ifdef DEBUG
        blocksCount--;
        consumed_memory -= getValue(block->flaggedSize);
#endif
        assert(hasFlag(block->flaggedSize, IS_CONSUMED));
        FreeBlock *freeBlock = reinterpret_cast<FreeBlock *>(block);
        freeBlock->setSize(getValue(block->flaggedSize), getFlag(block->flaggedSize, LEFT_CONSUMED));
        freeBlock->prev = nullptr;
        freeBlocksHead->prev = freeBlock;
        freeBlock->next = freeBlocksHead;
#ifdef DEBUG
        freeBlock->value = 0x5afe;
#endif

        freeBlocksHead = freeBlock;
        FreeBlock *left = freeBlock->leftBlock();
        if (left != nullptr) {
            freeBlock = left->joinRight(&freeBlocksHead);
        }
        freeBlock->joinRight(&freeBlocksHead);

        size_t *right_size = freeBlock->shiftRight(getValue(freeBlock->flaggedSize));
        if (*right_size != 0) {
            *right_size &= ~LEFT_CONSUMED;
        }
    }

#ifdef DEBUG

    void dumpStat() const override {
        int freeCount = 0;
        for (FreeBlock *block = freeBlocksHead; block != nullptr; block = block->next) {
            freeCount++;
        }
        std::cout << "Consumed blocks: " << blocksCount << "; Free blocks: " << freeCount << std::endl;

        int pageCount = 0;
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            pageCount++;
        }
        std::cout << "Memory consumed: " << consumed_memory << " / " << pageCount * DATA_SIZE << std::endl;
        std::cout << "Consumed OS blocks: " << pageCount << std::endl;
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            std::cout << "page " << (void *) page << ' ' << PAGE_SIZE << std::endl;
        }
    }

    void dumpBlock() const override {
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            FreeBlock *block = reinterpret_cast<FreeBlock *>(page + 1);
            while (toByte(block) < toByte(page + 1) + DATA_SIZE) {
                if (hasFlag(block->flaggedSize, IS_CONSUMED)) {
                    std::cout << (void *) block << ' ' << getValue(block->flaggedSize) << std::endl;
                }
                block = reinterpret_cast<FreeBlock *>(block->shiftRight(getValue(block->flaggedSize)));
            }
        }
    }

#endif

    bool isInAllocRange(void *p) const override {
        for (MemPage *page = mem; page != nullptr; page = page->nextPage) {
            if (toByte(page) < toByte(p) && toByte(p) < toByte(page) + PAGE_SIZE) {
                return true;
            }
        }
        return false;
    }

    size_t maxAllocSize() const override {
        return DATA_SIZE - sizeof(size_t);
    }
};

#endif //ALLOCATOR_COALESCEALLOCATOR_H
