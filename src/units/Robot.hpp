#pragma once
#include "types/Robot.hpp"
#include "types/Types.hpp"

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace Robot {
    extern Robot::PRobotInterfacePrefix RobotInterface;

    extern Robot::TRobotCallbacks RobotCallbacks;

    extern pas::Array<std::int32_t, 0, 5> RobotBattleStatistics;

    extern pas::DynArray<std::int32_t> SupportedMultiSamples;

    extern Robot::TRobotDisplaySettingsPrefix RobotSettings;

    extern std::uint8_t RobotSound;

    extern std::uint8_t RobotMusic;

    extern std::uint8_t RobotVSync;

    extern std::int32_t RobotFSAASamples;

    extern std::int32_t RobotAnisotropy;

    extern std::int32_t RobotMaxDistance;

    extern std::int32_t SupportedMultiSampleCount;

    extern std::uint32_t MaximumAnisotropy;

    extern std::uint32_t RobotModule;

    std::int32_t GetRobotMultiSampleIndex();

    void InitializeRobotRuntime();

    void FinalizeRobotRuntime();

    std::int32_t FRun(const pas::WideString& MapName, const pas::WideString& StartText, const pas::WideString& WinText, const pas::WideString& LossText, const pas::WideString& TerronName);

    void PAS_STDCALL RobotPlaySound(char16_t* Name);

    GR_Sound::TSoundBufferControl* PAS_STDCALL RobotCreateSound(char16_t* Name, std::int32_t Group, std::int32_t Looping);

    void PAS_STDCALL RobotFreeSound(GR_Sound::TSoundBufferControl* Sound);

    void PAS_STDCALL RobotStartSound(GR_Sound::TSoundBufferControl* Sound);

    std::int32_t PAS_STDCALL RobotIsSoundPlaying(GR_Sound::TSoundBufferControl* Sound);

    void PAS_STDCALL RobotSetSoundVolume(GR_Sound::TSoundBufferControl* Sound, float Value);

    void PAS_STDCALL RobotSetSoundPan(GR_Sound::TSoundBufferControl* Sound, float Value);

    float PAS_STDCALL RobotGetSoundVolume(GR_Sound::TSoundBufferControl* Sound);

    float PAS_STDCALL RobotGetSoundPan(GR_Sound::TSoundBufferControl* Sound);

    void PAS_STDCALL RobotRenderText(char16_t* Text, char16_t* FontName, std::uint32_t Color, std::int32_t Width, std::int32_t Height, std::int32_t AlignX, std::int32_t AlignY, std::int32_t Wrap, std::int32_t OffsetX, std::int32_t OffsetY, Types::PRect Clip, PRobotTextImage Image);

    void PAS_STDCALL RobotFreeText(PRobotTextImage Image);

    void PAS_STDCALL RobotSetProgress(float Fraction);

    float PAS_STDCALL RobotGetMusicVolume();

    void PAS_STDCALL RobotSetMusicVolume(float Value);

    void PAS_STDCALL RobotPlayMusic();

    void PAS_STDCALL RobotReleaseTextures();

} // namespace Robot
