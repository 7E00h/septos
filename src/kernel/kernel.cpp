#include <stdint.h>

#include <kernel/memory/physical.hpp>
#include <kernel/memory/kmalloc.hpp>
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
  // kernel::idt_init();
}

static void print_memory_map(kernel::mem_info_t* mem_info, uint64_t amt)
{
  size_t total = 0;

  kernel::printf("Base                | Length              | Type\n");
  kernel::printf("--------------------+---------------------+-----\n");

  for (int i = 0; i < amt; i++)
  {
    kernel::mem_info_t* cur = &mem_info[i];
    kernel::printf("%xl | %xl | %d\n", cur->base, cur->length, cur->type);
    if (cur->type == 1)
      total += cur->length;
  }

  total /= (1024 * 1024);

  kernel::printf("\nTotal Memory = %l MB\n", total);
}

extern "C"
void kmain(kernel::mem_info_t* mem_info, uint64_t amt)
{   
  init();

  print_memory_map(mem_info, amt);
  
  while (1);
}

