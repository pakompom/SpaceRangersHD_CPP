#pragma once
#include "types/EC_Thread.hpp"

namespace EC_Thread {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Thread::TThreadEC) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, Lock) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, ThreadHandle) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, ThreadId) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, Priority) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, StopRequested) == 17);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, StopEvent) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, Flag18) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, ShutdownEvent) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, StartEvent) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, RunningEvent) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Thread::TThreadEC, IdleEvent) == 40);

} // namespace EC_Thread
