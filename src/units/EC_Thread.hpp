#pragma once
#include "types/EC_Thread.hpp"

namespace EC_Thread {
    extern const pas::Array<std::int32_t, 0, 6> ThreadPriorityValues;

    std::int32_t ThreadEntryEC(void* Thread);

    void TThreadEC_Create(TThreadEC* Self);

    void TThreadEC_Destroy(TThreadEC* Self);

    void TThreadEC_Execute(TThreadEC* Self);

} // namespace EC_Thread
