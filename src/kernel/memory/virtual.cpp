#include "virtual.hpp"

#include <kernel/asm/asm.hpp>
#include <kernel/memory/physical.hpp>
#include <kernel/vga.hpp>
#include <kernel/utility/utility.hpp>

using namespace kernel;

#define _1GB (1 << 30)
#define _2MB (1 << 21)
#define _4KB (1 << 12)

#define L4IDX(v) ((v & 0xFFFFFF8000000000) >> 39)
#define L3IDX(v) ((v & 0x0000007FC0000000) >> 30)
#define L2IDX(v) ((v & 0x000000003FE00000) >> 21)
#define L1IDX(v) ((v & 0xFFFFFF00001FF000) >> 12)

#define ALIGN(addr, align) (addr & ~(align >> 1))

const uint64_t kernel::ID_OFFSET     = 1UL << 46;
const uint64_t kernel::IDENTITY_PAGE = 0x8000;
const uint64_t kernel::L3P           = 0x7000;
const uint64_t kernel::L4P           = 0x6000;

#define HIGH(ADDR) (ADDR + ID_OFFSET)
#define LOW(ADDR)  (ADDR - ID_OFFSET)

void set_entry(page_t* page, frame_t addr, uint64_t flags)
{
    uint64_t entry = (uint64_t) addr;
    entry |= PAGE_PRESENT | flags;
    *page = entry;
}

static bool _is_present(page_t page)
{
    return (page & 1) == 1;
}

static page_t* _get_entry(page_t page)
{
    return (page_t*) (page & ~0xFFF);
}

static void _init_identity()
{
    // Zero the page
    utility::mem_zero((uint8_t*) IDENTITY_PAGE, 4096);

    // Calculate total amount RAM in GB
    // This is used for large (1 GB) paging
    uint64_t mem_total = kernel::get_total_memory();
    uint64_t gb        = utility::uint_divceil(mem_total, _1GB + 0UL);

    page_t* ident = (page_t*) IDENTITY_PAGE;

    // Identity map in increments of 1 GB
    while (gb--)
        set_entry(&ident[gb], gb * _1GB, PAGE_RW | PAGE_SIZE);
}

static void _map_identity(page_t* l4)
{
    static uint64_t idx = ID_OFFSET >> 39;
    set_entry(&l4[idx], IDENTITY_PAGE, PAGE_RW);
}

void kernel::vmm_init()
{
    _init_identity();
    _map_identity((page_t*) L4P);

    auto ptr = create_address_space();
    _asm_load_cr3((uint64_t) ptr);
}

vaddr_t kernel::create_address_space()
{
    page_t* l4 = alloc_pagetable();
    page_t* l3 = alloc_pagetable();
    page_t* l2 = alloc_pagetable();

    // Set up kernel address space
    for (int idx = 0; idx < 4; idx++)
        set_entry(&l2[idx], (frame_t) _2MB * idx, PAGE_RW | PAGE_SIZE);

    // Set up higher-half identity map
    _map_identity((page_t*) l4);

    // Link the tables together
    set_entry(&l4[0], LOW((frame_t) l3), PAGE_RW);
    set_entry(&l3[0], LOW((frame_t) l2), PAGE_RW);

    return LOW((vaddr_t) l4);
}

page_t* kernel::alloc_pagetable()
{
    frame_t new_frame = kernel::pmm_alloc(1);
    new_frame         = HIGH(new_frame);

    utility::mem_zero((uint8_t*) new_frame, 4096);

    return (page_t*) new_frame;
}

static void _alloc_table(page_t* entry)
{
    frame_t temp = LOW((frame_t) alloc_pagetable());
    set_entry(entry, temp, PAGE_RW);
}

// This is the slowest possible way to do this,
// but also the simplest
static void _map_4k(vaddr_t addr)
{
    auto l4i = L4IDX(addr);
    auto l3i = L3IDX(addr);
    auto l2i = L2IDX(addr);
    auto l1i = L1IDX(addr);

    // Traverse the paging map and allocate more tables if needed
    page_t *l4, *l3, *l2, *l1;

    l4 = (page_t*) HIGH(_asm_read_cr3());

    if (!_is_present(l4[l4i]))
        _alloc_table(&l4[l4i]);

    l3 = (page_t*) HIGH((frame_t) _get_entry(l4[l4i]));

    if (!_is_present(l3[l3i]))
        _alloc_table(&l3[l3i]);

    l2 = (page_t*) HIGH((frame_t) _get_entry(l3[l3i]));

    if (!_is_present(l2[l2i]))
        _alloc_table(&l2[l2i]);
    
    l1 = (page_t*) HIGH((frame_t) _get_entry(l2[l2i]));

    // Finally, allocate the 4 KB page
    if (_is_present(l1[l1i]))
        return;

    frame_t frame = kernel::pmm_alloc(1);
    set_entry(&l1[l1i], frame, PAGE_RW);
}

void kernel::map(vaddr_t vaddr, size_t length)
{
    size_t pages = utility::uint_divceil(length, _4KB + 0UL);
    vaddr        = ALIGN(vaddr, _4KB);

    while (pages--)
        _map_4k(vaddr + pages * _4KB);
}

void kernel::load_address_space(page_t* root)
{
    _asm_load_cr3((uint64_t) root);
}

page_t* kernel::read_address_space()
{
    return (page_t*) _asm_read_cr3();
}