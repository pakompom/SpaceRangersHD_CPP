#include "layout/GI_InfiniteImage.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/GI_InfiniteImage.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"

namespace GI_InfiniteImage {
    void TInfiniteImageGI_Create(TInfiniteImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
    }

    void TInfiniteImageGI_Destroy(TInfiniteImageGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Resets size to two billion pixels on each axis and centers the origin.
    void TInfiniteImageGI::SetImagePath(pas::WideString Path) {
        SetSize(ClassesImports::Point(2000000000, 2000000000));
        SetOrigin(ClassesImports::Point(ClientSize.X / 2, ClientSize.Y / 2));
        ImageCache->SetCacheKey(Path);
    }

    void TInfiniteImageGI_LoadFromConfigPath(TInfiniteImageGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TInfiniteImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
    }

    void TInfiniteImageGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        SetSize(ClassesImports::Point(2000000000, 2000000000));
        SetOrigin(ClassesImports::Point(ClientSize.X / 2, ClientSize.Y / 2));
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            SetImagePath(Block->GetParam(u"Image"_wref.get()));
        }
    }

    // The hardware drawing path is unimplemented.
    void TInfiniteImageGI::Draw(Types::TRect ClipRect) {
        std::int32_t StartY{};
        std::int32_t StartX{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t X{};
        std::int32_t Y{};
        EC_CacheBitmap::TCBitmapEC* Image = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            Width = Image->Bitmap->Width;
            Height = Image->Bitmap->Height;
            StartX = MathImports::Floor(pas::real_divide(ClipRect.Left - AbsolutePosition.X, Width)) * Width + AbsolutePosition.X;
            StartY = MathImports::Floor(pas::real_divide(ClipRect.Top - AbsolutePosition.Y, Height)) * Height + AbsolutePosition.Y;
            if (GlobalsV::HardwareRenderingEnabled) {
                Y = StartY;
                while (Y < ClipRect.Bottom) {
                    X = StartX;
                    while (X < ClipRect.Right) {
                        // This native path only logs; it does not draw a hardware tile.
                        GR_Main::AppendLogLineThreadSafe("InfiniteImageDraw"_a);
                        X += Width;
                    }
                    Y += Height;
                }
            } else {
                Y = StartY;
                while (Y < ClipRect.Bottom) {
                    X = StartX;
                    while (X < ClipRect.Right) {
                        {
                            GR_GraphBuf::TGraphBufGR* bitmap = Image->Bitmap;
                            void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                            std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                            GR_Main::CopyGraphBuffer16Clipped(pixels, pitchBytes, X, Y, bitmap, ClipRect, false, false);
                        }
                        X += Width;
                    }
                    Y += Height;
                }
            }
        }
    }

    void TInfiniteImageGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TInfiniteImageGI::p_destroy() {
        GI_InfiniteImage::TInfiniteImageGI_Destroy(this);
    }

    void TInfiniteImageGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_InfiniteImage::TInfiniteImageGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_InfiniteImage
