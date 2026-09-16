#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Line {
    struct TLineGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TLineGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TLineGI, GI_MessageLoop::TObjectGI, "TLineGI", 292)
        void p_destroy() override;
        void Clear() override;
        void SetColor(std::uint32_t Value);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadLineProperties(EC_BlockPar::TBlockParEC* Block);
        void Draw(Types::TRect ClipRect) override;
        std::uint32_t Color;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Line
