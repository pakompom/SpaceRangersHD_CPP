#include "layout/GI_RotateImageGAI.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_DX.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aMyFunction.hpp"
#include "units/ClassesImports.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_CacheRotateBuf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_RotateImageGAI.hpp"
#include "units/GR_GraphBufPal.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_RotateImageGAI {
    void TRotateImageGaiGI_Create(TRotateImageGaiGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->RotationCache = pas::construct_call<EC_CacheRotateBuf::TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->RotationCache);
        Self->RotatedImage = pas::construct_call<GR_GraphBufPal::TGraphBufPalGR>(GR_GraphBufPal::TGraphBufPalGR_Create);
        Self->RenderedAngle = 0;
        Self->Angle = 0;
        Self->Alpha = 255;
        Self->ImageDirty = true;
        Self->AnimationIndex = -1;
        Self->FrameTexture = nullptr;
    }

    void TRotateImageGaiGI_Destroy(TRotateImageGaiGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        pas::free(Self->RotationCache);
        Self->RotationCache = nullptr;
        pas::free(Self->RotatedImage);
        Self->RotatedImage = nullptr;
        Self->FrameTexture = nullptr;
        Self->ClearFrameSequence();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TRotateImageGaiGI::Clear() {
        RenderedFrameIndex = 0;
        FrameIndex = 0;
        ImageDirty = true;
        RenderedAngle = 255;
        Angle = 0;
        Alpha = 255;
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TRotateImageGaiGI::SetAngle(std::uint8_t Value) {
        if (Value != Angle) {
            Angle = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    void TRotateImageGaiGI::SetAlpha(std::uint8_t Value) {
        if (Value != Alpha) {
            Alpha = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    void TRotateImageGaiGI::SetImage(pas::WideString Path, Types::TPoint ImageSize, Types::TPoint Pivot) {
        EC_CacheGAI::TCGaiEC* Data{};
        double Radius{};
        RenderedFrameIndex = 0;
        ImageCache->SetCacheKey(Path);
        Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            try {
                RotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(ImageSize.X), ",", SysUtils::IntToStr(ImageSize.Y), ",", SysUtils::IntToStr(Data->GetCanvasSize().X), ",", SysUtils::IntToStr(Data->GetCanvasSize().Y), ",", SysUtils::IntToStr(Pivot.X), ",", SysUtils::IntToStr(Pivot.Y)})));
            } catch (...) {
                pas::raise(pas::make_exception<pas::Exception>("Error in TRotateImageGaiGI.SetImage"_a));
            }
            this->ImageSize = ImageSize;
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
                RotatedImage->AllocateBuffer(ClientSize.X, ClientSize.Y, 256, ClientSize.X);
            }
            ImageDirty = true;
        }
        Invalidate();
    }

    void TRotateImageGaiGI::SetFrame(std::int32_t Value) {
        if (Value != FrameIndex) {
            FrameIndex = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    void TRotateImageGaiGI::ClearFrameSequence() {
        if (FrameIndexTable != nullptr) {
            EC_Mem::FreeFromHeapEC(GI_GAI::GaiFrameHeap, FrameIndexTable);
            FrameIndexTable = nullptr;
        }
        if (FrameDelayTable != nullptr) {
            EC_Mem::FreeFromHeapEC(GI_GAI::GaiFrameHeap, FrameDelayTable);
            FrameDelayTable = nullptr;
        }
        RenderedFrameIndex = 0;
        FrameCount = 0;
    }

    std::int32_t TRotateImageGaiGI::GetFrameSourceIndex(std::int32_t Index) {
        return EC_Mem::ReadIntegerEC(EC_Mem::AddPointerOffset(FrameIndexTable, Index * static_cast<std::int32_t>(sizeof(std::int32_t))));
    }

    void TRotateImageGaiGI_LoadFromConfigPath(TRotateImageGaiGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TRotateImageGaiGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        RenderedAngle = 255;
        Angle = 0;
        Alpha = 255;
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
        ImageDirty = true;
    }

    void TRotateImageGaiGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
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

    // Diagnostic retains TgaiGI.AfterLoad, but this is TRotateImageGaiGI's geometry-update override.
    void TRotateImageGaiGI::UpdateAutoGeometry() {
        EC_CacheGAI::TCGaiEC* Data{};
        if (AnimationIndex >= 0) {
            ClearFrameSequence();
            if (ImageCache != nullptr && ImageCache->CacheKey != u"") {
                Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
                {
                    pas::ScopeExit cpp_cleanup = [&]() noexcept {
                        ImageCache->Release();
                    };
                    if (AnimationIndex < 0 || Data->GetSequenceCount() <= AnimationIndex) {
                        pas::raise(pas::make_exception<pas::Exception>("TgaiGI.AfterLoad. Anim not found."_a));
                    }
                    FrameCount = Data->GetSequenceFrameCount(AnimationIndex);
                    FrameIndexTable = static_cast<System::PInteger>(EC_Mem::ReAllocFromHeapREC(GI_GAI::GaiFrameHeap, FrameIndexTable, FrameCount * static_cast<std::int32_t>(sizeof(std::int32_t))));
                    FrameDelayTable = static_cast<System::PInteger>(EC_Mem::ReAllocFromHeapREC(GI_GAI::GaiFrameHeap, FrameDelayTable, FrameCount * static_cast<std::int32_t>(sizeof(std::int32_t))));
                    Data->FillSequenceFrameIndexTable(AnimationIndex, FrameIndexTable, static_cast<std::int32_t>(sizeof(std::int32_t)));
                    Data->FillSequenceFrameDelayTable(AnimationIndex, FrameDelayTable, static_cast<std::int32_t>(sizeof(std::int32_t)));
                }
            }
        }
    }

    void TRotateImageGaiGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        EC_CacheGAI::TCGaiEC* Data{};
        EC_CacheRotateBuf::TCRotateBufEC* Rotation{};
        GR_gi::TgiGR* FrameGi{};
        GR_gi::PgiPlaneGR IndexPlane{};
        GR_gi::PgiPlaneGR PalettePlane{};
        float Degrees{};
        float C{};
        float S{};
        float LeftX{};
        float RightX{};
        float TopY{};
        float BottomY{};
        float CenterX{};
        float CenterY{};
        Types::TRect OldClip{};
        if (HitTestBounds.Left + ClientSize.X < 0 || HitTestBounds.Left - ClientSize.X / 2 > GR_Main::GameScreenWidth || HitTestBounds.Top + ClientSize.Y < 0 || HitTestBounds.Top - ClientSize.Y / 2 > GR_Main::GameScreenHeight) {
            return;
        }
        if (GlobalsV::HardwareRenderingEnabled) {
            if (RenderedAngle != Angle || ImageDirty == true || FrameIndex != RenderedFrameIndex || FrameTexture == nullptr) {
                ImageDirty = false;
                RenderedAngle = Angle;
                RenderedFrameIndex = FrameIndex;
                Data = nullptr;
                {
                    std::exception_ptr cpp_error{};
                    try {
                        Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
                        FrameTexture = (Data->GetOrCreateFrameSurface(GetFrameSourceIndex(RenderedFrameIndex), cpp_result), cpp_result);
                    } catch (...) {
                        cpp_error = std::current_exception();
                    }
                    if (Data != nullptr) {
                        ImageCache->Release();
                    }
                    if (cpp_error) {
                        std::rethrow_exception(cpp_error);
                    }
                }
            }
            Vertices[0].Color = static_cast<std::uint32_t>(Alpha) << 24 | GR_DX::RgbWhite;
            Vertices[1].Color = Vertices[0].Color;
            Vertices[2].Color = Vertices[0].Color;
            Vertices[3].Color = Vertices[0].Color;
            Vertices[0].Z = 1.0f;
            Vertices[1].Z = 1.0f;
            Vertices[2].Z = 1.0f;
            Vertices[3].Z = 1.0f;
            Vertices[0].RHW = 1.0f;
            Vertices[1].RHW = 1.0f;
            Vertices[2].RHW = 1.0f;
            Vertices[3].RHW = 1.0f;
            Vertices[0].U = 0.0f;
            Vertices[0].V = 0.0f;
            Vertices[1].U = 1.0f;
            Vertices[1].V = 0.0f;
            Vertices[2].U = 1.0f;
            Vertices[2].V = 1.0f;
            Vertices[3].U = 0.0f;
            Vertices[3].V = 1.0f;
            LeftX = pas::real_divide(-ImageSize.X, 2.0L);
            TopY = pas::real_divide(-ImageSize.Y, 2.0L);
            RightX = pas::real_divide(ImageSize.X, 2.0L);
            BottomY = pas::real_divide(ImageSize.Y, 2.0L);
            CenterX = pas::real_divide(ClientSize.X, 2.0L);
            CenterY = pas::real_divide(ClientSize.Y, 2.0L);
            Degrees = pas::real_divide(Angle, 256.0L) * 3.6E+2L;
            C = System::Cos(pas::constant(aMyFunction::GamePi / 1.8E+2L) * Degrees);
            S = System::Sin(pas::constant(aMyFunction::GamePi / 1.8E+2L) * Degrees);
            Vertices[0].X = static_cast<long double>(LeftX) * C - static_cast<long double>(TopY) * S + CenterX + HitTestBounds.Left;
            Vertices[0].Y = static_cast<long double>(LeftX) * S + static_cast<long double>(TopY) * C + CenterY + HitTestBounds.Top;
            Vertices[1].X = static_cast<long double>(RightX) * C - static_cast<long double>(TopY) * S + CenterX + HitTestBounds.Left;
            Vertices[1].Y = static_cast<long double>(RightX) * S + static_cast<long double>(TopY) * C + CenterY + HitTestBounds.Top;
            Vertices[2].X = static_cast<long double>(RightX) * C - static_cast<long double>(BottomY) * S + CenterX + HitTestBounds.Left;
            Vertices[2].Y = static_cast<long double>(RightX) * S + static_cast<long double>(BottomY) * C + CenterY + HitTestBounds.Top;
            Vertices[3].X = static_cast<long double>(LeftX) * C - static_cast<long double>(BottomY) * S + CenterX + HitTestBounds.Left;
            Vertices[3].Y = static_cast<long double>(LeftX) * S + static_cast<long double>(BottomY) * C + CenterY + HitTestBounds.Top;
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, &ClipRect);
            Direct3D9::IDirect3DDevice9_SetTexture(GR_Main::Direct3DDevice, 0u, FrameTexture);
            Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLEFAN, 2u, &Vertices, static_cast<std::int32_t>(sizeof(GR_DX::TScreenVertexGR)));
            Direct3D9::IDirect3DDevice9_SetTexture(GR_Main::Direct3DDevice, 0u, nullptr);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, &OldClip);
        } else {
            if (RenderedAngle != Angle || ImageDirty == true || FrameIndex != RenderedFrameIndex) {
                ImageDirty = false;
                RenderedAngle = Angle;
                RenderedFrameIndex = FrameIndex;
                Data = nullptr;
                Rotation = nullptr;
                {
                    std::exception_ptr cpp_error_2{};
                    try {
                        Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
                        Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(RotationCache);
                        RotatedImage->ClearPixels();
                        FrameGi = Data->LoadFrameGi(GetFrameSourceIndex(RenderedFrameIndex));
                        if (FrameGi->GetFormat() != 4) {
                            GR_Main::RaiseWideMessage(u"rotate GAI 1"_wref.get());
                        }
                        if (FrameGi->GetContentSize().X != Data->GetCanvasSize().X || FrameGi->GetContentSize().Y != Data->GetCanvasSize().Y) {
                            GR_Main::RaiseWideMessage(u"rotate GAI 2"_wref.get());
                        }
                        IndexPlane = FrameGi->GetPlane(0);
                        PalettePlane = FrameGi->GetPlane(1);
                        RotatedImage->SetPalette(reinterpret_cast<GR_GraphBuf::PColorRGBA>(static_cast<std::uint8_t*>(FrameGi->Data) + PalettePlane->DataOffset), static_cast<std::uint32_t>(PalettePlane->DataSize) >> 2);
                        GR_Main::Ex_OKGR_RotateBuf_Draw_BYTE(RotatedImage->Pixels, RotatedImage->PitchBytes, static_cast<std::uint8_t*>(FrameGi->Data) + IndexPlane->DataOffset, FrameGi->GetContentSize().X, OriginPoint.X, OriginPoint.Y, Angle, Rotation->Buffer);
                        // Native software alpha adjustment uses Pixels with a four-byte stride.
                        if (Alpha != 255) {
                            GR_Main::Ex_OKGR_Light_BYTE(EC_Mem::AddPointerOffset(RotatedImage->Pixels, 3), 4, RotatedImage->PitchBytes - RotatedImage->Width * 4, RotatedImage->Width, RotatedImage->Height, Alpha);
                        }
                    } catch (...) {
                        cpp_error_2 = std::current_exception();
                    }
                    if (Data != nullptr) {
                        ImageCache->Release();
                    }
                    if (Rotation != nullptr) {
                        RotationCache->Release();
                    }
                    if (cpp_error_2) {
                        std::rethrow_exception(cpp_error_2);
                    }
                }
            }
            {
                std::int32_t top = HitTestBounds.Top;
                GR_GraphBufPal::TGraphBufPalGR* rotatedImage = RotatedImage;
                WindowsSdk::TRect clipRect = ClipRect;
                void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                std::int32_t left = HitTestBounds.Left;
                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                GR_Main::DrawPaletteAlphaBuffer16Clipped(pixels, pitchBytes, left, top, rotatedImage, clipRect);
            }
        }
    }

    void TRotateImageGaiGI::QueueImageLoad(pas::List* PendingLoads) {
        ImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TRotateImageGaiGI::p_destroy() {
        GI_RotateImageGAI::TRotateImageGaiGI_Destroy(this);
    }

    void TRotateImageGaiGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_RotateImageGAI::TRotateImageGaiGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_RotateImageGAI
