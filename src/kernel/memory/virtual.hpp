#pragma once

#include <stdint.h>
#include <kernel/memory/physical.hpp>

#define PAGE_PRESENT       1 << 0
#define PAGE_RW            1 << 1
#define PAGE_USER          1 << 2
#define PAGE_WRITETHROUGH  1 << 3
#define PAGE_CACHE_DISABLE 1 << 4
#define PAGE_ACCESSED      1 << 5
#define PAGE_DIRTY         1 << 6
#define PAGE_SIZE          1 << 7
#define PAGE_GLOBAL        1 << 8
#define PAGE_SOFTWARE      1 << 9
#define PAGE_NX            1 << 63

namespace kernel
{
    using vaddr_t = uint64_t;
    using page_t  = uint64_t;

    extern const uint64_t ID_OFFSET;
    extern const uint64_t IDENTITY_PAGE;
    extern const uint64_t L3P;
    extern const uint64_t L4P;

    void    vmm_init();
    vaddr_t create_address_space();
    page_t* alloc_pagetable();

    void map(vaddr_t vaddr, size_t length);

    void load_address_space(page_t* root);
    page_t* read_address_space();
}