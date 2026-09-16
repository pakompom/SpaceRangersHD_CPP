#pragma once
#include "runtime_support.hpp"
#include "types/Windows_group.hpp"

namespace Registry {
    struct TRegistry;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRegistry : pas::Object {
        PAS_CLASS_META(TRegistry, pas::Object, "TRegistry", 28)
        void p_destroy() override;
        void CloseKey();
        void SetRootKey(WindowsSdk::HKEY Value);
        WindowsSdk::HKEY GetBaseKey(std::uint8_t Relative);
        WindowsSdk::HKEY GetKey(const pas::AnsiString& Key);
        std::uint8_t KeyExists(const pas::AnsiString& Key);
        WindowsSdk::HKEY FCurrentKey;
        WindowsSdk::HKEY FRootKey;
        std::uint8_t FLazyWrite;
        std::uint8_t cpp_padding[3];
        pas::AnsiString FCurrentPath;
        std::uint8_t FCloseRootKey;
        std::uint8_t cpp_padding_2[3];
        std::uint32_t FAccess;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace Registry
