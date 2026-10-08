#include "units/BitmapPorts.hpp"

// Pascal implementations of the recovered palette and pixel operations.
namespace BitmapPorts {
    // Move keeps Word/Cardinal loads and stores valid for unaligned pixel rows.
    // Copy32 snapshots one complete pixel before writing it, then moves forward;
    // replacing the whole operation with Move would change overlapping-copy results.
    void MaskPixels8(void* Image, void* Mask, void* Dest, std::int32_t Columns, std::int32_t Rows,
        std::int32_t ImageSkip, std::int32_t MaskSkip, std::int32_t DestSkip, void* Table) {
        std::int32_t X{};
        std::int32_t Index{};
        std::uint8_t* SourceBytes = static_cast<std::uint8_t*>(Image);
        std::uint8_t* MaskBytes = static_cast<std::uint8_t*>(Mask);
        std::uint8_t* DestBytes = static_cast<std::uint8_t*>(Dest);
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                Index = *reinterpret_cast<std::uint8_t*>(MaskBytes) * 256 +
                        *reinterpret_cast<std::uint8_t*>(SourceBytes);
                *reinterpret_cast<std::uint8_t*>(DestBytes) = *reinterpret_cast<std::uint8_t*>(pas::pointer_add(
                        static_cast<std::uint8_t*>(Table), Index));
                pas::inc(SourceBytes);
                pas::inc(MaskBytes);
                pas::inc(DestBytes);
                ++X;
            }
            SourceBytes = pas::pointer_add(SourceBytes, ImageSkip);
            MaskBytes = pas::pointer_add(MaskBytes, MaskSkip);
            DestBytes = pas::pointer_add(DestBytes, DestSkip);
            ++Y;
        }
    }

    void BlendPalette16(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip,
        std::int32_t DestSkip, void* Palette, void* Table, std::uint8_t RGB565) {
        std::uint8_t* SourceBytes{};
        std::uint8_t* DestBytes{};
        std::uint8_t* Foreground{};
        std::uint8_t* Background{};
        std::int32_t X{};
        std::int32_t RedShift{};
        std::int32_t GreenShift{};
        std::uint32_t RedMask{};
        std::uint32_t GreenMask{};
        std::uint32_t GreenBits{};
        std::uint32_t Color{};
        std::uint32_t Alpha{};
        std::uint32_t Value{};
        std::uint16_t OldPixel{};
        std::uint16_t NewPixel{};
        auto Pack = [&](std::uint32_t R, std::uint32_t G, std::uint32_t B) -> std::uint32_t {
            return pas::shl(R, RedShift) & RedMask | pas::shl(G, GreenShift) & GreenMask | B >> 3;
        };
        SourceBytes = static_cast<std::uint8_t*>(Source);
        DestBytes = static_cast<std::uint8_t*>(Dest);
        if (RGB565) {
            RedShift = 8;
            GreenShift = 3;
            RedMask = 0x0000f800u;
            GreenMask = 0x000007e0u;
            GreenBits = 0x000000fcu;
        } else {
            RedShift = 7;
            GreenShift = 2;
            RedMask = 0x00007c00u;
            GreenMask = 0x000003e0u;
            GreenBits = 0x000000f8u;
        }
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                pas::move_memory(
                    pas::pointer_add(static_cast<std::uint8_t*>(Palette),
                        *reinterpret_cast<std::uint8_t*>(SourceBytes) * 4), &Color,
                    static_cast<std::int32_t>(sizeof(std::uint32_t)));
                Alpha = Color >> 24;
                // Transparent entries do not even read or write the destination pixel.
                if (Alpha != 0) {
                    Foreground = pas::pointer_add(static_cast<std::uint8_t*>(Table),
                            static_cast<std::int32_t>(Alpha * 256));
                    Background = pas::pointer_add(static_cast<std::uint8_t*>(Table),
                            static_cast<std::int32_t>((255 - Alpha) * 256));
                    pas::move_memory(DestBytes, &OldPixel, static_cast<std::int32_t>(sizeof(std::uint16_t)));
                    Value = Pack(
                            *reinterpret_cast<std::uint8_t*>(pas::pointer_add(Foreground,
                                static_cast<std::int32_t>(Color & 255))),
                            *reinterpret_cast<std::uint8_t*>(pas::pointer_add(Foreground,
                                static_cast<std::int32_t>(Color >> 8 & 255))),
                            *reinterpret_cast<std::uint8_t*>(pas::pointer_add(Foreground,
                                static_cast<std::int32_t>(Color >> 16 & 255)))) + Pack(
                                *reinterpret_cast<std::uint8_t*>(pas::pointer_add(Background,
                                static_cast<std::int32_t>(pas::shr(static_cast<std::uint32_t>(OldPixel), RedShift) &
                                0x000000f8))),
                                *reinterpret_cast<std::uint8_t*>(pas::pointer_add(Background,
                                static_cast<std::int32_t>(pas::shr(static_cast<std::uint32_t>(OldPixel), GreenShift) &
                                GreenBits))),
                                *reinterpret_cast<std::uint8_t*>(pas::pointer_add(Background,
                                static_cast<std::int32_t>(static_cast<std::uint32_t>(OldPixel) << 3 & 0x000000f8))));
                    // Quantize each contribution separately before adding and narrowing.
                    NewPixel = Value;
                    pas::move_memory(&NewPixel, DestBytes, static_cast<std::int32_t>(sizeof(std::uint16_t)));
                }
                pas::inc(SourceBytes);
                DestBytes = pas::pointer_add(DestBytes, 2);
                ++X;
            }
            SourceBytes = pas::pointer_add(SourceBytes, SourceSkip);
            DestBytes = pas::pointer_add(DestBytes, DestSkip);
            ++Y;
        }
    }

    void CopyPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip,
        std::int32_t DestSkip) {
        std::uint8_t* SourceBytes{};
        std::uint8_t* DestBytes{};
        std::int32_t X{};
        std::uint32_t Pixel{};
        SourceBytes = static_cast<std::uint8_t*>(Source);
        DestBytes = static_cast<std::uint8_t*>(Dest);
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                pas::move_memory(SourceBytes, &Pixel, static_cast<std::int32_t>(sizeof(std::uint32_t)));
                pas::move_memory(&Pixel, DestBytes, static_cast<std::int32_t>(sizeof(std::uint32_t)));
                SourceBytes = pas::pointer_add(SourceBytes, 4);
                DestBytes = pas::pointer_add(DestBytes, 4);
                ++X;
            }
            SourceBytes = pas::pointer_add(SourceBytes, SourceSkip);
            DestBytes = pas::pointer_add(DestBytes, DestSkip);
            ++Y;
        }
    }

    void BlendPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip,
        std::int32_t DestSkip, void* Table) {
        std::int32_t X{};
        std::int32_t Channel{};
        std::int32_t ForegroundIndex{};
        std::int32_t BackgroundIndex{};
        std::uint32_t Value{};
        std::uint8_t* SourceBytes = static_cast<std::uint8_t*>(Source);
        std::uint8_t* DestBytes = static_cast<std::uint8_t*>(Dest);
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                Channel = 0;
                while (Channel < 3) {
                    // Re-read source alpha per channel: source and destination may overlap.
                    ForegroundIndex = *reinterpret_cast<std::uint8_t*>(pas::pointer_add(SourceBytes, Channel)) * 256 +
                            *reinterpret_cast<std::uint8_t*>(pas::pointer_add(SourceBytes, 3));
                    BackgroundIndex = *reinterpret_cast<std::uint8_t*>(pas::pointer_add(DestBytes, Channel)) * 256 +
                            255 - *reinterpret_cast<std::uint8_t*>(pas::pointer_add(SourceBytes, 3));
                    Value = static_cast<std::uint32_t>(*reinterpret_cast<std::uint8_t*>(pas::pointer_add(
                            static_cast<std::uint8_t*>(Table), ForegroundIndex))) +
                            static_cast<std::uint32_t>(*reinterpret_cast<std::uint8_t*>(pas::pointer_add(
                                static_cast<std::uint8_t*>(Table), BackgroundIndex)));
                    *reinterpret_cast<std::uint8_t*>(pas::pointer_add(DestBytes, Channel)) = Value;
                    ++Channel;
                }
                Value = static_cast<std::uint32_t>(*reinterpret_cast<std::uint8_t*>(pas::pointer_add(SourceBytes, 3))) +
                        static_cast<std::uint32_t>(*reinterpret_cast<std::uint8_t*>(pas::pointer_add(DestBytes, 3)));
                if (Value > 255) {
                    Value = 255u;
                }
                *reinterpret_cast<std::uint8_t*>(pas::pointer_add(DestBytes, 3)) = Value;
                SourceBytes = pas::pointer_add(SourceBytes, 4);
                DestBytes = pas::pointer_add(DestBytes, 4);
                ++X;
            }
            SourceBytes = pas::pointer_add(SourceBytes, SourceSkip);
            DestBytes = pas::pointer_add(DestBytes, DestSkip);
            ++Y;
        }
    }

    void ShadowPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip) {
        std::uint8_t* DestBytes{};
        std::int32_t X{};
        std::uint32_t Pixel{};
        DestBytes = static_cast<std::uint8_t*>(Pixels);
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                pas::move_memory(DestBytes, &Pixel, static_cast<std::int32_t>(sizeof(std::uint32_t)));
                Pixel = Pixel >> 2 & 0xff000000u;
                pas::move_memory(&Pixel, DestBytes, static_cast<std::int32_t>(sizeof(std::uint32_t)));
                DestBytes = pas::pointer_add(DestBytes, 4);
                ++X;
            }
            DestBytes = pas::pointer_add(DestBytes, RowSkip);
            ++Y;
        }
    }

    void SwapRedBlue32(void* Pixels, std::int32_t Count) {
        std::uint8_t Saved{};
        std::uint8_t* DestBytes = static_cast<std::uint8_t*>(Pixels);
        std::int32_t I = 0;
        while (I < Count) {
            Saved = *reinterpret_cast<std::uint8_t*>(DestBytes);
            *reinterpret_cast<std::uint8_t*>(DestBytes) = *reinterpret_cast<std::uint8_t*>(pas::pointer_add(DestBytes,
                    2));
            *reinterpret_cast<std::uint8_t*>(pas::pointer_add(DestBytes, 2)) = Saved;
            DestBytes = pas::pointer_add(DestBytes, 4);
            ++I;
        }
    }

    void FillLine16(void* Pixels, std::int32_t Count, std::int32_t Step, std::uint32_t Color) {
        std::uint8_t* DestBytes{};
        std::uint16_t Pixel{};
        DestBytes = static_cast<std::uint8_t*>(Pixels);
        Pixel = Color;
        std::int32_t I = 0;
        while (I < Count) {
            pas::move_memory(&Pixel, DestBytes, static_cast<std::int32_t>(sizeof(std::uint16_t)));
            DestBytes = pas::pointer_add(DestBytes, Step);
            ++I;
        }
    }

    void FillPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip,
        std::uint32_t Color) {
        std::uint8_t* DestBytes{};
        std::int32_t X{};
        DestBytes = static_cast<std::uint8_t*>(Pixels);
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                pas::move_memory(&Color, DestBytes, static_cast<std::int32_t>(sizeof(std::uint32_t)));
                DestBytes = pas::pointer_add(DestBytes, 4);
                ++X;
            }
            DestBytes = pas::pointer_add(DestBytes, RowSkip);
            ++Y;
        }
    }

    void ScaleAlpha32(void* Alpha, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, void* Table) {
        std::int32_t X{};
        std::uint8_t* DestBytes = static_cast<std::uint8_t*>(Alpha);
        std::int32_t Y = 0;
        while (Y < Rows) {
            X = 0;
            while (X < Columns) {
                *reinterpret_cast<std::uint8_t*>(DestBytes) = *reinterpret_cast<std::uint8_t*>(pas::pointer_add(
                        static_cast<std::uint8_t*>(Table), *reinterpret_cast<std::uint8_t*>(DestBytes)));
                DestBytes = pas::pointer_add(DestBytes, 4);
                ++X;
            }
            DestBytes = pas::pointer_add(DestBytes, RowSkip);
            ++Y;
        }
    }

} // namespace BitmapPorts
