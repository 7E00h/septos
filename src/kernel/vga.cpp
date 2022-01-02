#include "vga.hpp"

#include <stdarg.h>
#include <stdint.h>

static void write_char(char c);
static void write_str(char* str);
static void write_int(int32_t num);
static void write_long(long num);
static void color_screen(uint8_t color);
static void advance();

template <typename T>
static void write_hex(T num);

template <typename T>
void write_uint(T num);

uint16_t* VGA = (uint16_t*) 0xB8000;

uint8_t row = 0;
uint8_t col = 0;

void kernel::printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    short short_arg;
    char char_arg;

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
                    write_uint(va_arg(args, int));
                    break;

                case 'l':
                    write_uint(va_arg(args, long));
                    break;

                case 'x':
                    if (*(fmt + 1) == 'l')
                    {
                        fmt++;
                        write_hex(va_arg(args, long));
                    }
                    else
                        write_hex(va_arg(args, int));
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
    color_screen(0x00);
}

template <typename T>
void write_hex(T num)
{
    static char HEX[]      = "0123456789ABCDEF";
    static char buffer[20] = {0};

    int len = sizeof(T) * 2;
    int idx = 18;

    while (len > 0)
    {
        uint8_t nibble = num & 0b1111;
        buffer[idx--]  = HEX[nibble];
        
        len--;
        num >>= 4;
    }

    buffer[idx--] = 'x';
    buffer[idx]   = '0';

    write_str(buffer + idx);
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

template <typename T>
void write_uint(T num)
{
    if (!num)
    {
        write_char('0');
        return;
    }

    char buffer[32];
    buffer[31] = '\0';
    int idx = 30;
    for (; idx >= 0 && num > 0; idx--, num /= 10)
        buffer[idx] = (num % 10) + '0';

    write_str(buffer + idx + 1);
}

void color_screen(uint8_t color)
{
    uint16_t _color = color << 8;
    for (uint16_t i = 0; i < 80 * 25; i++)
        VGA[i] = _color;
}

void advance()
{
    col = (col + 1) % 80;
    row = (col == 0) ? row + 1 : row;
}