#pragma once

#include <stddef.h>

namespace kernel
{
    void   heap_init(void* begin, size_t limit);
    void*  malloc(size_t);
    void   free(void*);
    size_t get_allocated_memory();
}