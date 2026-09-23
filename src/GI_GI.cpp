#include "layout/GI_GI.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGI.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"

namespace GI_GI {
    void LoadGiByPathIntoGraphBuf(const pas::WideString& GiPath, GR_GraphBuf::TGraphBufGR* Destination) {
        EC_Cache::TCacheControlEC* Control = pas::construct_call<EC_CacheGI::TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Control);
        Control->SetCacheKey(GiPath);
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(Control);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Control->Release();
            };
            Image->Image->DecodeToGraphBuf(Destination, false);
        }
        pas::free(Control);
    }

    void TgiGI_Create(TgiGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheGI::TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
        Self->Alpha = 255;
    }

    void TgiGI_Destroy(TgiGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves alpha and the cache key.
    void TgiGI::Clear() {
        ImageKindX = GI_Main::ikxCenter;
        ImageKindY = GI_Main::ikyCenter;
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TgiGI::SetImagePath(const pas::WideString& ImagePath) {
        if (ImageCache->CacheKey != ImagePath) {
            Invalidate();
            ImageCache->SetCacheKey(ImagePath);
        }
    }

    pas::WideString TgiGI::GetImagePath() {
        return ImageCache->CacheKey;
    }

    Types::TPoint TgiGI::GetContentSize() {
        Types::TPoint Result{};
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(ImageCache);
        {
            Result = Image->Image->GetContentSize();
            ImageCache->Release();
        }
        return Result;
    }

    Types::TPoint TgiGI::GetContentOrigin() {
        Types::TPoint Result{};
        Types::TRect Bounds{};
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(ImageCache);
        {
            Bounds = Image->Image->GetBoundsRect();
            Result = pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Bounds, 0));
            ImageCache->Release();
        }
        return Result;
    }

    void TgiGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TgiGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    void TgiGI::SetAlpha(std::uint8_t Value) {
        if (Alpha != Value) {
            Alpha = Value;
            Invalidate();
        }
    }

    // Black pixels do not count as hits.
    std::uint8_t TgiGI::HitTestPixel(Types::TPoint Point) {
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
        GR_GraphBuf::TGraphBufGR* Buffer{};
        Types::TRect Clip{};
        std::uint8_t Result = false;
        Pixel = 0u;
        Clip.Left = Point.X;
        Clip.Top = Point.Y;
        Clip.Right = Point.X + 1;
        Clip.Bottom = Point.Y + 1;
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            Width = Image->Image->GetContentSize().X;
            Height = Image->Image->GetContentSize().Y;
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
            Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Buffer->AttachPixels(1, 1, GR_Main::ScreenRenderBuffer->PitchBytes, Pixels);
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    Image->Image->DrawToGraphBuf(Buffer, X, Y, Clip, 0, 255);
                    X += Width;
                }
                Y += Height;
            }
            pas::free(Buffer);
        }
        return Pixel != 0;
    }

    // Returns the mean coordinates of nonzero rendered pixels, or (0,0) when none exist.
    Types::TPoint TgiGI::GetVisualCenter() {
        Types::TPoint Result{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        Types::TRect SavedBounds{};
        Types::TRect Clip{};
        SavedBounds = HitTestBounds;
        HitTestBounds.Right -= HitTestBounds.Left;
        HitTestBounds.Bottom -= HitTestBounds.Top;
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        Clip = HitTestBounds;
        GR_GraphBuf::TGraphBufGR* Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Buffer->AllocateNative(HitTestBounds.Right, HitTestBounds.Bottom);
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(ImageCache);
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                Width = Image->Image->GetContentSize().X;
                Height = Image->Image->GetContentSize().Y;
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
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
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
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Buffer->ClearPixels();
                Y = Top;
                while (Y < Bottom) {
                    X = Left;
                    while (X < Right) {
                        Image->Image->DrawToGraphBuf(Buffer, X, Y, Clip, 0, 255);
                        X += Width;
                    }
                    Y += Height;
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            ImageCache->Release();
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        Result.X = 0;
        Result.Y = 0;
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Buffer->Height - 1); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Buffer->Width - 1); cpp_range_2.next(X); ) {
                if (Buffer->GetPixel16(X, Y) != 0) {
                    Result.X += X;
                    Result.Y += Y;
                    ++Count;
                }
            }
        }
        pas::free(Buffer);
        HitTestBounds = SavedBounds;
        if (Count < 1) {
            Result = ClassesImports::Point(0, 0);
        } else {
            std::int32_t cpp_arg = pas::idiv(Result.Y, Count);
            std::int32_t cpp_arg_2 = pas::idiv(Result.X, Count);
            Result = ClassesImports::Point(cpp_arg_2, cpp_arg);
        }
        return Result;
    }

    void TgiGI_LoadFromConfigPath(TgiGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TgiGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
    }

    void TgiGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        ImageCache->SetCacheKey(Block->GetParam(u"Image"sv));
        if (Block->CountParams(u"KindX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(pas::view(Block->GetParam(u"KindX"sv))));
        }
        if (Block->CountParams(u"KindY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(pas::view(Block->GetParam(u"KindY"sv))));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(pas::view(Block->GetParam(u"AlignX"sv))));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(pas::view(Block->GetParam(u"AlignY"sv))));
        }
    }

    void TgiGI::SetHardwareMirrorHorizontal(std::uint8_t Value) {
        HardwareMirrorHorizontal = Value;
    }

    void TgiGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        std::int32_t TileIndex{};
        Types::TPoint TileOrigin{};
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            Width = Image->Image->GetContentSize().X;
            Height = Image->Image->GetContentSize().Y;
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
                if (Image->UsesTiledSurfaces) {
                    while (Y < Bottom) {
                        X = Left;
                        while (X < Right) {
                            TileIndex = 0;
                            while (TileIndex < Image->TileCount) {
                                TileOrigin = Image->GetTileOrigin(TileIndex);
                                {
                                    std::int32_t alpha = Alpha;
                                    std::uint8_t hardwareMirrorHorizontal = HardwareMirrorHorizontal;
                                    pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateSurface = (Image->GetOrCreateSurface(TileIndex, cpp_result), cpp_result);
                                    GR_DX::DrawTexture(orCreateSurface, X + TileOrigin.X, Y + TileOrigin.Y, alpha, GR_DX::RgbWhite, &ClipRect, false, hardwareMirrorHorizontal);
                                }
                                ++TileIndex;
                            }
                            X += Width;
                        }
                        Y += Height;
                    }
                } else {
                    while (Y < Bottom) {
                        X = Left;
                        while (X < Right) {
                            {
                                std::int32_t alpha_2 = Alpha;
                                std::uint8_t hardwareMirrorHorizontal_2 = HardwareMirrorHorizontal;
                                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateSurface_2 = (Image->GetOrCreateSurface(0, cpp_result_2), cpp_result_2);
                                GR_DX::DrawTexture(orCreateSurface_2, X, Y, alpha_2, GR_DX::RgbWhite, &ClipRect, false, hardwareMirrorHorizontal_2);
                            }
                            X += Width;
                        }
                        Y += Height;
                    }
                }
            } else {
                Y = Top;
                while (Y < Bottom) {
                    X = Left;
                    while (X < Right) {
                        Image->Image->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, X, Y, ClipRect, 0, Alpha);
                        X += Width;
                    }
                    Y += Height;
                }
            }
        }
    }

    void TgiGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TgiGI::p_destroy() {
        GI_GI::TgiGI_Destroy(this);
    }

    void TgiGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_GI::TgiGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_GI
