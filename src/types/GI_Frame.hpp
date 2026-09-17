#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Frame {
    struct TFrameGI;

    enum TFrameKindGI : std::uint8_t {
        fkHide = 0,
        fkRect = 1,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TFrameGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TFrameGI, GI_MessageLoop::TObjectGI, "TFrameGI", 304)
        void p_destroy() override;
        // Preserves fill and color fields.
        void Clear() override;
        void SetKind(TFrameKindGI Value);
        void SetColor(std::uint32_t Value);
        void SetFillColor(std::uint32_t Value);
        void SetFill(std::uint8_t Value);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFrameProperties(EC_BlockPar::TBlockParEC* Block);
        // Fill is independent of Kind. FillAlpha values other than 255 all produce alpha 64.
        void Draw(Types::TRect ClipRect) override;
        TFrameKindGI Kind;
        std::uint8_t cpp_padding[3];
        std::uint32_t Color;
        std::uint32_t FillColor;
        std::uint8_t Fill;
        std::uint8_t FillAlpha;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Frame
