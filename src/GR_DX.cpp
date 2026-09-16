#include "layout/GR_DX.hpp"
#include "types/Types.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/MMSystem.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"

namespace GR_DX {
    double LineFractionDX(double Value);

    double AnimatedLineFractionDX(double Value);

    void AdvanceLinePhase(std::int32_t& Phase);

    std::uint8_t TextureManagerDisabled = false;

    WindowsSdk::TPoint MaxTextureSize{};

    GR_DX::TScreenVerticesGR DrawVertices{};

    pas::DynArray<GR_DX::TScreenVertexGR> PendingPoints{};

    pas::List* TextureCaches = nullptr;

    std::uint32_t AvailableTextureBytes = 0u;

    GR_DX::TCircleTableGR CircleCos{};

    GR_DX::TCircleTableGR CircleSin{};

    GR_DX::TLineAlphaTableGR LineAlphaTable{};

    std::uint32_t ReservedTextureBytes = 0u;

    std::int32_t TextureIdleSeconds = 120;

    std::uint32_t LastTextureEvictionTick = 0u;

    std::int32_t PendingPointCount = 0;

    std::int32_t PendingPointCapacity = 0;

    std::uint32_t ResidentTextureBytes = 0u;

    void EvictTextureCaches(std::uint8_t Force) {
        pas::Object* Texture{};
        std::uint32_t LastUsed{};
        if (TextureCaches == nullptr) {
            return;
        }
        if (GR_Main::Direct3DDevice == nullptr) {
            return;
        }
        std::uint32_t NowTick = MMSystem::timeGetTime();
        if (NowTick - LastTextureEvictionTick < 10 && static_cast<std::uint8_t>(Force ^ 1)) {
            return;
        }
        LastTextureEvictionTick = NowTick;
        std::uint32_t AvailableBytes = std::max<std::int32_t>(0, static_cast<std::int32_t>(([&] {
            std::uint32_t cpp_left = Direct3D9::IDirect3DDevice9_GetAvailableTextureMem(GR_Main::Direct3DDevice);
            return cpp_left - ReservedTextureBytes;
        }())));
        if (ResidentTextureBytes < 0x10000000 && AvailableBytes > 0x01400000 && static_cast<std::uint8_t>(Force ^ 1)) {
            return;
        }
        std::int32_t i = 0;
        while (i < pas::list_count(TextureCaches)) {
            Texture = pas::list_at<pas::Object>(TextureCaches, i);
            if (pas::class_cast_if<TTextureGR*>(Texture) != nullptr) {
                LastUsed = reinterpret_cast<TTextureGR*>(Texture)->LastUseTick;
                if (NowTick - LastUsed > static_cast<std::uint32_t>(TextureIdleSeconds * 1000)) {
                    reinterpret_cast<TTextureGR*>(Texture)->ReleaseSurfaces();
                }
            }
            ++i;
        }
        AvailableBytes = std::max<std::int32_t>(0, static_cast<std::int32_t>(([&] {
            std::uint32_t cpp_left_2 = Direct3D9::IDirect3DDevice9_GetAvailableTextureMem(GR_Main::Direct3DDevice);
            return cpp_left_2 - ReservedTextureBytes;
        }())));
        if (AvailableBytes > 3.145728E+7L && TextureIdleSeconds < 120) {
            TextureIdleSeconds += 10;
        }
        if (ResidentTextureBytes > 0x10000000 || AvailableBytes < 0x01400000) {
            if (TextureIdleSeconds > 20) {
                TextureIdleSeconds -= 10;
            }
        }
    }

    void SubtractResidentTextureBytes(std::uint32_t ByteCount) {
        if (ResidentTextureBytes > ByteCount) {
            ResidentTextureBytes -= ByteCount;
            return;
        }
        ResidentTextureBytes = 0u;
    }

    TTextureGR* CreateTextureCache() {
        if (TextureCaches == nullptr) {
            TextureCaches = pas::make_object<pas::List>();
        }
        TTextureGR* Texture = pas::construct_call<TTextureGR>(TTextureGR_Create);
        pas::list_add(TextureCaches, reinterpret_cast<void*>(Texture));
        return Texture;
    }

