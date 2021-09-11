#include "physical.hpp"

#include <kernel/vga.hpp>

using namespace kernel;

#define BITMAP_SIZE 4096 / 8

static uint64_t bitmap[BITMAP_SIZE];
static int cached_idx = -1, cached_bit = -1;

#define CALC_ADDR(idx, bit) (((idx << 6) + bit) << 21)
#define SET_BIT(idx, bit) (bitmap[idx] |= (1 << bit))
#define CLR_BIT(idx, bit) (bitmap[idx] &= ~(1 << bit))

void kernel::frame_init()
{
    for (int idx = 0; idx < BITMAP_SIZE; idx++)
        bitmap[idx] = 0;
}

frame_t kernel::frame_alloc()
{
    if (cached_idx != -1)
    {
        SET_BIT(cached_idx, cached_bit);
        frame_t addr = CALC_ADDR(cached_idx, cached_bit);
        cached_idx = -1;
        cached_bit = -1;
        return addr;
    }

    // Linear search for free frame
    int      idx  =  0;
    uint64_t full = ~0;
    for (; idx < BITMAP_SIZE && bitmap[idx] == full; idx++);

    if (idx == BITMAP_SIZE)
        return full;

    uint64_t qword = bitmap[idx];
    int      bit   = __builtin_ffs(~qword) - 1;

    SET_BIT(idx, bit);

    return CALC_ADDR(idx, bit);
}

void kernel::frame_free(frame_t frame)
{
    int idx = frame / (1 << 27);
    int bit = frame / (1 << 21);

    CLR_BIT(idx, bit);

    cached_idx = idx;
    cached_bit = bit;
}