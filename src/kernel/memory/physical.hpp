#pragma once

#include <stdint.h>

namespace kernel
{
    using frame_t = uint64_t;

    void    frame_init();
    frame_t frame_alloc();
    void    frame_free(frame_t);
}