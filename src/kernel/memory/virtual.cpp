#include "virtual.hpp"

#include <kernel/asm/asm.hpp>
#include <kernel/memory/physical.hpp>
#include <kernel/memory/utility.hpp>
#include <kernel/memory/kmalloc.hpp>

#include <kernel/vga.hpp>

using namespace kernel;

static uint64_t page_bitmap[256];

#define BITMAP_START 0x600000

void zero_memory(char* src, size_t size)
{
    while (size--) *src++ = 0;
}

void set_entry(page_t* page, paddr_t addr, uint64_t flags)
{
    uint64_t entry = (uint64_t) addr;
    entry |= PAGE_PRESENT | flags;
    *page = entry;
}

void kernel::vmm_init()
{
    zero_memory((char*) &page_bitmap, sizeof(page_bitmap));
    page_t* vmap = create_address_space();
    //_asm_load_cr3((uint64_t) vmap);
    kernel::printf("vmap = %xl\n", vmap);
}

page_t* kernel::create_address_space()
{
    page_t* l2 = alloc_pagetable();
    page_t* l3 = alloc_pagetable();
    page_t* l4 = alloc_pagetable();

    set_entry(l4, (paddr_t) l3, PAGE_RW);
    set_entry(l3, (paddr_t) l2, PAGE_RW);

    // Identity map first 8 MB for kernel
    set_entry(&l2[0], (paddr_t) 0x000000, PAGE_RW | PAGE_SIZE);
    set_entry(&l2[1], (paddr_t) 0x200000, PAGE_RW | PAGE_SIZE);
    set_entry(&l2[2], (paddr_t) 0x400000, PAGE_RW | PAGE_SIZE);
    set_entry(&l2[3], (paddr_t) 0x600000, PAGE_RW | PAGE_SIZE);

    return l4;
}

page_t* kernel::alloc_pagetable()
{
    // Find a free page slot
    int idx     =  0;
    uint64_t FULL = ~0;
    for (; page_bitmap[idx] == FULL; idx++);

    uint64_t qword = page_bitmap[idx];
    int      bit   = __builtin_ffs(~qword) - 1;

    // Mark as taken
    page_bitmap[idx] |= (1 << bit);

    page_t* ptr = (page_t*) (BITMAP_START + 4096 * (idx * 64 + bit));
    zero_memory((char*) ptr, 4096);
    return ptr;
}