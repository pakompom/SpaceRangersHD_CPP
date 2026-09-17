#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_ShrLight {
    struct TShrLightGI;

    enum TShrLightKindGI : std::uint8_t {
        slkAll = 0,
        slkBuffer = 1,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TShrLightGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TShrLightGI, GI_MessageLoop::TObjectGI, "TShrLightGI", 312)
        void p_destroy() override;
        void Clear() override;
        void SetKind(TShrLightKindGI Value);
        void SetLightShift(std::int32_t Value);
        void SetSize(Types::TPoint Size) override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadLightProperties(EC_BlockPar::TBlockParEC* Block);
        void Draw(Types::TRect ClipRect) override;
        TShrLightKindGI Kind;
        std::uint8_t cpp_padding[3];
        std::int32_t LightShift;
        // Owned grayscale mask when Kind=slkBuffer.
        GR_GraphBuf::TGraphBufGR* LightBuffer;
        std::uint8_t cpp_padding_2[12];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_ShrLight
