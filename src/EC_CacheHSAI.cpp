#include "layout/EC_CacheHSAI.hpp"
#include "types/EC_Buf.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheHSAI.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Windows.hpp"

namespace EC_CacheHSAI {
    TCHSAIEC* AcquireCachedHSAI(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCHSAIEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCHSAIEC>()));
    }

    void TCHSAIControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCHSAIControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(pas::view(CacheKey), pas::class_ref<TCHSAIEC>()) == nullptr) {
            Control = pas::construct_call<TCHSAIControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCHSAIControlEC::CreateData() {
        return pas::construct_call<TCHSAIEC>(TCHSAIEC_Create);
    }

    EC_Cache::TCacheDataEC* TCHSAIControlEC::AcquireData() {
        return EC_CacheHSAI::AcquireCachedHSAI(this);
    }

    void TCHSAIEC_Create(TCHSAIEC* Self) {
        Self->FrameSurfaceCache = nullptr;
        EC_Cache::TCacheDataEC_Create(Self);
    }

    void TCHSAIEC_Destroy(TCHSAIEC* Self) {
        if (Self->BlobData != nullptr) {
            EC_Mem::FreeEC(Self->BlobData);
            Self->BlobData = nullptr;
        }
        if (Self->FrameSurfaceCache != nullptr) {
            GR_DX::FreeTextureCache(Self->FrameSurfaceCache);
            Self->FrameSurfaceCache = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    std::uint32_t TCHSAIEC::GetFrameCount() {
        return Header->FrameCount;
    }

    // Returns nil when FrameIndex is outside the header count.
    void* TCHSAIEC::GetFrameIndexPlane(std::uint32_t FrameIndex) {
        if (FrameIndex >= Header->FrameCount) {
            return nullptr;
        }
        return EC_Mem::AddPointerOffset(BlobData, static_cast<std::int32_t>(sizeof(THSAIHeaderEC)) + FrameIndex * Header->FrameStride);
    }

    // Returns nil for an invalid frame or absent palette.
    GR_GraphBuf::PColorRGBA TCHSAIEC::GetFramePalette(std::uint32_t FrameIndex) {
        if (FrameIndex >= Header->FrameCount) {
            return nullptr;
        } else if (Header->PaletteBytes == 0) {
            return nullptr;
        } else {
            return static_cast<GR_GraphBuf::PColorRGBA>(EC_Mem::AddPointerOffset(BlobData, static_cast<std::int32_t>(sizeof(THSAIHeaderEC)) + FrameIndex * Header->FrameStride + Header->PitchBytes * Header->Height));
        }
    }

    // Requires a valid frame and palette; uses Width rather than PitchBytes as the source pitch.
    void TCHSAIEC::GetOrCreateFrameSurface(std::uint32_t FrameIndex, Direct3D9::IDirect3DTexture9& Result) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 Texture{};
        Direct3D9::TD3DLockedRect Locked{};
        if (FrameSurfaceCache == nullptr) {
            FrameSurfaceCache = GR_DX::CreateTextureCache();
        }
        Texture = (FrameSurfaceCache->GetSurface(FrameIndex, cpp_result), cpp_result);
        if (Texture == nullptr) {
            Texture = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_2), cpp_result_2);
            if (Texture != nullptr) {
                Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                if (Locked.Bits != nullptr) {
                    GR_Main::ExpandPaletteToBgra(Locked.Bits, Locked.Pitch, Width, Height, GetFrameIndexPlane(FrameIndex), Width, GetFramePalette(FrameIndex));
                    Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                }
            }
            FrameSurfaceCache->SetSurface(Texture, FrameIndex);
        }
        Result = Texture;
        return;
    }

    std::int32_t TCHSAIEC::GetSourcePitchBytes() {
        return Header->PitchBytes;
    }

    // Only the minimum 0x34-byte header size is validated. Ignores LoadOption.
    void TCHSAIEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        if (SourceBuffer->DataSize < static_cast<std::int32_t>(sizeof(THSAIHeaderEC))) {
            pas::raise(pas::make_exception<pas::Exception>("Error Load HSAI"_a));
        }
        BlobData = EC_Mem::AllocEC(SourceBuffer->DataSize);
        Windows::CopyMemory(BlobData, SourceBuffer->Data, SourceBuffer->DataSize);
        ResidentBytes = SourceBuffer->DataSize;
        Header = static_cast<PHSAIHeaderEC>(BlobData);
        Width = Header->Width;
        Height = Header->Height;
    }

    void TCHSAIEC::p_destroy() {
        EC_CacheHSAI::TCHSAIEC_Destroy(this);
    }

} // namespace EC_CacheHSAI
