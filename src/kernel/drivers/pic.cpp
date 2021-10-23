// http://www.brokenthorn.com/Resources/OSDevPic.html
// Fantastic resource explaining the 8259 PIC

#include "pic.hpp"

#include <kernel/asm/asm.hpp>

#define PIC_MASTER      0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE       0xA0
#define PIC_SLAVE_DATA  0xA1

// ICW = initialization control words
#define ICW1        0x11
#define ICW2_MASTER 0x21
#define ICW2_SLAVE  0x27
#define ICW3_MASTER 0x04
#define ICW3_SLAVE  0x02
#define ICW4        0x01

#define PIC_EOI 0x20

void kernel::pic_init()
{
    // Save interrupt masks
    uint8_t imr1 = _asm_in_8(PIC_MASTER_DATA);
    uint8_t imr2 = _asm_in_8(PIC_SLAVE_DATA);

    // TODO: add io_wait() for real hardware (just to be safe)

    // Enable cascading mode
    _asm_out_8(PIC_MASTER, ICW1);
    _asm_out_8(PIC_SLAVE,  ICW1);

    // Set IRQ offsets as to not interfere with 0x00 - 0x1F (reserved)
    _asm_out_8(PIC_MASTER_DATA, ICW2_MASTER);
    _asm_out_8(PIC_SLAVE_DATA,  ICW2_SLAVE);

    // Inform PICs which IRQ lines are used to communicate with each other
    _asm_out_8(PIC_MASTER_DATA, ICW3_MASTER);
    _asm_out_8(PIC_SLAVE_DATA,  ICW3_SLAVE);

    // Last step, just enable 8086 mode
    _asm_out_8(PIC_MASTER_DATA, ICW4);
    _asm_out_8(PIC_SLAVE_DATA,  ICW4);

    // Restore masks
    _asm_out_8(PIC_MASTER_DATA, imr1);
    _asm_out_8(PIC_SLAVE_DATA,  imr2);
}

void kernel::pic_enable(uint8_t irq)
{
    uint16_t port = PIC_MASTER_DATA;
    if (irq >= 8)
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    // Negation required since 0 = enabled, 1 = disabled
    uint8_t mask = ~_asm_in_8(port);
    mask |= (1 << irq);
    _asm_out_8(port, ~mask);
}

void kernel::pic_disable(uint8_t irq)
{
    uint16_t port = PIC_MASTER_DATA;
    if (irq >= 8)
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    } 

    uint8_t mask = ~_asm_in_8(port);
    mask &= ~(1 << irq);
    _asm_out_8(port, ~mask);
}

void kernel::pic_eoi(uint8_t irq)
{
    if (irq >= 8)
        _asm_out_8(PIC_SLAVE, PIC_EOI);

    _asm_out_8(PIC_MASTER, PIC_EOI);
}

