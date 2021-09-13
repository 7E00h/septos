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
  kernel::idt_init();
}

extern "C"
void kmain()
{   
  init();

  int* p1 = (int*) kernel::malloc(1024);
  int* p2 = (int*) kernel::malloc(1);

  kernel::printf("p1 = %xl\n", p1);
  kernel::printf("p2 = %xl\n", p2);

  kernel::free(p1);
  p1 = (int*) kernel::malloc(512);

  kernel::printf("p1 = %xl\n", p1);

  kernel::printf("total allocated = %xl\n", kernel::get_allocated_memory());

  while (1);
}

