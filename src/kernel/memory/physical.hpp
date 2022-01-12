#pragma once

#include <stdint.h>
#include <stddef.h>

namespace kernel
{
    enum
    {
        MEM_FREE = 0x01,
        MEM_RESERVED,
        MEM_ACPI_RECLAIMABLE,
        MEM_ACPI_NVS,
        MEM_BAD
    };

    struct mem_info_t
    {
        uint64_t base;
        uint64_t length;
        uint32_t type;
        uint32_t acpi_extended;
    } __attribute__((packed));

    using frame_t = uint64_t;
    using paddr_t = uint8_t*;

    void pmm_init(mem_info_t* mem_info, size_t amt);

    frame_t pmm_alloc(size_t amt);
    void    pmm_reserve(frame_t base, size_t amt);
    void    pmm_free(frame_t base, size_t amt);

    size_t get_total_memory();
    size_t get_available_memory();
}