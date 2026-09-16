#include "layout/EC_Thread.hpp"
#include "types/System.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/SystemImports.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_Thread {
    const pas::Array<std::int32_t, 0, 6> ThreadPriorityValues = pas::Array<std::int32_t, 0, 6>{{-15, -2, -1, 0, 1, 2, 15}};

    std::int32_t ThreadEntryEC(void* Thread) {
        try {
            static_cast<TThreadEC*>(Thread)->ProcessRequests();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
            } else {
                throw;
            }
        }
        if (static_cast<TThreadEC*>(Thread)->ThreadHandle != 0) {
            WindowsImports::CloseHandle(static_cast<TThreadEC*>(Thread)->ThreadHandle);
            static_cast<TThreadEC*>(Thread)->ThreadHandle = 0u;
        }
        static_cast<TThreadEC*>(Thread)->ThreadId = 0u;
        return 0;
    }

    void TThreadEC_Create(TThreadEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Lock = pas::make_critical_section<pas::CriticalSection>();
        Self->StopEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
        if (Self->StopEvent == 0) {
            pas::raise(pas::make_exception<pas::Exception>("TThreadEC.Create CreateEvent"_a));
        }
        Self->ShutdownEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
        if (Self->ShutdownEvent == 0) {
            pas::raise(pas::make_exception<pas::Exception>("TThreadEC.Create CreateEvent"_a));
        }
        Self->StartEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
        if (Self->StartEvent == 0) {
            pas::raise(pas::make_exception<pas::Exception>("TThreadEC.Create CreateEvent"_a));
        }
        Self->RunningEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
        if (Self->RunningEvent == 0) {
            pas::raise(pas::make_exception<pas::Exception>("TThreadEC.Create CreateEvent"_a));
        }
        Self->IdleEvent = WindowsImports::CreateEvent(nullptr, -1, -1, nullptr);
        if (Self->IdleEvent == 0) {
            pas::raise(pas::make_exception<pas::Exception>("TThreadEC.Create CreateEvent"_a));
        }
        Self->ThreadHandle = SystemImports::BeginThread(nullptr, 0u, System::TThreadFunc(ThreadEntryEC), Self, WindowsSdk::CREATE_SUSPENDED, Self->ThreadId);
        WindowsSdk::SetThreadPriority(Self->ThreadHandle, WindowsSdk::THREAD_PRIORITY_NORMAL);
        WindowsSdk::ResumeThread(Self->ThreadHandle);
    }

    void TThreadEC_Destroy(TThreadEC* Self) {
        if (Self->ShutdownEvent != 0) {
            WindowsSdk::SetEvent(Self->ShutdownEvent);
            WindowsSdk::WaitForSingleObject(Self->ThreadHandle, WindowsSdk::INFINITE);
        }
        if (Self->IdleEvent != 0) {
            WindowsImports::CloseHandle(Self->IdleEvent);
            Self->IdleEvent = 0u;
        }
        if (Self->StartEvent != 0) {
            WindowsImports::CloseHandle(Self->StartEvent);
            Self->StartEvent = 0u;
        }
        if (Self->RunningEvent != 0) {
            WindowsImports::CloseHandle(Self->RunningEvent);
            Self->RunningEvent = 0u;
        }
        if (Self->ShutdownEvent != 0) {
            WindowsImports::CloseHandle(Self->ShutdownEvent);
            Self->ShutdownEvent = 0u;
        }
        if (Self->StopEvent != 0) {
            WindowsImports::CloseHandle(Self->StopEvent);
            Self->StopEvent = 0u;
        }
        pas::free(Self->Lock);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TThreadEC::ProcessRequests() {
        pas::Array<WindowsImports::THandle, 0, 1> Events{};
        std::uint32_t WaitResult{};
        Events[0] = ShutdownEvent;
        Events[1] = StartEvent;
        while (true) {
            WaitResult = WindowsSdk::WaitForMultipleObjects(2u, reinterpret_cast<WindowsSdk::PWOHandleArray>(&Events), 0, WindowsSdk::INFINITE);
            if (WaitResult != WindowsSdk::WAIT_OBJECT_0 + 1) {
                break;
            }
            pas::critical_enter(Lock);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    pas::critical_leave(Lock);
                };
                if (!IsRunning()) {
                    WindowsSdk::ResetEvent(StopEvent);
                    StopRequested = false;
                    WindowsSdk::ResetEvent(IdleEvent);
                    WindowsSdk::SetEvent(RunningEvent);
                }
            }
            try {
                this->virtual_TThreadEC_Execute();
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(E->message);
                    GR_Main::AppendLogLineThreadSafe("Thread exception"_a);
                    throw;
                } else {
                    throw;
                }
            }
            pas::critical_enter(Lock);
            {
                pas::ScopeExit cpp_cleanup_2 = [&]() noexcept {
                    pas::critical_leave(Lock);
                };
                WindowsSdk::ResetEvent(RunningEvent);
                WindowsSdk::SetEvent(IdleEvent);
            }
        }
    }

    void TThreadEC_Execute(TThreadEC* Self) {
        while (!Self->IsStopRequested()) {
            SysUtilsImports::Sleep(100u);
        }
    }

    void TThreadEC::SetPriority(std::uint8_t Value) {
        Priority = Value;
        if (ThreadHandle != 0) {
            WindowsSdk::SetThreadPriority(ThreadHandle, ThreadPriorityValues[Value]);
        }
    }

    void TThreadEC::SetFlag18() {
        Flag18 = true;
    }

    void TThreadEC::ClearFlag18() {
        Flag18 = false;
    }

    void TThreadEC::RequestStop() {
        pas::critical_enter(Lock);
        StopRequested = true;
        WindowsSdk::SetEvent(StopEvent);
        pas::critical_leave(Lock);
    }

    std::uint8_t TThreadEC::IsStopRequested() {
        pas::critical_enter(Lock);
        std::uint8_t Result = StopRequested;
        pas::critical_leave(Lock);
        return Result;
    }

    void TThreadEC::SetStopRequested(std::uint8_t Value) {
        if (Value) {
            RequestStop();
        } else {
            pas::critical_enter(Lock);
            StopRequested = false;
            WindowsSdk::ResetEvent(StopEvent);
            pas::critical_leave(Lock);
        }
    }

    // Schedules Execute on the existing OS thread; does nothing while a run is pending or active.
    void TThreadEC::Start() {
        pas::critical_enter(Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Lock);
            };
            if (IsRunning()) {
                return;
            }
            WindowsSdk::ResetEvent(StopEvent);
            StopRequested = false;
            WindowsSdk::ResetEvent(IdleEvent);
            WindowsSdk::SetEvent(RunningEvent);
            WindowsSdk::SetEvent(StartEvent);
        }
    }

    std::uint8_t TThreadEC::IsRunning() {
        pas::critical_enter(Lock);
        std::uint8_t Result = WindowsSdk::WaitForSingleObject(IdleEvent, 0u) == WindowsSdk::WAIT_TIMEOUT;
        pas::critical_leave(Lock);
        return Result;
    }

    // False only on timeout; a wait failure also returns true.
    std::uint8_t TThreadEC::WaitForIdle(std::uint32_t TimeoutMs) {
        return !(WindowsSdk::WaitForSingleObject(IdleEvent, TimeoutMs) == WindowsSdk::WAIT_TIMEOUT);
    }

    void TThreadEC::p_destroy() {
        EC_Thread::TThreadEC_Destroy(this);
    }

    void TThreadEC::virtual_TThreadEC_Execute() {
        EC_Thread::TThreadEC_Execute(this);
    }

} // namespace EC_Thread
