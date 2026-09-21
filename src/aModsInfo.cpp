#include "layout/aModsInfo.hpp"
#include "types/GI_Image.hpp"
#include "types/SystemImports.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Expression.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aModsInfo.hpp"

// Native TModInfo RTTI and module-manager accesses establish the class layout.
namespace aModsInfo {
    // Owns the TModInfo objects.
    pas::List* ModInfos = nullptr;

    // Non-owning references into ModInfos.
    pas::List* SelectedModInfos = nullptr;

    EC_BlockPar::TBlockParEC* ModIdCounts = nullptr;

    // Maps declared conflict IDs to matching mod indices.
    EC_BlockPar::TBlockParEC* ModConflictIndex = nullptr;

    // Maps required mod IDs to matching mod indices.
    EC_BlockPar::TBlockParEC* ModDependencyIndex = nullptr;

    std::uint8_t ModInfosInitialized = false;

    std::int32_t FindOrInsertModFolder(pas::WideString Folder) {
        std::int32_t Result{};
        std::int32_t Middle{};
        TModInfo* Info{};
        if (pas::list_count(ModInfos) < 1) {
            pas::list_add(ModInfos, nullptr);
            return 0;
        }
        std::int32_t Left = 0;
        Info = pas::list_at<TModInfo>(ModInfos, 0);
        std::int32_t Comparison = EC_Expression::CompareScriptNames(Folder.pchar(), Info->Folder.pchar());
        if (Comparison == 0) {
            return 0;
        }
        if (Comparison < 0) {
            pas::list_insert(ModInfos, 0, nullptr);
            return 0;
        }
        std::int32_t Right = pas::list_count(ModInfos) - 1;
        Info = pas::list_at<TModInfo>(ModInfos, Right);
        Comparison = EC_Expression::CompareScriptNames(Folder.pchar(), Info->Folder.pchar());
        if (Comparison == 0) {
            return Right;
        }
        if (Comparison > 0) {
            pas::list_add(ModInfos, nullptr);
            return Right + 1;
        }
        while (true) {
            if (Right - Left < 2) {
                pas::list_insert(ModInfos, Right, nullptr);
                return Right;
            }
            Middle = (Left + Right) / 2;
            Info = pas::list_at<TModInfo>(ModInfos, Middle);
            Comparison = EC_Expression::CompareScriptNames(Folder.pchar(), Info->Folder.pchar());
            if (Comparison == 0) {
                return Middle;
            }
            if (Comparison < 0) {
                Right = Middle;
            } else {
                Left = Middle;
            }
        }
        return Result;
    }

