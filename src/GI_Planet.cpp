#include "layout/GI_Planet.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/BitmapPorts.hpp"
#include "units/ClassesImports.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheLightPal.hpp"
#include "units/EC_CachePalBitmap.hpp"
#include "units/EC_CachePlanetTempl.hpp"
#include "units/EC_CacheRotateBuf.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Planet.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_GraphBufPal.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"

namespace GI_Planet {
    void TPlanetGI_Create(TPlanetGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->TemplateCache = pas::construct_call<EC_CachePlanetTempl::TCPlanetTemplControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->TemplateCache);
        Self->SurfaceImageCache = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->SurfaceImageCache);
        Self->SurfacePaletteCache = pas::construct_call<EC_CacheLightPal::TCLightPalControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->SurfacePaletteCache);
        Self->Cloud1ImageCache = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->Cloud1ImageCache);
        Self->Cloud1PaletteCache = pas::construct_call<EC_CacheLightPal::TCLightPalControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->Cloud1PaletteCache);
        Self->Cloud2ImageCache = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->Cloud2ImageCache);
        Self->Cloud2PaletteCache = pas::construct_call<EC_CacheLightPal::TCLightPalControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->Cloud2PaletteCache);
        Self->Cloud3ImageCache = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->Cloud3ImageCache);
        Self->Cloud3PaletteCache = pas::construct_call<EC_CacheLightPal::TCLightPalControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->Cloud3PaletteCache);
        Self->LightRotationCache = pas::construct_call<EC_CacheRotateBuf::TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->LightRotationCache);
        Self->AtmosphereRotationCache = pas::construct_call<EC_CacheRotateBuf::TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->AtmosphereRotationCache);
        Self->AtmosphereBuffer = pas::construct_call<GR_GraphBufPal::TGraphBufPalGR>(GR_GraphBufPal::TGraphBufPalGR_Create);
        Self->AtmosphereImageCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->AtmosphereImageCache);
        Self->AtmosphereMaskCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->AtmosphereMaskCache);
        Self->RenderedMapOffsets[0] = 0x00ffffff;
        Self->RenderedMapOffsets[1] = Self->RenderedMapOffsets[0];
        Self->RenderedMapOffsets[2] = Self->RenderedMapOffsets[0];
        Self->RenderedMapOffsets[3] = Self->RenderedMapOffsets[0];
        Self->TextureCache = nullptr;
    }

    void TPlanetGI_Destroy(TPlanetGI* Self) {
        pas::free(Self->TemplateCache);
        Self->TemplateCache = nullptr;
        pas::free(Self->SurfacePaletteCache);
        Self->SurfacePaletteCache = nullptr;
        pas::free(Self->SurfaceImageCache);
        Self->SurfaceImageCache = nullptr;
        pas::free(Self->Cloud1PaletteCache);
        Self->Cloud1PaletteCache = nullptr;
        pas::free(Self->Cloud1ImageCache);
        Self->Cloud1ImageCache = nullptr;
        pas::free(Self->Cloud2PaletteCache);
        Self->Cloud2PaletteCache = nullptr;
        pas::free(Self->Cloud2ImageCache);
        Self->Cloud2ImageCache = nullptr;
        pas::free(Self->Cloud3PaletteCache);
        Self->Cloud3PaletteCache = nullptr;
        pas::free(Self->Cloud3ImageCache);
        Self->Cloud3ImageCache = nullptr;
        pas::free(Self->LightRotationCache);
        Self->LightRotationCache = nullptr;
        pas::free(Self->AtmosphereRotationCache);
        Self->AtmosphereRotationCache = nullptr;
        pas::free(Self->AtmosphereBuffer);
        Self->AtmosphereBuffer = nullptr;
        pas::free(Self->AtmosphereImageCache);
        Self->AtmosphereImageCache = nullptr;
        pas::free(Self->AtmosphereMaskCache);
        Self->AtmosphereMaskCache = nullptr;
        if (Self->TextureCache != nullptr) {
            GR_DX::FreeTextureCache(Self->TextureCache);
            Self->TextureCache = nullptr;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves image caches, atmosphere storage and texture cache.
    void TPlanetGI::Clear() {
        if (SourceLightBuffer != nullptr) {
            GR_Main::Ex_OKGR_LightBuf_Destroy(SourceLightBuffer);
            SourceLightBuffer = nullptr;
        }
        if (RotatedLightBuffer != nullptr) {
            GR_Main::Ex_OKGR_LightBuf_Destroy(RotatedLightBuffer);
            RotatedLightBuffer = nullptr;
        }
        LightAngle = 0;
        GI_MessageLoop::TObjectGI::Clear();
    }

    // Image width must be a power of two from 16 through 2048; height must not exceed half the width. The light map must cover that height on both axes.
    void TPlanetGI::SetImage(const pas::WideString& MaskPath, const pas::WideString& ImagePath, const pas::WideString& LightMapPath) {
        EC_CachePalBitmap::TCPalBitmapEC* Image{};
        EC_CachePalBitmap::TCPalBitmapControlEC* LightControl{};
        EC_CachePalBitmap::TCPalBitmapEC* LightImage{};
        Invalidate();
        if (SourceLightBuffer != nullptr) {
            GR_Main::Ex_OKGR_LightBuf_Destroy(SourceLightBuffer);
            SourceLightBuffer = nullptr;
        }
        if (RotatedLightBuffer != nullptr) {
            GR_Main::Ex_OKGR_LightBuf_Destroy(RotatedLightBuffer);
            RotatedLightBuffer = nullptr;
        }
        SurfaceImageCache->SetCacheKey(ImagePath);
        SurfacePaletteCache->SetCacheKey(ImagePath);
        Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(SurfaceImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                SurfaceImageCache->Release();
            };
            MapWidth = Image->Bitmap->Width;
            MapHeight = Image->Bitmap->Height;
            if (static_cast<std::uint32_t>(Image->Bitmap->Width) >> 1 < static_cast<std::uint32_t>(Image->Bitmap->Height)) {
                pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create template planet. (LenX div 2)<LenY"_a));
            }
            if (Image->Bitmap->Width != 16 && Image->Bitmap->Width != 32 && Image->Bitmap->Width != 64 && Image->Bitmap->Width != 128 && Image->Bitmap->Width != 256 && Image->Bitmap->Width != 512 && Image->Bitmap->Width != 1024 && Image->Bitmap->Width != 2048) {
                pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create template planet. LenX<>16 or 32 or 64 or 128 or 256 or 512 or 1024 or 2048"_a));
            }
            MapWidthMask = Image->Bitmap->Width - 1;
            SetSize(ClassesImports::Point(Image->Bitmap->Height + 1, Image->Bitmap->Height + 1));
            TemplateCache->SetCacheKey(pas::concat_wide({MaskPath, u"?", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Image->Bitmap->Width))), u",", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Image->Bitmap->Height)))}));
            LightControl = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(LightControl);
            LightControl->SetCacheKey(LightMapPath);
            LightImage = EC_CachePalBitmap::AcquireOrCreatePalBitmap(LightControl);
            {
                try {
                    if (static_cast<std::uint32_t>(Image->Bitmap->Height) > static_cast<std::uint32_t>(LightImage->Bitmap->Width) || static_cast<std::uint32_t>(Image->Bitmap->Height) > static_cast<std::uint32_t>(LightImage->Bitmap->Height)) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error: Size light map < planet."_a));
                    }
                    SourceLightBuffer = GR_Main::Ex_OKGR_LightBuf_Create(LightImage->Bitmap->Width, LightImage->Bitmap->Height);
                    GR_Main::Ex_OKGR_LightBuf_SetSme(SourceLightBuffer, pas::shr(LightImage->Bitmap->Width, 1), pas::shr(LightImage->Bitmap->Height, 1));
                    if (SourceLightBuffer == nullptr) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create light buffer."_a));
                    }
                    RotatedLightBuffer = GR_Main::Ex_OKGR_LightBuf_Create(LightImage->Bitmap->Width, LightImage->Bitmap->Height);
                    GR_Main::Ex_OKGR_LightBuf_SetSme(RotatedLightBuffer, pas::shr(LightImage->Bitmap->Width, 1), pas::shr(LightImage->Bitmap->Height, 1));
                    if (RotatedLightBuffer == nullptr) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create light buffer."_a));
                    }
                    GR_Main::Ex_OKGR_LightBuf_Init(SourceLightBuffer, 0);
                    GR_Main::Ex_OKGR_LightBuf_Init(RotatedLightBuffer, 0);
                    try {
                        LightRotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(LightImage->Bitmap->Width, 1))), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(LightImage->Bitmap->Height, 1)))})));
                    } catch (...) {
                        pas::raise(pas::make_exception<pas::Exception>("Error in TPlanetGI.SetImage"_a));
                    }
                    GR_Main::Ex_OKGR_LightBuf_LoadFromPalBuf(SourceLightBuffer, LightImage->Bitmap->Pixels, LightImage->Bitmap->Width, LightImage->Bitmap->Height, LightImage->Bitmap->PitchBytes, LightImage->Bitmap->Palette);
                } catch (...) {
                    LightControl->Release();
                    pas::free(LightControl);
                    throw;
                }
                LightControl->Release();
                pas::free(LightControl);
            }
        }
        LightAngle = 1;
        SetLightAngle(0);
    }

    // Requires the same dimensions as the surface map.
    void TPlanetGI::SetCloud1Image(const pas::WideString& Path) {
        Cloud1ImageCache->SetCacheKey(Path);
        Cloud1PaletteCache->SetCacheKey(Path);
        EC_CachePalBitmap::TCPalBitmapEC* Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(Cloud1ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Cloud1ImageCache->Release();
            };
            if (MapWidth != Image->Bitmap->Width || MapHeight != Image->Bitmap->Height) {
                pas::raise(pas::make_exception<pas::Exception>("Cloud size incorrect"_a));
            }
        }
    }

    // Requires the same dimensions as the surface map.
    void TPlanetGI::SetCloud2Image(const pas::WideString& Path) {
        Cloud2ImageCache->SetCacheKey(Path);
        Cloud2PaletteCache->SetCacheKey(Path);
        EC_CachePalBitmap::TCPalBitmapEC* Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(Cloud2ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Cloud2ImageCache->Release();
            };
            if (MapWidth != Image->Bitmap->Width || MapHeight != Image->Bitmap->Height) {
                pas::raise(pas::make_exception<pas::Exception>("Cloud size incorrect"_a));
            }
        }
    }

    // Requires the same dimensions as the surface map.
    void TPlanetGI::SetCloud3Image(const pas::WideString& Path) {
        Cloud3ImageCache->SetCacheKey(Path);
        Cloud3PaletteCache->SetCacheKey(Path);
        EC_CachePalBitmap::TCPalBitmapEC* Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(Cloud3ImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Cloud3ImageCache->Release();
            };
            if (MapWidth != Image->Bitmap->Width || MapHeight != Image->Bitmap->Height) {
                pas::raise(pas::make_exception<pas::Exception>("Cloud size incorrect"_a));
            }
        }
    }

    // Uses a diameter of 2*Radius+1. Applies the surface-map dimension checks but omits the light-map size check.
    void TPlanetGI::SetImageWithRadius(const pas::WideString& MaskPath, const pas::WideString& ImagePath, const pas::WideString& LightMapPath, std::int32_t Radius) {
        EC_CachePalBitmap::TCPalBitmapEC* Image{};
        EC_CachePalBitmap::TCPalBitmapControlEC* LightControl{};
        EC_CachePalBitmap::TCPalBitmapEC* LightImage{};
        Invalidate();
        if (SourceLightBuffer != nullptr) {
            GR_Main::Ex_OKGR_LightBuf_Destroy(SourceLightBuffer);
            SourceLightBuffer = nullptr;
        }
        if (RotatedLightBuffer != nullptr) {
            GR_Main::Ex_OKGR_LightBuf_Destroy(RotatedLightBuffer);
            RotatedLightBuffer = nullptr;
        }
        SurfaceImageCache->SetCacheKey(ImagePath);
        SurfacePaletteCache->SetCacheKey(ImagePath);
        Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(SurfaceImageCache);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                SurfaceImageCache->Release();
            };
            MapWidth = Image->Bitmap->Width;
            MapHeight = Image->Bitmap->Height;
            if (static_cast<std::uint32_t>(Image->Bitmap->Width) >> 1 < static_cast<std::uint32_t>(Image->Bitmap->Height)) {
                pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create template planet. (LenX div 2)<LenY"_a));
            }
            if (Image->Bitmap->Width != 16 && Image->Bitmap->Width != 32 && Image->Bitmap->Width != 64 && Image->Bitmap->Width != 128 && Image->Bitmap->Width != 256 && Image->Bitmap->Width != 512 && Image->Bitmap->Width != 1024 && Image->Bitmap->Width != 2048) {
                pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create template planet. LenX<>16 or 32 or 64 or 128 or 256 or 512 or 1024 or 2048"_a));
            }
            MapWidthMask = Image->Bitmap->Width - 1;
            SetSize(ClassesImports::Point(Radius * 2 + 1, Radius * 2 + 1));
            TemplateCache->SetCacheKey(pas::concat_wide({MaskPath, u"?", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Image->Bitmap->Width))), u",", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Image->Bitmap->Height)))}));
            LightControl = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(LightControl);
            LightControl->SetCacheKey(LightMapPath);
            LightImage = EC_CachePalBitmap::AcquireOrCreatePalBitmap(LightControl);
            {
                try {
                    SourceLightBuffer = GR_Main::Ex_OKGR_LightBuf_Create(LightImage->Bitmap->Width, LightImage->Bitmap->Height);
                    GR_Main::Ex_OKGR_LightBuf_SetSme(SourceLightBuffer, pas::shr(LightImage->Bitmap->Width, 1), pas::shr(LightImage->Bitmap->Height, 1));
                    if (SourceLightBuffer == nullptr) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create light buffer."_a));
                    }
                    RotatedLightBuffer = GR_Main::Ex_OKGR_LightBuf_Create(LightImage->Bitmap->Width, LightImage->Bitmap->Height);
                    GR_Main::Ex_OKGR_LightBuf_SetSme(RotatedLightBuffer, pas::shr(LightImage->Bitmap->Width, 1), pas::shr(LightImage->Bitmap->Height, 1));
                    if (RotatedLightBuffer == nullptr) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create light buffer."_a));
                    }
                    GR_Main::Ex_OKGR_LightBuf_Init(SourceLightBuffer, 0);
                    GR_Main::Ex_OKGR_LightBuf_Init(RotatedLightBuffer, 0);
                    try {
                        LightRotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(LightImage->Bitmap->Width, 1))), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(LightImage->Bitmap->Height, 1)))})));
                    } catch (...) {
                        pas::raise(pas::make_exception<pas::Exception>("Error in TPlanetGI.SetImageEx"_a));
                    }
                    GR_Main::Ex_OKGR_LightBuf_LoadFromPalBuf(SourceLightBuffer, LightImage->Bitmap->Pixels, LightImage->Bitmap->Width, LightImage->Bitmap->Height, LightImage->Bitmap->PitchBytes, LightImage->Bitmap->Palette);
                } catch (...) {
                    LightControl->Release();
                    pas::free(LightControl);
                    throw;
                }
                LightControl->Release();
                pas::free(LightControl);
            }
        }
        LightAngle = 1;
        SetLightAngle(0);
    }

    // Uses a diameter of 2*Radius. Reuses an existing surface image and light buffers; ImagePath is used only when the surface cache key is empty.
    void TPlanetGI::SetImageFromTemplate(const pas::WideString& TemplateKey, const pas::WideString& ImagePath, std::int32_t Radius) {
        EC_CachePalBitmap::TCPalBitmapControlEC* LightControl{};
        EC_CachePalBitmap::TCPalBitmapEC* LightImage{};
        Invalidate();
        if (SurfaceImageCache->HasEmptyCacheKey()) {
            SurfaceImageCache->SetCacheKey(ImagePath);
            SurfacePaletteCache->SetCacheKey(ImagePath);
        }
        TemplateCache->SetCacheKey(TemplateKey);
        SetSize(ClassesImports::Point(Radius * 2, Radius * 2));
        if (SourceLightBuffer == nullptr) {
            MapWidthMask = GlobalsV::SatelliteTemplateParameter1 - 1;
            LightControl = pas::construct_call<EC_CachePalBitmap::TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(LightControl);
            LightControl->SetCacheKey(GlobalsV::SatelliteLightMapPath);
            LightImage = EC_CachePalBitmap::AcquireOrCreatePalBitmap(LightControl);
            {
                try {
                    SourceLightBuffer = GR_Main::Ex_OKGR_LightBuf_Create(LightImage->Bitmap->Width, LightImage->Bitmap->Height);
                    GR_Main::Ex_OKGR_LightBuf_SetSme(SourceLightBuffer, pas::shr(LightImage->Bitmap->Width, 1), pas::shr(LightImage->Bitmap->Height, 1));
                    if (SourceLightBuffer == nullptr) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create light buffer."_a));
                    }
                    RotatedLightBuffer = GR_Main::Ex_OKGR_LightBuf_Create(LightImage->Bitmap->Width, LightImage->Bitmap->Height);
                    GR_Main::Ex_OKGR_LightBuf_SetSme(RotatedLightBuffer, pas::shr(LightImage->Bitmap->Width, 1), pas::shr(LightImage->Bitmap->Height, 1));
                    if (RotatedLightBuffer == nullptr) {
                        pas::raise(pas::make_exception<pas::Exception>("TPlanetGI.SetImage. Error create light buffer."_a));
                    }
                    GR_Main::Ex_OKGR_LightBuf_Init(SourceLightBuffer, 0);
                    GR_Main::Ex_OKGR_LightBuf_Init(RotatedLightBuffer, 0);
                    try {
                        LightRotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(LightImage->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(LightImage->Bitmap->Width, 1))), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(LightImage->Bitmap->Height, 1)))})));
                    } catch (...) {
                        pas::raise(pas::make_exception<pas::Exception>("Error in TPlanetGI.SetImageSputnik"_a));
                    }
                    GR_Main::Ex_OKGR_LightBuf_LoadFromPalBuf(SourceLightBuffer, LightImage->Bitmap->Pixels, LightImage->Bitmap->Width, LightImage->Bitmap->Height, LightImage->Bitmap->PitchBytes, LightImage->Bitmap->Palette);
                } catch (...) {
                    LightControl->Release();
                    pas::free(LightControl);
                    throw;
                }
                LightControl->Release();
                pas::free(LightControl);
            }
            LightAngle = 1;
            SetLightAngle(0);
        }
    }

    // Appends ?Gray to both paths; Color is 0x00BBGGRR. Zero suppresses atmosphere drawing.
    void TPlanetGI::SetAtmosphere(pas::WideString ImagePath, pas::WideString MaskPath, std::uint32_t Color) {
        EC_CacheBitmap::TCBitmapEC* Mask{};
        AtmosphereColor = Color;
        AtmosphereImageCache->SetCacheKey(pas::concat_wide({ImagePath, u"?Gray"}));
        AtmosphereMaskCache->SetCacheKey(pas::concat_wide({MaskPath, u"?Gray"}));
        EC_CacheBitmap::TCBitmapEC* Image = nullptr;
        Mask = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Image = EC_CacheBitmap::AcquireOrCreateBitmap(AtmosphereImageCache);
                Mask = EC_CacheBitmap::AcquireOrCreateBitmap(AtmosphereMaskCache);
                AtmosphereRotationCache->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(static_cast<std::uint32_t>(Mask->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Mask->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Mask->Bitmap->Width)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Mask->Bitmap->Height)), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(Mask->Bitmap->Width, 1))), ",", SysUtils::Int64ToStr(static_cast<std::uint32_t>(pas::shr(Mask->Bitmap->Height, 1)))})));
                SetSize(ClassesImports::Point(Image->Bitmap->Width, Image->Bitmap->Height));
                AtmosphereDirty = true;
                AtmospherePaletteDirty = true;
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Image != nullptr) {
                AtmosphereImageCache->Release();
            }
            if (Mask != nullptr) {
                AtmosphereMaskCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    // Requires space for 256 colors. RGB channels and the alpha ramp are truncated to multiples of eight.
    void TPlanetGI::BuildAtmospherePalette() {
        std::int32_t Index{};
        GR_GraphBuf::PColorRGBA Color = AtmosphereBuffer->Palette;
        for (Index = 0; Index <= 255; ++Index) {
            Color->R = AtmosphereColor & 0x000000ff & 0x000000f8;
            Color->G = AtmosphereColor >> 8 & 0x000000ff & 0x000000f8;
            Color->B = AtmosphereColor >> 16 & 0x000000ff & 0x000000f8;
            Color->A = Index & 0x000000f8;
            Color = reinterpret_cast<GR_GraphBuf::PColorRGBA>(reinterpret_cast<std::uint8_t*>(Color) + static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
        }
    }

    void TPlanetGI_RebuildAtmosphereImage(TPlanetGI* Self) {
        void* ImagePixels{};
        void* MaskPixels{};
        void* DestPixels{};
        std::int32_t Width{};
        void* MulTable{};
        std::int32_t ImageSkip{};
        std::int32_t MaskSkip{};
        std::int32_t DestSkip{};
        std::int32_t Height{};
        EC_CacheBitmap::TCBitmapEC* Image = nullptr;
        EC_CacheBitmap::TCBitmapEC* Mask = nullptr;
        EC_CacheRotateBuf::TCRotateBufEC* Rotation = nullptr;
        GR_GraphBuf::TGraphBufGR* RotatedMask = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Image = EC_CacheBitmap::AcquireOrCreateBitmap(Self->AtmosphereImageCache);
                Mask = EC_CacheBitmap::AcquireOrCreateBitmap(Self->AtmosphereMaskCache);
                Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(Self->AtmosphereRotationCache);
                RotatedMask = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                if (Self->AtmosphereBuffer->Width != Image->Bitmap->Width || Self->AtmosphereBuffer->Height != Image->Bitmap->Height) {
                    Self->AtmosphereBuffer->AllocateTight(Image->Bitmap->Width, Image->Bitmap->Height, 256);
                    Self->AtmospherePaletteDirty = true;
                }
                if (Self->AtmospherePaletteDirty) {
                    Self->AtmospherePaletteDirty = false;
                    Self->BuildAtmospherePalette();
                }
                RotatedMask->AllocateGrayscale(Mask->Bitmap->Width + pas::shr(Mask->Bitmap->Width, 1), Mask->Bitmap->Height + pas::shr(Mask->Bitmap->Height, 1));
                RotatedMask->ClearPixels();
                {
                    std::int32_t pitchBytes = Mask->Bitmap->PitchBytes;
                    std::int32_t cpp_arg = pas::shr(RotatedMask->Width, 1);
                    std::int32_t cpp_arg_2 = pas::shr(RotatedMask->Height, 1);
                    std::uint8_t lightAngle = Self->LightAngle;
                    void* buffer = Rotation->Buffer;
                    void* pixels = Mask->Bitmap->GetPixels();
                    void* pixels_2 = RotatedMask->GetPixels();
                    std::int32_t pitchBytes_2 = RotatedMask->PitchBytes;
                    GR_Main::Ex_OKGR_RotateBuf_Draw_BYTE(pixels_2, pitchBytes_2, pixels, pitchBytes, cpp_arg, cpp_arg_2, lightAngle, buffer);
                }
                Width = Image->Bitmap->Width;
                Height = Image->Bitmap->Height;
                ImagePixels = Image->Bitmap->GetPixels();
                {
                    std::uint8_t* cpp_left_2 = static_cast<std::uint8_t*>(RotatedMask->GetPixels());
                    std::uint8_t* cpp_left = cpp_left_2 + (pas::shr(RotatedMask->Width, 1) - pas::shr(Width, 1));
                    MaskPixels = cpp_left + (pas::shr(RotatedMask->Height, 1) - pas::shr(Height, 1)) * RotatedMask->PitchBytes;
                }
                DestPixels = Self->AtmosphereBuffer->Pixels;
                ImageSkip = Image->Bitmap->PitchBytes - Width;
                MaskSkip = RotatedMask->PitchBytes - Width;
                DestSkip = Self->AtmosphereBuffer->PitchBytes - Width;
                MulTable = GR_Main::Ex_OKGF_MulTable256x256();
                // This register-based pixel loop is handwritten assembly in the native routine.
                BitmapPorts::MaskPixels8(ImagePixels, MaskPixels, DestPixels, Width, Height, ImageSkip, MaskSkip, DestSkip, MulTable);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Image != nullptr) {
                Self->AtmosphereImageCache->Release();
            }
            if (Mask != nullptr) {
                Self->AtmosphereMaskCache->Release();
            }
            if (Rotation != nullptr) {
                Self->AtmosphereRotationCache->Release();
            }
            if (RotatedMask != nullptr) {
                pas::free(RotatedMask);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TPlanetGI::SetSurfaceMapOffset(std::int32_t Value) {
        if (SurfaceMapOffset != Value) {
            SurfaceMapOffset = Value;
            Invalidate();
        }
    }

    void TPlanetGI::SetCloud1MapOffset(std::int32_t Value) {
        if (Cloud1MapOffset != Value) {
            Cloud1MapOffset = Value;
            Invalidate();
        }
    }

    void TPlanetGI::SetCloud2MapOffset(std::int32_t Value) {
        if (Cloud2MapOffset != Value) {
            Cloud2MapOffset = Value;
            Invalidate();
        }
    }

    void TPlanetGI::SetCloud3MapOffset(std::int32_t Value) {
        if (Cloud3MapOffset != Value) {
            Cloud3MapOffset = Value;
            Invalidate();
        }
    }

    // A full turn has 256 steps; requires initialized light buffers when the angle changes.
    void TPlanetGI::SetLightAngle(std::uint8_t Value) {
        EC_CacheRotateBuf::TCRotateBufEC* Rotation{};
        if (LightAngle != Value) {
            LightAngle = Value;
            GR_Main::Ex_OKGR_LightBuf_Init(RotatedLightBuffer, 0);
            Rotation = EC_CacheRotateBuf::AcquireOrCreateRotateBuf(LightRotationCache);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    LightRotationCache->Release();
                };
                GR_Main::Ex_OKGR_LightBuf_Rotate(RotatedLightBuffer, SourceLightBuffer, Rotation->Buffer, LightAngle);
            }
            AtmosphereDirty = true;
            Invalidate();
        }
    }

    void TPlanetGI_LoadFromConfigPath(TPlanetGI* Self, const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Mask"_wref.get()) > 0 || Block->CountParams(u"Image"_wref.get()) > 0 || Block->CountParams(u"ImageLight"_wref.get()) > 0) {
            const pas::WideString& param = Block->GetParam(u"ImageLight"_wref.get());
            const pas::WideString& param_2 = Block->GetParam(u"Image"_wref.get());
            const pas::WideString& param_3 = Block->GetParam(u"Mask"_wref.get());
            Self->SetImage(param_3, param_2, param);
        }
        if (Block->CountParams(u"SmeMap"_wref.get()) > 0) {
            Self->SurfaceMapOffset = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"SmeMap"_wref.get())));
        }
        if (Block->CountParams(u"AngleLight"_wref.get()) > 0) {
            Self->LightAngle = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AngleLight"_wref.get())));
        }
    }

    void TPlanetGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        {
            const pas::WideString& param = Block->GetParam(u"ImageLight"_wref.get());
            const pas::WideString& param_2 = Block->GetParam(u"Image"_wref.get());
            const pas::WideString& param_3 = Block->GetParam(u"Mask"_wref.get());
            SetImage(param_3, param_2, param);
        }
        if (Block->CountParams(u"SmeMap"_wref.get()) > 0) {
            SurfaceMapOffset = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"SmeMap"_wref.get())));
        }
        if (Block->CountParams(u"AngleLight"_wref.get()) > 0) {
            LightAngle = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AngleLight"_wref.get())));
        }
    }

    // The native routine does not release its third cloud layer's cache acquisitions.
    void TPlanetGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 cpp_result_3{};
        Direct3D9::IDirect3DTexture9 cpp_result_4{};
        Direct3D9::IDirect3DTexture9 cpp_result_5{};
        Direct3D9::IDirect3DTexture9 cpp_result_6{};
        Direct3D9::IDirect3DTexture9 cpp_result_7{};
        Direct3D9::IDirect3DTexture9 cpp_result_8{};
        Direct3D9::IDirect3DTexture9 cpp_result_9{};
        Direct3D9::IDirect3DTexture9 cpp_result_10{};
        Direct3D9::TD3DLockedRect Locked{};
        std::int32_t Index{};
        Direct3D9::IDirect3DTexture9 Texture{};
        std::uint8_t HasCloud1 = Cloud1ImageCache->CacheKey != u"";
        std::uint8_t HasCloud2 = Cloud2ImageCache->CacheKey != u"";
        std::uint8_t HasCloud3 = Cloud3ImageCache->CacheKey != u"";
        EC_CachePalBitmap::TCPalBitmapEC* Image = nullptr;
        EC_CacheLightPal::TCLightPalEC* Palette = nullptr;
        EC_CachePlanetTempl::TCPlanetTemplEC* Template = nullptr;
        EC_CachePalBitmap::TCPalBitmapEC* Cloud1 = nullptr;
        EC_CacheLightPal::TCLightPalEC* CloudPalette1 = nullptr;
        EC_CachePalBitmap::TCPalBitmapEC* Cloud2 = nullptr;
        EC_CacheLightPal::TCLightPalEC* CloudPalette2 = nullptr;
        EC_CachePalBitmap::TCPalBitmapEC* Cloud3 = nullptr;
        EC_CacheLightPal::TCLightPalEC* CloudPalette3 = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Template = EC_CachePlanetTempl::AcquireOrCreatePlanetTemplate(TemplateCache);
                Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(SurfaceImageCache);
                Palette = EC_CacheLightPal::AcquireOrCreateLightPalette(SurfacePaletteCache);
                if (HasCloud1) {
                    Cloud1 = EC_CachePalBitmap::AcquireOrCreatePalBitmap(Cloud1ImageCache);
                    CloudPalette1 = EC_CacheLightPal::AcquireOrCreateLightPalette(Cloud1PaletteCache);
                }
                if (HasCloud2) {
                    Cloud2 = EC_CachePalBitmap::AcquireOrCreatePalBitmap(Cloud2ImageCache);
                    CloudPalette2 = EC_CacheLightPal::AcquireOrCreateLightPalette(Cloud2PaletteCache);
                }
                if (HasCloud3) {
                    Cloud3 = EC_CachePalBitmap::AcquireOrCreatePalBitmap(Cloud3ImageCache);
                    CloudPalette3 = EC_CacheLightPal::AcquireOrCreateLightPalette(Cloud3PaletteCache);
                }
                if (GlobalsV::HardwareRenderingEnabled) {
                    if (TextureCache == nullptr) {
                        TextureCache = GR_DX::CreateTextureCache();
                    }
                    if (HitTestBounds.Right - HitTestBounds.Left != TextureSize.X || HitTestBounds.Bottom - HitTestBounds.Top != TextureSize.Y) {
                        RenderedMapOffsets[0] = 0x00ffffff;
                        RenderedMapOffsets[1] = RenderedMapOffsets[0];
                        RenderedMapOffsets[2] = RenderedMapOffsets[0];
                        RenderedMapOffsets[3] = RenderedMapOffsets[0];
                        for (Index = 0; Index <= 3; ++Index) {
                            TextureCache->SetSurface(nullptr, Index);
                        }
                    }
                    TextureSize = ClassesImports::Point(HitTestBounds.Right - HitTestBounds.Left, HitTestBounds.Bottom - HitTestBounds.Top);
                    Texture = (TextureCache->GetSurface(0, cpp_result), cpp_result);
                    if (SurfaceMapOffset != RenderedMapOffsets[0] || Texture == nullptr) {
                        RenderedMapOffsets[0] = SurfaceMapOffset;
                        if (Texture == nullptr) {
                            Texture = (GR_DX::GR_CreateTexture(TextureSize.X, TextureSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_2), cpp_result_2);
                        }
                        Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                        if (Locked.Bits != nullptr) {
                            if (Image->Bitmap->BytesPerPixel == 2) {
                                GR_Main::Ex_OKGR_Planet3_DrawAndLight_32(Locked.Bits, Locked.Pitch, Template->TemplateData, Image->Bitmap->Pixels, Image->Bitmap->PitchBytes, MapWidthMask, SurfaceMapOffset, RotatedLightBuffer, Palette->PaletteData, TextureSize.X / 2, TextureSize.Y / 2);
                            } else {
                                GR_Main::Ex_OKGR_Planet2_DrawAndLight_32(Locked.Bits, Locked.Pitch, Template->TemplateData, Image->Bitmap->Pixels, Image->Bitmap->PitchBytes, MapWidthMask, SurfaceMapOffset, RotatedLightBuffer, Palette->PaletteData, TextureSize.X / 2, TextureSize.Y / 2);
                            }
                            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                            TextureCache->SetSurface(Texture, 0);
                        }
                    }
                    GR_DX::DrawTexture(Texture, HitTestBounds.Left, HitTestBounds.Top, 255, 0x00ffffffu, &ClipRect, false, false);
                    if (HasCloud1) {
                        Texture = (TextureCache->GetSurface(1, cpp_result_3), cpp_result_3);
                        if (Cloud1MapOffset != RenderedMapOffsets[1]) {
                            RenderedMapOffsets[1] = Cloud1MapOffset;
                            if (Texture == nullptr) {
                                Texture = (GR_DX::GR_CreateTexture(TextureSize.X, TextureSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_4), cpp_result_4);
                            }
                            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                            if (Locked.Bits != nullptr) {
                                GR_Main::Ex_OKGR_Planet4_DrawAndLight_32(Locked.Bits, Locked.Pitch, Template->TemplateData, Cloud1->Bitmap->Pixels, Cloud1->Bitmap->PitchBytes, MapWidthMask, Cloud1MapOffset, RotatedLightBuffer, CloudPalette1->PaletteData, TextureSize.X / 2, TextureSize.Y / 2);
                            }
                            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                            TextureCache->SetSurface(Texture, 1);
                        }
                        GR_DX::DrawTexture(Texture, HitTestBounds.Left, HitTestBounds.Top, 255, 0x00ffffffu, &ClipRect, false, false);
                    } else {
                        TextureCache->SetSurface(nullptr, 1);
                    }
                    if (HasCloud2) {
                        Texture = (TextureCache->GetSurface(2, cpp_result_5), cpp_result_5);
                        if (Cloud2MapOffset != RenderedMapOffsets[2]) {
                            RenderedMapOffsets[2] = Cloud2MapOffset;
                            if (Texture == nullptr) {
                                Texture = (GR_DX::GR_CreateTexture(TextureSize.X, TextureSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_6), cpp_result_6);
                            }
                            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                            if (Locked.Bits != nullptr) {
                                GR_Main::Ex_OKGR_Planet4_DrawAndLight_32(Locked.Bits, Locked.Pitch, Template->TemplateData, Cloud2->Bitmap->Pixels, Cloud2->Bitmap->PitchBytes, MapWidthMask, Cloud2MapOffset, RotatedLightBuffer, CloudPalette2->PaletteData, TextureSize.X / 2, TextureSize.Y / 2);
                            }
                            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                            TextureCache->SetSurface(Texture, 2);
                        }
                        GR_DX::DrawTexture(Texture, HitTestBounds.Left, HitTestBounds.Top, 255, 0x00ffffffu, &ClipRect, false, false);
                    } else {
                        TextureCache->SetSurface(nullptr, 2);
                    }
                    if (HasCloud3) {
                        Texture = (TextureCache->GetSurface(3, cpp_result_7), cpp_result_7);
                        if (Cloud3MapOffset != RenderedMapOffsets[3]) {
                            RenderedMapOffsets[3] = Cloud3MapOffset;
                            if (Texture == nullptr) {
                                Texture = (GR_DX::GR_CreateTexture(TextureSize.X, TextureSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_8), cpp_result_8);
                            }
                            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                            if (Locked.Bits != nullptr) {
                                GR_Main::Ex_OKGR_Planet4_DrawAndLight_32(Locked.Bits, Locked.Pitch, Template->TemplateData, Cloud3->Bitmap->Pixels, Cloud3->Bitmap->PitchBytes, MapWidthMask, Cloud3MapOffset, RotatedLightBuffer, CloudPalette3->PaletteData, TextureSize.X / 2, TextureSize.Y / 2);
                            }
                            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                            TextureCache->SetSurface(Texture, 3);
                        }
                        GR_DX::DrawTexture(Texture, HitTestBounds.Left, HitTestBounds.Top, 255, 0x00ffffffu, &ClipRect, false, false);
                    } else {
                        TextureCache->SetSurface(nullptr, 3);
                    }
                    if (static_cast<std::uint8_t>(AtmosphereImageCache->HasEmptyCacheKey() ^ 1) && AtmosphereDirty && AtmosphereColor != 0) {
                        AtmosphereDirty = false;
                        GI_Planet::TPlanetGI_RebuildAtmosphereImage(this);
                        AtmosphereTextureSize = ClassesImports::Point(AtmosphereBuffer->Width, AtmosphereBuffer->Height);
                        Texture = (GR_DX::GR_CreateTexture(AtmosphereTextureSize.X, AtmosphereTextureSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_9), cpp_result_9);
                        if (Texture != nullptr) {
                            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                            if (Locked.Bits != nullptr) {
                                GR_Main::ExpandPaletteToBgra(Locked.Bits, Locked.Pitch, AtmosphereTextureSize.X, AtmosphereTextureSize.Y, AtmosphereBuffer->Pixels, AtmosphereBuffer->PitchBytes, AtmosphereBuffer->Palette);
                            }
                            Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                            TextureCache->SetSurface(Texture, 4);
                        }
                    }
                    Texture = (TextureCache->GetSurface(4, cpp_result_10), cpp_result_10);
                    if (static_cast<std::uint8_t>(AtmosphereImageCache->HasEmptyCacheKey() ^ 1) && Texture != nullptr && AtmosphereColor != 0) {
                        GR_DX::DrawTexture(Texture, HitTestBounds.Left, HitTestBounds.Top, 255, 0x00ffffffu, &ClipRect, false, false);
                    }
                } else {
                    if (Image->Bitmap->BytesPerPixel == 2) {
                        void* pixels = Image->Bitmap->Pixels;
                        std::int32_t pitchBytes = Image->Bitmap->PitchBytes;
                        std::int32_t mapWidthMask = MapWidthMask;
                        std::int32_t surfaceMapOffset = SurfaceMapOffset;
                        void* rotatedLightBuffer = RotatedLightBuffer;
                        void* paletteData = Palette->PaletteData;
                        std::int32_t cpp_arg = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2;
                        std::int32_t cpp_arg_2 = HitTestBounds.Top + (HitTestBounds.Bottom - HitTestBounds.Top) / 2;
                        const WindowsSdk::TRect& clipRect = ClipRect;
                        void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                        void* templateData = Template->TemplateData;
                        std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::Ex_OKGR_Planet3_DrawAndLightClip_16(pixels_2, pitchBytes_2, templateData, pixels, pitchBytes, mapWidthMask, surfaceMapOffset, rotatedLightBuffer, paletteData, cpp_arg, cpp_arg_2, clipRect);
                    } else {
                        void* pixels_3 = Image->Bitmap->Pixels;
                        std::int32_t pitchBytes_3 = Image->Bitmap->PitchBytes;
                        std::int32_t mapWidthMask_2 = MapWidthMask;
                        std::int32_t surfaceMapOffset_2 = SurfaceMapOffset;
                        void* rotatedLightBuffer_2 = RotatedLightBuffer;
                        void* paletteData_2 = Palette->PaletteData;
                        std::int32_t cpp_arg_3 = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2;
                        std::int32_t cpp_arg_4 = HitTestBounds.Top + (HitTestBounds.Bottom - HitTestBounds.Top) / 2;
                        const WindowsSdk::TRect& clipRect_2 = ClipRect;
                        void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                        void* templateData_2 = Template->TemplateData;
                        std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::Ex_OKGR_Planet2_DrawAndLightClip_16(pixels_4, pitchBytes_4, templateData_2, pixels_3, pitchBytes_3, mapWidthMask_2, surfaceMapOffset_2, rotatedLightBuffer_2, paletteData_2, cpp_arg_3, cpp_arg_4, clipRect_2);
                    }
                    if (HasCloud1) {
                        void* pixels_5 = Cloud1->Bitmap->Pixels;
                        std::int32_t pitchBytes_5 = Cloud1->Bitmap->PitchBytes;
                        std::int32_t mapWidthMask_3 = MapWidthMask;
                        std::int32_t cloud1MapOffset = Cloud1MapOffset;
                        void* rotatedLightBuffer_3 = RotatedLightBuffer;
                        void* paletteData_3 = CloudPalette1->PaletteData;
                        std::int32_t cpp_arg_5 = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2;
                        std::int32_t cpp_arg_6 = HitTestBounds.Top + (HitTestBounds.Bottom - HitTestBounds.Top) / 2;
                        const WindowsSdk::TRect& clipRect_3 = ClipRect;
                        void* pixels_6 = GR_Main::ScreenRenderBuffer->GetPixels();
                        void* templateData_3 = Template->TemplateData;
                        std::int32_t pitchBytes_6 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::Ex_OKGR_Planet4_DrawAndLightClip_16(pixels_6, pitchBytes_6, templateData_3, pixels_5, pitchBytes_5, mapWidthMask_3, cloud1MapOffset, rotatedLightBuffer_3, paletteData_3, cpp_arg_5, cpp_arg_6, clipRect_3);
                    }
                    if (HasCloud2) {
                        void* pixels_7 = Cloud2->Bitmap->Pixels;
                        std::int32_t pitchBytes_7 = Cloud2->Bitmap->PitchBytes;
                        std::int32_t mapWidthMask_4 = MapWidthMask;
                        std::int32_t cloud2MapOffset = Cloud2MapOffset;
                        void* rotatedLightBuffer_4 = RotatedLightBuffer;
                        void* paletteData_4 = CloudPalette2->PaletteData;
                        std::int32_t cpp_arg_7 = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2;
                        std::int32_t cpp_arg_8 = HitTestBounds.Top + (HitTestBounds.Bottom - HitTestBounds.Top) / 2;
                        const WindowsSdk::TRect& clipRect_4 = ClipRect;
                        void* pixels_8 = GR_Main::ScreenRenderBuffer->GetPixels();
                        void* templateData_4 = Template->TemplateData;
                        std::int32_t pitchBytes_8 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::Ex_OKGR_Planet4_DrawAndLightClip_16(pixels_8, pitchBytes_8, templateData_4, pixels_7, pitchBytes_7, mapWidthMask_4, cloud2MapOffset, rotatedLightBuffer_4, paletteData_4, cpp_arg_7, cpp_arg_8, clipRect_4);
                    }
                    if (HasCloud3) {
                        void* pixels_9 = Cloud3->Bitmap->Pixels;
                        std::int32_t pitchBytes_9 = Cloud3->Bitmap->PitchBytes;
                        std::int32_t mapWidthMask_5 = MapWidthMask;
                        std::int32_t cloud3MapOffset = Cloud3MapOffset;
                        void* rotatedLightBuffer_5 = RotatedLightBuffer;
                        void* paletteData_5 = CloudPalette3->PaletteData;
                        std::int32_t cpp_arg_9 = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2;
                        std::int32_t cpp_arg_10 = HitTestBounds.Top + (HitTestBounds.Bottom - HitTestBounds.Top) / 2;
                        const WindowsSdk::TRect& clipRect_5 = ClipRect;
                        void* pixels_10 = GR_Main::ScreenRenderBuffer->GetPixels();
                        void* templateData_5 = Template->TemplateData;
                        std::int32_t pitchBytes_10 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::Ex_OKGR_Planet4_DrawAndLightClip_16(pixels_10, pitchBytes_10, templateData_5, pixels_9, pitchBytes_9, mapWidthMask_5, cloud3MapOffset, rotatedLightBuffer_5, paletteData_5, cpp_arg_9, cpp_arg_10, clipRect_5);
                    }
                    if (static_cast<std::uint8_t>(AtmosphereImageCache->HasEmptyCacheKey() ^ 1) && AtmosphereDirty && AtmosphereColor != 0) {
                        AtmosphereDirty = false;
                        GI_Planet::TPlanetGI_RebuildAtmosphereImage(this);
                    }
                    if (static_cast<std::uint8_t>(AtmosphereImageCache->HasEmptyCacheKey() ^ 1) && AtmosphereBuffer->Pixels != nullptr && AtmosphereColor != 0) {
                        std::int32_t cpp_arg_11 = HitTestBounds.Top + (HitTestBounds.Bottom - HitTestBounds.Top) / 2 - pas::shr(AtmosphereBuffer->Height, 1);
                        GR_GraphBufPal::TGraphBufPalGR* atmosphereBuffer = AtmosphereBuffer;
                        WindowsSdk::TRect clipRect_6 = ClipRect;
                        std::int32_t cpp_arg_12 = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2 - pas::shr(AtmosphereBuffer->Width, 1);
                        void* pixels_11 = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t pitchBytes_11 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::BlendPaletteBuffer16Clipped(pixels_11, pitchBytes_11, cpp_arg_12, cpp_arg_11, atmosphereBuffer, clipRect_6);
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Template != nullptr) {
                TemplateCache->Release();
            }
            if (Image != nullptr) {
                SurfaceImageCache->Release();
            }
            if (Palette != nullptr) {
                SurfacePaletteCache->Release();
            }
            if (Cloud1 != nullptr) {
                Cloud1ImageCache->Release();
            }
            if (CloudPalette1 != nullptr) {
                Cloud1PaletteCache->Release();
            }
            if (Cloud2 != nullptr) {
                Cloud2ImageCache->Release();
            }
            if (CloudPalette2 != nullptr) {
                Cloud2PaletteCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    // Resizes and clears Buffer; excludes clouds and atmosphere.
    void TPlanetGI::RenderSurfaceToBuffer(GR_GraphBuf::TGraphBufGR* Buffer) {
        EC_CachePalBitmap::TCPalBitmapEC* Image = nullptr;
        EC_CacheLightPal::TCLightPalEC* Palette = nullptr;
        EC_CachePlanetTempl::TCPlanetTemplEC* Template = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Template = EC_CachePlanetTempl::AcquireOrCreatePlanetTemplate(TemplateCache);
                Image = EC_CachePalBitmap::AcquireOrCreatePalBitmap(SurfaceImageCache);
                Palette = EC_CacheLightPal::AcquireOrCreateLightPalette(SurfacePaletteCache);
                Buffer->AllocateRgbaTight(Template->ImageHeight + 4, Template->ImageHeight + 4);
                Buffer->ClearPixels();
                if (Image->Bitmap->BytesPerPixel == 2) {
                    void* pixels = Image->Bitmap->Pixels;
                    std::int32_t pitchBytes = Image->Bitmap->PitchBytes;
                    std::int32_t mapWidthMask = MapWidthMask;
                    std::int32_t surfaceMapOffset = SurfaceMapOffset;
                    void* rotatedLightBuffer = RotatedLightBuffer;
                    void* paletteData = Palette->PaletteData;
                    std::int32_t cpp_arg = pas::shr(Buffer->Width, 1);
                    std::int32_t cpp_arg_2 = pas::shr(Buffer->Height, 1);
                    void* pixels_2 = Buffer->GetPixels();
                    void* templateData = Template->TemplateData;
                    std::int32_t pitchBytes_2 = Buffer->PitchBytes;
                    GR_Main::Ex_OKGR_Planet3_DrawAndLight_32(pixels_2, pitchBytes_2, templateData, pixels, pitchBytes, mapWidthMask, surfaceMapOffset, rotatedLightBuffer, paletteData, cpp_arg, cpp_arg_2);
                } else {
                    void* pixels_3 = Image->Bitmap->Pixels;
                    std::int32_t pitchBytes_3 = Image->Bitmap->PitchBytes;
                    std::int32_t mapWidthMask_2 = MapWidthMask;
                    std::int32_t surfaceMapOffset_2 = SurfaceMapOffset;
                    void* rotatedLightBuffer_2 = RotatedLightBuffer;
                    void* paletteData_2 = Palette->PaletteData;
                    std::int32_t cpp_arg_3 = pas::shr(Buffer->Width, 1);
                    std::int32_t cpp_arg_4 = pas::shr(Buffer->Height, 1);
                    void* pixels_4 = Buffer->GetPixels();
                    void* templateData_2 = Template->TemplateData;
                    std::int32_t pitchBytes_4 = Buffer->PitchBytes;
                    GR_Main::Ex_OKGR_Planet2_DrawAndLight_32(pixels_4, pitchBytes_4, templateData_2, pixels_3, pitchBytes_3, mapWidthMask_2, surfaceMapOffset_2, rotatedLightBuffer_2, paletteData_2, cpp_arg_3, cpp_arg_4);
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Template != nullptr) {
                TemplateCache->Release();
            }
            if (Image != nullptr) {
                SurfaceImageCache->Release();
            }
            if (Palette != nullptr) {
                SurfacePaletteCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    // Queues the template, surface, surface palette and light rotation only.
    void TPlanetGI::QueueImageLoad(pas::List* PendingLoads) {
        TemplateCache->QueueLoadIfMissing(PendingLoads);
        SurfaceImageCache->QueueLoadIfMissing(PendingLoads);
        SurfacePaletteCache->QueueLoadIfMissing(PendingLoads);
        LightRotationCache->QueueLoadIfMissing(PendingLoads);
    }

    void TPlanetGI::p_destroy() {
        GI_Planet::TPlanetGI_Destroy(this);
    }

    void TPlanetGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Planet::TPlanetGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Planet
