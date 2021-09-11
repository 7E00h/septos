#include "vga.hpp"

#include <stdarg.h>
#include <stdint.h>

static void write_char(char c);
static void write_str(char* str);
static void write_int(int32_t num);
static void color_scr(uint8_t color);
static void advance(void);

uint16_t* VGA = (uint16_t*) 0xB8000;

uint8_t row = 0;
uint8_t col = 0;

void kernel::kprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
                case 's':
                    write_str(va_arg(args, char*));
                    break;    

                case 'd':
                    write_int(va_arg(args, int));
                    break;

                default:
                    write_char('%');
                    fmt--;
                    break;
            }
        }

        else
            write_char(*fmt);

        fmt++;
    }

    va_end(args);
}

void kernel::clear_screen()
{
    color_scr(0x00);
}

void write_char(char c)
{
    if (c == '\n')
    {
        row++;
        col = 0;
        return;
    }

    VGA[row * 80 + col] = (0x0A << 8) | c;
    advance();
}

void write_str(char* str)
{
    while (*str)
        write_char(*str++);
}

void write_int(int num)
{
    if (!num)
    {
        write_char('0');
        return;
    }

    char buffer[16];
    buffer[15] = 0;
    int idx = 14;
    for (; idx >= 0 && num > 0; idx--, num /= 10)
        buffer[idx] = (num % 10) + '0';

    write_str(buffer + idx + 1);
}

void color_scr(uint8_t color)
{
    uint16_t _color = color << 8;
    for (uint16_t i = 0; i < 80 * 25; i++)
        VGA[i] = _color;
}

void advance(void)
{
    col = (col + 1) % 80;
    row = (col == 0) ? row + 1 : row;
}