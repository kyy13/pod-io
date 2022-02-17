// PODserializer
// Kyle J Burgess

#include "Bytes.h"

uint32_t pad64(uint32_t byteCount)
{
    uint32_t rem = byteCount & 7;
    byteCount >>= 3;
    if (rem != 0) ++byteCount;
    return byteCount;
}
