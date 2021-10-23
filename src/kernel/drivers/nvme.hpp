#pragma once

#include <stdint.h>
#include <stddef.h>

#include "block_dev.hpp"

namespace kernel
{
    // See NVM Express Base Specification 2.0 (3.1.3, Controller Properties)
    struct nvme_regs
    {
        uint64_t cap;     // Controller Capabilities
        uint32_t vs;      // Version 
        uint32_t intms;   // Interrupt Mask Set
        uint32_t intmc;   // Interrupt Mask Clear
        uint32_t cc;      // Controller Configuration
        uint32_t rsv1[3]; // Reserved
        uint32_t csts;    // Controller Status
        uint32_t nssr;    // NVM Subsystem Reset
        uint32_t aqa;     // Admin Queue Attributes
        uint64_t asq;     // Admin Submission Queue Base Address
        uint64_t acq;     // Admin Completion Queue Base Address
        
        // Note: this structure is not complete; it only contains fields
        // relevant to the operation of this driver.
    } __attribute__((packed));

    class nvme_block_dev
        : public block_dev
    {
    public:
        nvme_block_dev(nvme_regs* regs);

        void read_blocks(size_t lba, size_t amt, char* dst);
        void write_blocks(size_t lba, size_t amt, char* src);

    private:
        nvme_regs* m_regs;
    };
}