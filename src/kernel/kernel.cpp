#include <stdint.h>

#include <kernel/memory/physical.hpp>
#include <kernel/vga.hpp>

void kprintf(const char* fmt, ...);

extern "C"
void kmain()
{
    kernel::frame_init();
    kernel::clear_screen();

    

    while (1);
}

