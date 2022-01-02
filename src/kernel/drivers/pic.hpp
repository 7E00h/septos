#pragma once

#include <stdint.h>

namespace kernel
{
    void pic_init();

    void pic_enable(uint8_t irq);
    void pic_disable(uint8_t irq);
    void pic_eoi(uint8_t irq);
}