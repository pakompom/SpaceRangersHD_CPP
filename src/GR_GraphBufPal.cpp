#include "layout/GR_GraphBufPal.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_OKGF.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SysUtilsImports.hpp"
#include "types/System.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_GraphBufPal.hpp"
#include "units/GR_Main.hpp"
#include "units/Windows.hpp"

namespace GR_GraphBufPal {
    void TGraphBufPalGR_Create(TGraphBufPalGR* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TGraphBufPalGR_Destroy(TGraphBufPalGR* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TGraphBufPalGR::Clear() {
        if (Pixels != nullptr) {
            EC_Mem::FreeEC(Pixels);
            Pixels = nullptr;
        }
        Width = 0;
        Height = 0;
        PitchBytes = 0;
        if (Palette != nullptr) {
            EC_Mem::FreeEC(Palette);
            Palette = nullptr;
        }
        PaletteCount = 0;
    }

    // Discards existing pixels and palette; uses one byte per pixel with no row padding.
    void TGraphBufPalGR::AllocateTight(std::int32_t AWidth, std::int32_t AHeight, std::int32_t APaletteCount) {
        Clear();
        Width = AWidth;
        Height = AHeight;
        PitchBytes = AWidth;
        PaletteCount = APaletteCount;
        Pixels = EC_Mem::AllocEC(PitchBytes * Height);
        Palette = static_cast<GR_GraphBuf::PColorRGBA>(EC_Mem::AllocEC(APaletteCount * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA))));
    }

    // Discards existing pixels and palette; rounds pitch up to a multiple of four.
    void TGraphBufPalGR::AllocateBuffer(std::int32_t AWidth, std::int32_t AHeight, std::int32_t APaletteCount, std::int32_t APitchBytes) {
        Clear();
        Width = AWidth;
        Height = AHeight;
        PitchBytes = APitchBytes;
        if ((PitchBytes & 3) != 0) {
            PitchBytes = PitchBytes + 4 - (PitchBytes & 3);
        }
        PaletteCount = APaletteCount;
        Pixels = EC_Mem::AllocEC(PitchBytes * Height);
        Palette = static_cast<GR_GraphBuf::PColorRGBA>(EC_Mem::AllocEC(APaletteCount * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA))));
        if ((PitchBytes & 3) != 0 || (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Pixels)) & 3) != 0) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufPalGR.CreateN"_a));
        }
    }

    // Index is unchecked.
    std::uint32_t TGraphBufPalGR::GetPaletteColor(std::int32_t Index) {
        return pas::load_unaligned<std::uint32_t>(static_cast<System::PCardinal>(EC_Mem::AddPointerOffset(Palette, Index * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)))));
    }

    void TGraphBufPalGR::SetPalette(GR_GraphBuf::PColorRGBA Source, std::int32_t Count) {
        if (PaletteCount != Count) {
            PaletteCount = Count;
            Palette = static_cast<GR_GraphBuf::PColorRGBA>(EC_Mem::ReAllocREC(Palette, PaletteCount * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA))));
        }
        Windows::CopyMemory(Palette, Source, Count * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
    }

    // Coordinates are unchecked.
    std::uint8_t TGraphBufPalGR::GetPixelIndex(std::int32_t X, std::int32_t Y) {
        SysUtilsImports::PByteArray Data = static_cast<SysUtilsImports::PByteArray>(Pixels);
        return *static_cast<std::uint8_t*>(pas::byte_offset(Data, (Y * PitchBytes + X) * sizeof(std::uint8_t)));
    }

    // Decodes the entire payload, ignoring Position; retains the codec's one- or two-byte indexed pixel width. Failures raise.
    void TGraphBufPalGR::LoadImage(EC_Buf::TBufEC* Buffer) {
        Clear();
        EC_OKGF::POkgfReadContext Context = GR_Main::BeginIndexedImageRead(Buffer->Data, Buffer->DataSize, Width, Height, PaletteCount, BytesPerPixel);
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufPalGR.LoadFromFile. Error load file"_a));
        }
        AllocateBuffer(Width, Height, PaletteCount, Width * BytesPerPixel);
        Context = static_cast<EC_OKGF::POkgfReadContext>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GR_Main::ReadIndexedImagePixels(Context, Pixels, PitchBytes, Palette)))));
        if (Context == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TGraphBufPalGR.LoadFromFile. Error load file"_a));
        }
    }

    void TGraphBufPalGR::ClearPixels() {
        pas::fill_memory(Pixels, PitchBytes * Height, static_cast<std::uint8_t>(0));
    }

    // Includes row padding.
    void TGraphBufPalGR::FillPixels(std::uint8_t Value) {
        Windows::FillMemory(Pixels, PitchBytes * Height, Value);
    }

    void TGraphBufPalGR::p_destroy() {
        GR_GraphBufPal::TGraphBufPalGR_Destroy(this);
    }

} // namespace GR_GraphBufPal
