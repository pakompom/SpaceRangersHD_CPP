#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aAsteroid {
    struct TAsteroid;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAsteroid : EC_Struct::TObjectEx {
        PAS_CLASS_META(TAsteroid, EC_Struct::TObjectEx, "TAsteroid", 60)
        void p_destroy() override;
        void Init(aGalaxy::TStar* Star, const pas::WideString& GraphKey);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        void RespawnIfOutsideSystem();
        void PrepareTurnMovement(std::int32_t StartStepIndex, std::uint8_t RecordFilm);
        void AdvanceOrbitStep(std::int32_t StepIndex, std::uint8_t RecordFilm);
        void Respawn();
        void SpawnSiblingAsteroidInCurrentStar();
        void IntegrateMotion(float TimeScale);
        void WritePredictedPositions(EC_Struct::PPointF Positions, std::int32_t Count);
        pas::WideString GetDisplayName();
        pas::WideString GetInfoText();
        std::uint32_t Id;
        aGalaxy::TStar* CurrentStar;
        EC_Struct::TPointF Position;
        EC_Struct::TPointF PhysicsPosition;
        EC_Struct::TPointF Velocity;
        float Mass;
        float GravityForceFactor;
        float InverseMass;
        std::int32_t MineralCount;
        SE_Space::TObjectSE* GraphObject;
        aEFilm::TEFilmObj* FilmObject;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    inline constexpr long double AsteroidCentralMass = 2.0E+30L - 137438953472.0L;

    inline constexpr long double AsteroidWorldScale = 5.9999999999999999997E-9L;

    inline constexpr long double AsteroidInverseScaleSquared = 27777777777777777.78L;

} // namespace aAsteroid
