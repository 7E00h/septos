#include "idt.hpp"

#include <kernel/asm/asm.hpp>
#include <kernel/vga.hpp>

struct idt_entry_t
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist       : 3;
    uint8_t  reserved1 : 5;

    uint8_t  type      : 4;
    uint8_t  zero1     : 1;
    uint8_t  dpl       : 2;
    uint8_t  p         : 1;

    uint16_t offset_middle;
    uint32_t offset_high;

    uint32_t reserved2;
} __attribute__((packed));

idt_entry_t IDT[256];

struct idtr_t
{
    uint16_t limit;
    uint64_t addr;
} __attribute__((packed));

extern "C"
extern void isr_default_handler(kernel::int_frame_t*);

void kernel::idt_init()
{
    _asm_cli();

    // Zero table
    char* byte = (char*) IDT;
    for (int idx = 0; idx < 256 * 16; idx++)
        *byte++ = 0;

    // Initialize table entries with default handler
    for (int idx = 0; idx < 256; idx++)
        kernel::idt_install_gate(isr_default_handler, idx);

    // Load IDTR
    idtr_t ptr = {
        256 * 16,
        (uint64_t) IDT
    };

    _asm_lidt((uint64_t) &ptr);
}

void kernel::idt_install_gate(kernel::isr_t handler, int index)
{
    uint64_t addr = (uint64_t) *handler;

    IDT[index].offset_low    = addr & 0xFFFF;
    IDT[index].offset_middle = (addr & 0xFFFF0000) >> 16;
    IDT[index].offset_high   = (addr & 0xFFFFFFFF00000000) >> 32;

    IDT[index].dpl  = 0;
    IDT[index].p    = 1;
    IDT[index].type = 0xF;
    IDT[index].selector = 0x08;
}