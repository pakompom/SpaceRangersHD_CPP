#pragma once
#include "types/EC_Struct.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_WorldImage {
    extern std::uint32_t WorldImageHeap;

    extern ab_WorldImage::PabWorldImage FirstWorldImage;

    extern ab_WorldImage::PabWorldImage LastWorldImage;

    void ab_WorldImage_Clear();

    // Allocates and links a node owned by the world list.
    PabWorldImage ab_WorldImage_Add();

    void ab_WorldImage_Delete(PabWorldImage Entry);

    PabWorldImage ab_WorldImage_Create(EC_Struct::TVector3D Position, const pas::WideString& FrontPath, const pas::WideString& BackPath, std::uint8_t StopAnimation);

    void ab_WorldImage_Set(PabWorldImage Entry, EC_Struct::TVector3D Position, const pas::WideString& FrontPath, const pas::WideString& BackPath);

    void ab_WorldImage_SetPosition(PabWorldImage Entry, EC_Struct::TVector3D Position);

    void ab_WorldImage_SetDepth(PabWorldImage Entry, float FrontDepth, float BackDepth);

    void ab_WorldImage_SetFrameMode(PabWorldImage Entry, std::int32_t Value);

    void ab_WorldImage_SetLooping(PabWorldImage Entry, std::uint8_t Value);

    void ab_WorldImage_Update();

} // namespace ab_WorldImage
