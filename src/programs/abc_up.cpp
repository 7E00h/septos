#include <stdint.h>

uint8_t* char_buffer = (uint8_t*) 0xB8000;
// (Character, Color)

extern "C"
void pmain()
{
    char c = 'A';

    char_buffer[3] = 0xF0;

    while (1)
    {
        char_buffer[2] = c;

        c++;
        if (c > 'Z')
            c = 'A';

        uint64_t counter = (1 << 27);
        while (counter--);
    }
}