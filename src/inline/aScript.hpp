#pragma once
#include "units/EC_Thread.hpp"
#include "units/aScript.hpp"

namespace aScript {
    inline void TScriptThread_Create(TScriptThread* Self) {
        EC_Thread::TThreadEC_Create(Self);
    }

} // namespace aScript