    std::uint8_t LoadModInfo(pas::WideString Folder, TModInfo* Info) {
        std::uint8_t Result{};
        pas::AnsiString SavedDir{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Language{};
        pas::WideString Languages{};
        std::int32_t I{};
        std::int32_t Count{};
        auto HasOtherLanguageResources = [&]() -> std::uint8_t {
            pas::WideString FileName{};
            Windows::TWin32FindDataA FindData{};
            std::uint8_t Result = false;
            FindData.dwFileAttributes = WindowsImports::FILE_ATTRIBUTE_NORMAL;
            WindowsImports::THandle Handle = WindowsSdk::FindFirstFile(pas::literal_pointer("*.txt"), FindData);
            if (Handle != WindowsImports::INVALID_HANDLE_VALUE) {
                do {
                    if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) == 0) {
                        FileName = pas::array_text<pas::WideString>(FindData.cFileName.elements, 260);
                        FileName = EC_Str::LowerCaseWideString(FileName);
                        if (([&] {
                            pas::WideString cpp_string = EC_Str::LowerCaseWideString(pas::concat_wide({u"install_", GR_Main::SelectedLanguage, u".txt"}));
                            return FileName != cpp_string;
                        }()) && FileName.length() > 12 && EC_Str::FindTextOffsetW(FileName, u"install_"_wref.get(), 0) == 0) {
                            Result = true;
                            WindowsImports::FindClose(Handle);
                            return Result;
                        }
                    }
                } while (WindowsSdk::FindNextFile(Handle, FindData));
                WindowsImports::FindClose(Handle);
            }
            if (SysUtilsImports::DirectoryExists(static_cast<pas::AnsiString>(pas::concat_wide({Folder, u"\\CFG"})))) {
                pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
                std::exception_ptr cpp_error{};
                try {
                    SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(pas::concat_wide({Folder, u"\\CFG"})));
                    FindData.dwFileAttributes = WindowsImports::FILE_ATTRIBUTE_NORMAL;
                    Handle = WindowsSdk::FindFirstFile(pas::literal_pointer("*.*"), FindData);
                    if (Handle != WindowsImports::INVALID_HANDLE_VALUE) {
                        do {
                            if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) != 0) {
                                FileName = pas::array_text<pas::WideString>(FindData.cFileName.elements, 260);
                                if (FileName != u"." && FileName != u".." && ([&] {
                                    pas::WideString cpp_string_2 = EC_Str::LowerCaseWideString(FileName);
                                    pas::WideString cpp_string_3 = EC_Str::LowerCaseWideString(GR_Main::LanguageInstallConfig->GetParam(u"Lang"sv));
                                    return cpp_string_2 != cpp_string_3;
                                }()) && SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({FileName, u"\\Lang.dat"})))) {
                                    Result = true;
                                    WindowsImports::FindClose(Handle);
                                    cpp_flow = pas::FinallyFlow::Return;
                                    goto cpp_cleanup;
                                }
                            }
                        } while (WindowsSdk::FindNextFile(Handle, FindData));
                        WindowsImports::FindClose(Handle);
                    }
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                cpp_cleanup:;
                SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(Folder));
                if (cpp_error) {
                    std::rethrow_exception(cpp_error);
                }
                if (cpp_flow == pas::FinallyFlow::Return) {
                    return Result;
                }
            }
            return Result;
        };
        std::uint8_t HasForeignResources = false;
        std::uint8_t HasLanguageResources = false;
        std::uint8_t HasCommonResources = false;
        Block = nullptr;
        SavedDir = SysUtilsImports::GetCurrentDir();
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(Folder));
                if (SysUtilsImports::FileExists("install.txt"_a) || SysUtilsImports::FileExists("CFG\\Main.dat"_a) || SysUtilsImports::FileExists("CFG\\CacheData.dat"_a)) {
                    HasCommonResources = true;
                }
                Language = GR_Main::LanguageInstallConfig->GetParam(u"Lang"sv);
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"install_", GR_Main::SelectedLanguage, u".txt"}))) || SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"CFG\\", Language, u"\\Lang.dat"})))) {
                    HasLanguageResources = true;
                }
                if (static_cast<std::uint8_t>(HasCommonResources ^ 1) && static_cast<std::uint8_t>(HasLanguageResources ^ 1)) {
                    HasForeignResources = HasOtherLanguageResources();
                }
                Result = HasCommonResources || HasLanguageResources || HasForeignResources;
                if (!SysUtilsImports::FileExists("ModuleInfo.txt"_a)) {
                    if (!Result) {
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    if (!HasLanguageResources) {
                        if (!HasForeignResources) {
                            HasForeignResources = HasOtherLanguageResources();
                        }
                        Info->UnsupportedLanguage = HasForeignResources;
                    }
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                try {
                    Block->LoadFromTextFileWithEncodingProbe(pas::literal_pointer(u"ModuleInfo.txt"), false);
                    if (!Result) {
                        if (Block->CountParams(u"NoNormalResources"_wref.get()) <= 0) {
                            pas::free(Block);
                            Block = nullptr;
                            cpp_flow = pas::FinallyFlow::Return;
                            goto cpp_cleanup;
                        }
                        Result = true;
                    }
                    if (Block->CountParams(u"Name"_wref.get()) > 0) {
                        Info->Name = EC_Str::TrimWideString(Block->GetParam(u"Name"sv));
                    }
                    if (Block->CountParams(pas::concat_wide({u"Section", Language})) > 0) {
                        Info->Section = EC_Str::TrimWideString(Block->GetParam(pas::view(pas::concat_wide({u"Section", Language}))));
                    } else if (Block->CountParams(u"Section"_wref.get()) > 0) {
                        Info->Section = EC_Str::TrimWideString(Block->GetParam(u"Section"sv));
                    }
                    if (Block->CountParams(pas::concat_wide({u"SmallDescription", Language})) > 0) {
                        Count = Block->CountParams(pas::concat_wide({u"SmallDescription", Language}));
                        Info->SmallDescription = Block->GetParamByPath(pas::concat_wide({u"SmallDescription", Language, u":0"}));
                        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count - 1); cpp_range.next(I); ) {
                            Info->SmallDescription = pas::concat_wide({Info->SmallDescription, u"\r\n", Block->GetParamByPath(pas::concat_wide({u"SmallDescription", Language, u":", EC_Str::IntToWideString(I)}))});
                        }
                    } else if (Block->CountParams(u"SmallDescription"_wref.get()) > 0) {
                        Count = Block->CountParams(u"SmallDescription"_wref.get());
                        Info->SmallDescription = Block->GetParamByPath(u"SmallDescription:0"_wref.get());
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Count - 1); cpp_range_2.next(I); ) {
                            Info->SmallDescription = pas::concat_wide({Info->SmallDescription, u"\r\n", Block->GetParamByPath(pas::concat_wide({u"SmallDescription:", EC_Str::IntToWideString(I)}))});
                        }
                    }
                    if (Block->CountParams(pas::concat_wide({u"FullDescription", Language})) > 0) {
                        Count = Block->CountParams(pas::concat_wide({u"FullDescription", Language}));
                        Info->FullDescription = Block->GetParamByPath(pas::concat_wide({u"FullDescription", Language, u":0"}));
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, Count - 1); cpp_range_3.next(I); ) {
                            Info->FullDescription = pas::concat_wide({Info->FullDescription, u"\r\n", Block->GetParamByPath(pas::concat_wide({u"FullDescription", Language, u":", EC_Str::IntToWideString(I)}))});
                        }
                    } else if (Block->CountParams(u"FullDescription"_wref.get()) > 0) {
                        Count = Block->CountParams(u"FullDescription"_wref.get());
                        Info->FullDescription = Block->GetParamByPath(u"FullDescription:0"_wref.get());
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, Count - 1); cpp_range_4.next(I); ) {
                            Info->FullDescription = pas::concat_wide({Info->FullDescription, u"\r\n", Block->GetParamByPath(pas::concat_wide({u"FullDescription:", EC_Str::IntToWideString(I)}))});
                        }
                    }
                    if (Block->CountParams(pas::concat_wide({u"Author", Language})) > 0) {
                        Info->Author = EC_Str::TrimWideString(Block->GetParam(pas::view(pas::concat_wide({u"Author", Language}))));
                    } else if (Block->CountParams(u"Author"_wref.get()) > 0) {
                        Info->Author = EC_Str::TrimWideString(Block->GetParam(u"Author"sv));
                    }
                    if (Block->CountParams(u"Languages"_wref.get()) > 0) {
                        Info->UnsupportedLanguage = true;
                        Language = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Language)));
                        Languages = EC_Str::TrimWideString(Block->GetParam(u"Languages"sv));
                        Count = EC_Str::CountDelimitedPartsW(pas::view(Languages), u","sv);
                        if (Languages != u"") {
                            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
                                if (([&] {
                                    pas::WideString cpp_string = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Languages), I, u","sv)))));
                                    return cpp_string == Language;
                                }())) {
                                    Info->UnsupportedLanguage = false;
                                    break;
                                }
                            }
                        }
                    } else if (!HasLanguageResources) {
                        if (!HasForeignResources) {
                            HasForeignResources = HasOtherLanguageResources();
                        }
                        Info->UnsupportedLanguage = HasForeignResources;
                    }
                    if (Block->CountParams(u"Dependence"_wref.get()) > 0) {
                        Info->DependencyNames = EC_Str::TrimWideString(Block->GetParam(u"Dependence"sv));
                    }
                    if (Block->CountParams(u"Conflict"_wref.get()) > 0) {
                        Info->ConflictNames = EC_Str::TrimWideString(Block->GetParam(u"Conflict"sv));
                    }
                    if (Block->CountParams(u"Priority"_wref.get()) > 0) {
                        Info->Priority = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"Priority"sv)));
                    }
                } catch (...) {
                    Info->Name = pas::WideString();
                    Info->Section = pas::WideString();
                    Info->SmallDescription = pas::WideString();
                    Info->FullDescription = pas::WideString();
                    Info->Author = pas::WideString();
                    if (!HasLanguageResources) {
                        if (!HasForeignResources) {
                            HasForeignResources = HasOtherLanguageResources();
                        }
                        Info->UnsupportedLanguage = HasForeignResources;
                    } else {
                        Info->UnsupportedLanguage = true;
                    }
                    Info->DependencyNames = pas::WideString();
                    Info->ConflictNames = pas::WideString();
                    Info->Priority = 0u;
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            SysUtilsImports::SetCurrentDir(SavedDir);
            if (Block != nullptr) {
                pas::free(Block);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        return Result;
    }

    void ScanModFolders(pas::WideString Folder, pas::WideString Prefix) {
        pas::WideString FileName{};
        pas::WideString Path{};
        pas::WideString ChildPrefix{};
        TModInfo* Info{};
        std::int32_t Index{};
        Windows::TWin32FindDataA FindData{};
        Info = pas::construct_call<TModInfo>(TModInfo_Create);
        SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(Folder));
        FindData.dwFileAttributes = WindowsImports::FILE_ATTRIBUTE_NORMAL;
        WindowsImports::THandle Handle = WindowsSdk::FindFirstFile(pas::literal_pointer("*.*"), FindData);
        if (Handle != WindowsImports::INVALID_HANDLE_VALUE) {
            do {
                if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) != 0) {
                    FileName = pas::array_text<pas::WideString>(FindData.cFileName.elements, 260);
                    if (FileName != u"." && FileName != u"..") {
                        Path = pas::concat_wide({Folder, u"\\", FileName});
                        if (aModsInfo::LoadModInfo(Path, Info)) {
                            Info->Folder = pas::concat_wide({Prefix, FileName});
                            Index = aModsInfo::FindOrInsertModFolder(Info->Folder);
                            pas::list_put(ModInfos, Index, reinterpret_cast<void*>(Info));
                            if (Info->Name != u"") {
                                if (ModIdCounts->CountParams(Info->Name) <= 0) {
                                    ModIdCounts->AddParam(Info->Name, u"1"_wref.get());
                                } else {
                                    ModIdCounts->SetOrAddParam(Info->Name, u"0"_wref.get());
                                }
                            }
                            Info = pas::construct_call<TModInfo>(TModInfo_Create);
                        } else {
                            ChildPrefix = pas::concat_wide({Prefix, FileName, u"\\"});
                            aModsInfo::ScanModFolders(Path, ChildPrefix);
                        }
                    }
                }
            } while (WindowsSdk::FindNextFile(Handle, FindData));
            WindowsImports::FindClose(Handle);
        }
        pas::free(Info);
    }

    void InitializeModInfos() {
        pas::AnsiString SavedDir{};
        pas::WideString Folder{};
        pas::WideString Name{};
        pas::WideString Names{};
        pas::WideString Indices{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Index{};
        std::int32_t Count{};
        TModInfo* Info{};
        if (ModInfosInitialized) {
            return;
        }
        if (ModInfos == nullptr) {
            ModInfos = pas::make_object<pas::List>();
        }
        if (SelectedModInfos == nullptr) {
            SelectedModInfos = pas::make_object<pas::List>();
        }
        if (ModIdCounts == nullptr) {
            ModIdCounts = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        }
        if (ModConflictIndex == nullptr) {
            ModConflictIndex = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        }
        if (ModDependencyIndex == nullptr) {
            ModDependencyIndex = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        }
        SavedDir = SysUtilsImports::GetCurrentDir();
        Folder = static_cast<pas::WideString>(SavedDir);
        Folder = pas::concat_wide({Folder, u"\\Mods"});
        aModsInfo::ScanModFolders(Folder, pas::WideString());
        SysUtilsImports::SetCurrentDir(SavedDir);
        if (GR_Main::SelectedMods != u"") {
            Names = GR_Main::SelectedMods;
            Names = EC_Str::ReplaceAllWideString(Names, u"/"_wref.get(), u"\\"sv);
            Count = EC_Str::CountDelimitedPartsW(pas::view(Names), u","sv);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Names), I, u","sv));
                if (Name != u"") {
                    Index = aModsInfo::FindOrInsertModFolder(Name);
                    if (pas::list_get(ModInfos, Index) != nullptr) {
                        Info = pas::list_at<TModInfo>(ModInfos, Index);
                        pas::list_add(SelectedModInfos, reinterpret_cast<void*>(Info));
                        Info->Selected = true;
                    } else {
                        Info = pas::construct_call<TModInfo>(TModInfo_Create);
                        pas::list_put(ModInfos, Index, reinterpret_cast<void*>(Info));
                        pas::list_add(SelectedModInfos, reinterpret_cast<void*>(Info));
                        if (SysUtilsImports::DirectoryExists(static_cast<pas::AnsiString>(pas::concat_wide({Folder, u"\\", Name}))) && aModsInfo::LoadModInfo(pas::concat_wide({Folder, u"\\", Name}), Info)) {
                            Info->Folder = Name;
                            if (Info->Name != u"") {
                                if (ModIdCounts->CountParams(Info->Name) <= 0) {
                                    ModIdCounts->AddParam(Info->Name, u"1"_wref.get());
                                } else {
                                    ModIdCounts->SetOrAddParam(Info->Name, u"0"_wref.get());
                                }
                            }
                        } else {
                            Info->Folder = Name;
                            Info->MissingFolder = true;
                            Info->Selected = true;
                        }
                    }
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ModInfos) - 1); cpp_range_2.next(I); ) {
            Info = pas::list_at<TModInfo>(ModInfos, I);
            Info->IndexText = EC_Str::IntToWideString(I);
            if (Info->Name != u"") {
                Info->DuplicateName = ModIdCounts->GetParam(pas::view(Info->Name)) != u"1";
            }
            if (Info->ConflictNames != u"") {
                Count = EC_Str::CountDelimitedPartsW(pas::view(Info->ConflictNames), u","sv);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(J); ) {
                    Indices = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Info->ConflictNames), J, u","sv));
                    if (ModConflictIndex->CountParams(Indices) <= 0) {
                        ModConflictIndex->AddParam(Indices, u""_wref.get());
                    }
                }
            }
            if (Info->DependencyNames != u"") {
                Count = EC_Str::CountDelimitedPartsW(pas::view(Info->DependencyNames), u","sv);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(J); ) {
                    Indices = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Info->DependencyNames), J, u","sv));
                    if (ModDependencyIndex->CountParams(Indices) <= 0) {
                        ModDependencyIndex->AddParam(Indices, u""_wref.get());
                    }
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(ModInfos) - 1); cpp_range_5.next(I); ) {
            Info = pas::list_at<TModInfo>(ModInfos, I);
            if (Info->Name == u"") {
                continue;
            }
            if (ModConflictIndex->CountParams(Info->Name) > 0) {
                Indices = ModConflictIndex->GetParam(pas::view(Info->Name));
                if (Indices == u"") {
                    ModConflictIndex->SetParam(pas::view(Info->Name), EC_Str::IntToWideString(I));
                } else {
                    ModConflictIndex->SetParam(pas::view(Info->Name), pas::concat_wide({Indices, u",", EC_Str::IntToWideString(I)}));
                }
            }
            if (ModDependencyIndex->CountParams(Info->Name) > 0) {
                Indices = ModDependencyIndex->GetParam(pas::view(Info->Name));
                if (Indices == u"") {
                    ModDependencyIndex->SetParam(pas::view(Info->Name), EC_Str::IntToWideString(I));
                } else {
                    ModDependencyIndex->SetParam(pas::view(Info->Name), pas::concat_wide({Indices, u",", EC_Str::IntToWideString(I)}));
                }
            }
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(ModInfos) - 1); cpp_range_6.next(I); ) {
            Info = pas::list_at<TModInfo>(ModInfos, I);
            if (Info->DependencyNames == u"") {
                continue;
            }
            Info->DependencyCount = EC_Str::CountDelimitedPartsW(pas::view(Info->DependencyNames), u","sv);
            Info->Dependencies.set_length(Info->DependencyCount);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Info->DependencyCount - 1); cpp_range_7.next(J); ) {
                Indices = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Info->DependencyNames), J, u","sv));
                Indices = ModDependencyIndex->GetParam(pas::view(Indices));
                if (Indices == u"") {
                    Info->MissingDependency = true;
                    Info->Dependencies[J] = nullptr;
                } else {
                    Info->Dependencies[J] = pas::list_at<TModInfo>(ModInfos, EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Indices), 0, u","sv))));
                }
            }
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(ModInfos) - 1); cpp_range_8.next(I); ) {
            Info = pas::list_at<TModInfo>(ModInfos, I);
            if (Info->ConflictNames == u"") {
                continue;
            }
            Info->ConflictCount = EC_Str::CountDelimitedPartsW(pas::view(Info->ConflictNames), u","sv);
            Info->Conflicts.set_length(Info->ConflictCount);
            for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Info->ConflictCount - 1); cpp_range_9.next(J); ) {
                Indices = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Info->ConflictNames), J, u","sv));
                Indices = ModConflictIndex->GetParam(pas::view(Indices));
                if (Indices == u"") {
                    Info->Conflicts[J] = nullptr;
                } else {
                    Info->Conflicts[J] = pas::list_at<TModInfo>(ModInfos, EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Indices), 0, u","sv))));
                }
            }
        }
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, ModConflictIndex->GetParamCount() - 1); cpp_range_10.next(I); ) {
            Indices = ModConflictIndex->GetParamValue(I);
            Count = EC_Str::CountDelimitedPartsW(pas::view(Indices), u","sv);
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(J); ) {
                Index = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Indices), J, u","sv)));
                pas::list_at<TModInfo>(ModInfos, Index)->ReferencedAsConflict = true;
            }
        }
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, ModDependencyIndex->GetParamCount() - 1); cpp_range_12.next(I); ) {
            Indices = ModDependencyIndex->GetParamValue(I);
            Count = EC_Str::CountDelimitedPartsW(pas::view(Indices), u","sv);
            for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_13.next(J); ) {
                Index = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Indices), J, u","sv)));
                pas::list_at<TModInfo>(ModInfos, Index)->ReferencedAsDependency = true;
            }
        }
        ModInfosInitialized = true;
    }

    // Frees mod objects and clears the existing containers, retaining their allocation for reload.
    void ClearModInfoState() {
        std::int32_t Index{};
        ModInfosInitialized = false;
        if (ModInfos != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ModInfos) - 1); cpp_range.next(Index); ) {
                pas::free(pas::list_at<pas::Object>(ModInfos, Index));
            }
            pas::list_clear(ModInfos);
        }
        if (SelectedModInfos != nullptr) {
            pas::list_clear(SelectedModInfos);
        }
        if (ModIdCounts != nullptr) {
            ModIdCounts->Clear();
        }
        if (ModConflictIndex != nullptr) {
            ModConflictIndex->Clear();
        }
        if (ModDependencyIndex != nullptr) {
            ModDependencyIndex->Clear();
        }
    }

    void TModInfo_Create(TModInfo* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Folder = pas::WideString();
        Self->SwitchImage = nullptr;
        Self->Name = pas::WideString();
        Self->Section = pas::WideString();
        Self->SmallDescription = pas::WideString();
        Self->FullDescription = pas::WideString();
        Self->Author = pas::WideString();
        Self->DependencyNames = pas::WideString();
        Self->DependencyCount = 0;
        Self->Dependencies.set_length(0);
        Self->ConflictNames = pas::WideString();
        Self->ConflictCount = 0;
        Self->Conflicts.set_length(0);
        Self->Priority = 0u;
        Self->UnsupportedLanguage = false;
        Self->MissingFolder = false;
        Self->MissingDependency = false;
        Self->Misplaced = false;
        Self->DuplicateName = false;
        Self->ReferencedAsConflict = false;
        Self->ReferencedAsDependency = false;
        Self->Selected = false;
    }

    void TModInfo_Destroy(TModInfo* Self) {
        Self->Dependencies.set_length(0);
        Self->Conflicts.set_length(0);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    pas::WideString TModInfo::GetDisplayName() {
        if (Name != u"") {
            return Name;
        }
        return pas::concat_wide({u"[", Folder, u"]"});
    }

    TModInfo* TModInfo::GetConflict(std::int32_t Index, std::int32_t VariantIndex) {
        TModInfo* Info{};
        pas::WideString Indices{};
        TModInfo* Result = nullptr;
        if (Index < 0 || Index >= ConflictCount) {
            return Result;
        }
        if (VariantIndex == 0) {
            return Conflicts[Index];
        }
        Info = Conflicts[Index];
        if (!Info->DuplicateName) {
            return Result;
        }
        Indices = ModConflictIndex->GetParam(pas::view(Info->Name));
        std::int32_t Count = EC_Str::CountDelimitedPartsW(pas::view(Indices), u","sv);
        if (VariantIndex < 0 || VariantIndex >= Count) {
            return Result;
        }
        return pas::list_at<TModInfo>(ModInfos, EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Indices), VariantIndex, u","sv))));
    }

    TModInfo* TModInfo::GetDependency(std::int32_t Index, std::int32_t VariantIndex) {
        TModInfo* Info{};
        pas::WideString Indices{};
        TModInfo* Result = nullptr;
        if (Index < 0 || Index >= DependencyCount) {
            return Result;
        }
        if (VariantIndex == 0) {
            return Dependencies[Index];
        }
        Info = Dependencies[Index];
        if (!Info->DuplicateName) {
            return Result;
        }
        Indices = ModDependencyIndex->GetParam(pas::view(Info->Name));
        std::int32_t Count = EC_Str::CountDelimitedPartsW(pas::view(Indices), u","sv);
        if (VariantIndex < 0 || VariantIndex >= Count) {
            return Result;
        }
        return pas::list_at<TModInfo>(ModInfos, EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Indices), VariantIndex, u","sv))));
    }

    void TModInfo::p_destroy() {
        aModsInfo::TModInfo_Destroy(this);
    }

} // namespace aModsInfo
