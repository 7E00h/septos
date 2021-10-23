#pragma once

#include <stddef.h>
#include <stdint.h>

namespace kernel
{
    // Block device interface
    class block_dev
    {
    public:
        virtual void read_blocks(size_t lba, size_t amt, char* dst) = 0;
        virtual void write_blocks(size_t lba, size_t amt, char* src) = 0;
    };
}