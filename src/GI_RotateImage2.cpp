#include "layout/GI_RotateImage2.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheRotateBuf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_RotateImage2.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_RotateImage2 {
    void TRotateImage2GI_Create(TRotateImage2GI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageDirty = true;
        Self->ImageCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->RotationCache = pas::construct_call<EC_CacheRotateBuf::TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->RotationCache);
        Self->RotatedImage = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Self->RenderedAngle = 0;
        Self->Angle = 0;
        Self->Alpha = 255;
        Self->ImageDirty = true;
    }

    void TRotateImage2GI_Destroy(TRotateImage2GI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        pas::free(Self->RotationCache);
        Self->RotationCache = nullptr;
        pas::free(Self->RotatedImage);
        Self->RotatedImage = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves cache keys and the allocated image buffer.
    void TRotateImage2GI::Clear() {
        ImageDirty = true;
        RenderedAngle = 255;
        Angle = 0;
        Alpha = 255;
        GI_MessageLoop::TObjectGI::Clear();
    }

    // A full turn has 256 steps.
    void TRotateImage2GI::SetAngle(std::uint8_t Value) {
        if (Value != Angle) {
            Angle = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    void TRotateImage2GI::SetAlpha(std::uint8_t Value) {
        if (Value != Alpha) {
            Alpha = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    // Appends ?RGBA to Path; replaces size and origin with a centered square enclosing all rotations.
    void TRotateImage2GI::SetImage(pas::WideString Path, Types::TPoint ImageSize, Types::TPoint Pivot) {
        EC_CacheBitmap::TCBitmapEC* Image{};
        double Radius{};
        ImageCache->SetCacheKey(pas::concat_wide({Path, EC_CacheBitmap::RgbaImagePathSuffix}));
        Image = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            RotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(ImageSize.X), ",", SysUtils::IntToStr(ImageSize.Y), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Image->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Image->Bitmap->Height)), ",", SysUtils::IntToStr(Pivot.X), ",", SysUtils::IntToStr(Pivot.Y)})));
            Radius = EC_Struct::SquaredDistanceToPoint(Pivot, 0, 0);
            Radius = pas::real_max<double>(Radius, static_cast<double>(EC_Struct::SquaredDistanceToPoint(Pivot, ImageSize.X, ImageSize.Y)));
            Radius = pas::real_max<double>(Radius, static_cast<double>(EC_Struct::SquaredDistanceToPoint(Pivot, ImageSize.X, 0)));
            Radius = pas::real_max<double>(Radius, static_cast<double>(EC_Struct::SquaredDistanceToPoint(Pivot, 0, ImageSize.Y)));
            Radius = MathImports::Floor(System::Sqrt(Radius) * 2.0L + 2.0L);
            {
                std::int32_t trunc = System::Trunc(Radius);
                std::int32_t trunc_2 = System::Trunc(Radius);
                SetSize(ClassesImports::Point(trunc_2, trunc));
            }
            SetOrigin(ClassesImports::Point(ClientSize.X / 2, ClientSize.Y / 2));
            if (RotatedImage->Width != ClientSize.X || RotatedImage->Height != ClientSize.Y) {
                RotatedImage->AllocateRgbaTight(ClientSize.X, ClientSize.Y);
            }
            ImageDirty = true;
        }
        Invalidate();
    }

    void TRotateImage2GI_LoadFromConfigPath(TRotateImage2GI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TRotateImage2GI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        RenderedAngle = 255;
        Angle = 0;
        Alpha = 255;
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
        ImageDirty = true;
    }

    void TRotateImage2GI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Image"_wref.get()) > 0 && Block->CountParams(u"Size"_wref.get()) > 0 && Block->CountParams(u"Sme"_wref.get()) > 0) {
            Types::TPoint pointGI = GI_Main::GetPointGI(pas::view(Block->GetParam(u"Sme"sv)));
            Types::TPoint pointGI_2 = GI_Main::GetPointGI(pas::view(Block->GetParam(u"Size"sv)));
            pas::WideString param = Block->GetParam(u"Image"sv);
            SetImage(std::move(param), pointGI_2, pointGI);
        }
        if (Block->CountParams(u"Angle"_wref.get()) > 0) {
            SetAngle(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Angle"sv))));
        }
        if (Block->CountParams(u"Trans"_wref.get()) > 0) {
            SetAlpha(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Trans"sv))));
        }
    }

    void TRotateImage2GI::Draw(Types::TRect ClipRect) {
        EC_CacheBitmap::TCBitmapEC* Image{};
        EC_CacheRotateBuf::TCRotateBufEC* Rotation{};
        if (HitTestBounds.Left + ClientSize.X < 0 || HitTestBounds.Left - ClientSize.X / 2 > GR_Main::GameScreenWidth || HitTestBounds.Top + ClientSize.Y < 0 || HitTestBounds.Top - ClientSize.Y / 2 > GR_Main::GameScreenHeight) {
            return;
        }
        if (RenderedAngle != Angle || ImageDirty == true) {
            ImageDirty = false;
            RenderedAngle = Angle;
            Image = nullptr;
            Rotation = nullptr;
            {
                std::exception_ptr cpp_error{};
                try {
                    Image = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
                    Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(RotationCache);
                    RotatedImage->ClearPixels();
                    {
                        std::int32_t pitchBytes = Image->Bitmap->PitchBytes;
                        std::int32_t x = OriginPoint.X;
                        std::int32_t y = OriginPoint.Y;
                        std::uint8_t angle = Angle;
                        void* buffer = Rotation->Buffer;
                        void* pixels = Image->Bitmap->GetPixels();
                        void* pixels_2 = RotatedImage->GetPixels();
                        std::int32_t pitchBytes_2 = RotatedImage->PitchBytes;
                        GR_Main::Ex_OKGR_RotateBuf_Draw_DWORD(pixels_2, pitchBytes_2, pixels, pitchBytes, x, y, angle, buffer);
                    }
                    if (Alpha != 255) {
                        std::int32_t width = RotatedImage->Width;
                        std::int32_t height = RotatedImage->Height;
                        std::uint8_t alpha = Alpha;
                        void* addPointerOffset = EC_Mem::AddPointerOffset(RotatedImage->GetPixels(), 3);
                        std::int32_t cpp_arg = RotatedImage->PitchBytes - 4 * RotatedImage->Width;
                        GR_Main::Ex_OKGR_Light_BYTE(addPointerOffset, 4, cpp_arg, width, height, alpha);
                    }
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                if (Image != nullptr) {
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
        {
            std::int32_t top = HitTestBounds.Top;
            GR_GraphBuf::TGraphBufGR* rotatedImage = RotatedImage;
            void* pixels_3 = GR_Main::ScreenRenderBuffer->GetPixels();
            std::int32_t left = HitTestBounds.Left;
            std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
            GR_Main::DrawAlphaGraphBuffer16Clipped(pixels_3, pitchBytes_3, left, top, rotatedImage, ClipRect);
        }
    }

    void TRotateImage2GI::p_destroy() {
        GI_RotateImage2::TRotateImage2GI_Destroy(this);
    }

    void TRotateImage2GI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_RotateImage2::TRotateImage2GI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_RotateImage2
