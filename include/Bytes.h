// PODserializer
// Kyle J Burgess

#ifndef PS_BYTES_H
#define PS_BYTES_H

#include <cstdint>

inline bool isBigEndian()
{
    union
    {
        uint32_t i;
        uint8_t c[4];
    };

    i = 0x01020304u;

    return c[0] == 1u;
}

// Fits a byteCount into the smallest 64 * N
// returns N
uint32_t pad64(uint32_t byteCount)
{
    uint32_t rem = byteCount & 0x0000003F;
    byteCount >>= 6;
    if (rem != 0) ++byteCount;
    return byteCount;
}

template<class T>
T byteSwap(T value)
{
    T result;

    auto* src = reinterpret_cast<uint8_t*>(&value);
    auto* dst = reinterpret_cast<uint8_t*(&result);

    for (size_t i = 0; i != sizeof(T); ++i)
    {
        constexpr size_t end = sizeof(T) - 1;
        dst[i] = src[end - i];
    }

    return result;
}

#endif
