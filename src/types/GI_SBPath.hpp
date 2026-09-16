#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_SBPath {
    struct TSBPathGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSBPathGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TSBPathGI, GI_MessageLoop::TObjectGI, "TSBPathGI", 328)
        void p_destroy() override;
        void Clear() override;
        void SetImagePath(pas::WideString Path);
        void SetPositionValue(std::int32_t Value);
        void UpdateThumbPosition();
        std::int32_t PositionFromPointIndex(std::int32_t Index);
        std::int32_t FindClosestPoint(Types::TPoint Point, std::int32_t& DistanceSquared);
        void OnActivate() override;
        void OnDeactivate() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadPathProperties(EC_BlockPar::TBlockParEC* Block);
        std::int32_t PointCount;
        pas::DynArray<Types::TPoint> Points;
        std::int32_t Minimum;
        std::int32_t Maximum;
        std::int32_t Position;
        std::uint8_t Dragging;
        std::uint8_t cpp_padding[3];
        GI_Image::TImageGI* ThumbImage;
        std::int32_t HitRadius;
        GI_MessageLoop::TObjectNotifyEventGI ChangeCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_SBPath
