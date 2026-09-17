#include "layout/GI_RotateImage5.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_DX.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheHSAI.hpp"
#include "units/EC_CacheRotateBuf.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_RotateImage5.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_GraphBufPal.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_RotateImage5 {
    std::int32_t RotateImageConstructionStage = -1;

    void TRotateImage5GI_Create(TRotateImage5GI* Self, GI_MessageLoop::TObjectGI* Owner) {
        RotateImageConstructionStage = 0;
        try {
            GI_MessageLoop::TObjectGI_Create(Self, Owner);
            Self->ImageDirty = true;
            RotateImageConstructionStage = 1;
            Self->ImageCache = pas::construct_call<EC_CacheHSAI::TCHSAIControlEC>(EC_Cache::TCacheControlEC_Create);
            RotateImageConstructionStage = 2;
            EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
            RotateImageConstructionStage = 3;
            Self->RotationCache = pas::construct_call<EC_CacheRotateBuf::TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
            RotateImageConstructionStage = 4;
            EC_Cache::TCacheEC::ResetControl(Self->RotationCache);
            RotateImageConstructionStage = 5;
            Self->RotatedImage = pas::construct_call<GR_GraphBufPal::TGraphBufPalGR>(GR_GraphBufPal::TGraphBufPalGR_Create);
            RotateImageConstructionStage = 6;
            Self->RenderedAngle = 0;
            Self->RenderedFrameIndex = 0u;
            Self->Angle = 0;
            Self->FrameIndex = 0u;
            Self->Alpha = 255;
            Self->ImageDirty = true;
            Self->Unknown13C = nullptr;
            Self->FrameTexture = nullptr;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"self=", SysUtils::Int64ToStr(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self)))}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"FGBC=", SysUtils::Int64ToStr(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->ImageCache)))}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"FRBC=", SysUtils::Int64ToStr(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->RotationCache)))}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"FIR=", SysUtils::Int64ToStr(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->RotatedImage)))}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TRotateImage5GI.Create, label = ", SysUtils::IntToStr(RotateImageConstructionStage)})));
            } else {
                throw;
            }
        }
        RotateImageConstructionStage = -1;
    }

    void TRotateImage5GI_Destroy(TRotateImage5GI* Self) {
        if (Self->Unknown13C != nullptr) {
            pas::free(Self->Unknown13C);
            Self->Unknown13C = nullptr;
        }
        Self->FrameTexture = nullptr;
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        pas::free(Self->RotationCache);
        Self->RotationCache = nullptr;
        pas::free(Self->RotatedImage);
        Self->RotatedImage = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves cache keys, image storage and FrameTexture.
    void TRotateImage5GI::Clear() {
        ImageDirty = true;
        RenderedAngle = 255;
        Angle = 0;
        RenderedFrameIndex = 0u;
        FrameIndex = 0u;
        Alpha = 255;
        GI_MessageLoop::TObjectGI::Clear();
    }

    // A full turn has 256 steps.
    void TRotateImage5GI::SetAngle(std::uint8_t Value) {
        // Preserve the native byte comparison operand order.
        if (static_cast<std::uint8_t>(Value + 0) != Angle) {
            Angle = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    // Does not validate against the frame count.
    void TRotateImage5GI::SetFrameIndex(std::uint32_t Value) {
        if (Value + 0 != FrameIndex) {
            FrameIndex = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    void TRotateImage5GI::SetAlpha(std::uint8_t Value) {
        if (static_cast<std::uint8_t>(Value + 0) != Alpha) {
            Alpha = Value;
            ImageDirty = true;
            Invalidate();
        }
    }

    // Replaces size and origin with a centered square enclosing all rotations.
    void TRotateImage5GI::SetImage(pas::WideString Path, Types::TPoint ImageSize, Types::TPoint Pivot) {
        EC_CacheHSAI::TCHSAIEC* Data{};
        double Radius{};
        std::int32_t I{};
        float Radians{};
        float C{};
        float S{};
        float LeftX{};
        float TopY{};
        float RightX{};
        float BottomY{};
        float CenterX{};
        float CenterY{};
        ImageCache->SetCacheKey(Path);
        Data = EC_CacheHSAI::AcquireCachedHSAI(ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                ImageCache->Release();
            };
            try {
                RotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(ImageSize.X), ",", SysUtils::IntToStr(ImageSize.Y), ",", SysUtils::IntToStr(Data->Width), ",", SysUtils::IntToStr(Data->Height), ",", SysUtils::IntToStr(Pivot.X), ",", SysUtils::IntToStr(Pivot.Y)})));
            } catch (...) {
                pas::raise(pas::make_exception<pas::Exception>("Error in TRotateImage5GI.SetImage"_a));
            }
            Radius = pas::sqr(Pivot.X - 0) + pas::sqr(Pivot.Y - 0);
            Radius = pas::real_max<double>(Radius, static_cast<double>(pas::sqr(Pivot.X - ImageSize.X) + pas::sqr(Pivot.Y - ImageSize.Y)));
            Radius = pas::real_max<double>(Radius, static_cast<double>(pas::sqr(Pivot.X - ImageSize.X) + pas::sqr(Pivot.Y - 0)));
            Radius = pas::real_max<double>(Radius, static_cast<double>(pas::sqr(Pivot.X - 0) + pas::sqr(Pivot.Y - ImageSize.Y)));
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
            LeftX = -Pivot.X;
            TopY = -Pivot.Y;
            RightX = ImageSize.X - Pivot.X - 1;
            BottomY = ImageSize.Y - Pivot.Y - 1;
            CenterX = pas::real_divide(ClientSize.X, 2.0L);
            CenterY = pas::real_divide(ClientSize.Y, 2.0L);
            for (I = 0; I <= 255; ++I) {
                Radians = pas::real_divide(pas::real_divide(I, 256.0L) * 3.6E+2L * 3.1415926L, 1.8E+2L);
                C = System::Cos(Radians);
                S = System::Sin(Radians);
                TopLeftX[I] = static_cast<long double>(System::Trunc(static_cast<long double>(LeftX) * C - static_cast<long double>(TopY) * S)) + CenterX;
                TopLeftY[I] = static_cast<long double>(System::Trunc(static_cast<long double>(LeftX) * S + static_cast<long double>(TopY) * C)) + CenterY;
                TopRightX[I] = static_cast<long double>(System::Trunc(static_cast<long double>(RightX) * C - static_cast<long double>(TopY) * S)) + CenterX;
                TopRightY[I] = static_cast<long double>(System::Trunc(static_cast<long double>(RightX) * S + static_cast<long double>(TopY) * C)) + CenterY;
                BottomRightX[I] = static_cast<long double>(System::Trunc(static_cast<long double>(RightX) * C - static_cast<long double>(BottomY) * S)) + CenterX;
                BottomRightY[I] = static_cast<long double>(System::Trunc(static_cast<long double>(RightX) * S + static_cast<long double>(BottomY) * C)) + CenterY;
                BottomLeftX[I] = static_cast<long double>(System::Trunc(static_cast<long double>(LeftX) * C - static_cast<long double>(BottomY) * S)) + CenterX;
                BottomLeftY[I] = static_cast<long double>(System::Trunc(static_cast<long double>(LeftX) * S + static_cast<long double>(BottomY) * C)) + CenterY;
            }
            ImageDirty = true;
        }
        Invalidate();
    }

    // Uses the last rendered image. Alpha must exceed 8 in software, or 0 in hardware.
    std::uint8_t TRotateImage5GI::HitTestPixel(Types::TPoint Point) {
        Direct3D9::IDirect3DSurface9 Surface{};
        Direct3D9::IDirect3DSurface9 OldSurface{};
        Direct3D9::IDirect3DTexture9 Texture{};
        Direct3D9::TD3DLockedRect Locked{};
        GR_GraphBuf::TGraphBufGR* Buffer{};
        pas::Array<GR_DX::TScreenVertexGR, 0, 3> Quad{};
        std::uint8_t Result = false;
        if (ContainsPoint(Point)) {
            if (GlobalsV::HardwareRenderingEnabled) {
                if (FrameTexture != nullptr) {
                    Quad[0] = Vertices[0];
                    Quad[1] = Vertices[1];
                    Quad[2] = Vertices[2];
                    Quad[3] = Vertices[3];
                    Quad[0].X = TopLeftX[RenderedAngle];
                    Quad[0].Y = TopLeftY[RenderedAngle];
                    Quad[1].X = TopRightX[RenderedAngle];
                    Quad[1].Y = TopRightY[RenderedAngle];
                    Quad[2].X = BottomRightX[RenderedAngle];
                    Quad[2].Y = BottomRightY[RenderedAngle];
                    Quad[3].X = BottomLeftX[RenderedAngle];
                    Quad[3].Y = BottomLeftY[RenderedAngle];
                    {
                        Direct3D9::IDirect3DSurface9& surface = pas::out_interface(Surface);
                        std::uint32_t y = ClientSize.Y;
                        std::uint32_t x = ClientSize.X;
                        pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice = GR_Main::Direct3DDevice;
                        Direct3D9::IDirect3DDevice9_CreateRenderTarget(direct3DDevice, x, y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DMULTISAMPLE_NONE, 0u, 0, surface, nullptr);
                    }
                    {
                        Direct3D9::IDirect3DTexture9& texture = pas::out_interface(Texture);
                        std::uint32_t y_2 = ClientSize.Y;
                        std::uint32_t x_2 = ClientSize.X;
                        pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_2 = GR_Main::Direct3DDevice;
                        Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice_2, x_2, y_2, 1u, 0u, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_SYSTEMMEM, texture, nullptr);
                    }
                    if (Surface != nullptr) {
                        if (Texture == nullptr) {
                            Surface = nullptr;
                        } else {
                            {
                                Direct3D9::IDirect3DSurface9& oldSurface = pas::out_interface(OldSurface);
                                pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_3 = GR_Main::Direct3DDevice;
                                Direct3D9::IDirect3DDevice9_GetRenderTarget(direct3DDevice_3, 0u, oldSurface);
                            }
                            Direct3D9::IDirect3DDevice9_SetRenderTarget(GR_Main::Direct3DDevice, 0u, Surface);
                            Direct3D9::IDirect3DDevice9_SetTextureStageState(GR_Main::Direct3DDevice, 0u, Direct3D9::D3DTSS_ALPHAOP, Direct3D9::D3DTOP_MODULATE);
                            Direct3D9::IDirect3DDevice9_SetSamplerState(GR_Main::Direct3DDevice, 0u, Direct3D9::D3DSAMP_MINFILTER, Direct3D9::D3DTEXF_LINEAR);
                            Direct3D9::IDirect3DDevice9_SetSamplerState(GR_Main::Direct3DDevice, 0u, Direct3D9::D3DSAMP_MAGFILTER, Direct3D9::D3DTEXF_LINEAR);
                            Direct3D9::IDirect3DDevice9_Clear(GR_Main::Direct3DDevice, 0u, nullptr, Direct3D9::D3DCLEAR_TARGET, 0u, 1.0f, 0u);
                            Direct3D9::IDirect3DDevice9_SetTexture(GR_Main::Direct3DDevice, 0u, FrameTexture);
                            Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLEFAN, 2u, &Quad, static_cast<std::int32_t>(sizeof(GR_DX::TScreenVertexGR)));
                            Direct3D9::IDirect3DDevice9_SetTexture(GR_Main::Direct3DDevice, 0u, nullptr);
                            Direct3D9::IDirect3DDevice9_SetRenderTarget(GR_Main::Direct3DDevice, 0u, OldSurface);
                            {
                                Direct3D9::IDirect3DSurface9& oldSurface_2 = pas::out_interface(OldSurface);
                                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> texture_2 = Texture;
                                Direct3D9::IDirect3DTexture9_GetSurfaceLevel(texture_2, 0u, oldSurface_2);
                            }
                            if (Direct3D9::IDirect3DDevice9_GetRenderTargetData(GR_Main::Direct3DDevice, Surface, OldSurface) != 0) {
                                GR_Main::AppendLogLineThreadSafe("GetRenderTargetData fail"_a);
                            }
                            OldSurface = nullptr;
                            Surface = nullptr;
                            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                            if (Locked.Bits != nullptr) {
                                Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                                Buffer->AttachPixels(ClientSize.X, ClientSize.Y, Locked.Pitch, Locked.Bits);
                                Result = Buffer->GetPixel32(Point.X - HitTestBounds.Left, Point.Y - HitTestBounds.Top) > 0x00ffffff;
                                pas::free(Buffer);
                            }
                            Texture = nullptr;
                        }
                    }
                }
                return Result;
            }
            return static_cast<std::uint8_t>(RotatedImage->GetPaletteColor(RotatedImage->GetPixelIndex(Point.X - HitTestBounds.Left, Point.Y - HitTestBounds.Top)) >> 24) > 8;
        }
        return Result;
    }

    void TRotateImage5GI_LoadFromConfigPath(TRotateImage5GI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TRotateImage5GI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        RenderedAngle = 255;
        Angle = 0;
        Alpha = 255;
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
        ImageDirty = true;
    }

    void TRotateImage5GI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            if (Block->CountParams(u"Size"_wref.get()) > 0) {
                if (Block->CountParams(u"Sme"_wref.get()) > 0) {
                    Types::TPoint pointGI = GI_Main::GetPointGI(Block->GetParam(u"Sme"_wref.get()));
                    Types::TPoint pointGI_2 = GI_Main::GetPointGI(Block->GetParam(u"Size"_wref.get()));
                    pas::WideString param = Block->GetParam(u"Image"_wref.get());
                    SetImage(std::move(param), pointGI_2, pointGI);
                }
            }
        }
        if (Block->CountParams(u"Angle"_wref.get()) > 0) {
            SetAngle(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Angle"_wref.get()))));
        }
        if (Block->CountParams(u"Trans"_wref.get()) > 0) {
            SetAlpha(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Trans"_wref.get()))));
        }
    }

    void TRotateImage5GI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        EC_CacheHSAI::TCHSAIEC* Data{};
        EC_CacheRotateBuf::TCRotateBufEC* Rotation{};
        Types::TRect OldClip{};
        if (HitTestBounds.Right < 0 || HitTestBounds.Left > GR_Main::GameScreenWidth || HitTestBounds.Bottom < 0 || HitTestBounds.Top > GR_Main::GameScreenHeight) {
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
                        Data = EC_CacheHSAI::AcquireCachedHSAI(ImageCache);
                        FrameTexture = (Data->GetOrCreateFrameSurface(RenderedFrameIndex, cpp_result), cpp_result);
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
            Vertices[0].Color = static_cast<std::uint32_t>(Alpha) << 24 | 0x00ffffff;
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
            Vertices[0].X = static_cast<long double>(HitTestBounds.Left) + TopLeftX[Angle];
            Vertices[0].Y = static_cast<long double>(HitTestBounds.Top) + TopLeftY[Angle];
            Vertices[1].X = static_cast<long double>(HitTestBounds.Left) + TopRightX[Angle];
            Vertices[1].Y = static_cast<long double>(HitTestBounds.Top) + TopRightY[Angle];
            Vertices[2].X = static_cast<long double>(HitTestBounds.Left) + BottomRightX[Angle];
            Vertices[2].Y = static_cast<long double>(HitTestBounds.Top) + BottomRightY[Angle];
            Vertices[3].X = static_cast<long double>(HitTestBounds.Left) + BottomLeftX[Angle];
            Vertices[3].Y = static_cast<long double>(HitTestBounds.Top) + BottomLeftY[Angle];
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
                        Data = EC_CacheHSAI::AcquireCachedHSAI(ImageCache);
                        Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(RotationCache);
                        RotatedImage->FillPixels(EC_Mem::ReadByteEC(Data->GetFrameIndexPlane(RenderedFrameIndex)));
                        RotatedImage->SetPalette(Data->GetFramePalette(RenderedFrameIndex), 256);
                        GR_Main::Ex_OKGR_RotateBuf_Draw_BYTE(RotatedImage->Pixels, RotatedImage->PitchBytes, Data->GetFrameIndexPlane(RenderedFrameIndex), Data->GetSourcePitchBytes(), OriginPoint.X, OriginPoint.Y, Angle, Rotation->Buffer);
                        if (Alpha != 255) {
                            GR_Main::Ex_OKGR_Light_BYTE(EC_Mem::AddPointerOffset(RotatedImage->Palette, 3), 4, 1024, 256, 1, Alpha);
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

    std::uint32_t TRotateImage5GI::GetFrameCount() {
        std::uint32_t Result{};
        EC_CacheHSAI::TCHSAIEC* Data = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Data = EC_CacheHSAI::AcquireCachedHSAI(ImageCache);
                Result = Data->GetFrameCount();
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
        return Result;
    }

    // Queues an arbitrary HSAI path; does not change this object's image.
    void TRotateImage5GI::QueueImagePath(pas::List* PendingLoads, pas::WideString Path) {
        EC_CacheHSAI::TCHSAIControlEC* Control = pas::construct_call<EC_CacheHSAI::TCHSAIControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Control);
        Control->SetCacheKey(Path);
        Control->QueueLoadIfMissing(PendingLoads);
        pas::free(Control);
    }

    void TRotateImage5GI::p_destroy() {
        GI_RotateImage5::TRotateImage5GI_Destroy(this);
    }

    void TRotateImage5GI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_RotateImage5::TRotateImage5GI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_RotateImage5
