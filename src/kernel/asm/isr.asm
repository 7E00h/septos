BITS 64

global isr_default_handler


isr_default_handler:
    cli

    mov rax, 0x12693469

    hlt
    iretq