#pragma once

#include <stdint.h>

#define CHANNEL0 0x40
#define COMMAND  0x43

namespace kernel
{
    void init_pit(uint32_t hz);}