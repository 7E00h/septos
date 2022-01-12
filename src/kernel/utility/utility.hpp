#pragma once

#include <kernel/int.hpp>

namespace utility
{
    void memset(u8* src, size_t amt, u8 val);
    void memzro(u8* src, size_t amt);
    void memcpy(u8* src, u8* dst, size_t amt);

    template <typename T>
    T divceil(T a, T b)
    {
        return a / b + (a % b != 0);
    }
}