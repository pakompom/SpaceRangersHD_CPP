#include "types/EC_BlockPar.hpp"
#include "units/EC_HsFile.hpp"
#include "units/GR_Main.hpp"
#include "units/aPacket.hpp"

namespace aPacket {
    // Creates the loose-file package and returns true regardless of OpenAllPackages' result.
    std::uint8_t InitializePackageCollection() {
        EC_HsFile::PackageFileLock = pas::make_critical_section<pas::CriticalSection>();
        std::uint8_t Result = true;
        EC_HsFile::PackageCollection = nullptr;
        EC_HsFile::PackageCollection = pas::construct_call<EC_HsFile::TPackCollectionEC>(EC_HsFile::TPackCollectionEC_Create);
        EC_HsFile::TPackFileEC* Pack = pas::construct_call<EC_HsFile::TPackFileEC>(EC_HsFile::TPackFileEC_Create);
        Pack->UseLooseFiles = true;
        Pack->SetPackagePath(pas::AnsiString());
        EC_HsFile::PackageCollection->AddPackToFront(Pack);
        EC_HsFile::PackageCollection->OpenAllPackages();
        return Result;
    }

    // Appends packages in language-mod, language, mod, then base order; existing entries are retained.
    std::uint8_t LoadConfiguredPackages() {
        EC_HsFile::TPackFileEC* Pack{};
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t ParamIndex{};
        std::int32_t ModIndex{};
        EC_HsFile::PackageCollection->CloseAllPackages();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(GR_Main::ModLanguageInstallConfigs) - 1); cpp_range.next(ModIndex); ) {
            Block = pas::list_at<EC_BlockPar::TBlockParEC>(GR_Main::ModLanguageInstallConfigs, ModIndex);
            Block = Block->GetBlock(u"Packages"sv);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Block->GetParamCount() - 1); cpp_range_2.next(ParamIndex); ) {
                Pack = pas::construct_call<EC_HsFile::TPackFileEC>(EC_HsFile::TPackFileEC_Create);
                Pack->SetPackagePath(static_cast<pas::AnsiString>(Block->GetParamValue(ParamIndex)));
                EC_HsFile::PackageCollection->AddPackToBack(Pack);
            }
        }
        Block = GR_Main::LanguageInstallConfig->GetBlock(u"Packages"sv);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Block->GetParamCount() - 1); cpp_range_3.next(ParamIndex); ) {
            Pack = pas::construct_call<EC_HsFile::TPackFileEC>(EC_HsFile::TPackFileEC_Create);
            Pack->SetPackagePath(static_cast<pas::AnsiString>(Block->GetParamValue(ParamIndex)));
            EC_HsFile::PackageCollection->AddPackToBack(Pack);
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(GR_Main::ModInstallConfigs) - 1); cpp_range_4.next(ModIndex); ) {
            Block = pas::list_at<EC_BlockPar::TBlockParEC>(GR_Main::ModInstallConfigs, ModIndex);
            Block = Block->GetBlock(u"Packages"sv);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Block->GetParamCount() - 1); cpp_range_5.next(ParamIndex); ) {
                Pack = pas::construct_call<EC_HsFile::TPackFileEC>(EC_HsFile::TPackFileEC_Create);
                Pack->SetPackagePath(static_cast<pas::AnsiString>(Block->GetParamValue(ParamIndex)));
                EC_HsFile::PackageCollection->AddPackToBack(Pack);
            }
        }
        Block = GR_Main::InstallConfig->GetBlock(u"Packages"sv);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Block->GetParamCount() - 1); cpp_range_6.next(ParamIndex); ) {
            Pack = pas::construct_call<EC_HsFile::TPackFileEC>(EC_HsFile::TPackFileEC_Create);
            Pack->SetPackagePath(static_cast<pas::AnsiString>(Block->GetParamValue(ParamIndex)));
            EC_HsFile::PackageCollection->AddPackToBack(Pack);
        }
        return EC_HsFile::PackageCollection->OpenAllPackages();
    }

    // Requires an initialized package collection.
    void FinalizePackageCollection() {
        EC_HsFile::PackageCollection->CloseAllPackages();
        EC_HsFile::PackageCollection->Clear(true);
        pas::free(EC_HsFile::PackageCollection);
        EC_HsFile::PackageCollection = nullptr;
        if (EC_HsFile::PackageFileLock != nullptr) {
            pas::free(EC_HsFile::PackageFileLock);
            EC_HsFile::PackageFileLock = nullptr;
        }
    }

} // namespace aPacket
