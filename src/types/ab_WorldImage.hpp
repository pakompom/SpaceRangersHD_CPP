#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace ab_WorldImage {
    struct TabWorldImage;

    using PabWorldImage = TabWorldImage*;

    struct TabWorldImage {
        PabWorldImage Prev;
        PabWorldImage Next;
        EC_Struct::TVector3D Position;
        GI_Image::TImageGI* Image;
        pas::WideString FrontImagePath;
        pas::WideString BackImagePath;
        float FrontDepth;
        float BackDepth;
        std::uint8_t Dirty;
        std::uint8_t cpp_padding[3];
        std::int32_t FrameMode;
        std::uint8_t LoopAnimation;
        std::uint8_t Finished;
        std::uint8_t StopAnimation;
        std::uint8_t cpp_padding_2[1];
    };

    inline constexpr std::int32_t afmRestart = 0;

    inline constexpr std::int32_t afmRandomStart = 1;

} // namespace ab_WorldImage
