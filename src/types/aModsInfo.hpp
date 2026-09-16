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
        // Relative path beneath Mods.
        pas::WideString Folder;
        // Borrowed module-manager control.
        GI_Image::TImageGI* SwitchImage;
        // Decimal index in ModInfos.
        pas::WideString IndexText;
        // Metadata ID; duplicates are permitted.
        pas::WideString Name;
        pas::WideString Section;
        pas::WideString SmallDescription;
        pas::WideString FullDescription;
        pas::WideString Author;
        // Comma-separated IDs.
        pas::WideString DependencyNames;
        std::int32_t DependencyCount;
        // Borrowed references.
        pas::DynArray<TModInfo*> Dependencies;
        // Comma-separated IDs.
        pas::WideString ConflictNames;
        std::int32_t ConflictCount;
        // Borrowed references.
        pas::DynArray<TModInfo*> Conflicts;
        std::uint32_t Priority;
        std::uint8_t UnsupportedLanguage;
        std::uint8_t MissingFolder;
        std::uint8_t MissingDependency;
        // Reported as ProblemsInfoMisplaced by the module manager.
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
