#pragma once

#include <kernel/memory/virtual.hpp>
#include <kernel/idt.hpp>

namespace kernel
{
    using pid_t = uint64_t;

    struct regs_t
    {
        uint64_t rax;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t rdx;
        uint64_t rsi;
        uint64_t rdi;
        uint64_t rbp;

        uint64_t r8;
        uint64_t r9;
        uint64_t r10;
        uint64_t r11;
        uint64_t r12;
        uint64_t r13;
        uint64_t r14;
        uint64_t r15;
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

        uint8_t* text_buffer;
    };

    void create_process_from_image(uint8_t* image, process_t* out);
}