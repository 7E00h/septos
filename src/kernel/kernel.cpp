#include <kernel/int.hpp>

#include <kernel/process/scheduler.hpp>
#include <kernel/memory/physical.hpp>
#include <kernel/memory/virtual.hpp>

#include <kernel/process/process.hpp>

#include <kernel/vga.hpp>
#include <kernel/drivers/pic.hpp>
#include <kernel/x64/cpu.hpp>

#include <programs/abc_up.inc>
#include <programs/abc_down.inc>

static void _init(kernel::mem_info_t* mem_info, size_t amt)
{
  kernel::clear_screen();

  kernel::pmm_init(mem_info, amt);
  kernel::vmm_init();

  kernel::idt_init();

  kernel::pic_init();
  kernel::pic_enable(0);

  kernel::scheduler_init();
}

extern "C"
void kmain(kernel::mem_info_t* mem_info, size_t amt)
{   
  _init(mem_info, amt);
  kernel::printf("Initialization complete\n");

  // Enable timer
  _asm_sti();

  while (1);
}