    void GR_CreateTexture(std::int32_t Width, std::int32_t Height, std::uint32_t Format, std::uint32_t Pool, Direct3D9::IDirect3DTexture9& Result) {
        Direct3D9::IDirect3DTexture9 Texture{};
        std::int32_t ErrorCode{};
        if (GR_Main::Direct3DDevice == nullptr) {
            Result = nullptr;
            return;
        }
        if (Width < 16) {
            Width = 16;
        }
        if (Height < 16) {
            Height = 16;
        }
        ErrorCode = ([&] {
            Direct3D9::IDirect3DTexture9& texture = pas::out_interface(Texture);
            std::uint32_t height = Height;
            std::uint32_t width = Width;
            pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice = GR_Main::Direct3DDevice;
            return Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice, width, height, 1u, 0u, Format, Pool, texture, nullptr);
        }());
        if (ErrorCode == static_cast<std::int32_t>(0x8007000eu)) {
            GR_Main::AppendLogTextThreadSafe("Failed to create texture, trying to free some textures... "_a);
            GR_DX::EvictTextureCaches(true);
            ErrorCode = ([&] {
                Direct3D9::IDirect3DTexture9& texture_2 = pas::out_interface(Texture);
                std::uint32_t height_2 = Height;
                std::uint32_t width_2 = Width;
                pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_2 = GR_Main::Direct3DDevice;
                return Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice_2, width_2, height_2, 1u, 0u, Format, Pool, texture_2, nullptr);
            }());
            if (ErrorCode == 0) {
                GR_Main::AppendLogLineThreadSafe("success"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe("fail"_a);
                GR_Main::LogMemoryUsage();
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"GR_CreateTexture()::CreateTexture(", EC_Str::IntToWideString(Width), u",", EC_Str::IntToWideString(Height), u") error=", EC_Str::IntToWideString(ErrorCode)})));
            }
        }
        GR_DX::ClearTexturePixels(Texture);
        Result = Texture;
        return;
    }

    void FreeTextureCache(TTextureGR* Cache) {
        std::int32_t Index{};
        if (Cache != nullptr) {
            if (TextureCaches != nullptr) {
                Index = pas::list_indexof(TextureCaches, reinterpret_cast<void*>(Cache));
                if (Index >= 0) {
                    pas::list_delete(TextureCaches, Index);
                }
            }
            pas::free(Cache);
        }
    }

    void ClearTexturePixels(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg) {
        Direct3D9::IDirect3DTexture9 Texture = cpp_value_arg;
        Direct3D9::TD3DLockedRect Locked{};
        std::uint32_t Y{};
        Direct3D9::TD3DSurfaceDesc Desc{};
        if (Texture != nullptr) {
            Direct3D9::IDirect3DTexture9_GetLevelDesc(Texture, 0u, Desc);
            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
            if (Locked.Bits != nullptr) {
                for (auto cpp_range = pas::for_to<std::uint32_t>(0u, Desc.Height - 1); cpp_range.next(Y); ) {
                    Windows::FillMemory(static_cast<std::int32_t>(Y) * Locked.Pitch + static_cast<std::uint8_t*>(Locked.Bits), Locked.Pitch, 0);
                }
            }
            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
        }
    }

    void ReleaseAllTextureSurfaces() {
        std::int32_t Index{};
        TTextureGR* Texture{};
        if (TextureCaches != nullptr) {
            Index = 0;
            while (pas::list_count(TextureCaches) > Index) {
                Texture = pas::list_at<TTextureGR>(TextureCaches, Index);
                Texture->ReleaseSurfaces();
                ++Index;
            }
        }
    }

    std::uint32_t GetTextureByteSize(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg) {
        Direct3D9::IDirect3DTexture9 Texture = cpp_value_arg;
        Direct3D9::TD3DSurfaceDesc Desc{};
        std::uint32_t ByteCount = 0u;
        if (Texture != nullptr) {
            Direct3D9::IDirect3DTexture9_GetLevelDesc(Texture, 0u, Desc);
            if (Desc.Format == Direct3D9::D3DFMT_A8R8G8B8 || Desc.Format == Direct3D9::D3DFMT_A8R8G8B8) {
                ByteCount = 4u;
            } else if (Desc.Format == Direct3D9::D3DFMT_R8G8B8) {
                ByteCount = 3u;
            } else if (Desc.Format == Direct3D9::D3DFMT_R5G6B5) {
                ByteCount = 2u;
            } else if (Desc.Format == Direct3D9::D3DFMT_A8) {
                ByteCount = 1u;
            }
            ByteCount = Desc.Width * Desc.Height * ByteCount;
        }
        return ByteCount;
    }

    void AddResidentTextureBytes(std::uint32_t ByteCount) {
        ResidentTextureBytes += ByteCount;
    }

    std::uint32_t Color565ToArgb(std::uint32_t Color) {
        return Color << 3 & 0x000000f8 | (Color >> 3 & 0x000000fc) << 8 | (Color >> 8 & 0x000000f8) << 16 | 0xff000000u;
    }

    std::uint32_t ColorWithAlpha(std::uint32_t Color, std::uint32_t Alpha) {
        return Color & 0x00ffffff | (Alpha & 0x000000ff) << 24;
    }

    void CreateTextureFromPixels(std::int32_t Width, std::int32_t Height, std::uint32_t Format, void* Pixels, std::int32_t PitchBytes, std::uint32_t Pool, Direct3D9::IDirect3DTexture9& Result) {
        std::int32_t Y{};
        Direct3D9::IDirect3DTexture9 Staging{};
        Direct3D9::IDirect3DTexture9 Texture{};
        Direct3D9::TD3DLockedRect Locked{};
        std::int32_t BytesPerPixel{};
        if (GR_Main::Direct3DDevice == nullptr) {
            Result = nullptr;
            return;
        }
        if (Pool == Direct3D9::D3DPOOL_DEFAULT) {
            {
                Direct3D9::IDirect3DTexture9& staging = pas::out_interface(Staging);
                pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice = GR_Main::Direct3DDevice;
                Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice, Width, Height, 1u, 0u, Format, Direct3D9::D3DPOOL_SYSTEMMEM, staging, nullptr);
            }
            Direct3D9::IDirect3DTexture9_LockRect(Staging, 0u, Locked, nullptr, 0u);
            BytesPerPixel = pas::idiv(PitchBytes, Width);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
                Windows::CopyMemory(EC_Mem::AddPointerOffset(Locked.Bits, Locked.Pitch * Y), EC_Mem::AddPointerOffset(Pixels, PitchBytes * Y), Width * BytesPerPixel);
            }
            Direct3D9::IDirect3DTexture9_UnlockRect(Staging, 0u);
            {
                Direct3D9::IDirect3DTexture9& texture = pas::out_interface(Texture);
                pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_2 = GR_Main::Direct3DDevice;
                Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice_2, Width, Height, 1u, 0u, Format, Pool, texture, nullptr);
            }
            {
                pas::ComView<Direct3D9::IDirect3DBaseTexture9_Tag> texture_2 = Texture;
                pas::ComView<Direct3D9::IDirect3DBaseTexture9_Tag> staging_2 = Staging;
                Direct3D9::IDirect3DDevice9_UpdateTexture(GR_Main::Direct3DDevice, staging_2, texture_2);
            }
            Staging = nullptr;
        } else {
            {
                Direct3D9::IDirect3DTexture9& texture_3 = pas::out_interface(Texture);
                pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_3 = GR_Main::Direct3DDevice;
                Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice_3, Width, Height, 1u, 0u, Format, Pool, texture_3, nullptr);
            }
            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
            BytesPerPixel = pas::idiv(PitchBytes, Width);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Height - 1); cpp_range_2.next(Y); ) {
                Windows::CopyMemory(EC_Mem::AddPointerOffset(Locked.Bits, Locked.Pitch * Y), EC_Mem::AddPointerOffset(Pixels, PitchBytes * Y), Width * BytesPerPixel);
            }
            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
        }
        Result = Texture;
        return;
    }

    void QueueDrawPoint(std::int32_t X, std::int32_t Y, std::uint32_t Color, std::int32_t Alpha) {
        if (Alpha == 0) {
            return;
        }
        if (PendingPointCount >= PendingPointCapacity) {
            PendingPointCapacity += 256;
            PendingPoints.set_length(PendingPointCapacity);
        }
        std::int32_t Index = PendingPointCount;
        ++PendingPointCount;
        PendingPoints[Index].Color = GR_DX::ColorWithAlpha(Color, Alpha);
        PendingPoints[Index].X = X;
        PendingPoints[Index].Y = Y;
        PendingPoints[Index].Z = 1.0f;
        PendingPoints[Index].RHW = 1.0f;
        PendingPoints[Index].U = 0.0f;
        PendingPoints[Index].V = 0.0f;
    }

    void FlushDrawPoints(WindowsSdk::PRect ClipRect) {
        WindowsSdk::TRect OldClip{};
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, ClipRect);
        }
        Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_POINTLIST, PendingPointCount, PendingPoints.data(), static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
        PendingPointCount = 0;
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, reinterpret_cast<Types::PRect>(&OldClip));
        }
    }

    void DrawAlphaLine(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::PRect ClipRect) {
        GR_DX::DrawGradientLine(X1, Y1, GR_DX::ColorWithAlpha(Color, Alpha), X2, Y2, GR_DX::ColorWithAlpha(Color, Alpha), ClipRect);
    }

    void DrawGradientLine(std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, WindowsSdk::PRect ClipRect) {
        WindowsSdk::TRect OldClip{};
        DrawVertices[0].Color = Color1;
        DrawVertices[0].X = X1;
        DrawVertices[0].Y = Y1;
        DrawVertices[1].Color = Color2;
        DrawVertices[1].X = X2;
        DrawVertices[1].Y = Y2;
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, ClipRect);
        }
        Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_LINESTRIP, 1u, &DrawVertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, reinterpret_cast<Types::PRect>(&OldClip));
        }
    }

    void DrawColoredTriangle(std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, std::int32_t X3, std::int32_t Y3, std::uint32_t Color3, std::uint8_t Filled, WindowsSdk::PRect ClipRect) {
        WindowsSdk::TRect OldClip{};
        DrawVertices[0].Color = Color1;
        DrawVertices[0].X = X1;
        DrawVertices[0].Y = Y1;
        DrawVertices[1].Color = Color2;
        DrawVertices[1].X = X2;
        DrawVertices[1].Y = Y2;
        DrawVertices[2].Color = Color3;
        DrawVertices[2].X = X3;
        DrawVertices[2].Y = Y3;
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, ClipRect);
        }
        if (!Filled) {
            Direct3D9::IDirect3DDevice9_SetRenderState(GR_Main::Direct3DDevice, Direct3D9::D3DRS_FILLMODE, Direct3D9::D3DFILL_WIREFRAME);
        }
        Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLELIST, 1u, &DrawVertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
        Direct3D9::IDirect3DDevice9_SetRenderState(GR_Main::Direct3DDevice, Direct3D9::D3DRS_FILLMODE, Direct3D9::D3DFILL_SOLID);
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, reinterpret_cast<Types::PRect>(&OldClip));
        }
    }

    void DrawColoredRect(std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::uint32_t Color, std::int32_t Alpha, std::uint8_t Filled, WindowsSdk::PRect ClipRect) {
        WindowsSdk::TRect OldClip{};
        DrawVertices[0].Color = GR_DX::ColorWithAlpha(Color, Alpha);
        DrawVertices[0].X = X - 0.5L;
        DrawVertices[0].Y = Y - 0.5L;
        DrawVertices[1].Color = DrawVertices[0].Color;
        DrawVertices[1].X = X + Width - 0.5L;
        DrawVertices[1].Y = Y - 0.5L;
        DrawVertices[2].Color = DrawVertices[0].Color;
        DrawVertices[2].X = X + Width - 0.5L;
        DrawVertices[2].Y = Y + Height - 0.5L;
        DrawVertices[3].Color = DrawVertices[0].Color;
        DrawVertices[3].X = X - 0.5L;
        DrawVertices[3].Y = Y + Height - 0.5L;
        DrawVertices[4].Color = DrawVertices[0].Color;
        DrawVertices[4].X = X - 0.5L;
        DrawVertices[4].Y = Y - 0.5L;
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, ClipRect);
        }
        if (Filled) {
            Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLEFAN, 4u, &DrawVertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
        } else {
            Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_LINESTRIP, 4u, &DrawVertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
        }
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, reinterpret_cast<Types::PRect>(&OldClip));
        }
    }

    void DrawAntialiasedCircle(std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::PRect ClipRect) {
        std::int32_t SX{};
        std::int32_t SY{};
        float Coverage{};
        std::int32_t DX = Radius;
        std::int32_t PreviousDX = Radius;
        std::int32_t DY = 0;
        float PreviousCoverage = 0.0f;
        std::int32_t Quadrant = 0;
        while (Quadrant < 4) {
            SX = Quadrant % 2 * 2 - 1;
            SY = Quadrant / 2 % 2 * 2 - 1;
            GR_DX::QueueDrawPoint(SX * DX + X, SY * DY + Y, Color, Alpha);
            GR_DX::QueueDrawPoint(SX * DY + X, SY * DX + Y, Color, Alpha);
            ++Quadrant;
        }
        while (DX > DY) {
            ++DY;
            Coverage = System::Sqrt(pas::sqr(Radius) - pas::sqr(DY));
            Coverage = static_cast<long double>(MathImports::Ceil(Coverage)) - Coverage;
            if (Coverage < PreviousCoverage) {
                --DX;
            }
            if (DX < DY) {
                break;
            }
            if (DX == DY && PreviousDX == DX) {
                break;
            }
            Quadrant = 0;
            while (Quadrant < 4) {
                SX = Quadrant % 2 * 2 - 1;
                SY = Quadrant / 2 % 2 * 2 - 1;
                GR_DX::QueueDrawPoint(SX * DX + X, SY * DY + Y, Color, System::Trunc((1.0L - Coverage) * Alpha));
                GR_DX::QueueDrawPoint(SX * DY + X, SY * DX + Y, Color, System::Trunc((1.0L - Coverage) * Alpha));
                if (DX - 1 >= DY) {
                    GR_DX::QueueDrawPoint((DX - 1) * SX + X, SY * DY + Y, Color, System::Trunc(static_cast<long double>(Alpha) * Coverage));
                    GR_DX::QueueDrawPoint(SX * DY + X, (DX - 1) * SY + Y, Color, System::Trunc(static_cast<long double>(Alpha) * Coverage));
                }
                ++Quadrant;
            }
            PreviousCoverage = Coverage;
            PreviousDX = DX;
        }
        GR_DX::FlushDrawPoints(ClipRect);
    }

    void DrawCircle(std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint32_t Color, std::int32_t Alpha, std::int32_t Mode, WindowsSdk::PRect ClipRect) {
        pas::Array<TScreenVertexGR, 0, 4> Vertices{};
        WindowsSdk::TRect OldClip{};
        std::int32_t Index = 0;
        do {
            Vertices[Index].Color = GR_DX::ColorWithAlpha(Color, Alpha);
            Vertices[Index].X = X;
            Vertices[Index].Y = Y;
            Vertices[Index].Z = 1.0f;
            Vertices[Index].RHW = 1.0f;
            ++Index;
        } while (!(Index == 5));
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, ClipRect);
        }
        if (Mode == 0) {
            GR_DX::DrawAntialiasedCircle(X, Y, Radius, Color, Alpha, ClipRect);
        } else if (Mode == 1) {
            Index = 0;
            do {
                Vertices[0].X = static_cast<long double>(Radius) * CircleCos[Index] + X;
                Vertices[0].Y = static_cast<long double>(Radius) * CircleSin[Index] + Y;
                Vertices[1].X = static_cast<long double>(Radius) * CircleCos[Index + 1] + X;
                Vertices[1].Y = static_cast<long double>(Radius) * CircleSin[Index + 1] + Y;
                Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLELIST, 1u, &Vertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
                ++Index;
            } while (!(Index == 360));
        } else if (Mode == 2) {
            Index = 0;
            do {
                Vertices[1].X = X + System::Trunc(static_cast<long double>(Radius) * CircleCos[Index + 1]);
                Vertices[1].Y = Y + System::Trunc(static_cast<long double>(Radius) * CircleSin[Index + 1]);
                Vertices[2].X = X + System::Trunc(static_cast<long double>(Radius) * CircleCos[Index]);
                Vertices[2].Y = Y + System::Trunc(static_cast<long double>(Radius) * CircleSin[Index]);
                if (Index < 90) {
                    Vertices[0].X = X + Radius;
                    Vertices[0].Y = Y + Radius;
                } else if (Index < 180) {
                    Vertices[0].X = X - Radius;
                    Vertices[0].Y = Y + Radius;
                } else if (Index < 270) {
                    Vertices[0].X = X - Radius;
                    Vertices[0].Y = Y - Radius;
                } else {
                    Vertices[0].X = X + Radius;
                    Vertices[0].Y = Y - Radius;
                }
                Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLELIST, 1u, &Vertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
                ++Index;
            } while (!(Index == 360));
        }
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, reinterpret_cast<Types::PRect>(&OldClip));
        }
    }

    void DrawTexture(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg, std::int32_t X, std::int32_t Y, std::int32_t Alpha, std::uint32_t Color, WindowsSdk::PRect ClipRect, std::uint8_t UsePreparedVertices, std::uint8_t MirrorHorizontal) {
        Direct3D9::IDirect3DTexture9 Texture = cpp_value_arg;
        GR_DX::DrawTextureSized(Texture, X, Y, 0, 0, Alpha, Color, ClipRect, UsePreparedVertices, MirrorHorizontal);
    }

    void DrawTextureSized(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg, std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::int32_t Alpha, std::uint32_t Color, WindowsSdk::PRect ClipRect, std::uint8_t UsePreparedVertices, std::uint8_t MirrorHorizontal) {
        Direct3D9::IDirect3DTexture9 Texture = cpp_value_arg;
        Direct3D9::TD3DSurfaceDesc Desc{};
        WindowsSdk::TRect OldClip{};
        if (Texture == nullptr) {
            return;
        }
        if (!UsePreparedVertices) {
            Direct3D9::IDirect3DTexture9_GetLevelDesc(Texture, 0u, Desc);
            if (Width == 0) {
                Width = Desc.Width;
            }
            if (Height == 0) {
                Height = Desc.Height;
            }
            DrawVertices[0].Color = GR_DX::ColorWithAlpha(Color, Alpha);
            DrawVertices[0].X = X - 0.5L;
            DrawVertices[0].Y = Y - 0.5L;
            DrawVertices[0].U = static_cast<std::int32_t>(MirrorHorizontal);
            DrawVertices[0].V = 0.0f;
            DrawVertices[1].Color = DrawVertices[0].Color;
            DrawVertices[1].X = X + Width - 0.5L;
            DrawVertices[1].Y = Y - 0.5L;
            DrawVertices[1].U = 1 - MirrorHorizontal;
            DrawVertices[1].V = 0.0f;
            DrawVertices[2].Color = DrawVertices[0].Color;
            DrawVertices[2].X = X + Width - 0.5L;
            DrawVertices[2].Y = Y + Height - 0.5L;
            DrawVertices[2].U = 1 - MirrorHorizontal;
            DrawVertices[2].V = 1.0f;
            DrawVertices[3].Color = DrawVertices[0].Color;
            DrawVertices[3].X = X - 0.5L;
            DrawVertices[3].Y = Y + Height - 0.5L;
            DrawVertices[3].U = static_cast<std::int32_t>(MirrorHorizontal);
            DrawVertices[3].V = 1.0f;
        }
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_GetScissorRect(GR_Main::Direct3DDevice, OldClip);
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, ClipRect);
        }
        Direct3D9::IDirect3DDevice9_SetTexture(GR_Main::Direct3DDevice, 0u, Texture);
        Direct3D9::IDirect3DDevice9_DrawPrimitiveUP(GR_Main::Direct3DDevice, Direct3D9::D3DPT_TRIANGLEFAN, 2u, &DrawVertices, static_cast<std::int32_t>(sizeof(TScreenVertexGR)));
        Direct3D9::IDirect3DDevice9_SetTexture(GR_Main::Direct3DDevice, 0u, nullptr);
        if (ClipRect != nullptr) {
            Direct3D9::IDirect3DDevice9_SetScissorRect(GR_Main::Direct3DDevice, reinterpret_cast<Types::PRect>(&OldClip));
        }
    }

    void DrawAntialiasedLineDX(std::int32_t StartX, std::int32_t StartY, std::int32_t FinishX, std::int32_t FinishY, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::PRect UnusedClipRect) {
        std::uint8_t Steep{};
        double Temp{};
        double X1 = StartX;
        double Y1 = StartY;
        double X2 = FinishX;
        double Y2 = FinishY;
        double DX = static_cast<long double>(X2) - X1;
        double DY = static_cast<long double>(Y2) - Y1;
        if (DX == 0.0L && DY == 0.0L) {
            return;
        }
        if (std::fabs(static_cast<pas::Extended>(DX)) > std::fabs(static_cast<pas::Extended>(DY))) {
            Steep = false;
        } else {
            Steep = true;
            Temp = X1;
            X1 = Y1;
            Y1 = Temp;
            Temp = X2;
            X2 = Y2;
            Y2 = Temp;
            Temp = DX;
            DX = DY;
            DY = Temp;
        }
        if (X1 > X2) {
            Temp = X1;
            X1 = X2;
            X2 = Temp;
            Temp = Y1;
            Y1 = Y2;
            Y2 = Temp;
            DX = static_cast<long double>(X2) - X1;
            DY = static_cast<long double>(Y2) - Y1;
        }
        double Slope = pas::real_divide(DY, DX);
        double EndX = MathImports::Floor(X1 + 0.5L);
        double EndY = Y1 + (static_cast<long double>(EndX) - X1) * Slope;
        double Gap = 1.0L - GR_DX::LineFractionDX(X1 + 0.5L);
        std::int32_t FirstX = MathImports::Floor(X1 + 0.5L);
        std::int32_t FirstY = MathImports::Floor(EndY);
        double Coverage1 = (1.0L - GR_DX::LineFractionDX(EndY)) * Gap;
        double Coverage2 = static_cast<long double>(GR_DX::LineFractionDX(EndY)) * Gap;
        if (Steep) {
            GR_DX::QueueDrawPoint(FirstY, FirstX, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            GR_DX::QueueDrawPoint(FirstY + 1, FirstX, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        } else {
            GR_DX::QueueDrawPoint(FirstX, FirstY, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            GR_DX::QueueDrawPoint(FirstX, FirstY + 1, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        }
        std::int32_t X = FirstX + 1;
        double InterY = static_cast<long double>(EndY) + Slope;
        EndX = MathImports::Floor(X2 + 0.5L);
        EndY = Y2 + (static_cast<long double>(EndX) - X2) * Slope;
        Gap = 1.0L - GR_DX::LineFractionDX(X2 - 0.5L);
        std::int32_t LastX = MathImports::Floor(X2 + 0.5L);
        std::int32_t LastY = MathImports::Floor(EndY);
        while (LastX - 1 >= X) {
            Coverage1 = 1.0L - GR_DX::LineFractionDX(InterY);
            Coverage2 = GR_DX::LineFractionDX(InterY);
            if (Steep) {
                {
                    std::int32_t ceil = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1);
                    std::int32_t floor = MathImports::Floor(InterY);
                    GR_DX::QueueDrawPoint(floor, X, Color, ceil);
                }
                {
                    std::int32_t ceil_2 = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2);
                    std::int32_t cpp_arg = MathImports::Floor(InterY) + 1;
                    GR_DX::QueueDrawPoint(cpp_arg, X, Color, ceil_2);
                }
            } else {
                {
                    std::int32_t ceil_3 = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1);
                    std::int32_t floor_2 = MathImports::Floor(InterY);
                    GR_DX::QueueDrawPoint(X, floor_2, Color, ceil_3);
                }
                {
                    std::int32_t ceil_4 = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2);
                    std::int32_t cpp_arg_2 = MathImports::Floor(InterY) + 1;
                    GR_DX::QueueDrawPoint(X, cpp_arg_2, Color, ceil_4);
                }
            }
            InterY = static_cast<long double>(InterY) + Slope;
            ++X;
        }
        Coverage1 = (1.0L - GR_DX::LineFractionDX(EndY)) * Gap;
        Coverage2 = static_cast<long double>(GR_DX::LineFractionDX(EndY)) * Gap;
        if (Steep) {
            GR_DX::QueueDrawPoint(LastY, LastX, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            GR_DX::QueueDrawPoint(LastY + 1, LastX, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        } else {
            GR_DX::QueueDrawPoint(LastX, LastY, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            GR_DX::QueueDrawPoint(LastX, LastY + 1, Color, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        }
        GR_DX::FlushDrawPoints(nullptr);
    }

    void DrawAnimatedLineDX(std::int32_t StartX, std::int32_t StartY, std::int32_t FinishX, std::int32_t FinishY, std::uint32_t Color, std::int32_t Phase, WindowsSdk::PRect UnusedClipRect) {
        std::uint8_t Steep{};
        double Temp{};
        double X1 = StartX;
        double Y1 = StartY;
        double X2 = FinishX;
        double Y2 = FinishY;
        double DX = static_cast<long double>(X2) - X1;
        double DY = static_cast<long double>(Y2) - Y1;
        if (DX == 0.0L && DY == 0.0L) {
            return;
        }
        if (std::fabs(static_cast<pas::Extended>(DX)) > std::fabs(static_cast<pas::Extended>(DY))) {
            Steep = false;
        } else {
            Steep = true;
            Temp = X1;
            X1 = Y1;
            Y1 = Temp;
            Temp = X2;
            X2 = Y2;
            Y2 = Temp;
            Temp = DX;
            DX = DY;
            DY = Temp;
        }
        if (X1 > X2) {
            Temp = X1;
            X1 = X2;
            X2 = Temp;
            Temp = Y1;
            Y1 = Y2;
            Y2 = Temp;
            DX = static_cast<long double>(X2) - X1;
            DY = static_cast<long double>(Y2) - Y1;
        }
        double Slope = pas::real_divide(DY, DX);
        double EndX = MathImports::Floor(X1 + 0.5L);
        double EndY = Y1 + (static_cast<long double>(EndX) - X1) * Slope;
        double Gap = 1.0L - GR_DX::AnimatedLineFractionDX(X1 + 0.5L);
        std::int32_t FirstX = MathImports::Floor(X1 + 0.5L);
        std::int32_t FirstY = MathImports::Floor(EndY);
        double Coverage1 = (1.0L - GR_DX::AnimatedLineFractionDX(EndY)) * Gap;
        double Coverage2 = static_cast<long double>(GR_DX::AnimatedLineFractionDX(EndY)) * Gap;
        if (Steep) {
            GR_DX::QueueDrawPoint(FirstY, FirstX, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage1));
            GR_DX::QueueDrawPoint(FirstY + 1, FirstX, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage2));
        } else {
            GR_DX::QueueDrawPoint(FirstX, FirstY, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage1));
            GR_DX::QueueDrawPoint(FirstX, FirstY + 1, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage2));
        }
        GR_DX::AdvanceLinePhase(Phase);
        std::int32_t X = FirstX + 1;
        double InterY = static_cast<long double>(EndY) + Slope;
        EndX = MathImports::Floor(X2 + 0.5L);
        EndY = Y2 + (static_cast<long double>(EndX) - X2) * Slope;
        Gap = 1.0L - GR_DX::AnimatedLineFractionDX(X2 - 0.5L);
        std::int32_t LastX = MathImports::Floor(X2 + 0.5L);
        std::int32_t LastY = MathImports::Floor(EndY);
        while (LastX - 1 >= X) {
            Coverage1 = 1.0L - GR_DX::AnimatedLineFractionDX(InterY);
            Coverage2 = GR_DX::AnimatedLineFractionDX(InterY);
            if (Steep) {
                {
                    std::int32_t ceil = MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage1);
                    std::int32_t floor = MathImports::Floor(InterY);
                    GR_DX::QueueDrawPoint(floor, X, Color, ceil);
                }
                {
                    std::int32_t ceil_2 = MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage2);
                    std::int32_t cpp_arg = MathImports::Floor(InterY) + 1;
                    GR_DX::QueueDrawPoint(cpp_arg, X, Color, ceil_2);
                }
            } else {
                {
                    std::int32_t ceil_3 = MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage1);
                    std::int32_t floor_2 = MathImports::Floor(InterY);
                    GR_DX::QueueDrawPoint(X, floor_2, Color, ceil_3);
                }
                {
                    std::int32_t ceil_4 = MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage2);
                    std::int32_t cpp_arg_2 = MathImports::Floor(InterY) + 1;
                    GR_DX::QueueDrawPoint(X, cpp_arg_2, Color, ceil_4);
                }
            }
            GR_DX::AdvanceLinePhase(Phase);
            InterY = static_cast<long double>(InterY) + Slope;
            ++X;
        }
        Coverage1 = (1.0L - GR_DX::AnimatedLineFractionDX(EndY)) * Gap;
        Coverage2 = static_cast<long double>(GR_DX::AnimatedLineFractionDX(EndY)) * Gap;
        if (Steep) {
            GR_DX::QueueDrawPoint(LastY, LastX, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage1));
            GR_DX::QueueDrawPoint(LastY + 1, LastX, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage2));
        } else {
            GR_DX::QueueDrawPoint(LastX, LastY, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage1));
            GR_DX::QueueDrawPoint(LastX, LastY + 1, Color, MathImports::Ceil(static_cast<long double>(LineAlphaTable[Phase]) * Coverage2));
        }
        GR_DX::FlushDrawPoints(nullptr);
    }

    void TTextureGR_Create(TTextureGR* Self) {
        Self->SurfaceCount = 0;
        Self->LastUseTick = 0u;
        Self->ResidentBytes = 0u;
    }

    void TTextureGR_Destroy(TTextureGR* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    void TTextureGR::Clear() {
        std::int32_t Index = 0;
        while (Index < SurfaceCount) {
            if (Surfaces[Index] != nullptr) {
                Surfaces[Index] = nullptr;
            }
            ++Index;
        }
        if (ResidentBytes > 0) {
            GR_DX::SubtractResidentTextureBytes(ResidentBytes);
        }
        ResidentBytes = 0u;
        Surfaces.set_length(0);
        SurfaceCount = 0;
        LastUseTick = 0u;
    }

    void TTextureGR::ReleaseSurfaces() {
        std::int32_t i = 0;
        while (i < SurfaceCount) {
            Surfaces[i] = nullptr;
            ++i;
        }
        if (ResidentBytes > 0) {
            GR_DX::SubtractResidentTextureBytes(ResidentBytes);
        }
        ResidentBytes = 0u;
        LastUseTick = 0u;
    }

    void TTextureGR::GetSurface(std::int32_t Index, Direct3D9::IDirect3DTexture9& Result) {
        Result = nullptr;
        if (Index < 0 || Index >= SurfaceCount) {
            return;
        }
        LastUseTick = MMSystem::timeGetTime();
        Result = Surfaces[Index];
        return;
    }

    void TTextureGR::SetSurface(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg, std::int32_t Index) {
        Direct3D9::IDirect3DTexture9 Value = cpp_value_arg;
        std::int32_t i{};
        std::uint32_t ByteCount{};
        if (Index < 0 || Index == SurfaceCount) {
            Index = SurfaceCount;
            ++SurfaceCount;
            Surfaces.set_length(SurfaceCount);
        } else if (Index > SurfaceCount) {
            i = SurfaceCount;
            SurfaceCount = Index + 1;
            Surfaces.set_length(SurfaceCount);
            while (i < SurfaceCount) {
                Surfaces[i] = nullptr;
                ++i;
            }
        }
        if (Surfaces[Index] != nullptr) {
            ByteCount = GR_DX::GetTextureByteSize(Surfaces[Index]);
            if (ByteCount > 0) {
                ResidentBytes -= ByteCount;
                GR_DX::SubtractResidentTextureBytes(ByteCount);
            }
        }
        ByteCount = GR_DX::GetTextureByteSize(Value);
        ResidentBytes += ByteCount;
        GR_DX::AddResidentTextureBytes(ByteCount);
        Surfaces[Index] = nullptr;
        Surfaces[Index] = Value;
    }

    double LineFractionDX(double Value) {
        return static_cast<long double>(Value) - MathImports::Floor(Value);
    }

    double AnimatedLineFractionDX(double Value) {
        return static_cast<long double>(Value) - MathImports::Floor(Value);
    }

    void AdvanceLinePhase(std::int32_t& Phase) {
        Phase += 20;
        if (Phase >= 360) {
            Phase -= 360;
        }
    }

    void TTextureGR::p_destroy() {
        GR_DX::TTextureGR_Destroy(this);
    }

} // namespace GR_DX
