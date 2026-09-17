#include "layout/GI_TransImage.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheTBitmap.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_TransImage.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace GI_TransImage {
    void TTransImageGI_Create(TTransImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheTBitmap::TCTBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
    }

    void TTransImageGI_Destroy(TTransImageGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves HalfAlpha and the cache key.
    void TTransImageGI::Clear() {
        ImageKindX = GI_Main::ikxCenter;
        ImageKindY = GI_Main::ikyCenter;
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TTransImageGI::SetImagePath(const pas::WideString& ImagePath) {
        if (ImageCache->CacheKey != ImagePath) {
            Invalidate();
            ImageCache->SetCacheKey(ImagePath);
        }
    }

    Types::TPoint TTransImageGI::GetContentSize() {
        Types::TPoint Result{};
        EC_CacheTBitmap::TCTBitmapEC* Bitmap = EC_CacheTBitmap::AcquireCachedTransBitmap(ImageCache);
        {
            Result = Bitmap->PixelSize;
            ImageCache->Release();
        }
        return Result;
    }

    void TTransImageGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TTransImageGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    void TTransImageGI::SetHalfAlpha(std::uint8_t Value) {
        if (HalfAlpha != Value) {
            HalfAlpha = Value;
            Invalidate();
        }
    }

    void TTransImageGI_LoadFromConfigPath(TTransImageGI* Self, const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        EC_CacheTBitmap::TCTBitmapEC* Bitmap{};
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            Self->ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
            Bitmap = EC_CacheTBitmap::AcquireCachedTransBitmap(Self->ImageCache);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    Self->ImageCache->Release();
                };
                Self->SetSize(Bitmap->PixelSize);
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

    void TTransImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        EC_CacheTBitmap::TCTBitmapEC* Bitmap = EC_CacheTBitmap::AcquireCachedTransBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            SetSize(Bitmap->PixelSize);
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

    void TTransImageGI::Draw(Types::TRect ClipRect) {
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        EC_CacheTBitmap::TCTBitmapEC* Bitmap = EC_CacheTBitmap::AcquireCachedTransBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            Width = Bitmap->PixelSize.X;
            Height = Bitmap->PixelSize.Y;
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
            GR_Main::AppendLogLineThreadSafe("Test"_a);
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        void* transBuffer = Bitmap->TransBuffer;
                        std::uint8_t halfAlpha = HalfAlpha;
                        void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::DrawTransparentBuffer16(pixels, pitchBytes, X, Y, transBuffer, ClipRect, halfAlpha);
                    }
                    X += Width;
                }
                Y += Height;
            }
        }
    }

    void TTransImageGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TTransImageGI::p_destroy() {
        GI_TransImage::TTransImageGI_Destroy(this);
    }

    void TTransImageGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_TransImage::TTransImageGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_TransImage
