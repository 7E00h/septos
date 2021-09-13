#pragma once

#include <stdint.h>

namespace kernel
{
    struct mem_info_t
    {
        uint64_t base;
        uint64_t length;
        uint32_t type;
        uint32_t acpi_extended;
    } __attribute__((packed));

    using frame_t = uint64_t;

    void    frame_init();
    frame_t frame_alloc();
    void    frame_free(frame_t);
}