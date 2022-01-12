#include <stdint.h>

uint8_t* char_buffer = (uint8_t*) 0xB8000;
// (Character, Color)

extern "C"
void pmain()
{
    char c = 'Z';

    char_buffer[1] = 0xF0;

    while (1)
    {
        char_buffer[0] = c;

        c--;
        if (c < 'A')
            c = 'Z';

        uint64_t counter = (1 << 27);
        while (counter--);
    }
}