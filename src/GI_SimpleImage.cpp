#include "layout/GI_SimpleImage.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_SimpleImage.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"

namespace GI_SimpleImage {
    void TSimpleImageGI_Create(TSimpleImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
        Self->HalfAlpha = false;
    }

    void TSimpleImageGI_Destroy(TSimpleImageGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves the cache control and its key.
    void TSimpleImageGI::Clear() {
        ImageKindX = GI_Main::ikxCenter;
        ImageKindY = GI_Main::ikyCenter;
        HalfAlpha = false;
        SourceRGBA = false;
        GI_MessageLoop::TObjectGI::Clear();
    }

    // The RGBA key suffix enables SourceRGBA; keys shorter than four characters preserve the previous flag.
    void TSimpleImageGI::SetImagePath(const pas::WideString& ImagePath) {
        Invalidate();
        ImageCache->SetCacheKey(ImagePath);
        if (ImagePath.length() >= 4) {
            SourceRGBA = pas::copy(ImagePath, ImagePath.length() - 4 + 1, 4) == u"RGBA";
        }
    }

    Types::TPoint TSimpleImageGI::GetContentSize() {
        Types::TPoint Result{};
        EC_CacheBitmap::TCBitmapEC* Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
        {
            Result = ClassesImports::Point(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height);
            ImageCache->Release();
        }
        return Result;
    }

    void TSimpleImageGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TSimpleImageGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    void TSimpleImageGI::SetHalfAlpha(std::uint8_t Value) {
        if (HalfAlpha != Value) {
            HalfAlpha = Value;
            Invalidate();
        }
    }

    // Image is optional.
    void TSimpleImageGI_LoadFromConfigPath(TSimpleImageGI* Self, const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        EC_CacheBitmap::TCBitmapEC* Bitmap{};
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            Self->ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
            Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(Self->ImageCache);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    Self->ImageCache->Release();
                };
                Self->SetSize(ClassesImports::Point(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height));
            }
        }
        if (Block->CountParams(u"Size"_wref.get()) > 0) {
            Text = Block->GetParam(u"Size"_wref.get());
            Self->SetSize(([&] {
                std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
                return ClassesImports::Point(strToInt_2, strToInt);
            }()));
        }
        if (Block->CountParams(u"KindX"_wref.get()) > 0) {
            Self->SetImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"KindX"_wref.get())));
        }
        if (Block->CountParams(u"KindY"_wref.get()) > 0) {
            Self->SetImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"KindY"_wref.get())));
        }
        if (Block->CountParams(u"HalfAlpha"_wref.get()) > 0) {
            Self->SetHalfAlpha(GI_Main::ParseEnabledNameGI(Block->GetParam(u"HalfAlpha"_wref.get())));
        }
    }

    // Requires Image; does not update SourceRGBA from the key suffix.
    void TSimpleImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        EC_CacheBitmap::TCBitmapEC* Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            SetSize(ClassesImports::Point(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height));
        }
        if (Block->CountParams(u"Size"_wref.get()) > 0) {
            Text = Block->GetParam(u"Size"_wref.get());
            SetSize(([&] {
                std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
                return ClassesImports::Point(strToInt_2, strToInt);
            }()));
        }
        if (Block->CountParams(u"KindX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"KindX"_wref.get())));
        }
        if (Block->CountParams(u"KindY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"KindY"_wref.get())));
        }
        if (Block->CountParams(u"HalfAlpha"_wref.get()) > 0) {
            SetHalfAlpha(GI_Main::ParseEnabledNameGI(Block->GetParam(u"HalfAlpha"_wref.get())));
        }
    }

    // CenterFill is unimplemented on both axes.
    void TSimpleImageGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        EC_CacheBitmap::TCBitmapEC* Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            Width = Bitmap->Bitmap->Width;
            Height = Bitmap->Bitmap->Height;
            if (ImageKindX == GI_Main::ikxLeftFill) {
                Left = HitTestBounds.Left;
                Right = HitTestBounds.Right;
            } else if (ImageKindX == GI_Main::ikxRightFill) {
                Right = HitTestBounds.Right;
                Left = Right;
                while (Left > ClipRect.Left) {
                    Left -= Width;
                }
            } else if (ImageKindX == GI_Main::ikxLeft) {
                Left = HitTestBounds.Left;
                Right = Left + Width;
            } else if (ImageKindX == GI_Main::ikxRight) {
                Right = HitTestBounds.Right;
                Left = Right - Width;
            } else if (ImageKindX == GI_Main::ikxCenter) {
                Left = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - Width / 2;
                Right = Left + Width;
            } else {
                return;
            }
            if (ImageKindY == GI_Main::ikyTopFill) {
                Top = HitTestBounds.Top;
                Bottom = HitTestBounds.Bottom;
            } else if (ImageKindY == GI_Main::ikyBottomFill) {
                Bottom = HitTestBounds.Bottom;
                Top = Bottom;
                while (Top > ClipRect.Top) {
                    Top -= Height;
                }
            } else if (ImageKindY == GI_Main::ikyTop) {
                Top = HitTestBounds.Top;
                Bottom = Top + Height;
            } else if (ImageKindY == GI_Main::ikyBottom) {
                Bottom = HitTestBounds.Bottom;
                Top = Bottom - Height;
            } else if (ImageKindY == GI_Main::ikyCenter) {
                Top = (HitTestBounds.Bottom - HitTestBounds.Top) / 2 + HitTestBounds.Top - Height / 2;
                Bottom = Top + Height;
            } else {
                return;
            }
            if (GlobalsV::HardwareRenderingEnabled) {
                Y = Top;
                while (Y < Bottom) {
                    X = Left;
                    while (X < Right) {
                        GR_DX::DrawTexture((Bitmap->Bitmap->GetTexture(cpp_result), cpp_result), X, Y, 255, 0x00ffffffu, &ClipRect, false, false);
                        X += Width;
                    }
                    Y += Height;
                }
            } else if (SourceRGBA) {
                Y = Top;
                while (Y < Bottom) {
                    X = Left;
                    while (X < Right) {
                        {
                            GR_GraphBuf::TGraphBufGR* bitmap = Bitmap->Bitmap;
                            WindowsSdk::TRect clipRect = ClipRect;
                            void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                            std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                            GR_Main::DrawAlphaGraphBuffer16Clipped(pixels, pitchBytes, X, Y, bitmap, clipRect);
                        }
                        X += Width;
                    }
                    Y += Height;
                }
            } else {
                Y = Top;
                while (Y < Bottom) {
                    X = Left;
                    while (X < Right) {
                        {
                            GR_GraphBuf::TGraphBufGR* bitmap_2 = Bitmap->Bitmap;
                            WindowsSdk::TRect clipRect_2 = ClipRect;
                            std::uint8_t halfAlpha = HalfAlpha;
                            void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                            std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                            GR_Main::CopyGraphBuffer16Clipped(pixels_2, pitchBytes_2, X, Y, bitmap_2, clipRect_2, halfAlpha, false);
                        }
                        X += Width;
                    }
                    Y += Height;
                }
            }
        }
    }

    void TSimpleImageGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TSimpleImageGI::p_destroy() {
        GI_SimpleImage::TSimpleImageGI_Destroy(this);
    }

    void TSimpleImageGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_SimpleImage::TSimpleImageGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_SimpleImage
