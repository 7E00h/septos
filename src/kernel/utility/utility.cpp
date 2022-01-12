#include "utility.hpp"

using namespace utility;

void utility::mem_set(uint8_t* src, size_t amt, uint8_t val)
{
    while (amt--) *src++ = val;
}

void utility::mem_zero(uint8_t* src, size_t amt)
{
    mem_set(src, amt, 0);
}

void utility::mem_copy(uint8_t* src, uint8_t* dst, size_t amt)
{
    for (size_t idx = 0; idx < amt; idx++)
        dst[idx] = src[idx];
}