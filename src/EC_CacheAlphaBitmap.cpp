#include "layout/EC_CacheAlphaBitmap.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Buf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheAlphaBitmap.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"

namespace EC_CacheAlphaBitmap {
    TCAlphaBitmapEC* AcquireOrCreateAlphaBitmap(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCAlphaBitmapEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCAlphaBitmapEC>()));
    }

    void TCAlphaBitmapControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCAlphaBitmapControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCAlphaBitmapEC>()) == nullptr) {
            Control = pas::construct_call<TCAlphaBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCAlphaBitmapControlEC::CreateData() {
        return pas::construct_call<TCAlphaBitmapEC>(TCAlphaBitmapEC_Create);
    }

    EC_Cache::TCacheDataEC* TCAlphaBitmapControlEC::AcquireData() {
        return EC_CacheAlphaBitmap::AcquireOrCreateAlphaBitmap(this);
    }

    void TCAlphaBitmapEC_Create(TCAlphaBitmapEC* Self) {
        Self->SurfaceCache = nullptr;
        EC_Cache::TCacheDataEC_Create(Self);
    }

    void TCAlphaBitmapEC_Destroy(TCAlphaBitmapEC* Self) {
        if (Self->TransBuf16 != nullptr) {
            EC_Mem::FreeEC(Self->TransBuf16);
            Self->TransBuf16 = nullptr;
        }
        if (Self->TransAlphaBuf16 != nullptr) {
            EC_Mem::FreeEC(Self->TransAlphaBuf16);
            Self->TransAlphaBuf16 = nullptr;
        }
        if (Self->AlphaBuf != nullptr) {
            EC_Mem::FreeEC(Self->AlphaBuf);
            Self->AlphaBuf = nullptr;
        }
        if (Self->SurfaceCache != nullptr) {
            GR_DX::FreeTextureCache(Self->SurfaceCache);
            Self->SurfaceCache = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCAlphaBitmapEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        GR_GraphBuf::TGraphBufGR* Bitmap = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Bitmap->LoadImageRgba(SourceBuffer);
        ResidentBytes = 0;
        std::int32_t height = Bitmap->Height;
        void* pixels = Bitmap->GetPixels();
        std::int32_t width = Bitmap->Width;
        std::int32_t pitchBytes = Bitmap->PitchBytes;
        std::uint32_t ByteCount = GR_Main::Ex_OKGR_TransBuf_BuildFromRGBA_16(pixels, pitchBytes, width, height, nullptr);
        if (ByteCount < 1) {
            pas::raise(pas::make_exception<pas::Exception>("TCAlphaBitmapEC.Load. Error load file."_a));
        }
        TransBuf16 = EC_Mem::AllocEC(ByteCount);
        {
            std::int32_t height_2 = Bitmap->Height;
            void* transBuf16 = TransBuf16;
            void* pixels_2 = Bitmap->GetPixels();
            std::int32_t width_2 = Bitmap->Width;
            std::int32_t pitchBytes_2 = Bitmap->PitchBytes;
            GR_Main::Ex_OKGR_TransBuf_BuildFromRGBA_16(pixels_2, pitchBytes_2, width_2, height_2, transBuf16);
        }
        ResidentBytes += ByteCount;
        {
            std::int32_t height_3 = Bitmap->Height;
            void* pixels_3 = Bitmap->GetPixels();
            std::int32_t width_3 = Bitmap->Width;
            std::int32_t pitchBytes_3 = Bitmap->PitchBytes;
            ByteCount = GR_Main::Ex_OKGR_TransAlphaBuf_BuildFromRGBA_16(pixels_3, pitchBytes_3, width_3, height_3, nullptr);
        }
        if (ByteCount < 1) {
            pas::raise(pas::make_exception<pas::Exception>("TCAlphaBitmapEC.Load. Error load file."_a));
        }
        TransAlphaBuf16 = EC_Mem::AllocEC(ByteCount);
        {
            std::int32_t height_4 = Bitmap->Height;
            void* transAlphaBuf16 = TransAlphaBuf16;
            void* pixels_4 = Bitmap->GetPixels();
            std::int32_t width_4 = Bitmap->Width;
            std::int32_t pitchBytes_4 = Bitmap->PitchBytes;
            GR_Main::Ex_OKGR_TransAlphaBuf_BuildFromRGBA_16(pixels_4, pitchBytes_4, width_4, height_4, transAlphaBuf16);
        }
        ResidentBytes += ByteCount;
        {
            std::int32_t height_5 = Bitmap->Height;
            void* pixels_5 = Bitmap->GetPixels();
            std::int32_t width_5 = Bitmap->Width;
            std::int32_t pitchBytes_5 = Bitmap->PitchBytes;
            ByteCount = GR_Main::Ex_OKGR_AlphaBuf_BuildFromRGBA(pixels_5, pitchBytes_5, width_5, height_5, nullptr);
        }
        if (ByteCount < 1) {
            pas::raise(pas::make_exception<pas::Exception>("TCAlphaBitmapEC.Load. Error load file."_a));
        }
        AlphaBuf = EC_Mem::AllocEC(ByteCount);
        {
            std::int32_t height_6 = Bitmap->Height;
            void* alphaBuf = AlphaBuf;
            void* pixels_6 = Bitmap->GetPixels();
            std::int32_t width_6 = Bitmap->Width;
            std::int32_t pitchBytes_6 = Bitmap->PitchBytes;
            GR_Main::Ex_OKGR_AlphaBuf_BuildFromRGBA(pixels_6, pitchBytes_6, width_6, height_6, alphaBuf);
        }
        ResidentBytes += ByteCount;
        PixelSize.X = Bitmap->Width;
        PixelSize.Y = Bitmap->Height;
        pas::free(Bitmap);
    }

    void TCAlphaBitmapEC::Draw16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, WindowsSdk::TRect Clip) {
        WindowsSdk::TRect InclusiveClip{};
        InclusiveClip.Left = Clip.Left;
        InclusiveClip.Top = Clip.Top;
        InclusiveClip.Right = Clip.Right - 1;
        InclusiveClip.Bottom = Clip.Bottom - 1;
        GR_Main::Ex_OKGR_AlphaBuf_DrawClip_16(Dest, Pitch, X, Y, AlphaBuf, InclusiveClip);
        GR_Main::Ex_OKGR_TransAlphaBuf_DrawClip_WORD(Dest, Pitch, X, Y, TransAlphaBuf16, InclusiveClip);
        GR_Main::Ex_OKGR_TransBuf_DrawClip_WORD(Dest, Pitch, X, Y, TransBuf16, InclusiveClip);
    }

    void TCAlphaBitmapEC::DecodeToGraphBuf(GR_GraphBuf::TGraphBufGR* Buffer) {
        Buffer->AllocateRgbaTight(PixelSize.X, PixelSize.Y);
        Buffer->ClearPixels();
        {
            void* pixels = Buffer->GetPixels();
            void* alphaBuf = AlphaBuf;
            std::int32_t pitchBytes = Buffer->PitchBytes;
            GR_Main::Ex_OKGR_AlphaBuf_Draw_RGBA(pixels, pitchBytes, alphaBuf);
        }
        {
            void* pixels_2 = Buffer->GetPixels();
            void* transAlphaBuf16 = TransAlphaBuf16;
            std::int32_t pitchBytes_2 = Buffer->PitchBytes;
            GR_Main::Ex_OKGR_TransAlphaBuf_Draw_RGBA(pixels_2, pitchBytes_2, transAlphaBuf16);
        }
        {
            void* pixels_3 = Buffer->GetPixels();
            void* transBuf16 = TransBuf16;
            std::int32_t pitchBytes_3 = Buffer->PitchBytes;
            GR_Main::Ex_OKGR_TransBuf_Draw_RGBA(pixels_3, pitchBytes_3, transBuf16);
        }
    }

    void TCAlphaBitmapEC::GetTexture(Direct3D9::IDirect3DTexture9& Result) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        GR_GraphBuf::TGraphBufGR* Buffer{};
        Direct3D9::IDirect3DTexture9 Texture{};
        if (SurfaceCache == nullptr) {
            SurfaceCache = GR_DX::CreateTextureCache();
        }
        Texture = (SurfaceCache->GetSurface(0, cpp_result), cpp_result);
        if (Texture == nullptr) {
            Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DecodeToGraphBuf(Buffer);
            Texture = (([&] {
                void* pixels = Buffer->GetPixels();
                std::int32_t pitchBytes = Buffer->PitchBytes;
                std::int32_t height = Buffer->Height;
                std::int32_t width = Buffer->Width;
                return GR_DX::CreateTextureFromPixels(width, height, 21u, pixels, pitchBytes, 1u, cpp_result_2);
            }()), cpp_result_2);
            pas::free(Buffer);
            SurfaceCache->SetSurface(Texture, 0);
        }
        Result = Texture;
        return;
    }

    void TCAlphaBitmapEC::p_destroy() {
        EC_CacheAlphaBitmap::TCAlphaBitmapEC_Destroy(this);
    }

} // namespace EC_CacheAlphaBitmap
