// PODserializer
// Kyle J Burgess

#ifndef PS_BYTES_H
#define PS_BYTES_H

#include <cstdint>
#include <cstring>

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

// Fits a byteCount into the smallest 8 * N
// returns N
uint32_t pad64(uint32_t byteCount);

template<class T>
T byteSwap(T value)
{
    T result;

    auto* src = reinterpret_cast<uint8_t*>(&value);
    auto* dst = reinterpret_cast<uint8_t*>(&result);

    for (size_t i = 0; i != sizeof(T); ++i)
    {
        constexpr size_t end = sizeof(T) - 1;
        dst[i] = src[end - i];
    }

    return result;
}

template<class T, bool swapBytes>
void copyToAligned64(uint64_t* dst, const T* src, uint32_t count)
{
    if constexpr ((!swapBytes) || (sizeof(T) == 1))
    {
        memcpy(dst, src, count * sizeof(T));
    }
    else
    {
        auto* ptr = reinterpret_cast<T*>(dst);

        for (uint32_t i = 0; i != count; ++i)
        {
            ptr[i] = byteSwap<T>(src[i]);
        }
    }
}

#endif
