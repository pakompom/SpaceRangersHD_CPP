#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Thread {
    struct TThreadEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TThreadEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TThreadEC, EC_Struct::TObjectEx, "TThreadEC", 44)
        void p_destroy() override;
        void ProcessRequests();
        virtual void virtual_TThreadEC_Execute();
        void SetPriority(std::uint8_t Value);
        void SetFlag18();
        void ClearFlag18();
        void RequestStop();
        std::uint8_t IsStopRequested();
        void SetStopRequested(std::uint8_t Value);
        // Schedules Execute on the existing OS thread; does nothing while a run is pending or active.
        void Start();
        std::uint8_t IsRunning();
        // False only on timeout; a wait failure also returns true.
        std::uint8_t WaitForIdle(std::uint32_t TimeoutMs);
        pas::CriticalSection* Lock;
        std::uint32_t ThreadHandle;
        std::uint32_t ThreadId;
        std::uint8_t Priority;
        std::uint8_t StopRequested;
        std::uint8_t cpp_padding[2];
        std::uint32_t StopEvent;
        // Set/cleared by two helpers; its purpose is unresolved.
        std::uint8_t Flag18;
        std::uint8_t cpp_padding_2[3];
        std::uint32_t ShutdownEvent;
        std::uint32_t StartEvent;
        std::uint32_t RunningEvent;
        std::uint32_t IdleEvent;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Indices into ThreadPriorityValues, not Win32 priority values.
    inline constexpr std::int32_t ThreadPriorityLowest = 1;

    inline constexpr std::int32_t ThreadPriorityAboveNormal = 4;

} // namespace EC_Thread
