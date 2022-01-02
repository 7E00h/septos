#include <stdint.h>

#include <kernel/memory/physical.hpp>
#include <kernel/memory/virtual.hpp>
#include <kernel/memory/kmalloc.hpp>
#include <kernel/drivers/nvme.hpp>
#include <kernel/drivers/pic.hpp>
#include <kernel/pci/pci.hpp>
#include <kernel/asm/asm.hpp>
#include <kernel/idt.hpp>
#include <kernel/vga.hpp>

static void init()
{
  kernel::clear_screen();

  kernel::vmm_init();

  // Initialize frame manager
  kernel::frame_init();

  // First two 2 MB frames are reserved for kernel
  kernel::frame_alloc();
  kernel::frame_alloc();

  // Allocate a 2 MB frame for the heap
  void* heap = (void*) kernel::frame_alloc();
  kernel::heap_init(heap, 1 << 21);

  // Initialize IDT and PIC
  //kernel::idt_init();
  //kernel::pic_init();
  //_asm_sti();

  // PCI
  //kernel::pci_init();
  //kernel::pci_endpoint* dev = kernel::pci_device_array();
}

extern "C"
void kmain(kernel::mem_info_t* mem_info, size_t amt)
{   
  init();

  /*
  kernel::parse_mem_info(mem_info, amt);

  for (int i = 0; i < 100; i++)
    kernel::alloc_pagetable();
  */

  kernel::printf("HERE");
  
  while (1);
}