#include "layout/EC_CacheGAI.hpp"
#include "types/EC_Buf.hpp"
#include "types/Types.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_gi.hpp"
#include "units/Windows.hpp"

namespace EC_CacheGAI {
    TCGaiEC* AcquireCachedGai(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCGaiEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCGaiEC>()));
    }

    // Native directory indexing adds the two header dwords separately.
    std::uint32_t ReadGaiSequenceOffset(GR_gi::PGaiSequenceTableHeader Table, std::int32_t Index) {
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(Table, Index * static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceDirectoryEntry)) + 4 + 4));
    }

    void TCGaiControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCGaiControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(pas::view(CacheKey), pas::class_ref<TCGaiEC>()) == nullptr) {
            Control = pas::construct_call<TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCGaiControlEC::CreateData() {
        return pas::construct_call<TCGaiEC>(TCGaiEC_Create);
    }

    EC_Cache::TCacheDataEC* TCGaiControlEC::AcquireData() {
        return EC_CacheGAI::AcquireCachedGai(this);
    }

    // CachedFrameOrigins has Header.FrameCount entries.
    void TCGaiEC_Create(TCGaiEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->DecodedFrameGi = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
        Self->FrameSurfaceCache = nullptr;
    }

    void TCGaiEC_Destroy(TCGaiEC* Self) {
        if (Self->RawGaiData != nullptr) {
            EC_Mem::FreeEC(Self->RawGaiData);
            Self->RawGaiData = nullptr;
        }
        pas::free(Self->DecodedFrameGi);
        if (Self->FrameSurfaceCache != nullptr) {
            GR_DX::FreeTextureCache(Self->FrameSurfaceCache);
            Self->FrameSurfaceCache = nullptr;
            Self->CachedFrameOrigins.set_length(0);
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    std::int32_t TCGaiEC::GetFrameCount() {
        return Header->FrameCount;
    }

    std::uint8_t TCGaiEC::HasPlaybackFlags() {
        return Header->Flags != 0;
    }

    WindowsSdk::TRect TCGaiEC::GetBoundsRect() {
        WindowsSdk::TRect Result{};
        Result = Header->Bounds;
        return Result;
    }

    WindowsSdk::TPoint TCGaiEC::GetCanvasSize() {
        WindowsSdk::TPoint Result{};
        Result = EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Header->Bounds, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Header->Bounds, 0)));
        return Result;
    }

    void TCGaiEC::GetOrCreateFrameSurface(std::int32_t FrameIndex, Direct3D9::IDirect3DTexture9& Result) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 cpp_result_3{};
        Direct3D9::IDirect3DTexture9 Texture{};
        Direct3D9::TD3DLockedRect Locked{};
        WindowsSdk::TPoint FrameSize{};
        WindowsSdk::TPoint Origin{};
        GR_gi::TgiGR* Frame{};
        if (FrameSurfaceCache == nullptr) {
            FrameSurfaceCache = GR_DX::CreateTextureCache();
        }
        Texture = (FrameSurfaceCache->GetSurface(FrameIndex, cpp_result), cpp_result);
        if (Texture == nullptr) {
            Frame = LoadFrameGi(FrameIndex);
            if (Frame != nullptr) {
                FrameSize = Frame->GetContentSize();
                if (FrameSize.X == 0 || FrameSize.Y == 0) {
                    Result = nullptr;
                    return;
                }
                if (Frame->Header->Format == 0 && Frame->Header->AlphaMask == 0) {
                    Texture = (GR_DX::GR_CreateTexture(FrameSize.X, FrameSize.Y, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result_2), cpp_result_2);
                } else {
                    Texture = (GR_DX::GR_CreateTexture(FrameSize.X, FrameSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_3), cpp_result_3);
                }
                if (Texture != nullptr) {
                    Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                    if (Locked.Bits != nullptr) {
                        Frame->DecodeToPixels(Locked.Bits, Locked.Pitch, FrameSize.X, FrameSize.Y, true);
                        Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                    }
                }
                Origin = Frame->GetTopLeft();
                CachedFrameOrigins[FrameIndex].X = Origin.X - Header->Bounds.Left;
                CachedFrameOrigins[FrameIndex].Y = Origin.Y - Header->Bounds.Top;
                FrameSurfaceCache->SetSurface(Texture, FrameIndex);
            }
        }
        Result = Texture;
        return;
    }

    // Requires a valid index and a prior GetOrCreateFrameSurface call.
    WindowsSdk::TPoint TCGaiEC::GetFrameOrigin(std::int32_t FrameIndex) {
        WindowsSdk::TPoint Result{};
        Result = CachedFrameOrigins[FrameIndex];
        return Result;
    }

    // Returns borrowed, reused DecodedFrameGi storage, or nil.
    GR_gi::TgiGR* TCGaiEC::LoadFrameGi(std::int32_t FrameIndex) {
        GR_gi::TgiGR* Result = nullptr;
        if (FrameIndex < 0 || Header->FrameCount <= FrameIndex) {
            return Result;
        }
        std::int32_t Offset = EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(RawGaiData, FrameIndex * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader))));
        if (Offset == 0) {
            return nullptr;
        }
        if (EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(RawGaiData, Offset)) == 0x00004c5a) {
            DecodedFrameGi->LoadCompressedGiBytes(EC_Mem::AddPointerOffset(RawGaiData, Offset), EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(RawGaiData, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_gi::PGaiFrameEntry>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(FrameIndex * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)))))->DataSize)))));
            if (!SkipPalettedColorCacheBuild) {
                DecodedFrameGi->BuildPalettedFormat4ColorCache();
            }
        } else {
            DecodedFrameGi->LoadRawGiBytes(EC_Mem::AddPointerOffset(RawGaiData, Offset), EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(RawGaiData, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_gi::PGaiFrameEntry>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(FrameIndex * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)))))->DataSize)))));
        }
        if (!DecodedFrameGi->IsEmpty()) {
            return DecodedFrameGi;
        }
        return Result;
    }

    // Does not validate FrameIndex.
    std::uint8_t TCGaiEC::IsFrameCompressed(std::int32_t FrameIndex) {
        std::int32_t Offset = EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(RawGaiData, FrameIndex * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader))));
        if (Offset == 0) {
            return false;
        }
        return EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(RawGaiData, Offset)) == 0x00004c5a;
    }

    // Returns zero when no sequence table exists. Other sequence accessors require a valid table and indexes.
    std::int32_t TCGaiEC::GetSequenceCount() {
        if (SequenceTableData == nullptr) {
            return 0;
        }
        return EC_Mem::ReadDWordEC(&SequenceTableData->SequenceCount);
    }

    std::int32_t TCGaiEC::GetSequenceFrameCount(std::int32_t SequenceIndex) {
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(SequenceTableData, EC_CacheGAI::ReadGaiSequenceOffset(SequenceTableData, SequenceIndex)));
    }

    void TCGaiEC::FillSequenceFrameIndexTable(std::int32_t SequenceIndex, void* DestTable, std::int32_t EntryStride) {
        std::int32_t Index{};
        void* Source = EC_Mem::AddPointerOffset(SequenceTableData, EC_CacheGAI::ReadGaiSequenceOffset(SequenceTableData, SequenceIndex));
        std::int32_t Count = EC_Mem::ReadDWordEC(Source);
        Source = EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceDataBlock)));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            EC_Mem::WriteIntegerEC(DestTable, EC_Mem::ReadDWordEC(Source));
            DestTable = EC_Mem::AddPointerOffset(DestTable, EntryStride);
            Source = EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceFrameEntry)));
        }
    }

    void TCGaiEC::FillSequenceFrameDelayTable(std::int32_t SequenceIndex, void* DestTable, std::int32_t EntryStride) {
        std::int32_t Index{};
        void* Source = EC_Mem::AddPointerOffset(SequenceTableData, EC_CacheGAI::ReadGaiSequenceOffset(SequenceTableData, SequenceIndex));
        std::int32_t Count = EC_Mem::ReadDWordEC(Source);
        Source = EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceDataBlock)) + static_cast<std::int32_t>(sizeof(std::int32_t)));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            EC_Mem::WriteIntegerEC(DestTable, EC_Mem::ReadDWordEC(Source));
            DestTable = EC_Mem::AddPointerOffset(DestTable, EntryStride);
            Source = EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceFrameEntry)));
        }
    }

    std::int32_t TCGaiEC::GetSequenceFrameIndex(std::int32_t SequenceIndex, std::int32_t FrameInSequence) {
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(SequenceTableData, EC_CacheGAI::ReadGaiSequenceOffset(SequenceTableData, SequenceIndex) + static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_gi::PGaiSequenceFrameEntry>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(FrameInSequence * static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceFrameEntry)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceDataBlock)))))->SourceFrameIndex))));
    }

    std::int32_t TCGaiEC::GetSequenceFrameDelay(std::int32_t SequenceIndex, std::int32_t FrameInSequence) {
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(SequenceTableData, EC_CacheGAI::ReadGaiSequenceOffset(SequenceTableData, SequenceIndex) + static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_gi::PGaiSequenceFrameEntry>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(FrameInSequence * static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceFrameEntry)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiSequenceDataBlock)))))->FrameDelay))));
    }

    // NoConvertPF disables palette conversion. Only the minimum header size is validated; frame and sequence offsets are trusted.
    void TCGaiEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        std::int32_t FrameIndex{};
        GR_gi::TgiGR* Frame{};
        if (LoadOption == u"NoConvertPF") {
            SkipPalettedColorCacheBuild = true;
        }
        TCGaiEC::ApplyAB2BackgroundFixup(SourceBuffer, CacheKey);
        try {
            RawGaiData = EC_Mem::AllocEC(SourceBuffer->DataSize);
            Windows::CopyMemory(RawGaiData, SourceBuffer->Data, SourceBuffer->DataSize);
            ResidentBytes = SourceBuffer->DataSize;
        } catch (...) {
            RawGaiData = nullptr;
            ResidentBytes = 0;
        }
        Header = static_cast<GR_gi::PGaiHeader>(RawGaiData);
        if (SourceBuffer->DataSize < static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)) || RawGaiData == nullptr) {
            GR_Main::AppendLogLineThreadSafe("Error Load Gai"_a);
            Header = static_cast<GR_gi::PGaiHeader>(EC_Mem::AllocClearEC(static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader))));
        }
        if (Header->SequenceTableOffset != 0) {
            SequenceTableData = static_cast<GR_gi::PGaiSequenceTableHeader>(EC_Mem::AddPointerOffset(RawGaiData, Header->SequenceTableOffset));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Header->FrameCount - 1); cpp_range.next(FrameIndex); ) {
            if (!IsFrameCompressed(FrameIndex)) {
                Frame = LoadFrameGi(FrameIndex);
                if (Frame != nullptr && static_cast<std::uint8_t>(SkipPalettedColorCacheBuild ^ 1)) {
                    Frame->BuildPalettedFormat4ColorCache();
                }
            }
        }
        CachedFrameOrigins.set_length(Header->FrameCount);
    }

    // Only affects the single-frame Bm.FormAB2.2bg resource; replaces SourceBuffer with RGB565 GI data.
    void TCGaiEC::ApplyAB2BackgroundFixup(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& ResourceKey) {
        GR_gi::TgiGR* Image{};
        GR_GraphBuf::TGraphBufGR* GraphBuf{};
        std::int32_t ByteCount{};
        std::int32_t Offset{};
        GR_gi::TGaiHeader OldHeader{};
        auto LogGaiRescaleStart = [&]() -> void {
            GR_Main::AppendLogTextThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Rescaling ", ResourceKey, u"... "})));
        };
        auto LogGaiRescaleDone = [&]() -> void {
            GR_Main::AppendLogLineThreadSafe("ok"_a);
        };
        if (EC_Str::FindTextOffsetW(ResourceKey, u"Bm.FormAB2.2bg"_wref.get(), 0) == 0) {
            LogGaiRescaleStart();
            Windows::CopyMemory(&OldHeader, SourceBuffer->Data, static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)));
            if (OldHeader.FrameCount != 1) {
                return;
            }
            Offset = EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(SourceBuffer->Data, static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader))));
            ByteCount = EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(SourceBuffer->Data, static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)) + 4));
            if (Offset == 0 || ByteCount == 0) {
                return;
            }
            Image = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
            if (EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(SourceBuffer->Data, Offset)) == 0x00004c5a) {
                Image->LoadCompressedGiBytes(EC_Mem::AddPointerOffset(SourceBuffer->Data, Offset), ByteCount);
            } else {
                Image->LoadRawGiBytes(EC_Mem::AddPointerOffset(SourceBuffer->Data, Offset), ByteCount);
            }
            if (Image->IsEmpty()) {
                return;
            }
            GraphBuf = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            GraphBuf->AllocateRgbaTight(Image->GetContentSize().X, Image->GetContentSize().Y);
            Image->DecodeToGraphBuf(GraphBuf, false);
            Image->ClearData();
            GraphBuf->RescaleRGBA_HW(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight, true, 1, 1);
            Image->CreateFromGraphBuf(GraphBuf, 1);
            GraphBuf->Clear();
            OldHeader.Bounds.Right = GR_Main::GameScreenWidth;
            OldHeader.Bounds.Bottom = GR_Main::GameScreenHeight;
            SourceBuffer->Clear();
            SourceBuffer->AddBytes(&OldHeader, static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)));
            SourceBuffer->AddDWord(static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)) + static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)));
            SourceBuffer->AddDWord(Image->DataSize);
            SourceBuffer->AddBytes(Image->Data, Image->DataSize);
            Image->ClearData();
            pas::free(Image);
            LogGaiRescaleDone();
        }
    }

    void TCGaiEC::p_destroy() {
        EC_CacheGAI::TCGaiEC_Destroy(this);
    }

} // namespace EC_CacheGAI
