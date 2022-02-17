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

template<class T>
T byteSwap(T value)
{
    T result;

    uint8_t* src = reinterpret_cast<uint8_t*>(&value);
    uint8_t* dst = reinterpret_cast<uint8_t*(&result);

    for (size_t i = 0; i != sizeof(T); ++i)
    {
        constexpr size_t end = sizeof(T) - 1;
        dst[i] = src[end - i];
    }

    return result;
}

#endif
