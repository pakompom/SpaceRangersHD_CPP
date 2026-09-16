#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"

namespace ThreadCalc {
    struct TThreadCalc;

    enum TTurnCalculationJob : std::uint32_t {
        tcjGalaxy = 1,
        tcjPlayerStar = 2,
        tcjPreparePlayerStar = 3,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TThreadCalc : EC_Thread::TThreadEC {
        PAS_CLASS_META(TThreadCalc, EC_Thread::TThreadEC, "TThreadCalc", 48)
        void virtual_TThreadEC_Execute() override;
        TTurnCalculationJob Job;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TTurnCalculationPhase : std::int32_t {
        tcpIdle = 0,
        tcpGalaxyRunning = 1,
        tcpGalaxyFinished = 2,
        tcpPlayerStarRunning = 3,
        tcpPlayerStarFinished = 4,
        tcpPlayerStarPreparationRunning = 5,
        tcpPlayerStarPrepared = 6,
        tcpNotStarted = -1,
    };

} // namespace ThreadCalc
