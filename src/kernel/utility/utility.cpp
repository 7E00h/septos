#include "utility.hpp"

using namespace utility;

void utility::memset(uint8_t* src, size_t amt, uint8_t val)
{
    while (amt--) *src++ = val;
}

void utility::memzro(uint8_t* src, size_t amt)
{
    memset(src, amt, 0);
}

void utility::memcpy(uint8_t* src, uint8_t* dst, size_t amt)
{
    for (size_t idx = 0; idx < amt; idx++)
        dst[idx] = src[idx];
}