#pragma once

#include <kernel/int.hpp>

#include <kernel/memory/virtual.hpp>
#include <kernel/x64/idt.hpp>

namespace kernel
{
    using pid_t = u64;

    struct regs_t
    {
        u64 rax;
        u64 rbx;
        u64 rcx;
        u64 rdx;
        u64 rsi;
        u64 rdi;
        u64 rbp;

        u64 r8;
        u64 r9;
        u64 r10;
        u64 r11;
        u64 r12;
        u64 r13;
        u64 r14;
        u64 r15;
    } __attribute__((packed));

    struct cpu_state_t
    {
        regs_t      regs;
        int_frame_t int_frame;
    } __attribute__((packed));

    struct thread
    {
        cpu_state_t* cpu_state;
    } __attribute__((packed));

    struct process_t
    {
        enum state_e
        {
            STATE_WAITING,
            STATE_RUNNING,
            STATE_BLOCKED,
            STATE_STOPPED
        };

        vaddr_t entry;
        pid_t   pid;
        state_e state;
        page_t* vms;
        thread  main_thread;
    };

    void create_process_from_image(u8* image, process_t* out);
}