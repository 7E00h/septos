#pragma once

extern "C"
{
    // IDT stuff
    void _asm_cli();
    void _asm_sti();
    void _asm_lidt(uint64_t pointer);

    // IO
    void _asm_out_8(uint16_t addr, uint8_t  data);
    void _asm_out_16(uint16_t addr, uint16_t data);
    void _asm_out_32(uint16_t addr, uint32_t data);

    uint8_t  _asm_in_8(uint16_t addr);
    uint16_t _asm_in_16(uint16_t addr);
    uint32_t _asm_in_32(uint16_t addr);
}