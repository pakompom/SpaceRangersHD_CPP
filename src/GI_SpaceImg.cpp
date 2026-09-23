#include "layout/GI_SpaceImg.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Cache.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_SpaceImg.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/aMyFunction.hpp"

namespace GI_SpaceImg {
    void TSpaceImgGI_Create(TSpaceImgGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ViewDirty = true;
    }

    void TSpaceImgGI_Destroy(TSpaceImgGI* Self) {
        if (Self->AnimationTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->AnimationTimer);
            Self->AnimationTimer = nullptr;
        }
        Self->ClearImages();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TSpaceImgGI::ClearImages() {
        if (Images != nullptr) {
            EC_Mem::FreeEC(Images);
            Images = nullptr;
        }
        ImageCount = 0;
    }

    // Inserts in descending depth order; reallocates and invalidates earlier pointers.
    PSpaceImageGI TSpaceImgGI::AllocateImage(float Depth) {
        ++ImageCount;
        Images = static_cast<PSpaceImageGI>(EC_Mem::ReAllocREC(Images, ImageCount * static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
        PSpaceImageGI Image = Images;
        std::int32_t I = 0;
        while (ImageCount - 1 > I) {
            if (Depth > Image->Depth) {
                break;
            }
            Image = static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Image, static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
            ++I;
        }
        std::int32_t J = ImageCount - 1;
        while (J > I) {
            Windows::CopyMemory(EC_Mem::AddPointerOffset(Images, J * static_cast<std::int32_t>(sizeof(TSpaceImageGI))), EC_Mem::AddPointerOffset(Images, (J - 1) * static_cast<std::int32_t>(sizeof(TSpaceImageGI))), static_cast<std::int32_t>(sizeof(TSpaceImageGI)));
            --J;
        }
        return static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Images, I * static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
    }

    PSpaceImageGI TSpaceImgGI::AddImage(std::int32_t TemplateIndex, float X, float Y, float Depth) {
        PSpaceImageGI Image = AllocateImage(Depth);
        Image->TemplateIndex = pas::imod(TemplateIndex, GlobalsV::SpaceImageTemplates.length() - 1 + 1);
        Image->X = X;
        Image->Y = Y;
        Image->Depth = Depth;
        Image->InverseDepth = pas::real_divide(1.0L, Depth);
        EC_CacheGAI::TCGaiEC* Data = EC_CacheGAI::AcquireCachedGai(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CacheControl));
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CacheControl)->Release();
            };
            pas::store_unaligned<WindowsSdk::TPoint>(&Image->ImageSize, Data->GetCanvasSize());
            pas::store_unaligned<WindowsSdk::TPoint>(&Image->ImageOffset, EC_Struct::HalfPoint(Image->ImageSize));
            Image->FrameIndex = aMyFunction::RandomIntRange(0, Data->GetSequenceFrameCount(0) - 1);
            Image->FrameTicks = System::Round(pas::real_divide(Data->GetSequenceFrameDelay(0, Image->FrameIndex), 1.0E+1L));
        }
        pas::store_unaligned<EC_Struct::TVector3D>(&Image->Unknown38, EC_Struct::MakeVector3D(0.0, 0.0, 0.0));
        Image->SavedTemplateIndex = 0;
        Image->OrbitAngleRadians = 0.0;
        Image->OrbitRadius = 0.0;
        pas::store_unaligned<EC_Struct::TVector3D>(&Image->OrbitCenter, EC_Struct::MakeVector3D(0.0, 0.0, 0.0));
        Image->OrbitStepDegrees = 0.0;
        ViewDirty = true;
        return Image;
    }

    float TSpaceImgGI::NearestImageDistance(float X, float Y) {
        std::int32_t I{};
        float DistanceSquared{};
        float Result = 1.0E+10f;
        PSpaceImageGI Image = Images;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ImageCount - 1); cpp_range.next(I); ) {
            DistanceSquared = pas::sqr(static_cast<long double>(X) - Image->X) + pas::sqr(static_cast<long double>(Y) - Image->Y);
            if (DistanceSquared < Result) {
                Result = DistanceSquared;
            }
            Image = static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Image, static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
        }
        return System::Sqrt(Result);
    }

    void TSpaceImgGI::UpdateImageOrbitAndFrame(PSpaceImageGI Image) {
        Image->OrbitRadius = System::Sqrt(pas::sqr(static_cast<long double>(Image->X) - Image->OrbitCenter.X) + pas::sqr(static_cast<long double>(Image->Y) - Image->OrbitCenter.Y));
        if (Image->OrbitRadius == 0.0L) {
            Image->OrbitAngleRadians = 0.0;
        } else {
            Image->OrbitAngleRadians = Math::ArcTan2(static_cast<long double>(Image->X) - Image->OrbitCenter.X, -(static_cast<long double>(Image->Y) - Image->OrbitCenter.Y));
        }
        EC_CacheGAI::TCGaiEC* Data = EC_CacheGAI::AcquireCachedGai(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CacheControl));
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CacheControl)->Release();
            };
            pas::store_unaligned<WindowsSdk::TPoint>(&Image->ImageSize, Data->GetCanvasSize());
            pas::store_unaligned<WindowsSdk::TPoint>(&Image->ImageOffset, EC_Struct::HalfPoint(Image->ImageSize));
            Image->FrameIndex = pas::imod(Image->FrameIndex, Data->GetSequenceFrameCount(0));
            Image->FrameTicks = System::Round(pas::real_divide(Data->GetSequenceFrameDelay(0, Image->FrameIndex), 1.0E+1L));
        }
    }

    void TSpaceImgGI::ProjectImages() {
        std::int32_t I{};
        if (ImageCount < 1) {
            return;
        }
        PSpaceImageGI Image = Images;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ImageCount - 1); cpp_range.next(I); ) {
            Image->PixelPosition.X = AbsolutePosition.X + static_cast<std::int32_t>(System::Round((static_cast<long double>(Image->X) - ViewPosition.X) * Image->InverseDepth));
            Image->PixelPosition.Y = AbsolutePosition.Y + static_cast<std::int32_t>(System::Round((static_cast<long double>(Image->Y) - ViewPosition.Y) * Image->InverseDepth));
            Image = static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Image, static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
        }
        ViewDirty = false;
    }

    PSpaceImageGI TSpaceImgGI::GetImage(std::int32_t Index) {
        return static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Images, Index * static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
    }

    void TSpaceImgGI::AnimateImages(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        EC_CacheGAI::TCGaiEC* Data{};
        PSpaceImageGI Image = Images;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ImageCount - 1); cpp_range.next(I); ) {
            --Image->FrameTicks;
            if (Image->FrameTicks <= 0) {
                Data = EC_CacheGAI::AcquireCachedGai(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CacheControl));
                {
                    pas::ScopeExit cpp_cleanup = [&]() noexcept {
                        reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CacheControl)->Release();
                    };
                    ++Image->FrameIndex;
                    if (Data->GetSequenceFrameCount(0) <= Image->FrameIndex) {
                        Image->FrameIndex = 0;
                    }
                    Image->FrameTicks = System::Round(pas::real_divide(Data->GetSequenceFrameDelay(0, Image->FrameIndex), 1.0E+1L));
                }
            }
            if (Image->OrbitRadius != 0.0L && Image->OrbitStepDegrees != 0.0L) {
                Image->OrbitAngleRadians = pas::constant(aMyFunction::GamePi / 1.8E+2L) * Image->OrbitStepDegrees + Image->OrbitAngleRadians;
                Image->X = System::Sin(Image->OrbitAngleRadians) * Image->OrbitRadius + Image->OrbitCenter.X;
                Image->Y = Image->OrbitCenter.Y - System::Cos(Image->OrbitAngleRadians) * Image->OrbitRadius;
            }
            Image = static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Image, static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
        }
        ProjectImages();
    }

    void TSpaceImgGI::SetViewPosition(EC_Struct::TPointF Position) {
        if (ViewPosition.X != Position.X || ViewPosition.Y != Position.Y) {
            Invalidate();
            ViewPosition = Position;
            ViewDirty = true;
            ProjectImages();
            Invalidate();
        }
    }

    void TSpaceImgGI::Invalidate() {
        std::int32_t I{};
        WindowsSdk::TRect Bounds{};
        PSpaceImageGI Image = Images;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ImageCount - 1); cpp_range.next(I); ) {
            Bounds.Left = Image->PixelPosition.X + Image->ImageOffset.X;
            Bounds.Top = Image->PixelPosition.Y + Image->ImageOffset.Y;
            Bounds.Right = Bounds.Left + Image->ImageSize.X;
            Bounds.Bottom = Bounds.Top + Image->ImageSize.Y;
            MessageLoop->QueueUpdateRect(Bounds);
            Image = static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Image, static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
        }
    }

    void TSpaceImgGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        AnimationTimer = MessageLoop->ScheduleCallbackTimer(10, 10, pas::bind_method<&TSpaceImgGI::AnimateImages>(this), 0);
    }

    void TSpaceImgGI::OnDeactivate() {
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        GI_MessageLoop::TObjectGI::OnDeactivate();
    }

    void TSpaceImgGI_LoadFromConfigPath(TSpaceImgGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        TSpaceImgGI::LoadSpaceImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TSpaceImgGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TSpaceImgGI::LoadSpaceImageProperties(Block);
    }

    // Empty in native code.
    void TSpaceImgGI::LoadSpaceImageProperties(EC_BlockPar::TBlockParEC* Block) {
    }

    // Empty in native code.
    void TSpaceImgGI::UpdateAutoGeometry() {
    }

    void TSpaceImgGI::Draw(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        PSpaceImageGI Image{};
        std::int32_t I{};
        EC_CacheGAI::TCGaiEC* Data{};
        GR_gi::TgiGR* Frame{};
        WindowsSdk::TPoint Origin{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect Intersection{};
        {
            const std::int32_t cpp_last = GlobalsV::SpaceImageTemplates.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    GlobalsV::SpaceImageTemplates[I].CachedData = nullptr;
                }
            }
        }
        {
            std::exception_ptr cpp_error{};
            try {
                {
                    const std::int32_t cpp_last_2 = GlobalsV::SpaceImageTemplates.length() - 1;
                    if (0 <= cpp_last_2) {
                        for (I = 0; I <= cpp_last_2; ++I) {
                            GlobalsV::SpaceImageTemplates[I].CachedData = EC_CacheGAI::AcquireCachedGai(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[I].CacheControl));
                        }
                    }
                }
                Image = Images;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, ImageCount - 1); cpp_range.next(I); ) {
                    Bounds.Left = Image->PixelPosition.X + Image->ImageOffset.X;
                    Bounds.Top = Image->PixelPosition.Y + Image->ImageOffset.Y;
                    Bounds.Right = Bounds.Left + Image->ImageSize.X;
                    Bounds.Bottom = Bounds.Top + Image->ImageSize.Y;
                    if (EC_Struct::IntersectRects(Intersection, Bounds, ClipRect)) {
                        Data = reinterpret_cast<EC_CacheGAI::TCGaiEC*>(GlobalsV::SpaceImageTemplates[Image->TemplateIndex].CachedData);
                        Frame = Data->LoadFrameGi(Data->GetSequenceFrameIndex(0, Image->FrameIndex));
                        if (GlobalsV::HardwareRenderingEnabled) {
                            Data->GetOrCreateFrameSurface(Data->GetSequenceFrameIndex(0, Image->FrameIndex), cpp_result), cpp_result;
                            Origin = Data->GetFrameOrigin(Data->GetSequenceFrameIndex(0, Image->FrameIndex));
                            {
                                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateFrameSurface = (Data->GetOrCreateFrameSurface(Data->GetSequenceFrameIndex(0, Image->FrameIndex), cpp_result_2), cpp_result_2);
                                std::int32_t cpp_arg = Bounds.Top + Origin.Y;
                                std::int32_t cpp_arg_2 = Bounds.Left + Origin.X;
                                GR_DX::DrawTexture(orCreateFrameSurface, cpp_arg_2, cpp_arg, 255, GR_DX::RgbWhite, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                            }
                        } else {
                            Frame->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, Bounds.Left + Frame->GetBoundsRect().Left - Data->GetBoundsRect().Left, Bounds.Top + Frame->GetBoundsRect().Top - Data->GetBoundsRect().Top, ClipRect, 0, 255);
                        }
                    }
                    Image = static_cast<PSpaceImageGI>(EC_Mem::AddPointerOffset(Image, static_cast<std::int32_t>(sizeof(TSpaceImageGI))));
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            {
                const std::int32_t cpp_last_3 = GlobalsV::SpaceImageTemplates.length() - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        if (reinterpret_cast<EC_CacheGAI::TCGaiEC*>(GlobalsV::SpaceImageTemplates[I].CachedData) != nullptr) {
                            reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[I].CacheControl)->Release();
                            GlobalsV::SpaceImageTemplates[I].CachedData = nullptr;
                        }
                    }
                }
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TSpaceImgGI::p_destroy() {
        GI_SpaceImg::TSpaceImgGI_Destroy(this);
    }

    void TSpaceImgGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_SpaceImg::TSpaceImgGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_SpaceImg
