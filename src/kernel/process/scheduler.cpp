#include "scheduler.hpp"

#include <kernel/memory/virtual.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/utility/utility.hpp>
#include <kernel/drivers/pic.hpp>
#include <kernel/vga.hpp>

using namespace kernel;

process_t* kernel::running_process = nullptr;

// Defined in isr_timer.asm
// The interrupt handler which calls
// scheduler_context_switch(cpu_state_t*)
extern "C"
{
    extern void _asm_isr_timer(kernel::int_frame_t*);
    extern void _asm_isr_timer_continue(page_t* vms, uint64_t new_rsp);
}

// TODO: Obviously replace this hardcoded value
// to utilize the kernel heap instead.
#define CAPACITY 64

#define MODINC(v) (v = (v + 1) % CAPACITY)

static process_t* _queue[CAPACITY];
static uint16_t _front = 0, _back = 0, _amount = 0;

static process_t* _peek()
{
    return _queue[_front];
}

// _queue[0] = elf
// _queue[1] = kernel

static void _enqueue(process_t* p)
{
    if (_amount == CAPACITY)
        return;

    _queue[_back] = p;
    MODINC(_back);
    _amount++;
}

static process_t* _dequeue()
{
    if (_amount == 0)
        return nullptr;

    process_t* ret = _queue[_front];
    MODINC(_front);
    _amount--;

    return ret;
}

// This function should only be called from
// the interrupt handler.
extern "C"
void scheduler_context_switch(cpu_state_t* cpu_state)
{
    if (running_process == nullptr)
        [[unlikely]]
    {
        kernel::printf("No process\n");
        __asm__("hlt");
    }

    process_t* c = running_process;

    c->state = process_t::STATE_WAITING;
    c->main_thread.cpu_state = cpu_state;
    _enqueue(c);

    process_t* n = _dequeue();  

    running_process = n;

    kernel::pic_eoi(0);

    _asm_isr_timer_continue(n->vms, (uint64_t) n->main_thread.cpu_state);
}

void kernel::scheduler_init()
{
    const uint16_t TIME_QUANTUM = 11;
    kernel::idt_install_gate(_asm_isr_timer, 32);
    kernel::init_pit(TIME_QUANTUM);
}

void kernel::schedule_process(process_t* process)
{
    if (running_process == nullptr)
    {
        running_process = process;
        return;
    }

    _enqueue(process);
}

void kernel::kill_process(pid_t pid)
{
}