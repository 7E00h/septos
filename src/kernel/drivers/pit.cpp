#include "pit.hpp"

#include <kernel/x64/cpu.hpp>

using namespace kernel;

#define BASEFREQ 1193182

void kernel::init_pit(uint32_t hz)
{
    _asm_cli();

    uint32_t divisor = BASEFREQ / hz;
    uint8_t low      = (divisor & 0x000000FF);
    uint8_t high     = ((divisor & 0x0000FF00) >> 8);

    _asm_out_8(COMMAND, 0b00110100); // channel 0, low/high, rate generator (3rd mode)
    _asm_out_8(CHANNEL0, low);
    _asm_out_8(CHANNEL0, high);
}