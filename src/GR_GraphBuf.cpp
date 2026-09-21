#include "layout/GR_GraphBuf.hpp"
#include "types/EC_OKGF.hpp"
#include "types/SysUtilsImports.hpp"
#include "types/Types.hpp"
#include "units/BitmapPorts.hpp"
#include "units/ClassesImports.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"

namespace GR_GraphBuf {
    // Ignores disjoint bits after each mask's first contiguous run; BytesPerPixel is unchanged.
    void TPixelFormatGR::RebuildChannelMetrics() {
        std::uint32_t Mask{};
        RedShift = 0u;
        RedBits = 0u;
        RedLevels = 0u;
        GreenShift = 0u;
        GreenBits = 0u;
        GreenLevels = 0u;
        BlueShift = 0u;
        BlueBits = 0u;
        BlueLevels = 0u;
        AlphaShift = 0u;
        AlphaBits = 0u;
        AlphaLevels = 0u;
        if (RedMask != 0) {
            Mask = RedMask;
            while ((Mask & 1) == 0) {
                ++RedShift;
                Mask = Mask >> 1;
            }
            while ((Mask & 1) != 0) {
                ++RedBits;
                Mask = Mask >> 1;
            }
            RedLevels = pas::shl(1, RedBits);
        }
        if (GreenMask != 0) {
            Mask = GreenMask;
            while ((Mask & 1) == 0) {
                ++GreenShift;
                Mask = Mask >> 1;
            }
            while ((Mask & 1) != 0) {
                ++GreenBits;
                Mask = Mask >> 1;
            }
            GreenLevels = pas::shl(1, GreenBits);
        }
        if (BlueMask != 0) {
            Mask = BlueMask;
            while ((Mask & 1) == 0) {
                ++BlueShift;
                Mask = Mask >> 1;
            }
            while ((Mask & 1) != 0) {
                ++BlueBits;
                Mask = Mask >> 1;
            }
            BlueLevels = pas::shl(1, BlueBits);
        }
        if (AlphaMask != 0) {
            Mask = AlphaMask;
            while ((Mask & 1) == 0) {
                ++AlphaShift;
                Mask = Mask >> 1;
            }
            while ((Mask & 1) != 0) {
                ++AlphaBits;
                Mask = Mask >> 1;
            }
            AlphaLevels = pas::shl(1, AlphaBits);
        }
        TotalChannelBits = RedBits + GreenBits + BlueBits + AlphaBits;
    }

    std::uint32_t TPixelFormatGR::PackRgbBytes(std::uint8_t Red, std::uint8_t Green, std::uint8_t Blue) {
        double cpp_arg = pas::real_divide(Red, 255.0L);
        double cpp_arg_2 = pas::real_divide(Green, 255.0L);
        double cpp_arg_3 = pas::real_divide(Blue, 255.0L);
        return PackNormalizedRgb(cpp_arg, cpp_arg_2, cpp_arg_3);
    }

    std::uint32_t TPixelFormatGR::PackRgb(std::int32_t Red, std::int32_t Green, std::int32_t Blue) {
        double cpp_arg = pas::real_divide(Red, 255.0L);
        double cpp_arg_2 = pas::real_divide(Green, 255.0L);
        double cpp_arg_3 = pas::real_divide(Blue, 255.0L);
        return PackNormalizedRgb(cpp_arg, cpp_arg_2, cpp_arg_3);
    }

    // Does not clamp inputs or include alpha.
    std::uint32_t TPixelFormatGR::PackNormalizedRgb(double Red, double Green, double Blue) {
        std::uint32_t cpp_left_2 = pas::shl(static_cast<std::uint32_t>(System::Trunc(static_cast<long double>(Red) * (RedLevels - 1))), RedShift);
        std::uint32_t cpp_left = cpp_left_2 | pas::shl(static_cast<std::uint32_t>(System::Trunc(static_cast<long double>(Green) * (GreenLevels - 1))), GreenShift);
        return cpp_left | pas::shl(static_cast<std::uint32_t>(System::Trunc(static_cast<long double>(Blue) * (BlueLevels - 1))), BlueShift);
    }

    std::uint32_t TPixelFormatGR::InterpolateRgb(std::uint32_t First, std::uint32_t Second, float Amount) {
        std::int32_t R1 = pas::imod(pas::shr(First, RedShift), RedLevels);
        std::int32_t G1 = pas::imod(pas::shr(First, GreenShift), GreenLevels);
        std::int32_t B1 = pas::imod(pas::shr(First, BlueShift), BlueLevels);
        std::int32_t R2 = pas::imod(pas::shr(Second, RedShift), RedLevels);
        std::int32_t G2 = pas::imod(pas::shr(Second, GreenShift), GreenLevels);
        std::int32_t B2 = pas::imod(pas::shr(Second, BlueShift), BlueLevels);
        std::uint32_t cpp_left_2 = pas::shl(static_cast<std::uint32_t>(System::Trunc(R1 + static_cast<long double>(R2 - R1) * Amount)), RedShift);
        std::uint32_t cpp_left = cpp_left_2 | pas::shl(static_cast<std::uint32_t>(System::Trunc(G1 + static_cast<long double>(G2 - G1) * Amount)), GreenShift);
        return cpp_left | pas::shl(static_cast<std::uint32_t>(System::Trunc(B1 + static_cast<long double>(B2 - B1) * Amount)), BlueShift);
    }

    std::uint8_t TPixelFormatGR::UnpackRed(std::uint32_t Color) {
        if (TotalChannelBits == 16) {
            return Color >> 8;
        }
        return Color >> 7;
    }

    std::uint8_t TPixelFormatGR::UnpackGreen(std::uint32_t Color) {
        if (TotalChannelBits == 16) {
            return Color >> 3;
        }
        return Color >> 2;
    }

    std::uint8_t TPixelFormatGR::UnpackBlue(std::uint32_t Color) {
        return pas::shl(static_cast<std::int32_t>(static_cast<std::uint8_t>(Color)), 3);
    }

