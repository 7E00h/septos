#pragma once

#include "process.hpp"

namespace kernel
{
    extern process_t* running_process;

    void scheduler_init();
    void schedule_process(process_t* process);
    void kill_process(pid_t pid);
}