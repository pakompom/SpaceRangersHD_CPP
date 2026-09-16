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

namespace GI_Circle {
    struct TCircleGI;

    enum TCircleKindGI : std::uint8_t {
        ckSimple = 0,
        ckCircle = 1,
        ckFill = 2,
        ckShrLight = 3,
        ckMulLight = 4,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCircleGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TCircleGI, GI_MessageLoop::TObjectGI, "TCircleGI", 320)
        void p_destroy() override;
        void Clear() override;
        void SetKind(TCircleKindGI Value);
        void SetColor(std::uint32_t Value);
        void SetFillColor(std::uint32_t Value);
        void SetCenter(Types::TPoint Value);
        void SetRadius(std::int32_t Value);
        void SetShrLightInner(std::uint8_t Value);
        void SetShrLightOuter(std::uint8_t Value);
        void SetSize(Types::TPoint Size) override;
        void SetActive(std::uint8_t Enabled) override;
        void OnActivate() override;
        void OnDeactivate() override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadShapeProperties(EC_BlockPar::TBlockParEC* Block);
        void Draw(Types::TRect ClipRect) override;
        TCircleKindGI Kind;
        std::uint8_t cpp_padding[3];
        std::uint32_t Color;
        std::uint32_t FillColor;
        Types::TPoint Center;
        std::int32_t Radius;
        std::uint8_t ShrLightInner;
        std::uint8_t ShrLightOuter;
        std::uint8_t LightBufferDirty;
        std::uint8_t cpp_padding_2[1];
        GR_GraphBuf::TGraphBufGR* LightBuffer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Circle
