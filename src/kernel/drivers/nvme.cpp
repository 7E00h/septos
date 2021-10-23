#include "nvme.hpp"

#include <kernel/vga.hpp>

using namespace kernel;

nvme_block_dev::nvme_block_dev(nvme_regs* regs)
    : m_regs(regs)
{
    kernel::printf("vs = %d\n", regs->vs);
    kernel::printf("awq = %l\n", regs->asq);
}

void nvme_block_dev::read_blocks(size_t lba, size_t amt, char* dst)
{

}

void nvme_block_dev::write_blocks(size_t lba, size_t amt, char* dst)
{

}

// For some reason, this function must exist or else
// pure virtual functions won't work.
extern "C" void __cxa_pure_virtual() {}