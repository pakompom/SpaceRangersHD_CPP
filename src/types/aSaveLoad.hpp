#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aSaveLoad {
    struct TSaver;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSaver : EC_Thread::TThreadEC {
        PAS_CLASS_META(TSaver, EC_Thread::TThreadEC, "TSaver", 68)
        void virtual_TThreadEC_Execute() override;
        void QueueSave(pas::WideString AFileName, EC_Buf::TBufEC* Header, EC_Buf::TBufEC* Preview, EC_Buf::TBufEC* SecondaryPreview, EC_Buf::TBufEC* GameState, EC_Buf::TBufEC* Films);
        pas::WideString FileName;
        EC_Buf::TBufEC* HeaderBuffer;
        EC_Buf::TBufEC* PreviewBuffer;
        EC_Buf::TBufEC* SecondaryPreviewBuffer;
        EC_Buf::TBufEC* GameStateBuffer;
        EC_Buf::TBufEC* FilmBuffer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aSaveLoad
