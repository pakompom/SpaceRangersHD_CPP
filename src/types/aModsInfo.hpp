#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace aModsInfo {
    struct TModInfo;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TModInfo : EC_Struct::TObjectEx {
        PAS_CLASS_META(TModInfo, EC_Struct::TObjectEx, "TModInfo", 72)
        void p_destroy() override;
        pas::WideString GetDisplayName();
        TModInfo* GetConflict(std::int32_t Index, std::int32_t VariantIndex);
        TModInfo* GetDependency(std::int32_t Index, std::int32_t VariantIndex);
        pas::WideString Folder;
        GI_Image::TImageGI* SwitchImage;
        pas::WideString IndexText;
        pas::WideString Name;
        pas::WideString Section;
        pas::WideString SmallDescription;
        pas::WideString FullDescription;
        pas::WideString Author;
        pas::WideString DependencyNames;
        std::int32_t DependencyCount;
        pas::DynArray<TModInfo*> Dependencies;
        pas::WideString ConflictNames;
        std::int32_t ConflictCount;
        pas::DynArray<TModInfo*> Conflicts;
        std::uint32_t Priority;
        std::uint8_t UnsupportedLanguage;
        std::uint8_t MissingFolder;
        std::uint8_t MissingDependency;
        std::uint8_t Misplaced;
        std::uint8_t DuplicateName;
        std::uint8_t ReferencedAsConflict;
        std::uint8_t ReferencedAsDependency;
        std::uint8_t Selected;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aModsInfo
