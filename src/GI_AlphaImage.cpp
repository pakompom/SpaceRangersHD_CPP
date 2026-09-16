#include "layout/GI_AlphaImage.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheAlphaBitmap.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_AlphaImage.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"

namespace GI_AlphaImage {
    void TAlphaImageGI_Create(TAlphaImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheAlphaBitmap::TCAlphaBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
    }

    void TAlphaImageGI_Destroy(TAlphaImageGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TAlphaImageGI::Clear() {
        ImageKindX = GI_Main::ikxCenter;
        ImageKindY = GI_Main::ikyCenter;
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TAlphaImageGI::SetImagePath(const pas::WideString& ImagePath) {
        if (ImageCache->CacheKey != ImagePath) {
            Invalidate();
            ImageCache->SetCacheKey(ImagePath);
        }
    }

    Types::TPoint TAlphaImageGI::GetContentSize() {
        Types::TPoint Result{};
        EC_CacheAlphaBitmap::TCAlphaBitmapEC* Bitmap = EC_CacheAlphaBitmap::AcquireOrCreateAlphaBitmap(ImageCache);
        {
            Result = Bitmap->PixelSize;
            ImageCache->Release();
        }
        return Result;
    }

    void TAlphaImageGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TAlphaImageGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    // Black pixels do not count as hits.
    std::uint8_t TAlphaImageGI::HitTestPixel(Types::TPoint Point) {
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        std::uint32_t Pixel{};
        void* Pixels{};
        Types::TRect Clip{};
        std::uint8_t Result = false;
        Pixel = 0u;
        Clip.Left = Point.X;
        Clip.Top = Point.Y;
        Clip.Right = Point.X + 1;
        Clip.Bottom = Point.Y + 1;
        EC_CacheAlphaBitmap::TCAlphaBitmapEC* Bitmap = EC_CacheAlphaBitmap::AcquireOrCreateAlphaBitmap(ImageCache);
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
                while (Left > Clip.Left) {
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
                return Result;
            }
            if (ImageKindY == GI_Main::ikyTopFill) {
                Top = HitTestBounds.Top;
                Bottom = HitTestBounds.Bottom;
            } else if (ImageKindY == GI_Main::ikyBottomFill) {
                Bottom = HitTestBounds.Bottom;
                Top = Bottom;
                while (Top > Clip.Top) {
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
                return Result;
            }
            Pixels = EC_Mem::AddPointerOffset(&Pixel, -(GR_Main::ScreenRenderBuffer->PitchBytes * Point.Y + Point.X * static_cast<std::int32_t>(sizeof(std::uint16_t))));
            Y = Top;
            while (Y < Bottom && Pixel == 0) {
                X = Left;
                while (X < Right && Pixel == 0) {
                    Bitmap->Draw16(Pixels, GR_Main::ScreenRenderBuffer->PitchBytes, X, Y, Clip);
                    X += Width;
                }
                Y += Height;
            }
        }
        return Pixel != 0;
    }

    void TAlphaImageGI::LoadFromConfigPath(const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        EC_CacheAlphaBitmap::TCAlphaBitmapEC* Bitmap{};
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
            Bitmap = EC_CacheAlphaBitmap::AcquireOrCreateAlphaBitmap(ImageCache);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    ImageCache->Release();
                };
                SetSize(Bitmap->PixelSize);
            }
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
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"AlignX"_wref.get())));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"AlignY"_wref.get())));
        }
    }

    void TAlphaImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        EC_CacheAlphaBitmap::TCAlphaBitmapEC* Bitmap = EC_CacheAlphaBitmap::AcquireOrCreateAlphaBitmap(ImageCache);
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
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"AlignX"_wref.get())));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"AlignY"_wref.get())));
        }
    }

    void TAlphaImageGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        EC_CacheAlphaBitmap::TCAlphaBitmapEC* Bitmap = EC_CacheAlphaBitmap::AcquireOrCreateAlphaBitmap(ImageCache);
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
            if (GlobalsV::HardwareRenderingEnabled) {
                Y = Top;
                while (Y < Bottom) {
                    X = Left;
                    while (X < Right) {
                        GR_DX::DrawTexture((Bitmap->GetTexture(cpp_result), cpp_result), X, Y, 255, 0x00ffffffu, &ClipRect, false, false);
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
                            WindowsSdk::TRect clipRect = ClipRect;
                            void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                            std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                            Bitmap->Draw16(pixels, pitchBytes, X, Y, clipRect);
                        }
                        X += Width;
                    }
                    Y += Height;
                }
            }
        }
    }

    void TAlphaImageGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TAlphaImageGI::p_destroy() {
        GI_AlphaImage::TAlphaImageGI_Destroy(this);
    }

} // namespace GI_AlphaImage
