#include "layout/Registry.hpp"
#include "units/Registry.hpp"
#include "units/WindowsSdk.hpp"

namespace Registry {
    std::uint8_t IsRelative(const pas::AnsiString& Value) {
        return !(Value != "" && Value.read(1) == '\\');
    }

    void TRegistry_Create(TRegistry* Self) {
        Self->SetRootKey(WindowsSdk::HKEY_CURRENT_USER);
        Self->FAccess = WindowsSdk::KEY_ALL_ACCESS;
        Self->FLazyWrite = true;
    }

    void TRegistry_Destroy(TRegistry* Self) {
        Self->CloseKey();
        pas::object_destroy(Self);
    }

    void TRegistry::CloseKey() {
        if (FCurrentKey != 0) {
            if (!FLazyWrite) {
                WindowsSdk::RegFlushKey(FCurrentKey);
            }
            WindowsSdk::RegCloseKey(FCurrentKey);
            FCurrentKey = 0u;
            FCurrentPath = pas::AnsiString();
        }
    }

    void TRegistry::SetRootKey(WindowsSdk::HKEY Value) {
        if (FRootKey != Value) {
            if (FCloseRootKey) {
                WindowsSdk::RegCloseKey(FRootKey);
                FCloseRootKey = false;
            }
            FRootKey = Value;
            CloseKey();
        }
    }

    WindowsSdk::HKEY TRegistry::GetBaseKey(std::uint8_t Relative) {
        if (FCurrentKey == 0 || static_cast<std::uint8_t>(Relative ^ 1)) {
            return FRootKey;
        }
        return FCurrentKey;
    }

    WindowsSdk::HKEY TRegistry::GetKey(const pas::AnsiString& Key) {
        WindowsSdk::HKEY Result{};
        pas::AnsiString S{};
        S = Key;
        std::uint8_t Relative = Registry::IsRelative(S);
        if (!Relative) {
            S = pas::copy(S, 2, S.length());
        }
        Result = 0u;
        WindowsSdk::RegOpenKeyExA(GetBaseKey(Relative), S.pchar(), 0u, FAccess, Result);
        return Result;
    }

    std::uint8_t TRegistry::KeyExists(const pas::AnsiString& Key) {
        std::uint8_t Result{};
        WindowsSdk::HKEY TempKey{};
        std::uint32_t OldAccess = FAccess;
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                FAccess = OldAccess;
            };
            FAccess = WindowsSdk::STANDARD_RIGHTS_READ | WindowsSdk::KEY_QUERY_VALUE | WindowsSdk::KEY_ENUMERATE_SUB_KEYS | OldAccess & WindowsSdk::KEY_WOW64_RES;
            TempKey = GetKey(Key);
            if (TempKey != 0) {
                WindowsSdk::RegCloseKey(TempKey);
            }
            Result = TempKey != 0;
        }
        return Result;
    }

    void TRegistry::p_destroy() {
        Registry::TRegistry_Destroy(this);
    }

} // namespace Registry
