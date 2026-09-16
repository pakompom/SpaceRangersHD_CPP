#include "layout/GI_SimpleButton.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_SimpleButton.hpp"
#include "units/GR_Main.hpp"

// Native class ownership follows reference/unit_ownership.json.
namespace GI_SimpleButton {
    void TSimpleButtonGI_Create(TSimpleButtonGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->NormalImage = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->NormalImage);
        Self->ActiveImage = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ActiveImage);
    }

    void TSimpleButtonGI_Destroy(TSimpleButtonGI* Self) {
        pas::free(Self->NormalImage);
        Self->NormalImage = nullptr;
        pas::free(Self->ActiveImage);
        Self->ActiveImage = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TSimpleButtonGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TSimpleButtonGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
        CurrentImage = ActiveImage;
        Invalidate();
    }

    void TSimpleButtonGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
        CurrentImage = NormalImage;
        Invalidate();
    }

    void TSimpleButtonGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        DispatchNamedEvent(1, Point.X, Point.Y);
    }

    void TSimpleButtonGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        DispatchNamedEvent(2, Point.X, Point.Y);
    }

    void TSimpleButtonGI::LoadFromConfigPath(const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        EC_CacheBitmap::TCBitmapEC* Bitmap{};
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            NormalImage->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
            Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(NormalImage);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    NormalImage->Release();
                };
                SetSize(ClassesImports::Point(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height));
            }
        }
        if (Block->CountParams(u"ImageActive"_wref.get()) > 0) {
            ActiveImage->SetCacheKey(Block->GetParam(u"ImageActive"_wref.get()));
        }
    }

    void TSimpleButtonGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        NormalImage->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        EC_CacheBitmap::TCBitmapEC* Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(NormalImage);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                NormalImage->Release();
            };
            SetSize(ClassesImports::Point(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height));
        }
        ActiveImage->SetCacheKey(Block->GetParam(u"ImageActive"_wref.get()));
        CurrentImage = NormalImage;
    }

    void TSimpleButtonGI::Draw(Types::TRect ClipRect) {
        EC_CacheBitmap::TCBitmapEC* Bitmap{};
        if (CurrentImage != nullptr) {
            Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(CurrentImage);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    CurrentImage->Release();
                };
                {
                    void* pixels = Bitmap->Bitmap->GetPixels();
                    std::int32_t pitchBytes = Bitmap->Bitmap->PitchBytes;
                    std::int32_t cpp_arg = ClipRect.Left - HitTestBounds.Left;
                    std::int32_t cpp_arg_2 = ClipRect.Top - HitTestBounds.Top;
                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels_2, pitchBytes_2, ClipRect.Left, ClipRect.Top, pixels, pitchBytes, cpp_arg, cpp_arg_2, ClipRect.Right - ClipRect.Left, ClipRect.Bottom - ClipRect.Top);
                }
            }
        }
    }

    void TSimpleButtonGI::QueueImageLoad(pas::List* PendingLoads) {
        NormalImage->QueueLoadIfMissing(PendingLoads);
        ActiveImage->QueueLoadIfMissing(PendingLoads);
    }

    void TSimpleButtonGI::p_destroy() {
        GI_SimpleButton::TSimpleButtonGI_Destroy(this);
    }

} // namespace GI_SimpleButton
