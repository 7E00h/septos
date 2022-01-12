ENTRY(pmain)

SECTIONS
{
    . = 0xDEADBEEF;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
}