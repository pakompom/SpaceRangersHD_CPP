#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_AImage {
    struct TAImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TAImageGI, GI_MessageLoop::TObjectGI, "TAImageGI", 300)
        void p_destroy() override;
        void Clear() override;
        Types::TPoint GetContentSize();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetHalfAlpha(std::uint8_t Value);
        void SetSize(Types::TPoint Size) override;
        void AdvanceFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        std::uint8_t HitTest(Types::TPoint Point);
        void OnActivate() override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadAnimationProperties(EC_BlockPar::TBlockParEC* Block);
        void QueueImageLoad(pas::List* PendingLoads) override;
        GI_MessageLoop::PCallbackTimerGI FrameTimer;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t HalfAlpha;
        std::uint8_t cpp_padding[1];
        GI_MessageLoop::TObjectGI* CurrentFrame;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_AImage
