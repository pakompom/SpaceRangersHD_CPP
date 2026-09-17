#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_TransImage {
    struct TTransImageGI;

} // namespace GI_TransImage

namespace GI_CheckBox {
    struct TCheckBoxGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCheckBoxGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TCheckBoxGI, GI_MessageLoop::TObjectGI, "TCheckBoxGI", 312)
        void p_destroy() override;
        // Does not refresh child activation or call inherited Clear.
        void Clear() override;
        void SetConfigPath(const pas::WideString& Path) override;
        void SetSize(Types::TPoint Size) override;
        void RefreshStateImages();
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        GI_TransImage::TTransImageGI* CheckedImage;
        GI_TransImage::TTransImageGI* UncheckedImage;
        std::uint8_t Checked;
        std::uint8_t cpp_padding[7];
        GI_MessageLoop::TObjectNotifyEventGI ChangedCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_CheckBox
