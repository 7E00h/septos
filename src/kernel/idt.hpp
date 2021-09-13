#pragma once

#include <stdint.h>

namespace kernel
{
    struct int_frame_t
    {
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t ss;
    } __attribute__((packed));

    using isr_t = void (*)(int_frame_t*);

    void idt_init();
    void idt_install_gate(isr_t handler, int index);
}