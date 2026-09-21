#include "types/EC_BlockPar.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_PSWeapon14Vertix.hpp"
#include "units/GR_Main.hpp"

// Native animation palette resources; this effect uses the shared GAI control.
namespace GI_PSWeapon14Vertix {
    pas::DynArray<GI_PSWeapon14Vertix::TGAISet> Weapon14AnimationPaths{};

    void LoadWeapon14AnimationPaths() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.13.Palettes"_wref.get());
        std::int32_t BlockCount = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, BlockCount - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index))) + 1);
        }
        Weapon14AnimationPaths.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                if (PaletteBlock->CountParams(u"GAI"_wref.get()) > 0) {
                    Weapon14AnimationPaths[Index][0] = PaletteBlock->GetParam(u"GAI"sv);
                }
            }
        }
    }

} // namespace GI_PSWeapon14Vertix
