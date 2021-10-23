#include <stdint.h>

#include <kernel/memory/physical.hpp>
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

  // Initialize frame manager
  kernel::frame_init();

  // First two 2 MB frames are reserved for kernel
  kernel::frame_alloc();
  kernel::frame_alloc();

  // Allocate a 2 MB frame for the heap
  void* heap = (void*) kernel::frame_alloc();
  kernel::heap_init(heap, 1 << 21);

  // Initialize IDT and PIC
  kernel::idt_init();
  kernel::pic_init();
  _asm_sti();

  // PCI
  kernel::pci_init();
  kernel::pci_endpoint* dev = kernel::pci_device_array();

  while (dev->vendor_id != 0xFFFF)
  {
    if (dev->cls == 0x01 && dev->subcls == 0x08)
    {
      kernel::printf("NVMe device found (%d, %d, %d)\n", dev->bus, dev->device, 0);
      uint32_t bar0 = dev->get_data_32(kernel::PCI::HDR_BAR0);
      // bar0 &= ~(0x0F);
      kernel::nvme_regs* nvme_regs = (kernel::nvme_regs*) bar0;
      kernel::printf("BAR0: %x\n", bar0);
      // kernel::nvme_block_dev dev(nvme_regs);
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

