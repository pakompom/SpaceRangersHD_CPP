#pragma once
#include "runtime_support.hpp"

namespace GlobalsV {
    struct TSpaceImageTemplate;

    struct TStarFieldImageTemplate;

    enum TGameScreenId : std::uint8_t {
        screenNone = 0,
        screenMainMenu = 1,
        screenIntroduction = 3,
        screenHangar = 4,
        screenPlanet = 5,
        screenPlanetNO = 6,
        screenPlanetQuest = 7,
        screenEquipmentShop = 8,
        screenShip = 10,
        screenTalk = 11,
        screenScanner = 12,
        screenGovernment = 15,
        screenStarMap = 16,
        screenFilm = 17,
        screenGalaxy = 18,
        screenJump = 19,
        screenRuinsTalk = 20,
        screenArcadeBattle = 21,
        screenLoad = 22,
        screenSaveManager = 23,
        screenGameLoad = 24,
        screenGameMenu = 25,
        screenSettings = 26,
        screenGameEnd = 27,
        screenInfo = 28,
        screenRewards = 30,
        screenAbout = 31,
        screenScores = 32,
        screenNewGame = 33,
        screenGoodsShop = 34,
        screenRating = 35,
        screenSelectFace = 36,
        screenJournal = 37,
        screenLoadRobot = 38,
        screenLoadQuest = 39,
        screenLoadArcade = 40,
        screenAchievements = 41,
    };

    // Keep the registry independent of UI/cache implementation units. Consumers
    // cast these object references to the concrete cache and message-loop types.
    #pragma pack(push, 1)
    struct TSpaceImageTemplate {
        // SpaceImg parameter name; used for weighted selection.
        std::int32_t Kind;
        std::int32_t Weight;
        // Owned, released by UI shutdown.
        pas::Object* CacheControl;
        // Borrowed during rendering.
        pas::Object* CachedData;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TStarFieldImageTemplate {
        // Zero-initialized; use not yet established.
        std::int32_t Reserved;
        // StarFieldImg parameter name.
        std::int32_t Weight;
        // Owned, released by UI shutdown.
        pas::Object* CacheControl;
        // Borrowed during rendering.
        pas::Object* CachedData;
    };
    #pragma pack(pop)

    using TGameScreenTable = pas::Array<pas::Object*, 0, 41>;

    enum TGalaxyMapFontChoice : std::uint32_t {
        gmfRanger = 1,
        gmfMini = 2,
        gmfSmall = 3,
        gmfSmallBold = 4,
        gmfNormal = 5,
        gmfNormalBold = 6,
    };

} // namespace GlobalsV
