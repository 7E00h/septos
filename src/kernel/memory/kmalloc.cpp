#include "kmalloc.hpp"

#include <stdint.h>

#include <kernel/vga.hpp>

#define FLAG_FREE 1

struct mem_block_t;

struct mem_block_t
{
    mem_block_t* next;
    size_t       size;
    uint8_t      flags;
} __attribute__((packed));

#define DATA_START(mem_block) (((char*) mem_block) + sizeof(mem_block_t))

static mem_block_t* mem_head  = nullptr;
static mem_block_t* tail      = nullptr;
static size_t       limit     = 0;
static size_t       allocated = 0;

static mem_block_t* first_fit(size_t size);

void kernel::heap_init(void* begin, size_t limit)
{
    mem_head        = (mem_block_t*) begin;
    mem_head->next  = nullptr;
    mem_head->size  = 0;
    mem_head->flags = 0;
    tail            = mem_head;

    ::limit   = limit;
    allocated = 0;
}

void* kernel::malloc(size_t size)
{
    if (allocated + size > limit)
        return nullptr;

    mem_block_t* region = first_fit(size);

    if (region == nullptr)
    {
        char* next    = DATA_START(tail) + tail->size;
        tail->next    = (mem_block_t*) next;
        region        = tail->next;
        region->size  = size;
        region->flags = 0;
        region->next  = nullptr;
        tail          = region;

        allocated += size;
    }

    return DATA_START(region);
}

void kernel::free(void* addr)
{
    mem_block_t* cur = mem_head;
    while (cur != nullptr && addr != DATA_START(cur))
        cur = cur->next;

    cur->flags |= FLAG_FREE;
}

size_t kernel::get_allocated_memory()
{
    return allocated;
}

mem_block_t* first_fit(size_t size)
{
    mem_block_t* cur = mem_head;
    while (cur != nullptr)
    {
        if ((cur->flags & FLAG_FREE) && cur->size >= size)
            return cur;

        cur = cur->next;
    }

    return nullptr;
}