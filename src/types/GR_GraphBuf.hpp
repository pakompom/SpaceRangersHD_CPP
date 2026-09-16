#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_GraphBuf {
    struct TColorBGRA;

    struct TPixelFormatGR;

    struct TColorRGBA;

    struct TGraphBufGR;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGraphBufGR : EC_Struct::TObjectEx {
        PAS_CLASS_META(TGraphBufGR, EC_Struct::TObjectEx, "TGraphBufGR", 44)
        void p_destroy() override;
        void Clear();
        // Locks texture storage for writing if necessary.
        void* GetPixels();
        // Records 16-bit pixels; software pitch uses CurrentPixelFormat.BytesPerPixel and four-byte alignment.
        void AllocateNative(std::int32_t Width, std::int32_t Height);
        void AllocateNativePitch(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes);
        void AttachPixels(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes, void* Data);
        // Software storage uses Width*4 pitch; texture storage uses the returned surface pitch.
        void AllocateRgbaTight(std::int32_t Width, std::int32_t Height);
        void AllocateRgba(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes);
        void AllocateRgbTight(std::int32_t Width, std::int32_t Height);
        // Always allocates software storage, even when UseTexture is enabled.
        void AllocateRgb(std::int32_t Width, std::int32_t Height, std::int32_t PitchBytes);
        // Eight-bit software pixels with four-byte-aligned pitch.
        void AllocateGrayscale(std::int32_t Width, std::int32_t Height);
        // Decode the entire file payload, ignoring Buffer.Position. Failures raise.
        // Uses CurrentPixelFormat masks and byte width.
        void LoadImage(EC_Buf::TBufEC* Buffer);
        // Produces BGRA byte order, with alpha in the high byte.
        void LoadImageRgba(EC_Buf::TBufEC* Buffer);
        // Produces RGB byte order.
        void LoadImageRgb(EC_Buf::TBufEC* Buffer);
        void LoadImageGrayscale(EC_Buf::TBufEC* Buffer);
        std::uint32_t GetPixel16(std::int32_t X, std::int32_t Y);
        void SetPixel16(std::int32_t X, std::int32_t Y, std::uint32_t Color);
        std::uint32_t GetBrightness16(std::int32_t X, std::int32_t Y);
        void BlendPixel16(std::int32_t X, std::int32_t Y, std::uint32_t Color, std::uint8_t Alpha);
        void DrawAlphaLine16(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::uint8_t Alpha, WindowsSdk::TRect Clip);
        std::uint32_t GetPixel32(std::int32_t X, std::int32_t Y);
        void DrawHorizontalLine16Clipped(std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color, WindowsSdk::TRect Clip);
        void DrawVerticalLine16Clipped(std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color, WindowsSdk::TRect Clip);
        void DrawLine16(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color);
        void DrawAnimatedLine16(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t Phase, WindowsSdk::TRect Clip);
        void DrawShadowLine16(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t Phase, WindowsSdk::TRect Clip, void* ShadowPixels, std::int32_t ShadowPitch);
        void DrawAlphaTrapezium16(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, std::uint8_t Alpha, WindowsSdk::TRect Clip);
        void DrawLine16Clipped(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, WindowsSdk::TRect Clip);
        // RGBA pixels; coverage replaces the color alpha. Blending preserves an existing pixel's alpha unless coverage is fully opaque.
        void DrawAntialiasedLine(WindowsSdk::TPoint FirstPoint, WindowsSdk::TPoint SecondPoint, std::uint32_t Color);
        void ClearPixels();
        void FillPixels(std::uint8_t Value);
        void FillPixels16(std::uint16_t Color);
        void FlipHorizontal16();
        void RotateLeft16();
        void Stretch16(std::uint32_t Width, std::uint32_t Height);
        void ConvertRgbTo565();
        void Convert565ToRgb();
        void ShiftLight16(std::int32_t Shift, WindowsSdk::TRect Rect);
        void DrawCircle16(WindowsSdk::TPoint Center, std::int32_t Radius, std::uint32_t OutlineColor, std::uint32_t FillColor, WindowsSdk::TRect Clip);
        void DrawCircle8(WindowsSdk::TPoint Center, std::int32_t Radius, std::uint32_t OutlineColor, std::uint32_t FillColor, WindowsSdk::TRect Clip);
        void ApplyOperations(const pas::WideString& Operations);
        void RescaleRgb(std::int32_t Width, std::int32_t Height);
        void RescaleRgba(std::int32_t Width, std::int32_t Height, std::int32_t Filter);
        void RescaleBilinearRgba(std::int32_t Width, std::int32_t Height);
        void FillPolygon32(pas::OpenArray<WindowsSdk::TPoint> Points, std::uint32_t Color);
        WindowsSdk::TPoint GetPixelCentroid();
        // Replaces Buffer with width, height, pitch and raw pixels. Leaves Position at 12, before the pixel data.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Accepts zlib-packed or raw buffer data. Allocates software pixels and leaves Position immediately after the 12-byte image header.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        // Assumes four-byte BGRA pixels. Converts FileName to ANSI and ignores the writer's status.
        void SavePng(pas::WideString FileName);
        // Assumes 32-bit BGRA pixels; alpha is excluded. Converts FileName to ANSI and ignores the writer's status.
        void SaveBmp(pas::WideString FileName);
        // Writes an intermediate BMP to FileName, then replaces it with JPEG. Quality is truncated to one byte; exceptions after the BMP write are swallowed.
        void SaveJpeg(pas::WideString FileName, std::int32_t Quality);
        void DrawNinePatch(std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, TGraphBufGR* Source, WindowsSdk::TRect SourceRect, WindowsSdk::TRect Borders);
        void RescaleWithAspect(std::uint32_t Width, std::uint32_t Height, std::uint8_t CropToAspect, std::int32_t HorizontalAlign, std::int32_t VerticalAlign, std::int32_t Filter);
        // Alignment values: 0=start, 1=center, 2=end.
        void RescaleRGBA_HW(std::uint32_t Width, std::uint32_t Height, std::uint8_t CropToAspect, std::int32_t HorizontalAlign, std::int32_t VerticalAlign);
        void Crop(WindowsSdk::TRect Rect);
        void AdjustBrightness(std::int32_t Percent);
        void ConvertToGrayscale();
        void GetTexture(Direct3D9::IDirect3DTexture9& Result);
        // The byte-sized option is ignored in this build.
        void LoadFromScreen(std::uint8_t UnusedOption);
        void LockTexture(std::uint8_t ReadOnly);
        void UnlockTexture();
        void ConvertBgraToRgb24();
        void DrawAntialiasedCircle16(WindowsSdk::TPoint Center, std::int32_t Radius, std::uint32_t Color, WindowsSdk::TRect Clip);
        void DrawAntialiasedLine16(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::TRect Clip);
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t PitchBytes;
        void* Pixels;
        // Zero owns software pixels; one borrows them.
        std::int32_t StorageKind;
        std::int32_t BitsPerPixel;
        std::int32_t BytesPerPixel;
        std::uint8_t UseTexture;
        std::uint8_t UsesTextureStorage;
        // Cleared on allocation and reset; purpose unresolved.
        std::uint8_t TextureFlag22;
        std::uint8_t cpp_padding[1];
        Direct3D9::IDirect3DTexture9 Texture;
        std::uint8_t TextureLocked;
        std::uint8_t TextureLockedReadOnly;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PColorRGBA = TColorRGBA*;

    #pragma pack(push, 1)
    struct TColorRGBA {
        std::uint8_t R;
        std::uint8_t G;
        std::uint8_t B;
        std::uint8_t A;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPixelFormatGR : pas::Object {
        PAS_CLASS_META(TPixelFormatGR, pas::Object, "TPixelFormatGR", 76)
        // Ignores disjoint bits after each mask's first contiguous run; BytesPerPixel is unchanged.
        void RebuildChannelMetrics();
        std::uint32_t PackRgbBytes(std::uint8_t Red, std::uint8_t Green, std::uint8_t Blue);
        std::uint32_t PackRgb(std::int32_t Red, std::int32_t Green, std::int32_t Blue);
        // Does not clamp inputs or include alpha.
        std::uint32_t PackNormalizedRgb(double Red, double Green, double Blue);
        std::uint32_t InterpolateRgb(std::uint32_t First, std::uint32_t Second, float Amount);
        std::uint8_t UnpackRed(std::uint32_t Color);
        std::uint8_t UnpackGreen(std::uint32_t Color);
        static std::uint8_t UnpackBlue(std::uint32_t Color);
        std::uint32_t RedMask;
        std::uint32_t GreenMask;
        std::uint32_t BlueMask;
        std::uint32_t AlphaMask;
        std::uint32_t RedShift;
        std::uint32_t GreenShift;
        std::uint32_t BlueShift;
        std::uint32_t AlphaShift;
        std::uint32_t RedLevels;
        std::uint32_t GreenLevels;
        std::uint32_t BlueLevels;
        std::uint32_t AlphaLevels;
        std::uint32_t RedBits;
        std::uint32_t GreenBits;
        std::uint32_t BlueBits;
        std::uint32_t AlphaBits;
        std::int32_t BytesPerPixel;
        std::uint32_t TotalChannelBits;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Screen/texture byte order used by the brightness and grayscale routines.
    #pragma pack(push, 1)
    struct TColorBGRA {
        std::uint8_t B;
        std::uint8_t G;
        std::uint8_t R;
        std::uint8_t A;
    };
    #pragma pack(pop)

    using PColorBGRA = TColorBGRA*;

    using TColorRGBAArray = pas::Array<TColorRGBA, 0, 0>;

    using PColorRGBAArray = TColorRGBAArray*;

} // namespace GR_GraphBuf
