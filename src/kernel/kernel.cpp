#include <stdint.h>

#include <kernel/process/scheduler.hpp>
#include <kernel/memory/physical.hpp>
#include <kernel/memory/virtual.hpp>
#include <kernel/memory/kmalloc.hpp>
#include <kernel/drivers/nvme.hpp>
#include <kernel/drivers/pic.hpp>
#include <kernel/pci/pci.hpp>
#include <kernel/asm/asm.hpp>
#include <kernel/idt.hpp>
#include <kernel/vga.hpp>

#include <kernel/process/process.hpp>

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

typedef void(*program)(void);

extern "C"
void kmain(kernel::mem_info_t* mem_info, size_t amt)
{   
  _init(mem_info, amt);
  kernel::printf("Initialization complete\n");

  // Create kernel process
  kernel::process_t kernel_proc;
  kernel_proc.pid = 69;
  kernel_proc.vms = kernel::read_address_space();
  kernel::schedule_process(&kernel_proc);

  // Create and schedule process
  kernel::process_t abc_up;
  kernel::create_process_from_image((uint8_t*) abc_up_elf, &abc_up);
  kernel::schedule_process(&abc_up);

  kernel::process_t abc_down;
  kernel::create_process_from_image((uint8_t*) abc_down_elf, &abc_down);
  kernel::schedule_process(&abc_down);

  kernel::printf("Running...\n");

  // Enable timer
  _asm_sti();

  while (1);
}