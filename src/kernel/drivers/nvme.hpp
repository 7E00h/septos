#pragma once

#include <kernel/drivers/block_dev.hpp>

namespace kernel
{
    class nvme_block_dev
        : public block_dev
    {
    public:
        virtual void read_blocks(size_t initial_block, size_t amt, char* dst) override;
        virtual void write_blocks(size_t initial_block, size_t amt, char* src) override;
    };
}