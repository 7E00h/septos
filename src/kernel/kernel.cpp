#include <stdint.h>

#include <kernel/memory/physical.hpp>
#include <kernel/memory/kmalloc.hpp>
#include <kernel/pci/pci.hpp>
#include <kernel/idt.hpp>
#include <kernel/vga.hpp>

static void init()
{
  kernel::clear_screen();

  // Initialize frame manager
  kernel::frame_init();

  // First two 2 MB frames are reserved for kernel
  kernel::frame_alloc();
  kernel::frame_alloc();

  // Allocate a 2 MB frame for the heap
  void* heap = (void*) kernel::frame_alloc();
  kernel::heap_init(heap, 1 << 21);

  // Initialize IDT
  kernel::idt_init();

  // PCI
  kernel::pci_init();

  kernel::pci_endpoint* dev = kernel::pci_device_array();

  while (dev->vendor_id != 0xFFFF)
  {
    if (dev->cls == 0x01 && dev->subcls == 0x06)
    {
      kernel::printf("SATA device found (%d, %d, %d)\n", dev->bus, dev->device, 0);
      uint32_t bar5 = dev->get_data_32(kernel::PCI::HDR_BAR5);
      kernel::printf("- BAR5: %x\n", bar5);
    }

    dev++;
  }
}

extern "C"
void kmain(kernel::mem_info_t* mem_info, size_t amt)
{   
  init();
  kernel::parse_mem_info(mem_info, amt);
  
  while (1);
}

