#pragma once

namespace kernel
{
    // Block device interface
    class block_dev
    {
    public:
        virtual void read_blocks(size_t initial_block, size_t amt, char* dst) = 0;
        virtual void write_blocks(size_t initial_block, size_t amt, char* src) = 0;
    };
}