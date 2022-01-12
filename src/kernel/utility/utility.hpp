#pragma once

#include <stdint.h>
#include <stddef.h>

namespace utility
{
    using uint = unsigned int;

    void mem_set(uint8_t* src, size_t amt, uint8_t val);
    void mem_zero(uint8_t* src, size_t amt);
    void mem_copy(uint8_t* src, uint8_t* dst, size_t amt);

    template <typename T>
    T uint_divceil(T a, T b)
    {
        return a / b + (a % b != 0);
    }
}