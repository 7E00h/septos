#include "process.hpp"

#include <kernel/memory/virtual.hpp>
#include <kernel/process/elf/elf.hpp>
#include <kernel/vga.hpp>

using namespace kernel;

#define _STACK_START (10 * (1 << 21)) // 20 MB

static pid_t pid_counter = 0;

void _load_elf_segments(uintptr_t base, elf_program_hdr* hdr, uint16_t amt)
{
    for (uint16_t idx = 0; idx < amt; idx++)
    {
        elf_program_hdr* cur = &hdr[idx];
        if (cur->p_type != PT_LOAD)
            continue;

        uintptr_t offset = base + cur->p_offset; // Offset in file
        vaddr_t   dest   = cur->p_vaddr;         // Virtual address
        size_t    size   = cur->p_filesz;        // Size of segment

        kernel::map(dest, size);
        utility::mem_copy((uint8_t*) offset, (uint8_t*) dest, size);
    }
}

void kernel::create_process_from_image(uint8_t* image, process_t* out)
{
    elf_img  img;
    elf_hdr* hdr = (elf_hdr*) image;

    if (!kernel::parse_elf_hdr(hdr, &img))
    {
        kernel::printf("Could not parse ELF image at %xl\n", hdr);
        return;
    }

    // Configure the process
    out->entry = hdr->e_entry;
    out->pid   = pid_counter++;
    out->state = process_t::STATE_WAITING;
    out->vms   = (page_t*) kernel::create_address_space();

    page_t* old = kernel::read_address_space();

    // Switch to the new address space to configure process
    kernel::load_address_space(out->vms);
    _load_elf_segments((uintptr_t) image, img.program_hdr, hdr->e_phnum);

    // Set up stack
    kernel::map(_STACK_START, 4096);

    out->main_thread.cpu_state = (cpu_state_t*) _STACK_START;
    // Also, initialize the interrupt stack frame for
    // when the process begins to execute
    out->main_thread.cpu_state->int_frame.rsp    = _STACK_START + 4080;
    out->main_thread.cpu_state->int_frame.rip    = hdr->e_entry;
    out->main_thread.cpu_state->int_frame.rflags = (1 << 9);
    out->main_thread.cpu_state->int_frame.ss     = 0x10;
    out->main_thread.cpu_state->int_frame.cs     = 0x08;
    utility::mem_zero((uint8_t*) &out->main_thread.cpu_state->regs, sizeof(regs_t));

    // Return to previous address space
    kernel::load_address_space(old);
}