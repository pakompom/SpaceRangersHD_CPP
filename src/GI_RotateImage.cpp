#include "layout/GI_RotateImage.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheRotateBuf.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_RotateImage.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace GI_RotateImage {
    void TRotateImageGI_Create(TRotateImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->RotationCache = pas::construct_call<EC_CacheRotateBuf::TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->RotationCache);
    }

    void TRotateImageGI_Destroy(TRotateImageGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        pas::free(Self->RotationCache);
        Self->RotationCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves both cache keys.
    void TRotateImageGI::Clear() {
        Angle = 0;
        GI_MessageLoop::TObjectGI::Clear();
    }

    // A full turn has 256 steps.
    void TRotateImageGI::SetAngle(std::uint8_t Value) {
        if (Value == Angle) {
            return;
        }
        if (!Active) {
            Angle = Value;
        } else {
            Invalidate();
            Angle = Value;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
        }
    }

    // Leaves bounds unchanged when the rotation cache key is empty.
    void TRotateImageGI::UpdateHitTestBounds() {
        if (RotationCache->HasEmptyCacheKey()) {
            return;
        }
        EC_CacheRotateBuf::TCRotateBufEC* Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(RotationCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                RotationCache->Release();
            };
            GR_Main::Ex_OKGR_RotateBuf_Size(AbsolutePosition.X, AbsolutePosition.Y, Angle, Rotation->Buffer, HitTestBounds);
            ++HitTestBounds.Right;
            ++HitTestBounds.Bottom;
        }
    }

    // Leaves Result unwritten when the rotation cache key is empty.
    Types::TRect TRotateImageGI::GetLocalBounds() {
        Types::TRect Result{};
        if (RotationCache->HasEmptyCacheKey()) {
            return Result;
        }
        EC_CacheRotateBuf::TCRotateBufEC* Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(RotationCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                RotationCache->Release();
            };
            GR_Main::Ex_OKGR_RotateBuf_Size(LocalPosition.X, LocalPosition.Y, Angle, Rotation->Buffer, Result);
            ++Result.Right;
            ++Result.Bottom;
        }
        return Result;
    }

    // Uses the current Origin as the rotation pivot.
    void TRotateImageGI::SetImage(pas::WideString Path, Types::TPoint ImageSize) {
        EC_CacheBitmap::TCBitmapEC* Bitmap{};
        ImageCache->SetCacheKey(Path);
        Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            RotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(ImageSize.X), ",", SysUtils::IntToStr(ImageSize.Y), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Bitmap->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Bitmap->Bitmap->Height)), ",", SysUtils::IntToStr(OriginPoint.X), ",", SysUtils::IntToStr(OriginPoint.Y)})));
            SetSize(ImageSize);
        }
    }

    void TRotateImageGI_LoadFromConfigPath(TRotateImageGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Image"_wref.get()) > 0 && Block->CountParams(u"Size"_wref.get()) > 0) {
            Types::TPoint pointGI = GI_Main::GetPointGI(Block->GetParam(u"Size"_wref.get()));
            pas::WideString param = Block->GetParam(u"Image"_wref.get());
            Self->SetImage(std::move(param), pointGI);
        }
        if (Block->CountParams(u"Angle"_wref.get()) > 0) {
            Self->Angle = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Angle"_wref.get())));
        }
    }

    void TRotateImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        Angle = 1;
        SetAngle(0);
        {
            Types::TPoint pointGI = GI_Main::GetPointGI(Block->GetParam(u"Size"_wref.get()));
            pas::WideString param = Block->GetParam(u"Image"_wref.get());
            SetImage(std::move(param), pointGI);
        }
        if (Block->CountParams(u"Angle"_wref.get()) > 0) {
            Angle = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Angle"_wref.get())));
        }
    }

    void TRotateImageGI::Draw(Types::TRect ClipRect) {
        Types::TRect Rect{};
        if (HitTestBounds.Left + ClientSize.X < 0) {
            return;
        }
        if (HitTestBounds.Left - ClientSize.X / 2 > GR_Main::GameScreenWidth) {
            return;
        }
        if (HitTestBounds.Top + ClientSize.Y < 0) {
            return;
        }
        if (HitTestBounds.Top - ClientSize.Y / 2 > GR_Main::GameScreenHeight) {
            return;
        }
        EC_CacheBitmap::TCBitmapEC* Bitmap = nullptr;
        EC_CacheRotateBuf::TCRotateBufEC* Rotation = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
                Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(RotationCache);
                Rect.Left = ClipRect.Left;
                Rect.Top = ClipRect.Top;
                Rect.Right = ClipRect.Right - 1;
                Rect.Bottom = ClipRect.Bottom - 1;
                {
                    std::int32_t pitchBytes = Bitmap->Bitmap->PitchBytes;
                    std::int32_t x = AbsolutePosition.X;
                    std::int32_t y = AbsolutePosition.Y;
                    std::uint8_t angle = Angle;
                    void* buffer = Rotation->Buffer;
                    const WindowsSdk::TRect& rect = Rect;
                    void* pixels = Bitmap->Bitmap->GetPixels();
                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_RotateBuf_DrawTransClip_WORD(pixels_2, pitchBytes_2, pixels, pitchBytes, x, y, angle, buffer, rect);
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Bitmap != nullptr) {
                ImageCache->Release();
            }
            if (Rotation != nullptr) {
                RotationCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TRotateImageGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
        RotationCache->QueueLoadIfMissing(PendingLoads);
    }

    void TRotateImageGI::p_destroy() {
        GI_RotateImage::TRotateImageGI_Destroy(this);
    }

    void TRotateImageGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_RotateImage::TRotateImageGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_RotateImage