    void TGraphBufGR_Create(TGraphBufGR* Self, std::uint8_t AUseTexture) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Width = 0;
        Self->Height = 0;
        Self->PitchBytes = 0;
        Self->StorageKind = 0;
        Self->BitsPerPixel = 0;
        Self->BytesPerPixel = 0;
        Self->UseTexture = AUseTexture;
        Self->UsesTextureStorage = false;
        Self->KeepTextureUntilReplacement = false;
        Self->Texture = nullptr;
        Self->TextureLocked = false;
        Self->TextureLockedReadOnly = false;
    }

    void TGraphBufGR_Destroy(TGraphBufGR* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TGraphBufGR::Clear() {
        UnlockTexture();
        if (StorageKind == 0) {
            if (static_cast<std::uint8_t>(UsesTextureStorage ^ 1) && Pixels != nullptr) {
                EC_Mem::FreeEC(Pixels);
            }
            Pixels = nullptr;
        }
        Texture = nullptr;
        UsesTextureStorage = false;
        KeepTextureUntilReplacement = false;
        Width = 0;
        Height = 0;
        PitchBytes = 0;
        StorageKind = 0;
        BitsPerPixel = 0;
        BytesPerPixel = 0;
    }

    // Locks texture storage for writing if necessary.
    void* TGraphBufGR::GetPixels() {
        LockTexture(false);
        return Pixels;
    }

    // Records 16-bit pixels; software pitch uses CurrentPixelFormat.BytesPerPixel and four-byte alignment.
    void TGraphBufGR::AllocateNative(std::int32_t Width, std::int32_t Height) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::TD3DLockedRect Locked{};
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 16;
        BytesPerPixel = static_cast<std::int32_t>(sizeof(std::uint16_t));
        if (UseTexture) {
            Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            if (Texture != nullptr) {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
                PitchBytes = Locked.Pitch;
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                UsesTextureStorage = true;
            }
        } else {
            PitchBytes = GR_Main::CurrentPixelFormat->BytesPerPixel * Width;
            if ((PitchBytes & 3) != 0) {
                PitchBytes = PitchBytes + 4 - (PitchBytes & 3);
            }
            Pixels = EC_Mem::AllocEC(PitchBytes * this->Height);
            if ((PitchBytes & 3) != 0 || (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Pixels)) & 3) != 0) {
                pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.CreateN"_a));
            }
        }
    }

    void TGraphBufGR::AllocateNativePitch(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::TD3DLockedRect Locked{};
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 16;
        BytesPerPixel = static_cast<std::int32_t>(sizeof(std::uint16_t));
        if (UseTexture) {
            Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            if (Texture != nullptr) {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
                this->PitchBytes = Locked.Pitch;
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                UsesTextureStorage = true;
            }
        } else {
            this->PitchBytes = PitchBytes;
            Pixels = EC_Mem::AllocEC(this->PitchBytes * this->Height);
        }
    }

    void TGraphBufGR::AttachPixels(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes, void* Data) {
        Clear();
        Pixels = Data;
        this->Width = Width;
        this->Height = Height;
        this->PitchBytes = PitchBytes;
        StorageKind = 1;
        BytesPerPixel = pas::idiv(static_cast<std::uint32_t>(this->PitchBytes), static_cast<std::uint32_t>(this->Width));
        BitsPerPixel = BytesPerPixel * 8;
    }

    // Software storage uses Width*4 pitch; texture storage uses the returned surface pitch.
    void TGraphBufGR::AllocateRgbaTight(std::int32_t Width, std::int32_t Height) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::TD3DLockedRect Locked{};
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 32;
        BytesPerPixel = static_cast<std::int32_t>(sizeof(TColorRGBA));
        if (UseTexture) {
            Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            if (Texture != nullptr) {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
                PitchBytes = Locked.Pitch;
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                UsesTextureStorage = true;
            }
        } else {
            PitchBytes = Width * static_cast<std::int32_t>(sizeof(TColorRGBA));
            Pixels = EC_Mem::AllocEC(PitchBytes * this->Height);
        }
    }

    void TGraphBufGR::AllocateRgba(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::TD3DLockedRect Locked{};
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 32;
        BytesPerPixel = static_cast<std::int32_t>(sizeof(TColorRGBA));
        if (UseTexture) {
            Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            if (Texture != nullptr) {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
                this->PitchBytes = Locked.Pitch;
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                UsesTextureStorage = true;
            }
        } else {
            this->PitchBytes = PitchBytes;
            Pixels = EC_Mem::AllocEC(this->PitchBytes * this->Height);
        }
    }

    void TGraphBufGR::AllocateRgbTight(std::int32_t Width, std::int32_t Height) {
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 24;
        BytesPerPixel = 3;
        PitchBytes = Width * 3;
        Pixels = EC_Mem::AllocEC(PitchBytes * this->Height);
    }

    // Always allocates software storage, even when UseTexture is enabled.
    void TGraphBufGR::AllocateRgb(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes) {
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 24;
        BytesPerPixel = 3;
        this->PitchBytes = PitchBytes;
        Pixels = EC_Mem::AllocEC(this->PitchBytes * this->Height);
    }

    // Eight-bit software pixels with four-byte-aligned pitch.
    void TGraphBufGR::AllocateGrayscale(std::int32_t Width, std::int32_t Height) {
        Clear();
        this->Width = Width;
        this->Height = Height;
        BitsPerPixel = 8;
        BytesPerPixel = 1;
        PitchBytes = Width;
        if ((PitchBytes & 3) != 0) {
            PitchBytes = PitchBytes + 4 - (PitchBytes & 3);
        }
        Pixels = EC_Mem::AllocEC(PitchBytes * this->Height);
        if ((PitchBytes & 3) != 0 || (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Pixels)) & 3) != 0) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.CreateBYTE"_a));
        }
    }

    // Decode the entire file payload, ignoring Buffer.Position. Failures raise.
    // Uses CurrentPixelFormat masks and byte width.
    void TGraphBufGR::LoadImage(EC_Buf::TBufEC* Buffer) {
        Clear();
        EC_OKGF::POkgfReadContext Context = GR_Main::BeginImageRead(Buffer->Data, Buffer->DataSize, Width, Height);
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TGraphBufGR.LoadFromBuf. Error load file 1 (buf size=", EC_Str::IntToWideString(Buffer->DataSize), u")"}))));
        }
        AllocateNative(Width, Height);
        {
            std::uint32_t redMask = GR_Main::CurrentPixelFormat->RedMask;
            std::uint32_t greenMask = GR_Main::CurrentPixelFormat->GreenMask;
            std::uint32_t blueMask = GR_Main::CurrentPixelFormat->BlueMask;
            std::uint32_t alphaMask = GR_Main::CurrentPixelFormat->AlphaMask;
            std::int32_t bytesPerPixel = GR_Main::CurrentPixelFormat->BytesPerPixel;
            void* pixels = GetPixels();
            std::int32_t pitchBytes = PitchBytes;
            Context = static_cast<EC_OKGF::POkgfReadContext>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GR_Main::ReadImagePixels(Context, pixels, pitchBytes, redMask, greenMask, blueMask, alphaMask, bytesPerPixel)))));
        }
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TGraphBufGR.LoadFromBuf. Error load file 2 (buf size=", EC_Str::IntToWideString(Buffer->DataSize), u")"}))));
        }
    }

    // Produces BGRA byte order, with alpha in the high byte.
    void TGraphBufGR::LoadImageRgba(EC_Buf::TBufEC* Buffer) {
        Clear();
        EC_OKGF::POkgfReadContext Context = GR_Main::BeginImageRead(Buffer->Data, Buffer->DataSize, Width, Height);
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.LoadFromBufRGBA. Error load file"_a));
        }
        AllocateRgbaTight(Width, Height);
        {
            void* pixels = GetPixels();
            std::int32_t pitchBytes = PitchBytes;
            Context = static_cast<EC_OKGF::POkgfReadContext>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GR_Main::ReadImagePixels(Context, pixels, pitchBytes, 0x00ff0000u, 0x0000ff00u, 0x000000ffu, 0xff000000u, 4)))));
        }
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.LoadFromBufRGBA. Error load file"_a));
        }
    }

    // Produces RGB byte order.
    void TGraphBufGR::LoadImageRgb(EC_Buf::TBufEC* Buffer) {
        Clear();
        EC_OKGF::POkgfReadContext Context = GR_Main::BeginImageRead(Buffer->Data, Buffer->DataSize, Width, Height);
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.LoadFromBufRGB. Error load file"_a));
        }
        AllocateRgbTight(Width, Height);
        {
            void* pixels = GetPixels();
            std::int32_t pitchBytes = PitchBytes;
            Context = static_cast<EC_OKGF::POkgfReadContext>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GR_Main::ReadImagePixels(Context, pixels, pitchBytes, 0x000000ffu, 0x0000ff00u, 0x00ff0000u, 0u, 3)))));
        }
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.LoadFromBufRGB. Error load file"_a));
        }
    }

    void TGraphBufGR::LoadImageGrayscale(EC_Buf::TBufEC* Buffer) {
        Clear();
        EC_OKGF::POkgfReadContext Context = GR_Main::BeginImageRead(Buffer->Data, Buffer->DataSize, Width, Height);
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.LoadFromBufGrayscale. Error load file"_a));
        }
        AllocateGrayscale(Width, Height);
        {
            void* pixels = GetPixels();
            std::int32_t pitchBytes = PitchBytes;
            Context = static_cast<EC_OKGF::POkgfReadContext>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GR_Main::ReadImagePixels(Context, pixels, pitchBytes, 0x000000ffu, 0u, 0u, 0u, 1)))));
        }
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufGR.LoadFromBufGrayscale. Error load file"_a));
        }
    }

    std::uint32_t TGraphBufGR::GetPixel16(std::int32_t X, std::int32_t Y) {
        SysUtilsImports::PByteArray Data{};
        LockTexture(true);
        Data = static_cast<SysUtilsImports::PByteArray>(Pixels);
        WindowsSdk::PWORD Pixel = reinterpret_cast<WindowsSdk::PWORD>(reinterpret_cast<std::uint8_t*>(pas::byte_offset(Data, (Y * PitchBytes + X * static_cast<std::int32_t>(sizeof(std::uint16_t))) * sizeof(std::uint8_t))));
        return pas::load_unaligned<std::uint16_t>(Pixel);
    }

    void TGraphBufGR::SetPixel16(std::int32_t X, std::int32_t Y, std::uint32_t Color) {
        SysUtilsImports::PByteArray Data{};
        LockTexture(false);
        Data = static_cast<SysUtilsImports::PByteArray>(Pixels);
        WindowsSdk::PWORD Pixel = reinterpret_cast<WindowsSdk::PWORD>(reinterpret_cast<std::uint8_t*>(pas::byte_offset(Data, (Y * PitchBytes + X * static_cast<std::int32_t>(sizeof(std::uint16_t))) * sizeof(std::uint8_t))));
        pas::store_unaligned<std::uint16_t>(Pixel, static_cast<std::uint16_t>(Color));
    }

    std::uint32_t TGraphBufGR::GetBrightness16(std::int32_t X, std::int32_t Y) {
        SysUtilsImports::PByteArray Data{};
        LockTexture(true);
        std::uint32_t Result = 0u;
        if (X < 0 || Y < 0 || Width - 1 < X || Height - 1 < Y) {
            return Result;
        }
        Data = static_cast<SysUtilsImports::PByteArray>(Pixels);
        WindowsSdk::PWORD Pixel = reinterpret_cast<WindowsSdk::PWORD>(reinterpret_cast<std::uint8_t*>(pas::byte_offset(Data, (Y * PitchBytes + X * static_cast<std::int32_t>(sizeof(std::uint16_t))) * sizeof(std::uint8_t))));
        std::uint32_t Color = pas::load_unaligned<std::uint16_t>(Pixel);
        if (GR_Main::CurrentPixelFormat->TotalChannelBits == 16) {
            return (Color & 31) + (Color >> 6 & 31) + (Color >> 11 & 31);
        }
        return (Color & 31) + (Color >> 5 & 31) + (Color >> 10 & 31);
    }

    void TGraphBufGR::BlendPixel16(std::int32_t X, std::int32_t Y, std::uint32_t Color, std::uint8_t Alpha) {
        LockTexture(false);
        GR_Main::BlendPixel16(EC_Mem::AddPointerOffset(Pixels, Y * PitchBytes + X * static_cast<std::int32_t>(sizeof(std::uint16_t))), Color, Alpha);
    }

    void TGraphBufGR::DrawAlphaLine16(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::uint8_t Alpha, WindowsSdk::TRect Clip) {
        LockTexture(false);
        GR_Main::Ex_OKGR_Line_DrawClip_Alpha_16(Pixels, PitchBytes, X1, Y1, X2, Y2, Color, Alpha, Clip);
    }

    std::uint32_t TGraphBufGR::GetPixel32(std::int32_t X, std::int32_t Y) {
        LockTexture(true);
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(Pixels, Y * PitchBytes + X * static_cast<std::int32_t>(sizeof(TColorRGBA))));
    }

    void TGraphBufGR_DrawHorizontalLine16(TGraphBufGR* Self, std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color) {
        if (Count == 0) {
            return;
        }
        if (Count < 0) {
            X = X + Count + 1;
            Count = -Count;
        }
        Self->LockTexture(false);
        X = Y * Self->PitchBytes + X * static_cast<std::int32_t>(sizeof(std::uint16_t));
        void* Data = Self->Pixels;
        // Native bug: EDI is clobbered without saving/restoring it, violating
        // Delphi's callee-save convention. The original function has no outer save.
        // DCC32 18.5 O+ callers can retain a destination pointer in EDI; subsequent
        // writes then use the end of this line instead. Recompiling this routine
        // with O+ still leaves EDI unpreserved.
        BitmapPorts::FillLine16(static_cast<std::uint8_t*>(Data) + X, Count, 2, Color);
    }

    void TGraphBufGR_DrawVerticalLine16(TGraphBufGR* Self, std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color) {
        if (Count == 0) {
            return;
        }
        if (Count < 0) {
            Y = Y + Count + 1;
            Count = -Count;
        }
        Self->LockTexture(false);
        X = Y * Self->PitchBytes + X * static_cast<std::int32_t>(sizeof(std::uint16_t));
        void* Data = Self->Pixels;
        std::int32_t Step = Self->PitchBytes;
        // Native bug: EDI and EBX are clobbered without saving/restoring them,
        // violating Delphi's callee-save convention; there are no outer saves.
        // DCC32 18.5 O+ callers can retain Self in EBX and a destination in EDI:
        // after this call they may dereference PitchBytes as Self or write through
        // the advanced pixel pointer. O+ recompilation adds an outer EBX save for
        // Pascal's Self register in the probe, but still leaves EDI unpreserved.
        BitmapPorts::FillLine16(static_cast<std::uint8_t*>(Data) + X, Count, Step, Color);
    }

    void TGraphBufGR::DrawHorizontalLine16Clipped(std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color, WindowsSdk::TRect Clip) {
        if (Count == 0) {
            return;
        }
        if (Count < 0) {
            X = X + Count + 1;
            Count = -Count;
        }
        if (Y < Clip.Top || Y >= Clip.Bottom || X >= Clip.Right || X + Count <= Clip.Left) {
            return;
        }
        if (X < Clip.Left) {
            Count -= Clip.Left - X;
            X = Clip.Left;
        }
        if (X + Count > Clip.Right) {
            Count -= X + Count - Clip.Right;
        }
        GR_GraphBuf::TGraphBufGR_DrawHorizontalLine16(this, X, Y, Count, Color);
    }

    void TGraphBufGR::DrawVerticalLine16Clipped(std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color, WindowsSdk::TRect Clip) {
        if (Count == 0) {
            return;
        }
        if (Count < 0) {
            Y = Y + Count + 1;
            Count = -Count;
        }
        if (X < Clip.Left || X >= Clip.Right || Y >= Clip.Bottom || Y + Count <= Clip.Top) {
            return;
        }
        if (Y < Clip.Top) {
            Count -= Clip.Top - Y;
            Y = Clip.Top;
        }
        if (Y + Count > Clip.Bottom) {
            Count -= Y + Count - Clip.Bottom;
        }
        GR_GraphBuf::TGraphBufGR_DrawVerticalLine16(this, X, Y, Count, Color);
    }

    void TGraphBufGR::DrawLine16(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color) {
        if (First.Y == Last.Y) {
            GR_GraphBuf::TGraphBufGR_DrawHorizontalLine16(this, First.X, First.Y, Last.X - First.X + 1, Color);
        } else if (First.X == Last.X) {
            GR_GraphBuf::TGraphBufGR_DrawVerticalLine16(this, First.X, First.Y, Last.Y - First.Y + 1, Color);
        } else {
            LockTexture(false);
            GR_Main::Ex_OKGR_Line_Draw_WORD(Pixels, PitchBytes, First.X, First.Y, Last.X, Last.Y, Color);
        }
    }

    void TGraphBufGR::DrawAnimatedLine16(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t Phase, WindowsSdk::TRect Clip) {
        LockTexture(false);
        GR_Main::Ex_OKGR_AnimLine_Draw_16(Pixels, PitchBytes, First.X, First.Y, Last.X, Last.Y, Color, Phase, Clip);
    }

    void TGraphBufGR::DrawShadowLine16(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t Phase, WindowsSdk::TRect Clip, void* ShadowPixels, std::int32_t ShadowPitch) {
        LockTexture(false);
        GR_Main::Ex_OKGR_AnimShadowLine_Draw_16(Pixels, PitchBytes, First.X, First.Y, Last.X, Last.Y, Color, Phase, Clip, ShadowPixels, ShadowPitch);
    }

    void TGraphBufGR::DrawAlphaTrapezium16(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, std::uint8_t Alpha, WindowsSdk::TRect Clip) {
        LockTexture(false);
        if (Alpha == 64) {
            GR_Main::Ex_OKGR_Alpha64Trapezium_16(Pixels, PitchBytes, X1, Y1, X2, Y2, X3, X4, Color, Clip);
        } else if (Alpha == 128) {
            GR_Main::Ex_OKGR_Alpha128Trapezium_16(Pixels, PitchBytes, X1, Y1, X2, Y2, X3, X4, Color, Clip);
        }
    }

    void TGraphBufGR::DrawLine16Clipped(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, WindowsSdk::TRect Clip) {
        WindowsSdk::TRect InclusiveClip{};
        if (First.X >= Clip.Left && First.Y >= Clip.Top && First.X < Clip.Right && First.Y < Clip.Bottom && Last.X >= Clip.Left && Last.Y >= Clip.Top && Last.X < Clip.Right && Last.Y < Clip.Bottom) {
            DrawLine16(First, Last, Color);
        } else if (First.Y == Last.Y) {
            DrawHorizontalLine16Clipped(First.X, First.Y, Last.X - First.X + 1, Color, Clip);
        } else if (First.X == Last.X) {
            DrawVerticalLine16Clipped(First.X, First.Y, Last.Y - First.Y + 1, Color, Clip);
        } else {
            InclusiveClip.Left = Clip.Left;
            InclusiveClip.Top = Clip.Top;
            InclusiveClip.Right = Clip.Right - 1;
            InclusiveClip.Bottom = Clip.Bottom - 1;
            LockTexture(false);
            GR_Main::Ex_OKGR_Line_DrawClip_WORD(Pixels, PitchBytes, First.X, First.Y, Last.X, Last.Y, Color, InclusiveClip);
        }
    }

    // RGBA pixels; coverage replaces the color alpha. Blending preserves an existing pixel's alpha unless coverage is fully opaque.
    void TGraphBufGR::DrawAntialiasedLine(WindowsSdk::TPoint FirstPoint, WindowsSdk::TPoint SecondPoint, std::uint32_t Color) {
        std::uint8_t Steep{};
        double Temp{};
        auto LineFraction = [&](double Value) -> double {
            return static_cast<long double>(Value) - MathImports::Floor(Value);
        };
        auto PlotLinePixel = [&](std::int32_t X, std::int32_t Y, std::uint32_t Color, std::int32_t Alpha) -> void {
            std::uint32_t Denominator{};
            auto BlendLineChannel = [&](std::uint32_t DestColor, std::uint32_t DestAlpha, std::uint32_t SourceColor, std::uint32_t SourceAlpha, std::uint32_t Denominator) -> std::uint32_t {
                return pas::idiv((255 - SourceAlpha) * DestColor * DestAlpha + SourceAlpha * SourceColor * 255, Denominator);
            };
            if (Alpha == 0 || X < 0 || Y < 0 || static_cast<std::uint32_t>(this->Width) <= static_cast<std::uint32_t>(X) || static_cast<std::uint32_t>(this->Height) <= static_cast<std::uint32_t>(Y)) {
                return;
            }
            PColorRGBA Source = reinterpret_cast<PColorRGBA>(&Color);
            Source->A = Alpha;
            PColorRGBA Dest = static_cast<PColorRGBA>(EC_Mem::AddPointerOffset(this->Pixels, this->PitchBytes * Y + X * static_cast<std::int32_t>(sizeof(TColorRGBA))));
            if (Dest->A == 0 || Source->A == 255) {
                pas::store_unaligned<TColorRGBA>(Dest, pas::load_unaligned<TColorRGBA>(Source));
            } else if (Dest->A == 255) {
                Dest->R = ((255 - Source->A) * Dest->R + Source->R * Source->A) / 255;
                Dest->G = ((255 - Source->A) * Dest->G + Source->G * Source->A) / 255;
                Dest->B = ((255 - Source->A) * Dest->B + Source->B * Source->A) / 255;
            } else {
                Denominator = 255 * 255 - (255 - Source->A) * (255 - Dest->A);
                Dest->R = BlendLineChannel(Dest->R, Dest->A, Source->R, Source->A, Denominator);
                Dest->G = BlendLineChannel(Dest->G, Dest->A, Source->G, Source->A, Denominator);
                Dest->B = BlendLineChannel(Dest->B, Dest->A, Source->B, Source->A, Denominator);
            }
        };
        LockTexture(false);
        double X1 = FirstPoint.X;
        double Y1 = FirstPoint.Y;
        double X2 = SecondPoint.X;
        double Y2 = SecondPoint.Y;
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
        double Gap = 1.0L - LineFraction(X1 + 0.5L);
        std::int32_t FirstX = MathImports::Floor(X1 + 0.5L);
        std::int32_t FirstY = MathImports::Floor(EndY);
        double Coverage1 = (1.0L - LineFraction(EndY)) * Gap;
        double Coverage2 = static_cast<long double>(LineFraction(EndY)) * Gap;
        if (Steep) {
            PlotLinePixel(FirstY, FirstX, Color, MathImports::Ceil(Coverage1 * 255.0L));
            PlotLinePixel(FirstY + 1, FirstX, Color, MathImports::Ceil(Coverage2 * 255.0L));
        } else {
            PlotLinePixel(FirstX, FirstY, Color, MathImports::Ceil(Coverage1 * 255.0L));
            PlotLinePixel(FirstX, FirstY + 1, Color, MathImports::Ceil(Coverage2 * 255.0L));
        }
        std::int32_t X = FirstX + 1;
        double InterY = static_cast<long double>(EndY) + Slope;
        EndX = MathImports::Floor(X2 + 0.5L);
        EndY = Y2 + (static_cast<long double>(EndX) - X2) * Slope;
        Gap = 1.0L - LineFraction(X2 - 0.5L);
        std::int32_t LastX = MathImports::Floor(X2 + 0.5L);
        std::int32_t LastY = MathImports::Floor(EndY);
        while (LastX - 1 >= X) {
            Coverage1 = 1.0L - LineFraction(InterY);
            Coverage2 = LineFraction(InterY);
            if (Steep) {
                {
                    std::int32_t ceil = MathImports::Ceil(Coverage1 * 255.0L);
                    std::int32_t floor = MathImports::Floor(InterY);
                    PlotLinePixel(floor, X, Color, ceil);
                }
                {
                    std::int32_t ceil_2 = MathImports::Ceil(Coverage2 * 255.0L);
                    std::int32_t cpp_arg = MathImports::Floor(InterY) + 1;
                    PlotLinePixel(cpp_arg, X, Color, ceil_2);
                }
            } else {
                {
                    std::int32_t ceil_3 = MathImports::Ceil(Coverage1 * 255.0L);
                    std::int32_t floor_2 = MathImports::Floor(InterY);
                    PlotLinePixel(X, floor_2, Color, ceil_3);
                }
                {
                    std::int32_t ceil_4 = MathImports::Ceil(Coverage2 * 255.0L);
                    std::int32_t cpp_arg_2 = MathImports::Floor(InterY) + 1;
                    PlotLinePixel(X, cpp_arg_2, Color, ceil_4);
                }
            }
            InterY = static_cast<long double>(InterY) + Slope;
            ++X;
        }
        Coverage1 = (1.0L - LineFraction(EndY)) * Gap;
        Coverage2 = static_cast<long double>(LineFraction(EndY)) * Gap;
        if (Steep) {
            PlotLinePixel(LastY, LastX, Color, MathImports::Ceil(Coverage1 * 255.0L));
            PlotLinePixel(LastY + 1, LastX, Color, MathImports::Ceil(Coverage2 * 255.0L));
        } else {
            PlotLinePixel(LastX, LastY, Color, MathImports::Ceil(Coverage1 * 255.0L));
            PlotLinePixel(LastX, LastY + 1, Color, MathImports::Ceil(Coverage2 * 255.0L));
        }
    }

    void TGraphBufGR::ClearPixels() {
        LockTexture(false);
        pas::fill_memory(Pixels, PitchBytes * Height, static_cast<std::uint8_t>(0));
    }

    void TGraphBufGR::FillPixels(std::uint8_t Value) {
        LockTexture(false);
        Windows::FillMemory(Pixels, PitchBytes * Height, Value);
    }

    void TGraphBufGR::FillPixels16(std::uint16_t Color) {
        LockTexture(false);
        GR_Main::Ex_OKGR_Fill_WORD(Pixels, PitchBytes, Width, Height, Color);
    }

    void TGraphBufGR_FillRect32(TGraphBufGR* Self, WindowsSdk::TRect Rect, std::uint32_t Color) {
        Self->LockTexture(false);
        std::int32_t Columns = Rect.Right - Rect.Left;
        std::int32_t Rows = Rect.Bottom - Rect.Top;
        std::int32_t RowSkip = Self->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        void* Data = Rect.Top * Self->PitchBytes + Rect.Left * static_cast<std::int32_t>(sizeof(TColorRGBA)) + static_cast<std::uint8_t*>(Self->Pixels);
        // Native precondition: Columns and Rows must be positive. Neither is checked
        // before writing; zero wraps on DEC and the loop writes beyond the rectangle.
        BitmapPorts::FillPixels32(Data, Columns, Rows, RowSkip, Color);
    }

    void TGraphBufGR_ScaleAlpha(TGraphBufGR* Self, WindowsSdk::TRect Rect, std::uint8_t Alpha) {
        Self->LockTexture(false);
        std::int32_t Columns = Rect.Right - Rect.Left;
        std::int32_t Rows = Rect.Bottom - Rect.Top;
        std::int32_t RowSkip = Self->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        // Form the relative field address before adding Pixels to retain native load order.
        void* Data = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorRGBA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Rect.Top * Self->PitchBytes + Rect.Left * static_cast<std::int32_t>(sizeof(TColorRGBA)))))->A)) + static_cast<std::uint8_t*>(Self->Pixels);
        void* Table = static_cast<std::uint8_t*>(GR_Main::Ex_OKGF_MulTable256x256()) + pas::shl(static_cast<std::int32_t>(Alpha), 8);
        // Native precondition: Columns and Rows must be positive. Neither is checked
        // before writing; zero wraps on DEC and the loop writes beyond the rectangle.
        BitmapPorts::ScaleAlpha32(Data, Columns, Rows, RowSkip, Table);
    }

    void TGraphBufGR::FlipHorizontal16() {
        WindowsSdk::PWORD LeftPixel{};
        WindowsSdk::PWORD RightPixel{};
        std::uint16_t Temp{};
        std::int32_t X{};
        std::int32_t Y{};
        LockTexture(false);
        WindowsSdk::PWORD LeftRow = static_cast<WindowsSdk::PWORD>(Pixels);
        WindowsSdk::PWORD RightRow = static_cast<WindowsSdk::PWORD>(static_cast<void*>(static_cast<std::uint8_t*>(Pixels) + (Width - 1) * static_cast<std::int32_t>(sizeof(std::uint16_t))));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
            LeftPixel = LeftRow;
            RightPixel = RightRow;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::shr(Width, 1) - 1); cpp_range_2.next(X); ) {
                Temp = pas::load_unaligned<std::uint16_t>(RightPixel);
                pas::store_unaligned<std::uint16_t>(RightPixel, pas::load_unaligned<std::uint16_t>(LeftPixel));
                pas::store_unaligned<std::uint16_t>(LeftPixel, Temp);
                ++LeftPixel;
                --RightPixel;
            }
            LeftRow = reinterpret_cast<WindowsSdk::PWORD>(reinterpret_cast<std::uint8_t*>(LeftRow) + PitchBytes);
            RightRow = reinterpret_cast<WindowsSdk::PWORD>(reinterpret_cast<std::uint8_t*>(RightRow) + PitchBytes);
        }
    }

    void TGraphBufGR::RotateLeft16() {
        std::int32_t X{};
        if (StorageKind == 1 || static_cast<std::uint32_t>(Width) < 1 || static_cast<std::uint32_t>(Height) < 1) {
            return;
        }
        std::int32_t NewWidth = Height;
        std::int32_t NewHeight = Width;
        std::int32_t NewPitch = NewWidth * static_cast<std::int32_t>(sizeof(std::uint16_t));
        void* NewPixels = EC_Mem::AllocEC(NewHeight * NewPitch);
        void* Source = Pixels;
        std::int32_t Y = Height;
        std::int32_t DestRows = NewHeight;
        void* Dest = EC_Mem::AddPointerOffset(NewPixels, (NewHeight - 1) * NewPitch);
        while (Y > 0) {
            X = Width;
            while (X > 0) {
                EC_Mem::WriteWordEC(Dest, EC_Mem::ReadWordEC(Source));
                --DestRows;
                Dest = EC_Mem::AddPointerOffset(Dest, -NewPitch);
                if (DestRows <= 0) {
                    DestRows = NewHeight;
                    Dest = EC_Mem::AddPointerOffset(Dest, NewPitch * NewHeight + static_cast<std::int32_t>(sizeof(std::uint16_t)));
                }
                Source = EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(sizeof(std::uint16_t)));
                --X;
            }
            Source = EC_Mem::AddPointerOffset(Source, PitchBytes - Width * static_cast<std::int32_t>(sizeof(std::uint16_t)));
            --Y;
        }
        EC_Mem::FreeEC(Pixels);
        Pixels = NewPixels;
        Width = NewWidth;
        Height = NewHeight;
        PitchBytes = NewPitch;
    }

    void TGraphBufGR::Stretch16(std::uint32_t Width, std::uint32_t Height) {
        if (Width == static_cast<std::uint32_t>(this->Width) && Height == static_cast<std::uint32_t>(this->Height)) {
            return;
        }
        if (static_cast<std::uint32_t>(this->Width) < 1 || static_cast<std::uint32_t>(this->Height) < 1 || Width < 1 || Height < 1) {
            return;
        }
        void* Data = EC_Mem::AllocEC(Width * Height * static_cast<std::int32_t>(sizeof(std::uint16_t)));
        GR_Main::Ex_OKGR_StretchGdi_WORD(Data, Width, Height, Pixels, this->Width, this->Height);
        EC_Mem::FreeEC(Pixels);
        Pixels = Data;
        this->Width = Width;
        this->Height = Height;
        PitchBytes = Width * static_cast<std::int32_t>(sizeof(std::uint16_t));
    }

    void TGraphBufGR::ConvertRgbTo565() {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        void* Data{};
        std::int32_t NewPitch{};
        Direct3D9::IDirect3DTexture9 NewTexture{};
        Direct3D9::TD3DLockedRect Locked{};
        if (static_cast<std::uint32_t>(Width) < 1 || static_cast<std::uint32_t>(Height) < 1) {
            return;
        }
        if (UseTexture) {
            NewTexture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            Direct3D9::IDirect3DTexture9_LockRect(NewTexture, 0u, Locked, nullptr, 0u);
            NewPitch = Locked.Pitch;
            Data = Locked.Bits;
        } else {
            NewPitch = Width * static_cast<std::int32_t>(sizeof(std::uint16_t));
            Data = EC_Mem::AllocEC(Height * NewPitch);
        }
        GR_Main::Ex_OKGF_ConvertRGBto565(Pixels, Data, NewPitch, Width, Height);
        if (StorageKind == 0 && Pixels != nullptr) {
            EC_Mem::FreeEC(Pixels);
        }
        if (UseTexture) {
            Direct3D9::IDirect3DTexture9_UnlockRect(NewTexture, 0u);
            Texture = NewTexture;
        } else {
            Pixels = Data;
        }
        PitchBytes = NewPitch;
        BitsPerPixel = 16;
        BytesPerPixel = static_cast<std::int32_t>(sizeof(std::uint16_t));
    }

    void TGraphBufGR::Convert565ToRgb() {
        if (static_cast<std::uint32_t>(Width) < 1 || static_cast<std::uint32_t>(Height) < 1) {
            return;
        }
        LockTexture(true);
        std::int32_t NewPitch = Width * 3;
        void* Data = EC_Mem::AllocEC(Height * NewPitch);
        GR_Main::Ex_OKGF_Convert565toRGB(Pixels, PitchBytes, Data, NewPitch, Width, Height);
        UnlockTexture();
        if (UsesTextureStorage) {
            Texture = nullptr;
            UsesTextureStorage = false;
        } else if (StorageKind == 0 && Pixels != nullptr) {
            EC_Mem::FreeEC(Pixels);
        }
        Pixels = Data;
        PitchBytes = NewPitch;
        BitsPerPixel = 24;
        BytesPerPixel = 3;
    }

    void TGraphBufGR::ShiftLight16(std::int32_t Shift, WindowsSdk::TRect Rect) {
        LockTexture(false);
        if (Pixels == nullptr) {
            return;
        }
        GR_Main::Ex_OKGR_ShrLight_16(EC_Mem::AddPointerOffset(Pixels, Rect.Left * static_cast<std::int32_t>(sizeof(std::uint16_t)) + Rect.Top * PitchBytes), PitchBytes, Rect.Right - Rect.Left, Rect.Bottom - Rect.Top, Shift);
    }

    void TGraphBufGR::DrawCircle16(WindowsSdk::TPoint Center, std::int32_t Radius, std::uint32_t OutlineColor, std::uint32_t FillColor, WindowsSdk::TRect Clip) {
        WindowsSdk::TRect InclusiveClip{};
        LockTexture(false);
        InclusiveClip.Left = Clip.Left;
        InclusiveClip.Top = Clip.Top;
        InclusiveClip.Right = Clip.Right - 1;
        InclusiveClip.Bottom = Clip.Bottom - 1;
        GR_Main::Ex_OKGR_Circle_DrawFillClip_WORD(Pixels, PitchBytes, Center.X, Center.Y, Radius, FillColor, InclusiveClip);
        if (OutlineColor != FillColor) {
            GR_Main::Ex_OKGR_Circle_DrawClip_WORD(Pixels, PitchBytes, Center.X, Center.Y, Radius, OutlineColor, InclusiveClip);
        }
    }

    void TGraphBufGR::DrawCircle8(WindowsSdk::TPoint Center, std::int32_t Radius, std::uint32_t OutlineColor, std::uint32_t FillColor, WindowsSdk::TRect Clip) {
        WindowsSdk::TRect InclusiveClip{};
        LockTexture(false);
        InclusiveClip.Left = Clip.Left;
        InclusiveClip.Top = Clip.Top;
        InclusiveClip.Right = Clip.Right - 1;
        InclusiveClip.Bottom = Clip.Bottom - 1;
        GR_Main::Ex_OKGR_Circle_DrawFillClip_BYTE(Pixels, PitchBytes, Center.X, Center.Y, Radius, FillColor, InclusiveClip);
        if (OutlineColor != FillColor) {
            GR_Main::Ex_OKGR_Circle_DrawClip_BYTE(Pixels, PitchBytes, Center.X, Center.Y, Radius, OutlineColor, InclusiveClip);
        }
    }

    void TGraphBufGR::ApplyOperations(const pas::WideString& Operations) {
        std::int32_t i{};
        pas::WideString Part{};
        pas::WideString Value{};
        std::int32_t Count = EC_Str::CountDelimitedPartsW(pas::view(Operations), u"&"sv);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Part = EC_Str::ExtractDelimitedPartW(pas::view(Operations), i, u"&"sv);
            if (EC_Str::CountDelimitedPartsW(pas::view(Part), u"="sv) <= 1) {
                if (Part == u"270") {
                    RotateLeft16();
                }
            } else {
                Value = EC_Str::ExtractDelimitedPartW(pas::view(Part), 0, u"="sv);
                if (Value == u"Stretch") {
                    Value = EC_Str::ExtractDelimitedPartW(pas::view(Part), 1, u"="sv);
                    if (EC_Str::CountDelimitedPartsW(pas::view(Value), u","sv) > 1) {
                        std::uint32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Value), 1, u","sv)));
                        std::uint32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Value), 0, u","sv)));
                        Stretch16(strToInt_2, strToInt);
                    }
                }
            }
        }
    }

    void TGraphBufGR::RescaleRgb(std::int32_t Width, std::int32_t Height) {
        if (static_cast<std::uint32_t>(this->Width) < 1 || static_cast<std::uint32_t>(this->Height) < 1 || Width < 1 || Height < 1) {
            return;
        }
        if (this->Width == Width && this->Height == Height) {
            return;
        }
        void* Data = EC_Mem::AllocEC(Width * 3 * Height);
        GR_Main::Ex_OKGF_Rescale(Data, Width, Height, Width * 3, Pixels, this->Width, this->Height, PitchBytes, 3, 5);
        if (StorageKind == 0) {
            EC_Mem::FreeEC(Pixels);
        }
        StorageKind = 0;
        Pixels = Data;
        this->Width = Width;
        this->Height = Height;
        PitchBytes = Width * 3;
    }

    void TGraphBufGR::RescaleRgba(std::int32_t Width, std::int32_t Height, std::int32_t Filter) {
        if (static_cast<std::uint32_t>(this->Width) < 1 || static_cast<std::uint32_t>(this->Height) < 1 || Width < 1 || Height < 1) {
            return;
        }
        if (this->Width == Width && this->Height == Height) {
            return;
        }
        void* Data = EC_Mem::AllocEC(Width * static_cast<std::int32_t>(sizeof(TColorRGBA)) * Height);
        GR_Main::Ex_OKGF_Rescale(Data, Width, Height, Width * static_cast<std::int32_t>(sizeof(TColorRGBA)), Pixels, this->Width, this->Height, PitchBytes, 4, Filter);
        if (StorageKind == 0) {
            EC_Mem::FreeEC(Pixels);
        }
        StorageKind = 0;
        Pixels = Data;
        this->Width = Width;
        this->Height = Height;
        PitchBytes = Width * static_cast<std::int32_t>(sizeof(TColorRGBA));
    }

    void TGraphBufGR::RescaleBilinearRgba(std::int32_t Width, std::int32_t Height) {
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t SourceX{};
        std::int32_t PixelX{};
        std::int32_t FractionX{};
        std::int32_t BottomWeight{};
        std::int32_t TopWeight{};
        std::int32_t TopLeftWeight{};
        std::int32_t TopRightWeight{};
        std::int32_t BottomLeftWeight{};
        std::int32_t BottomRightWeight{};
        PColorRGBAArray TopRow{};
        PColorRGBAArray BottomRow{};
        if (static_cast<std::uint32_t>(this->Width) < 1 || static_cast<std::uint32_t>(this->Height) < 1 || Width < 1 || Height < 1) {
            return;
        }
        if (this->Width == Width && this->Height == Height) {
            return;
        }
        void* Data = EC_Mem::AllocEC(Width * static_cast<std::int32_t>(sizeof(TColorRGBA)) * Height);
        std::int32_t YPosition = 0;
        std::int32_t XStep = pas::idiv(pas::shl(this->Width - 1, 16), Width);
        std::int32_t YStep = pas::idiv(pas::shl(this->Height - 1, 16), Height);
        PColorRGBA Dest = static_cast<PColorRGBA>(Data);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
            SourceX = pas::shr(YPosition, 16);
            TopRow = static_cast<PColorRGBAArray>(EC_Mem::AddPointerOffset(Pixels, PitchBytes * SourceX));
            if (this->Height - 1 > SourceX) {
                ++SourceX;
            }
            BottomRow = static_cast<PColorRGBAArray>(EC_Mem::AddPointerOffset(Pixels, PitchBytes * SourceX));
            SourceX = 0;
            BottomWeight = (YPosition & 0x0000ffff) + 1;
            TopWeight = (~YPosition & 0x0000ffff) + 1;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Width - 1); cpp_range_2.next(X); ) {
                PixelX = pas::shr(SourceX, 16);
                FractionX = SourceX & 0x0000ffff;
                TopRightWeight = pas::shr(TopWeight * FractionX, 16);
                TopLeftWeight = TopWeight - TopRightWeight;
                BottomRightWeight = pas::shr(BottomWeight * FractionX, 16);
                BottomLeftWeight = BottomWeight - BottomRightWeight;
                Dest->R = pas::shr(pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, R))) * TopLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, R))) * TopRightWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, R))) * BottomLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, R))) * BottomRightWeight, 16);
                Dest->G = pas::shr(pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, G))) * TopLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, G))) * TopRightWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, G))) * BottomLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, G))) * BottomRightWeight, 16);
                Dest->B = pas::shr(pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, B))) * TopLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, B))) * TopRightWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, B))) * BottomLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, B))) * BottomRightWeight, 16);
                Dest->A = pas::shr(pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, A))) * TopLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(TopRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, A))) * TopRightWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, PixelX * sizeof(TColorRGBA)), offsetof(TColorRGBA, A))) * BottomLeftWeight + pas::load_unaligned<std::uint8_t>(pas::byte_offset(pas::byte_offset(BottomRow, (PixelX + 1) * sizeof(TColorRGBA)), offsetof(TColorRGBA, A))) * BottomRightWeight, 16);
                SourceX += XStep;
                ++Dest;
            }
            YPosition += YStep;
        }
        if (StorageKind == 0) {
            EC_Mem::FreeEC(Pixels);
        }
        StorageKind = 0;
        Pixels = Data;
        this->Width = Width;
        this->Height = Height;
        PitchBytes = Width * static_cast<std::int32_t>(sizeof(TColorRGBA));
    }

    void TGraphBufGR::FillPolygon32(pas::OpenArray<WindowsSdk::TPoint> Points, std::uint32_t Color) {
        auto cpp_array_copy = pas::copy_open_array(Points);
        Points = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        std::int32_t NextY{};
        std::int32_t NextLeftX{};
        std::int32_t NextRightX{};
        WindowsSdk::TRect Clip{};
        std::int32_t Count = Points.length();
        if (Count < 3) {
            return;
        }
        Clip = ClassesImports::Rect(0, 0, Width, Height);
        std::int32_t Top = 0;
        std::int32_t Bottom = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count - 1); cpp_range.next(i); ) {
            if (Points[i].Y < Points[Top].Y) {
                Top = i;
            }
            if (Points[i].Y > Points[Bottom].Y) {
                Bottom = i;
            }
        }
        if (Points[Top].Y == Points[Bottom].Y) {
            return;
        }
        std::int32_t LeftStart = Top;
        std::int32_t RightStart = Top;
        std::int32_t LeftEnd = Top;
        std::int32_t RightEnd = Top;
        std::int32_t LeftX = Points[Top].X;
        std::int32_t RightX = LeftX;
        std::int32_t Y = Points[Top].Y;
        do {
            if (Points[LeftEnd].Y == Y) {
                while (Points[LeftEnd].Y == Y) {
                    LeftStart = LeftEnd;
                    --LeftEnd;
                    if (LeftEnd < 0) {
                        LeftEnd = Count - 1;
                    }
                }
                if (Points[Top].Y == Y) {
                    LeftX = Points[LeftStart].X;
                }
            }
            if (Points[RightEnd].Y == Y) {
                while (Points[RightEnd].Y == Y) {
                    RightStart = RightEnd;
                    ++RightEnd;
                    if (RightEnd >= Count) {
                        RightEnd = 0;
                    }
                }
                if (Points[Top].Y == Y) {
                    RightX = Points[RightStart].X;
                }
            }
            if (Points[RightEnd].Y < Points[LeftEnd].Y) {
                NextY = Points[RightEnd].Y;
                NextRightX = Points[RightEnd].X;
                NextLeftX = pas::idiv((Points[RightEnd].Y - Points[LeftStart].Y) * (Points[LeftEnd].X - Points[LeftStart].X), Points[LeftEnd].Y - Points[LeftStart].Y) + Points[LeftStart].X;
            } else {
                NextY = Points[LeftEnd].Y;
                NextLeftX = Points[LeftEnd].X;
                NextRightX = pas::idiv((Points[RightEnd].X - Points[RightStart].X) * (Points[LeftEnd].Y - Points[RightStart].Y), Points[RightEnd].Y - Points[RightStart].Y) + Points[RightStart].X;
            }
            if (LeftX < RightX == NextLeftX < NextRightX) {
                GR_Main::Ex_OKGR_FillTrapezium_DWORD(Pixels, PitchBytes, RightX, LeftX, Y, NextRightX, NextLeftX, NextY, Color, Clip);
            } else {
                GR_Main::Ex_OKGR_FillTrapezium_DWORD(Pixels, PitchBytes, LeftX, RightX, Y, NextLeftX, NextRightX, NextY, Color, Clip);
            }
            LeftX = NextLeftX;
            RightX = NextRightX;
            Y = NextY;
        } while (!(Points[Bottom].Y == Y));
    }

    WindowsSdk::TPoint TGraphBufGR::GetPixelCentroid() {
        WindowsSdk::TPoint Result{};
        std::int32_t X{};
        std::int32_t Y{};
        Result.X = 0;
        Result.Y = 0;
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Width - 1); cpp_range_2.next(X); ) {
                if (GetPixel32(X, Y) != 0) {
                    Result.X += X;
                    Result.Y += Y;
                    ++Count;
                }
            }
        }
        if (Count < 1) {
            Result = ClassesImports::Point(0, 0);
        } else {
            std::int32_t cpp_arg = pas::idiv(Result.Y, Count);
            std::int32_t cpp_arg_2 = pas::idiv(Result.X, Count);
            Result = ClassesImports::Point(cpp_arg_2, cpp_arg);
        }
        return Result;
    }

    void TGraphBufGR_CopyRect32(TGraphBufGR* Self, WindowsSdk::TPoint Dest, TGraphBufGR* Source, WindowsSdk::TRect Rect) {
        std::uint8_t* cpp_left = static_cast<std::uint8_t*>(Source->GetPixels());
        void* Src = cpp_left + (Rect.Top * Source->PitchBytes + Rect.Left * static_cast<std::int32_t>(sizeof(TColorRGBA)));
        std::int32_t Columns = Rect.Right - Rect.Left;
        std::int32_t Rows = Rect.Bottom - Rect.Top;
        if (Columns == 0 || Rows == 0) {
            return;
        }
        std::int32_t SrcSkip = Source->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        std::uint8_t* cpp_left_2 = static_cast<std::uint8_t*>(Self->GetPixels());
        void* Dst = cpp_left_2 + (Dest.Y * Self->PitchBytes + Dest.X * static_cast<std::int32_t>(sizeof(TColorRGBA)));
        std::int32_t DstSkip = Self->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        BitmapPorts::CopyPixels32(Src, Dst, Columns, Rows, SrcSkip, DstSkip);
    }

    void TGraphBufGR_BlendRect32(TGraphBufGR* Self, WindowsSdk::TPoint Dest, TGraphBufGR* Source, WindowsSdk::TRect Rect) {
        std::uint8_t* cpp_left = static_cast<std::uint8_t*>(Source->GetPixels());
        void* Src = cpp_left + (Rect.Top * Source->PitchBytes + Rect.Left * static_cast<std::int32_t>(sizeof(TColorRGBA)));
        std::int32_t Columns = Rect.Right - Rect.Left;
        std::int32_t Rows = Rect.Bottom - Rect.Top;
        std::int32_t SrcSkip = Source->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        std::uint8_t* cpp_left_2 = static_cast<std::uint8_t*>(Self->GetPixels());
        void* Dst = cpp_left_2 + (Dest.Y * Self->PitchBytes + Dest.X * static_cast<std::int32_t>(sizeof(TColorRGBA)));
        std::int32_t DstSkip = Self->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        void* Table = GR_Main::Ex_OKGF_MulTable256x256();
        // Native precondition: Columns and Rows must be positive. Neither is checked
        // before access; zero wraps on DEC and the loop overruns the rectangle buffers.
        BitmapPorts::BlendPixels32(Src, Dst, Columns, Rows, SrcSkip, DstSkip, Table);
    }

    void TGraphBufGR_MakeShadow(TGraphBufGR* Self) {
        void* Data = Self->GetPixels();
        std::int32_t Columns = Self->Width;
        std::int32_t Rows = Self->Height;
        std::int32_t RowSkip = Self->PitchBytes - Columns * static_cast<std::int32_t>(sizeof(TColorRGBA));
        // Native precondition: Width and Height must be positive. Neither is checked
        // before access; zero wraps on DEC and the loop overruns the pixel buffer.
        BitmapPorts::ShadowPixels32(Data, Columns, Rows, RowSkip);
    }

    // Replaces Buffer with width, height, pitch and raw pixels. Leaves Position at 12, before the pixel data.
    void TGraphBufGR::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->Clear();
        Buffer->AddDWord(Width);
        Buffer->AddDWord(Height);
        Buffer->AddDWord(PitchBytes);
        std::int32_t Size = PitchBytes * Height;
        Buffer->SetSize(Buffer->DataSize + Size);
        {
            void* pixels = GetPixels();
            void* addPointerOffset = EC_Mem::AddPointerOffset(Buffer->Data, Buffer->Position);
            Windows::CopyMemory(addPointerOffset, pixels, Size);
        }
    }

    // Accepts zlib-packed or raw buffer data. Allocates software pixels and leaves Position immediately after the 12-byte image header.
    void TGraphBufGR::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        Clear();
        Buffer->ExpandZlibPayloadInPlace();
        Width = EC_Buf::TBufEC_GetUInt32(Buffer);
        Height = EC_Buf::TBufEC_GetUInt32(Buffer);
        PitchBytes = EC_Buf::TBufEC_GetUInt32(Buffer);
        std::int32_t Size = PitchBytes * Height;
        if (Buffer->DataSize - Buffer->Position < Size) {
            GR_Main::RaiseWideMessage(u"load bin"_wref.get());
        }
        Pixels = EC_Mem::AllocEC(Size);
        {
            void* addPointerOffset = EC_Mem::AddPointerOffset(Buffer->Data, Buffer->Position);
            void* pixels = GetPixels();
            Windows::CopyMemory(pixels, addPointerOffset, Size);
        }
        BytesPerPixel = pas::idiv(static_cast<std::uint32_t>(PitchBytes), static_cast<std::uint32_t>(Width));
        BitsPerPixel = BytesPerPixel * 8;
    }

    // Assumes four-byte BGRA pixels. Converts FileName to ANSI and ignores the writer's status.
    void TGraphBufGR::SavePng(pas::WideString FileName) {
        pas::AnsiString cpp_text{};
        LockTexture(true);
        {
            std::int32_t width = Width;
            std::int32_t height = Height;
            void* pixels = GetPixels();
            std::uint8_t* cpp_arg = (cpp_text = static_cast<pas::AnsiString>(FileName), cpp_text.pchar());
            std::int32_t pitchBytes = PitchBytes;
            GR_Main::WritePngFile(cpp_arg, pixels, pitchBytes, width, height, 1, 1);
        }
    }

    // Assumes 32-bit BGRA pixels; alpha is excluded. Converts FileName to ANSI and ignores the writer's status.
    void TGraphBufGR::SaveBmp(pas::WideString FileName) {
        pas::AnsiString cpp_text{};
        LockTexture(true);
        {
            std::int32_t width = Width;
            std::int32_t height = Height;
            void* pixels = GetPixels();
            std::uint8_t* cpp_arg = (cpp_text = static_cast<pas::AnsiString>(FileName), cpp_text.pchar());
            std::int32_t pitchBytes = PitchBytes;
            GR_Main::WriteBmpFile(cpp_arg, pixels, pitchBytes, 32, 0x00ff0000u, 0x0000ff00u, 0x000000ffu, 0u, width, height);
        }
    }

    // Writes an intermediate BMP to FileName, then replaces it with JPEG. Quality is truncated to one byte; exceptions after the BMP write are swallowed.
    void TGraphBufGR::SaveJpeg(pas::WideString FileName, std::int32_t Quality) {
        pas::Bitmap* Bitmap{};
        pas::JpegImage* Image{};
        SaveBmp(FileName);
        try {
            Image = pas::make_object<pas::JpegImage>();
            {
                try {
                    Bitmap = pas::make_object<pas::Bitmap>();
                    {
                        try {
                            pas::graphic_load(Bitmap, static_cast<pas::AnsiString>(FileName));
                            pas::jpeg_assign(Image, static_cast<pas::Persistent*>(Bitmap));
                        } catch (...) {
                            pas::free(Bitmap);
                            throw;
                        }
                        pas::free(Bitmap);
                    }
                    Image->Quality = Quality;
                    pas::jpeg_compress(Image);
                    pas::graphic_save(Image, static_cast<pas::AnsiString>(FileName));
                } catch (...) {
                    pas::free(Image);
                    throw;
                }
                pas::free(Image);
            }
        } catch (...) {
        }
    }

    void TGraphBufGR::DrawNinePatch(std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, TGraphBufGR* Source, WindowsSdk::TRect SourceRect, WindowsSdk::TRect Borders) {
        WindowsSdk::TRect TileRect{};
        auto TilePatch = [&](std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Rect) -> void {
            std::int32_t TileX{};
            std::int32_t TileWidth = Rect.Right - Rect.Left;
            if (TileWidth <= 0) {
                return;
            }
            std::int32_t TileHeight = Rect.Bottom - Rect.Top;
            if (TileHeight <= 0) {
                return;
            }
            std::int32_t TileY = 0;
            while (TileY < Height) {
                if (TileY + TileHeight > Height) {
                    Rect.Bottom = Height - TileY + Rect.Top;
                }
                TileX = 0;
                while (TileX < Width) {
                    if (TileX + TileWidth > Width) {
                        GR_GraphBuf::TGraphBufGR_CopyRect32(this, ClassesImports::Point(X + TileX, Y + TileY), Source, ClassesImports::Rect(Rect.Left, Rect.Top, Width - TileX + Rect.Left, Rect.Bottom));
                    } else {
                        GR_GraphBuf::TGraphBufGR_CopyRect32(this, ClassesImports::Point(X + TileX, Y + TileY), Source, Rect);
                    }
                    TileX += TileWidth;
                }
                TileY += TileHeight;
            }
        };
        if (Width <= 0) {
            Width = this->Width - X;
        }
        if (Height <= 0) {
            Height = this->Height - Y;
        }
        if (SourceRect.Right <= SourceRect.Left) {
            SourceRect.Right = Source->Width;
        }
        if (SourceRect.Bottom <= SourceRect.Top) {
            SourceRect.Bottom = Source->Height;
        }
        TileRect = ClassesImports::Rect(SourceRect.Left, SourceRect.Top, SourceRect.Left + Borders.Left, SourceRect.Top + Borders.Top);
        GR_GraphBuf::TGraphBufGR_CopyRect32(this, ClassesImports::Point(X, Y), Source, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Right - Borders.Right, SourceRect.Top, SourceRect.Right, SourceRect.Top + Borders.Top);
        GR_GraphBuf::TGraphBufGR_CopyRect32(this, ClassesImports::Point(X + Width - Borders.Right, Y), Source, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Left, SourceRect.Bottom - Borders.Bottom, SourceRect.Left + Borders.Left, SourceRect.Bottom);
        GR_GraphBuf::TGraphBufGR_CopyRect32(this, ClassesImports::Point(X, Y + Height - Borders.Bottom), Source, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Right - Borders.Right, SourceRect.Bottom - Borders.Bottom, SourceRect.Right, SourceRect.Bottom);
        GR_GraphBuf::TGraphBufGR_CopyRect32(this, ClassesImports::Point(X + Width - Borders.Right, Y + Height - Borders.Bottom), Source, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Left + Borders.Left, SourceRect.Top, SourceRect.Right - Borders.Right, SourceRect.Top + Borders.Top);
        TilePatch(X + Borders.Left, Y, Width - Borders.Left - Borders.Right, Borders.Top, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Left + Borders.Left, SourceRect.Bottom - Borders.Bottom, SourceRect.Right - Borders.Right, SourceRect.Bottom);
        TilePatch(X + Borders.Left, Y + Height - Borders.Bottom, Width - Borders.Left - Borders.Right, Borders.Bottom, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Left, SourceRect.Top + Borders.Top, SourceRect.Left + Borders.Left, SourceRect.Bottom - Borders.Bottom);
        TilePatch(X, Y + Borders.Top, Borders.Left, Height - Borders.Top - Borders.Bottom, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Right - Borders.Right, SourceRect.Top + Borders.Top, SourceRect.Right, SourceRect.Bottom - Borders.Bottom);
        TilePatch(X + Width - Borders.Right, Y + Borders.Top, Borders.Right, Height - Borders.Top - Borders.Bottom, TileRect);
        TileRect = ClassesImports::Rect(SourceRect.Left + Borders.Left, SourceRect.Top + Borders.Top, SourceRect.Right - Borders.Right, SourceRect.Bottom - Borders.Bottom);
        TilePatch(X + Borders.Left, Y + Borders.Top, Width - Borders.Left - Borders.Right, Height - Borders.Top - Borders.Bottom, TileRect);
    }

    void TGraphBufGR::RescaleWithAspect(std::uint32_t Width, std::uint32_t Height, std::uint8_t CropToAspect, std::int32_t HorizontalAlign, std::int32_t VerticalAlign, std::int32_t Filter) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::uint32_t CropWidth{};
        std::uint32_t CropHeight{};
        void* Source{};
        void* Dest{};
        std::int32_t SourcePitch{};
        std::int32_t DestPitch{};
        Direct3D9::IDirect3DTexture9 NewTexture{};
        Direct3D9::TD3DLockedRect Locked{};
        if (Width == static_cast<std::uint32_t>(this->Width) && Height == static_cast<std::uint32_t>(this->Height)) {
            return;
        }
        std::uint32_t Left = 0u;
        std::uint32_t Top = 0u;
        if (CropToAspect) {
            CropWidth = this->Width;
            CropHeight = System::Round(pas::real_divide(Height, Width) * static_cast<std::uint32_t>(this->Width));
            if (CropHeight > static_cast<std::uint32_t>(this->Height)) {
                CropWidth = System::Round(pas::real_divide(Width, Height) * static_cast<std::uint32_t>(this->Height));
                CropHeight = this->Height;
            }
            if (HorizontalAlign == 1) {
                Left = static_cast<std::uint32_t>(this->Width) - CropWidth >> 1;
            } else if (HorizontalAlign == 2) {
                Left = static_cast<std::uint32_t>(this->Width) - CropWidth;
            } else {
                Left = 0u;
            }
            if (VerticalAlign == 1) {
                Top = static_cast<std::uint32_t>(this->Height) - CropHeight >> 1;
            } else if (VerticalAlign == 2) {
                Top = static_cast<std::uint32_t>(this->Height) - CropHeight;
            } else {
                Top = 0u;
            }
            if (Width == static_cast<std::uint32_t>(this->Width) && Height < static_cast<std::uint32_t>(this->Height) || Height == static_cast<std::uint32_t>(this->Height) && Width < static_cast<std::uint32_t>(this->Width)) {
                Crop(ClassesImports::Rect(Left, Top, Left + CropWidth, Top + CropHeight));
                return;
            }
        } else {
            CropWidth = this->Width;
            CropHeight = this->Height;
        }
        if (UseTexture && BitsPerPixel == 32) {
            NewTexture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            Direct3D9::IDirect3DTexture9_LockRect(NewTexture, 0u, Locked, nullptr, 0u);
            Dest = Locked.Bits;
            DestPitch = Locked.Pitch;
        } else {
            DestPitch = Width * BytesPerPixel;
            Dest = EC_Mem::AllocEC(Height * DestPitch);
        }
        if (UsesTextureStorage && Texture != nullptr) {
            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
            Source = Locked.Bits;
            SourcePitch = Locked.Pitch;
        } else {
            Source = Pixels;
            SourcePitch = PitchBytes;
        }
        GR_Main::Ex_OKGF_Rescale(Dest, Width, Height, DestPitch, EC_Mem::AddPointerOffset(Source, Top * SourcePitch + Left * BytesPerPixel), CropWidth, CropHeight, SourcePitch, BytesPerPixel, Filter);
        if (UseTexture && BitsPerPixel == 32) {
            if (!KeepTextureUntilReplacement) {
                Texture = nullptr;
            }
            Direct3D9::IDirect3DTexture9_UnlockRect(NewTexture, 0u);
            Texture = NewTexture;
            TextureLocked = false;
            UsesTextureStorage = true;
            Pixels = nullptr;
        } else {
            if (StorageKind == 0) {
                EC_Mem::FreeEC(Pixels);
            }
            StorageKind = 0;
            Pixels = Dest;
            Texture = nullptr;
        }
        this->Width = Width;
        this->Height = Height;
        PitchBytes = DestPitch;
    }

    // Alignment values: 0=start, 1=center, 2=end.
    void TGraphBufGR::RescaleRGBA_HW(std::uint32_t Width, std::uint32_t Height, std::uint8_t CropToAspect, std::int32_t HorizontalAlign, std::int32_t VerticalAlign) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::uint32_t Left{};
        std::uint32_t Top{};
        std::uint32_t CropWidth{};
        std::uint32_t CropHeight{};
        void* Dest{};
        std::int32_t DestPitch{};
        Direct3D9::IDirect3DTexture9 Staging{};
        Direct3D9::IDirect3DTexture9 DeviceTexture{};
        Direct3D9::IDirect3DSurface9 SourceSurface{};
        Direct3D9::IDirect3DSurface9 TargetSurface{};
        Direct3D9::TD3DLockedRect Locked{};
        WindowsSdk::TRect SourceRect{};
        if (Width == static_cast<std::uint32_t>(this->Width) && Height == static_cast<std::uint32_t>(this->Height)) {
            return;
        }
        if (CropToAspect) {
            CropWidth = this->Width;
            CropHeight = System::Round(pas::real_divide(Height, Width) * static_cast<std::uint32_t>(this->Width));
            if (CropHeight > static_cast<std::uint32_t>(this->Height)) {
                CropWidth = System::Round(pas::real_divide(Width, Height) * static_cast<std::uint32_t>(this->Height));
                CropHeight = this->Height;
            }
            if (HorizontalAlign == 1) {
                Left = static_cast<std::uint32_t>(this->Width) - CropWidth >> 1;
            } else if (HorizontalAlign == 2) {
                Left = static_cast<std::uint32_t>(this->Width) - CropWidth;
            } else {
                Left = 0u;
            }
            if (VerticalAlign == 1) {
                Top = static_cast<std::uint32_t>(this->Height) - CropHeight >> 1;
            } else if (VerticalAlign == 2) {
                Top = static_cast<std::uint32_t>(this->Height) - CropHeight;
            } else {
                Top = 0u;
            }
            SourceRect = ClassesImports::Rect(Left, Top, Left + CropWidth, Top + CropHeight);
            if (Width == static_cast<std::uint32_t>(this->Width) && Height < static_cast<std::uint32_t>(this->Height) || Height == static_cast<std::uint32_t>(this->Height) && Width < static_cast<std::uint32_t>(this->Width)) {
                Crop(SourceRect);
                return;
            }
        } else {
            SourceRect = ClassesImports::Rect(0, 0, this->Width, this->Height);
        }
        LockTexture(true);
        {
            Direct3D9::IDirect3DTexture9& staging = pas::out_interface(Staging);
            std::uint32_t height = this->Height;
            std::uint32_t width = this->Width;
            pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice = GR_Main::Direct3DDevice;
            Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice, width, height, 1u, 0u, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_SYSTEMMEM, staging, nullptr);
        }
        Direct3D9::IDirect3DTexture9_LockRect(Staging, 0u, Locked, nullptr, 0u);
        Windows::CopyMemory(Locked.Bits, Pixels, this->Height * PitchBytes);
        Direct3D9::IDirect3DTexture9_UnlockRect(Staging, 0u);
        UnlockTexture();
        if (UsesTextureStorage && Texture != nullptr) {
            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
        }
        {
            Direct3D9::IDirect3DTexture9& deviceTexture = pas::out_interface(DeviceTexture);
            std::uint32_t height_2 = this->Height;
            std::uint32_t width_2 = this->Width;
            pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_2 = GR_Main::Direct3DDevice;
            Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice_2, width_2, height_2, 1u, 0u, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_DEFAULT, deviceTexture, nullptr);
        }
        if (([&] {
            pas::ComView<Direct3D9::IDirect3DBaseTexture9_Tag> deviceTexture_2 = DeviceTexture;
            pas::ComView<Direct3D9::IDirect3DBaseTexture9_Tag> staging_2 = Staging;
            return Direct3D9::IDirect3DDevice9_UpdateTexture(GR_Main::Direct3DDevice, staging_2, deviceTexture_2);
        }()) != 0) {
            GR_Main::AppendLogLineThreadSafe("TGraphBufGR.RescaleRGBA_HW(...)::UpdateTexture fail"_a);
        }
        Staging = nullptr;
        {
            Direct3D9::IDirect3DSurface9& targetSurface = pas::out_interface(TargetSurface);
            pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_3 = GR_Main::Direct3DDevice;
            Direct3D9::IDirect3DDevice9_CreateRenderTarget(direct3DDevice_3, Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DMULTISAMPLE_NONE, 0u, 0, targetSurface, nullptr);
        }
        {
            Direct3D9::IDirect3DSurface9& sourceSurface = pas::out_interface(SourceSurface);
            pas::ComView<Direct3D9::IDirect3DTexture9_Tag> deviceTexture_3 = DeviceTexture;
            Direct3D9::IDirect3DTexture9_GetSurfaceLevel(deviceTexture_3, 0u, sourceSurface);
        }
        if (Direct3D9::IDirect3DDevice9_StretchRect(GR_Main::Direct3DDevice, SourceSurface, reinterpret_cast<Types::PRect>(&SourceRect), TargetSurface, nullptr, Direct3D9::D3DTEXF_LINEAR) != 0) {
            GR_Main::AppendLogLineThreadSafe("TGraphBufGR.RescaleRGBA_HW(...)::StretchRect fail"_a);
        }
        SourceSurface = nullptr;
        DeviceTexture = nullptr;
        {
            Direct3D9::IDirect3DTexture9& staging_3 = pas::out_interface(Staging);
            pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_4 = GR_Main::Direct3DDevice;
            Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice_4, Width, Height, 1u, 0u, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_SYSTEMMEM, staging_3, nullptr);
        }
        {
            Direct3D9::IDirect3DSurface9& sourceSurface_2 = pas::out_interface(SourceSurface);
            pas::ComView<Direct3D9::IDirect3DTexture9_Tag> staging_4 = Staging;
            Direct3D9::IDirect3DTexture9_GetSurfaceLevel(staging_4, 0u, sourceSurface_2);
        }
        if (Direct3D9::IDirect3DDevice9_GetRenderTargetData(GR_Main::Direct3DDevice, TargetSurface, SourceSurface) != 0) {
            GR_Main::AppendLogLineThreadSafe("TGraphBufGR.RescaleRGBA_HW(...)::GetRenderTargetData fail"_a);
        }
        SourceSurface = nullptr;
        TargetSurface = nullptr;
        if (UseTexture) {
            if (!KeepTextureUntilReplacement) {
                Texture = nullptr;
            }
            Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
            Dest = Locked.Bits;
            DestPitch = Locked.Pitch;
            UsesTextureStorage = true;
        } else {
            DestPitch = Width * static_cast<std::int32_t>(sizeof(TColorRGBA));
            Dest = EC_Mem::AllocEC(Height * DestPitch);
        }
        Direct3D9::IDirect3DTexture9_LockRect(Staging, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
        Windows::CopyMemory(Dest, Locked.Bits, Height * DestPitch);
        Direct3D9::IDirect3DTexture9_UnlockRect(Staging, 0u);
        Staging = nullptr;
        if (UsesTextureStorage) {
            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
            TextureLocked = false;
        }
        if (!UseTexture) {
            if (StorageKind == 0) {
                EC_Mem::FreeEC(Pixels);
            }
            StorageKind = 0;
            Pixels = Dest;
        }
        this->Width = Width;
        this->Height = Height;
        PitchBytes = DestPitch;
    }

    void TGraphBufGR::Crop(WindowsSdk::TRect Rect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        void* Dest{};
        std::int32_t NewPitch{};
        Direct3D9::IDirect3DTexture9 NewTexture{};
        Direct3D9::TD3DLockedRect Locked{};
        std::int32_t NewWidth = Rect.Right - Rect.Left;
        std::int32_t NewHeight = Rect.Bottom - Rect.Top;
        if (NewWidth <= 0 || NewHeight <= 0 || Rect.Left < 0 || Rect.Left >= Width || Rect.Right > Width || Rect.Top < 0 || Rect.Top >= Height || Rect.Bottom > Height) {
            return;
        }
        if (UseTexture) {
            if (BitsPerPixel == 16) {
                NewTexture = (GR_DX::GR_CreateTexture(NewWidth, NewHeight, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
            } else if (BitsPerPixel == 32) {
                NewTexture = (GR_DX::GR_CreateTexture(NewWidth, NewHeight, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_2), cpp_result_2);
            }
            Direct3D9::IDirect3DTexture9_LockRect(NewTexture, 0u, Locked, nullptr, 0u);
            Dest = Locked.Bits;
            NewPitch = Locked.Pitch;
        } else {
            NewPitch = NewWidth * BytesPerPixel;
            Dest = EC_Mem::AllocEC(NewHeight * NewPitch);
        }
        LockTexture(true);
        void* Source = EC_Mem::AddPointerOffset(Pixels, Rect.Top * PitchBytes + Rect.Left * BytesPerPixel);
        std::int32_t Y = 0;
        while (Y < NewHeight) {
            Windows::CopyMemory(EC_Mem::AddPointerOffset(Dest, Y * NewPitch), EC_Mem::AddPointerOffset(Source, Y * PitchBytes), NewWidth * BytesPerPixel);
            ++Y;
        }
        UnlockTexture();
        if (UseTexture) {
            Direct3D9::IDirect3DTexture9_UnlockRect(NewTexture, 0u);
            if (!KeepTextureUntilReplacement) {
                Texture = nullptr;
            }
            Texture = NewTexture;
        } else {
            if (StorageKind == 0) {
                EC_Mem::FreeEC(Pixels);
            }
            StorageKind = 0;
            Pixels = Dest;
        }
        Width = NewWidth;
        Height = NewHeight;
        PitchBytes = NewPitch;
    }

    void TGraphBufGR::AdjustBrightness(std::int32_t Percent) {
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t C{};
        std::int32_t B{};
        std::int32_t G{};
        std::int32_t R{};
        std::uint8_t* Pixel{};
        pas::Array<std::int16_t, 0, 255> Table{};
        for (C = 0; C <= 255; ++C) {
            Table[C] = (Percent + 100) * C / 100;
            Table[C] = std::max<std::int32_t>(0, std::min<std::int32_t>(255, static_cast<std::int32_t>(Table[C])));
        }
        if (BitsPerPixel == 16) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
                {
                    std::uint8_t* cpp_left = static_cast<std::uint8_t*>(GetPixels());
                    Pixel = static_cast<std::uint8_t*>(static_cast<void*>(cpp_left + PitchBytes * Y));
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Width - 1); cpp_range_2.next(X); ) {
                    C = pas::load_unaligned<std::uint16_t>(reinterpret_cast<WindowsSdk::PWORD>(Pixel));
                    B = pas::shl(C & 31, 3);
                    G = pas::shr(C, 3) & 0x000000fc;
                    R = pas::shr(C, 8) & 0x000000f8;
                    B = Table[B];
                    G = Table[G];
                    R = Table[R];
                    C = pas::shr(B, 3) | pas::shl(pas::shr(G, 2), 5) | pas::shl(pas::shr(R, 3), 11);
                    pas::store_unaligned<std::uint16_t>(reinterpret_cast<WindowsSdk::PWORD>(Pixel), static_cast<std::uint16_t>(C));
                    Pixel += static_cast<std::int32_t>(sizeof(std::uint16_t));
                }
            }
        } else if (BitsPerPixel == 32) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Height - 1); cpp_range_3.next(Y); ) {
                {
                    std::uint8_t* cpp_left_2 = static_cast<std::uint8_t*>(GetPixels());
                    Pixel = static_cast<std::uint8_t*>(static_cast<void*>(cpp_left_2 + PitchBytes * Y));
                }
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Width - 1); cpp_range_4.next(X); ) {
                    reinterpret_cast<PColorBGRA>(Pixel)->B = Table[reinterpret_cast<PColorBGRA>(Pixel)->B];
                    *(&reinterpret_cast<PColorBGRA>(Pixel)->G) = Table[*(&reinterpret_cast<PColorBGRA>(Pixel)->G)];
                    *(&reinterpret_cast<PColorBGRA>(Pixel)->R) = Table[*(&reinterpret_cast<PColorBGRA>(Pixel)->R)];
                    Pixel += static_cast<std::int32_t>(sizeof(TColorRGBA));
                }
            }
        }
    }

    void TGraphBufGR::ConvertToGrayscale() {
        std::int32_t X{};
        std::int32_t Y{};
        std::uint8_t* Pixel{};
        std::uint32_t C{};
        std::uint32_t R{};
        std::uint32_t G{};
        std::uint32_t B{};
        std::uint32_t RWeight{};
        std::uint32_t GWeight{};
        std::uint32_t BWeight{};
        if (BitsPerPixel == 16) {
            RWeight = 19595u;
            GWeight = 38469u;
            BWeight = 7471u;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
                {
                    std::uint8_t* cpp_left = static_cast<std::uint8_t*>(GetPixels());
                    Pixel = static_cast<std::uint8_t*>(static_cast<void*>(cpp_left + PitchBytes * Y));
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Width - 1); cpp_range_2.next(X); ) {
                    C = pas::load_unaligned<std::uint16_t>(reinterpret_cast<WindowsSdk::PWORD>(Pixel));
                    R = C >> 11 & 31;
                    G = (C >> 5 & 63) >> 1;
                    B = C & 31;
                    R = R * RWeight + G * GWeight + B * BWeight >> 16;
                    G = R * 2;
                    B = R;
                    C = R << 11 | G << 5 | B;
                    pas::store_unaligned<std::uint16_t>(reinterpret_cast<WindowsSdk::PWORD>(Pixel), static_cast<std::uint16_t>(C));
                    Pixel += static_cast<std::int32_t>(sizeof(std::uint16_t));
                }
            }
        } else if (BitsPerPixel == 32) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Height - 1); cpp_range_3.next(Y); ) {
                {
                    std::uint8_t* cpp_left_2 = static_cast<std::uint8_t*>(GetPixels());
                    Pixel = static_cast<std::uint8_t*>(static_cast<void*>(cpp_left_2 + PitchBytes * Y));
                }
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Width - 1); cpp_range_4.next(X); ) {
                    B = reinterpret_cast<PColorBGRA>(Pixel)->B;
                    G = *(&reinterpret_cast<PColorBGRA>(Pixel)->G);
                    R = *(&reinterpret_cast<PColorBGRA>(Pixel)->R);
                    C = System::Trunc(R * 0.299L + G * 0.587L + B * 0.114L);
                    reinterpret_cast<PColorBGRA>(Pixel)->B = C;
                    *(&reinterpret_cast<PColorBGRA>(Pixel)->G) = C;
                    *(&reinterpret_cast<PColorBGRA>(Pixel)->R) = C;
                    Pixel += static_cast<std::int32_t>(sizeof(TColorRGBA));
                }
            }
        }
    }

    void TGraphBufGR::GetTexture(Direct3D9::IDirect3DTexture9& Result) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 cpp_result_3{};
        Direct3D9::TD3DLockedRect Locked{};
        std::uint32_t Y{};
        std::int32_t RowBytes{};
        auto CopyRgbToOpaqueRgba = [&](void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height) -> void {
            std::int32_t X{};
            std::int32_t Y = 0;
            while (Y < Height) {
                X = 0;
                while (X < Width) {
                    Windows::CopyMemory(EC_Mem::AddPointerOffset(Dest, X * static_cast<std::int32_t>(sizeof(TColorRGBA))), EC_Mem::AddPointerOffset(Source, X * static_cast<std::int32_t>(sizeof(TColorRGB))), static_cast<std::int32_t>(sizeof(TColorRGB)));
                    *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorRGBA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(TColorRGBA)))))->A)))) = 255;
                    ++X;
                }
                Dest = EC_Mem::AddPointerOffset(Dest, DestPitch);
                Source = EC_Mem::AddPointerOffset(Source, SourcePitch);
                ++Y;
            }
        };
        if (static_cast<std::uint8_t>(UsesTextureStorage ^ 1) && Texture == nullptr) {
            if (BitsPerPixel == 24) {
                Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                CopyRgbToOpaqueRgba(Locked.Bits, Locked.Pitch, Pixels, PitchBytes, Width, Height);
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
            } else {
                if (BitsPerPixel == 16) {
                    Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result_2), cpp_result_2);
                } else if (BitsPerPixel == 32) {
                    Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_3), cpp_result_3);
                }
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                RowBytes = Width * BytesPerPixel;
                Y = 0u;
                while (Y < static_cast<std::uint32_t>(Height)) {
                    Windows::CopyMemory(EC_Mem::AddPointerOffset(Locked.Bits, Locked.Pitch * Y), EC_Mem::AddPointerOffset(Pixels, PitchBytes * Y), RowBytes);
                    ++Y;
                }
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
            }
        }
        UnlockTexture();
        Result = Texture;
        return;
    }

    // The byte-sized option is ignored in this build.
    void TGraphBufGR::LoadFromScreen(std::uint8_t UnusedOption) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DSurface9 Offscreen{};
        Direct3D9::IDirect3DSurface9 RenderTarget{};
        Direct3D9::TD3DLockedRect Locked{};
        std::uint32_t Y{};
        std::int32_t ErrorCode{};
        Direct3D9::TD3DSurfaceDesc Desc{};
        auto SetRowAlpha = [&](void* Pixels, std::int32_t Count, std::int32_t Alpha) -> void {
            std::int32_t X{};
            Alpha &= 0x000000ff;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(X); ) {
                *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Pixels, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorRGBA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(TColorRGBA)))))->A)))) = Alpha;
            }
        };
        try {
            if (GlobalsV::HardwareRenderingEnabled) {
                ErrorCode = ([&] {
                    Direct3D9::IDirect3DSurface9& renderTarget = pas::out_interface(RenderTarget);
                    pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice = GR_Main::Direct3DDevice;
                    return Direct3D9::IDirect3DDevice9_GetRenderTarget(direct3DDevice, 0u, renderTarget);
                }());
                if (ErrorCode == 0) {
                    ErrorCode = Direct3D9::IDirect3DSurface9_GetDesc(RenderTarget, Desc);
                    if (ErrorCode == 0) {
                        ErrorCode = ([&] {
                            Direct3D9::IDirect3DSurface9& offscreen = pas::out_interface(Offscreen);
                            std::uint32_t format = Desc.Format;
                            std::uint32_t height = Desc.Height;
                            std::uint32_t width = Desc.Width;
                            pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_2 = GR_Main::Direct3DDevice;
                            return Direct3D9::IDirect3DDevice9_CreateOffscreenPlainSurface(direct3DDevice_2, width, height, format, Direct3D9::D3DPOOL_SYSTEMMEM, offscreen, nullptr);
                        }());
                        if (ErrorCode == 0) {
                            ErrorCode = Direct3D9::IDirect3DDevice9_GetRenderTargetData(GR_Main::Direct3DDevice, RenderTarget, Offscreen);
                            if (ErrorCode == 0) {
                                Clear();
                                Width = Desc.Width;
                                Height = Desc.Height;
                                BitsPerPixel = 32;
                                BytesPerPixel = static_cast<std::int32_t>(sizeof(TColorRGBA));
                                if (UseTexture) {
                                    Texture = (GR_DX::GR_CreateTexture(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
                                    UsesTextureStorage = true;
                                    LockTexture(false);
                                } else {
                                    PitchBytes = Width * static_cast<std::int32_t>(sizeof(TColorRGBA));
                                    Pixels = EC_Mem::AllocEC(Height * PitchBytes);
                                }
                                if (Pixels != nullptr) {
                                    Direct3D9::IDirect3DSurface9_LockRect(Offscreen, Locked, nullptr, 0u);
                                    for (auto cpp_range = pas::for_to<std::uint32_t>(0u, static_cast<std::uint32_t>(Height) - 1); cpp_range.next(Y); ) {
                                        Windows::CopyMemory(EC_Mem::AddPointerOffset(Pixels, PitchBytes * Y), EC_Mem::AddPointerOffset(Locked.Bits, Locked.Pitch * Y), Width * static_cast<std::int32_t>(sizeof(TColorRGBA)));
                                        SetRowAlpha(EC_Mem::AddPointerOffset(Pixels, PitchBytes * Y), Width, 255);
                                    }
                                    Direct3D9::IDirect3DSurface9_UnlockRect(Offscreen);
                                }
                                UnlockTexture();
                            } else {
                                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGraphBufGR.LoadFromScreen()::GetRenderTargetData fail (", SysUtils::IntToStr(ErrorCode), ")"})));
                            }
                            Offscreen = nullptr;
                        } else {
                            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGraphBufGR.LoadFromScreen()::CreateOffscreenPlainSurface fail (", SysUtils::IntToStr(ErrorCode), ")"})));
                        }
                    } else {
                        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGraphBufGR.LoadFromScreen()::GetDesc fail (", SysUtils::IntToStr(ErrorCode), ")"})));
                    }
                    RenderTarget = nullptr;
                } else {
                    pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGraphBufGR.LoadFromScreen()::GetRenderTarget fail (", SysUtils::IntToStr(ErrorCode), ")"})));
                }
            } else {
                AllocateNative(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
                try {
                    void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                    std::int32_t gameScreenWidth = GR_Main::GameScreenWidth;
                    std::int32_t gameScreenHeight = GR_Main::GameScreenHeight;
                    void* pixels_2 = GetPixels();
                    std::int32_t pitchBytes_2 = PitchBytes;
                    GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels_2, pitchBytes_2, 0, 0, pixels, pitchBytes, 0, 0, gameScreenWidth, gameScreenHeight);
                } catch (...) {
                    pas::raise(pas::make_exception<pas::Exception>("Error in TGraphBufGR.LoadFromScreen()::OKGR_Copy_XY_XY_WORD"_a));
                }
            }
        } catch (...) {
            Clear();
            AllocateNative(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        }
    }

    void TGraphBufGR::LockTexture(std::uint8_t ReadOnly) {
        Direct3D9::TD3DLockedRect Locked{};
        if (TextureLockedReadOnly && static_cast<std::uint8_t>(ReadOnly ^ 1)) {
            UnlockTexture();
        }
        if (!TextureLocked) {
            if (static_cast<std::uint8_t>(UsesTextureStorage ^ 1) || Texture == nullptr) {
                TextureLocked = false;
                return;
            }
            if (ReadOnly) {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, Direct3D9::D3DLOCK_READONLY);
            } else {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
            }
            Pixels = Locked.Bits;
            PitchBytes = Locked.Pitch;
            TextureLockedReadOnly = ReadOnly;
            TextureLocked = true;
        }
    }

    void TGraphBufGR::UnlockTexture() {
        if (TextureLocked) {
            TextureLocked = false;
            TextureLockedReadOnly = false;
            if (UsesTextureStorage && Texture != nullptr) {
                Pixels = nullptr;
                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
            }
        }
    }

    void TGraphBufGR::ConvertBgraToRgb24() {
        std::uint32_t X{};
        if (static_cast<std::uint32_t>(Width) < 1) {
            return;
        }
        if (static_cast<std::uint32_t>(Height) < 1) {
            return;
        }
        LockTexture(true);
        std::int32_t NewPitch = Width * static_cast<std::int32_t>(sizeof(TColorRGB));
        void* NewPixels = EC_Mem::AllocEC(NewPitch * Height);
        void* Dest = NewPixels;
        void* Source = Pixels;
        std::uint32_t Y = 0u;
        while (Y < static_cast<std::uint32_t>(Height)) {
            X = 0u;
            while (X < static_cast<std::uint32_t>(Width)) {
                static_cast<PColorRGB>(EC_Mem::AddPointerOffset(Dest, X * static_cast<std::int32_t>(sizeof(TColorRGB))))->R = *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorBGRA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(TColorBGRA)))))->R))));
                *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorRGB>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(TColorRGB)))))->G)))) = *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorBGRA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(TColorBGRA)))))->G))));
                *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<PColorRGB>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(TColorRGB)))))->B)))) = static_cast<PColorBGRA>(EC_Mem::AddPointerOffset(Source, X * static_cast<std::int32_t>(sizeof(TColorBGRA))))->B;
                ++X;
            }
            Dest = EC_Mem::AddPointerOffset(Dest, NewPitch);
            Source = EC_Mem::AddPointerOffset(Source, PitchBytes);
            ++Y;
        }
        UnlockTexture();
        if (UsesTextureStorage) {
            Texture = nullptr;
            UsesTextureStorage = false;
        } else if (StorageKind == 0) {
            if (Pixels != nullptr) {
                EC_Mem::FreeEC(Pixels);
            }
        }
        Pixels = NewPixels;
        PitchBytes = NewPitch;
        BitsPerPixel = 24;
        BytesPerPixel = 3;
    }

    void TGraphBufGR::DrawAntialiasedCircle16(WindowsSdk::TPoint Center, std::int32_t Radius, std::uint32_t Color, WindowsSdk::TRect Clip) {
        std::int32_t SignX{};
        std::int32_t SignY{};
        float Coverage{};
        auto PlotCirclePixel16 = [&](std::int32_t X, std::int32_t Y, std::int32_t Alpha) -> void {
            if (X >= Clip.Left && X < Clip.Right && Y >= Clip.Top && Y < Clip.Bottom) {
                BlendPixel16(X, Y, Color, Alpha);
            }
        };
        std::int32_t X = Radius;
        std::int32_t PreviousX = Radius;
        std::int32_t Y = 0;
        float PreviousCoverage = 0.0f;
        std::int32_t Quadrant = 0;
        while (Quadrant < 4) {
            SignX = 2 * (Quadrant % 2) - 1;
            SignY = 2 * (Quadrant / 2 % 2) - 1;
            PlotCirclePixel16(Center.X + SignX * X, Center.Y + SignY * Y, 255);
            PlotCirclePixel16(Center.X + SignX * Y, Center.Y + SignY * X, 255);
            ++Quadrant;
        }
        while (X > Y) {
            ++Y;
            Coverage = System::Sqrt(pas::sqr(Radius) - pas::sqr(Y));
            Coverage = static_cast<long double>(MathImports::Ceil(Coverage)) - Coverage;
            if (Coverage < PreviousCoverage) {
                --X;
            }
            if (X < Y) {
                break;
            }
            if (X == Y && PreviousX == X) {
                break;
            }
            Quadrant = 0;
            while (Quadrant < 4) {
                SignX = 2 * (Quadrant % 2) - 1;
                SignY = 2 * (Quadrant / 2 % 2) - 1;
                PlotCirclePixel16(Center.X + SignX * X, Center.Y + SignY * Y, System::Trunc((1.0L - Coverage) * 255.0L));
                PlotCirclePixel16(Center.X + SignX * Y, Center.Y + SignY * X, System::Trunc((1.0L - Coverage) * 255.0L));
                if (X - 1 >= Y) {
                    PlotCirclePixel16(Center.X + (X - 1) * SignX, Center.Y + SignY * Y, System::Trunc(255.0L * Coverage));
                    PlotCirclePixel16(Center.X + SignX * Y, Center.Y + (X - 1) * SignY, System::Trunc(255.0L * Coverage));
                }
                ++Quadrant;
            }
            PreviousCoverage = Coverage;
            PreviousX = X;
        }
    }

    void TGraphBufGR::DrawAntialiasedLine16(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::TRect Clip) {
        std::uint8_t Steep{};
        double Temp{};
        auto LineFraction16 = [&](double Value) -> double {
            return static_cast<long double>(Value) - MathImports::Floor(Value);
        };
        auto PlotLinePixel16 = [&](std::int32_t X, std::int32_t Y, std::int32_t Alpha) -> void {
            if (X >= Clip.Left && X < Clip.Right && Y >= Clip.Top && Y < Clip.Bottom && Alpha > 0) {
                BlendPixel16(X, Y, Color, Alpha);
            }
        };
        double StartX = X1;
        double StartY = Y1;
        double FinishX = X2;
        double FinishY = Y2;
        double DX = static_cast<long double>(FinishX) - StartX;
        double DY = static_cast<long double>(FinishY) - StartY;
        if (DX == 0.0L && DY == 0.0L) {
            return;
        }
        if (std::fabs(static_cast<pas::Extended>(DX)) > std::fabs(static_cast<pas::Extended>(DY))) {
            Steep = false;
        } else {
            Steep = true;
            Temp = StartX;
            StartX = StartY;
            StartY = Temp;
            Temp = FinishX;
            FinishX = FinishY;
            FinishY = Temp;
            Temp = DX;
            DX = DY;
            DY = Temp;
        }
        if (StartX > FinishX) {
            Temp = StartX;
            StartX = FinishX;
            FinishX = Temp;
            Temp = StartY;
            StartY = FinishY;
            FinishY = Temp;
            DX = static_cast<long double>(FinishX) - StartX;
            DY = static_cast<long double>(FinishY) - StartY;
        }
        double Slope = pas::real_divide(DY, DX);
        double EndX = MathImports::Floor(StartX + 0.5L);
        double EndY = StartY + (static_cast<long double>(EndX) - StartX) * Slope;
        double Gap = 1.0L - LineFraction16(StartX + 0.5L);
        std::int32_t FirstX = MathImports::Floor(StartX + 0.5L);
        std::int32_t FirstY = MathImports::Floor(EndY);
        double Coverage1 = (1.0L - LineFraction16(EndY)) * Gap;
        double Coverage2 = static_cast<long double>(LineFraction16(EndY)) * Gap;
        if (Steep) {
            PlotLinePixel16(FirstY, FirstX, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            PlotLinePixel16(FirstY + 1, FirstX, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        } else {
            PlotLinePixel16(FirstX, FirstY, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            PlotLinePixel16(FirstX, FirstY + 1, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        }
        std::int32_t X = FirstX + 1;
        double InterY = static_cast<long double>(EndY) + Slope;
        EndX = MathImports::Floor(FinishX + 0.5L);
        EndY = FinishY + (static_cast<long double>(EndX) - FinishX) * Slope;
        Gap = 1.0L - LineFraction16(FinishX - 0.5L);
        std::int32_t LastX = MathImports::Floor(FinishX + 0.5L);
        std::int32_t LastY = MathImports::Floor(EndY);
        while (LastX - 1 >= X) {
            Coverage1 = 1.0L - LineFraction16(InterY);
            Coverage2 = LineFraction16(InterY);
            if (Steep) {
                {
                    std::int32_t ceil = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1);
                    std::int32_t floor = MathImports::Floor(InterY);
                    PlotLinePixel16(floor, X, ceil);
                }
                {
                    std::int32_t ceil_2 = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2);
                    std::int32_t cpp_arg = MathImports::Floor(InterY) + 1;
                    PlotLinePixel16(cpp_arg, X, ceil_2);
                }
            } else {
                {
                    std::int32_t ceil_3 = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1);
                    std::int32_t floor_2 = MathImports::Floor(InterY);
                    PlotLinePixel16(X, floor_2, ceil_3);
                }
                {
                    std::int32_t ceil_4 = MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2);
                    std::int32_t cpp_arg_2 = MathImports::Floor(InterY) + 1;
                    PlotLinePixel16(X, cpp_arg_2, ceil_4);
                }
            }
            InterY = static_cast<long double>(InterY) + Slope;
            ++X;
        }
        Coverage1 = (1.0L - LineFraction16(EndY)) * Gap;
        Coverage2 = static_cast<long double>(LineFraction16(EndY)) * Gap;
        if (Steep) {
            PlotLinePixel16(LastY, LastX, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            PlotLinePixel16(LastY + 1, LastX, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        } else {
            PlotLinePixel16(LastX, LastY, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage1));
            PlotLinePixel16(LastX, LastY + 1, MathImports::Ceil(static_cast<long double>(Alpha) * Coverage2));
        }
    }

    void TGraphBufGR::p_destroy() {
        GR_GraphBuf::TGraphBufGR_Destroy(this);
    }

} // namespace GR_GraphBuf
