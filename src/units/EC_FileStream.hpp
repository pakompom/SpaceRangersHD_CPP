#pragma once
#include "types/EC_FileStream.hpp"

namespace EC_FileStream {
    void TFileStreamEC_Create(TFileStreamEC* Self, std::int32_t BufferBytes, const pas::WideString& FileName);

    void TFileStreamEC_Destroy(TFileStreamEC* Self);

    void TFileStreamEC_Execute(TFileStreamEC* Self);

} // namespace EC_FileStream
